# S32K146OFBMU 电池管理系统 — 运行流程、调度与架构深度分析

**文档版本**: V3.0 (深度完整版)  
**生成日期**: 2026-04-20  
**适用项目**: BMU-146 (基于 NXP S32K146)

---

## 全局目录 (Table of Contents)

1.  **第一章：系统硬件与软件架构总览**
    *   1.1 硬件平台深度解析 (MCU 与外设拓扑)
    *   1.2 关键外围芯片功能与通信拓扑
    *   1.3 软件分层架构设计 (BSP -> HAL -> APP)
    *   1.4 源码目录功能映射表
2.  **第二章：系统启动完整流程 (代码级细节)**
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
3.  **第三章：中断处理机制与数据流 (ISR)**
    *   3.1 FTM1 定时器中断 (0.1ms) — Modbus 时序驱动
    *   3.2 FTM3 定时器中断 (1ms) — 系统心跳与积分
        *   3.2.1 电流积分 `ISRCurrCapAddTask` 细节
        *   3.2.2 任务调度分发逻辑
    *   3.3 CAN 收发与错误中断处理机制
    *   3.4 SCI (UART) 字节收发与 Modbus 组帧
    *   3.5 SPI 传输完成中断
    *   3.6 中断与 RTOS 任务的交互模型 (ISR to Task)
4.  **第四章：FreeRTOS 软件定时器任务调度表**
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
5.  **第五章：核心算法完整运行逻辑 (数据流与状态机)**
    *   5.1 SOC/SOE 算法：从安时积分到端点校正
    *   5.2 故障诊断流水线：生成 -> 滤波 -> SOS -> 保护
    *   5.3 电芯均衡策略：积分决策与温度限流
    *   5.4 绝缘检测与粘连检测状态机
    *   5.5 限流计算：多维度取最小值

---

# 第一章：系统硬件与软件架构总览

## 1.1 硬件平台深度解析

### 核心微控制器 (MCU)
系统基于 NXP 的 **S32K146** 微控制器，属于车规级 Cortex-M4F 系列，专为高可靠性 BMS 应用设计。

| 特性 | 参数值 | 说明 |
| :--- | :--- | :--- |
| **内核** | ARM Cortex-M4F | 带单精度 FPU，适合浮点运算 (如 SOC/SOE 浮点计算) |
| **主频** | 80 MHz | 通过 PLL 锁相环将外部 16MHz 晶振倍频得到 |
| **Flash** | 1 MB (1024 KB) | 存储代码 (`0x0000`-`0x060000`) 及内部模拟 EEPROM/Flash |
| **RAM** | 128 KB | 运行时数据，包括全局变量、堆栈 (Heap/Stack)、FreeRTOS 内存 |
| **FlexCAN** | x3 通道 | 3 路 CAN 控制器，硬件 29 位 ID 过滤，带环形接收缓冲区 |
| **LPUART (SCI)** | x4 通道 | 4 路串行通信，支持中断收发，用于 RS485 Modbus |
| **LPSPI** | x4 通道 | 4 路 SPI 主机，用于连接外部传感器、存储器等 |
| **ADC** | 2 模块 (16 通道/模块) | 12 位精度，用于采集霍尔、温度、电源等模拟信号 |
| **FTM** | x4 定时器 | 用于 PWM 生成、输入捕获及系统时基 (FTM0-3) |

### 关键外围芯片拓扑

系统采用分布式架构，主控 MCU 通过 SPI 总线与多个专用芯片通信：

1.  **FS6503 (SBC - System Basis Chip)**
    *   **接口**: SPI0
    *   **功能**: 电源管理 (VDD/VDD_MCU_5V/VCORE)，安全看门狗 (WD)，复位监控。
    *   **重要性**: 保证系统的硬件基础安全性，若看门狗不刷新则硬件强制复位。

2.  **LTC6813 (电芯电压/温度监控)**
    *   **接口**: SPI2 (菊花链 daisy-chain)
    *   **功能**: 测量多达 12 串电芯电压 (精度 <1.2mV) 及 GPIO 温度。
    *   **扩展**: 支持多片串联 (最高 20 片)，通过 isoSPI 隔离通信，覆盖 240 串电芯。

3.  **LTC2949 (高压库仑计)**
    *   **接口**: SPI1
    *   **功能**: 高精度电流/电压/功率积分。提供独立的 Shunt 分流器和霍尔校准数据。
    *   **特性**: 内部 48 位累加器，直接计算 Charge/Energy，减轻 MCU 负担，提供高精度基准。

4.  **SST26VF064 (外部 Flash)**
    *   **接口**: SPI3
    *   **功能**: 8MB 存储空间。
    *   **用途**: 故障记录 (2M-3M)、运行数据记录 (5M-7M)、波形记录 (4M-5M)、OTA 升级包缓存 (1M-2M)。

5.  **M24512 (外部 EEPROM)**
    *   **接口**: I2C (模拟)
    *   **功能**: 64KB 参数存储，用于保存配置、标定系数、历史统计等。

## 1.2 软件分层架构

代码库采用严格的分层设计，确保硬件无关性与代码复用率。

```text
[App Layer]  (业务逻辑与核心算法)
   |
   +--- Funtion/      : 核心算法库 (SOC/SOE, 诊断, 统计, 通信栈)
   +--- Logics/       : 业务策略 (状态机, 均衡, 保护, 快充)
   +--- PileSys/      : 堆级管理 (多簇并联协调, 可选条件编译)
   +--- Addition/     : 附加功能 (标定, 升级, 权限, 专有协议)
   |
   +--- OSUser/       : RTOS 适配层 (任务创建, 定时器回调注册)
   |
[HW Abstraction] (BSP User)
   |
   +--- BSPUser/      : 统一对外 API (BSPCANInit, DEVADCGetSampAD 等)
   |
[HW Driver]  (MCU 外设驱动)
   |
   +--- BSPSys/Driver/: S32K146 底层驱动 (UART, CAN, SPI, FTM)
   +--- BSPSys/Util/  : 通用工具 (环形队列, 数据类型)
   +--- BSPSys/BSPCfg/: 编译配置 (BSPConfig.h) - 决定系统能力的宏集合
   |
[Device]   (芯片级驱动)
   |
   +--- BSPSys/Device/: 芯片专用驱动 (FS6503, LTC6813, LTC2949, M24512, SST26)
```

