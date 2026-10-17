/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPLPIT.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 低功耗中断定时器驱动模块
** @Instructions :
**===========================================================================================*/
#ifndef FUNTION_SYSTEMINFO_PILEINFO_H_
#define FUNTION_SYSTEMINFO_PILEINFO_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
/* LPIT Timers settings */
/* Watchdog timer. */
#define LPIT_CHANNEL_WD     0UL
/* Diagnostic timer. */
#define LPIT_CHANNEL_DIAG   1UL

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*LPIT通道定义*/
typedef enum
{
    eLPIT0 = 0,

	eLPITNUM
}e_LPITNum;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPLPITInit(e_LPITNum lpitNum)
** @Input     : lpitNum:lpit通道号
** @Output    : 初始化成功或失败：TRUE:成功;FALSE:失败
** @Function  : LPIT模块初始化
** @The notes : 设备抽象层调用，上电初始化
**===========================================================================================*/
u8 BSPLPITInit(e_LPITNum lpitNum);

/*=============================================================================================
** @Name      : void BSPLpitStartTime(u32 instance,  u32 mask)
** @Input     : instance:    mask：
** @Output    : void
** @Function  : 开始定时器计数
** @The notes :
**===========================================================================================*/
void BSPLpitStartTime(u32 instance,  u32 mask);

/*=============================================================================================
** @Name      : void BSPLpitStopTime(u32 instance,  u32 mask)
** @Input     : instance:    mask：
** @Output    : void
** @Function  : 停止定时器计数
** @The notes :
**===========================================================================================*/
void BSPLpitStopTime(u32 instance,  u32 mask);

/*=============================================================================================
** @Name      : void LPIT_ISR_0(void)
** @Input     : config:FS6503配置参数结构体
** @Output    : void
** @Function  : LPIT interrupt handler used for the Watchdog refresh timing.
** @The notes : MCU需要注册使能中断
**===========================================================================================*/
void LPIT_ISR_0(void);

/*=============================================================================================
** @Name      : void LPIT_ISR_1(void)
** @Input     : config:FS6503配置参数结构体
** @Output    : void
** @Function  : LPIT interrupt handler used for the diagnostics and report timing.
** @The notes : MCU需要注册使能中断
**===========================================================================================*/
void LPIT_ISR_1(void);

#endif

