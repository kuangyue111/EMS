# BMS 软件系统 30 天学习计划

> **目标**: 在 30 天内深入理解 BMS 系统架构，并能够独立实现 BMS 软件代码
> **适用对象**: 具有 C/C++ 基础的嵌入式开发者
> **项目规模**: 18.3 万行代码，343 个源文件
> **硬件平台**: NXP S32K146 车规级 MCU

---

## 📚 系统架构概览

```
┌─────────────────────────────────────────────────────────────┐
│                      应用层 (Logics)                        │
│  ┌─────────────┬─────────────┬─────────────┬─────────────┐  │
│  │  电池管理   │  CAN 通信    │   均衡控制  │   事件记录  │  │
│  │  GroupState │  CanUser    │  Balance    │  EventUser  │  │
│  └─────────────┴─────────────┴─────────────┴─────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                     功能层 (Function)                        │
│  ┌─────────────┬─────────────┬─────────────┬─────────────┐  │
│  │  能量计算   │  数据采集   │   CAN 库     │   参数管理  │  │
│  │ EnerCalc    │ Measurement │  CanLib     │  Parameter  │  │
│  ├─────────────┼─────────────┼─────────────┼─────────────┤  │
│  │  SOC/SOH    │  诊断系统   │   Modbus    │   充电控制  │  │
│  │ SocLib      │ Diagnosis   │  ModbusLib  │  ChgCan     │  │
│  └─────────────┴─────────────┴─────────────┴─────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                    驱动层 (BSPSys)                           │
│  ┌─────────────┬─────────────┬─────────────┬─────────────┐  │
│  │  GPIO/ADC   │  CAN/SPI    │  EEPROM     │   看门狗    │  │
│  │  定时器/PWM │  中断管理   │   FLASH     │    RTC      │  │
│  └─────────────┴─────────────┴─────────────┴─────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                      硬件抽象层 (HAL)                         │
│         LTC6813 电池监控芯片驱动 (AFE 前端采集)                │
│         FS65 电源管理芯片驱动                                 │
│         外部 EEPROM/FLASH 存储驱动                            │
└─────────────────────────────────────────────────────────────┘
```

---

## 📅 第一阶段：基础认知 (第 1-7 天)

### 第 1-2 天：开发环境与硬件基础

**学习目标**:
- 搭建完整的开发环境
- 理解 S32K146 MCU 硬件架构
- 掌握 Processor Expert 配置工具

**学习内容**:
1. **开发环境搭建**
   - S32 Design Studio 安装与配置
   - Processor Expert 插件使用
   - 调试器 (PE Micro) 配置

2. **硬件架构理解**
   - S32K146 核心资源：80MHz ARM Cortex-M4
   - 存储器映射：1MB Flash, 128KB SRAM
   - 外设资源：3 路 CAN, 3 路 UART, 4 路 SPI, 18 路 ADC

3. **阅读关键文件**
   ```
   Sources/main.c                    # 主入口与系统初始化
   Sources/BSPSys/BSPCfg/BSPConfig.h # 系统配置宏定义
   Sources/OSUser/OSUserHOOK.h       # RTOS 任务优先级定义
   ```

**实践任务**:
- [ ] 成功编译工程并下载到开发板
- [ ] 通过调试器单步跟踪启动流程
- [ ] 绘制系统启动流程图

---

### 第 3-4 天：BSP 驱动层深入

**学习目标**:
- 掌握底层驱动编程模型
- 理解硬件抽象层设计思想

**核心模块**:
```
Sources/BSPSys/Driver/
├── BSPGPIO.c/h      # GPIO 驱动 (继电器/接触器控制)
├── BSPADC.c/h       # ADC 驱动 (模拟信号采集)
├── BSPCAN.c/h       # CAN 驱动 (通信基础)
├── BSPUART.c/h      # UART 驱动 (RS485/MODBUS)
├── BSPSPI.c/h       # SPI 驱动 (AFE 芯片通信)
├── BSPI2C.c/h       # I2C 驱动 (EEPROM/RTC)
├── BSPFTM.c/h       # 定时器驱动 (PWM 输出)
├── BSPLPIT.c/h      # 低功耗定时器 (系统节拍)
├── BSPWDOG.c/h      # 看门狗驱动 (系统保护)
└── BSPFLASH.c/h     # Flash 驱动 (参数存储)
```

**关键 API 学习**:
```c
// GPIO 控制示例
u8 BSPGPIOInit(eGPIOPORT port, eGPIOPIN pin, eGPIOPORTMUX portmode, u8 dir);
u8 BSPGPIOSetPortState(eGPIOPORT port, eGPIOPIN pin, e_IOState data);

// ADC 采集示例
u8 BSPADCInit(e_ADCModule module, e_ADCChan *chan, u8 num);
u16 BSPADCGetSampData(e_ADCModule module, e_ADCChan channel);

// CAN 通信示例
u8 BSPCANInit(e_CANNUM canNum, u8 osc, u16 rate);
u8 BSPCANSendMsg(e_CANNUM canNum, t_CANMSG *msg);
```

