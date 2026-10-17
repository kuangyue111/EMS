# S32K146OFBMU 电池管理系统 — 运行流程、调度与架构深度分析

---

## 文档控制信息

| 项目 | 内容 |
| :--- | :--- |
| **项目名称** | BMU-146 (S32K146OFBMU) |
| **目标 MCU** | NXP S32K146 (ARM Cortex-M4F, 1MB Flash, 128KB RAM) |
| **主频** | 80 MHz (16MHz OSC -> PLL) |
| **操作系统** | FreeRTOS (v8.x/v9.x based via Processor Expert) |
| **文档版本** | V3.0 (深度分析版) |
| **生成日期** | 2026-04-20 |
| **说明** | 本文档包含系统启动流程、中断机制、RTOS 调度、核心算法数据流及架构设计深度分析。 |

---

## 目录 (Table of Contents)

1.  **[第一章] 系统硬件与软件架构总览**
    *   1.1 硬件平台深度解析 (MCU 与外设拓扑)
    *   1.2 关键外围芯片功能与通信拓扑
    *   1.3 软件分层架构设计 (BSP -> HAL -> APP)
    *   1.4 源码目录功能映射表
2.  **[第二章] 系统启动完整流程 (代码级细节)**
    *   2.1 阶段一：硬件复位与 Startup Code
    *   2.2 阶段二：BSP 配置与加载 (`BSPUserCfgInit`)
    *   2.3 阶段三：MCU 外设驱动初始化 (`BSPDriverInit`)
        *   2.3.1 时钟系统与看门狗
        *   2.3.2 通信外设 (UART/CAN/SPI) 配置
        *   2.3.3 存储与外设 (EEPROM/Flash/PWM)
    *   2.4 阶段四：外部专用芯片驱动 (`DEVDriverInit`)
        *   2.4.1 SBC (FS6503) 初始化
        *   2.4.2 外部 Flash/EEPROM/RTC 探测
    *   2.5 阶段五：临界区初始化 (关中断/开中断)
        *   2.5.1 `BSPDisIRQUserInit` (关中断下)
        *   2.5.2 `BSPEnIRQUserInit` (开中断后)
    *   2.6 阶段六：RTOS 环境创建 (`RtosTaskStart`)
3.  **[第三章] 中断处理机制与数据流 (ISR)**
    *   3.1 FTM1 定时器中断 (0.1ms) — Modbus 时序驱动
    *   3.2 FTM3 定时器中断 (1ms) — 系统心跳与积分
        *   3.2.1 电流积分 `ISRCurrCapAddTask` 细节
        *   3.2.2 任务调度分发逻辑
    *   3.3 CAN 收发与错误中断处理机制
    *   3.4 SCI (UART) 字节收发与 Modbus 组帧
    *   3.5 SPI 传输完成中断
    *   3.6 中断与 RTOS 任务的交互模型 (ISR to Task)
4.  **[第四章] FreeRTOS 软件定时器任务调度表**
    *   4.1 调度架构设计原则 (Timer vs Task)
    *   4.2 高频周期任务 (2ms ~ 10ms)
        *   4.2.1 `Task2ms`：Modbus 触发
        *   4.2.2 `Task5ms`：SBC 诊断与 CAN 处理
        *   4.2.3 `Task10ms` + 10 步轮询器：全系统核心循环
    *   4.3 中频周期任务 (50ms ~ 100ms)
        *   4.3.1 `Task50ms`：SOC 算法与故障滤波
        *   4.3.2 `Task100ms`：状态机与保护执行
    *   4.4 低频周期任务 (200ms ~ 2000ms)
        *   4.4.1 `Task200ms`：热管理与标定轮询
        *   4.4.2 `Task500ms`：快充协议与辅助管理
        *   4.4.3 `Task1000ms` & `Task2000ms`：统计与系统健康
5.  **[第五章] RTOS 常驻并发任务 (Tasks)**
    *   5.1 `CellInfoSampleTask` (Pri 18)：LTC6813 轮询
    *   5.2 `HigInfoSampleTask` (Pri 14)：LTC2949 轮询
    *   5.3 通信队列任务 (Send/Rcv Task)