这种设计的核心在于：**业务层不直接使用 S32K146 的外设驱动，而是调用 BSPUser 接口。** 这样如果更换 MCU 或外设引脚，只需修改 Driver 层和 BSPUser 映射关系，上层业务无需改动。

---

# 第二章：系统启动完整流程 (代码级细节)

系统上电后，经历由 汇编启动代码 -> C main 入口 -> 硬件初始化 -> 软件初始化 -> OS 启动 的严密过程。

## 2.1 阶段一：硬件复位与 Startup Code

**文件**: `Project_Settings/Startup_Code/startup_S32K146.S`
**执行流**:
1.  `Reset_Handler`: 硬件复位后执行的第一段代码。
2.  **堆栈设置**: MSP (Main Stack Pointer) 指向 `__StackTop`。
3.  **数据段初始化**: 调用 `copy` 函数将 `.data` 段（已初始化全局变量）从 Flash 复制到 RAM。
4.  **BSS 段清零**: 将 `.bss` 段（未初始化全局变量/静态变量）清零，防止随机垃圾值导致错误判断。
5.  **SystemInit**: 配置 Flash Wait States，启用外部晶振 (OSC)，配置 PLL。
6.  **调用 main**: `bl main`，进入 C 世界。

## 2.2 阶段二：BSP 配置加载

**入口**: `main()` -> `BSPUserCfgInit()`
**文件**: `Sources/BSPSys/BSPCfg/BSPConfig.c`

**详细流程**:
```c
void BSPUserCfgInit(void) {
    /* 1. 将编译宏 (BSPConfig.h) 映射到运行时结构体 gBSPFunCfg */
    // gBSPFunCfg 是一个联合体 (union)，每个位代表一个驱动模块的开关
    gBSPFunCfg.Bit.FTM1_FUN_EN = FTM1_FUN_EN; // 0.1ms 定时器使能
    gBSPFunCfg.Bit.FTM3_FUN_EN = FTM3_FUN_EN; // 1ms 定时器使能
    gBSPFunCfg.Bit.CAN0_FUN_EN = CAN0_FUN_EN;
    
    /* 2. 初始化参数配置结构体 gBSPParaCfg */
    // 这些值将被具体的 Driver Init 函数读取
    gBSPParaCfg.extOsc = 16000000;          // 外部晶振 16MHz
    gBSPParaCfg.busClk = 80000000;          // 总线时钟 80MHz
    gBSPParaCfg.can0Bps  = 500000;          // CAN0 波特率
    gBSPParaCfg.sci0Baud = 19200;           // UART0 波特率
    gBSPParaCfg.wdogPeriod = 100;           // 看门狗周期 100ms 等
}
```
**意义**: 使得同一套代码可以通过修改 `BSPConfig.h` 适应不同项目，无需大改逻辑。

## 2.3 阶段三：MCU 外设驱动初始化

**入口**: `main()` -> `BSPDriverInit()`
**文件**: `Sources/BSPSys/BSPCfg/BSPConfig.c`

**执行顺序** (严格依赖关系，不可乱序):

1.  **看门狗 (WDOG) 初始化**: 
    *   配置 `WDOG_TOVAL` 寄存器和 `WDOG_PRESC`。
    *   **关键**: 立即调用 `BSPWatchDogTask()` 喂狗，否则后续初始化耗时超过 100ms 会导致芯片复位死循环。
2.  **时钟 (CRG/MCG) 配置**: 
    *   确认 PLL 锁定，切换系统时钟源到 PCC (Phase Locked Loop Clock)，达到 80MHz。
3.  **UART (SCI) 初始化**: 
    *   SCI0/SCI1/SCI2: 配置波特率寄存器 `BAUD` (SBR 值计算), 使能接收中断 `C2[RIE]`。
4.  **定时器 (FTM) 初始化**: 
    *   FTM1 (0.1ms): 配置 `MOD` 值 (计数终点)，使能溢出中断。
    *   FTM3 (1ms): 配置 `MOD` 值，使能溢出中断。
5.  **CAN 控制器初始化**: 
    *   配置 `CTRL1` (时钟分频, 波特率段)。
    *   配置 `MCR` (模块控制寄存器, 进入 Freeze 模式)。
    *   配置 `IMEUR`/`IFLAG1` (邮箱与标志)。
    *   初始化环形接收缓冲区 `sRcvMsgRingBuf`。
6.  **SPI 控制器初始化**: 
    *   SPI0/1/2/3: 配置时钟、极性、相位 (CPOL/CPHA)，设为 Master 模式。

## 2.4 阶段四：外部专用芯片驱动初始化 (DEVDriverInit)

**入口**: `BSPDriverInit()` -> `DEVDriverInit()`
**文件**: `Sources/BSPSys/Driver/`

此阶段探测并初始化挂在 MCU 外部总线上的芯片：
1.  **GPIO (DEVGPIOInit)**: 配置所有端口方向。
    *   **输入**: 接触器反馈信号、干接点信号、硬件版本 DIP。
    *   **输出**: 继电器控制、均衡控制、LED 灯。
2.  **SBC (FS6503) 初始化**:
    *   通过 SPI0 发送 `FS65_Init()`。
    *   读取 `INT_M` 寄存器确认无故障。
    *   配置看门狗种子 (Seed)，进入挑战-应答模式。
3.  **RTC (RX-8010) 初始化**: 
    *   模拟 I2C 读取寄存器 `0x00` (Second)，确认为有效 BCD 码。
4.  **EEPROM (M24512) 初始化**: 
    *   I2C 读取设备 ID 或特定标志位 `0x5AA5A55A`，确认 EEPROM 完好且参数未丢失。
5.  **Flash (SST26VF064) 初始化**: 
    *   SPI3 发送 JEDEC ID 读取指令，校验返回值 `0xBF2643`。

## 2.5 阶段五：临界区初始化 (关键!)

### 2.5.1 `BSPDisIRQUserInit()` (关中断)
```asm
CPSID i  ; 汇编指令，关闭全局 IRQ
```
**目的**: 此时系统处于临界区。我们在做大量的内存拷贝、链表初始化、传感器寄存器配置，**绝对不能被中断打断**。如果被 1ms 中断打断，中断服务程序读取到只初始化了一半的变量，会导致灾难性的计算错误或状态机乱序。