**实践任务**:
- [ ] 编写 GPIO 控制 LED 闪烁程序
- [ ] 实现 ADC 电压采集并串口打印
- [ ] 实现 CAN 自发自收测试

---

### 第 5-7 天：硬件抽象层 (HAL)

**学习目标**:
- 理解 AFE 前端采集芯片工作原理
- 掌握电池监控芯片 LTC6813 驱动

**核心模块**:
```
Sources/BSPSys/Device/
├── LTC6813_driver/     # LTC6813 电池监控芯片 (18 串电池)
├── LTC2949_driver/     # LTC2949 电流检测芯片
├── FS65_driver/        # FS65 电源管理芯片
├── BF8915A_driver/     # BF8915A AFE 芯片 (备选方案)
├── M24512_driver/      # M24512 EEPROM 驱动
├── SST26VF064_driver/  # SST26VF064 Flash 驱动
├── RX8010_driver/      # RX8010 RTC 时钟驱动
└── MCP3913_driver/     # MCP3913 电能计量芯片
```

**LTC6813 关键知识点**:
- daisy-chain 菊花链通信架构
- 电压采集精度：1mV
- 温度采集：4 路 GPIO 可配置
- 被动均衡控制：每串 300mA

**实践任务**:
- [ ] 阅读 LTC6813 数据手册关键章节
- [ ] 分析 `CellAFESamp.c` 电池采样代码
- [ ] 绘制电池电压采集时序图

---

## 📅 第二阶段：功能模块 (第 8-20 天)

### 第 8-10 天：数据采集系统

**学习目标**:
- 理解电池数据采集完整链路
- 掌握滤波算法与异常处理

**核心模块**:
```
Sources/Funtion/Measurement/
├── CellAFESamp.c/h    # 电池电压/温度采集
├── HigAFESamp.c/h     # 高压采集 (总压/绝缘)
├── LowADCSamp.c/h     # 低压 ADC 采集
├── CurrSample.c/h     # 电流采集 (Hall/分流器)
├── InsuCheck.c/h      # 绝缘检测
└── PortCheck.c/h      # 端口状态检测
```

**关键算法**:
```c
// 滑动平均滤波
#define FILTER_NUM 4
uint16_t volt_filter(uint16_t new_val) {
    static uint16_t buf[FILTER_NUM];
    // 实现滑动平均
}

// 采样异常处理
- 连续采样超时处理
- 通信失败重试机制
- 数据合理性校验
```

**实践任务**:
- [ ] 实现滑动平均滤波算法
- [ ] 编写采样数据有效性校验函数
- [ ] 分析并绘制数据采集状态机

---

### 第 11-13 天：能量计算核心 (SOC/SOH/SOE)

**学习目标**:
- 深入理解 SOC 估算算法
- 掌握 SOH 健康度计算方法
- 理解安时积分与开路电压法

**核心模块**:
```
Sources/Funtion/EnergyCalc/
├── EnerInterface.c/h   # 能量计算接口层
├── EnerInfoCalc.c/h    # 能量信息计算
├── SocDisplay.c/h      # SOC 显示计算
├── SoeDisplay.c/h      # SOE 显示计算
├── SohDisplay.c/h      # SOH 显示计算
├── CapInfoCalc.c/h     # 容量计算
├── CurrIntegral.c/h    # 电流积分
├── CellFadeCalc.c/h    # 电芯衰减计算
└── EnerUser.c/h        # 能量用户接口
```

**核心算法详解**:

1. **安时积分法**
```c
// 充电容量积分
void ChgCapIntTask(u32 curr, u8 cycle) {
    // curr: 充电电流 (10mA 单位)
    // cycle: 积分周期 (1-10ms)
    g_chg_cap += curr * cycle / 36000; // 转换为 Ah
}

// 放电容量积分
void DhgCapIntTask(u32 curr, u8 cycle) {
    g_dhg_cap += curr * cycle / 36000;
}
```

2. **SOC 计算公式**
```
SOC = (剩余容量 / 总容量) × 100%
剩余容量 = 初始容量 + 充电积分 - 放电积分
```

3. **SOH 健康度**
```
SOH = (当前实际容量 / 额定容量) × 100%
SOH < 80% 判定为电池寿命终止
```

**实践任务**:
- [ ] 实现安时积分算法
- [ ] 编写 SOC 查表修正函数 (OCV-SOC 曲线)
- [ ] 实现 SOH 衰减计算逻辑

---

### 第 14-16 天：CAN 通信协议栈