6.  **[第六章] 核心算法完整运行逻辑 (数据流与状态机)**
    *   6.1 SOC/SOE 算法：从安时积分到端点校正
    *   6.2 故障诊断流水线：生成 -> 滤波 -> SOS -> 保护
    *   6.3 电芯均衡策略：积分决策与温度限流
    *   6.4 绝缘检测与粘连检测状态机
    *   6.5 限流计算：多维度取最小值
7.  **[第七章] 架构设计深度分析**
    *   7.1 设计哲学：为什么使用 "中断+Timer+Task" 三级调度？
    *   7.2 架构优势分析
    *   7.3 当前设计缺陷与潜在风险
    *   7.4 改进路线与重构建议

---

# 第一章：系统硬件与软件架构总览

## 1.1 硬件平台深度解析

### 核心微控制器 (MCU)
系统基于 NXP 的 **S32K146** 微控制器，属于车规级 Cortex-M4F 系列，专为高可靠性 BMS 应用设计。

| 特性 | 参数值 | 说明 |
| :--- | :--- | :--- |
| **内核** | ARM Cortex-M4F | 带单精度 FPU，适合浮点运算 (如 SOC/SOE 计算) |
| **主频** | 80 MHz | 通过 PLL 锁相环将外部 16MHz 晶振倍频得到 |
| **Flash** | 1 MB (1024 KB) | 存储代码 (`0x0000`-`0x060000`) 及内部模拟 EEPROM/Flash |
| **RAM** | 128 KB | 运行时数据，包括全局变量、堆栈 (Heap/Stack) |
| **FlexCAN** | x3 通道 | 3 路 CAN 控制器，硬件 29 位 ID 过滤，带缓冲区 |
| **LPUART (SCI)** | x4 通道 | 4 路串行通信，支持中断收发，用于 RS485 Modbus |
| **LPSPI** | x4 通道 | 4 路 SPI 主机，用于连接外部传感器、存储器等 |
| **ADC** | 2 模块 (16 通道/模块) | 12 位精度，用于采集霍尔、温度、电源等模拟信号 |
| **FTM** | x4 定时器 | 用于 PWM 生成、输入捕获及系统时基 (FTM0-3) |

### 关键外围芯片拓扑

系统采用分布式架构，主控 MCU 通过 SPI 总线与多个专用芯片通信：

1.  **FS6503 (SBC - System Basis Chip)**
    *   **接口**: SPI0
    *   **功能**: 电源管理 (VDD/VDD_MCU_5V/VCORE)，安全看门狗 (WD)，复位监控。
    *   **重要性**: 保证系统的硬件基础安全性，若看门狗不刷新则硬件复位。

2.  **LTC6813 (电芯电压/温度监控)**
    *   **接口**: SPI2 (菊花链 daisy-chain)
    *   **功能**: 测量多达 12 串电芯电压 (精度 <1.2mV) 及 GPIO 温度。
    *   **扩展**: 支持多片串联 (最高 20 片)，通过 isoSPI 隔离通信。

3.  **LTC2949 (高压库仑计)**
    *   **接口**: SPI1
    *   **功能**: 高精度电流/电压/功率积分。提供独立的 Shunt 分流器和霍尔校准数据。
    *   **特性**: 内部 48 位累加器，直接计算 Charge/Energy，减轻 MCU 负担。

4.  **SST26VF064 (外部 Flash)**
    *   **接口**: SPI3
    *   **功能**: 8MB 存储空间。
    *   **用途**: 故障记录、运行数据记录 (Data Logger)、波形记录 (Scope)、OTA 升级包缓存。

5.  **M24512 (外部 EEPROM)**
    *   **接口**: I2C (模拟)
    *   **功能**: 64KB 参数存储，用于保存配置、标定系数、历史统计等。

---

## 1.2 软件分层架构

代码库采用严格的分层设计，确保硬件无关性与代码复用率。