**执行内容**:
*   **参数加载**: 读取 EEPROM 中的系统参数 (`gGBmuGenPara_102` 等)，校验 CRC32。
*   **传感器底层配置**: 
    *   `CellLTC6813Init()`: 唤醒 LTC6813 (发送 `WRCFG` 唤醒命令)，配置 ADC 转换速率 (27kHz)。
    *   `HigLVSampleInit()`: 配置 LTC2949 的采样模式。
*   **通信库状态机复位**: CAN/Modbus 队列头尾指针清零 (`QueInit`)。
*   **诊断模块清零**: 错误标志数组全零。

### 2.5.2 `BSPEnIRQUserInit()` (开中断)
```asm
CPSIE i  ; 汇编指令，开启全局 IRQ
```
**后果**: 此时 FTM1 (0.1ms) 和 FTM3 (1ms) 开始触发中断，CPU 随时可能跳转执行 ISR。

**执行内容**:
*   `BootLoaderInit()`: 检查 Flash 中的 Boot 标志，决定是否跳转 Bootloader。
*   `CalibrationInit()`: 加载高压/电流的 K/B 标定系数。
*   `SocSoeInit()`: 恢复上一次断电时的 SOC 值 (从 EEPROM)。

## 2.6 阶段六：RTOS 调度器启动

**入口**: `main()` -> `PEX_RTOS_START()` -> `RtosTaskStart()`
**文件**: `Sources/OSUser/OSUserHOOK.c`

```c
void RtosTaskStart(void) {
    // 1. 创建软件定时器 (共 11 个，周期 2ms ~ 2000ms)
    //    使用 xTimerCreate 创建，自动重载 (pdTRUE)
    //    注意: 创建时定时器并不立即运行，需 xTimerStart

    // 2. 启动所有定时器
    xTimerStart(Timer2ms_Handle, 0);
    // ...
    xTimerStart(Timer2000ms_Handle, 0);

    // 3. 创建常驻 RTOS 任务 (Tasks)
    //    任务拥有自己的栈空间 (Stack) 和优先级
    xTaskCreate(HigInfoSampleTask,       "HV_SAMP",       400, NULL, 14, NULL);
    xTaskCreate(CellInfoSampleTask,      "CELL_SAMP",     300, NULL, 18, NULL);
    xTaskCreate(BSPCANRcvTask,           "CAN_RCV",       400, NULL, 15, NULL);
    xTaskCreate(BSPCANSendTask,          "CAN_TX",        400, NULL, 17, NULL);

    // 4. 启动 RTOS 调度器
    vTaskStartScheduler();
    
    // 此处理论上永远不会执行
    for(;;);
}
```
**状态**: 此后，CPU 时间片由 FreeRTOS 接管，根据优先级和定时器到期情况自动调度。

---

# 第三章：中断处理机制与数据流 (ISR)

中断是实时系统和轮询系统的分水岭。在 BMS 中，电流积分和 Modbus 时序对时间要求极高，必须在中断中处理。

## 3.1 FTM1 定时器中断 (0.1ms)

**频率**: 10 kHz
**代码回调**: `Time_ISR_Hook(eFTM1)`

**任务**: **Modbus 帧时序维持**
Modbus RTU 协议依赖字符间的时间间隔 (t1.5, t3.5) 来判断一帧数据的开始和结束。如果 MCU 处理慢，导致间隔超时，会误判帧头，导致通信错误。

```c
void Time_ISR_Hook(e_FTMNum Channel) {
    if (Channel == eFTM1) {
        // 必须在启动 1ms 后才执行，防止初始化未完成时干扰
        if (BSPGetOSMillSecTimeAPI() > 1) {
            ModbusRcvTickTask(); // 更新静默时间计时器
        }
    }
}
```

## 3.2 FTM3 定时器中断 (1ms)

**频率**: 1 kHz
**代码回调**: `Time_ISR_Hook(eFTM3)`

这是系统最重要的实时循环之一，包含 5 个关键任务。**注意：ISR 中不能调用阻塞函数（如 `vTaskDelay`）。**

```c
void Time_ISR_Hook(e_FTMNum Channel) {
    if (Channel == eFTM3) {
        // 1. 电流积分 (Ah Counting) - ★ 精度最高要求
        // 必须在 1ms 精确触发，否则 Ah 计算会有累积误差
        ISRCurrCapAddTask(); 
        
        // 2. 系统时钟驱动 (ms -> s -> m -> h -> d -> y)
        SysClockCountTask();

        // 3. Modbus 客户端/主站 状态驱动
        // 处理请求超时重传逻辑
        MbClientTimerTask();

        // 4. CAN 协议栈心跳
        // 处理多帧超时、心跳检测
        CANLibTxRcvHandleTask_API();
        
        // 5. 从属 MCU (KEA128) 心跳检测
        // 每 100ms 执行一次，检查 EWM 信号或 SPI 地址同步
        static u8 sCount = 0;
        if (++sCount >= 100) {
            HeartToSlaveMcuTask();
            sCount = 0;
        }
    }
}
```

**设计隐患**: `CANLibTxRcvHandleTask_API()` 可能包含较多的逻辑判断。如果 CAN 报文量大，ISR 执行时间可能接近 0.8ms，导致系统负载 (CPU Load) 升高，甚至阻塞下一次 1ms 中断。

## 3.3 CAN 收发与错误中断处理机制

*   **接收 (Rx)**:
    *   **触发**: CAN 总线收到匹配 ID 的帧 -> 硬件置位 `Rx Flag`。
    *   **ISR 动作**: `BSPCAN.c` 读取 `CANn_MB[i].CS` 和 `DATA` 寄存器 -> 将帧结构体拷贝到 **内存环形队列** -> 释放信号量 `SEM_CAN_RCV`。
    *   **后续**: `BSPCANRcvTask` (优先级 15) 获取信号量，从队列取出帧，进行协议解析 (如判断是否是广播帧 `0x050...`)。
    *   **优势**: ISR 仅做搬运 (耗时 < 10us)，繁重的解析逻辑放在任务中，不阻塞其他中断。
*   **发送 (Tx)**:
    *   发送请求通常由应用层发起 -> 放入发送队列 -> `BSPCANSendTask` 轮询检查邮箱是否空闲 -> 触发发送。
    *   完成中断主要用于释放队列空间或触发下一帧发送。

## 3.4 SCI (UART) 收发中断

