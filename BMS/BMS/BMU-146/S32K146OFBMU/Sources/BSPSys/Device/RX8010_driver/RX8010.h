/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : RX8010.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 外部RTC时钟驱动模块头文件
** @Instructions :
**===========================================================================================*/
#ifndef BSPSYS_DEVICE_RX8010_DRIVER_RX8010_H_
#define BSPSYS_DEVICE_RX8010_DRIVER_RX8010_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "stdbool.h"
#include "PublicInfo.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
/* RX8010SJ WRITE/READ*/
#define RX8010_DEVICE_WR        ( 0x64 )
#define RX8010_DEVICE_RD        ( 0x65 )

/* register address , for RX-8010SJ*/
#define RX8010_ADDR_SECOND      ( 0x10 )
#define RX8010_ADDR_MINUTE      ( 0x11 )
#define RX8010_ADDR_HOUR        ( 0x12 )
#define RX8010_ADDR_WEEK        ( 0x13 )
#define RX8010_ADDR_DATE        ( 0x14 )
#define RX8010_ADDR_MONTH       ( 0x15 )
#define RX8010_ADDR_YEAR        ( 0x16 )
#define RX8010_ADDR_RSV17       ( 0x17 )
#define RX8010_ADDR_ALM_MINUTE  ( 0x18 )
#define RX8010_ADDR_ALM_HOUR    ( 0x19 )
#define RX8010_ADDR_ALM_WEEK    ( 0x1A )
#define RX8010_ADDR_ALM_DATE    ( 0x1A )
#define RX8010_ADDR_TMR_CNT0    ( 0x1B )
#define RX8010_ADDR_TMR_CNT1    ( 0x1C )
#define RX8010_ADDR_EXT_REG     ( 0x1D )
#define RX8010_ADDR_FLAG_REG    ( 0x1E )
#define RX8010_ADDR_CTRL_REG    ( 0x1F )

#define RX8010_ADDR_RAM_START   ( 0x20 )
#define RX8010_ADDR_RAM_END     ( 0x2F )

#define RX8010_ADDR_RSV30       ( 0x30 )
#define RX8010_ADDR_RSV31       ( 0x31 )
#define RX8010_ADDR_IRQ_CTRL    ( 0x32 )

/* RX8010内部RAM用作数据存储时的管理状态机*/
#define RX8010RAM_STT_NULL      ( 0x00 )
#define RX8010RAM_STT_WR_PRE    ( 0x01 )
#define RX8010RAM_STT_WR_DATA   ( 0x02 )
#define RX8010RAM_STT_LD_PRE    ( 0x10 )
#define RX8010RAM_STT_LD_DATA   ( 0x11 )
#define RX8010RAM_STT_RPR_PRE   ( 0x20 )

#define SECOND_VAL_MAX    		( 0x60 )	/*时间-秒最大值(16进制表示10进制数)*/
#define MINUTE_VAL_MAX    		( 0x60 )	/*时间-分最大值(16进制表示10进制数)*/
#define HOUR_VAL_MAX      		( 0x24 )	/*时间-时最大值(16进制表示10进制数)*/
#define DATE_VAL_MAX      		( 0x32 )	/*时间-日最大值(16进制表示10进制数)*/
#define MONTH_VAL_MAX      		( 0x13 )	/*时间-月最大值(16进制表示10进制数)*/
#define YEAR_VAL_MAX      		( 0x100)	/*时间-年最大值(16进制表示10进制数)*/
#define RTC_DEV_ID        		( 0x32 )	/*RTC设备地址*/
#define BASI_YEAR_VALUE   		( 2000 )    /*年限基础值*/

/*=============================================================================================
** @Data type definition
**===========================================================================================*/

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=================================================================================================
** @Name      : u8 DEVRTCRX8010Init(void)
** @Input     : void
** @Output    : 初始化结果:TRUE:成功 FALSE:失败
** @Function  : 外部RTC芯片RX8010初始化
** @The notes : 用户调用
**===============================================================================================*/
u8 DEVRTCRX8010Init(void);

/*=================================================================================================
** @Name      : void DEVRTCResetRX8010API(void)
** @Input     : void
** @Output    : void
** @Function  : 外部RTC芯片RX8010复位
** @The notes : RX8010总线锁死后复位总线
**===============================================================================================*/
void DEVRTCResetRX8010API(void);

/*=============================================================================================
** @Name      : void DEVEXTRTCCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 外部RTC检测任务
** @The notes : 4s
**===========================================================================================*/
void DEVEXTRTCCheckTask(void);

/*=================================================================================================
** @Name      : u8 DEVEXTRTCGetNowClockAPI(t_SysClock *rtcClock)
** @Input     : *rtcClock:读取的RTC时钟信息
** @Output    : 读结果:TRUE:成功 FALSE:失败
** @Function  : 从外部RTC芯片RX8010获取当前时钟
** @The notes : 当读取的数据非法时,不更新*rtcClock的数据
**===============================================================================================*/
u8 DEVEXTRTCGetNowClockAPI(t_SysClock *rtcClock);

/*=================================================================================================
** @Name      : u8 DEVEXTRTCSetNowClockAPI(t_SysClock *aimClock)
** @Input     : *aimClock:设置的目标时钟
** @Output    : 设置结果:TRUE:成功 FALSE:失败
** @Function  : 设置外部RTC芯片RX8010的当前时钟
** @The notes :
**===============================================================================================*/
u8 DEVEXTRTCSetNowClockAPI(t_SysClock *aimClock);

/*=============================================================================================
** @Name      : u8 DEVEXTRTCGetReadyFlagAPI(void)
** @Input     : void
** @Output    : 0:未就绪 1:就绪
** @Function  : 获取外部RTC就绪状态标志
** @The notes : 与外部RTC芯片通讯状态
**===========================================================================================*/
u8 DEVEXTRTCGetReadyFlagAPI(void);

#endif