```text
App Layer  (业务逻辑)
   |
   +--- Funtion/      : 核心算法库 (SOC/SOE, 诊断, 统计, 通信栈)
   +--- Logics/       : 业务策略 (状态机, 均衡, 保护, 快充)
   +--- PileSys/      : 堆级管理 (多簇并联协调, 可选)
   +--- Addition/     : 附加功能 (标定, 升级, 权限)
   |
   +--- OSUser/       : RTOS 适配层 (任务创建, 定时器回调注册)
   |
HW Abstraction (BSP User)
   |
   +--- BSPUser/      : 统一对外 API (BSPCANInit, DEVADCGetSampAD 等)
   |
HW Driver  (MCU 外设)
   |
   +--- BSPSys/Driver/: S32K146 底层驱动 (UART, CAN, SPI, FTM)
   +--- BSPSys/Util/  : 通用工具 (环形队列, 数据类型)
   +--- BSPSys/BSPCfg/: 编译配置 (BSPConfig.h)
   |
Device   (芯片驱动)
   |
   +--- BSPSys/Device/: 芯片专用驱动 (FS6503, LTC6813, LTC2949...)
```

这种设计的核心在于：**业务层不直接使用 S32K146 的外设驱动，而是调用 BSPUser 接口。** 这样如果更换 MCU 或外设引脚，只需修改 Driver 层和 BSPUser 映射关系，上层业务无需改动。

---

# 第二章：系统启动完整流程 (代码级细节)

系统上电后，经历由汇编启动代码 -> C main 入口 -> 硬件初始化 -> 软件初始化 -> OS 启动 的严密过程。

## 2.1 阶段一：硬件复位与 Startup Code

*   **动作**: 硬件复位向量指向 `Reset_Handler` (汇编)。
*   **执行**:
    1.  设置主堆栈指针 (MSP)。
    2.  初始化 `.data` 段 (已初始化全局变量) 从 Flash 复制到 RAM。
    3.  清零 `.bss` 段 (未初始化全局变量)。
    4.  调用 `SystemInit()` 配置时钟源 (16MHz Ext OSC)。
    5.  最终调用 `main()`。

## 2.2 阶段二：BSP 配置加载

**入口**: `main()` -> `BSPUserCfgInit()`
**文件**: `BSPSys/BSPCfg/BSPConfig.c`

```c
void BSPUserCfgInit(void) {
    // 1. 将 BSPConfig.h 中的编译宏配置映射到运行时结构体
    gBSPFunCfg.Bit.FTM0 = FTM0_FUN_EN;
    gBSPFunCfg.Bit.FTM1 = FTM1_FUN_EN; // 0.1ms 定时器使能
    gBSPFunCfg.Bit.FTM3 = FTM3_FUN_EN; // 1ms 定时器使能
    // ... 其他驱动开关 ...

    // 2. 初始化参数结构体
    gBSPParaCfg.extOsc = 16000000;   // 16MHz
    gBSPParaCfg.can0Rate = 500000;   // 500kbps
    gBSPParaCfg.sci0Baudrate = 19200;// 19.2kbps
}
```
**目的**: 允许代码在运行时根据配置动态决定初始化哪些外设，便于不同项目复用同一套代码。

## 2.3 阶段三：MCU 外设驱动初始化

**入口**: `main()` -> `BSPDriverInit()`
**执行顺序** (严格按此顺序，避免依赖错误):

1.  **看门狗 (WDOG)**: 初始化并首次刷新，防止初始化时间过长导致复位。
2.  **时钟 (CRG/MCG)**: PLL 锁频到 80MHz。
3.  **UART**: SCI0/SCI1/SCI2 初始化，配置波特率、中断使能。
4.  **定时器**: FTM0~FTM3 配置，分频系数设定，中断使能。
5.  **CAN**: CAN0~CAN2 初始化，配置邮箱掩码，波特率。
6.  **SPI**: SPI0~SPI3 初始化。
7.  **其他**: I2C(模拟), ADC, PWM, LPIT...

## 2.4 阶段四：外部专用芯片初始化 (DEVDriverInit)

**文件**: `BSPSys/Device/`

