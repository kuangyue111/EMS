# S32K146OFBMU 电池管理系统（BMS）架构深度剖析

> **文档版本**: V2.0  
> **生成日期**: 2026-04-21  
> **代码规模**: 18.3 万行代码，343 个源文件  
> **硬件平台**: NXP S32K146 (ARM Cortex-M4F @ 80MHz)  
> **适用对象**: 嵌入式软件工程师、BMS 开发者  

---

## 📋 目录

1. [整体架构层次](#一整体架构层次)
2. [主要模块及其职责](#二主要模块及其职责)
3. [核心数据结构](#三核心数据结构)
4. [关键算法详解](#四关键算法详解)
5. [通信协议](#五通信协议)
6. [任务调度机制](#六任务调度机制)
7. [硬件抽象层设计](#七硬件抽象层设计)
8. [模块关系图](#八模块关系图)
9. [关键代码片段](#九关键代码片段说明)
10. [30 天学习计划](#十 30 天学习计划)

---

## 一、整体架构层次

### 1.1 五层架构图

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        Application Layer (应用层)                            │
│  ┌─────────────┐ ┌───────────────┐ ┌─────────────┐ ┌───────────────────────┐ │
│  │   Logics/   │ │    Funtion/   │ │   PileSys/  │ │     Addition/         │ │
│  │  业务逻辑层 │ │   功能算法层  │ │  电堆管理层 │ │   附加功能层          │ │
│  │ (状态/均衡) │ │ (SOC/诊断)    │ │ (并联管理)  │ │ (标定/Bootloader)     │ │
│  └─────────────┘ └───────────────┘ └─────────────┘ └───────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                      BSP User API Layer (用户接口层)                          │
│  ┌─────────────────────────────────────────────────────────────────────────┐ │
│  │ BSPUserAPI.h/c -- 统一对外 API 封装                                        │ │
│  │ BSPISRHook.c   -- 中断回调处理                                           │ │
│  │ BSPUserHook.c  -- 事件 Hook 函数                                           │ │
│  └─────────────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                   Device Abstraction Layer (设备抽象层)                       │
│  ┌────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────────────────────┐│
│  │  DEVGPIO   │ │   DEVADC    │ │   DEVCAN    │ │     DEVSCI               │ │
│  │ PCB级 IO 控制│ │ PCB级 ADC 通道│ │ PCB级 CAN 配置│ │    PCB 级串口配置         │ │
│  └────────────┘ └─────────────┘ └─────────────┘ └───────────────────────────┘│
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│               Peripheral Chip Drivers (专用芯片驱动层)                        │
│  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌─────────────────┐ │
│  │ FS65_driver│ │LTC6813_drv│ │LTC2949_drv│ │M24512_drv │ │ SST26VF064_drv │ │
│  │ SBC 安全芯片│ │电芯监控 AFE│ │高压库仑计 │ │外部 EEPROM │ │  外部 Flash     │ │
│  └───────────┘ └───────────┘ └───────────┘ └───────────┘ └─────────────────┘ │
│  ┌───────────┐ ┌───────────┐ ┌───────────┐                                  │
│  │ BF8915A   │ │ MCP3913   │ │ RX8010    │                                  │
│  │国产 AFE 芯片│ │ 电表 AFE   │ │ 外部 RTC   │                                  │
│  └───────────┘ └───────────┘ └───────────┘                                  │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                 MCU Peripheral HAL (MCU 外设抽象层)                           │
│  ┌────────┐ ┌──────┐ ┌─────────┐ ┌───────┐ ┌───────┐ ┌───────┐ ┌────────┐  │
│  │BSPCAN  │ │BSPSPI│ │BSPUART  │ │BSPADC │ │BSPGPIO│ │BSPFTM │ │BSPFLASH│  │
│  │3 路 CAN  │ │4 路 SPI│ │4 路串口  │ │2 模块  │ │5 端口  │ │4 定时器│ │ 1MB   │  │
│  └────────┘ └──────┘ └─────────┐ └───────┘ └───────┘ └───────┘ └────────┘  │
│  ┌─────────┐ ┌───────┐ ┌───────┐ ┌───────┐ ┌───────┐ ┌───────┐             │
│  │BSPEEPROM│ │BSPMCG │ │BSPWDOG│ │BSPPWM │ │BSPI2C │ │BSPRTC │             │
│  │  4KB    │ │时钟 PLL│ │看门狗 │ │PWM 捕获│ │2 路 I2C │ │内部 RTC│             │
│  └─────────┘ └───────┘ └───────┘ └───────┘ └───────┘ └───────┘             │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│               Configuration & Utilities (配置与工具层)                        │
│  ┌─────────────────────┐ ┌────────────────────────────────────────────────┐ │
│  │ BSPCfg/BSPConfig.h  │ │     Util/BSPTypeDef.h + BSPQueue.c             │ │
│  │ 编译时宏配置        │ │     公共数据类型定义 + 环形队列实现            │ │
│  └─────────────────────┘ └────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Hardware (硬件层)                                       │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  MCU: NXP S32K146 (ARM Cortex-M4F @ 80MHz, 1MB Flash, 128KB RAM)        ││
│  │  外设：FS6503(SBC) + LTC6813(电芯 AFE) + LTC2949(库仑计) + M24512(EEPROM)││
│  └─────────────────────────────────────────────────────────────────────────┐│
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 源码目录结构

```
Sources/
├── main.c                    -- 程序入口
├── BSPSys/                   -- 板级支持包 (底层)
│   ├── BSPCfg/               -- 编译配置 (28 个功能使能宏)
│   │   ├── BSPConfig.h       -- 宏定义 + 配置结构体
│   │   └── BSPConfig.c       -- 配置初始化
│   ├── BSPUser/              -- 用户 API 接口层
│   │   ├── BSPUserAPI.h/c    -- 统一对外 API
│   │   ├── BSPISRHook.h/c    -- 中断回调处理
│   │   └── BSPUserHook.h/c   -- 事件 Hook 函数
│   ├── Device/               -- 专用芯片驱动 (8 类)
│   │   ├── FS65_driver/      -- NXP FS6503 SBC (1301 行)
│   │   ├── LTC6813_driver/   -- 电芯监控 AFE (2309 行)
│   │   ├── LTC2949_driver/   -- 高压库仑计 (3178 行)
│   │   ├── BF8915A_driver/   -- 国产 AFE 替代方案 (2165 行)
│   │   ├── M24512_driver/    -- 外部 EEPROM (614 行)
│   │   ├── SST26VF064_driver/-- 外部 Flash (1794 行)
│   │   ├── MCP3913_driver/   -- 电表 AFE (436 行)
│   │   ├── RX8010_driver/    -- 外部 RTC (319 行)
│   │   ├── DEVGPIO/DEVADC/DEVCAN/DEVSCI -- PCB级抽象
│   ├── Driver/               -- MCU 外设 HAL (17 个模块)
│   │   ├── BSPCAN.c/h        -- FlexCAN 驱动 (865 行)
│   │   ├── BSPSPI.c/h        -- LPSPI 驱动 (454 行)
│   │   ├── BSPUART.c/h       -- LPUART 驱动 (382 行)
│   │   ├── BSPADC.c/h        -- ADC 驱动 (287 行)
│   │   ├── BSPGPIO.c/h       -- GPIO 驱动 (279 行)
│   │   ├── BSPFTM.c/h        -- 定时器驱动 (215 行)
│   │   ├── BSPFLASH.c/h      -- Flash 驱动 (696 行)
│   │   ├── BSPEEPROM.c/h     -- EEPROM 驱动 (383 行)
│   │   └── 其他...
│   └── Util/                 -- 公共工具
│       ├── BSPTypeDef.h      -- 数据类型定义
│       ├── BSPQueue.c/h      -- 环形队列实现
├── Funtion/                  -- 功能算法层 (13 个模块)
│   ├── SocLib/               -- SOC/SOE算法库
│   ├── EnergyCalc/           -- 能量/容量/SOH 计算
│   ├── Measurement/          -- 传感器采样模块
│   ├── DataCalc/             -- 限流/滤波/特征计算
│   ├── Diagnosis/            -- 故障诊断系统
│   ├── Statistics/           -- 使用统计模块
│   ├── Recording/            -- 数据记录模块
│   ├── Parameter/            -- 参数管理模块
│   ├── CanLib/               -- CAN 通信协议库
│   ├── ModbusLib/            -- Modbus RTU 协议
│   ├── PowerManag/           -- 电源管理 (FS6503)
│   ├── SystemInfo/           -- 系统信息管理
│   ├── ChgCan/               -- GB/T 27930快充协议
├── Logics/                   -- 业务逻辑层 (6 个模块)
│   ├── Tactics/              -- 核心控制策略
│   ├── Balance/              -- 电芯均衡策略
│   ├── CanUser/              -- CAN 消息处理
│   ├── FstChg/               -- 快充策略
│   ├── SciUser/              -- Modbus 用户接口
│   ├── EventUser/            -- 事件记录
├── PileSys/                  -- 电堆管理系统 (8 个模块，条件编译)
│   ├── PileCalc/PileCan/PileDiag/PilePara/PileSci/PileStati/PileTact/PileUser
├── OSUser/                   -- FreeRTOS 接口层
│   ├── OSUserHOOK.c/h        -- 任务与定时器定义
├── Addition/                 -- 附加功能 (5 个模块)
│   ├── Calibration/          -- 电流/高压标定
│   ├── LoadApp/              -- CAN Bootloader
│   ├── OperAuth/             -- 操作权限系统
│   ├── UserDef/              -- 用户自定义功能
│   ├── UserProto/            -- 用户协议 (CATL 等)
```

---

## 二、主要模块及其职责

### 2.1 BSP 层模块职责

| 模块 | 文件 | 职责 |
|------|------|------|
| **BSPCfg** | BSPConfig.h/c | 编译时宏配置 (28 个功能开关)、运行时参数结构体 |
| **Driver** | BSPCAN/BSPSPI/BSPUART等 | MCU 外设硬件抽象层 (HAL) |
| **Device** | LTC6813/LTC2949/FS65等 | 专用芯片驱动初始化与操作 |
| **BSPUser** | BSPUserAPI.c | 统一 API 封装 (BSPxxx/DEVxxx 命名) |
| **Util** | BSPTypeDef.h/BSPQueue.c | 数据类型定义、环形队列实现 |

### 2.2 功能层模块职责

| 模块 | 核心文件 | 职责 |
|------|----------|------|
| **SocLib** | SocUserAPI.c, SocSoeCorr.c, CapEnerCorr.c | SOC/SOE 计算、校正、同步算法 |
| **EnergyCalc** | EnerInterface.c, CurrIntegral.c | 容量/能量积分、SOH 衰减计算 |
| **Measurement** | CellAFESamp.c, CurrSample.c, InsuCheck.c | 电芯电压/温度采样、电流采样、绝缘检测 |
| **DataCalc** | CurrLimCalc.c, CharactCalc.c | 电流限流计算、电压特征检查、温度降额 |
| **Diagnosis** | DiagFilter.c, DiagMaker.c, DiagSos.c | 故障滤波、故障生成、SOS 等级计算 |
| **Statistics** | DodRcdStatis.c, EnergyStatis.c | DOD 记录、能量统计、循环次数统计 |
| **Recording** | RecordHandle.c, RecordWave.c | 事件/故障记录、波形记录 |
| **Parameter** | ParaInterface.c, ParaIntEep.c | 参数读写、EEPROM 地址映射 |
| **CanLib** | CANInterface.c, CANTxCycle.c | CAN 协议栈、周期上报、多帧传输 |
| **ModbusLib** | MbInterface.c, MbServer.c | Modbus RTU 协议实现 |
| **PowerManag** | Fs6503_PowerManag.c | FS6503 看门狗刷新、LDT 定时器管理 |
| **SystemInfo** | GroupInfo.c, PublicInfo.c | 工作状态/保护状态/电流状态枚举 |

### 2.3 逻辑层模块职责

| 模块 | 核心文件 | 职责 |
|------|----------|------|
| **Tactics** | GroupTactic.c, GroupProtect.c, GroupState.c | 接触器控制、保护执行、状态机管理 |
| **Balance** | BalanTactic.c | 被动均衡策略 (电压差判定、温度保护) |
| **CanUser** | GroupMsg.c, SlaveMsg.c | BMU CAN 消息处理、从控通信 |
| **FstChg** | ChgTactics.c | GB/T 27930快充协议 8 阶段状态机 |
| **SciUser** | SciCtrlHandle.c, ModbusMap.c | Modbus 寄存器映射 (6500+ 地址) |
| **EventUser** | EventUser.c | 状态变化、端口变化、控制命令事件记录 |

---

## 三、核心数据结构

### 3.1 BSP 配置数据结构

```c
// 功能使能联合位域 (28 个开关压缩在 u32)
typedef union _FunConfig_ {
    u32 Word;
    struct {
        u16 FTM0En:1, FTM1En:1, FTM2En:1, FTM3En:1;    // 定时器
        u16 CAN0En:1, CAN1En:1, CAN2En:1;              // CAN
        u16 SCI0En:1, SCI1En:1, SCI2En:1;              // UART
        u16 SPI0En:1, SPI1En:1, SPI2En:1, SPI3En:1;    // SPI
        u16 I2C0En:1, I2C1En:1, GPIOEn:1, ADCEn:1;     // 其他外设
        u16 RTCEn:1, PWMEn:1, EEPEn:1, FLASHEn:1;      // 存储/定时
        u16 WDOGEn:1, LPITEn:1, DMAEn:1;               // 系统功能
    }Bits;
} u_BSPFunCfg;

// 运行时参数结构体
typedef struct {
    u8 OscRate, BusRate;           // 晶振 16MHz, 总线 80MHz
    u16 Ftm0Time~Ftm3Time;         // 定时器中断周期
    u16 Can0Rate~Can2Rate;         // CAN 波特率 500kbps
    u16 Sci0Rate~Sci2Rate;         // UART 波特率 19200bps
    u16 Spi0Rate~Spi3Rate;         // SPI 速率
} s_BSPParaCfg;
```

### 3.2 SOC/SOE 数据结构

```c
// 容量结构 (单位：1mAH)
typedef struct {
    u32 nowCap;    // 当前容量
    u32 remCap;    // 剩余容量
    u32 baseCap;   // 基准容量
    u32 topCap;    // 顶部容量
    u32 standCap;  // 标称容量
} t_CAPFORM;

// 能量结构 (单位：1WH)
typedef struct {
    u32 nowEner, remEner, baseEner, topEner, standEner;
} t_ENERFORM;

// 积分结构
typedef struct {
    u32 chgCap10ma1ms;  // 充电容量积分 (10mA 精度)
    u32 dhgCap10ma1ms;  // 放电容量积分
    u32 chgCap1ma1h;    // 累计充电容量
    u32 dhgCap1ma1h;    // 累计放电容量
} t_CAPINT;

// 衰减结构 (SOH)
typedef struct {
    u16 fadeCap;     // 衰减容量 (0.1AH)
    u16 ratedCap;    // 额定容量
    u16 fadeCycle;   // 衰减循环次数
    u16 ratedCycle;  // 额定循环次数
} t_FADEFORM;
```

### 3.3 CAN 通信数据结构

```c
// CAN 帧结构
typedef struct {
    u32 ID;          // 29 位扩展 ID
    u8 DLC;          // 数据长度
    u8 Data[8];      // 数据域
} t_CANFram;

// CAN ID 解析 (29 位)
typedef union {
    u32 Word;
    struct {
        u16 saddr:8;       // 源地址
        u16 daddr:8;       // 目的地址
        u16 mindex:8;      // 消息索引
        u16 attrib:1;      // 帧属性
        e_CANFunCode funcode:4;  // 功能码
        u16 rsv:3;         // 保留
    }Bits;
} u_CANId;

// 功能码定义
typedef enum {
    eCANFUN_VFT=0,   // 校时帧
    eCANFUN_BRE=1,   // 广播帧
    eCANFUN_RUP=2,   // 请求修改
    eCANFUN_AUP=3,   // 回复修改
    eCANFUN_REQ=5,   // 请求查询
    eCANFUN_UPM=8,   // 参数上传 (多帧)
    eCANFUN_UPP=12,  // 参数上传 (单帧)
} e_CANFunCode;
```

### 3.4 故障诊断数据结构

```c
// 错误 ID 系统 (96+ 种)
typedef enum {
    eERR_ID_CEL_VH2 = 0,    // 电芯过压 L2
    eERR_ID_CEL_VH1 = 16,   // 电芯过压 L1
    eERR_ID_SOC_ERR = 32,   // SOC 错误
    eERR_ID_RTC_ERR = 64,   // RTC 错误
    eERR_ID_CONTACTOR_ERR = 80, // 接触器故障
} e_ErrId;

// SOS 安全等级
typedef enum {
    SOS_LEVEL_0 = 0,   // 最高安全
    SOS_LEVEL_1~3,     // 逐渐降低
    SOS_LEVEL_4,       // 最低 (锁定)
} e_SosLevel;

// 保护状态枚举
typedef enum {
    eProState_ChgLim,   // 充限流
    eProState_DhgLim,   // 放限流
    eProState_ChgPro,   // 充保护
    eProState_DhgPro,   // 放保护
    eProState_SysPro,   // 系统保护
    eProState_SysErr,   // 系统错误
} e_ProState;
```

---

## 四、关键算法详解

### 4.1 SOC 计算算法

**核心原理：安时积分法 + 多重校正**

```
SOC = SOC₀ + ∫(I × η) / Capacity dt

其中:
- I: 实时电流 (充电正，放电负)
- η: 库仑效率 (充电 101.5%, 放电 100%)
- Capacity: 当前可用总容量
```

**校正机制：**

| 校正类型 | 触发条件 | 目标 SOC |
|---------|---------|---------|
| 高压校正 (VHP) | 充电电压到达 VH2Lim+110~150mV | 99.3% |
| 低压校正 (VLP) | 放电电压到达 VL2Lim-150mV | 0.13% |
| 温度校正 | 2 维查表 (温度×C 率) | 补偿容量 |
| 满充检测 | SOC>=85% 且空闲>=144h | 触发内阻测量 |

**关键代码：**

```c
void SocSoeLibAllTask(void)  // 50ms 周期
{
    switch(sTime) {
        case 0:  TotalCapEnerCorrTask();      // 容量能量校正 (1s)
        case 4:  SocSoeTogetherSyncTask();    // SOC/SOE内部同步 (500ms)
        case 6:  SocSoeGroupSyncTask();       // 包间同步 (500ms)
        case 8:  SocSoeAllCorrTask();         // SOC 校正 (500ms)
    }
}
```

**显示平滑：SOC 缓变速率 5%/s**

### 4.2 均衡策略算法

**被动均衡状态机 (100ms 周期)：**

```c
void GroupVoltBalanTacticTask(void)
{
    switch(sState) {
        case 0: // 前提条件检查
            if(调试模式 || 均衡禁用 || 通信断开) {
                ClrBalanCheckData();  // 清除数据
            }
            break;
        case 1: // 积分计时 (~300s)
            CalcCellVoltSubInte();    // 累计电压偏差
            break;
        case 2: // 判定决策
            JudgeBalanReqByPasCir();  // 决定哪些电芯均衡
            break;
        case 3: // 更新结果
            UpdateBalanShowState();   // 发送均衡指令
            break;
    }
}
```

**三种触发条件：**

1. **最低电压阈值法：** `Vcell_min >= BALAN_DHG_MIN_VOLT`，放电高于最低电压的电芯
2. **平均值阈值法：** `Vavg >= 阈值 && Vmax >= Vavg + 阈值`，放电高于平均的电芯
3. **低压保护余量法：** `Vavg >= 阈值 && Vmin >= VL2 + 80mV`

**温度保护：**
- 从控温度 >= 80°C：禁止全部均衡
- 从控温度 >= 60°C：每从控最多 8 个电芯均衡（按电压偏差冒泡排序）

**均衡阈值（按化学类型）：**

| 化学类型 | 阈值 |
|---------|------|
| NCM(三元) | 3600mV |
| LFP(磷酸铁锂) | 3100mV |
| LTO(钛酸锂) | 1900mV |

### 4.3 保护逻辑算法

**保护等级体系：**

```c
// 错误严重等级
0=Normal -> 1=Warning -> 2=ChgLimit -> 3=DhgLimit -> 4=SysLimit
-> 5=ChgPro -> 6=DhgPro -> 7=SysPro -> 8=SysErr
```

**保护执行流程 (100ms 周期)：**

```c
void GroupProtectTask(void)
{
    // 1. 空闲异常保护 (强制充放电)
    if(IDLE 状态 && 有电流 && |I| >= 50A)
        GroupProCtrlCheck(eProSort_IdlExp);
    
    // 2. 系统错误保护
    if(WORK_ERR 状态 || SOS==SysErr)
        GroupProCtrlCheck(eProSort_SysErr);
    
    // 3. 系统保护 (充放电同时保护)
    if(SOS==SysPro)
        GroupProCtrlCheck(eProSort_SysPro);
    
    // 4. 充电保护
    if(SOS==ChgPro && 充电状态)
        GroupProCtrlCheck(eProSort_ChgPro);
    
    // 5. 放电保护
    if(SOS==DhgPro && 放电状态)
        GroupProCtrlCheck(eProSort_DhgPro);
}
```

**保护延时机制：**

| 保护类型 | 延时时间 | 执行动作 |
|---------|---------|---------|
| 充限流 | 参数配置 | 降低充电电流 |
| 放限流 | 参数配置 | 降低放电电流 |
| 充保护 | 参数配置 (秒级) | 断开充电接触器 |
| 放保护 | 参数配置 (秒级) | 断开放电接触器 |
| 系统保护 | 空闲 1s 后执行 | 断开所有接触器 |
| 系统错误 | 空闲 1s 后执行 | 系统关机 |

### 4.4 电流限流算法

**温度降额表 (15 个温度点)：**

```c
// gEepCfg_105[] 格式：{温度 (0.1°C), 充电 C 率 (‰), 放电 C 率 (‰)}
// 例如：{-300, 100, 200} 表示 -30°C 时充电 0.1C，放电 0.2C
```

**CV 降额 (充电末端平滑)：**

```c
// 10 级 SLOW_CURRLIMIT_NUM 表
// 定义：{电压偏移，电流百分比 (%)}
// 示例：{+50mV, 90%}, {+30mV, 70%}, {+20mV, 50%}
```

**PI 控制器结构：**

```c
typedef struct {
    s32 piIn, piInBak, piOut, piSum;  // 状态变量
} t_PIIO;

typedef struct {
    s32 piKp, piKpR;    // 比例系数
    s32 piKi, piKiR;    // 积分系数
    s32 piInMax/Min, piSumMax/Min, piOutMax/Min;  // 限幅
} t_PICfg;
```

---

## 五、通信协议

### 5.1 CAN 通信架构

**硬件配置：**

| 总线 | 用途 | 速率 | 接收缓冲 |
|------|------|------|---------|
| CAN0 | 外部通信/上位机 | 500kbps | 5 槽环形缓冲 |
| CAN1 | 从控板 BSU 通信 | 500kbps | 5 槽环形缓冲 |
| CAN2 | 外部通信/充电机 | 500kbps | 5 槽环形缓冲 |

**消息周期：**

| 消息 | 内容 | 周期 |
|------|------|------|
| Msg 50 | 系统状态 | 500ms |
| Msg 51 | 充放电限制 | 500ms |
| Msg 60 | 系统主信息 | 500ms |
| Msg 92/93 | 电芯电压 (分页) | 500ms |
| Msg 94/95 | 电芯温度 | 1s |

### 5.2 Modbus RTU 协议

**配置：**

| 参数 | 值 |
|------|-----|
| 波特率 | 19200bps(默认) |
| 数据位 | 8 |
| 停止位 | 1 |
| 校验 | CRC16 |
| 帧间隔 | t3.5 = 2ms(19200bps) |

**寄存器映射 (约 6500 个)：**

| 地址范围 | 描述 |
|---------|------|
| 0-12 | 负载命令寄存器 |
| 90-148 | 设备基本信息/故障记录 |
| 450-500 | 系统时钟/序列号 |
| 500-650 | 系统控制命令 (需权限) |
| 650-990 | 参数配置 |
| 5000-6500 | 组信息 (状态/电压/温度) |

### 5.3 GB/T 27930快充协议

**8 阶段状态机：**

| 阶段 | 状态 | 退出条件 |
|------|------|---------|
| Stage 0 | Idle | 充电枪插入 (FC>=90V) |
| Stage 1 | Link | CC2 电压约 2.5V |
| Stage 2 | Handshake | 收到 CRM+CBRM |
| Stage 3 | Config | 收到 CML |
| Stage 30 | Config-Confirm | 收到 CRO |
| Stage 4 | Charging | 保护/停止命令 |
| Stage 5 | Stopping | 电流降到空闲 |
| Stage 6 | Finish | FC<9V 且 CC2 约 5V |

---

## 六、任务调度机制

### 6.1 FreeRTOS 定时器任务

**11 个软件定时器：**

| 定时器 | 周期 | 主要任务 |
|--------|------|---------|
| Timer2ms | 2ms | Modbus 触发发送 |
| Timer5ms | 5ms | CAN 协议栈处理、FS6503 诊断 |
| Timer10ms | 10ms | 端口检查、10 步轮询器 |
| Timer50ms | 50ms | 故障滤波、SOC 算法 |
| Timer100ms | 100ms | 状态机、保护执行 |
| Timer200ms | 200ms | 热管理、电压限计算 |
| Timer300ms | 300ms | 循环统计、历史记录 |
| Timer400ms | 400ms | 能量统计、从属 MCU 通信 |
| Timer500ms | 500ms | 快充策略、辅助设备管理 |
| Timer1000ms | 1s | DOD 统计、外设检查 |
| Timer2000ms | 2s | 掉电保护、RTC 检查 |

### 6.2 10 步轮询器 (Task10ms 核心调度)

```c
switch(sTime) {
    case 0: CurrLimCalcTask(); CurrLimFallTask();     // 限流计算
    case 1: CheckVoltCharaTask(); CheckTempCharaTask();// 特征检查
    case 2: DiagVoltLimFilMakerTask(); ...            // 故障生成 1
    case 3: DiagPortExpFilMakerTask(); ...            // 故障生成 2
    case 4: DiagStateExpFilMakerTask(); ...           // 故障生成 3
    case 5: GroupCapSocCalcTask(); ...                // SOC 计算
    case 6: GroupFadeSohCalcTask(); ...               // SOH 计算
    case 7: PwmInfoCaptureTask(); GroupBattStateCheckTask(); // 均衡
    case 8: InsuAdhesMeasurTask(); GroupProStateTask();      // 保护
    case 9: CHGLibSendMsgTask(); ParaStoreCopyTask();        // 快充/存储
}
```

### 6.3 RTOS 常驻任务

| 任务名 | 优先级 | 栈大小 | 职责 |
|--------|--------|--------|------|
| CellInfoSampleTask | 18 | 300 words | LTC6813 电芯采样 (菊花链) |
| HigInfoSampleTask | 14 | 400 words | LTC2949 高压采样 |
| LowInfoSampleTask | 13 | 120 words | 低压 ADC 采样 |
| BSPCANRcvTask | 15 | 400 words | CAN 接收处理 |
| BSPCANSendTask | 17 | 400 words | CAN 发送处理 |
| BSPUARTRcvTask | 16 | 200 words | UART/Modbus 处理 |

### 6.4 硬件中断机制

| 中断源 | 周期 | 职责 |
|--------|------|------|
| FTM1 | 0.1ms | Modbus 帧间隔检测 |
| FTM3 | 1ms | 电流积分累加、系统时钟 |
| CAN0/1/2 | 事件 | CAN 收发 + 错误中断 |
| UART0/1/2 | 事件 | 字节收发中断 |
| SPI0/1/2/3 | 事件 | SPI 传输完成中断 |

---

## 七、硬件抽象层设计

### 7.1 HAL 层设计原则

**设计哲学：隔离"变"与"不变"**

- **Device 层**：处理芯片差异 (LTC6813 vs BF8915A)
- **Driver 层**：处理 MCU 差异 (S32K vs STM32)
- **BSPUser 层**：提供统一接口
- **App 层**：纯逻辑，不依赖特定硬件

### 7.2 SPI 通道分配

| SPI 通道 | 连接设备 | 用途 |
|---------|---------|------|
| SPI0 | FS6503 | 安全 SBC(看门狗/电源) |
| SPI1 | LTC2949 | 高压库仑计 (电流/功率) |
| SPI2 | LTC6813 | 电芯监控 AFE(菊花链) |
| SPI3 | SST26VF064 | 外部 Flash(数据记录) |

### 7.3 GPIO 抽象

**端口定义：**

```c
// 端口 A-E, 16 引脚/端口
typedef enum {
    eGPIO_PORTA, eGPIO_PORTB, eGPIO_PORTC, eGPIO_PORTD, eGPIO_PORTE
} e_GPIOPort;
```

**12 路接触器控制：**

| 编号 | 名称 | 驱动 | 说明 |
|------|------|------|------|
| 1 | 主接触器 | HS1+LS1 | 主回路 |
| 2 | 预充接触器 | HS2 | 预充电阻 |
| 3 | 负接触器 | HS3+LS3 | 负极回路 |
| 4 | 辅助电源接触器 | HS4 | 辅助供电 |
| 5-6 | RUN/ALARM 灯 | HS5/HS6 | 指示灯 |
| 7-9 | 烟感/加热/风机 | HS7-HS9 | 辅助控制 |
| 10-12 | 干接点 | DRY0-DRY2 | 状态输出 |

---

## 八、模块关系图

### 8.1 数据流图

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           传感器数据采集层                               │
│  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐ │
│  │ LTC6813   │ │ LTC2949   │ │ ADC(霍尔) │ │ ADC(低压) │ │ 绝缘电阻  │ │
│  │电芯电压/温度│ │电流/总压/功率│ │霍尔电流  │ │电源/温度 │ │正/负对地 │ │
│  └───────────┘ └───────────┘ └───────────┘ └───────────┘ └───────────┘ │
│       │              │              │              │              │      │
│       ▼              ▼              ▼              ▼              ▼      │
│  ┌─────────────────────────────────────────────────────────────────────┐│
│  │                     Measurement 模块 (采样处理)                       ││
│  │  CellAFESamp.c → CurrSample.c → LowADCSamp.c → InsuCheck.c          ││
│  │  4 点滑动平均滤波 → 零点校准 → 温度查表 → 状态机采样                   ││
│  └─────────────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                           状态数据层                              │
│  ┌─────────────────────────────────────────────────────────────────────┐│
│  │  GroupInfo.c (全局状态结构体)                                        ││
│  │  - gCellVolt[240] 电芯电压数组                                       ││
│  │  - gCellTemp[240] 电芯温度数组                                       ││
│  │  - gOutCurr 输出电流                                                 ││
│  │  - gTotalVolt 总电压                                                 ││
│  │  - gInsuRes 正/负绝缘电阻                                            ││
│  └─────────────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                           算法计算层                                     │
│  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐ │
│  │ SocLib    │ │EnergyCalc │ │ DataCalc  │ │ Diagnosis │ │ Statistics│ │
│  │ SOC/SOE   │ │ SOH/积分  │ │限流/特征  │ │ 故障判定  │ │ 循环统计 │ │
│  └───────────┘ └───────────┘ └───────────┘ └───────────┘ └───────────┘ │
│       │              │              │              │              │      │
│       ▼              ▼              ▼              ▼              ▼      │
│  ┌─────────────────────────────────────────────────────────────────────┐│
│  │  PublicInfo.c (公共状态输出)                                         ││
│  │  - SOC/SOE/SOH 显示值                                                ││
│  │  - 充放电限流值                                                       ││
│  │  - SOS 安全等级                                                        ││
│  │  - 故障/告警码                                                        ││
│  └─────────────────────────────────────────────────────────────────────┐│
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                           控制执行层                             │
│  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐ │
│  │ Tactics   │ │ Balance   │ │ FstChg    │ │ CanUser   │ │ EventUser │ │
│  │接触器控制 │ │均衡指令   │ │快充流程   │ │消息上报   │ │事件记录  │ │
│  └───────────┘ └───────────┘ └───────────┘ └───────────┘ └───────────┘ │
│       │              │              │              │              │      │
│       ▼              ▼              ▼              ▼              ▼      │
│  ┌─────────────────────────────────────────────────────────────────────┐│
│  │  硬件执行层                                              ││
│  │  DEVGPIO.c → 接触器驱动 → 状态反馈                                    ││
│  │  LTC6813 均衡寄存器 → 被动放电电路                                     ││
│  │  CAN 发送队列 → 周期上报                                               ││
│  │  Flash 写入 → 事件持久化                                               ││
│  └─────────────────────────────────────────────────────────────────────┐│
└─────────────────────────────────────────────────────────────────────────┘
```

### 8.2 模块依赖关系图

```
                    ┌─────────────┐
                    │   main.c    │
                    └─────────────┘
                          │
          ┌───────────────┼───────────────┐
          │               │               │
          ▼               ▼               ▼
   ┌───────────┐   ┌───────────┐   ┌───────────┐
   │BSPUserCfg │   │BSPDriver  │   │RtosTask   │
   │   Init    │   │   Init    │   │  Start    │
   └───────────┘   └───────────┘   └───────────┘
          │               │               │
          ▼               ▼               ▼
   ┌───────────┐   ┌───────────┐   ┌───────────┐
   │ BSPCfg/   │   │ Driver/   │   │ OSUser/   │
   │ BSPConfig │   │ BSPCAN    │   │ Timer 任务 │
   └───────────┘   │ BSPSPI    │   │ Task 任务  │
                   │ ...       │   └───────────┘
                   └───────────┘          │
                          │               │
                          ▼               ▼
                   ┌───────────┐   ┌───────────────────────┐
                   │ Device/   │   │ Funtion/ + Logics/    │
                   │ LTC6813   │   │ SOC 算法 + 保护策略    │
                   │ LTC2949   │   │ 均衡 + 通信处理       │
                   │ FS65      │   └───────────────────────┘
                   └───────────┘
```

---

## 九、关键代码片段说明

### 9.1 系统启动流程

```c
int main(void)
{
    PEX_RTOS_INIT();               // FreeRTOS 内部初始化
    
    BSPUserCfgInit();              // 1. 配置宏加载到运行时结构体
    BSPDriverInit();               // 2. MCU 外设初始化 (时钟/CAN/SPI/UART)
    
    BSP_DISABLE_IRQ();             // 3. 关闭总中断 (临界区)
    BSPDisIRQUserInit();           // 4. 关中断下初始化 (EEPROM/参数)
    
    BSP_ENABLE_IRQ();              // 5. 开启总中断
    BSPEnIRQUserInit();            // 6. 开中断后初始化 (SOC 恢复)
    
    PEX_RTOS_START();              // 7. 启动 FreeRTOS 调度器
    // 此后永不返回，进入多任务并发
}
```

### 9.2 电流积分中断 (1ms 精度)

```c
// FTM3 中断 (1ms)
void Time_ISR_Hook(eFTM3)
{
    ISRCurrCapAddTask();    // ★ 电流积分累加
    SysClockCountTask();    // 毫秒计数
    MbClientTimerTask();    // Modbus 客户端计时
    CANLibTxRcvHandleTask_API();  // CAN 协议栈处理
}
```

**设计理由：** 中断保证严格 1ms 间隔，避免任务抖动导致 Ah 误差累积。

### 9.3 均衡判定核心逻辑

```c
static void JudgeBalanReqByPasCir(void)
{
    for(slave = 0; slave < slaNum; slave++) {
        // 温度保护：>=80°C 禁止全部均衡
        if(GetGBalanSlavMaxTAPI(slave) >= BALAN_PAS_PRO_TEMP) {
            sGPasBalanReqState[slave] = 0;
        }
        else {
            // 累计电压偏差判定
            for(slaCel = 0; slaCel < slaCelNum; slaCel++) {
                if(sGBalanSubVolt[clusterCel] > 阈值) {
                    BitSet(sGPasBalanReqState[slave], slaCel);
                }
            }
            
            // 高温限流：>=60°C 最多 8 个电芯
            if(T >= 60°C && onNum > 8) {
                // 按电压偏差冒泡排序，取前 8 个
                冒泡排序;
                设置前 8 个均衡位;
            }
        }
    }
}
```

---

## 十、30 天学习计划

详细学习计划请参考 `BMS_30 天学习计划.md` 文档。

### 快速导航

| 阶段 | 时间 | 主题 | 关键任务 |
|------|------|------|---------|
| **第一阶段** | 第 1-7 天 | 基础认知 | 开发环境、BSP 驱动、HAL 层 |
| **第二阶段** | 第 8-20 天 | 功能模块 | 数据采集、SOC 算法、CAN 通信 |
| **第三阶段** | 第 21-27 天 | 应用逻辑 | 状态管理、均衡策略、事件记录 |
| **第四阶段** | 第 28-30 天 | 系统集成 | 联调测试、独立实现 |

### 每日学习时间分配建议

- **2 小时**：阅读源代码
- **2 小时**：实践编程
- **1 小时**：记录笔记和绘制图表

---

## 十一、架构特点总结

### 11.1 设计亮点

1. **清晰的分层架构**：BSP-HAL-Device-API-App 五层分离，易于移植
2. **精确的时间控制**：电流积分在 ISR 中执行，保证 SOC 精度
3. **10 步轮询器**：打散 100ms 任务到 10 个 10ms 时隙，避免 CPU 峰值负载
4. **多重保护机制**：6 级保护等级 + SOS 安全等级 + 滤波消抖
5. **完善的校正体系**：高压校正、低压校正、温度补偿、C 率补偿
6. **灵活的均衡策略**：三种触发条件 + 温度保护 + 高温限流
7. **冗余存储设计**：外部 EEPROM + 内部 EEPROM + 内部 Flash 三级备份
8. **Bootloader 支持**：CAN 远程升级 + 外部 Flash 中转 + CRC 校验

### 11.2 技术规格

| 项目 | 参数 |
|------|------|
| MCU | NXP S32K146 (Cortex-M4F @ 80MHz) |
| Flash | 1MB 代码 + 8MB 外部记录 |
| RAM | 128KB |
| RTOS | FreeRTOS (11 定时器 + 7 任务) |
| 最大从控 | 15 块 BSU |
| 最大电芯 | 240 串 (15×16) |
| SOC 精度 | 安时积分 + 多重校正 |
| 通信接口 | 3 路 CAN + 4 路 UART + 4 路 SPI |
| 支持电芯 | LFP/NCM/LTO/铅酸 |

---

## 十二、改进建议

### 12.1 架构优化

1. **增加 OS 抽象层**：当前直接使用 FreeRTOS API，建议封装为 `OSUserAPI` 便于更换 RTOS
2. **统一错误处理**：建议增加全局错误码返回机制，替代分散的 `e_FunStatus`
3. **日志系统增强**：建议增加分级日志 (LOG_DEBUG/INFO/WARN/ERROR)，便于调试

### 12.2 算法优化

1. **SOC 卡尔曼滤波**：当前仅用安时积分 + 电压校正，可引入卡尔曼滤波提升精度
2. **主动均衡支持**：当前仅被动均衡，可扩展为主动均衡能量转移
3. **AI 预测**：可引入 ML 模型预测电池寿命和故障概率

---

## 📚 推荐学习资料

### 数据手册
1. **S32K146 Reference Manual** - NXP 官方参考手册
2. **LTC6813 Datasheet** - Analog Devices 电池监控芯片
3. **FS65 Data Sheet** - NXP 电源管理芯片

### 技术文档
1. 《锂离子电池管理系统设计》
2. 《电动汽车 BMS 技术详解》
3. 《CAN 总线协议实战》

### 在线资源
- NXP S32K 社区论坛
- EEVblog 电池管理讨论区
- GitHub 开源 BMS 项目参考

---

> **最后提醒**: BMS 是安全关键系统，代码质量关乎人身安全。学习过程中要始终牢记：
> - 代码可靠性第一
> - 保护功能必须冗余设计
> - 充分测试才能上车/上储能柜
> - 遵循车规级/储能行业标准

**祝你学习顺利！30 天后期待你成为 BMS 专家！** 🚀

---

*文档生成时间：2026-04-21*  
*文档版本：V2.0*  
*作者：AI Assistant*
