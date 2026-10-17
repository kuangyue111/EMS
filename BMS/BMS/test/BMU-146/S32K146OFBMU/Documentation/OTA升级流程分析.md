# OTA (CAN总线远程升级) 完整流程分析

## 一、整体架构

本系统实现了一套完整的 **IAP (In-Application Programming)** 方案，基于 CAN 总线实现固件远程升级。代码位于 `Sources/Addition/LoadApp/LoadApp.c` 和 `LoadApp.h`，支持**下载 / 备份 / 安装 / 回滚**的完整生命周期管理。

### 核心文件

| 文件 | 作用 |
|------|------|
| `Sources/Addition/LoadApp/LoadApp.h` | 宏定义、数据结构、接口声明 |
| `Sources/Addition/LoadApp/LoadApp.c` | 全部升级逻辑实现 (2555行) |
| `Sources/BSPSys/BSPUser/BSPUserAPI.c` | 启动时调用 `BootLoaderInit()` |
| `Sources/BSPSys/BSPUser/BSPUserHook.c` | CAN中断回调 → `CanRcvLoadMsgHandleAPI()` |
| `Sources/OSUser/OSUserHOOK.c` | 周期调度 `LoadAppFlashErasHandleAPI()` / `CanSendRemLoadMsgAPI()` |

---

## 二、Flash 内存布局

### 2.1 MCU 内部 Flash (S32K146, 1MB)

```
0x000000 ────────────────────────────
  Bootloader 区 (4KB)                0x0000 ~ 0x0FFF
  ├─ 中断向量表
  ├─ Boot 启动代码
  └─ Flash 配置区
────────────────────────────────────
0x001000 ────────────────────────────  ← APP_ENRTY_POINT_FLASH_ADDR
  Application 区 (~792KB)           0x1000 ~ 0xC5000
  ├─ 当前运行的业务代码
  └─ APP_PRO_BLOCK1_FLASH_ADDR
────────────────────────────────────
0x0F7000 ────────────────────────────  ← BOOT_PARA_BLOCK4_FLASH_ADDR
  Boot 参数存储区 (4KB 扇区)         0xF7000 ~ 0xF7FFF
  ├─ 跳转标志 (模式/状态)
  ├─ 设备地址
  ├─ 运行标志/尝试标志
  ├─ 备份有效标志/下载有效标志
  └─ 硬件配置参数 (CAN/RS485波特率等)
```

### 2.2 外部 SPI Flash (SST26VF064B, 8MB)

```
0x000000 ────────────────────────────
  Zone 1: 备份镜像区 A (1MB)
  0x000000 ~ 0x0FFFFF
────────────────────────────────────
0x100000 ────────────────────────────  ← APP_PRO_EXTDOWN_FLASH_ADDR
  Zone 2: OTA下载缓冲区 (1MB)        0x100000 ~ 0x1FFFFF
  ├─ 远程固件先下载到这里
  └─ 每扇区 4096 字节 (APP_PRO_DOWN_FLASH_SEC_LEN)
────────────────────────────────────
0x200000+
  Zone 3~8: 故障记录 / 运行数据 / 波形 / DOD统计
```

---

## 三、关键宏定义

| 宏定义 | 值 | 说明 |
|--------|-----|------|
| `BOOT_ENRTY_POINT_FLASH_ADDR` | `0x0` | Bootloader 入口地址 |
| `APP_ENRTY_POINT_FLASH_ADDR` | `0x001000` | APP 入口地址 (4KB处) |
| `APP_PRO_BLOCK1_FLASH_ADDR` | `0x001000` | APP 程序起始地址 |
| `APP_PRO_BLOCK1_FIN_ADDR` | `0x0C5000` | APP 程序结束地址 |
| `APP_PRO_EXTDOWN_FLASH_ADDR` | `0x100000` | 远程下载外部Flash起始地址 |
| `APP_PRO_DOWN_FLASH_SEC_LEN` | `4096` | 下载扇区大小 (4KB) |
| `BOOT_PARA_BLOCK4_FLASH_ADDR` | `0x0F7000` | Boot参数存储区地址 |
| `LOAD_APP_RUN_FLAG` | `0x5A5A` | APP运行标志 |
| `LOAD_APP_TRY_FLAG` | `0x5A5A` | APP尝试运行标志 |
| `LOAD_APP_PARA_FLAG` | `0x5A5A` | APP参数有效标志 |
| `LOAD_CAN_NUM` | `3` | 使用的CAN通道数 |