**学习目标**:
- 理解自定义 CAN 通信协议
- 掌握多帧传输与重组机制

**核心模块**:
```
Sources/Funtion/CanLib/
├── CANInterface.c/h   # CAN 接口层
├── CANHandle.c/h      # CAN 报文处理
├── CANTxCycle.c/h     # 周期发送管理
├── CANTabOper.c/h     # 通信表操作
└── CANUser.c/h        # 用户应用层
```

**协议帧类型**:
```c
typedef enum _CAN_FUNCODE_ {
    eCANFUN_VFT = 0,    // 校时帧
    eCANFUN_BRE = 1,    // 广播帧
    eCANFUN_RUP = 2,    // 读取修改
    eCANFUN_AUP = 3,    // 回复修改
    eCANFUN_CUP = 4,    // 确认修改
    eCANFUN_REQ = 5,    // 读取查询
    eCANFUN_ARQ = 6,    // 回复查询
    eCANFUN_CRQ = 7,    // 确认查询
    eCANFUN_UPM = 8,    // 主动上报 (多帧)
    eCANFUN_UPS = 10,   // 主动上报 (首帧)
    eCANFUN_UPC = 11,   // 主动上报 (连续帧)
    eCANFUN_UPP = 12,   // 主动上报 (参数帧)
} e_CANFunCode;
```

**CAN ID 结构**:
```
|31          24|23          16|15          8 |7           0|
|    源地址     |    目的地址   |   主索引    | 功能码/属性 |
```

**实践任务**:
- [ ] 解析 CAN 通信协议文档
- [ ] 实现单帧数据读取功能
- [ ] 实现多帧数据重组功能
- [ ] 编写 CAN 日志分析工具

---

### 第 17-18 天：参数管理系统

**学习目标**:
- 理解参数分层存储架构
- 掌握 FLASH/EEPROM 读写策略

**核心模块**:
```
Sources/Funtion/Parameter/
├── ParaInterface.c/h  # 参数接口层
├── ParaUser.c/h       # 参数用户接口
├── ParaRead.c/h       # 参数读取
├── ParaWrite.c/h      # 参数写入
├── ParaCopy.c/h       # 参数备份
├── ParaDnload.c/h     # 参数下载
├── ParaIntFlash.c/h   # 内部 Flash 操作
├── ParaIntEep.c/h     # 内部 EEPROM 操作
├── ParaExtEep.c/h     # 外部 EEPROM 操作
└── GroupPara.c/h      # 电池组参数
```

**参数分层设计**:
```
┌─────────────────────────────────────┐
│         系统通用参数 (100)          │  ← 所有设备通用
├─────────────────────────────────────┤
│         系统硬件参数 (101)          │  ← 硬件配置相关
├─────────────────────────────────────┤
│         设备通用参数 (102)          │  ← 同类型设备通用
├─────────────────────────────────────┤
│         设备逻辑参数 (103)          │  ← 逻辑配置参数
├─────────────────────────────────────┤
│         设备硬件参数 (104)          │  ← 个体硬件差异
└─────────────────────────────────────┘
```

**实践任务**:
- [ ] 实现参数读写接口封装
- [ ] 编写参数 CRC 校验函数
- [ ] 实现参数备份与恢复机制

---

### 第 19-20 天：诊断与保护系统

**学习目标**:
- 理解 BMS 故障诊断机制
- 掌握多级保护策略

**核心模块**:
```
Sources/Funtion/Diagnosis/
├── DiagInterface.c/h  # 诊断接口层
├── DiagUser.c/h       # 诊断用户接口
├── DiagMaker.c/h      # 故障生成器
├── DiagCheck.c/h      # 故障检查
├── DiagFilter.c/h     # 故障滤波
└── DiagSos.c/h        # SOS 紧急处理
```

**故障分级**:
```
Level 1 - 警告 (Warning): 记录但不动作
Level 2 - 一级保护 (Protection): 限制功率
Level 3 - 二级保护 (Critical): 切断继电器
Level 4 - 紧急 (Emergency): 立即断开高压
```

**保护类型**:
- 过压保护 (OV)
- 欠压保护 (UV)
- 过流保护 (OC)
- 短路保护 (SC)
- 过温保护 (OT)
- 绝缘故障 (ISO)

**实践任务**:
- [ ] 设计故障状态机
- [ ] 实现故障滤波算法 (防抖)
- [ ] 编写保护动作执行逻辑

---

## 📅 第三阶段：应用逻辑 (第 21-27 天)

### 第 21-22 天：电池组状态管理

**学习目标**:
- 理解电池组工作状态机
- 掌握充放电控制逻辑

