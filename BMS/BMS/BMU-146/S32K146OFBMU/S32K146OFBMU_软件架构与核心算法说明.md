# S32K146OFBMU 电池管理系统 软件架构与核心算法说明文档

---

## 目录

1. [项目概述](#1-项目概述)
2. [硬件平台](#2-硬件平台)
3. [软件架构总览](#3-软件架构总览)
4. [初始化流程](#4-初始化流程)
5. [操作系统与任务调度](#5-操作系统与任务调度)
6. [BSPSys 板级支持包](#6-bspsys-板级支持包)
7. [Funtion 功能模块](#7-funtion-功能模块)
8. [Logics 业务逻辑](#8-logics-业务逻辑)
9. [PileSys 电池堆管理](#9-pilesys-电池堆管理)
10. [Addition 附加功能](#10-addition-附加功能)
11. [核心算法详解](#11-核心算法详解)
12. [通信协议](#12-通信协议)
13. [数据记录与存储](#13-数据记录与存储)

---

## 1 项目概述

S32K146OFBMU 是**深圳英飞源技术有限公司（Infypower）** 电能转换事业部 BMS 产品线开发的**储能电池管理系统固件**。

| 项目 | 说明 |
|------|------|
| 项目名称 | BMU-146（Battery Management Unit-146） |
| 目标 MCU | NXP S32K146（ARM Cortex-M4F，1MB Flash，128KB RAM） |
| 主频 | 80MHz（16MHz 外部晶振经 PLL 倍频） |
| 操作系统 | FreeRTOS |
| IDE/代码生成 | Processor Expert 10.1 |
| 最大支持从控数量 | 15 块从控板（BSU） |
| 每从控电池串数 | 14S / 16S / 18S / 52S（可配置） |
| 支持电芯类型 | 磷酸铁锂（LFP）、三元锂（NCM）、钛酸锂（LTO）、铅酸电池 |
| 作者 | SBK326 |
| 创建日期 | 2019-11-01 |

---

## 2 硬件平台

### 2.1 核心芯片

**NXP S32K146**（ARM Cortex-M4F），内部资源：
| 外设 | 数量 | 用途 |
|------|------|------|
| FlexCAN | 3路（CAN0、CAN1、CAN2，500kbps） | 外部通信、从控通信 |
| LPUART (SCI) | 4路（默认3路使用，19200bps） | RS485 通信、Modbus |
| LPSPI | 4路 | FS6503、LTC2949、LTC6813、外部 Flash |
| ADC | 2模块（ADC0、ADC1） | 内部 ADC 采样 |
| FTM | 4路（FTM0~FTM3） | 系统时钟、PWM 捕获 |
| I2C | 2路（硬件，未启用） | - |
| 内部 EEPROM | 4KB | 参数存储 |
| 内部 Flash | 1MB | 代码 + 参数备份 |

### 2.2 关键外围器件

| 器件 | 接口 | 用途 |
|------|------|------|
| **NXP FS6503** | SPI0 | 安全系统基础芯片（SBC），提供电源、安全看门狗、故障输出 |
| **ADI LTC6813** 或 **BF8915A** | SPI2 | 多串电池监控 AFE（12~18 串/片），电压 + 温度采样，被动均衡 |
| **ADI LTC2949** | SPI1 | 高压库仑计，总压、电流、功率、能量精确测量 |
| **MCP3913** | SPI | 6 通道电表 AFE（备选方案） |
| **SST26VF064** | SPI3 | 8MB 外部 SPI Flash，用于大容量数据记录 |
| **M24512** | 模拟 I2C | 64KB 外部 EEPROM，参数持久化存储 |
| **RX-8010SJ** | 模拟 I2C | 外部高精度 RTC 芯片 |
| **KEA128** | SPI3 + GPIO | 从属 MCU，负责部分采样功能 |

### 2.3 SPI 通道分配

| SPI 通道 | 连接设备 |
|----------|---------|
| SPI0 | FS6503 SBC |
| SPI1 | LTC2949 高压库仑计 |
| SPI2 | LTC6813 电芯监控（菊花链） |
| SPI3 | SST26VF064 外部 Flash |

---

## 3 软件架构总览

### 3.1 分层架构图

```
+--------------------------------------------------------------------------+
|                            Application Layer (APP)                        |
|                                                                           |
|  +--------------+  +-------------+  +----------+  +--------------------+ |
|  | Funtion/     |  | Logics/     |  | PileSys/ |  | Addition/          | |
|  | 核心算法库   |  | 业务逻辑    |  | 电池堆管 |  | 标定/升级/协议     | |
|  | SOC/SOE/SOH |  | 启动/停止/  |  | 理系统   |  | 电流/高压标定     | |
|  | 能量/功率   |  | 保护/均衡/  |  | (多组并  |  | CAN Bootloader     | |
|  | 诊断/统计   |  | 充电/事件   |  | 联管理) |  | CATL 协议         | |
|  +--------------+  +-------------+  +----------+  +--------------------+ |
+--------------------------------------------------------------------------+
                                    |
+--------------------------------------------------------------------------+
|                       BSPUser API Layer (BSPUser/)                        |
|  BSPUserAPI.h -- 所有模块 API 声明  |  BSPISRHook -- 中断回调             |
|  BSPUserHook -- 事件 Hook 函数      |  OSUserHOOK -- FreeRTOS 集成        |
+--------------------------------------------------------------------------+
                                    |
+--------------------------------------------------------------------------+
|                   Device Abstraction Layer (Device/)                      |
|  DEVGPIO -- PCB 级 I/O 控制     |  DEVADC -- PCB 级 ADC 通道              |
+--------------------------------------------------------------------------+
                                    |
+--------------------------------------------------------------------------+
|              Peripheral Chip Drivers (Device/*_driver/)                   |
|  FS6503  | LTC6813 | BF8915A | LTC2949 | MCP3913 | M24512 | SST26VF064  |
+--------------------------------------------------------------------------+
                                    |
+--------------------------------------------------------------------------+
|                    MCU Peripheral HAL (Driver/)                           |
|  BSPADC | BSPCAN | BSPFLASH | BSPEEPROM | BSPFTM | BSPGPIO | BSPI2C     |
|  BSPSimI2C | BSPLPIT | BSPMCG | BSPPWM | BSPRTC | BSPIRQ | BSPSPI       |
|  BSPUART | BSPWDOG | BSPDMA                                               |
+--------------------------------------------------------------------------+
                                    |
+--------------------------------------------------------------------------+
|                  Configuration (BSPCfg/)   |   Util (Util/)               |
|  BSPConfig.h/.c -- 编译/运行时配置         |   BSPTypeDef + BSPQueue       |
+--------------------------------------------------------------------------+
```

### 3.2 源码目录结构

```
Sources/
|-- main.c                    -- 程序入口
|-- BSPSys/                   -- 板级支持包 System 层
|   |-- BSPCfg/               -- 编译配置 + 运行时配置
|   |-- BSPUser/              -- 用户 API + Hook 函数
|   |-- Device/               -- 专用芯片驱动
|   |   |-- BF8915A_driver/  -- 国产电芯监控 AFE
|   |   |-- FS65_driver/     -- NXP FS6503 SBC
|   |   |-- LTC2949_driver/  -- 高压库仑计
|   |   |-- LTC6813_driver/  -- 多串电池监控
|   |   |-- M24512_driver/   -- 外部 EEPROM
|   |   |-- MCP3913_driver/  -- 电表 AFE
|   |   |-- RX8010_driver/   -- 外部 RTC
|   |   |-- SST26VF064_driver/-- 外部 SPI Flash
|   |-- Driver/               -- MCU 外设 HAL（17 个模块）
|   |-- Util/                 -- 公共数据类型 + 队列
|-- Funtion/                  -- 功能算法库（13 个子模块）
|   |-- CanLib/               -- CAN 通信协议库
|   |-- ChgCan/               -- 国标 GB/T 27930 快充协议
|   |-- DataCalc/             -- 限制值计算、滤波、特征分析
|   |-- Diagnosis/            -- 故障诊断
|   |-- EnergyCalc/           -- 能量/容量/SOH 计算
|   |-- Measurement/          -- 传感器采样（电压/电流/绝缘）
|   |-- ModbusLib/            -- Modbus RTU 协议
|   |-- Parameter/            -- 参数管理
|   |-- PowerManag/           -- 电源管理（FS6503）
|   |-- Recording/            -- 数据记录
|   |-- SocLib/               -- SOC/SOE 算法
|   |-- Statistics/           -- 统计信息
|   |-- SystemInfo/           -- 系统信息管理
|-- Logics/                   -- 业务逻辑层（6 个子模块）
|   |-- Balance/              -- 电芯均衡策略
|   |-- CanUser/              -- CAN 消息处理
|   |-- EventUser/            -- 事件记录
|   |-- FstChg/               -- 快充策略
|   |-- SciUser/              -- 串口/Modbus 控制
|   |-- Tactics/              -- 核心控制（启动/停止/保护）
|-- PileSys/                  -- 电池堆管理（8 个子模块，条件编译）
|   |-- PileCalc/             -- 堆级计算
|   |-- PileCan/              -- 堆级 CAN 通信
|   |-- PileDiag/             -- 堆级诊断
|   |-- PilePara/             -- 堆级参数
|   |-- PileSci/              -- 堆级串口
|   |-- PileStati/            -- 堆级统计
|   |-- PileTact/             -- 堆级策略
|   |-- PileUser/             -- 堆级用户接口
|-- Addition/                 -- 附加功能（5 个子模块）
|   |-- Calibration/          -- 电流/高压标定
|   |-- LoadApp/              -- CAN Bootloader
|   |-- OperAuth/             -- 操作权限
|   |-- UserDef/              -- 用户自定义（按键/掉电保护）
|   |-- UserProto/            -- 用户协议（CATL / 工装测试）
|-- OSUser/                   -- 操作系统集成
|   |-- OSUserHOOK.c/.h       -- FreeRTOS 任务与定时器
```

---

## 4 初始化流程

main() 执行的初始化顺序：

```c
main()
  |-- PEX_RTOS_INIT()                    -- FreeRTOS 初始化（内部）
  |-- BSPUserCfgInit()                   -- 加载配置宏到运行时结构（gBSPFunCfg、gBSPParaCfg）
  |-- BSPDriverInit()                    -- MCU 外设初始化
  |     |-- 看门狗（如启用）
  |     |-- CRG/PLL（16MHz -> 80MHz 总线）
  |     |-- UART SCI0/1/2（19200 bps）
  |     |-- FTM1/2/3（定时器）
  |     |-- CAN0/1/2（500kbps）
  |     |-- 内部 Flash
  |     |-- 内部 EEPROM
  |     |-- SPI0/1/2/3
  |     |-- I2C0/1（如启用）
  |     |-- PWM
  |     |-- LPIT
  |     |-- DMA（如启用）
  |     |-- DEVDriverInit() -- PCB 设备级初始化
  |           |-- DEVGPIOInit()
  |           |-- DEVADCInit()
  |           |-- Fs6503Init()        -- SBC 初始化
  |           |-- DEVRTCRX8010Init()  -- 外部 RTC
  |           |-- EXTEep24M512Init()  -- 外部 EEPROM
  |           |-- EXTFlashSST26Init() -- 外部 SPI Flash
  |-- BSP_DISABLE_IRQ()                 -- 关闭总中断
  |-- BSPDisIRQUserInit()               -- 关中断状态下应用初始化
  |     |-- 参数存储初始化
  |     |-- LTC6813 初始化
  |     |-- LTC2949 初始化
  |     |-- 电流采样初始化
  |     |-- 绝缘测量初始化
  |     |-- CAN 库初始化
  |     |-- Modbus 初始化
  |     |-- 系统信息初始化
  |     |-- 诊断初始化
  |-- BSP_ENABLE_IRQ()                  -- 开启总中断
  |-- BSPEnIRQUserInit()                -- 开中断后应用初始化
  |     |-- BootLoader 初始化
  |     |-- 电流/电压标定初始化
  |     |-- 能量计算初始化
  |     |-- 统计初始化
  |     |-- SOC/SOE 算法初始化
  |-- PEX_RTOS_START()                  -- 启动 FreeRTOS 调度器
```

---

## 5 操作系统与任务调度

### 5.1 FreeRTOS 定时器

系统使用 11 个软件定时器（自动重载）作为任务调度的基准时钟：

| 定时器 | 周期 | 优先级 |
|--------|------|--------|
| Timer 2ms | 2ms | 1 |
| Timer 5ms | 5ms | 2 |
| Timer 10ms | 10ms | 3 |
| Timer 50ms | 50ms | 4 |
| Timer 100ms | 100ms | 5 |
| Timer 200ms | 200ms | 6 |
| Timer 300ms | 300ms | 7 |
| Timer 400ms | 400ms | 8 |
| Timer 500ms | 500ms | 9 |
| Timer 1000ms | 1s | 10 |
| Timer 2000ms | 2s | 11 |

### 5.2 FreeRTOS 任务

| 任务名 | 优先级 | 栈大小 | 用途 |
|--------|--------|--------|------|
| PileAllTimer10msTask | 12 | 400 words | 电池堆管理（条件编译） |
| LowInfoSampleTask | 13 | 120 words | 低压 ADC 采样 |
| HighInfoSampleTask | 14 | 400 words | 高压 ADC 采样 |
| CAN Rcv Task | 15 | 400 words | CAN 接收处理 |
| UART Rcv Task | 16 | 200 words | UART 接收处理 |
| CAN Send Task | 17 | 400 words | CAN 发送处理 |
| CellInfoSampleTask | 18 | 300 words | 电芯采样（菊花链模式） |

### 5.3 定时器回调任务调度表

| 周期 | 主要执行任务 |
|------|-------------|
| **0.1ms** | Modbus 接收计时 |
| **1ms** | 电流积分累加中断（ISRCurrCapAddTask）、Modbus 客户端计时 |
| **2ms** | Modbus 触发发送 |
| **5ms** | CAN 库处理、FS6503 诊断 |
| **10ms** | 端口状态检查、10 状态轮询（限流计算、特征检查、故障滤波、SOC 计算、均衡等） |
| **50ms** | 故障滤波、SOC/SOE 库算法、上报消息发送 |
| **100ms** | 工作状态/充放电状态/保护/SOS 等级/电芯计算/SOH 计算/特征检查/电压均衡/能量计算/事件记录 |
| **300ms** | 循环容量统计、Bootloader Flash 擦除处理、历史数据落盘 |
| **400ms** | 历史容量/能量统计、从属 MCU SPI 地址同步 |
| **500ms** | 外设检查、快充策略、辅助外设管理 |
| **1s** | 外设策略管理、按键控制、充放电次数统计、DOD 统计、外部 Flash 检查、绝缘检查 |
| **2s** | 掉电保护、故障计数统计、系统时钟记录、外部 RTC & EEPROM 轮询检查 |

### 5.4 硬件中断

| 中断源 | 用途 |
|--------|------|
| FTM1（0.1ms） | Modbus 时序维持 |
| FTM2 | PWM 输入捕获 |
| FTM3（1ms） | 系统时钟计数、电流积分、Modbus 客户端、CAN 发送/接收、从属 MCU 心跳（每 100ms） |
| CAN0/1/2 | CAN 收发 + 错误中断 |
| UART0/1/2 | UART 收发中断 |
| SPI0/1/2/3 | SPI 中断 |

---

## 6 BSPSys 板级支持包

### 6.1 BSPCfg -- 配置模块

**功能**：所有驱动功能的编译时宏配置和运行时初始化。

**关键配置宏**：

| 宏 | 默认值 | 说明 |
|----|--------|------|
| FTM0_FUN_EN | 0 | 定时器 0（保留） |
| FTM1_FUN_EN | 1 | 定时器 1（用户定时器，0.1ms 中断） |
| FTM2_FUN_EN | 1 | 定时器 2（PWM 捕获） |
| FTM3_FUN_EN | 1 | 定时器 3（用户定时器，1ms 中断） |
| CAN0/1/2_FUN_EN | 1 | 3 路 FlexCAN（500kbps） |
| SCI0/1/2_FUN_EN | 1 | 3 路 UART（19200bps） |
| SPI0/1/2/3_FUN_EN | 1 | 4 路 SPI |
| ADC_FUN_EN | 1 | ADC 模块 |
| PWM_FUN_EN | 1 | PWM 输入捕获 |
| WDOG_FUN_EN | 0 | 看门狗 |
| LPIT_FUN_EN | 1 | 低功耗定时器 |

**运行时数据结构**：

- `u_BSPFunCfg` -- 联合位域，28 个驱动开关标志（压缩在 u32 中）
- `s_BSPParaCfg` -- 运行时参数结构体（晶振频率、总线频率、波特率等）

### 6.2 BSPUser -- 用户接口层

这是应用层调用的**公共 API 接口**，将底层驱动函数重新导出为 BSPxxx / DEVxxx 命名。

**主要 API 分类**：

| 类别 | 示例 API |
|------|---------|
| CAN | BSPCANInit、BSPCANSendMsg、BSPCANSendQin |
| UART | BSPSCIInit、BSPSCISendDataOnAsk |
| 内部 EEPROM | BSPINTEEPWriteByte、BSPINTEEPReadByte |
| 外部 EEPROM (M24512) | DEVEXTEEPWriteByte、DEVEXTEEPReadByte |
| 内部 Flash | BSPINTFLASHEraseSector、BSPINTFLASHWriteByte |
| 外部 Flash (SST26VF064) | DEVEXTFLASHWriteByte、DEVEXTFLASHEraseSector |
| GPIO | DEVGPIOSetPortState、DEVGPIOGetInPortState |
| ADC | DEVADCGetSampAD |
| PWM | BSPReadPWMfrequencyAPI |
| RTC | BSPRTCGetCurrSecTime、DEVEXTRTCGetNowClockAPI |
| 看门狗 | BSPWatchDogInit、BSPWatchDogTask |

### 6.3 Device -- 专用芯片驱动

| 驱动文件 | 用途 | 行数 |
|---------|------|------|
| BF8915A_driver | 国产电芯监控 AFE（最高支持 20 片菊花链，16 通道） | 2165 行 |
| FS65_driver | NXP FS6503 SBC（安全电源、看门狗、故障输出） | 1301 行 |
| LTC2949_driver | 高压库仑计（电流、功率、能量精确测量） | 3178 行 |
| LTC6813_driver | 多串电池监控（最高 12 串 <1.2mV 误差） | 2309 行 |
| MCP3913_driver | 6 通道电表 AFE | 436 行 |
| M24512_driver | 64KB I2C EEPROM（128 字节页写入） | 614 行 |
| RX8010_driver | 外部 RTC 芯片 | 319 行 |
| SST26VF064_driver | 8MB SPI Flash（带 CRC 校验） | 1794 行 |

**外部 Flash 分区表（SST26VF064，8MB）**：

| 区域 | 地址范围 | 容量 | 用途 |
|------|---------|------|------|
| 区域 1 | 0x000000 - 0x0FFFFF | 1MB | 启动程序区 1 |
| 区域 2 | 0x100000 - 0x1FFFFF | 1MB | 启动程序区 2 |
| 区域 3 | 0x200000 - 0x2FFFFF | 1MB | 累计故障记录 |
| 区域 4 | 0x300000 - 0x3FFFFF | 1MB | 累计事件记录 |
| 区域 5 | 0x400000 - 0x4FFFFF | 1MB | 累计数据记录 |
| 区域 6 | 0x500000 - 0x6FFFFF | 2MB | 累计运行数据记录 |
| 区域 7 | 0x700000 - 0x70FFFF | 64KB | DOD 记录 |
| 区域 8 | 0x710000 - 0x7EFEFF | 960KB | 保留 |
| 区域 9 | 0x7FF000 - 0x7FFFFF | 4KB | CRC 校验 |

### 6.4 Driver -- MCU 外设 HAL

提供底层 MCU 外设的硬件抽象层封装：

| 模块 | 行数 | 功能 |
|------|------|------|
| BSPADC | 287+126 | ADC0/ADC1（2 模块，端口 PA/PB/PC，0-15 通道） |
| BSPCAN | 865+176 | FlexCAN（3 路 CAN，收发邮箱，5 槽环形接收缓冲区） |
| BSPEEPROM | 383+97 | MCU 内部模拟 EEPROM（4KB @ 0x14000000） |
| BSPFLASH | 696+170 | MCU 内部 Flash（1MB，6 区布局） |
| BSPFTM | 215+104 | FlexTimer（4 定时器） |
| BSPGPIO | 279+128 | 原始 GPIO 抽象（端口 A-E，16 引脚/端口） |
| BSPI2C | 150+77 | 硬件 I2C（2 通道） |
| BSPLPIT | 148+89 | 低功耗定时器（2 通道） |
| BSPPWM | 172+93 | PWM 输入捕获（FTM） |
| BSPRTC | 142+68 | 内部 RTC（需 32kHz 外部晶振） |
| BSPSimI2C | 815+188 | 模拟 I2C（GPIO 位操作，支持多通道） |
| BSPSPI | 454+87 | LPSPI（4 通道，16 位，主机模式） |
| BSPUART | 382+111 | LPUART（4 通道，中断接收 + 轮询发送） |
| BSPWDOG | 121+59 | 看门狗定时器 |

---

## 7 Funtion 功能模块

### 7.1 SocLib -- SOC/SOE 算法库

**核心文件**：CapEnerCorr、SocInterface、SocSoeCorr、SocSoeSync、SocUserAPI

#### SOC 计算原理

```
SOC = (当前可用容量 / 总容量) × 100%
```

采用**安时积分法（库仑计数）**为主，**电压校正**为辅的混合算法。

#### SOE 计算原理

```
SOE = (当前可用能量 / 总能量) × 100%
```

采用**瓦时积分法**，追踪充放电能量。

#### SOH 计算原理

```
SOH = f(衰减容量 / 额定容量, 衰减循环次数 / 额定循环次数)
```

#### 自动校正算法（CapEnerCorr）

| 校正类型 | 触发条件 |
|---------|---------|
| 高压校正（VHP） | 充电到达电压保护限 |
| 低压校正（VLP） | 放电到达欠压限 |
| 温度校正 | 基于 2 维查表（温度 × C 率） |
| 电流校正 | 基于 2 维查表（温度×C 率）补偿 |

**温度校正表**：
- 温度范围：-30°C ~ +60°C（10 个温度点，步长 10°C）
- C 率范围：0.2C ~ 2.0C（10 个 C 率点，步长 0.2C）
- 公式：`校正后容量 = 原始容量 × 10000 / (温度百分比 × 电流百分比)`

**有效性校验**：新测量值必须在前次测量值的 80%~130% 范围内，否则标记 `sCapEnerDropExpFlag`。

**平滑处理**：新容量 = (测量值 + 旧值) / 2（首次测量采用 20:12 加权）。

**充放电效率系数**：
- 容量充电效率 = 101.5%
- 能量充电效率 = 95.5%

#### SOC/SOE 同步算法（SocSoeSync）

| 同步类型 | 周期 | 容差 |
|---------|------|------|
| 包内同步 | 10s | 保证 SOE 偏差不超过 SOC |
| 包间同步 | 500ms | SOC 偏差 <= 3.0%，SOE 偏差 <= 3.0%，电压偏差 <= 30mV |

#### 显示平滑

- SOC 缓变速率：5%/s
- SOE 缓变速率：5%/s
- 支持电芯化学特性电压平台检测（LFP/NCM/LTO/铅酸）

#### 运行周期

| 任务 | 周期 |
|------|------|
| SocSoeLibAllTask() | 50ms |
| SocSoeAllCorrTask() | 500ms |
| TotalCapEnerCorrTask() | 1s |

### 7.2 EnergyCalc -- 能量/容量/SOH 计算

**核心文件**：CapInfoCalc、CellFadeCalc、CurrIntegral、EnerInfoCalc、EnerInterface

#### 数据结构

```c
// 容量结构
t_CAPFORM { nowCap, remCap, baseCap, topCap, standCap }  // 单位: 1mAH

// 能量结构
t_ENERFORM { nowEner, remEner, baseEner, topEner, standEner }  // 单位: 1WH

// 容量积分
t_CAPINT { chgCap10ma1ms, dhgCap10ma1ms, chgCap1ma1h, dhgCap1ma1h }

// 能量积分
t_ENERINT { chgEner1w1ms, dhgEner1w1ms, chgEner1w1h, dhgEner1w1h }

// 衰减（SOH）
t_FADEFORM { fadeCap, ratedCap, fadeCycle, ratedCycle }  // 容量单位: 0.1AH
```

#### 安时积分算法（CurrIntegral）

- <10ms 周期充/放电积分
- 单位换算：1WH = 3,600,000 W·ms

#### SOH 计算（CellFadeCalc）

结合容量衰减和循环次数衰减综合计算。

#### 运行周期

| 任务 | 周期 |
|------|------|
| GroupCapSocCalcTask() | 100ms |
| GroupEnerSoeCalcTask() | 100ms |
| GroupFadeSohCalcTask() | 100ms |

### 7.3 Measurement -- 传感器采样模块

**核心文件**：CellAFESamp、CurrSample、HigAFESamp、InsuCheck、LowADCSamp、PortCheck

#### 电芯电压/温度采集（CellAFESamp）

- 支持 LTC6813 AFE（最高 18 串/片，20 片菊花链）
- 4 点滑动平均滤波
- 节点补偿电压：23~25 (0.1mV)
- 温度查表：167 项（-41°C ~ +126°C）

#### 电流采样（CurrSample）

**双通道设计**：
1. **霍尔传感器**（内部 ADC）
   - AD 范围：249~3890（对应 0.3V~4.7V）
   - 温度补偿霍尔读数
2. **分流器**（外部 AFE，通过 LTC2949）
   - 范围：-90mV ~ +90mV
   - 分辨率：10uV

- 自动零点校准（`CURR_AUT_ZERO_EN=1`）
- 电流分辨率：10mA
- 中断累加周期：1ms

**采样流程**：
- LTC2949 10 步采样序列：分流器 V -> 总压 -> 负载 V -> 母线 V -> 绝缘 V -> PGND V -> 温度 -> 参考 -> 5V

#### 绝缘检测（InsuCheck）

**有限状态机**：Init -> 选择 V1 -> 采样 V1 -> 选择 V2 -> 采样 V2 -> OK -> Diag OK

**原理**：通过两个已知电阻（R1 = 20kΩ、R2 = 2200kΩ）切换测量正负端对地绝缘电阻。

**告警线**：3000kΩ（正/负侧无效阈值）

**接触器粘连检测**：
- 母线+ 参考电压：3.150V
- 母线- 参考电压：2.973V

#### 低压 ADC 采样（LowADCSamp）

- 采样通道：霍尔信号、电源电压、ACC、霍尔 5V、PWM 电压、环境/电源温度、高驱电流反馈
- 7 点滑动平均滤波
- 温度查表：167 项

#### 端口检查（PortCheck）

- 周期：10ms
- 监测：启动按钮、隔离开关、烟感、急停、熔断器、防雷、火警/燃气浓度、绝缘报警

#### 运行周期

| 任务 | 周期 |
|------|------|
| PortStateCheckTask() | 10ms |
| InsuAdhesMeasurTask() | 100ms |
| PwmInfoCaptureTask() | 100ms |
| CellInfoSampleTask() | RTOS 任务 |
| HigInfoSampleTask() | RTOS 任务 |
| LowADCSampTask | RTOS 任务 |

### 7.4 DataCalc -- 数据计算（限流/滤波/特征）

**核心文件**：CalcInterface、CalcUtil、CharactCalc、CurrLimCalc、CurrLimFall、VoltLimCalc

#### 电流限流计算（CurrLimCalc）

基于以下因素计算充/放电电流和功率限制：
- 温度降额表（15 个温度点，来自 `gEepCfg_105[]`）
- SOC 限流
- 电芯电压接近限值的降额

输出：`GetGChgLimCurrAPI()`、`GetGDhgLimCurrAPI()`、`GetGChgLimPowerAPI()`、`GetGDhgLimPowerAPI()`

#### CV 降额（CurrLimFall）

在充放电末端使用平滑降额：
- 充电 CV 控制：当最高电芯电压超过目标值时，通过 10 级 SLOW_CURRLIMIT_NUM 逐步降流
- 每级定义目标电压偏移和初始电流百分比
- 使用 PI 控制器进行电压稳压控制

#### 电压限流计算（VoltLimCalc）

基于温度表（14 个点，来自 `gEepCfg_110[]`）计算充/放电电压限制。

#### 特征计算（CharactCalc）

- 电压特征检查（`CheckVoltCharaTask()`，100ms）
- 温度特征检查（`CheckTempCharaTask()`，100ms）
- 平均电流计算（5 秒窗口）
- 电流滤波（200 点缓冲区 `CUR_FIT_LEN=200`）

#### PI 控制器

```c
typedef struct {
    s32 piIn, piInBak, piOut, piSum;  // PI 状态
} t_PIIO;

typedef struct {
    s32 piKp, piKpR, piKi, piKiR;       // 系数
    s32 piInMax/Min, piSumMax/Min, piOutMax/Min;  // 限幅
} t_PICfg;
```

#### 运行周期

| 任务 | 周期 |
|------|------|
| 限流计算 | 100ms |
| 电压限流计算 | 1s |
| 特征检查 | 100ms |

### 7.5 Diagnosis -- 故障诊断

**核心文件**：DiagCheck、DiagFilter、DiagMaker、DiagSos、DiagUser

#### 错误 ID 系统（96+ 种错误）

| ID 范围 | 级别 | 示例 |
|---------|------|------|
| 0-15 | 2 级（最严重） | 充电停止、放电停止、系统极限（eERR_ID_CEL_VH2 电芯过压 L2） |
| 16-31 | 1 级 | 充电保护、放电保护（eERR_ID_CEL_VH1 电芯过压 L1） |
| 32-47 | 告警/显示类 | SOC 错误、容量错误、充放电超时、均衡温度异常 |
| 48-63 | 从控/内部/通信故障 | 均衡温度错误、电芯离线、内部通信丢失 |
| 64-79 | 内部硬件错误 | RTC 错误、EEPROM 错误、Flash 错误、电流采样错误 |
| 80-96 | 外部环境故障 | 接触器状态故障、火警、熔断器熔断、烟感 |

#### 故障滤波算法（DiagFilter）

**状态机**：
```
A（正常） --[AtoBTime]--> B（故障） --[BtoATime]--> A
```

- 滤波分辨率：`DIAG_FILTER_TICK = 50ms`
- 每个错误可配置不同的滤波时间

#### 故障生成（DiagMaker）

周期 100ms，生成原始错误状态：
- 电压限越限、温度限越限、电流限越限
- 端口异常、信息异常、状态异常

#### SOS 安全等级系统（DiagSos）

| 等级 | 说明 |
|------|------|
| 0 | 最高安全等级 |
| 1~3 | 逐渐降低 |
| 4 | 最低安全等级（锁定） |

触发因素（16+ 种）：
- 故障/告警、电压极值、电压偏差、温度极值
- 容量衰减、DOD 不足、充放电效率不足

#### 运行周期

| 任务 | 周期 |
|------|------|
| DiagErrFilterTask() | 50ms |
| DiagGroupSosTask() | 100ms |
| DiagCommuCheckTask() | 1s |
| DiagPerDevCheckTask() | 500ms |
| DiagErrDisplayTask() | 100ms |

### 7.6 Statistics -- 使用统计

**核心文件**：DodRcdStatis、EnergyStatis、StaInterface、StatisUser、TimesStatis

#### 能量/容量统计

| 任务 | 功能 | 周期 |
|------|------|------|
| GroupCycleCapStatisTask | 单次循环充放电容量统计 | 300ms |
| GroupHisCapStatisTask | 累计历史充放电容量 | 400ms |
| GroupHisEnerStatisTask | 累计历史充放电能量 | 400ms |

EEPROM 更新阈值：1AH（容量）、0.1kWh（能量）。

#### DOD（放电深度）记录

- 按次记录放电深度的统计
- 历史显示：50 条
- 周期：1s

#### 时间/次数统计

| 任务 | 功能 | 周期 |
|------|------|------|
| GroupChgTimesStatisTask | 充放电循环计数 | 2s |
| GroupAllowTimeStatisTask | 允许充放电时间累计 | 2s |
| GroupErrNumStatisTask | 故障/告警计数 | 2s |

循环判定：时间阈值 600s 为一个循环。

### 7.7 Recording -- 事件与故障数据记录

**核心文件**：RecordHandle、RecordInterface、RecordUser、RecordWave

#### Flash 存储组织

| 区域 | 容量 | 用途 |
|------|------|------|
| 故障记录 | 1MB（4K 状态 + 1M 数据） | 约 400 条/天 × 50 天 |
| 事件记录 | 1MB | 类似故障记录 |
| 故障波形记录 | 1MB | 37 个扇区，约 222 条 |
| 运行数据记录 | 2MB | 220s 周期，约 576 条 |
| DOD 记录 | 512MB | 10200 条 |

#### 波形记录（RecordWave）

**触发条件**：电芯电压高/低、总压高/低、温度高/低、电流过载

**状态机**：Wait -> Trigger -> Recording -> Finished

**捕获周期**：
- 电芯电压：2s
- 电流：1s

记录触发前后的波形数据。

#### 运行周期

| 任务 | 周期 |
|------|------|
| RcdStoreHandleTask() | 600ms |
| RcdFaultWaveHandleTask() | 100ms |

### 7.8 Parameter -- 配置参数管理

**核心文件**：GroupPara、ParaCopy、ParaDnload、ParaExtEep、ParaIntEep、ParaInterface

#### EEPROM 地址映射

| 地址范围 | 用途 |
|---------|------|
| 0-50 | 系统配置参数 |
| 50-100 | 启动参数 |
| 100-300 | 系统硬件参数 |
| 300-500 | 标定 KB 值 |
| 500-650 | 系统时钟和内部值 |
| 650-700 | 容量/能量结构 |
| 700-800 | 统计值 |
| 800-1000 | 系统公共参数 |
| 1000-2000 | 设备参数 |

#### 存储架构

```
外部 EEPROM -> 内部 EEPROM -> 内部 Flash（冗余备份）
```

- 双扇区轮换
- CRC 保护所有参数块
- 参数支持 U8/U16/U32 格式，有效性标志为 0x5A / 0x5A5A / 0x5A5A5A5A

#### 重要参数组

| 参数组 | 内容 |
|--------|------|
| gGBmuGenPara_102 | BMU 通用参数 |
| gGBmuHigLevPara_103 | BMU 高级参数 |
| gGroupParaRO_119 | 电压/SOC 平滑降额表 |
| gEepCfg_105 | 温度降额充/放电率表（15 项） |
| gEepCfg_110 | 温度-电压限表（14 项） |

### 7.9 CanLib -- CAN 通信协议库

**核心文件**：CANHandle、CANInterface、CANTabOper、CANTxCycle、CANUser

#### 功能码

| 功能码 | 含义 |
|--------|------|
| VFT | 心跳 |
| BRE | 广播 |
| RUP | 读取 |
| AUP | 确认修改 |
| CUP | 确认修改 |
| REQ | 请求 |
| ARQ | 确认查询 |
| UPM | 上传多帧 |
| UPP | 上传参数 |

#### 地址格式

源地址 + 目标地址 + 消息序号 + 功能码

#### 运行周期

| 任务 | 周期 |
|------|------|
| CANLibHandleTask_API() | 5ms |
| CANLibTxRcvHandleTask_API() | 1ms |

### 7.10 ModbusLib -- Modbus RTU 协议

**核心文件**：MbClient、MbHandle、MbInterface、MbQueue、MbServer、MbTriSend

#### 功能码

| FC | 说明 |
|----|------|
| 0x01 | 读线圈 |
| 0x02 | 读离散输入 |
| 0x03 | 读保持寄存器 |
| 0x04 | 读输入寄存器 |
| 0x05 | 写单个线圈 |
| 0x06 | 写单个寄存器 |
| 0x10 | 写多个寄存器 |

#### 客户端状态机

Leisure -> SendReq -> RspWait -> RspParseAndPro -> FrameSpaceSet

帧时序：t1.5/t3.5 字符间超时（基于波特率自动计算）

#### 运行周期

| 任务 | 周期 |
|------|------|
| ModbusRcvTickTask() | 0.1ms |
| MbClientTimerTask() | 1ms |
| MbTrigSendTimerTask() | 2ms |

### 7.11 PowerManag -- 电源管理（FS6503 SBC）

**核心文件**：Fs6503_PowerManag

#### 功能

- 看门狗刷新：512ms 窗口，384ms（开窗 3/4 处）刷新
- 长延时定时器（LDT）：5 秒运行后延时、3 秒唤醒延时
- SPI 通信超时：5ms
- 用户关机标志控制（优雅关机）
- RTOS 诊断定时器：500ms 系统健康检测

### 7.12 SystemInfo -- 系统信息管理

**核心文件**：GroupInfo、PileInfo、PublicInfo

#### 工作状态枚举

```c
// 保护状态
e_ProState { ChgLim, DhgLim, ChgPro, DhgPro, SysPro, SysErr 等 }

// 工作状态
e_WorkState { Init, Idle, Run, Start, Stop, Err, Off }

// 电流状态
e_CurrState { Idle, Charge, Discharge }

// 工作模式
e_WorkMode { Auto, Hand, Emerg, Debug }

// 错误严重等级
0=Normal -> 1=Warning -> 2=ChgLimit -> 3=DhgLimit -> 4=SysLimit
-> 5=ChgPro -> 6=DhgPro -> 7=SysPro -> 8=SysErr
```

---

## 8 Logics 业务逻辑

### 8.1 Balance -- 电芯均衡策略

**核心文件**：BalanTactic

#### 控制策略（4 状态机）

周期：100ms

| 状态 | 功能 |
|------|------|
| State 0 | 前提条件检查（非调试模式、均衡使能、无通信断开、无采样异常） |
| State 1 | 积分计时（累计电压偏差数据，每 ~300s 有效数据） |
| State 2 | 判定（决定哪些电芯需要均衡） |
| State 3 | 更新结果 |

#### 均衡决策（被动放电电路）

**三种触发条件**：

1. **最低电芯电压 >= 阈值**：放电高于最低电压的电芯
2. **平均电压 >= 阈值 且 最高电压 >= 平均 + 阈值**：放电高于平均的电芯
3. **平均 >= 阈值 且 最低 >= (VL2Lim + 80mV)**：放电高于最低电压的电芯

#### 均衡电压阈值（不同化学体系）

| 化学类型 | 阈值 |
|---------|------|
| NCM（三元） | 3600mV |
| LFP（磷酸铁锂） | 3100mV |
| LTO（钛酸锂） | 1900mV |
| 铅酸 | 2500mV |

#### 安全特性

- **温度保护**：从控板温度 >= 80°C，全部禁止均衡
- **高温限流**：温度 >= 60°C，每从控最多 8 个电芯均衡（按电压偏差大小冒泡排序，优先均衡偏差最大的）
- **通信故障处理**：采样或通信异常时广播"关闭所有均衡"指令
- **SOC 内阻测量**：在 SOC >= 85% 且空闲 >= 144 小时时触发

### 8.2 CanUser -- CAN 消息处理

**核心文件**：GroupMsg（BMU）、SlaveMsg（从控）、GTBmmMsg（GT BMM 协议）、AuxDevMsg（辅助器件）

#### BMU CAN 消息处理

| 索引 | 功能 | 权限要求 |
|------|------|---------|
| 2 | 设备序列号配置 | 管理员 |
| 22 | 下载参数读写 | 管理员 |
| 31-37 | 故障/事件/波形/DOD 数据记录查询 | 操作员 |
| 39 | 历史记录读写 | 多级权限 |
| 85 | 权限密码 | 用户/操作员/服务/管理员 |
| 86 | 系统控制命令 | 非用户 |
| 87 | 用户控制命令 | 多级 |
| 88 | 调试命令 | 管理员 |
| 105-109 | 参数配置 | 多级 |

#### 上报消息（50ms 周期，1 秒轮询广播）

| 消息 | 内容 | 周期 |
|------|------|------|
| Msg 50 | 系统状态 | 500ms |
| Msg 51 | 充放电限制 | 500ms |
| Msg 60 | 系统主信息 | 500ms |
| Msg 61 | 容量（SOC/SOH） | 1s |
| Msg 92/93 | 电芯电压（分页，最高 240 串） | 500ms |
| Msg 94/95 | 电芯温度 | 1s |
| Msg 90/91 | 充放电均衡状态 | 1s |

#### 从控通信支持两种模式

- `BMU_BSU_COM_TYPE = 0`：BSU 通信（SlaveMsg.c）
- `BMU_BSU_COM_TYPE = 1`：BMM 协议（GTBmmMsg.c）

### 8.3 EventUser -- 事件记录

**核心文件**：EventUser、RcdDisplayUser

#### 监测的事件（100ms 周期）

| 序号 | 事件 |
|------|------|
| 1 | 系统上电（记录电流、总压、预充电压、SOC） |
| 2 | 保护状态变化（带前后状态） |
| 3 | 工作状态变化（Idle/Run/Start/Stop/Err） |
| 4 | 充放电状态变化（30min 防抖） |
| 5 | 工作模式变化 |
| 6 | 输入端口状态变化 |
| 7 | 输出端口状态变化 |
| 8+ | 充放电限流变化、母线电压变化、AC 状态、卷帘门等 |

#### 事件分类

| 分类码 | 说明 |
|--------|------|
| 5 | 输入端口变化 |
| 6 | 输出端口变化 |
| 7 | 状态变化 |
| 8 | 接收到的控制命令 |
| 9 | 接收到的调试命令 |
| 10 | 接收到的参数配置 |

### 8.4 FstChg -- 快充策略（GB/T 27930）

**核心文件**：ChgTactics

#### 8 阶段状态机（500ms 周期）

| 阶段 | 说明 | 退出条件 |
|------|------|---------|
| Stage 0 (Idle) | 空闲，监测充电枪插入（FC>=90V 或收到 CHM） | 枪插入 |
| Stage 1 (Link) | 物理连接建立（CC2 约 2.5V） | 进入握手 |
| Stage 2 (Handshake) | 等待 CRM（充电机就绪）和 CBRM（BMS 就绪确认） | 60s 超时或完成 |
| Stage 3 (Config) | 等待 CML（充电机最大限制） | 60s 超时或完成 |
| Stage 30 (Config-Confirm) | 等待 CRO（充电机准备好输出） | 60s 超时或完成 |
| Stage 4 (Charging) | 充电中 | 保护/充电机停止/CC2 电压变化 |
| Stage 5 (Stopping) | 停止等待电流降到空闲 | + 3s 延时 |
| Stage 6 (Finish) | 等待 FC<9V 且 CC2 约 5V（拔出枪） | 返回 Stage 0 |

### 8.5 SciUser -- 串口通信接口（Modbus RTU）

**核心文件**：SciCtrlHandle、SciInfoPoint、SciParaHandle、ModbusMap

#### 什么是 SCI？

在本系统中，SCI（Serial Communication Interface）指**串行通信接口（RS485/UART）**，实现**类 Modbus RTU 协议**的通信层，允许外部设备（HMI、上位机、充电机）通过串口读写 BMS 寄存器。

#### Modbus 寄存器映射（约 6500 个寄存器）

| 地址范围 | 描述 | 访问类型 |
|---------|------|---------|
| 0-12 | 负载命令寄存器 | 读写 |
| 90-98 | 设备基本信息 | 只读 |
| 100-115 | 项目基本信息 | 只读 |
| 120-148 | 故障/事件记录信息 | 只读 |
| 148-349 | 故障记录信息（DTC） | 只读 |
| 350-446 | 历史数据记录 | 只读 |
| 450-460 | 系统时钟 | 读写 |
| 460-490 | 设备序列号 | 读写 |
| 490-500 | 组 ID 寄存器 | 读写 |
| 500-502 | 权限密码 | 写 |
| 502-550 | 系统控制命令 | 写 |
| 550-600 | 用户控制命令 | 写 |
| 600-650 | 调试命令 | 写 |
| 650-990 | 系统/充电/组参数配置 | 读写 |
| 1000-1150 | 命令响应信息 | 只读 |
| 1150-4500 | 系统/充电/组参数结果 | 只读 |
| 4500-5000 | 充电机信息 | 只读 |
| 5000-6500 | 组信息（状态/限制/电芯电压/温度） | 只读 |

### 8.6 Tactics -- 核心控制策略

**核心文件**：TacUser、GroupTactic、GroupMode、GroupProtect、GroupState、GroupDebug

#### 接触器控制（12 路输出）

| 编号 | 名称 | 驱动 | 说明 |
|------|------|------|------|
| 1 | 主接触器 | HS1+LS1 | 主回路 |
| 2 | 预充接触器 | HS2 | 预充电 |
| 3 | 负接触器 | HS3+LS3 | 负极回路 |
| 4 | 辅助电源接触器 | HS4 | 辅助电源 |
| 5 | RUN 灯 | HS5 | 运行指示灯 |
| 6 | ALARM 灯 | HS6 | 告警指示灯 |
| 7 | 烟感电源 | HS7 | |
| 8 | 加热接触器 | HS8 | |
| 9 | 风机/加热电源 | HS9（K9） | |
| 10 | 状态干接点 | DRY0 | |
| 11 | 充电干接点 | DRY1 | |
| 12 | 放电干接点 | DRY2 | |

#### 接触器反馈方法

| 类型 | 方法 |
|------|------|
| 0 | 仅驱动信号（无反馈验证） |
| 1 | 电平信号反馈（带故障线覆盖） |
| 2 | 电平 + 电流反馈（HS 驱动电流检查） |
| 3 | 外部干接点反馈 |

#### 工作模式

| 模式 | 编号 | 说明 |
|------|------|------|
| Auto | 0 | 正常自动运行 |
| Hand | 1 | 手动控制 |
| Emerg | 2 | 紧急操作（需 Service/Admin） |
| Debug | 3 | 工厂调试（需 Admin） |

紧急模式和调试模式只能回退到正常模式，不能直接互相切换。

#### 启动控制序列

```c
TacGroupStartCtrlTactic()
  1. 闭合预充接触器
  2. 等待预充完成（电压差检测）
  3. 闭合主接触器
  4. 断开预充接触器
```

#### 保护系统

| 保护码 | 类型 | 说明 |
|--------|------|------|
| 0 | 充限流 | 充电电流限制 |
| 1 | 放限流 | 放电电流限制 |
| 2 | 充保护 | 充电保护（降额->保护） |
| 3 | 放保护 | 放电保护（降额->保护） |
| 4 | 系统保护 | 充放电同时保护 |
| 5 | 系统错误保护 | 系统级错误 |
| 6 | 空闲异常期望错误 | 空闲异常 |

#### 风机/热管理

基于温度和充放电电流的 PWM 风机调速：
- 大电流运行时（>= 0.8C 持续 > 20min）全速运转
- 加热接触器控制
- 按项目类型可配置（标准 PACK 与 CATL PACK）

#### 辅助外设管理（IMEU2）

- AC 类型检测（国内型 vs 同盛型）
- PWM 风机错误检测
- 卷帘门控制
- 柜内照明控制
- 珠海三安报警覆盖

---

## 9 PileSys 电池堆管理

**说明**：`PILE_MANAG_EN` 当前设为 0，属于**条件编译模块**。启用后用于**多组电池并联运行的储能系统管理**。

"Pile"（堆）指**多组电池并联到共享直流母线的系统架构**，最高支持 4 组（`PILE_MAX_G_NUM=4`）。

### 9.1 系统架构

```
                    PCS（功率转换系统）
                         |
                    [上游 CAN]
                         |
              PileSys 主控制器（自选举 Master）
                         |
              +----------+--------+--------+
              |          |        |        |
           Group 0    Group 1  Group 2  Group 3  (各自由独立 BMU 管理)
```

**自选举机制**：Group 0 自选举为主控制器，协调所有其他组。

### 9.2 模块组成

| 模块 | 主要文件 | 功能 |
|------|---------|------|
| PileCalc | PileDataCalc、PileLimCalc | 堆级 SOC/SOE/SOH 聚合、电流/电压限制 |
| PileCan | PileUpMsg、PileDnMsg | 上游（PCS）和下游（电池组）CAN 通信 |
| PileDiag | PileDiagCheck、PileDiagMaker | 故障检测（PCS CAN 丢失 30s、BCU CAN 丢失 10s） |
| PilePara | PilePara、PileParaRd/Wr | 堆级参数存储 |
| PileSci | PileSciCtrlHd、PileSciInfoPt | 堆级串口寄存器映射 |
| PileStati | PileEnerStat | 历史能量统计 |
| PileTact | PileTactics、PileProtect、PileAutoCtrl | 状态机、保护、动态并联 |
| PileUser | PileUserAPI | 主任务循环（10ms 轮询，10 时隙） |

### 9.3 状态命令

| 命令 | 编号 | 说明 |
|------|------|------|
| 上电 | 0 | |
| 预充 | 1 | |
| 启动 | 2 | |
| 带充启动 | 3 | |
| 带放启动 | 4 | |
| 正常停止 | 5 | |
| 放电停止 | 6 | |
| 充电停止 | 7 | |
| 急停 | 8 | |
| 关机 | 9 | |

### 9.4 动态并联控制

管理电池组动态并网过程：
- 电压和电流 9 项查表
- 跟踪充电/放电/错误状态下正在分离的组
- 并联参数错误和分离错误监控

### 9.5 保护状态

| 类型 | 说明 |
|------|------|
| 充限流保护 | 上游电路断开 |
| 放限流保护 | 下游电路断开 |
| 快充故障 | |
| 快放故障 | |
| 系统充放保护 | |
| 系统错误保护 | |

### 9.6 主任务调度（10ms 轮询）

| 时隙 | 周期 | 执行任务 |
|------|------|---------|
| 0, 5 | 50ms | CAN 消息发送（上游 PCS） |
| 1 | 100ms | 数据计算（SOC/SOE/SOH）、限流计算 |
| 2 | 100ms | 各组状态检查、电压限计算 |
| 3 | 100ms | 告警码、错误码生成 |
| 6 | 100ms | 保护状态、充放电状态检查 |
| 7 | 100ms | 系统保护控制、动态并联控制 |
| 8 | 100ms | 工作状态、工作模式检查 |
| 9 | 1s 分片 | 特征计算、通信诊断、权限控制、按键检查、掉电保护 |

---

## 10 Addition 附加功能

### 10.1 Calibration -- 标定功能

#### 10.1.1 电流标定（OutCurrCorr）

**数学模型**：`实际值 = (原始值 × K + B) / 1000`（K/B 放大 1000 倍存储）

**标定通道**：

| 通道 | 说明 |
|------|------|
| 霍尔输出 | 单点（CS）和双点（CD）标定 |
| 分流器充电 | 小电流范围和大电流范围 K/B |
| 分流器放电 | 小电流范围和大电流 K/B |

**标定流程（状态机，1s 周期）**：

| 状态 | 功能 |
|------|------|
| State 0 | 空闲，等待标定命令 |
| State 1 | 清除上次 K/B，清除 EEPROM 记录 |
| State 2-3 | 采集第 1 标定点（5 点平滑平均滤波） |
| State 4-5 | （双点）采集第 2 标定点 |
| State 6 | 计算 K/B，验证精度（容差 0.5-5mV），写入 EEPROM |

**平滑过渡**：分流器在 110A~130A 区间，输出采用小量程和大量程 K/B 的平均值，实现平滑过渡。

#### 10.1.2 高压标定（HigVoltCorr）

**标定通道**：总压、预充电压、负载电压、母线+、母线-

**数学模型**：`实际值 = (原始值 × K + B) / 1000`

**容差要求**：总压 2V、预充 5V、母线电压 10mV

### 10.2 LoadApp -- CAN Bootloader

#### Flash 映射

| 区域 | 地址 | 用途 |
|------|------|------|
| BOOT 入口 | 0x000000 | Bootloader 代码起始 |
| APP 代码 | 0x001000 | 应用固件起始 |
| APP 结束 | 0x063000（内部）/ 0x0C5000（外部） | APP 区结束 |
| 拷贝缓冲 | 0x063000（内部）/ 外部起始 | APP 备份区 |
| 下载缓冲 | 0x100000 | 外部 Flash 远程下载区 |
| 启动参数 | 0x0F7000 | Boot 参数块（32 字节） |

#### 5 种工作模式

| 模式 | 代码 | 说明 |
|------|------|------|
| 正常 APP | 0 | 默认运行 |
| 远程传输 | 1 | APP 接受固件下载 |
| 无 APP | 2 | APP 丢失（留在 BOOT） |
| 本地启动 | 3 | BOOT 从备份拷贝 |
| 远程启动 | 4 | BOOT 安装下载的固件 |

#### 状态机（26 个状态）

关键状态：Route(0) -> 握手等待(1) -> 握手成功(2) -> 擦除成功(4) -> 传输开始(6) -> 传输中(7) -> 传输成功(8) -> 写入成功(10) -> 等待安装(12) -> 安装中(15) -> 安装成功(16)

#### 远程下载协议

- 通过外部 Flash 接收固件
- 按段（Section）CRC 验证
- 每 4096 帧为一个周期验证

### 10.3 OperAuth -- 操作权限系统

#### 5 级权限体系

| 级别 | 名称 | 代码 | 密码 | 权限 |
|------|------|------|------|------|
| 0 | 普通用户 | 0x00 | 0x0000 | 只读/基本操作 |
| 1 | 操作员 | 0x0A | 0x2D08 | 1 级操作 |
| 2 | 服务工程师 | 0x0D | 0xBC5A | 2 级操作 |
| 3 | 管理员 | 0x0100 | 0xEE5A | 完全访问 |
| 3 | 内部管理员 | 0x0000 | 0x0315 | 完全访问（备用） |

#### 按端口管理

| 端口 | 代码 | 说明 |
|------|------|------|
| CAN0/1/2 | 0-2 | |
| SCI0-SCI3（RS485） | 5-8 | |
| 按键 | 10 | 默认管理员 |
| 工厂 | 11 | |
| 内部 | 12 | 默认管理员 |

### 10.4 UserDef -- 用户自定义功能

#### 按键控制（1s 周期）

**急停按钮**：直接发送急停命令。

**电源按钮（长按动作）**：

| 动作 | 效果 |
|------|------|
| 按下 1-2s | 上电命令 |
| 按下 2-3s | 正常启动 |
| 按下 30s+ | 若在保护/错误态，切紧急模式；若空闲，正常启动 |
| 松开后按下 2-3s | 正常停止（电流在 -0.2A~0.3A 范围内） |
| 松开后按下 4s+ | 系统关机（无充放电时） |

**自动错误清除**：12 小时后（43200 秒）自动清除错误（SOS 等级 < 4）。

#### 掉电保护（2s 周期）

触发自动关机的条件：
- **欠压 2 级**：电芯电压低于 VL2
- **欠压 1 级**：电芯电压低于 VL1（更严重）
- **系统错误状态**
- **外部通信超时**

---

## 11 核心算法详解

### 11.1 SOC 算法

#### 整体架构

```
                    +------------------+
  电流积分 -------->|  SOC 累加器      |
                    |  (Ah Counting)   |
  电压校正(高压端) ->|  阶梯校正        |---> SocDisplay --> 输出 SOC
  温度补偿 --------->|  2 维温度 x C率  |       (50ms)
  C 率补偿 --------->|  查表补偿        |
                    +------------------+
                           |
                    自动校正(500ms)  ---->  EEPROM 存储
                    SOC/SOE 同步(10s)
```

#### SOC 计算流程

```
SOC = SOC_初值 + ∫ (I × η) / Capacity dt
```

其中：
- I：实时电流（充电为正，放电为负）
- η：库仑效率（充电 101.5%）
- Capacity：当前可用总容量（受温度和 C 率影响）

#### 电压阶梯校正

| 条件 | 目标 SOC |
|------|---------|
| 充电电压到达校正点（阈值 + 110~150mV） | 99.3% |
| 放电电压到达校正点（阈值 - 150mV） | 0.13% |

#### SOC 显示平滑

- 缓变速率：5%/s
- 满电 SOC 目标值：根据电芯化学查表
- 放空 SOC 目标值：根据电芯化学查表

### 11.2 SOE 算法

```
SOE = SOE_初值 + ∫ (P × η) / Energy dt
```

- P：实时功率（充电为正 P=V × I，放电为负）
- η：能量效率（充电 95.5%）
- Energy：当前可用总能量

**SOC/SOE 协调**：
- 保证 SOE 动态范围不小于 SOC
- 包间 SOE 同步偏差不超过 3.0%

### 11.3 SOH 算法

```
SOH = min(容量衰减率, 循环衰减率)

容量衰减率 = fadeCap / ratedCap × 100%
循环衰减率 = fadeCycle / ratedCycle × 100%
```

在 100ms 周期更新。

### 11.4 电流限流算法

#### 温度降额

使用 15 点温度查表，从 `gEepCfg_105[]` 读取：

```c
// gEepCfg_105 格式：{温度, 充电倍率, 放电倍率}
// 例如：{-300, 100, 200} 表示在 -30°C 时，充电限制 0.1C（100/1000），放电 0.2C
```

#### SOC 降额

接近满电时降低充电限流，接近放空时降低放电限流。

#### 电压接近限值降额

当最高电芯电压接近上限时降低充电限流，最低电芯电压接近下限时降低放电限流。

#### CV 平滑降额

在充电末端使用 10 级平滑表逐步降流：

```c
// 10 级表定义：偏移量(mV)、电流百分比
// 例如：{+50mV, 90%}, {+30mV, 70%}, {+20mV, 50%}, ...
```

### 11.5 均衡算法

#### 触发条件

```
当满足以下任一条件时启动均衡：
1. V_cell_min >= BALAN_DHG_MIN_VOLT 且 V_cell_i > V_cell_min
2. V_avg >= 阈值 且 V_cell_i > V_avg + 阈值
3. V_avg >= 阈值 且 V_cell_min >= V_VL2 + 80mV 且 V_cell_i > V_cell_min
```

#### 高温限流

```
如果 T_slave >= 60°C：
    每从控最多均衡 8 个电芯
    按电压偏差排序，优先均衡偏差最大的
如果 T_slave >= 80°C：
    禁止全部均衡
```

### 11.6 绝缘检测算法

#### 原理

通过切换两个已知电阻到正/负端到地，测量对地电压，求解方程组：

```
R1（20kΩ）和 R2（2200kΩ）分别连接到正负端
测量 V1（R1 切换到正极时的正极对地电压）
测量 V2（R2 切换到负极时的负极对地电压）
求解正/负侧绝缘电阻 R_pos 和 R_neg
```

#### 状态机

```
Init -> 选择 V1 -> 采样 V1 -> 选择 V2 -> 采样 V2 -> OK -> Diag OK
```

### 11.7 PI 控制器

系统在限流等场景使用 PI 控制器：

```c
// 比例项
proportional = PI.Kp * e >> PI.KpR

// 积分项
integral = PI.Ki * e >> PI.KiR
PI.sum = saturate(PI.sum + integral, PI.sumMin, PI.sumMax)

// 输出
out = proportional + PI.sum
out = saturate(out, PI.outMin, PI.outMax)
```

---

## 12 通信协议

### 12.1 内部 CAN 协议

| 总线 | 用途 | 速率 |
|------|------|------|
| CAN0 | 外部通信 / 上位机 | 500kbps |
| CAN1 | 从控板 BSU 通信 | 500kbps |
| CAN2 | 外部通信 / 充电机 | 500kbps |

#### 报文格式

29 位 CAN ID：方向位 + 功能码 + 消息类型 + 目标设备类型 + 目标设备地址

### 12.2 GB/T 27930 快充协议

| 报文 | 方向 | 说明 |
|------|------|------|
| CHM | 充电机->BMS | 充电机握手 |
| CRM | 充电机->BMS | 充电机就绪 |
| BCP | BMS->充电机 | 电池充电参数 |
| BRM | BMS->充电机 | BMS 辨识信息 |
| ABRM | 充电机->BMS | 充电机应答辨识 |
| CBRM | BMS->充电机 | BMS 应答辨识 |
| ABCP | BMS->充电机 | 电池充电参数总体 |
| CBCP | 充电机->BMS | 充电机充电参数 |
| ABCS | BMS->充电机 | 电池充电状态总体 |
| CBCS | 充电机->BMS | 充电机充电状态 |
| CCS | BMS->充电机 | 充电状态 |
| CML | 充电机->BMS | 充电机最大值 |
| CRO | 充电机->BMS | 充电机就绪输出 |
| ABMV/CBMV | BMS/充电机 | 电池电压测量 |
| CST/CSD | 充电机/BMS | 充电机停止/停止原因 |

### 12.3 Modbus RTU（RS485）

| 参数 | 值 |
|------|-----|
| 波特率 | 19200bps（默认） |
| 数据位 | 8 |
| 停止位 | 1 |
| 校验 | CRC16 |
| 支持功能码 | 01/02/03/04/05/06/16 |

### 12.4 CATL 协议

#### 上行消息（CAN0，SAE J1850 CRC-8）

| 消息 ID | 内容 | 周期 |
|---------|------|------|
| 0x040E8F91+ | BMS 状态（高压态、SOC、模式） | 50ms |
| 0x040E9091+ | 总数据 1（电压、电流） | 50ms |
| 0x040E9191+ | 总数据 2（充电能量、剩余能量、SOH） | 50ms |
| 0x040E9391+ | 总数据 3（电芯极值电压/温度） | 50ms |
| 0x040E9291+ | 限制数据（充放电限流、限压） | 50ms |
| 0x0701FF91+ | 告警信息（48 位告警字，40+ 故障标志） | 100ms |
| 0x0B03FF91+ | 特征信息 1（电芯电压极值） | 100ms 交替 |
| 0x0B04FF91+ | 特征信息 2（平均电压、温度极值） | 100ms 交替 |

#### 告警位定义（48 位，部分列举）

| 位 | 内容 |
|----|------|
| 0 | 电芯过压 1 级 |
| 1 | 电芯过压 2 级 |
| 2 | 电芯欠压 1 级 |
| 3 | 电芯欠压 2 级 |
| 4 | 电芯高温 1 级 |
| 5 | 电芯高温 2 级 |
| 6 | 电芯低温 1 级 |
| 7 | 电芯低温 2 级 |
| 8 | 总压过压 1 级 |
| ... | ... |
| 20+ | 过流、温差、SOC 低、均衡、接触器、通信、绝缘、风机等 |

### 12.5 工装测试协议（FrockMsg）

| 命令 | 功能 |
|------|------|
| 0x01 | CAN 通信状态和波特率 |
| 0x02 | RS485 通信状态 |
| 0x03 | 高压采样（总压、预充、母线-） |
| 0x04 | 电流采样（霍尔、分流器、温度） |
| 0x05 | 绝缘电阻 |
| 0x06 | 低压采样（电源、ACC、霍尔 5V） |
| 0x10-0x1F | 电芯电压（每帧 4 串，共 16 串） |
| 0x20-0x21 | 电芯温度（每帧 8 个） |
| 0x22-0x23 | 均衡温度和状态 |
| 0x2D | 序列号/条码（4 帧，共 22 字节） |
| 0x2E | 版本信息 |
| 0x2F | 标定状态 |
| 0x30 | 切换工作模式 |
| 0x31 | 直接控制接触器 |
| 0x32 | 系统时间同步 |
| 0x33 | 触发电流/电压标定 |
| 0x34 | 配置通信波特率 |
| 0x35 | 写入序列号 |
| 0x36 | 触发电芯均衡 |

---

## 13 数据记录与存储

### 13.1 外部 Flash 分区（SST26VF064，8MB）

| 区域 | 地址 | 容量 | 内容 |
|------|------|------|------|
| 区域 1 | 0x000000 - 0x0FFFFF | 1MB | 启动程序区 1 |
| 区域 2 | 0x100000 - 0x1FFFFF | 1MB | 启动程序区 2 |
| 区域 3 | 0x200000 - 0x2FFFFF | 1MB | 累计故障记录（~400 条/天 × 50 天） |
| 区域 4 | 0x300000 - 0x3FFFFF | 1MB | 累计事件记录 |
| 区域 5 | 0x400000 - 0x4FFFFF | 1MB | 累计故障波形记录（约 222 条） |
| 区域 6 | 0x500000 - 0x6FFFFF | 2MB | 累计运行数据记录（约 576 条，220s 周期） |
| 区域 7 | 0x700000 - 0x70FFFF | 64KB | DOD 记录（约 10200 条） |
| 区域 8 | 0x710000 - 0x7EFEFF | 960KB | 保留 |
| 区域 9 | 0x7FF000 - 0x7FFFFF | 4KB | CRC 校验 |

### 13.2 故障记录格式

包含记录时间、错误 ID、错误级别、系统状态、电芯电压/温度极值、充放电状态等。

### 13.3 事件记录格式

包含记录时间、事件分类（输入/输出/状态/命令）、前后状态值等。

### 13.4 波形记录

**触发条件**：细胞电压高/低、总压高/低、温度高/低、电流过载

**采样周期**：
- 电芯电压：2s
- 电流：1s

记录触发前和触发后的波形数据。

### 13.5 参数持久化

```
参数写入路径：RAM -> 内部 EEPROM -> 外部 EEPROM -> 内部 Flash（备份）
参数读取路径：外部 EEPROM（优先）-> 内部 EEPROM -> 内部 Flash（回退）
```

使用双扇区轮换和 CRC 校验确保数据安全。

---

## 附录：术语表

| 术语 | 中文释义 |
|------|---------|
| BMU | Battery Management Unit，电池管理单元 |
| BSU | Battery Slave Unit，电池从属单元 |
| SOC | State of Charge，荷电状态 |
| SOE | State of Energy，能量状态 |
| SOH | State of Health，健康状态 |
| AFE | Analog Front End，模拟前端 |
| SBC | System Basis Chip，系统基础芯片 |
| SAE | Society of Automotive Engineers，美国汽车工程师学会 |
| GB/T | 国家标准/推荐 |
| CAN | Controller Area Network，控制器局域网络 |
| RS485 | Recommended Standard 485，485 通信标准 |
| Modbus | 工业通信协议 |
| KB | 斜率 (K) 和偏置 (B)，线性标定参数 |
| DOD | Depth of Discharge，放电深度 |
| CV | Constant Voltage，恒压 |
| LFP | 磷酸铁锂 |
| NCM | 镍钴锰三元锂 |
| LTO | 钛酸锂 |
| SAE J1850 CRC-8 | SAE 标准 8 位循环冗余校验 |

---

*文档版本: 1.0*
*生成日期: 2026-04-20*