---

## 四、五种工作模式

```c
typedef enum {
    eLoMod_NorApp = 0,    // 正常应用模式 (APP 运行)
    eLoMod_RemTra,        // 远程传输模式 (APP 中接收固件)
    eLoMod_NotApp,        // 应用缺失模式 (BOOT 中)
    eLoMod_LocBoot,       // 本地 Boot 模式 (BOOT 中)
    eLoMod_RemBoot,       // 远程 Boot 模式 (BOOT 中)
    eLoMod_Fin,
} e_LoadWorkMode;
```

| 模式 | 值 | 代码位置 | 说明 |
|------|-----|----------|------|
| `NorApp` | 0 | APP | 正常运行，仅响应设备查询和握手命令 |
| `RemTra` | 1 | APP | 接收固件数据，写入外部Flash |
| `NotApp` | 2 | BOOT | Flash中无有效APP，停留在Bootloader |
| `LocBoot` | 3 | BOOT | 本地操作模式（备份/回滚/安装） |
| `RemBoot` | 4 | BOOT | 远程升级模式 |

---

## 五、完整升级流程

### 阶段 1: 系统启动 — `BootLoaderInit()`

**调用链**: `main()` → `BSPUserAPI.c::BootLoaderInit()`

```
BootLoaderInit()
  │
  ├─ 1. 清空下载缓冲区 ClrLoadAppBuffData()
  │
  ├─ 2. 从 EEPROM (地址 1000) 读取 APP 设备地址
  │     ├─ 有效 → 使用 APP 配置的地址
  │     └─ 无效 → 使用默认地址 0xFE
  │
  ├─ 3. 设置默认模式: eLoMod_NorApp / eLoSta_Route
  │
  ├─ 4. 从内部 Flash (0x0F7000) 读取 32 字节 Boot 参数
  │     └─ sFlashBootPara[32]
  │
  ├─ 5. 检查 Boot 参数标志 (PARA_FLAG == 0x5A5A)
  │     │
  │     ├─ [有 BOOT 参数配置]
  │     │   ├─ 设备地址更新？ → 写入APP参数 + 复位到Boot
  │     │   ├─ 硬件参数更新？ → CopyBootHWParaToApp() + 复位到Boot
  │     │   └─ [无配置更新]
  │     │       ├─ 检查中断状态: 等待安装/正在安装 → 恢复状态
  │     │       ├─ 检查中断状态: 等待备份/正在备份 → 恢复状态
  │     │       ├─ 检查跳转标志: 是否需要进入远程传输模式？
  │     │       └─ 同步设备地址、运行标志到 Boot 参数区
  │     │
  │     └─ [无 BOOT 参数] → 不做处理
```

**Boot 参数区 32 字节结构**:

| 偏移 | 字段 | 说明 |
|------|------|------|
| 0-1 | PARA_FLAG | Boot参数有效标志 (0x5A5A) |
| 2-3 | DEV_ECU | Boot配置的设备地址 |
| 4 | JUM_FLAG_MODE | 跳转目标模式 |
| 5 | JUM_FLAG_STAT | 跳转目标状态 |
| 6-7 | APP_RUN_FLAG | APP运行标志 |
| 8-9 | APP_TRY_FLAG | APP尝试运行标志 |
| 10 | APP_COPY_FLAG | APP备份有效标志 |
| 11 | APP_DOWN_FLAG | APP下载有效标志 |
| 12-13 | HW_CFGBIT | 硬件配置位 (bit0:CAN0, bit1:CAN1, ...) |
| 14-31 | CAN/RS485 波特率、地址 | 各路通信参数 |

---

### 阶段 2: CAN 消息处理 — `CanRcvLoadMsgHandleAPI()`

**触发**: CAN 接收中断回调 `BSPUserHook.c`

#### 2.1 CAN ID 编码 (32位)

```
Bit 31   Bit 30:25   Bit 24:16    Bit 15:8      Bit 7:0
方向     功能码       消息类型      目标设备类型    目标设备地址
```

- **方向** (Bit31): 0=上行, 1=下行
- **功能码** (Bit30:25): 13=请求, 14=应答, 15=修改
- **消息类型** (Bit24:16): 见下表
- **设备类型**: 0=从控, 1=高压模块, 2=主控(BMU), 3=簇控