*   **接收 (Rx)**:
    *   **触发**: 收到 1 个字节。
    *   **ISR 动作**: 读取 `UART0_D` -> 存入 Modbus 接收字节缓冲区 `sUartRxBuffer`。
    *   **逻辑**: 不解析 Modbus 功能码！只搬运字节。Modbus 的组帧逻辑在 `MbHandle.c` 中，依赖于 FTM1 提供的 t3.5 超时时间。
*   **发送 (Tx)**:
    *   **触发**: 发送数据寄存器空 (`TDRE` 变高)。
    *   **ISR 动作**: 如果待发缓冲区有数据，写入 `UART0_D` 启动下一个字节发送。
    *   **完成**: 当最后一个字节进入发送移位寄存器 (TC 标志位)，触发 TC 中断。
        *   此时切换 RS485 收发控制引脚 `RE/DE` 为接收模式 (代码中 `SCI_ISR_Send_Hook` 预留)。

## 3.5 SPI 传输完成中断

*   **触发**: 16-bit 数据发送完成且接收完成。
*   **ISR 动作**: 清除中断标志，读取接收寄存器 (防止 FIFO 溢出)。
*   **逻辑**: 通常用于通知等待的采样任务 (如 LTC6813 采样任务) "DMA/中断 传输完成，可以取数据了"。

## 3.6 中断到任务的交互模型 (ISR to Task)

为了保证系统稳定性，BMS 采用 **"中断置标志/入队 -> 任务处理"** 的异步模型。

```text
[中断] (高优先级, 短耗时)                      [任务] (低优先级, 耗时)
  |                                               ^
  |-- ISR 1: 清除标志位, 拷贝数据到缓冲            |
  |-- ISR 2: xSemaphoreGiveFromISR()             | 等待信号量
  `-- 退出中断                                    |-- xSemaphoreTake() (阻塞等待)
                                                |-- 处理数据
                                                `-- 回到阻塞
```

---

# 第四章：FreeRTOS 软件定时器任务调度表

FreeRTOS 的 Software Timers 提供了一种轻量级的定时机制。所有的 Timer 回调都在同一个 **Timer Service Task** (低优先级，通常为 1-2) 中执行。这意味着如果所有 Timer 回调函数执行时间总和超过了最短周期 (2ms)，Timer 就会发生滞后 (Drift)。

## 4.1 调度架构设计原则

*   **Timer 回调**: 适合执行 **短小、周期性、非阻塞** 的逻辑 (如状态翻转、计数器累加)。
*   **Task (任务)**: 适合执行 **长耗时、需要阻塞等待 (如 SPI 通信、I2C 读取)** 的逻辑。

本系统中，**绝大多数算法逻辑放在了 Timer 回调中**，这意味着 Timer Service Task 是系统的核心执行流之一。必须严格控制回调时长 (< 1ms)。

## 4.2 高频周期任务 (2ms ~ 10ms)

### 4.2.1 `Task2ms` (每 2ms)

*   **任务**: `MbTrigSendTimerTask()`
*   **功能**: Modbus 通信协议栈的“发送触发器”。
*   **逻辑**: 检查 `sMbTxQueue` (发送队列) 是否有帧。如果有，且总线空闲，则启动 UART DMA 或中断发送。
*   **注意**: 仅在 Modbus 从机模式下有效。

### 4.2.2 `Task5ms` (每 5ms)

*   **任务**:
    1.  `CANLibHandleTask_API()`:
        *   处理 CAN 应用层协议 (组包、解析多帧上传/下载)。
        *   发送周期性广播报文 (Broadcast)，如心跳帧。
    2.  `FS65DiagnosticTask()`:
        *   FSM (FS6503 State Machine) 刷新。
        *   **关键**: 必须在 512ms 窗口内完成一定次数的刷新，否则看门狗复位。这里 5ms 调用一次，频率足够高。
    3.  `CanSendUpCatlMsgCycleTask()` (条件编译):
        *   发送 CATL 专有协议的上报报文。

### 4.2.3 `Task10ms` (每 10ms) — 核心调度器 (10 步轮询)

这是整个系统最繁忙的逻辑部分。为了避免 100ms 级任务同时执行导致系统卡顿，实现了一个 **10 步轮询器**。

```c
static u8 sTime = 0; // 状态寄存器 0-9

void Task10ms(void) {
    // 【公共任务】每次 10ms 必执行
    DEVGPIOMulPortManagTask(); // 多路 GPIO 复用切换
    PortStateCheckTask();      // 输入端口滤波 (按键/干接点)
    // (如果有) GroupSendDnGTBmmMsg_API();

    // 【轮询调度】每次执行一个分支，100ms 循环一次
    switch(sTime) {
        case 0: // [10ms, 110ms...] 
            // ★ 计算充放电限流 (温度/电压/SOC 降额)
            CurrLimCalcTask(); 
            CurrLimFallTask(); // CV 软降额
            break;
        case 1: // [20ms, 120ms...]
            // 电压/温度 极值与一致性检查
            CheckVoltCharaTask();
            CheckTempCharaTask();
            // 计算平均电流 (200 点滑动平均)
            CheckAvgCurrTask(); 
            break;
        case 2: // [30ms, 130ms...]
            // ★ 故障诊断 - 原始状态生成 (电压/温度/电流越限)
            // 生成 0 / 1 标志位，尚未经过滤波
            DiagVoltLimFilMakerTask();
            DiagTempLimFilMakerTask();
            break;
        case 3: // [40ms, 140ms...]
            // ★ 故障诊断 - 原始状态生成 (IO/信息异常)
            DiagPortExpFilMakerTask();
            DiagInfoExpFilMakerTask();
            break;
        case 4: // [50ms, 150ms...]
            // ★ 故障诊断 - 原始状态生成 + 诊断显示更新
            DiagStateExpFilMakerTask();
            DiagErrDisplayTask();
            break;
        case 5: // [60ms, 160ms...]
            // ★ 核心算法：SOC/SOE 积分运算
            // 读取积分器数据，计算百分比，应用补偿系数
            GroupCapSocCalcTask();
            GroupEnerSoeCalcTask();
            break;
        case 6: // [70ms, 170ms...]
            // ★ 核心算法：SOH (健康状态) 计算
            GroupFadeSohCalcTask(); 
            GroupEnerDisplayTask(); // 计算显示功率/能量
            break;
        case 7: // [80ms, 180ms...]
            // 电芯被动均衡策略判定 (积分式)
            GroupVoltBalanTacticTask(); 
            PwmInfoCaptureTask();     // PWM 频率读取
            break;
        case 8: // [90ms, 190ms...]
            // 绝缘检测状态机 & 保护状态机
            InsuAdhesMeasurTask();
            GroupProStateTask();      // 根据 SOS 计算结果触发保护
            EventRecordUserTask();    // 状态变化触发事件记录
            break;
        case 9: // [100ms, 200ms...]
            // 接触器状态检查、快充协议发送、参数备份
            DiagSwhChangCStateCheckTask();
            CHGLibSendMsgTask();      // GB/T 27930 报文组包
            ParaStoreCopyTask();      // 将内存参数备份到 Flash
            break;
    }

    sTime = (++sTime % 10);
}
```