此函数在 MCU 外设就绪后被调用，负责探测和初始化外部设备：
1.  **FS6503**: SPI0 通信握手，配置安全看门狗时序，读取状态寄存器确认芯片在线。
2.  **RTC (RX8010)**: I2C 读取时间，检查时钟是否跑飞。
3.  **EEPROM (M24512)**: I2C 写入/读取测试，确认存储正常。
4.  **Flash (SST26VF064)**: SPI3 读取 JEDEC ID (0xBF2643)，确认 Flash 就绪。

## 2.5 阶段五：临界区初始化 (关键!)

### 2.5.1 `BSPDisIRQUserInit()` (关中断)

**代码**: `INT_SYS_DisableIRQGlobal();`
**目的**: 在初始化数据结构和传感器时，防止被中断打断导致状态不一致。

**执行内容**:
*   **参数加载**: 从 EEPROM/Flash 读取配置参数 (电压限值、温度限值、标定系数等)，校验 CRC 合法性。
*   **通信库初始化**: 分配 CAN/UART 的发送/接收缓冲区内存。
*   **传感器初态**: 读取 LTC6813/LTC2949 的初始配置寄存器等。
*   **诊断/保护初始化**: 清零错误标志、状态机复位。

### 2.5.2 `BSPEnIRQUserInit()` (开中断)

**代码**: `INT_SYS_EnableIRQGlobal();`
**后果**: 此时 FTM1 (0.1ms) 和 FTM3 (1ms) 开始触发中断！

**执行内容**:
*   **Bootloader**: 检查是否需要进入升级模式。
*   **标定加载**: 读取存储在 EEPROM 中的高压/电流标定参数。
*   **算法启动**: SOC/SOE 积分器清零或加载上次值。

## 2.6 阶段六：RTOS 调度器启动

**入口**: `main()` -> `PEX_RTOS_START()` -> `RtosTaskStart()`
**文件**: `Sources/OSUser/OSUserHOOK.c`

```c
void RtosTaskStart(void) {
    // 1. 创建所有软件定时器 (AutoReload)
    Timer2ms_Handle = xTimerCreate(..., Task2ms, ...);
    Timer10ms_Handle = xTimerCreate(..., Task10ms, ...);
    ...

    // 2. 创建常驻任务 (Tasks)
    xTaskCreate(HigInfoSampleTask, "HV_SAM");
    xTaskCreate(CellInfoSampleTask, "CELL_SAM");
    xTaskCreate(BSPCANRcvTask, "CAN_RCV");
    ...

    // 3. 启动调度器 (不再返回)
    vTaskStartScheduler();
    for(;;);
}
```

至此，系统进入多任务并发运行状态。

---

# 第三章：中断处理机制与数据流 (ISR)

系统主要依赖 **FTM 硬件定时器中断** 作为时基。通信中断负责数据搬运。

## 3.1 FTM1 定时器中断 (0.1ms)

**频率**: 10 kHz
**代码回调**: `Time_ISR_Hook(eFTM1)`

**任务**: **Modbus 帧时序维持**
Modbus RTU 协议依赖字符间的时间间隔 (3.5 字符静默时间) 来判断一帧数据的开始和结束。
```c
static u16 sModbusTimer100us = 0;

void Time_ISR_Hook(e_FTMNum Channel) {
    if (Channel == eFTM1) {
        // 更新 Modbus 字节间计时器
        ModbusRcvTickTask();
        // 内部逻辑：每 0.1ms 增加一个计数器，用于判断 t3.5 (约 2ms @ 19200bps)
    }
}
```

## 3.2 FTM3 定时器中断 (1ms) — 系统心跳

**频率**: 1 kHz
**代码回调**: `Time_ISR_Hook(eFTM3)`

这是系统最重要的实时循环之一，包含 4 个关键任务：

```c
void Time_ISR_Hook(e_FTMNum Channel) {
    if (Channel == eFTM3) {
        // 1. 电流积分 (Ah Counting) - ★ 必须在 ISR 保证精度
        ISRCurrCapAddTask(); 
        /* 逻辑: 获取瞬时电流 I -> 累加 Delta_Ah = I * (1ms/3600000s) */

        // 2. 系统时钟
        SysClockCountTask();

        // 3. Modbus 客户端/主站 状态驱动
        MbClientTimerTask();

        // 4. CAN 协议栈处理
        CANLibTxRcvHandleTask_API();
        
        // 5. 从属 MCU (KEA128) 心跳检测 (每 100ms 触发一次)
        static u8 sCount = 0;
        if (++sCount >= 100) {
            HeartToSlaveMcuTask();
            sCount = 0;
        }
    }
}
```