#### 2.2 CAN 消息类型

| 消息类型 | ID | 处理函数 | 说明 |
|----------|-----|----------|------|
| `eSEARCHDEV_MSG` | 0xF0 | 直接回复 | 广播查询设备 → 回复芯片型号/版本/模式/状态 |
| `eHANDLE_MSG` | 0xF1 | → `LoadHandleCmdDeal()` | 握手命令处理 |
| `eLOADSTART_MSG` | 0xF2 | APP忽略 | 本地升级起始 (仅BOOT处理) |
| `eLOADDATA_MSG` | 0xF3 | APP忽略 | 本地升级数据 (仅BOOT处理) |
| `eLOADFIN_MSG` | 0xF4 | APP忽略 | 本地升级完成 (仅BOOT处理) |
| `eREMSTATE_MSG` | 0xFA | (发送) | 远程升级状态上报 |
| `eREMHAND_MSG` | 0xFB | → `LoadHandleCmdDeal()` | 远程握手命令 |
| `eREMDATA_MSG` | 0xFD | → `LoadRemDownMsgDeal()` | **远程固件数据帧** |
| `eREMFIN_MSG` | 0xFE | 直接处理 | 远程传输完成 + CRC校验 |

#### 2.3 CAN 路由机制 (多路CAN)

```
CAN0 (UP 上行)   ←→   PC上位机
CAN1 (DN 下行)   ←→   下级模块
CAN2 (UP 上行)   ←→   其他模块

上行端口收到的下行消息: 转发到下行端口
下行端口收到的上行消息: 广播转发, 非本设备地址也转发
```

---

### 阶段 3: 握手与模式切换 — `LoadHandleCmdDeal()`

#### 3.1 握手命令类型

| 命令 | 值 | 说明 |
|------|-----|------|
| `eLoCmd_ParaCfg` | 1 | 设备参数配置 (地址/硬件参数) |
| `eLoCmd_ProgRes` | 2 | 设备复位 → `EnterBootPro()` |
| `eLoCmd_ChanMod` | 3 | 模式切换 |
| `eLoCmd_ProRdWr` | 4 | 安装/备份/回滚操作 |
| `eLoCmd_LoaCtrl` | 5 | 升级控制 (本地模式) |
| `eLoCmd_IntRemTra` | 10 | **进入远程传输模式** |
| `eLoCmd_IntRemLoa` | 11 | 进入远程Boot模式 |
| `eLoCmd_ExtRemLoa` | 12 | 退出远程模式 |
| `eLoCmd_ReqInst` | 13 | 请求安装 (BOOT本地模式) |
| `eLoCmd_EndInst` | 14 | 终止安装 |

#### 3.2 进入远程传输模式 (核心路径)

```
PC 下发 eLoCmd_IntRemTra (或 eLoCmd_LoaCtrl + eCtrlCmd_ReqLoad)
  │
  ├─ ChangeWorkModeHook(eCtrlCmd_RemTra)
  │   └─ 当前为 APP → 直接切换: gLoadWorkModel = eLoMod_RemTra
  │                    gLoadWorkState = eLoSta_HandWait
  │
  ├─ 验证设备识别码 (LOAD_DEV_ID or 0xFFFF广播)
  │   │
  │   ├─ [匹配成功]
  │   │   ├─ gLoadWorkState = eLoSta_HandSucc (握手成功)
  │   │   ├─ gRemLoadIndex = 0 (帧序号清零)
  │   │   ├─ gRemLoadVers = 0 (版本号清零)
  │   │   ├─ IntoCanLoadModelHook() → 启动Flash擦除
  │   │   │   ├─ 清除之前的中断安装/备份状态
  │   │   │   ├─ gFlashEraseStep = 20 (启动擦除流程)
  │   │   │   └─ gRemLoadState = 3 (擦除中)
  │   │   └─ PC 可指定固件总字节数 gRemLoadSumByte
  │   │
  │   └─ [匹配失败]
  │       └─ gLoadWorkState = eLoSta_HandFail
```

#### 3.3 安装/备份/回滚命令