**设计优势**: 
1.  **平滑 CPU 负载**: 原本 100ms 内要执行的 10 个任务被均匀打散到 100ms 中执行，避免 CPU 占用率峰值 (Spike)。
2.  **降低 RAM 需求**: 不需要为每个 100ms 任务创建独立的 Task，节省了栈空间 (每个 Task 至少 200 字节)。

## 4.3 中频周期任务

### 4.3.1 `Task50ms` (每 50ms)

*   **任务**:
    1.  `DiagErrFilterTask(eERR_ID_FINISH)`:
        *   **全局故障滤波**: 遍历 96+ 个错误 ID。
        *   **消抖逻辑**: 
            *   错误发生 -> 计时器累加 -> 超过 `Delay_On` -> 确认故障。
            *   错误消失 -> 计时器累减 -> 超过 `Delay_Off` -> 确认恢复。
    2.  `SocSoeLibAllTask()`:
        *   **SOC/SOE 高级处理**:
            *   **端点校正**: 如果检测到电芯电压 > 3.65V (满充)，强制 SOC = 99.3%。
            *   **同步**: 如果是并联系统，同步各个 BMU 的 SOC/SOE。
    3.  `GroupSendUpMsg1sBy50ms_API()`:
        *   将 1s 周期的大量上报报文拆分成多个 50ms 发送，保证总线负载率和数据实时性。

### 4.3.2 `Task100ms` (每 100ms)

*   **任务**:
    1.  `GroupWorkStateTask()`: 
        *   **工作状态机**: IDLE (空闲) -> START (启动/预充) -> RUN (运行) -> STOP (停机) -> ERR (错误)。
    2.  `DiagGroupSosTask()`:
        *   **SOS (Safety Operation State) 计算**: 汇总所有滤波后的故障，计算当前安全等级 (0=最安全, 4=最危险)。
    3.  `GroupProtectTask()`:
        *   根据 SOS 等级执行动作。例如 SOS >= 3 时，立即断开接触器。
    4.  `RcdFaultWaveHandleTask()`:
        *   如果发生故障，将故障前后的波形数据 (缓存区) 写入 Flash。

## 4.4 低频周期任务 (200ms ~ 2000ms)

| 定时器 | 任务 | 详细说明 |
| :--- | :--- | :--- |
| **Task200ms** | 轮询 (5 步) | 0:热管理(风机), 1:电压限值, 2:工作模式, 3:通信检查, 4:**标定处理** |
| **Task500ms** | `GroupFstChgTacticsTask` | **快充协议 (GB/T 27930) 主状态机** (握手/配置/充电/结束) |
| **Task1000ms** | `GroupSysIoUpDnCtrlTask` | **按键处理**: 短按/长按/双击检测。`GroupSysPwrOffProTask` |

---

# 第五章：核心算法完整运行逻辑 (数据流)

## 5.1 SOC 计算 (State of Charge) 全链路

**算法**: 安时积分 (Ah Counting) + 温度补偿 + 满充满放校正 (OCV)。

### 链路详解
1.  **采样 (1ms FTM3 ISR)**: 
    *   `ISRCurrCapAddTask()`:
        *   读取 LTC2949 或 Hall ADC -> 电压值 `V_raw`。
        *   **标定**: `I_real = (V_raw * K + B) / 1000`。
        *   **积分**: `Ah_count += (I_real * 1ms) / 3,600,000 ms` (单位转换)。
        *   区分充放电，分别累加 `chgCap` 和 `dhgCap`。
2.  **计算 (Task10ms Case 5)**:
    *   读取 `Ah_count` 和 `TotalCap` (总容量)。
    *   **温度补偿**: 查表 `cTempCapPercent[T]`。例如 25°C 时 100%，0°C 时 95%，-20°C 时 80%。修正后的容量 `Cap_adj = TotalCap * Percent`。
    *   **倍率补偿 (Peukert 效应)**: 查表 `cCurrCapPercent[C_rate]`。大电流放电时，可用容量减少。
    *   计算 SOC: `SOC = (Rem_Cap / Cap_adj) * 100`。
3.  **校正 (Task50ms)**:
    *   **满充校正**: 当最高电芯电压 `V_cell_max > V_Full (如 3.6V)` 且 `I_charge < C_End (如 0.05C)` 持续 N 秒 -> 判定满充 -> `SOC = 99.3%`。
    *   **放空校正**: 当最低电芯电压 `V_cell_min < V_Empty` -> 判定放空 -> `SOC = 0.0%`。
    *   **平滑滤波**: 显示的 SOC 值变化速率限制 (如 max 0.5%/s)，防止跳变影响用户体验。

## 5.2 故障诊断流水线 (四级架构)

系统采用 **"检测 -> 滤波 -> 评估 -> 执行"** 的四级流水线：

### Level 1: 故障生成 (Maker) @ 10ms (Case 2~4)
*   遍历每个监测量 (电压, 温度, 电流)。
*   对比阈值 (L1 告警, L2 停机)。
*   置位原始标志 `Raw_Flag[n] = 1`。

### Level 2: 故障滤波 (Filter) @ 50ms
*   **目的**: 消除毛刺。比如电压瞬间波动 2ms，不应导致停机。
*   **状态机**:
    *   如果 `Raw_Flag == 1` -> 计时器 `Timer` 增加。若 `Timer > Delay_On` (如 2s) -> `Stat = Fault`。
    *   如果 `Raw_Flag == 0` -> 计时器 `Timer` 减少。若 `Timer < Delay_Off` -> `Stat = Normal`。