**注意**: 这里的代码在 ISR 上下文中运行，**不能使用阻塞调用**（如 `vTaskDelay`）。必须执行极快。如果 `CANLib...` 逻辑过重，可能会导致 1ms 周期抖动。

## 3.3 CAN 收发与错误中断处理

*   **接收 (Rx)**:
    *   硬件 FIFO 收到帧 -> 触发 CAN ISR。
    *   BSP Driver 将帧拷贝到 **内存环形缓冲区 (Ring Buffer)**。
    *   释放信号量 (Semaphore)，唤醒 `BSPCANRcvTask`。
*   **发送 (Tx)**:
    *   BSP Driver 检查发送队列。
    *   若邮箱空闲，将帧写入邮箱寄存器，启动发送。
    *   发送完毕后触发 Tx 完成中断（通常用于确认或发送下一帧）。

## 3.4 SCI (UART) 收发中断

*   **接收**:
    *   每收到 1 个字节 -> 触发 SCI Rx ISR。
    *   将字节写入 Modbus 接收缓冲区。
    *   FTM1 ISR 中的 `ModbusRcvTickTask` 负责根据此流判断帧结束 (t3.5 超时)。
*   **发送**:
    *   通常采用轮询或中断发送。若使用中断，每发完 1 字节触发。
    *   RS485 模式下，发送完成后需切换收发控制引脚 (GPIO) 到接收状态 (代码中 `SCI_ISR_Send_Hook` 注释提及)。

---

# 第四章：FreeRTOS 软件定时器任务调度表

业务逻辑主要通过 FreeRTOS 的 **Software Timers** 周期性触发。所有 Timer 回调函数都在 **RTOS Timer Service Task** (通常优先级较高) 中执行。

## 4.1 高频周期任务

### 4.1.1 `Task2ms` (每 2ms)

*   **任务**: `MbTrigSendTimerTask()`: 
    *   Modbus 通信协议栈的"发送触发器"。检查是否有待发数据，如有则启动串口发送。
    *   用于维持 Modbus 通信的实时性。

### 4.1.2 `Task5ms` (每 5ms)

*   **任务**: 
    *   `CANLibHandleTask_API()`: CAN 应用层协议处理（解析多帧、组帧、超时重传检测）。
    *   `FS65DiagnosticTask()`: 读取 FS6503 SBC 状态寄存器，喂软看门狗。
    *   `CanSendUpCatlMsgCycleTask()`: 发送 CATL 专有协议的上报报文。

### 4.1.3 `Task10ms` (每 10ms) — 核心调度器

为了平衡 100ms 任务的执行时间，本系统实现了一个 **10 步轮询器 (Round-Robin Wheel)**。
每 10ms 调用一次，每 100ms 完整执行一轮所有子任务。

