# BMS 电流采样与安时积分系统详解

> **文档版本**: V1.0  
> **生成日期**: 2026-04-21  
> **适用对象**: BMS 软件开发者、嵌入式工程师  

---

## 目录

1. [系统概述](#一系统概述)
2. [硬件采样来源](#二硬件采样来源)
3. [软件架构流程](#三软件架构流程)
4. [关键代码解析](#四关键代码解析)
5. [精度与单位说明](#五精度与单位说明)
6. [设计要点总结](#六设计要点总结)

---

## 一、系统概述

### 1.1 电流采样在 BMS 中的作用

电流采样是 BMS 系统的核心功能，主要用于：

- **SOC 计算**：通过安时积分法计算电池剩余容量
- **能量统计**：计算充放电能量（WH）
- **保护功能**：过流、短路保护
- **均衡控制**：根据电流方向判断充放电状态
- **故障诊断**：检测传感器异常

### 1.2 双传感器设计

本系统采用**双冗余设计**，支持两种电流采样方式：

| 传感器类型 | 采集方式 | 输出范围 | 精度 | 适用场景 |
|-----------|---------|---------|------|---------|
| **霍尔传感器** | 内部 ADC | 0.3V-4.7V | 中等 | 大电流、隔离场合 |
| **分流器** | 外部 AFE (LTC2949) | -90mV~+90mV | 高 | 小电流、高精度场合 |

**配置宏定义** (CurrSample.h:25-27)：

```c
#define CURR_SAMP_MAIN_EN   1   /* 电流采样主使能 */
#define CURR_AUT_ZERO_EN    1   /* 自动校准使能 */
#define CURR_INT_ADD_EN     1   /* 中断电流累加使能 */
```

---

## 二、硬件采样来源

### 2.1 霍尔传感器 (Hall Sensor)

**硬件连接**：
```
霍尔传感器输出 → 运放 → MCU ADC 通道
```

**电气特性** (CurrSample.h:31-33)：
```c
#define CURR_HALL5V_MINV    (450)    /* 霍尔 5V 基准电压最小值 0.01V */
#define CURR_HALL_MIN_AD    (249)    /* 最小 AD 值 (0.3V) */
#define CURR_HALL_MAX_AD    (3890)   /* 最大 AD 值 (4.7V) */
```

**特点**：
- 非接触式测量，电气隔离
- 响应速度快
- 温度漂移较大，需要校准
- 适合大电流场合（>100A）

### 2.2 分流器 (Shunt Resistor)

**硬件连接**：
```
分流器两端 → LTC2949 AFE → SPI 通信 → MCU
```

**电气特性** (CurrSample.h:34-35)：
```c
#define CURR_SHUNT_MIN_V    (-9000)  /* 最小电压 -90mV 0.01mV */
#define CURR_SHUNT_MAX_V    (9000)   /* 最大电压 +90mV 0.01mV */
```

**特点**：
- 精度高，温漂小
- 需要电气隔离
- 有功率损耗（I²R）
- 适合小电流高精度场合

### 2.3 采样通道枚举

```c
typedef enum
{
    eCADC_Hall = 0,     /* 内部 ADC 通道采集霍尔传感器 */
    eCAFE_Shunt = 1,    /* 外部 AFE 通道采集分流器 */
    eCCHAN_Num          /* 电流采样通道数目 */
} e_CSampChan;
```

---

## 三、软件架构流程

### 3.1 完整数据流图

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              硬件层                                          │
│  ┌─────────────────────┐    ┌─────────────────────┐                       │
│  │    霍尔传感器        │    │      分流器          │                       │
│  │  输出: 0.3V-4.7V    │    │  输出: -90mV~+90mV  │                       │
│  └──────────┬──────────┘    └──────────┬──────────┘                       │
│             │                          │                                    │
│             ▼                          ▼                                    │
│  ┌─────────────────────┐    ┌─────────────────────┐                       │
│  │   MCU 内部 ADC      │    │   LTC2949 AFE       │                       │
│  │   12位分辨率         │    │   SPI 通信           │                       │
│  └──────────┬──────────┘    └──────────┬──────────┘                       │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                           驱动层 (Driver)                                    │
│  ┌───────────────────────────────────────────────────────────────────────┐ │
│  │  BSPADC.c / LTC2949_driver.c                                          │ │
│  │  - AD 值读取                                                          │ │
│  │  - SPI 通信                                                           │ │
│  │  - 数据缓存                                                           │ │
│  └───────────────────────────────┬─────────────────────────────────────────┘ │
└──────────────────────────────────┼────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                         采样处理层 (Measurement)                             │
│  ┌───────────────────────────────────────────────────────────────────────┐ │
│  │  CurrSample.c / CalcSampleADToCurr()                                  │ │
│  │                                                                       │ │
│  │  1. AD 值转换为电流值 (mV → mA)                                       │ │
│  │     - 霍尔: 查表或线性转换                                            │ │
│  │     - 分流器: V/R = I (欧姆定律)                                      │ │
│  │                                                                       │ │
│  │  2. 零点校准                                                          │ │
│  │     - sCurrZeroCorr (校准偏移量)                                      │ │
│  │     - 消除传感器零点漂移                                              │ │
│  │                                                                       │ │
│  │  3. 滑动平均滤波                                                      │ │
│  │     - 4 点滑动平均                                                    │ │
│  │     - 消除高频噪声                                                    │ │
│  │                                                                       │ │
│  │  4. 异常检测                                                          │ │
│  │     - 开路检测 (AD 值异常)                                            │ │
│  │     - 短路检测                                                        │ │
│  │     - 超限检测                                                        │ │
│  │                                                                       │ │
│  │  5. 双通道校验 (可选)                                                 │ │
│  │     - 霍尔 vs 分流器对比                                              │ │
│  │     - 差异报警                                                        │ │
│  └───────────────────────────────┬─────────────────────────────────────────┘ │
│                                  │                                          │
│                                  ▼                                          │
│  ┌───────────────────────────────────────────────────────────────────────┐ │
│  │  sRealCurr10mA (全局实时电流变量)                                     │ │
│  │  单位: 10mA (即数值 100 = 1A)                                         │ │
│  └───────────────────────────────┬─────────────────────────────────────────┘ │
└──────────────────────────────────┼────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          中断层 (ISR)                                        │
│  ┌───────────────────────────────────────────────────────────────────────┐ │
│  │  BSPISRHook.c / Time_ISR_Hook() (FTM3 定时器中断)                      │ │
│  │                                                                       │ │
│  │  周期: 1ms (高精度定时)                                                │ │
│  │                                                                       │ │
│  │  ┌─────────────────────────────────────────────────────────────────┐ │ │
│  │  │  ISRCurrCapAddTask()                                            │ │ │
│  │  │                                                                 │ │ │
│  │  │  1. 读取 sRealCurr10mA                                          │ │ │
│  │  │  2. 判断充放电方向                                              │ │ │
│  │  │     - curr > sCurrZeroMax: 放电                                 │ │ │
│  │  │     - curr < sCurrZeroMin: 充电                                 │ │ │
│  │  │  3. 调用积分函数                                                │ │ │
│  │  │     - DhgCapIntTask(): 放电容量积分                             │ │ │
│  │  │     - ChgCapIntTask(): 充电容量积分                             │ │ │
│  │  │  4. 能量积分 (可选)                                             │ │ │
│  │  │     - DhgEnerIntTask(): 放电能量积分                            │ │ │
│  │  │     - ChgEnerIntTask(): 充电能量积分                            │ │ │
│  │  └─────────────────────────────────────────────────────────────────┘ │ │
│  └───────────────────────────────┬─────────────────────────────────────────┘ │
└──────────────────────────────────┼────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                         积分计算层 (EnergyCalc)                              │
│  ┌───────────────────────────────────────────────────────────────────────┐ │
│  │  CurrIntegral.c                                                       │ │
│  │                                                                       │ │
│  │  容量积分计算:                                                        │ │
│  │  ┌─────────────────────────────────────────────────────────────────┐ │ │
│  │  │  sCapInt.chgCap10ma1ms += (curr * cycle)                        │ │ │
│  │  │                                                                 │ │ │
│  │  │  转换公式:                                                      │ │ │
│  │  │  1 mAH = 10mA × 1ms × 36000                                     │ │ │
│  │  │  即: 36000 (10mA·ms) = 1 mAH                                    │ │ │
│  │  └─────────────────────────────────────────────────────────────────┘ │ │
│  │                                                                       │ │
│  │  能量积分计算:                                                        │ │
│  │  ┌─────────────────────────────────────────────────────────────────┐ │ │
│  │  │  sEnerInt.chgEner1w1ms += (curr × volt × cycle)                  │ │ │
│  │  │                                                                 │ │ │
│  │  │  单位: 1WH = 10mA × 100mV × 1ms × 1000000                        │ │ │
│  │  └─────────────────────────────────────────────────────────────────┘ │ │
│  └───────────────────────────────┬─────────────────────────────────────────┘ │
└──────────────────────────────────┼────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                         SOC 计算层 (SocLib)                                  │
│  ┌───────────────────────────────────────────────────────────────────────┐ │
│  │  SocSoeLibAllTask()                                                   │ │
│  │                                                                       │ │
│  │  1. 读取积分值                                                        │ │
│  │  2. 安时积分法计算 SOC                                                │ │
│  │     SOC = SOC₀ + ∫(I × η)dt / Capacity                               │ │
│  │  3. 多重校正                                                          │ │
│  │     - 高压校正 (充电到 99.3%)                                         │ │
│  │     - 低压校正 (放电到 0.13%)                                         │ │
│  │     - 温度补偿                                                        │ │
│  └───────────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 任务调用关系

```
FTM3 定时器中断 (1ms)
    │
    ├──→ Time_ISR_Hook(eFTM3)
    │       │
    │       ├──→ ISRCurrCapAddTask()      【电流积分累加】
    │       │       │
    │       │       ├──→ 读取 sRealCurr10mA
    │       │       ├──→ 判断充放电方向
    │       │       ├──→ DhgCapIntTask()  【放电容量积分】
    │       │       │       └──→ sCapInt.dhgCap10ma1ms += curr
    │       │       ├──→ ChgCapIntTask()  【充电容量积分】
    │       │       │       └──→ sCapInt.chgCap10ma1ms += curr
    │       │       ├──→ DhgEnerIntTask() 【放电能量积分】
    │       │       └──→ ChgEnerIntTask() 【充电能量积分】
    │       │
    │       ├──→ SysClockCountTask()      【系统时钟计数】
    │       ├──→ MbClientTimerTask()      【Modbus 计时】
    │       └──→ CANLibTxRcvHandleTask_API() 【CAN 协议栈】
    │
    └──→ (每 100ms)
            └──→ HeartToSlaveMcuTask()    【心跳包】
```

---

## 四、关键代码解析

### 4.1 中断入口 (BSPISRHook.c:90-123)

```c
void Time_ISR_Hook(e_FTMNum Channel)
{
    static u8 sCount = 0;

    if(Channel == eFTM3)               /* 1ms 定时中断 */
    {
        /* 上电等待初始化完成，防止误操作 */
        if(BSPGetOSMillSecTimeAPI() < 1)
        {
            return;
        }

        /* ★★★ 电流积分累加 (核心功能) ★★★ */
        ISRCurrCapAddTask();

        /* 系统时钟计数 */
        SysClockCountTask();

        /* Modbus 客户端计时 */
        MbClientTimerTask();

        /* CAN 协议栈处理 */
        CANLibTxRcvHandleTask_API();

        /* 每 100ms 发送心跳包 */
        if(sCount >= 100)
        {
            HeartToSlaveMcuTask();
            sCount = 0;
        }
        else
        {
            sCount++;
        }
    }
}
```

### 4.2 电流积分任务 (CurrSample.c:126-167)

```c
void ISRCurrCapAddTask(void)
{
    /* 使用定时中断累加 (高精度) */
    #if(1 == CURR_INT_ADD_EN)
    
    /* ========== 放电状态 ========== */
    /* 条件检查:
     * 1. 无电流采样异常
     * 2. 电流值大于放电阈值 (sCurrZeroMax)
     * 3. 电流值超过硬件参数配置的最小放电电流
     */
    if((0 == SampGetCurrSampExpFlagAPI(eCCHAN_Num))
        && (sRealCurr10mA > sCurrZeroMax)
        && (sRealCurr10mA >= (s16)gGHardPara_104[eGHardPara104_DhgIntPoint]))
    {
        /* 放电容量积分 */
        DhgCapIntTask(sRealCurr10mA, 1);

        /* 放电能量积分 */
        /* 优先使用电芯总电压，异常时使用采样总电压 */
        if(ABS(GetGCellSumVoltAPI(), GetGSampSumVoltAPI()) <= 
           gGBmuHigLevPara_103[eBmuHigLevPara103_SumDiffV])
        {
            DhgEnerIntTask(sRealCurr10mA, GetGCellSumVoltAPI(), 1);
        }
        else
        {
            DhgEnerIntTask(sRealCurr10mA, GetGSampSumVoltAPI(), 1);
        }
    }
    /* ========== 充电状态 ========== */
    /* 条件检查:
     * 1. 无电流采样异常
     * 2. 电流值小于充电阈值 (sCurrZeroMin，负值)
     * 3. 电流值超过硬件参数配置的最小充电电流 (负值)
     */
    else if((0 == SampGetCurrSampExpFlagAPI(eCCHAN_Num))
        && (sRealCurr10mA < sCurrZeroMin)
        && (sRealCurr10mA <= (0 - (s16)gGHardPara_104[eGHardPara104_ChgIntPoint])))
    {
        /* 充电容量积分 (电流取绝对值) */
        ChgCapIntTask((0 - sRealCurr10mA), 1);

        /* 充电能量积分 */
        if(ABS(GetGCellSumVoltAPI(), GetGSampSumVoltAPI()) <= 
           gGBmuHigLevPara_103[eBmuHigLevPara103_SumDiffV])
        {
            ChgEnerIntTask((0 - sRealCurr10mA), GetGCellSumVoltAPI(), 1);
        }
        else
        {
            ChgEnerIntTask((0 - sRealCurr10mA), GetGSampSumVoltAPI(), 1);
        }
    }
    #endif
}
```

### 4.3 容量积分计算 (CurrIntegral.c:59-82)

```c
void ChgCapIntTask(u32 curr, u8 cycle)
{
    /* curr: 充电电流值 (单位 10mA) */
    /* cycle: 积分周期 (1ms) */
    
    /* 累加: 10mA × 1ms */
    sCapInt.chgCap10ma1ms += (curr * cycle);
    
    /* 转换到 1mAH 单位 */
    /* 36000 (10mA·ms) = 1 mAH */
    if(sCapInt.chgCap10ma1ms >= MULT_MAH_TO_10MAMS)
    {
        /* 计算变化量 (用于 SOC 实时更新) */
        sCapInt.changCap1ma1h -= (s32)(sCapInt.chgCap10ma1ms / MULT_MAH_TO_10MAMS);
        
        /* 累加到总充电容量 */
        sCapInt.chgCap1ma1h += (sCapInt.chgCap10ma1ms / MULT_MAH_TO_10MAMS);
        
        /* 保留余数，继续累加 */
        sCapInt.chgCap10ma1ms %= MULT_MAH_TO_10MAMS;
    }
    
    /* 小储能系统: 直接使用 mAH 为单位 */
    #if(0 == SMALL_ESS_EN)
    /* 转换到 1AH 单位 */
    if(sCapInt.chgCap1ma1h >= MULT_AH_TO_MAH)
    {
        sCapInt.chgCap1a1h += (sCapInt.chgCap1ma1h / MULT_AH_TO_MAH);
        sCapInt.chgCap1ma1h %= MULT_AH_TO_MAH;
    }
    #endif
}
```

### 4.4 能量积分计算 (CurrIntegral.c:123-155)

```c
void ChgEnerIntTask(u32 curr, u16 volt, u8 cycle)
{
    static u32 sChg1mw1ms = 0;
    
    /* curr: 电流 (10mA) */
    /* volt: 电压 (100mV) */
    /* cycle: 周期 (1ms) */
    
    /* 功率 = 电流 × 电压 */
    /* 单位: 10mA × 100mV × 1ms = 1mW·ms */
    sChg1mw1ms += (curr * (u32)volt * cycle);
    
    /* 转换到 1W·ms */
    if(sChg1mw1ms >= MULT_WMS_TO_MWMS)
    {
        sEnerInt.chgEner1w1ms += (sChg1mw1ms / MULT_WMS_TO_MWMS);
        sChg1mw1ms %= MULT_WMS_TO_MWMS;
    }
    
    /* 转换到 1WH */
    if(sEnerInt.chgEner1w1ms >= MULT_WH_TO_WMS)
    {
        /* 计算变化量 */
        sEnerInt.changEner1w1h -= (s32)(sEnerInt.chgEner1w1ms / MULT_WH_TO_WMS);
        
        /* 累加总能量 */
        sEnerInt.chgEner1w1h += (sEnerInt.chgEner1w1ms / MULT_WH_TO_WMS);
        sEnerInt.chgEner1w1ms %= MULT_WH_TO_WMS;
    }
    
    /* 转换到 0.1KWH */
    #if(0 == SMALL_ESS_EN)
    if(sEnerInt.chgEner1w1h >= MULT_100WH_TO_WH)
    {
        sEnerInt.chgEner100w1h += (sEnerInt.chgEner1w1h / MULT_100WH_TO_WH);
        sEnerInt.chgEner1w1h %= MULT_100WH_TO_WH;
    }
    #endif
}
```

---

## 五、精度与单位说明

### 5.1 单位换算表

| 物理量 | 存储单位 | 换算关系 | 说明 |
|-------|---------|---------|------|
| **电流** | 10mA | 1 个单位 = 10mA | 分辨率 10mA |
| **电压** | 100mV | 1 个单位 = 100mV | 分辨率 100mV |
| **容量** | 1mAH / 1AH | 1000 mAH = 1 AH | 根据系统配置 |
| **能量** | 1WH / 0.1KWH | 1000 WH = 0.1 KWH | 根据系统配置 |
| **时间** | 1ms | 积分周期 | FTM3 定时器 |

### 5.2 转换系数定义

```c
/* 容量转换系数 */
#define MULT_MAH_TO_10MAMS    36000    /* 36000 (10mA·ms) = 1 mAH */
#define MULT_AH_TO_MAH        1000     /* 1000 mAH = 1 AH */

/* 能量转换系数 */
#define MULT_WMS_TO_MWMS      1000     /* 1000 (mW·ms) = 1 W·ms */
#define MULT_WH_TO_WMS        3600000  /* 3600000 (W·ms) = 1 WH */
#define MULT_100WH_TO_WH      100      /* 100 WH = 0.1 KWH */
```

### 5.3 计算示例

**场景**：电池以 50A 电流充电 1 秒

```
电流 = 50A = 5000 × 10mA
时间 = 1s = 1000ms

容量积分计算:
累加值 = 5000 × 1000 = 5,000,000 (10mA·ms)
转换为 mAH = 5,000,000 / 36000 = 138.89 mAH

能量积分计算 (假设电压 360V):
电压 = 360V = 3600 × 100mV
功率累加 = 5000 × 3600 × 1000 = 18,000,000,000 (mW·ms)
转换为 W·ms = 18,000,000,000 / 1000 = 18,000,000
转换为 WH = 18,000,000 / 3600000 = 5 WH
```

---

## 六、设计要点总结

### 6.1 为什么在中断中积分？

| 方案 | 精度 | 实时性 | 复杂度 | 适用场景 |
|------|------|--------|--------|---------|
| **中断积分** | ⭐⭐⭐ 高 | ⭐⭐⭐ 强 | 中 | **本系统采用** |
| 任务积分 | ⭐⭐ 中 | ⭐⭐ 中 | 低 | 精度要求不高 |
| DMA 积分 | ⭐⭐⭐ 高 | ⭐⭐⭐ 强 | 高 | 需要硬件支持 |

**关键原因**：

1. **时间精度保证**
   - FTM3 定时器提供精确的 1ms 周期
   - 避免任务调度抖动导致的积分误差
   - 任务调度延迟可能达到 1-10ms，累积误差显著

2. **实时性要求**
   - SOC 计算需要精确的安时积分
   - 每 1ms 的误差在长时间运行后会累积
   - 例如：1ms 误差 × 3600s = 3.6s 误差/小时

3. **数据一致性**
   - 中断中读取的电流值是实时值
   - 任务中读取的可能有延迟
   - 避免"读取-处理-写入"过程中的数据不一致

### 6.2 零点校准机制

```c
/* 校准参数 */
static s16 sCurrZeroMax = 0;      /* 零点最大值 */
static s16 sCurrZeroMin = 0;      /* 零点最小值 */
static s16 sCurrZeroCorr = 0;     /* 零点校准值 */
static u8 sCurrZeroFin = 0;       /* 校准完成标志 */

/* 校准流程 */
void CurrSampleInit(void)
{
    u16 zeroCurr[2] = {0};
    
    /* 从 EEPROM 读取上次校准值 */
    if(TRUE == ParaReadStoreCorrZeroCurr(zeroCurr, 2))
    {
        sCurrZeroCorr = zeroCurr[1];
    }
    
    /* 设置阈值 */
    sCurrZeroMax = (s16)gGHardPara_104[eGHardPara104_DhgIntPoint];
    sCurrZeroMin = (0 - (s16)gGHardPara_104[eGHardPara104_ChgIntPoint]);
}
```

**校准条件**：
- 系统上电时自动校准
- 无电流状态 (接触器断开)
- 连续采样多次取平均

### 6.3 异常处理机制

```c
typedef enum
{
    eCExp_HalOff = 0,    /* 霍尔传感器 5V 异常 */
    eCExp_HalErr = 1,    /* 霍尔反馈信号异常 */
    eCExp_ADCErr = 2,    /* 电流采样 ADC 通道异常 */
    eCExp_HADErr = 3,    /* 霍尔 AD 值异常 */
    eCExp_AfeErr = 4,    /* 电流采样 AFE 模块异常 */
    eCExp_SADErr = 5,    /* 分流器 AD 值异常 */
    eCExp_ZroErr = 6,    /* 电流校准异常 */
    eCExp_HSDiff = 7,    /* 双通道不一致 */
    eCExp_IDLen
} e_CSampExp_ID;
```

**异常处理策略**：
- 检测到异常时停止积分
- 使用上次有效值或估算值
- 上报故障码
- 触发保护动作

### 6.4 关键设计要点

1. **双传感器冗余**
   - 霍尔 + 分流器双重保障
   - 可配置使用单一传感器或双传感器校验

2. **中断级积分**
   - 1ms 高精度定时
   - 避免任务调度抖动

3. **多级单位转换**
   - 10mA·ms → mAH → AH
   - 自动进位，保留余数

4. **零点自动校准**
   - 消除传感器漂移
   - EEPROM 存储校准值

5. **异常保护**
   - 8 种异常检测
   - 故障时停止积分

---

## 附录：相关文件索引

| 文件路径 | 功能说明 |
|---------|---------|
| `Sources/Funtion/Measurement/CurrSample.h` | 电流采样接口定义 |
| `Sources/Funtion/Measurement/CurrSample.c` | 电流采样实现 |
| `Sources/Funtion/EnergyCalc/CurrIntegral.h` | 积分计算接口 |
| `Sources/Funtion/EnergyCalc/CurrIntegral.c` | 积分计算实现 |
| `Sources/BSPSys/BSPUser/BSPISRHook.c` | 中断处理 Hook |
| `Sources/Funtion/EnergyCalc/EnerInterface.h` | 能量计算接口 |
| `Sources/Funtion/SocLib/SocUserAPI.c` | SOC 计算 API |

---

> **文档生成时间**: 2026-04-21  
> **作者**: AI Assistant  
> **版本**: V1.0