*   **恢复限制**: 如果短时间内多次触发 (10min内 > 3次)，则锁定 `Stat = Fault`，即使恢复正常也不清除，需人工复位。

### Level 3: 安全评估 (SOS) @ 100ms
*   汇总所有 `Stat == Fault` 的错误。
*   加权得出安全等级 `SOS_Level`:
    *   **Level 1 (告警)**: 不影响运行，仅上报。 (例: 通信偶发中断)
    *   **Level 2 (降额)**: 降低充放电电流限制。 (例: 温度过高 45°C)
    *   **Level 3 (保护)**: 断开接触器，但允许自检。 (例: 电芯过压 L2)
    *   **Level 4 (停机)**: 锁定系统，不可恢复，需断电重启。 (例: 硬件短路)

### Level 4: 保护执行 (Protect) @ 100ms
*   根据 SOS 级别，控制 GPIO (接触器/报警灯)。

## 5.3 电芯均衡策略

**策略**: 被动放电 (电阻耗散)。
**执行周期**: Task10ms Case 7 (每 80ms 判定一次)。

### 核心逻辑
1.  **前提条件**:
    *   BMS 非调试模式。
    *   均衡使能位 == 1。
    *   无通信、采样故障。

2.  **判定逻辑 (积分制)**:
    *   计算单体压差 `Delta = V_cell - V_avg`。
    *   若 `Delta > Threshold_P` -> `Integral[cell]++` (趋向于需要均衡)。
    *   若 `Delta < Threshold_N` -> `Integral[cell]--` (趋向于不需要均衡)。
    *   当 `Integral > Integral_Max` -> 确认为长期偏差 -> **开启均衡**。
    *   当 `Integral < Integral_Min` -> **关闭均衡**。

3.  **安全限制**:
    *   **温度限制**: 均衡电阻发热大。
        *   板温 > 60°C: 限制均衡数量 (如最多 8 路)。
        *   板温 > 80°C: **强制关闭所有均衡**，防止热失控。
    *   **电压保护**: 均衡开启可能导致电芯电压下降，需监控均衡后电压不跌破下限。

## 5.4 绝缘检测与粘连检测

**检测方法**: 桥式电路法 (电桥平衡法)。
**执行任务**: `InsuAdhesMeasurTask` (Task10ms Case 8 及 Task200ms)。

### 绝缘电阻计算
1.  **S1 闭合**: 将标准电阻 `R_std_pos` 接入正极与地之间。测量对地电压 `V_p1`。
2.  **S2 闭合**: 将标准电阻 `R_std_neg` 接入负极与地之间。测量对地电压 `V_p2`。
3.  **求解方程组**: 根据分压公式，反解出正极绝缘电阻 `R_iso_p` 和负极绝缘电阻 `R_iso_n`。
    *   `R_iso_p = ...`
    *   `R_iso_n = ...`
4.  若 `R_iso < Threshold (如 500Ω/V * 系统电压)` -> 报绝缘故障。

### 接触器粘连检测
*   **时机**: 系统停机或断开接触器后。
*   **原理**:
    *   断开主正/负接触器。
    *   检测接触器两端电压。如果接触器断开，两端应该有电位差 (等于电池电压)。如果电压接近 0V，说明接触器触点熔焊 (粘连)，未真正断开。

---

*(本文档第一部分结束，包含硬件架构、启动、中断、RTOS 调度、核心算法逻辑。)*
*(由于篇幅限制，后续章节包括：第六章通信协议栈详解、第七章架构设计深度分析[优势/缺陷/改进] 将在下一部分提供。)*
---

# 第六章：核心通信协议栈逻辑

系统涉及多路复杂的通信协议，涵盖内部通信 (BMU-BSU) 与外部通信 (上位机、充电机)。

## 6.1 CAN 通信协议栈

**文件**: `Sources/Funtion/CanLib/CANHandle.c`, `Sources/Logics/CanUser/GroupMsg.c`

### 报文 ID 结构 (29 位扩展帧)

```text
Bit [28]      : 方向位 (0=下行/Broadcast, 1=上行/Request)
Bit [27:24]   : 功能码 (0x1=心跳, 0x2=广播, 0x3=读取, 0x4=修改, ...)
Bit [23:16]   : 消息类型 (具体业务类型码)
Bit [15:8]    : 目标设备类型 (BMU, BSU, Charger)
Bit [7:0]     : 目标设备地址 (本机地址)
```

### 通信流程 (读写分离)

1.  **上行 (BMU -> Host)**:
    *   由定时器触发 (如 `Task50ms` 或 `Task100ms`)。
    *   应用层调用 `GroupSendUpMsg()`，填充数据内容。
    *   调用 `CanLibSendFrame()`，将报文压入发送队列。
    *   `BSPCANSendTask` 轮询队列，取出报文通过硬件 CAN 发出。
2.  **下行 (Host -> BMU)**:
    *   CAN 硬件接收帧，存入环形缓存区。
    *   信号量唤醒 `BSPCANRcvTask`。
    *   Rcv Task 解析报文头部 -> 根据功能码分发。
    *   **解析分发函数**: `CANRcvMsgHandle_Hook()`
        *   读请求 -> 读取对应参数/状态，构造回复帧压入发送队列。
        *   写请求 (如修改保护阈值) -> 校验密码权限 -> 修改内部参数 -> 写入 EEPROM -> 回复确认帧。
    *   心跳报文 -> 更新通信状态标志 `sRcvHeartbeatState = OK`。

## 6.2 Modbus RTU 通信协议栈

**文件**: `Sources/Funtion/ModbusLib/MbServer.c`

Modbus 通常运行在 SCI (UART/RS485) 上，作为从站响应上位机。

### 运行环境
*   **波特率**: 通常 19200, 38400 等。
*   **时序依赖**: `FTM1 (0.1ms)` 中断驱动 Modbus 帧解析。

### 帧解析与执行流
1.  **接收**: `SCI_ISR_Rcv_Hook` -> 存入 `MbRcvBuf`。
2.  **定界**: `FTM1 ISR` 检测到 `t3.5` 静默时间 (帧尾) -> 置位 `FrameReadyFlag`。
3.  **校验**: 主循环检测 `FrameReadyFlag` -> CRC16 校验。
4.  **解析 (MbServer)**:
    *   读取 `Function Code` (FC 03, 06, 16)。
    *   读取 `Start Register Address` 和 `Length`。