```
eLoCmd_ProRdWr (程序读写操作):
  │
  ├─ eCtrlCmd_ReqInst (请求安装)
  │   └─ 写Boot参数: MODE=RemBoot, STAT=DoInstal → EnterBootPro() 复位
  │
  ├─ eCtrlCmd_ReqCopy (请求备份)
  │   └─ 写Boot参数: MODE=LocBoot, STAT=Copying → EnterBootPro() 复位
  │
  └─ eCtrlCmd_ReqBack (请求回滚)
      └─ 写Boot参数: MODE=LocBoot, STAT=Backing → EnterBootPro() 复位

复位后 Bootloader 读取 JUM_FLAG_MODE/STAT，执行对应操作
```

---

### 阶段 4: 擦除外部 Flash — `LoadAppFlashErasHandleAPI()`

**调用周期**: 300ms (由 OSUserHOOK 调度)

```
状态机:
  gFlashEraseStep = 20  → 初始化，计算扇区数，每次10扇区
  gFlashEraseStep = 21  → 执行擦除 LoadDownFlashEraseSector(fstSec, endSec)
      │
      ├─ 擦除成功 → fstSec += 10，判断是否全部完成
      │   ├─ 完成 → gLoadWorkState = eLoSta_ErasSucc
      │   │         gFlashEraseStep = 0xFF
      │   │         gRemLoadState = 3
      │   └─ 未完 → 继续下一批
      │
      └─ 擦除失败 → gLoadWorkState = eLoSta_ErasFail
                    gFlashEraseStep = 0xFF

  gFlashEraseStep = 0xFF → 空闲，不处理
```

---

### 阶段 5: 固件数据传输 — `LoadRemDownMsgDeal()`

**触发**: 收到 `eREMDATA_MSG` 消息

#### 5.1 数据包格式

每个 CAN 帧最多携带 **6 字节有效数据**:

```
Byte 0    Byte 1       Byte 2 3 4 5 6 7
┌────────┬──────────┬─────────────────┐
│ IndexL │ Len IndexH│  Firmware Data  │
└────────┴──────────┴─────────────────┘
 帧序号   数据长度    固件二进制数据
 (12bit)  (4bit,      (最多6字节)
           1-6)
```

#### 5.2 分帧处理逻辑

```
┌─────────┬──────────────────┬─────────────────────────────────────┐
│ Frame # │ 数据内容          │ 处理                                │
├─────────┼──────────────────┼─────────────────────────────────────┤
│    1    │ 设备识别码(2B)    │ 验证识别码                           │
│         │ + 首段数据        │ → eLoSta_LoaStart                   │
│         │                   │ → 数据存入 gFlashAppData 缓冲区      │
│         │                   │ → gRemLoadState = 2 (下载中)         │
├─────────┼──────────────────┼─────────────────────────────────────┤
│    2    │ 固件版本号(4B)    │ → gRemLoadVers = 版本号              │
│         │ + 数据            │ → 数据存入缓冲区                     │
├─────────┼──────────────────┼─────────────────────────────────────┤
│    3    │ 目标Flash地址(4B) │ → 验证地址 = APP_PRO_BLOCK1_FLASH_ADDR│
│         │ + 数据            │ → 写缓冲数据到 外部Flash 0x100000    │
│         │                   │ → gLoadStartAddr = 0x101000          │
├─────────┼──────────────────┼─────────────────────────────────────┤
│  4 ~ N  │ 固件二进制数据    │ → 每 1024 字节写一次外部Flash        │
│         │ (最多6B/帧)      │ → gLoadStartAddr += 写入长度          │
│         │                   │ → 每写1KB上报一次状态                │
├─────────┼──────────────────┼─────────────────────────────────────┤
│ 最后帧  │ 剩余数据          │ → 全部接收完毕？                     │
│         │                   │   ├─ 写剩余数据到Flash               │
│         │                   │   ├─ CheckFlashAppProCode(2) CRC校验 │
│         │                   │   ├─ 校验通过: eLoSta_WaitInst       │
│         │                   │   │           CanLoaderOkHook()      │
│         │                   │   └─ 校验失败: eLoSta_WritFail       │
└─────────┴──────────────────┴─────────────────────────────────────┘
```

**关键机制**:
- 缓冲区 `gFlashAppData[1024]`: 攒满 1KB 写入一次外部Flash，非满不写
- 写入目标: 外部Flash `APP_PRO_EXTDOWN_FLASH_ADDR (0x100000)` 开始
- 地址自增: 每次写入后 `gLoadStartAddr += 写入字节数`

