# S32K146OFBMU 系统运行流程与调度详细说明

---

## 目录

1. [系统启动完整流程](#1-系统启动完整流程)
    1.1 [硬件启动与时钟初始化](#11-硬件启动与时钟初始化)
    1.2 [RTOS 任务创建与调度器启动](#12-rtos-任务创建与调度器启动)
2. [中断处理机制](#2-中断处理机制)
    2.1 [硬件定时器中断 (FTM)](#21-硬件定时器中断-ftm)
    2.2 [通信外设中断 (CAN/SCI/SPI)](#22-通信外设中断-canscicspi)
    2.3 [中断与任务调度关系图](#23-中断与任务调度关系图)
3. [FreeRTOS 软件定时器任务调度](#3-freertos-软件定时器任务调度)
    3.1 [超高频任务 (0.1ms / 1ms)](#31-超高频任务-01ms--1ms)
    3.2 [高频任务 (2ms - 10ms)](#32-高频任务-2ms---10ms)
    3.3 [中频任务 (50ms - 100ms)](#33-中频任务-50ms---100ms)
    3.4 [低频任务 (200ms - 2000ms)](#34-低频任务-200ms---2000ms)
4. [RTOS 并发任务 (Tasks)](#4-rtos-并发任务-tasks)
    4.1 [采样任务 (ADC/LTC6813)](#41-采样任务-adcltc6813)
    4.2 [通信收发任务](#42-通信收发任务)
5. [核心算法运行逻辑](#5-核心算法运行逻辑)
    5.1 [SOC/SOE/SOH 计算流程](#51-socsoesoh-计算流程)
    5.2 [保护与故障诊断流程](#52-保护与故障诊断流程)
    5.3 [电芯均衡策略流程](#53-电芯均衡策略流程)

---

## 1 系统启动完整流程

系统上电后，经历硬件复位、BSP 初始化、应用层初始化，最后启动 FreeRTOS 调度器进入多任务运行阶段。

### 1.1 硬件启动与时钟初始化

**入口**: `main()`

```text
[Reset] (硬件复位)
   |
   +---> 1. Vector Table / Startup Code (汇编初始化: SP, 数据段, BSS)
   |
   +---> 2. main() (C 入口)
             |
             +--- [PEX_RTOS_INIT()] --> FreeRTOS 内部初始化
             |
             +--- [BSPUserCfgInit()] --> 加载编译宏配置到运行时结构 (gBSPFunCfg, gBSPParaCfg)
             |
             +--- [BSPDriverInit()] --> MCU 外设初始化 (顺序执行)
             |       |-- 1. 看门狗 (WDOG) 初始化
             |       |-- 2. CRG/PLL 时钟配置 (16MHz -> 80MHz)
             |       |-- 3. UART SCI0, SCI1, SCI2 初始化 (19200bps)
             |       |-- 4. FTM1, FTM2, FTM3 定时器初始化
             |       |-- 5. CAN0, CAN1, CAN2 初始化 (500kbps)
             |       |-- 6. 内部 Flash 初始化
             |       |-- 7. 内部 EEPROM 初始化
             |       |-- 8. SPI0, SPI1, SPI2, SPI3 初始化
             |       |-- 9. PWM 输入捕获初始化
             |       |-- 10. LPIT 低功耗定时器初始化
             |       |
             |       +---> DEVDriverInit() --> 外部设备驱动初始化
             |               |-- DEVGPIOInit(): 端口/干接点/继电器驱动初始化
             |               |-- DEVADCInit(): 内部/外部 ADC 通道初始化
             |               |-- Fs6503Init(): 安全 SBC (FS6503) 初始化 (看门狗/电源)
             |               |-- DEVRTCRX8010Init(): 外部 RTC 芯片初始化
             |               |-- EXTEep24M512Init(): 外部 EEPROM (M24512) 初始化
             |               |-- EXTFlashSST26Init(): 外部 Flash (SST26) 初始化
             |
             +--- [BSP_DISABLE_IRQ()] --> 关闭全局中断 (进入临界区)
             |
             +--- [BSPDisIRQUserInit()] --> 关中断下的应用层初始化
             |       |-- 参数管理初始化 (读取 EEPROM/Flash 参数)
             |       |-- 绝缘测量初始化
             |       |-- 诊断模块初始化
             |       |-- 系统信息管理初始化
             |       |-- CAN 协议库初始化 (CanLib)
             |       |-- Modbus 库初始化
             |       |
             |       +---> Device Specific Init:
             |               |-- CellLTC6813Init(): LTC6813 AFE 配置 (休眠唤醒/配置寄存器)
             |               |-- HigLVSampleInit(): LTC2949 高压库仑计初始化
             |
             +--- [BSP_ENABLE_IRQ()] --> 开启全局中断 (结束临界区)
             |
             +--- [BSPEnIRQUserInit()] --> 开中断后的应用层初始化
             |       |-- BootLoaderInit(): 固件升级引擎初始化
             |       |-- CalibrationInit(): 高压/电流标定参数加载
             |       |-- EnergyCalcInit(): 能量/容量/SOH 模块初始化
             |       |-- StatisticsInit(): 统计模块初始化
             |       |-- SocSoeInit(): SOC/SOE 算法初始化 (加载上一次的值)
             |
             +--- [PEX_RTOS_START()] --> 调用 RtosTaskStart()
```

### 1.2 RTOS 任务创建与调度器启动

**入口**: `RtosTaskStart()` (OSUserHOOK.c)

```text
[RtosTaskStart]
   |
   +--- 1. 创建 11 个软件定时器 (AutoReload):
   |       |-- Timer 2ms   (Priority 1)
   |       |-- Timer 5ms   (Priority 2)
   |       |-- Timer 10ms  (Priority 3) ... 依次类推 ...
   |       `-- Timer 2000ms(Priority 11)
   |
   +--- 2. 创建后台常驻任务 (Tasks):
   |       |-- HigInfoSampleTask   (Priority 14): 高压/电流采样 (LTC2949)
   |       |-- CellInfoSampleTask  (Priority 18): 电芯电压/温度采样 (LTC6813)
   |       |-- LowADCSampleTask    (Priority 13): 低压信号/NTC 采样 (内部 ADC)
   |       |-- CAN Send Task       (Priority 17): CAN 发送队列处理
   |       |-- CAN Rcv Task        (Priority 15): CAN 接收队列处理
   |       |-- UART Rcv Task       (Priority 16): 串口接收处理
   |       |-- PileAllTimerTask    (Priority 12): 堆级管理任务 (条件编译)
   |
   +--- 3. 启动调度器 (vTaskStartScheduler)
            ^
            |
      [RTOS Running] <-- 系统开始按时间片轮转调度任务和定时器回调
```

---

## 2 中断处理机制

中断是实时响应硬件事件的关键。本项目中，主要使用 FTM 定时器中断作为系统心跳，通信中断用于数据收发。

### 2.1 硬件定时器中断 (FTM)

FTM 中断是算法调度的基石，在 `BSPISRHook.c` 中定义了 Hook 函数。

#### FTM1 (0.1ms 中断)

*   **用途**: 维持 Modbus 协议时序。
*   **执行逻辑**:
    ```c
    if (BSPGetOSMillSecTimeAPI() < 1) return; // 等待系统初始化完成
    ModbusRcvTickTask(); // 更新 Modbus 接收超时计数器 (t1.5, t3.5)
    ```

#### FTM3 (1ms 中断)

*   **用途**: 系统核心高频运算与调度中心。
*   **执行逻辑**:
    ```c
    if (BSPGetOSMillSecTimeAPI() < 1) return;
    
    // 1. 电流积分 (最重要): 累加 1ms 内的电流 Ah 值
    ISRCurrCapAddTask(); 
    
    // 2. 系统时钟: 毫秒/秒计数累加
    SysClockCountTask(); 
    
    // 3. Modbus 客户端状态机驱动
    MbClientTimerTask(); 
    
    // 4. CAN 协议库调度 (处理多帧、超时等)
    CANLibTxRcvHandleTask_API(); 
    
    // 5. 从属 MCU (KEA128) 心跳 (每 100ms 触发一次)
    if (sCount >= 100) {
        HeartToSlaveMcuTask(); // 翻转 EWM 引脚或同步 SPI 地址
        sCount = 0;
    } else {
        sCount++;
    }
    ```

#### PWM ISR (FTM2)

*   **用途**: 输入捕获充电枪 CP 信号 (目前代码中该部分被注释，预留)。

### 2.2 通信外设中断 (CAN/SCI/SPI)

*   **CAN ISR**: 接收/发送 FIFO 满或空时触发 -> `CAN_ISR_Rcv_Hook` / `CAN_ISR_Send_Hook`。实际数据搬运在中断中完成，数据处理由 `CAN Rcv Task` 异步执行。
*   **SCI (UART) ISR**: 串口字节接收/发送完成 -> `SCI_ISR_Rcv_Hook`。
    *   *特殊处理*: 在 485 半双工模式下，发送完成中断中需切换回接收模式 `DEVSCIEnableRcvPin` (当前代码已注释)。
*   **SPI ISR**: 完成 SPI 传输 -> `SPI_ISR_Hook`。

### 2.3 中断与任务调度关系图

```text
       [Hardware Interrupts]                     [FreeRTOS Tasks / Timers]
    __________________________________|_________________________________________
   |                                                                          |
   |  FTM3 (1ms)                                                              |
   |  |-- ISRCurrCapAddTask (Ah 累加)                                         |
   |  |-- SysClockCountTask                                                   |
   |  `-- CANLibTxRcvHandle (CAN 协议栈)                                      |
   |                                         +-------> [HigInfoSampleTask]     |
   |  FTM1 (0.1ms)                             |       |-- Read LTC2949        |
   |  |-- ModbusRcvTick (Modbus 超时)            |       |-- Calc Current      |
   |                                            |       |-- Calc Voltage      |
   |  CAN ISR                                  |       |                       |
   |  |-- Put to Queue ----------------------->|       `-- Block on Delay      |
   |                                           |                               |
   |  SCI (UART) ISR                            |       [CellInfoSampleTask]    |
   |  |-- Byte Rcv -> Parse Modbus Frame ------>|-------|-- Read LTC6813 Cells  |
   |                                            |       |-- Read Temp NTC       |
   |  Timer Callbacks (RTOS)                    |       `-- Block on Delay      |
   |  |-- Task10ms:                             |                               |
   |  |   |-- Polling (限流, 诊断, SOC) --------|-------[CAN Rcv Task]          |
   |  |   `-- ...                               |       |-- Pop CAN Frame       |
   |  |-- Task100ms:                            |       `-- Route to Handler    |
   |  |   |-- State/Protect/Fault --------------|                               |
   |  |                                         |-------[UART Rcv Task]         |
   |  `-- ...                                   |       |-- Pop UART Byte       |
   |                                            |       `-- Build Modbus Cmd    |
   |____________________________________________|_______________________________|
```

---

## 3 FreeRTOS 软件定时器任务调度

除了中断中的实时处理，大量的算法逻辑由 FreeRTOS 软件定时器 (Software Timers) 周期触发。

### 3.1 超高频任务 (0.1ms / 1ms)

*   **0.1ms**: `ModbusRcvTickTask` (在 FTM1 中断中调用) - 维持 Modbus 帧计时。
*   **1ms**: `ISRCurrCapAddTask` (FTM3 中断中调用) - 电流积分。
*   **1ms**: `MbClientTimerTask` (FTM3 中断中调用) - Modbus 主机状态推进。
*   **1ms**: `CANLibTxRcvHandleTask_API` (FTM3 中断中调用) - CAN 协议状态机。

### 3.2 高频任务 (2ms - 10ms)

#### Task2ms (每 2ms)
*   `MbTrigSendTimerTask`: 触发 Modbus 从机延迟发送或主机请求重传。

#### Task5ms (每 5ms)
*   `CANLibHandleTask_API`: CAN 协议库上层业务处理（组包、解析应用层协议）。
*   `FS65DiagnosticTask`: FS6503 SBC 芯片状态读取与看门狗喂狗。
*   `CanSendUpCatlMsgCycleTask`: 发送 CATL 协议上行报文 (条件编译)。

#### Task10ms (每 10ms) + 10 状态轮询
此函数包含一个 10 状态的轮询器 (`static u8 sTime`)，将大量 100ms 任务分散到 10ms 中执行，相当于每 10ms 执行一部分，100ms 完成一轮。
*   **每次必执行**: GPIO 多路复用管理、端口数字输入滤波、GTBmm/CATL 协议发送。
*   **轮询 (sTime 0~9)**:
    *   `Case 0 (10ms)`: **电流/电压限流计算** (`CurrLimCalcTask`)
    *   `Case 1 (20ms)`: **电压/温度/电流 特征检查** (`CheckVoltCharaTask`, `CheckAvgCurrTask`)
    *   `Case 2 (30ms)`: **故障原始状态生成 - 电压/温度/电流** (`DiagMaker`)
    *   `Case 3 (40ms)`: **故障原始状态生成 - 端口/信息**
    *   `Case 4 (50ms)`: **故障原始状态生成 - 状态** + **故障显示更新**
    *   `Case 5 (60ms)`: **容量/SOC/SOE 计算** (`GroupCapSocCalcTask`, `GroupEnerSoeCalcTask`)
    *   `Case 6 (70ms)`: **SOH 计算** + **能量显示计算**
    *   `Case 7 (80ms)`: **PWM 捕获** + **电芯均衡策略** (`GroupVoltBalanTacticTask`)
    *   `Case 8 (90ms)`: **绝缘测量** (`InsuAdhesMeasurTask`) + **保护状态判断** (`GroupProStateTask`) + **事件记录**
    *   `Case 9 (100ms)`: **接触器状态变化检查** + **快充消息发送** + **参数备份**

### 3.3 中频任务 (50ms - 100ms)

#### Task50ms (每 50ms)
*   `DiagErrFilterTask`: **全局故障滤波** (从 L2 到 Finish 所有错误 ID 的状态机)。
*   `SocSoeLibAllTask`: **SOC/SOE 核心库算法** (包含显示平滑、同步校正)。
*   `GroupSendUpMsg1sBy50ms_API`: **上位机广播** (将 1s 广播任务切片到以 50ms 间隔发送不同帧)。
*   `GroupCheckRcvBsuMsgAPI`: **从控消息检查** (BSU)。

#### Task100ms (每 100ms)
*   `GroupWorkStateTask`: **工作状态机** (Idle/Start/Run/Stop/Err)。
*   `GroupChgDhgStateTask`: **充放电状态判断** (电流方向判定)。
*   `DiagGroupSosTask`: **SOS 安全等级评估** (计算系统安全指数)。
*   `GroupProtectTask`: **保护动作执行** (根据状态触发降流/断开接触器)。
*   `RcdFaultWaveHandleTask`: **故障波形记录处理** (写入 Flash 缓冲区)。

### 3.4 低频任务 (200ms - 2000ms)

#### Task200ms (每 200ms) + 5 状态轮询
*   `Case 0 (200ms)`: **热管理** (`TacGHeatManagTacticTask`, 风机/加热 PWM)
*   `Case 1 (400ms)`: **电压限值计算** (`VoltLimCalcTask`)
*   `Case 2 (600ms)`: **工作模式检查** (`GroupWorkModeTask`)
*   `Case 3 (800ms)`: **通信超时检查** (`DiagCommuCheckTask`)
*   `Case 4 (1000ms)`: **高压/电流标定处理** (`HigVoltSampCorrProcess`, `OutCurrSampCorrProcess`)

#### Task300ms (每 300ms)
*   `GroupCycleCapStatisTask`: **循环容量统计**。
*   `LoadAppFlashErasHandleAPI`: **Bootloader Flash 擦除** (外部 Flash 升级)。
*   `CanSendRemLoadMsgAPI`: **远程升级进度上报**。
*   `(每 600ms)`: `RcdStoreHandleTask` - **历史记录落盘**。

#### Task400ms (每 400ms)
*   `GroupHisCapStatisTask`: **历史容量统计** (累计充放电量)。
*   `GroupHisEnerStatisTask`: **历史能量统计**。
*   `CommuToSlaveMcuTask`: **从属 MCU (KEA128) SPI 通信** (同步地址/参数)。

#### Task500ms (每 500ms)
*   `DiagPerDevCheckTask`: **外部设备检查** (EEPROM/RTC 连通性)。
*   `GroupFstChgTacticsTask`: **快充策略主循环** (GB/T 27930 状态机)。
*   `OSDiagTimerAddTask`: **OS 自诊断计时**。
*   `TacAuxDevManagTask`: **辅助外设管理** (IMEU2/卷帘门/空调)。

#### Task1000ms (每 1s)
*   `TacGPeriManagTacticTask`: **外围设备策略**。
*   `GroupSysIoUpDnCtrlTask`: **按键/IO 交互** (开关机/急停逻辑)。
*   `GroupChgTimesStatisTask` / `GroupAllowTimeStatisTask`: **充放电次数/时间统计**。
*   `GroupDodStatisRcdTask`: **DOD 放电深度统计**。
*   `EXTFlashCheckTask`: **外部 Flash 健康检查**。

#### Task2000ms (每 2s)
*   `GroupSysPwrOffProTask`: **掉电保护** (欠压关机/通信超时关机)。
*   `SysClockRecordTask`: **系统时钟持久化** (记录时间到 EEPROM)。
*   `SysDispProjInfoTask`: **项目信息更新**。
*   `DEVEXTRTCCheckTask` (每 4s): 外部 RTC 检查。
*   `DEVEXTEEPCheckTask` (每 4s): 外部 EEPROM 检查。

---

## 4 RTOS 并发任务 (Tasks)

以下任务由 FreeRTOS 调度器管理，与定时器回调并发执行。

### 4.1 采样任务 (ADC/LTC6813)

这些任务通常包含长时间的 SPI 轮询等待，因此在独立任务中运行以免阻塞定时器回调。

#### **HigInfoSampleTask** (Priority 14)
*   **职责**: 采集高压信号。
*   **流程**:
    1.  通过 SPI 读取 LTC2949 寄存器。
    2.  解析总压、预充电压、母线电压、分流器电流、功率。
    3.  存入全局变量供算法层读取。
    4.  延时循环。

#### **CellInfoSampleTask** (Priority 18)
*   **职责**: 采集所有电芯电压和温度。
*   **流程**:
    1.  **发送转换命令**: 向 LTC6813 发送启动 ADC 转换命令。
    2.  **等待转换**: 延时等待转换完成 (约几 ms)。
    3.  **读取数据**: 通过 SPI 菊花链读取所有电芯电压寄存器 (V-Cell A/B...)。
    4.  **读取辅助通道**: 读取 GPIO/Temp 寄存器。
    5.  **数据处理**: 滤波、异常值剔除、存入全局数组 `sCellVol[]`。

#### **LowADCSampleTask** (Priority 13)
*   **职责**: 低压信号采集。
*   **流程**:
    1.  启动内部 ADC0/ADC1 转换。
    2.  读取霍尔电压、环境温度 NTC、电源温度 NTC。
    3.  计算实际物理值并更新。

### 4.2 通信收发任务

#### **CAN Rcv Task** (Priority 15)
*   阻塞等待 CAN 接收队列非空。
*   出队一帧 CAN 报文。
*   解析 29 位 CAN ID，分发到对应的处理函数 (GroupMsg, SlaveMsg, AuxDevMsg)。

#### **CAN Send Task** (Priority 17)
*   阻塞等待 CAN 发送队列非空。
*   出队报文，调用底层 `BSPCANSendMsg` 发送。

---

## 5 核心算法运行逻辑

### 5.1 SOC/SOE/SOH 计算流程

```text
[FTM3 1ms ISR]
   |-- ISRCurrCapAddTask()
         + 读取当前电流 I
         + 积分：Delta_Ah += I * 1ms / 3600
         + 累加到全局变量

[Task10ms Case 5]
   |-- GroupCapSocCalcTask()
         + 读取 Delta_Ah
         + 应用温度/C 率 补偿系数
         + SOC = (Current_Cap / Total_Cap) * 100%

[Task50ms]
   |-- SocSoeLibAllTask()
         |-- 校正逻辑 (SOC_Sync_Task)
         |    + 电芯充满校正 (V_Max > Threshold -> SOC=99.3%)
         |    + 电芯放空校正 (V_Min < Threshold -> SOC=0.13%)
         |    + 电池组间 SOC 同步 (Pile Sync)
         |-- 显示平滑 (SOC_Display_Task)
              + 限制变化速率 (如 max 5% per sec)
```

### 5.2 保护与故障诊断流程

```text
[Task10ms Cases 2,3,4] (每 30ms 一轮)
   |-- DiagMaker (故障生成器)
         + 比较采样值与阈值 (过压/欠压/过温/过流)
         + 更新全局错误标志的原始状态 (Raw State)

[Task50ms]
   |-- DiagErrFilterTask (故障滤波器)
         + 遍历所有 96+ 个错误 ID
         + 状态机:
              正常 --[延时 AtoB]--> 故障 (Fault)
              故障 --[延时 BtoA]--> 正常 (Recovery)
         + 生成最终错误状态 (Filtered State)

[Task100ms]
   |-- DiagGroupSosTask (安全等级评估)
         + 统计所有 Filtered Fault 的加权分数
         + 计算 SOS (0~4): 0=安全, 4=系统严重故障
     
   |-- GroupProtectTask (保护执行)
         + IF SOS > Threshold:
              + Level 1 (预警): 记录日志，限速
              + Level 2 (限流): 降低充/放电电流限制
              + Level 3 (停机): 发送 Stop Cmd -> 断开接触器
```

### 5.3 电芯均衡策略流程

```text
[Task10ms Case 7] (每 80ms 执行一次均衡检查)
   |-- GroupVoltBalanTacticTask()
         + 1. 检查前提: 非调试模式, 均衡开关开启, 通信正常
         + 2. 积分累积:
              + 如果电芯电压偏离均值超过阈值, 积分器 +1
              + 如果偏离不够, 积分器 -1
              + 达到设定时间 (如 300s) 进入下一状态
         + 3. 判定均衡方案:
              + 条件 1: 最低电芯 > 3.1V (LFP) -> 均衡高于最低的
              + 条件 2: 最高电芯 > 平均 + 阈值 -> 均衡高于平均的
              + 温度限制: >60°C 限制均衡数量; >80°C 停止均衡
         + 4. 发送均衡命令:
              + 通过 CAN (SlaveMsg) 发送均衡控制帧给对应 BSU
```

---

## 文档说明

1. **文档生成日期**: 2026-04-20
2. **适用固件版本**: S32K146OFBMU (BMU-146)
3. **文档版本**: V1.0