```c
static u8 sTime = 0; // 状态寄存器 0-9

void Task10ms(void) {
    // 公共任务: 每次执行
    GPIO_Management_Tasks(); 
    
    // 轮询调度
    switch(sTime) {
        case 0: // [10ms, 110ms ...] 限流计算
            CalcChargeDischargeCurrentLim(); 
            CalcCVSoftLimiting();
            break;
        case 1: // [20ms, 120ms ...] 特征检查
            CheckCellVoltageBalance(); // 压差检查
            CheckCellTempBalance();    // 温差检查
            CalcCurrentMovingAvg();    // 滑动平均
            break;
        case 2: // [30ms, 130ms ...] 故障生成 #1 (电压/温度/电流)
            CheckVoltageThresholds();
            CheckTempThresholds();
            break;
        case 3: // [40ms, 140ms ...] 故障生成 #2 (IO/信息)
            CheckIOState();
            break;
        case 4: // [50ms, 150ms ...] 故障生成 #3 + 诊断显示
            CheckSystemState();
            UpdateDiagDisplay();
            break;
        case 5: // [60ms, 160ms ...] SOC/SOE 计算 ★
            SOC_Calculation();   // 积分 + 补偿
            SOE_Calculation();
            break;
        case 6: // [70ms, 170ms ...] SOH 计算
            SOH_Estimation();
            break;
        case 7: // [80ms, 180ms ...] 均衡策略 & PWM
            PassiveBalancingTask(); 
            CheckPwmCapture();
            break;
        case 8: // [90ms, 190ms ...] 绝缘检测 & 保护状态
            InsulationCheckTask();
            UpdateProtectionState();
            EventRecordTask();
            break;
        case 9: // [100ms, 200ms ...] 接触器/外部交互
            CheckContactorState(); 
            CHG_SendMsg(); // 快充报文
            ParaStoreCopy(); // 参数备份
            break;
    }
    
    sTime = (sTime + 1) % 10;
}
```
**设计意图**: 将大量 100ms 周期任务打散，避免在某一时刻 CPU 占用率飙升 (Burst)，使得系统运行更平滑，且每个任务的实际执行周期约为 100ms+10ms(抖动)。

## 4.2 中频周期任务

### 4.2.1 `Task50ms` (每 50ms)

*   **任务**:
    *   `DiagErrFilterTask(...)`: **全局故障滤波器**。遍历所有 96+ 错误 ID，根据设定的 `DelayOn` 和 `DelayOff` 进行消抖。这是区分“瞬时干扰”和“真实故障”的关键。
    *   `SocSoeLibAllTask()`: SOC 高级算法。包括 **端点校正** (Cell Full/Empty 时强制校准 SOC) 和 **包间同步** (Sync 多个模块 SOC)。

### 4.2.2 `Task100ms` (每 100ms)

*   **任务**:
    *   `GroupWorkStateTask()`: **工作状态机**。IDLE -> START -> RUN -> STOP 状态转移逻辑。
    *   `DiagGroupSosTask()`: SOS (安全操作状态) 评估。汇总所有故障，计算安全等级 (0-4)。
    *   `GroupProtectTask()`: 根据 SOS 等级执行保护动作 (如降额、切断接触器)。

## 4.3 低频周期任务

| 任务 | 周期 | 主要功能 |
| :--- | :--- | :--- |
| `Task200ms` | 200ms | 5 步轮询 (热管理、高压标定、模式检查等) |
| `Task500ms` | 500ms | 快充协议栈 (GB/T 27930)、外设健康检查 |
| `Task1000ms` | 1000ms | 按键处理、DOD 统计、外部 Flash 检查 |
| `Task2000ms` | 2000ms | **掉电保护** (低电压关机逻辑)、RTC 同步 |

---

# 第五章：核心算法完整运行逻辑 (数据流)

## 5.1 SOC 计算 (State of Charge)

**原理**: 安时积分法 (Ah Counting) + 开路电压 (OCV) / 充电终压校正。

**数据流**:
1.  **1ms 中断**: `ISRCurrCapAddTask()` 累加电流。
    *   `Ah += Current(A) * 0.001(s)`.
    *   应用库伦效率 (Coulombic Efficiency): 充电通常效率 < 100%，此处代码配置为 101.5% (可能是补偿策略，或者特定电芯特性)。
2.  **10ms 任务 (Case 5)**: `GroupCapSocCalcTask()`
    *   读取积分 Ah 值。
    *   计算 `SOC_raw = (Capacity_Remaining / Capacity_Total)`。
    *   **温度补偿**: 查表 `cTempCapPercent[Temp]`。低温下容量缩减，显示 SOC 可能下降更快或进行补偿。
    *   **倍率补偿**: 大电流放电容量低于标称容量 (Peukert 效应)，查表 `cCurrCapPercent[C_rate]`。