**核心模块**:
```
Sources/Logics/Tactics/
├── GroupState.c/h     # 电池组状态
├── GroupMode.c/h      # 工作模式
├── GroupProtect.c/h   # 电池组保护
├── GroupTactic.c/h    # 电池组策略
├── GroupDebug.c/h     # 调试接口
└── TacInterface.c/h   # 策略接口
```

**工作状态定义**:
```c
typedef enum {
    e_WORK_STATE_IDLE = 0,      // 空闲
    e_WORK_STATE_CHARGE = 1,    // 充电
    e_WORK_STATE_DISCHARGE = 2, // 放电
    e_WORK_STATE_STANDBY = 3,   // 待机
    e_WORK_STATE_FAULT = 4,     // 故障
    e_WORK_STATE_MAINTAIN = 5,  // 维护
    e_WORK_STATE_SHUTDOWN = 6   // 关机
} e_WorkState;
```

**实践任务**:
- [ ] 绘制完整状态转移图
- [ ] 实现状态判断逻辑
- [ ] 编写模式切换处理函数

---

### 第 23-24 天：均衡控制策略

**学习目标**:
- 理解电池均衡原理
- 掌握被动均衡算法

**核心模块**:
```
Sources/Logics/Balance/
├── BalanTactic.c/h    # 均衡策略
└── (均衡相关接口)
```

**均衡策略**:
1. **充电均衡**: 充电末期对低电压电芯放电
2. **静态均衡**: 静置时对高电压电芯放电
3. **动态均衡**: 运行时根据温差/压差均衡

**均衡条件**:
```c
// 压差均衡启动条件
if (max_cell_volt - min_cell_volt > 50mV) {
    启动均衡;
}

// 温度保护
if (balance_temp > 45°C) {
    暂停均衡;
}
```

**实践任务**:
- [ ] 实现压差判断算法
- [ ] 编写均衡 MOSFET 控制逻辑
- [ ] 设计均衡温度保护机制

---

### 第 25-26 天：事件记录系统

**学习目标**:
- 理解事件记录机制
- 掌握黑匣子数据存储

**核心模块**:
```
Sources/Logics/EventUser/
├── EventUser.c/h      # 事件用户接口
└── RcdDisplayUser.c/h # 记录显示
```

**记录类型**:
- 故障事件记录
- 操作事件记录
- 状态变化记录
- 数据统计记录

**实践任务**:
- [ ] 设计事件记录数据结构
- [ ] 实现循环存储机制
- [ ] 编写事件查询接口

---

### 第 27 天：Modbus 通信 (可选)

**学习目标**:
- 理解 Modbus RTU 协议
- 掌握储能系统通信规范

**核心模块**:
```
Sources/Funtion/ModbusLib/
Sources/Logics/SciUser/
├── ModbusMap.c/h      # Modbus 地址映射
├── HeatModbusMap.c/h  # 加热 Modbus 映射
└── SciParaHandle.c/h  # 串口参数处理
```

---

## 📅 第四阶段：系统集成 (第 28-30 天)

### 第 28-29 天：系统联调

**学习目标**:
- 完成全系统功能验证
- 掌握调试技巧

**调试任务**:
- [ ] 单板上电测试
- [ ] 通信链路验证
- [ ] 保护功能测试
- [ ] 精度校准测试
- [ ] 高低温测试

---

### 第 30 天：独立实现

**最终目标**: 能够独立设计并实现 BMS 系统

**能力检查清单**:
- [ ] 能够阅读并理解任意模块代码
- [ ] 能够独立添加新功能
- [ ] 能够定位并修复 bug
- [ ] 能够进行代码审查
- [ ] 能够编写技术文档

---

## 📖 推荐学习资料

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

## 💡 学习建议

1. **理论与实践结合**: 每天保证 2 小时代码阅读 + 2 小时实践编程
2. **做好笔记**: 使用 Markdown 记录关键知识点
3. **绘制图表**: 流程图、状态图、时序图帮助理解
4. **代码注释**: 在理解的基础上添加中文注释
5. **版本管理**: 使用 Git 管理学习过程中的代码修改
6. **问题记录**: 建立问题清单，逐个击破

---

## 📝 每日学习模板

```markdown
## Day X - [学习主题]

### 今日目标
- [ ] 目标 1
- [ ] 目标 2

### 学习内容
- 阅读的源文件：
- 理解的知识点：

### 实践代码
```c
// 关键代码片段
```

### 遇到问题
- 问题描述：
- 解决方案：

### 明日计划
- 计划内容：
```

---

> **最后提醒**: BMS 是安全关键系统，代码质量关乎人身安全。学习过程中要始终牢记：
> - 代码可靠性第一
> - 保护功能必须冗余设计
> - 充分测试才能上车/上储能柜
> - 遵循车规级/储能行业标准

**祝你学习顺利！30 天后期待你成为 BMS 专家！** 🚀