---

### 阶段 6: 传输完成与校验 — `eREMFIN_MSG`

```
收到 eREMFIN_MSG (传输完成消息):
  │
  ├─ 1. 仅当 gLoadWorkModel == eLoMod_RemTra 且
  │       gLoadWorkState 处于传输相关状态时才处理
  │
  ├─ 2. 写入缓冲区剩余数据 (不满1KB的部分)
  │     └─ LoadDownFlashWriteByte()
  │
  ├─ 3. CheckFlashAppProCode(2) — CRC 校验
  │     │
  │     ├─ 读取外部Flash 0x100000 处头部信息:
  │     │   ├─ [0-1]: 总段数 sumLen
  │     │   └─ [2-3]: 总CRC sumCrc
  │     │
  │     ├─ 从 0x101000+4 开始逐段读取:
  │     │   ├─ [0-1]: 段长度
  │     │   ├─ [2-3]: 段CRC
  │     │   └─ 累加到 calcCrc
  │     │
  │     └─ 比对: sumCrc == calcCrc ?
  │
  ├─ [校验通过]
  │   ├─ gRemLoadState = 0 (下载成功)
  │   ├─ gLoadWorkState = eLoSta_WaitInst (等待安装)
  │   └─ CanLoaderOkHook()
  │       ├─ 记录 DOWN_FLAG = LOAD_APP_DOWN_FLAG (0x5A)
  │       └─ 写入 Boot 参数区 (0x0F7000)
  │
  └─ [校验失败]
      ├─ gRemLoadState = 1 (下载失败)
      └─ gLoadWorkState = eLoSta_WritFail
```

---

### 阶段 7: 安装固件到内部 Flash

```
PC 下发安装命令 (eLoCmd_ReqInst / eLoCmd_ProRdWr+eCtrlCmd_ReqInst):
  │
  ├─ 写 Boot 参数区:
  │   ├─ JUM_FLAG_MODE = eLoMod_RemBoot (4)
  │   └─ JUM_FLAG_STAT = eLoSta_DoInstal (15)
  │
  ├─ EnterBootPro()
  │   ├─ 关闭 LED
  │   ├─ BSP_DISABLE_IRQ() — 关闭所有中断
  │   └─ SystemSoftwareReset() — 硬件复位
  │
  └─ [复位后]
      Bootloader 启动 → 读取 Boot 参数区
      ├─ JUM_FLAG_MODE = eLoMod_RemBoot
      ├─ JUM_FLAG_STAT = eLoSta_DoInstal
      └─ 执行安装:
          从外部Flash (0x100000) 读取固件
          → 写入内部Flash (0x001000)
          → 校验
          → 安装成功 → gLoadWorkState = eLoSta_InstSucc
          → 记录 APP_RUN_FLAG
          → 跳转 APP
```

---

### 阶段 8: 状态上报 — `CanSendRemLoadMsgAPI()`

**调用周期**: 50ms (由 OSUserHOOK 调度)
**发送条件**: 仅在 `eLoMod_RemTra` 或 `eLoMod_RemBoot` 模式下

CAN 帧格式 (8字节):
```
Byte 0-1: 当前帧序号 (gRemLoadIndex % 4096)
Byte 1:   升级状态 (高4位) | 帧序号 (低4位高位)
Byte 2:   工作模式
Byte 3:   工作状态
Byte 4-7: 固件版本号
```

| gRemLoadState | 含义 |
|:---:|------|
| 0 | 空闲 / 下载成功 |
| 1 | 下载失败 |
| 2 | 正在下载 |
| 3 | Flash 擦除中 |
| 4 | 安装失败 |
| 5 | 正在安装 |
| 6 | 安装完成 |

---

## 六、完整状态机