3.  **50ms 任务**: `SocSoeLibAllTask()`
    *   **校正**:
        *   充电末端: 电芯电压 > `V_Full` 持续一定时间 -> `SOC = 99.3%` (强制拉满)。
        *   放电末端: 电芯电压 < `V_Empty` -> `SOC = 0.0%`。
    *   **平滑**: SOC 显示值变化速率限制 (如 Max 1% per sec)，防止显示跳变。

## 5.2 故障诊断流水线

系统采用 **"检测 -> 滤波 -> 评估 -> 执行"** 的四级流水线架构：

1.  **生成 (Generation)** @ 10ms: 
    *   采样值 vs 阈值 -> 置位原始标志 (Flag = 1 表示超限)。
2.  **滤波 (Filtering)** @ 50ms: 
    *   输入 Flag。
    *   **延时确认**: 
        *   若超限 -> `Timer_On++`。若 `Timer_On > Limit_On` -> 置位“故障”。
        *   若恢复 -> `Timer_Off++`。若 `Timer_Off > Limit_Off` -> 清除“故障”。
    *   **恢复计数限制**: 故障恢复后，在一定时间内 (如 10min) 再次触发，可能会加重处罚或锁定。
3.  **SOS 评估** @ 100ms: 
    *   检查所有“故障”标志。
    *   根据权重计算 SOS Level:
        *   Level 1: 告警 (Warning)。
        *   Level 2: 降额 (Derate) -> 修改电流限制。
        *   Level 3: 停机 (Protect) -> 打开接触器。
4.  **保护执行 (Execution)** @ 100ms: 
    *   驱动接触器控制逻辑。

## 5.3 电芯均衡 (Passive Balancing)

**策略**: 被动放电。
**执行周期**: `Task10ms` Case 7 (每 80ms)。
**逻辑**:
1.  **阈值**: 
    *   `V_Min > 3.1V` (LFP): 仅当最低电芯达到一定电压才开始均衡 (避免放空低电芯)。
    *   `Delta_V > 10mV`: 压差阈值。
2.  **积分判定**: 并不是压差一出现就均衡，而是累计时间。
    *   若 `V_Cell > V_Avg + Delta` -> `Timer_Eq[cell]++`。
    *   若 `V_Cell < V_Avg` -> `Timer_Eq[cell]--`。
    *   当 `Timer_Eq` 超过设定值 (如 60s)，确认为长期偏差，开始均衡。
3.  **温度限制**: 
    *   均衡电阻发热。若 PCB 温度 > 80°C，强制关闭均衡。
    *   若 > 60°C，限制均衡通道总数 (如最多开 8 路)。

---

# 第六章：系统整体运行逻辑总览

## 6.1 数据流总览 (Sensor -> Display)

```text
[物理层] 电芯/温度/电流/电压/状态
    │
    ▼
[采样层] LTC6813(电压) / LTC2949(电流) / ADC(辅助)
    │
    ▼
[数据清洗] 滤波、标定(K/B系数)、断线检测
    │
    ▼
[特征提取] 最大值/最小值/平均值/极差
    │
    ▼
[算法层] 
   ├── SOC/SOE 积分 ──> 显示
   ├── 故障判定 ──────> 保护停机
   ├── 均衡计算 ──────> 输出控制
   └── 绝缘计算 ──────> 告警
    │
    ▼
[通信层] CAN (广播/响应) / UART (Modbus)
    │
    ▼
[外部] 上位机 / 充电机
```

## 6.2 时间轴总结 (100ms 切片)

| 时间 | 0ms | 10ms | 20ms | 30ms | 40ms | 50ms |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **ISR** | FTM1(0.1ms)*100<br>FTM3(1ms)*100 | FTM1*100<br>FTM3*100 | ... | ... | ... | ... |
| **10ms** | GPIO | 限流 | 特征 | 故障#1 | 故降#2 | 滤波(50ms任务触发点) |
| **50ms** | - | - | - | - | SOC计算(10ms Case 5)<br>均衡(10ms Case 7) | **Task50ms 执行**<br>(滤波+校正) |
| **100ms**| - | - | 保护 #3(10ms Case 4) | - | - | **Task100ms 执行**<br>(状态机) |

(注：Timer 触发点可能与 10ms 轮询有相位差，取决于启动时间)

