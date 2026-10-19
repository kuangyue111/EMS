# S32K146OFBMU 系统运行流程与调度详细说明

---

## 目录

1. [系统启动完整流程（含函数级细节）](#1-系统启动完整流程含函数级细节)
    1.1 [第一阶段：硬件复位与启动代码](#11-第一阶段硬件复位与启动代码)
    1.2 [第二阶段：main() 入口与 BSP 初始化](#12-第二阶段main-入口与-bsp-初始化)
    1.3 [第三阶段：RTOS 调度器启动](#13-第三阶段rtos-调度器启动)
2. [中断处理机制与触发链](#2-中断处理机制与触发链)
    2.1 [FTM1 定时器中断 (0.1ms)](#21-ftm1-定时器中断-01ms)
    2.2 [FTM2 PWM 捕获中断](#22-ftm2-pwm-捕获中断)
    2.3 [FTM3 定时器中断 (1ms)](#23-ftm3-定时器中断-1ms)
    2.4 [CAN 收发与错误中断](#24-can-收发与错误中断)
    2.5 [SCI (UART) 收发中断](#25-sci-uart-收发中断)
    2.6 [SPI 中断](#26-spi-中断)
    2.7 [中断处理完整流程图](#27-中断处理完整流程图)
3. [FreeRTOS 软件定时器任务调度表](#3-freertos-软件定时器任务调度表)
    3.1 [Task2ms 任务](#31-task2ms-任务)
    3.2 [Task5ms 任务](#32-task5ms-任务)
    3.3 [Task10ms 任务（含 10 状态轮询）](#33-task10ms-任务含-10-状态轮询)
    3.4 [Task50ms 任务](#34-task50ms-任务)
    3.5 [Task100ms 任务](#35-task100ms-任务)
    3.6 [Task200ms 任务（含 5 状态轮询）](#36-task200ms-任务含-5-状态轮询)
    3.7 [其他低频任务](#37-其他低频任务)
4. [RTOS 常驻并发任务详解](#4-rtos-常驻并发任务详解)
    4.1 [HigInfoSampleTask 高压采样任务](#41-higinfosampletask-高压采样任务)
    4.2 [CellInfoSampleTask 电芯采样任务](#42-cellinfosampletask-电芯采样任务)
    4.3 [LowADCSampleTask 低压采样任务](#43-lowadcsampletask-低压采样任务)
    4.4 [CAN 发送与接收任务](#44-can-发送与接收任务)
    4.5 [UART 接收任务](#45-uart-接收任务)
5. [核心算法完整运行流程](#5-核心算法完整运行流程)
    5.1 [SOC/SOE 计算完整数据流](#51-socsoe-计算完整数据流)
    5.2 [SOH 健康状态计算](#52-soh-健康状态计算)
    5.3 [故障诊断与保护状态机](#53-故障诊断与保护状态机)
    5.4 [电芯均衡策略](#54-电芯均衡策略)
    5.5 [绝缘检测流程](#55-绝缘检测流程)
    5.6 [电流限流算法](#56-电流限流算法)
6. [系统整体运行逻辑总览](#6-系统整体运行逻辑总览)
    6.1 [数据流总览：从传感器到上位机](#61-数据流总览从传感器到上位机)
    6.2 [系统状态机运行图](#62-系统状态机运行图)
7. [架构设计分析](#7-架构设计分析)
    7.1 [设计哲学与原因](#71-设计哲学与原因)
    7.2 [当前设计的优势](#72-当前设计的优势)
    7.3 [当前设计的缺陷与风险](#73-当前设计的缺陷与风险)
    7.4 [未来改进方向](#74-未来改进方向)
8. [附录：术语表](#8-附录术语表)

---

# 1 系统启动完整流程（含函数级细节）

## 1.1 第一阶段：硬件复位与启动代码

系统上电或复位后，ARM Cortex-M4F 内核首先执行以下操作：

```text
[上电] 
  |
  +---> 1. 硬件自动：
  |       |-- SP (堆栈指针) 从地址 0x00000000 加载初始值
  |       |-- PC (程序计数器) 从地址 0x00000004 加载复位向量（指向 Reset_Handler）
  |
  +---> 2. Reset_Handler (汇编代码)：
  |       |-- 设置堆栈指针 (MSP)
  |       |-- 配置系统时钟（调用 CMSIS SystemInit()）
  |       |-- 复制 .data 段（已初始化全局变量）从 Flash 到 RAM
  |       |-- 清零 .bss 段
  |
  +---> 3. 调用 main() 函数 (C 语言入口)
```

## 1.2 第二阶段：main() 入口与 BSP 初始化

**文件位置**: Sources/main.c

### 详细流程图

```text
main() 函数入口
  |
  +--- [PEX_RTOS_INIT()] -- Processor Expert 内部初始化
  |
  +--- [BSPUserCfgInit()] -- BSP 配置初始化
  |      |-- 将 BSPConfig.h 中的宏复制到运行时结构体 gBSPFunCfg
  |      `-- 配置 s_BSPParaCfg: 晶振/总线频率, 各 CAN/UART/SPI 波特率
  |
  +--- [BSPDriverInit()] -- MCU 外设驱动初始化
  |      |-- (1) WDOG 初始化
  |      |-- (2) CRG/PLL 时钟配置 (16MHz -> 80MHz)
  |      |-- (3) WDOG 喂狗
  |      |-- (4) UART SCI0/1/2 初始化 (19200bps)
  |      |-- (5) FTM1/2/3 定时器初始化
  |      |-- (6) 喂狗
  |      |-- (7) CAN0/1/2 初始化 (500kbps)
  |      |-- (8) 内部 Flash 初始化
  |      |-- (9) 内部 EEPROM 初始化 (4KB)
  |      |-- (10) SPI0/1/2/3 初始化
  |      |-- (11) I2C / PWM / LPIT 初始化
  |      `-- DEVDriverInit() -- 外部设备初始化
  |             |-- DEVGPIOInit(): GPIO 端口/干接点/继电器
  |             |-- DEVADCInit(): ADC 通道
  |             |-- Fs6503Init(): FS6503 SBC 初始化 (看门狗/电源)
  |             |-- DEVRTCRX8010Init(): 外部 RTC
  |             |-- EXTEep24M512Init(): 外部 EEPROM
  |             `-- EXTFlashSST26Init(): 外部 Flash
  |
  +--- [BSP_DISABLE_IRQ()] -- 关闭全局中断 (进入临界区)
  |
  +--- [BSPDisIRQUserInit()] -- 关中断下应用初始化
  |      |-- 参数管理初始化 (读 EEPROM/Flash, CRC 校验)
  |      |-- 电流/绝缘/诊断/系统信息/CAN 库/Modbus 初始化
  |      |-- CellLTC6813Init(): 唤醒 LTC6813, 配置 ADC
  |      `-- HigLVSampleInit(): 配置 LTC2949 采样序列
  |
  +--- [BSP_ENABLE_IRQ()] -- 开启全局中断 (中断正常工作)
  |
  +--- [BSPEnIRQUserInit()] -- 开中断后应用初始化
  |      |-- BootLoaderInit(), 标定加载, 能量计算初始化
  |      |-- StatisticsInit(), SocSoeInit()
  |
  `--- [PEX_RTOS_START()] -- 调用 RtosTaskStart()
```

## 1.3 第三阶段：RTOS 调度器启动

**文件位置**: Sources/OSUser/OSUserHOOK.c -> `RtosTaskStart()`

```text
RtosTaskStart()
  | 创建 11 个 AutoReload 定时器: TIMER2MS ~ TIMER2000MS
  | 创建 7 个 FreeRTOS 任务:
  |   HigInfoSampleTask (Pri 14), CellInfoSampleTask (Pri 18)
  |   LowADCSampleTask (Pri 13), CAN Send Task (Pri 17)
  |   CAN Rcv Task (Pri 15), UART Rcv Task (Pri 16)
  |   PileAllTimer10msTask (Pri 12, 条件编译)
  `-- vTaskStartScheduler() -- 启动调度器, main() 永不返回
```

---

# 2 中断处理机制与触发链

## 2.1 FTM1 定时器中断 (0.1ms)

*   **用途**: 维持 Modbus 协议时序
*   **执行**: `Time_ISR_Hook(eFTM1)` -> `ModbusRcvTickTask()` (更新 t1.5/t3.5 字节间超时计数器)

## 2.2 FTM2 PWM 捕获中断

*   **用途**: 充电枪 CP 信号捕获 (当前代码已注释，保留)

## 2.3 FTM3 定时器中断 (1ms) — 系统心跳

```text
FTM3 ISR
  |-- ISRCurrCapAddTask()      -- 1ms 电流积分 (Ah), 必须中断中执行保证精度
  |-- SysClockCountTask()      -- 系统时钟计数
  |-- MbClientTimerTask()     -- Modbus 客户端状态机
  |-- CANLibTxRcvHandleTask() -- CAN 协议库调度 (多帧/超时/心跳)
  `-- HeartToSlaveMcuTask()   -- KEA128 心跳 (每 100ms 一次)
```

## 2.4 CAN 收发与错误中断

*   接收中断: 从邮箱读帧 -> 写入环形缓冲区 (5 槽) -> 唤醒 CAN Rcv Task
*   发送完成: 邮箱清除 -> 检查待发送队列
*   错误: Bus Off/Error Passive 记录

## 2.5 SCI (UART) 收发中断

*   接收: 字节 -> 接收缓冲区 -> Modbus 帧解析 (t3.5 判定 -> CRC -> 地址匹配 -> 功能码)
*   发送完成: RS485 模式下应切换回接收 (当前代码已注释)

## 2.6 SPI 中断

*   传输完成: 清标志 -> 读数据 -> 通知等待的采样任务

## 2.7 中断与任务调度关系图

```text
  硬件中断                          RTOS 任务/定时器
  ┌──────────────┐                 ┌─────────────────┐
  │ FTM3 (1ms)   │──────电流积分───▶│ Task10ms 轮询   │
  │ 电流/SOC/时钟│                 │ Task50ms 滤波   │
  ├──────────────┤                 │ Task100ms 状态机 │
  │ FTM1 (0.1ms) │──────Modbus─────▶│ Task200ms 标定  │
  │ t1.5/t3.5    │                 └─────────────────┘
  ├──────────────┤
  │ CAN ISR      │──────队列───────▶│ CAN Rcv Task    │
  │ (收发/错误)  │                 │ CAN Send Task   │
  ├──────────────┤                 └─────────────────┘
  │ SCI ISR      │──────字节───────▶│ UART Rcv Task   │
  │ (字节收发)   │                 └─────────────────┘
  └──────────────┘
```

---

# 3 FreeRTOS 软件定时器任务调度表

定时器在 FreeRTOS Timer Task 上下文中执行，优先级 1~11。

## 3.1 Task2ms

```text
Task2ms()
  `-- MbTrigSendTimerTask()  // Modbus 发送触发
```

## 3.2 Task5ms

```text
Task5ms()
  |-- CANLibHandleTask_API()      // CAN 应用层协议
  |-- FS65DiagnosticTask()        // SBC 状态 + 看门狗刷新
  `-- [条件] CATL 协议上行报文
```

## 3.3 Task10ms（含 10 状态轮询）

每 10ms 推进一个状态，100ms 完成一轮：

| sTime | 偏移时间 | 功能 |
|-------|---------|------|
| 每次必执行 | 0/10/.../90ms | GPIO 管理, 端口滤波, CATL/BMM 发送 |
| 0 | 10ms | 限流计算 (CurrLimCalcTask) + CV 降额 |
| 1 | 20ms | 特征检查 (电压/温度一致性, 5s 平均电流) |
| 2 | 30ms | 故障生成: 电压/温度/电流越限 |
| 3 | 40ms | 故障生成: 端口/信息异常 |
| 4 | 50ms | 故障生成: 状态异常 + 故障显示 |
| 5 | 60ms | **SOC/SOE 计算** (容量/能量) |
| 6 | 70ms | **SOH 计算** + 能量显示 |
| 7 | 80ms | PWM 捕获 + **电芯均衡策略** |
| 8 | 90ms | 绝缘检测 + 保护状态 + 事件记录 |
| 9 | 100ms | 接触器状态变化 + 快充消息 + 参数备份 |

## 3.4 Task50ms

```text
Task50ms()
  |-- DiagErrFilterTask()            -- 全局故障滤波 (96+ 错误 ID 状态机)
  |-- SocSoeLibAllTask()             -- SOC 端点校正 + 同步 + 自动校正 + 显示平滑
  |-- GroupSendUpMsg1sBy50ms_API()   -- 1s 广播消息拆分
  `-- [条件] 从控消息检查
```

## 3.5 Task100ms

```text
Task100ms()
  |-- GroupWorkStateTask()           -- 工作状态机 (IDLE/START/RUN/STOP/ERR)
  |-- GroupChgDhgStateTask()         -- 充放电状态判定
  |-- DiagGroupSosTask()             -- SOS 安全等级 (0~4)
  |-- GroupProtectTask()             -- 保护执行 (限流/保护/紧急停机)
  `-- RcdFaultWaveHandleTask()       -- 故障波形记录
```

## 3.6 Task200ms (含 5 状态轮询)

| sTime | 偏移 | 功能 |
|-------|------|------|
| 0 | 200ms | 热管理 (风机 PWM / 加热器) |
| 1 | 400ms | 电压限值计算 (查 14 点温度表) |
| 2 | 600ms | 工作模式检查 (Auto/Hand/Emerg/Debug) |
| 3 | 800ms | 通信超时检查 |
| 4 | 1000ms | **高压/电流标定处理** (K/B 系数计算) |

## 3.7 其他低频任务

| 任务 | 周期 | 功能 |
|------|------|------|
| Task300ms | 300ms | 容量统计, Bootloader, 历史记录落盘 (每 600ms) |
| Task400ms | 400ms | 历史能量统计, KEA128 SPI 通信 |
| Task500ms | 500ms | 外设健康, **快充策略 (GB/T 27930)**, 辅助外设 |
| Task1000ms | 1s | 按键控制, 充放电统计, DOD, Flash 检查 |
| Task2000ms | 2s | **掉电保护**, 故障计数, 时钟记录, RTC/EEPROM (每 4s) |

---

# 4 RTOS 常驻并发任务详解

| 任务名 | 优先级 | 栈 | 功能 |
|--------|--------|-----|------|
| CellInfoSampleTask | 18 (最高) | 300 | LTC6813 电芯采集 (启动→等待→读取→滤波) |
| HigInfoSampleTask | 14 | 400 | LTC2949 高压采集 (10 步序列) |
| LowADCSampleTask | 13 | 120 | 内部 ADC 低压采集 |
| BSPCANSendTask | 17 | 400 | CAN 发送队列处理 |
| CAN Rcv Task | 15 | 400 | CAN 接收解析, 路由分发 |
| UART Rcv Task | 16 | 200 | UART 解析, Modbus 处理 |
| PileAllTimer10msTask | 12 | 400 | 堆级管理 (条件编译) |

---

# 5 核心算法完整运行流程

## 5.1 SOC 计算数据流

```text
FTM3 1ms ISR → ISRCurrCapAddTask()
  读取电流 → K/B 标定 + 温度补偿 → 积分: Delta_Ah += I × 1ms / 3,600,000
  → 充电效率 101.5%, 放电效率 100%

Task10ms Case 5 (60ms) → GroupCapSocCalcTask()
  SOC_raw = nowCap / totalCap × 100% → 温度/C率 2D 查表补偿

Task50ms → SocSoeLibAllTask()
  电压校正 (V_Max 超阈值 → SOC=99.3%, V_Min 超阈值 → SOC=0.13%)
  SOC 同步 (包间偏差 ≤ 3.0%)
  自动校正 (充放电端点测量新容量, 新旧值平均, 80%~130% 校验)
  显示平滑 (限速率 5%/s)
```

## 5.2 故障诊断三级流水线

```text
[第一级: Task10ms Case 2~4, 每 30ms] DiagMaker
  比较采样值与阈值 → 生成 96+ 错误 ID 原始状态

[第二级: Task50ms] DiagErrFilterTask
  A(正常) --[延时]--> B(故障) --[延时]--> A
  恢复限制: 10min 内限制恢复次数

[第三级: Task100ms] DiagGroupSosTask
  16+ 原因加权 → SOS 等级 0(最高安全) ~ 4(最低安全)

[第四级: Task100ms] GroupProtectTask
  限流 → 保护 → 系统保护 → 紧急停机
```

## 5.3 电芯均衡策略

```text
Task10ms Case 7 (每 80ms):
  State 0: 检查前提 (非调试/均衡使能/无错误)
  State 1: 电压偏差积分 (约 300s)
  State 2: 判定方案
    条件1: V_min >= 阈值 → 放电高于最低的
    条件2: V_max >= 平均 + 阈值 → 放电高于平均的
    温度限制: ≥60°C 最多 8 个/板, ≥80°C 全部禁止
  State 3: 通过 CAN 发送均衡命令给 BSU
```

## 5.4 电流限流

```text
Task10ms Case 0 (每 10ms):
  限流 = min(温度降额, SOC 降额, 电压降额)
  CV 降额: 10 级平滑表, 充电末端逐步降流
```

---

# 6 系统整体运行逻辑总览

## 6.1 数据流总览

```text
[传感器] → [RTOS 采样任务] → [算法层] → [记录层] → [通信输出] → [上位机]
LTC6813  → CellInfoSampleTask → SOC/SOE/SOH → Flash 记录 → CAN 广播
LTC2949  → HigInfoSampleTask  → 限流/诊断   → EEPROM    → Modbus
内部 ADC → LowADCSampleTask   → 故障滤波    → 波形记录  → GB/T27930
GPIO     → PortCheckTask      → 保护执行    → 事件记录  → CATL 协议
```

## 6.2 系统状态机

```text
INIT → IDLE → START (预充序列) → RUN → STOP → IDLE
                    ↓                    ↓
                   ERR ←故障/错误←───────┘
                    ↓
        通信超时/欠压 → OFF
```

---

# 7 架构设计分析

## 7.1 设计哲学与原因

### (1) 硬件抽象分层设计 (BSP → Driver → Device → BSPUser)

**为什么这样设计？**

BMS 是典型的工业嵌入式系统，底层硬件变化频繁 (不同项目可能使用不同电芯监控芯片、不同 SBC)，而上层业务逻辑 (SOC/保护/均衡) 基本不变。

分层设计使得：
*   **Driver 层** 只与 S32K146 MCU 硬件有关，不同项目复用。
*   **Device 层** 封装具体芯片的通信协议 (LTC6813/LTC2949/FS6503)，上层只需调用 `DEVxxxInit()` / `DEVxxxRead()` 即可，不关心 SPI 时序。
*   **BSPUser 层** 统一对外接口，应用层代码 `#include "BSPUserAPI.h"` 即可访问所有硬件功能。

### (2) 中断 + 定时器回调 + 常驻任务 三级调度体系

**为什么这样设计？**

传统裸机系统用一个 `main()` 里的 `while(1)` 轮询所有任务，面临两个问题：
1.  电流积分 (Ah 计算) 必须在精确的 1ms 间隔执行，不能因为其他任务耗时而被延迟。
2.  LTC6813 采样需要等待几毫秒的 ADC 转换时间，如果用轮询，整个系统都会被阻塞。

三级调度的设计哲学：
*   **中断 (ISR)** 处理对时间精度要求最高的任务：电流积分 (1ms 累加)、Modbus 字节间超时 (0.1ms)——这些不能被 FreeRTOS 调度器延迟，否则 Ah 计算会漂移、Modbus 会丢帧。
*   **FreeRTOS Software Timer 回调** 处理周期性、确定性执行的业务逻辑：SOC/诊断/保护/均衡等，每个定时器的回调在 Timer Task 中顺序执行，不会出现重入问题。
*   **常驻 Task** 处理包含长延时/阻塞操作的逻辑：LTC6813 采样、CAN/UART 接收队列处理，使用 `OSIF_TimeDelay()` / `OSIF_SemaWait()` 让出 CPU，不浪费资源。

### (3) Task10ms 中的 10 状态轮询设计

**为什么这样设计？**

如果将 SOC 计算、均衡、绝缘、保护等所有 100ms 任务都在 Task10ms 回调中执行，那么：
1.  Task10ms 的执行时间会很长 (可能超过 2ms)，导致回调重叠或丢失。
2.  所有 100ms 任务同时执行 (jitter 大)，不利于诊断滤波的均匀分布。

10 状态轮询将 100ms 内的任务**打散到 10 个 10ms 片段**中执行，每次只做一部分，降低了单次回调的执行时间，也保证了任务之间的执行间隔更均匀。

### (4) 故障诊断三级流水线 (原始状态 → 滤波 → SOS 等级 → 保护执行)

**为什么这样设计？**

直接使用原始采样值判定故障会导致频繁 "抖动" (Flapping)。比如电压在阈值附近波动时，故障状态会反复切换接触器，严重影响系统稳定性，甚至损坏硬件。

三级流水线将故障判定和故障处理完全解耦：
*   **诊断生成** 只负责 "原始比对"，快速、轻量。
*   **滤波** 负责 "去抖动"，引入时间迟滞 (A→B 延时 / B→A 延时) 和恢复次数限制。
*   **SOS** 负责 "综合评估"，将多个故障的严重程度汇总为一个等级。
*   **保护** 只依赖 SOS 等级执行动作，完全不管底层是哪个错误。

这种设计使得：
1.  新增故障类型只需在 DiagMaker 中添加比对条件，不需要修改保护逻辑。
2.  调整滤波时间只需修改配置参数，不需要修改代码。
3.  保护动作集中在一处，便于维护和验证。

## 7.2 当前设计的优势

### 1. 实时性与确定性
1ms FTM3 中断内的电流积分保证了 Ah 计数的时间精度，不受 RTOS 调度抖动影响。这在 BMS 中至关重要——SOC 误差积累直接影响用户体验。

### 2. 分层解耦，可移植性强
*   BSP/Driver 层与 MCU 绑定，Device 层与芯片绑定，Funtion/Logics 层是纯业务逻辑，不依赖任何硬件。
*   更换 MCU 只需重写 BSP/Driver 层，业务代码不动。
*   更换电芯监控芯片 (LTC6813 vs BF8915A) 只需替换 Device 层，SOC/均衡算法不动。

### 3. 资源利用合理
*   使用 FreeRTOS Software Timer 而不是 Task 来做周期任务，Timer 的内存开销远小于 Task (每个 Timer 只需少量控制块 vs 几百字节的 Task 栈)。
*   7 个常驻 Task 都是真正需要阻塞/等待的函数，合理使用了 RTOS 特性。

### 4. 故障处理安全可靠
*   三级诊断流水线 + SOS 安全等级，避免了 "过度保护" 和 "保护不足" 两个极端。
*   滤波时间可配置，恢复次数限制防止频繁切换。

### 5. 数据记录完善
*   8MB 外部 Flash 分区用于故障记录、事件记录、波形记录、运行数据 (576 条/220s 周期)，DOD 记录 (10200 条)。
*   参数三级存储 (ExtEEP → IntEEP → IntFlash)，冗余备份，双扇区轮换 + CRC 校验。

### 6. 条件编译灵活
*   `PILE_MANAG_EN`: 堆级管理按需启用，同一份代码支持单机和堆式两种产品。
*   `BMU_BSU_COM_TYPE`: 支持 BSU 和 BMM 两种从控协议。
*   `BMU_CATL_CAN_EN`: CATL 专有协议按需编译。

## 7.3 当前设计的缺陷与风险

### 1. ISR 中执行了大量非实时逻辑

**问题**：FTM3 中断中调用了 `ISRCurrCapAddTask()`、`SysClockCountTask()`、`MbClientTimerTask()`、`CANLibTxRcvHandleTask_API()`、`HeartToSlaveMcuTask()`。其中 `CANLibTxRcvHandleTask_API()` 处理 CAN 多帧上传/下载、通信超时检测等，逻辑相当复杂。

**风险**：
*   如果某个任务执行超时，会阻塞后续 1ms 中断的处理，影响电流积分精度。
*   ISR 执行时间不可预测 (取决于 CAN 队列长度)，最坏情况可能超过 1ms。
*   当前 S32K146 主频 80MHz，如果 ISR 耗时过长，CPU 利用率会很高。

### 2. Modbus 帧解析在 UART ISR 中完成

**问题**：Modbus 完整帧解析（CRC 校验、地址匹配、功能码解析、寄存器映射查找）在 UART 中断中执行。而 Modbus 有 6500+ 寄存器，映射查找是一个庞大的 `switch/if-else` 结构。

**风险**：字节连续到达时，ISR 执行时间长，可能丢失后续字节。

### 3. 10 状态轮询的时间片分配不均

**问题**：Case 5/6 的 SOC/SOE/SOH 计算耗时 > Case 2 的故障生成，但每个 Case 占用相同的 10ms 时间片。

**风险**：SOC 计算可能在 10ms 内无法完成，导致回调叠加或跳过其他 Case。

### 4. 全局变量泛滥，缺乏数据封装

**问题**：系统中大量使用全局变量 (`extern u16 gGBmuGenPara_102[]` 等) 在各模块间传递数据，缺乏结构化的数据接口定义。

**风险**：
*   模块间耦合度高，难以独立测试。
*   多线程 (Task 和 Timer 回调) 访问同一全局变量时需要关中断保护，容易遗漏导致数据竞争。
*   Bug 难以定位 (修改一个全局变量，可能在任意位置产生副作用)。

### 5. 缺乏统一的错误处理机制

**问题**：错误码散落在 96+ 个 ID 中，诊断生成、滤波、SOS、保护之间缺乏统一的错误传递规范。新增错误需要修改多个文件。

### 6. Timer 回调中使用静态变量保持状态

**问题**：Task10ms/Task200ms 中使用 `static u8 sTime` 维持轮询状态。

**风险**：静态变量不是线程安全的。虽然 FreeRTOS Timer 是单线程回调，但如果需要暂停/重启定时器，状态可能丢失或混乱。

### 7. 缺乏配置管理工具

**问题**：所有参数使用数组索引 (gGxxx_102[])，参数含义需要对照文档查阅。参数增减容易出错。

### 8. 看门狗策略薄弱

**问题**：`WDOG_FUN_EN` 默认关闭，FS6503 SBC 提供安全看门狗，但代码中没有看到完整的双看门狗监控逻辑 (FS6503 看门狗刷新 + 应用层任务监控)。

## 7.4 未来改进方向

### 1. ISR 瘦身 → 将非实时逻辑移出中断

**方案**：将 `CANLibTxRcvHandleTask_API()`、`MbClientTimerTask()` 等从 FTM3 ISR 中移出，改为：
*   FTM3 ISR 只执行 `ISRCurrCapAddTask()` (电流积分) 和系统时钟。
*   设置一个 1ms 标志位，由一个 FreeRTOS Task 轮询该标志执行其他任务。

**优势**：ISR 执行时间确定 (可控制在 50us 内)，电流积分精度提高，系统更可靠。

### 2. Modbus 解析移至 Task

**方案**：UART ISR 只负责将字节放入环形队列，由 `UART Rcv Task` 出队并组装 Modbus 帧、执行 CRC 校验、处理请求。

**优势**：ISR 极短 (只写一个字节到缓冲区)，不再受 6500+ 寄存器映射查找的影响。

### 3. 软件定时器 → 专用 Task

**方案**：将 Task10ms/Task50ms/Task100ms 中的大量逻辑迁移到独立的 FreeRTOS Task 中，按功能分类 (如 `Task_SocCalc`、`Task_Diagnosis`、`Task_Protection`)。

**优势**：
*   每个 Task 可设置不同的优先级，关键任务 (保护) 优先执行。
*   利用 FreeRTOS 的任务间通信机制 (消息队列、信号量) 替代全局变量，提高数据安全性。
*   各 Task 可使用 `vTaskDelay` 独立控制周期，不再依赖全局 Timer 回调。

### 4. 引入数据结构封装

**方案**：将分散的全局变量组织为结构体，通过 API 函数访问 (Getter/Setter)，而非直接读写。

```c
// 当前:
extern u16 gGSoc_61[2];

// 改进:
typedef struct { u16 soc; u16 soh; u16 soe; } t_BmsState;
t_BmsState* BmsState_Get(void);  // 返回只读指针
void BmsState_SetSOC(u16 soc);   // 安全写入
```

### 5. 引入配置生成工具

**方案**：使用 Python/Excel 工具自动生成参数定义头文件和 EEPROM 映射表，替代手工维护数组索引。

### 6. 完善双看门狗策略

**方案**：
*   FS6503 看门狗由 ISR 刷新 (已实现)。
*   增加应用层看门狗：各关键 Task 在规定时间内设置 "心跳标志"，主监控 Task 检测到某 Task 超时则触发安全停机。

### 7. 增加单元测试和自动化验证

**方案**：由于大量算法 (SOC/SOE/SOH/限流/均衡/诊断) 是纯计算逻辑，与硬件解耦，可以移植到 PC 上进行单元测试。引入 CI/CD 流程，每次修改后自动运行测试。

### 8. 内存泄漏检测

**方案**：使用 FreeRTOS 的 `uxTaskGetSystemState()` 定期检查各 Task 的栈使用量，确保栈不溢出。使用堆内存分配检测工具 (`traceALLOC`, `traceFREE`) 检查内存泄漏。

---

# 8 附录：术语表

| 术语 | 说明 |
|------|------|
| BMU | Battery Management Unit，电池管理单元 |
| BSU | Battery Slave Unit，电池从属单元 |
| SOC | State of Charge，荷电状态 (剩余电量百分比) |
| SOE | State of Energy，能量状态 |
| SOH | State of Health，健康状态 |
| SOS | State of Safety，安全状态等级 |
| AFE | Analog Front End，模拟前端芯片 |
| SBC | System Basis Chip，系统基础芯片 (FS6503) |
| ISR | Interrupt Service Routine，中断服务例程 |
| RTOS | Real-Time Operating System，实时操作系统 |
| FTM | FlexTimer Module，NXP 的定时器外设 |
| GPIO | General Purpose I/O，通用输入输出 |
| SPI | Serial Peripheral Interface，串行外设接口 |
| CAN | Controller Area Network，控制器局域网络 |
| UART | Universal Asynchronous Receiver-Transmitter，通用异步收发器 (SCI) |
| NTC | Negative Temperature Coefficient，负温度系数热敏电阻 |
| DOD | Depth of Discharge，放电深度 |
| CV | Constant Voltage，恒压充电 |
| K/B | 标定系数：K=斜率 (Slope)，B=偏置 (Offset) |
| LFC/LFP | 磷酸铁锂 |
| NCM | 镍钴锰三元锂 |
| GB/T 27930 | 中国电动汽车直流充电通信协议国标 |
| CRC | Cyclic Redundancy Check，循环冗余校验 |
| Frock | 工装测试协议 (Factory Rock?) |
| CATL | 宁德时代 (Contemporary Amperex Technology) |

---

*文档版本: V1.0*
*生成日期: 2026-04-20*