```
                    ┌──────────┐
                    │ 正常APP   │ (NorApp, Route)
                    │  模式 0   │
                    └────┬─────┘
                         │ 收到远程传输命令
                         ▼
                    ┌──────────┐
                    │ 等待握手  │ (RemTra, HandWait)
                    │  状态 1   │
                    └────┬─────┘
                    ┌────┴─────┐
                    ▼          ▼
              ┌──────────┐ ┌──────────┐
              │ 握手成功  │ │ 握手失败  │
              │  状态 2   │ │  状态 3   │
              └────┬─────┘ └──────────┘
                   │ 启动Flash擦除
                   ▼
              ┌──────────┐
              │ 擦除Flash │ (300ms 周期性)
              │  状态 4/5 │
              └────┬─────┘
              ┌────┴─────┐
              ▼          ▼
        ┌──────────┐ ┌──────────┐
        │ 擦除成功  │ │ 擦除失败  │
        │  状态 4   │ │  状态 5   │
        └────┬─────┘ └──────────┘
             │ 开始接收数据
             ▼
        ┌──────────┐
        │ 开始传输  │
        │  状态 6   │
        └────┬─────┘
             │ 收到第1帧数据
             ▼
        ┌──────────┐
        │ 正在传输  │ ←── 丢帧时回到此状态
        │  状态 7   │
        └────┬─────┘
        ┌────┴────┐
        ▼         ▼
   ┌─────────┐ ┌──────────┐
   │ 传输成功 │ │ 传输失败  │ (丢帧/校验错)
   │  状态 8  │ │  状态 9   │
   └────┬────┘ └──────────┘
        │ 写入Flash
        ▼
   ┌─────────┐    ┌──────────┐
   │ 写入成功 │    │ 写入失败  │
   │  状态 10 │    │  状态 11  │
   └────┬────┘    └──────────┘
        │ 等待更多数据
   ┌────┴────┐
   │ 全部写完 │
   ▼         ▼
┌─────────┐ ┌──────────┐
│ 传输成功 │ │ CRC失败   │
│ 状态 8   │ │ 状态 11   │
└────┬────┘ └──────────┘
     │ 接收完成消息
     ▼
┌──────────┐
│ 等待安装  │
│  状态 12  │
└────┬─────┘
     │ 收到安装命令
     ▼
┌──────────┐
│ 复位到    │
│ Bootloader│
└────┬─────┘
     │
     ▼
┌──────────┐    ┌──────────┐
│ 安装成功  │    │ 安装失败  │
│  状态 16  │    │  状态 17  │
└──────────┘    └──────────┘
```

---

## 七、安全保障机制

### 7.1 防变砖 (Anti-Brick)

| 机制 | 实现方式 |
|------|----------|
| **Bootloader 保护** | Bootloader 代码区 (0x0-0xFFF) 独立于 APP，永远不会被升级覆盖 |
| **CRC 强校验** | 传输完成后对整包进行分段 CRC 累加校验，不通过则**绝对不跳转 APP** |
| **看门狗** | 所有 Flash 操作间均调用 `BSPWatchDogOutput()`，防止卡死导致系统无响应 |
| **断点续升** | Boot 参数区持久化状态，复位后可恢复到中断的升级流程继续执行 |
| **双备份** | 外部 Flash 设有 A/B 两个镜像区，升级失败可回滚到备份 |

### 7.2 状态持久化

Boot 参数区 (内部 Flash 0x0F7000) 在每次关键状态变更时都会先擦除再写入，确保：
- 复位后知道当前处于哪个升级阶段
- 不会因为意外掉电而丢失升级进度
- 可恢复到"等待安装"、"正在安装"、"等待备份"等状态

### 7.3 两层 CRC 校验

```
第一层: 每段数据 CRC — 传输过程中附加在每段数据头部
第二层: 总 CRC — 所有段 CRC 的累加值，存储在固件包头部
        只有两层都通过才认为固件完整
```

### 7.4 Flash 操作冲突保护

```c
u8 LoadGetRdWrFlashStateAPI(void)
{
    // gLoadWorkState 在 HandSucc ~ WritFail 之间时，
    // 返回 TRUE 表示正在读写Flash，APP其他模块禁止操作Flash
}
```

---

## 八、核心数据结构

### 8.1 全局状态变量

```c
u16 gLoadDevAddr;         // 设备地址 (0-0xFE)
u8  gLoadWorkModel;       // 工作模式 (0-4)
u8  gLoadWorkState;       // 工作状态 (0-25)
u8  gFlashEraseStep;      // Flash擦除步骤 (0xFF=空闲, 20=初始化, 21=执行)
u8  gFlashAppData[1024];  // 1KB 下载缓冲区
u32 gLoadStartAddr;       // 当前写入的外部Flash地址
u16 gLoadSectionLen;      // 当前缓冲区中数据长度
u8  gRemLoadState;        // 远程升级状态 (0-6)
u32 gRemLoadVers;         // 远程下载固件版本号
u32 gRemLoadIndex;        // 远程下载帧序号
u32 gRemLoadSumByte;      // 远程固件总字节数
u32 gRemLoadRcvByte;      // 远程已接收字节数
```