5.  **执行 (MbHandle)**:
    *   调用 `MbFindAllInfoAddrMap()` 将寄存器地址映射到内部 C 变量。
    *   **FC 03 (读)**: 将内部变量复制到 Modbus 响应缓冲区 -> 发送。
    *   **FC 06/16 (写)**:
        *   检查操作权限 (`CheckAuthLevel`)。
        *   写入内部变量 -> 触发配置生效 (如立即改变电流限制)。
        *   写入非易失存储 (如保存配置) -> 发送响应。

## 6.3 GB/T 27930 (充电机通信)

**文件**: `Sources/Logics/FstChg/ChgTactics.c`

国标直流充电通信协议，严格的状态机流程。

### 状态机流程 (8 阶段)

1.  **物理连接确认 (Physical Connection)**
    *   检测 `CC1/CC2` 电压或 `A+` 电压 (>90V)。
    *   状态: `CHG_STA_PHY`。
2.  **握手阶段 (Handshake)**
    *   BMS 发送 `BHM` (握手报文)。
    *   充电机回复 `CRM` (充电机就绪，最大能力)。
    *   状态: `CHG_STA_HAN`。
3.  **配置阶段 (Configuration)**
    *   BMS 发送 `BRM` (BMS 报文，电池参数) -> `BCP` (电池充电参数总信息) -> `BCL` (充电电流/电压需求)。
    *   充电机回复 `CCS` (充电机充电状态) -> `CML` (充电机充电限制)。
    *   状态: `CHG_STA_CFG`。
4.  **充电中 (Charging)**
    *   周期发送 `BCP`, `BCL` (动态调整需求)。
    *   监控充电机输出 `CCS`。
    *   状态: `CHG_STA_CHG` -> `CHG_STA_CHG_S` (充电中稳定)。
5.  **充电结束 (End)**
    *   满足条件 (SOC=100% 或 单体达到上限) -> BMS 发送 `BCS` (充电完成) 或 `BST` (中止)。
    *   充电机停止输出。
6.  **统计阶段**
    *   BMS 接收充电统计信息。

---

# 第七章：架构设计深度分析

## 7.1 设计哲学与原因分析

本系统的软件架构并非随意堆砌，而是针对 **BMS 系统的特异性要求** 经过深思熟虑的产物。BMS 不同于通用微控制器开发，它具有 **“高实时性 + 高可靠性 + 长生命周期”** 的特征。

### 1. 为什么使用 "中断 + Timer + Task" 的三级混合调度？

*   **纯裸机轮询 (Bare-metal Loop)**:
    *   *缺点*: 难以保证关键任务的执行间隔。如果某个逻辑阻塞，电流积分 (Ah) 的累积会产生巨大误差；Modbus 帧解析会因错过 `t3.5` 时间窗口而丢帧。
*   **纯 RTOS Task**:
    *   *缺点*: 任务切换 (Context Switch) 需要开销。如果为每个 0.1ms 的逻辑创建 Task，系统将被调度器拖垮。且 Task 的唤醒/挂起机制不如中断直接 (ISR -> Task 切换有延迟)。
*   **三级混合架构 (本案方案)**:
    *   **ISR (最底层)**: 处理 **"时间敏感型"** 任务。如电流积分 (1ms 必须触发) 和 Modbus 计时 (0.1ms 刷新)。这些任务逻辑极短，直接在中断执行不仅效率最高，而且保证了时间精度。
    *   **Timer (中间层)**: 处理 **"周期确定型"** 任务。如 SOC 计算、故障滤波、限流计算。利用 Timer 的周期性回调，无需维护复杂的状态机时间片，且代码结构清晰。
    *   **Task (最高层)**: 处理 **"事件驱动/阻塞型"** 任务。如 CAN/UART 的协议解析、Flash/EEPROM 的文件读写。这些操作耗时较长且不可预测，必须放在独立任务中，以免阻塞主循环或中断。

### 2. 为什么使用 "轮询器 (Round-Robin)" 打散 100ms 任务？

在 `Task10ms` 中实现的 10 步轮询器是一个精妙的设计。
*   如果让 10 个 100ms 级任务同时执行，瞬时 CPU 负载可能超过 80%，导致系统响应卡顿，甚至引发看门狗复位。
*   将它们打散到 10 个 10ms 的片段中执行，使系统的 CPU 负载曲线变得**平坦 (Flat)**。这对嵌入式系统的稳定性至关重要。

### 3. 故障诊断为何分为四级 (Maker -> Filter -> SOS -> Protect)？

*   **解耦 "判断" 与 "执行"**: 
    *   `Maker` 只管 "是否越限"，逻辑简单，不易出错。
    *   `Filter` 负责 "抗干扰" (消抖)，避免因为一瞬间的噪声导致系统误动作 (如突然断开接触器引发拉弧)。
    *   `SOS` 负责 "综合评估"，可能多个轻微故障叠加会导致更高级别的保护。
    *   `Protect` 是 "最终执行者"，它只认 SOS 等级。
*   **优势**: 新增一个故障类型，只需在 Maker 中加一行判定代码，无需修改复杂的保护逻辑，极大降低了维护成本。

## 7.2 当前设计的核心优势

1.  **高可靠性与鲁棒性**:
    *   通过 FS6503 SBC 提供硬件级看门狗，软件级通过双 EEP/Flash 备份参数，确保系统在恶劣工况下不"变砖"。
    *   故障滤波机制有效滤除毛刺干扰。

2.  **模块化与可扩展性**:
    *   硬件抽象层 (HAL) 使得更换 ADC/CAN 芯片或更改引脚定义时，上层业务代码零修改。
    *   条件编译 (`PILE_MANAG_EN`, `BMU_CATL_CAN_EN`) 使得一套代码能适应储能、动力、不同客户协议等多种产品线。

3.  **算法精度高**:
    *   电流积分在 1ms 硬件中断中执行，避免了 RTOS 调度抖动带来的累积误差。
    *   采用安时积分 + 开路电压校正 + 温度/倍率补偿的综合算法，SOC 精度更高。

4.  **资源利用率高**:
    *   使用 RTOS 定时器代替独立任务，大幅节省了 RAM 栈空间。
    *   MCU 资源被充分压榨 (3 CAN, 3 UART, 4 SPI, ADC 全开)，体现了对 S32K146 外设的极致利用。

