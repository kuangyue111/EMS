/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : MCP3913.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : MCP3913通讯驱动函数模块
** @Instructions :
**===========================================================================================*/
#ifndef __MCP3913_H
#define __MCP3913_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypedef.h"
#include "DEVGPIO.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
/*MCP3913设备地址*/
#define MCP_DEVICE_ADDRESS  0x01

#define USE_SPI             1

/*采样延迟配置*/
#define USE_PHASE_DELAY     1
#define DELAY_TIME          1

//#define MCP_MCLK          16
#define MCP_MCLK            4

/*采样通道号*/
#define MCP_CHANNEL0        0x00
#define MCP_CHANNEL1        0x01
#define MCP_CHANNEL2        0x02
#define MCP_CHANNEL3        0x03
#define MCP_CHANNEL4        0x04
#define MCP_CHANNEL5        0x05
#define MCP_CHANNUM         0x06

/*MCP3913控制命令*/
#define MCP_MOD             0x08
#define MCP_PHASE0          0x09
#define MCP_PHASE1          0x0a
#define MCP_GAIN            0x0b
#define MCP_STATUSCOM       0x0c
#define MCP_CONFIG0         0x0d
#define MCP_CONFIG1         0x0e
#define MCP_OFFCAL_CH0      0x0f
#define MCP_GAINCAL_CH0     0x10
#define MCP_OFFCAL_CH1      0x11
#define MCP_GAINCAL_CH1     0x12
#define MCP_OFFCAL_CH2      0x13
#define MCP_GAINCAL_CH2     0x14
#define MCP_OFFCAL_CH3      0x15
#define MCP_GAINCAL_CH3     0x16
#define MCP_OFFCAL_CH4      0x17
#define MCP_GAINCAL_CH4     0x18
#define MCP_OFFCAL_CH5      0x19
#define MCP_GAINCAL_CH5     0x1A
#define MCP_LOCK            0x1f

#define MCP_OSR32           0x00
#define MCP_OSR64           0x01
#define MCP_OSR128          0x02
#define MCP_OSR256          0x03
#define MCP_OSR512          0x04
#define MCP_OSR1024         0x05
#define MCP_OSR2048         0x06
#define MCP_OSR4096         0x07

#define MCP_BOOST2          0x03
#define MCP_BOOST1          0x02
#define MCP_BOOST066        0x01
#define MCP_BOOST05         0x00

#define MCP_READ            0x01
#define MCP_WRITE           0x00

/*MCP3913复位信号，低有效*/
#define MCP3913_RESET_SET   {(void)DEVGPIOSetPortState(ePOut_LTC2949CS, ePSTATE_ON);}//需修改
#define MCP3913_RESET_CLR   {(void)DEVGPIOSetPortState(ePOut_LTC2949CS, ePSTATE_OFF);}

/*MCP3913片选信号，低有效*/
#define MCP3913_CS_HIGH     {(void)DEVGPIOSetPortState(ePOut_LTC2949CS, ePSTATE_ON);}//需修改
#define MCP3913_CS_LOW      {(void)DEVGPIOSetPortState(ePOut_LTC2949CS, ePSTATE_OFF);}

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*MCP3913控制命令结构体*/
typedef struct
{
    u32 Config0;
    u32 Config1;
    u32 Phase0;
    u32 Phase1;
    u32 Gain;
    u32 Status;
    u32 OFFCAL_Ch0;
    u32 GAINCAL_Ch0;
    
}MCPControlTypedef;

/*MCP3913采集通道*/
typedef enum
{
    eMcp3913Chan_Curr = MCP_CHANNEL0,          /*分流器电流采样值*/
    eMcp3913Chan_SumV = MCP_CHANNEL1,          /*总电压采样值*/
    eMcp3913Chan_PreV = MCP_CHANNEL2,          /*外端预充电压采样值*/
    eMcp3913Chan_FastChV = MCP_CHANNEL3,       /*快充电压采样值*/
    eMcp3913Chan_SlowChV = MCP_CHANNEL4,       /*慢充电压采样值*/
    eMcp3913Chan_NbusV = MCP_CHANNEL5,         /*NBUS电压*/
    
    eMcp3913Chan_Fin = MCP_CHANNUM
}e_Mcp3913Chan;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void Mcp3913Init(void)
** @Input     : void
** @Output    : void
** @Function  : MCP3913采样芯片初始化
** @The notes :
**===========================================================================================*/
void Mcp3913Init(void);

/*=============================================================================================
** @Name      : void Mcp3913SampLosSetFlag(u8 flag)
** @Input     : flag: 0 或 1
** @Output    : void
** @Function  : 采样断线标志设置
** @The notes :
**===========================================================================================*/
void Mcp3913SampLosSetFlag(u8 flag);

/*=============================================================================================
** @Name      : u8 GetMcp3913SampLosFlag(void)
** @Input     : void
** @Output    : 采样断线标志
** @Function  : 获取采样断线标志
** @The notes :
**===========================================================================================*/
u8 GetMcp3913SampLosFlag(void);

/*=============================================================================================
** @Name      : s16 GetMcp3913SampleAD(u8 Channel)
** @Input     : Channel:采样通道号[0,6]
** @Output    : 读取到的AD值
** @Function  : 获得MCP3913指定通道采样AD值
** @The notes :  MCU读取AD值一次性最多只能读取4个通道,一次读取一个(SPI通讯限制)
**===========================================================================================*/
s16 GetMcp3913SampleAD(u8 Channel);

#endif