### 8.2 Boot 参数区 (32字节, 内部Flash 0x0F7000)

```c
static u8 sFlashBootPara[32];
// [0-1]:  PARA_FLAG      — 参数有效标志
// [2-3]:  DEV_ECU        — 设备地址
// [4]:    JUM_FLAG_MODE  — 跳转目标模式
// [5]:    JUM_FLAG_STAT  — 跳转目标状态
// [6-7]:  APP_RUN_FLAG   — APP运行标志
// [8-9]:  APP_TRY_FLAG   — APP尝试标志
// [10]:   APP_COPY_FLAG  — 备份有效标志
// [11]:   APP_DOWN_FLAG  — 下载有效标志
// [12-13]: HW_CFGBIT     — 硬件配置位
// [14-31]: CAN/RS485 各路参数
```

---

## 九、周期调度

在 `Sources/OSUser/OSUserHOOK.c` 中注册的两个周期任务：

| 函数 | 周期 | 作用 |
|------|------|------|
| `LoadAppFlashErasHandleAPI()` | 300ms | 分步擦除外部Flash，避免长时间阻塞 |
| `CanSendRemLoadMsgAPI()` | 50ms | 上报远程升级状态和进度给上位机 |

---

## 十、CAN 通信端口配置

```c
// LoadApp.c
volatile const s_LoadRouteCfg sLoadCanRouteCfg[LOAD_CAN_NUM] = {
    {eCAN0, eCANROUTE_UP},    // CAN0 — 上行 (连PC)
    {eCAN1, eCANROUTE_DN},    // CAN1 — 下行 (连下级模块)
    {eCAN2, eCANROUTE_UP}     // CAN2 — 上行
};
```

消息路由策略：
- **上行口收到下行消息** → 转发到下行口
- **下行口收到上行消息** → 广播转发到上行口
- **非本设备地址消息** → 转发，不处理

---

## 十一、关键函数调用关系

```
系统启动:
  main()
    └─ BSPUserAPI.c → BootLoaderInit()
         ├─ ClrLoadAppBuffData()
         ├─ BSPINTEEPReadByte()         // 读EEPROM设备地址
         ├─ BSPINTFLASHReadByte()       // 读Boot参数区
         ├─ CopyBootHWParaToApp()       // [条件] 迁移硬件参数
         └─ EnterBootPro()              // [条件] 软复位

CAN 中断:
  BSPUserHook.c
    └─ CanRcvLoadMsgHandleAPI(msg, canNum)
         ├─ [eSEARCHDEV]  → BuildLoadAREMsgID() + BSPCANSendMsg()
         ├─ [eHANDLE/eREMHAND] → LoadHandleCmdDeal()
         │    ├─ ChangeWorkModeHook()     // 模式切换
         │    ├─ IntoCanLoadModelHook()   // 进入升级模式
         │    ├─ CopyBootHWParaToApp()   // 硬件参数配置
         │    ├─ CanLoaderOkHook()        // 记录完成状态
         │    ├─ EnterBootPro()           // 跳转Boot
         │    └─ CfgBootHWParaHandle()   // [条件] 硬件参数配置
         ├─ [eREMDATA] → LoadRemDownMsgDeal()
         │    ├─ LoadDownFlashWriteByte()  // 写外部Flash
         │    ├─ CheckFlashAppProCode(2)  // CRC校验
         │    ├─ CanSendRemLoadMsgAPI()   // 上报状态
         │    └─ CanLoaderOkHook()        // 记录完成
         └─ [eREMFIN] → LoadDownFlashWriteByte() + CheckFlashAppProCode(2)

周期调度 (OSUserHOOK.c):
  ├─ LoadAppFlashErasHandleAPI() [300ms]
  │    └─ LoadDownFlashEraseSector()  // 擦除外部Flash
  │
  └─ CanSendRemLoadMsgAPI() [50ms]
       └─ BSPCANSendMsg()  // 上报状态/进度
```