## 7.3 当前设计的缺陷与潜在风险

尽管架构优秀，但在深度代码审查中，仍发现以下隐患：

1.  **ISR 负担过重 (Critical Risk)**:
    *   `ISRCurrCapAddTask()`, `CANLibTxRcvHandleTask_API()` 等放在 1ms 中断中。如果 CAN 总线负载率高，协议处理耗时激增，可能导致 ISR 执行时间超过 1ms 周期，产生 **重入 (Reentry) 或 任务堆积**，导致电流积分频率降低，SOC 漂移。
    *   *改进建议*: 将 CAN 协议栈移出 ISR，仅在中断中置位标志或入队。

2.  **硬编码 (Hard Coding) 严重**:
    *   大量的 Magic Numbers (魔法数字) 散落在代码中 (如 `3600000`, `1.2`, `99.3`)。
    *   Modbus 寄存器映射使用 `switch-case` 结构维护，6500+ 寄存器极其难以维护，极易出现地址冲突。
    *   *改进建议*: 引入脚本工具 (Python) 自动生成头文件和映射表。

3.  **缺乏任务级的看门狗监控**:
    *   目前仅有 FS6503 硬件看门狗。如果某个 RTOS Task 死锁 (如无限等待信号量)，硬件看门狗是检测不到的 (因为 Timer ISR 可能还在跑，喂了 FS6503)。
    *   *风险*: 某个模块 (如 Modbus) 挂死后，系统表面看正常运行，实际已丧失远程监控功能。

4.  **全局变量泛滥**:
    *   模块间依赖大量的 `extern u16 gGxxx[]` 传递数据。
    *   缺乏数据保护机制 (Mutex)，存在并发写冲突的隐患 (虽然目前 Timer 都在同一 Task 上下文，但 Task 与 ISR 之间可能出问题)。

5.  **缺乏自动化测试**:
    *   核心算法 (SOC, SOH, 故障) 未剥离出硬件依赖，无法在 PC 端进行 CTest 单元测试，只能全量烧录测试，开发效率低。

## 7.4 改进路线与重构建议

针对上述缺陷，提出以下改进路线图，可根据项目周期逐步实施：

### 阶段一：降低 ISR 风险 (短期)
*   **操作**: 将 `CANLibTxRcvHandleTask_API()` 移出 `Time_ISR_Hook(eFTM3)`。
*   **实现**: ISR 中仅执行 `CAN_ISR_Rcv_Hook` (数据入队) + 给 Signal。创建一个 `Task1ms` 任务，等待该信号进行处理 CAN 协议逻辑。
*   **受益**: ISR 时间缩短至 20us 以内，彻底杜绝 1ms 中断丢失风险。

### 阶段二：引入软件配置工具 (中期)
*   **操作**: 废除硬编码的 `switch-case` 寄存器映射；使用 Excel/CSV 定义 Modbus 点表。
*   **实现**: Python 脚本读取 Excel，生成 `MbMap.c` 和 `MbMap.h`。
*   **受益**: 避免人工录入错误点表，修改参数无需改代码。

### 阶段三：增加系统健康监控 (中期)
*   **操作**: 引入 **软件看门狗 (Software Watchdog)**。
*   **实现**: 在关键 Task (如 `Task10ms`, `Task50ms`, `Task100ms`) 末尾设置 "心跳标志位"。创建一个监控任务检查这些标志位是否超时更新。若发现某任务卡死，记录错误日志或软复位。
*   **受益**: 能够检测到 Task 级别的死锁。

### 阶段四：算法与硬件解耦 (长期)
*   **操作**: 核心算法 (SOC/SOE/故障) 提取为独立静态库 (`.a`)，输入输出通过纯 C 结构体传递。
*   **实现**: 算法不直接读取 ADC/EEPROM，而是接收 `SensorData_t` 结构体指针。
*   **受益**: 可以在 PC 上构造测试向量 (Mock Data) 进行单元测试，确保算法逻辑绝对正确后再集成到 MCU。

---

# 第八章：总结

**S32K146OFBMU** 是一个功能完备、架构清晰的企业级电池管理系统软件。

1.  **启动严谨**: 经过 6 个阶段的层层递进，确保硬件和软件环境绝对就绪后再进入主循环。
2.  **调度高效**: 采用 ISR 处理微秒/毫秒级硬实时逻辑，Timer 轮询处理百毫秒级软实时算法，Task 处理异步通信与 IO，各司其职。
3.  **算法精准**: 安时积分结合多维校正和消抖机制，提供了可靠的 SOC/SOE 估算和高可用性的故障保护机制。
4.  **扩展强劲**: 优秀的分层设计使得该平台具备了极强的生命力，能够衍生出适应不同客户需求 (如 CATL, GB/T, Modbus) 的多种产品。

尽管在 ISR 负载和代码可维护性上仍有优化空间，但就目前的架构而言，这已经是一个能够胜任工业/车规级应用的成熟方案。

---

# 附录：术语表

| 缩写 | 全称 | 中文释义 | 说明 |
| :--- | :--- | :--- | :--- |
| **BMU** | Battery Management Unit | 电池管理单元 | 本主控板 |
| **BSU** | Battery Slave Unit | 电池从属单元 | 采集从板 |
| **AFE** | Analog Front End | 模拟前端 | LTC6813 等电芯监控芯片 |
| **SBC** | System Basis Chip | 系统基础芯片 | FS6503, 电源+看门狗 |
| **SOC** | State of Charge | 荷电状态 | 电池剩余电量 (0-100%) |
| **SOH** | State of Health | 健康状态 | 电池寿命衰减指标 |
| **ISR** | Interrupt Service Routine | 中断服务例程 | 硬件触发的异步程序入口 |
| **RTOS** | Real-Time Operating System | 实时操作系统 | FreeRTOS, 负责任务调度 |
| **SPI** | Serial Peripheral Interface | 串行外设接口 | 高速全双工同步通信 |
| **CAN** | Controller Area Network | 控制器局域网络 | 汽车级差分总线 |
| **LFP** | Lithium Iron Phosphate | 磷酸铁锂 | 电池类型: LiFePO4 |
| **C-rate** | Capacity Rate | 充放电倍率 | 1C 表示 1 小时充满/放空 |

---
**文档结束**
