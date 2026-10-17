/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPLPIT.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 低功耗中断定时器驱动模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "lpit_driver.h"
#include "lpit1.h"
#include "Fs6503_PowerManag.h"
#include "BSPLPIT.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
/* Flag indicating WD refresh is required. */
extern volatile bool isWdRefreshRequired;

/* If true, low priority communication is enabled.
 * Will be set to true right after WD refresh.
 * Communication should be finished before the "closed"
 * WD window ends. */
extern volatile bool isCommEnabled;

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPLPITInit(e_LPITNum lpitNum)
** @Input     : lpitNum:lpit通道号
** @Output    : 初始化成功或失败：TRUE:成功;FALSE:失败
** @Function  : LPIT模块初始化
** @The notes : 设备抽象层调用，上电初始化
**===========================================================================================*/
u8 BSPLPITInit(e_LPITNum lpitNum)
{
	if(lpitNum > eLPITNUM)
	{
		return(false);
	}

	switch(lpitNum)
	{
		case 0:
		    /* Initialize LPIT instance 0
		     *  -   Reset and enable peripheral */
		    LPIT_DRV_Init(eLPIT0, &lpit1_InitConfig);
		    /* Initialize LPIT channel 0 and configure it as a periodic counter. */
			#ifdef FS65_WD_ENABLED
		    lpit1_ChnConfig0.period = WD_TIMER_PERIOD_US;
		    LPIT_DRV_InitChannel(eLPIT0, LPIT_CHANNEL_WD, &lpit1_ChnConfig0);
		    /* Install LPIT_ISR as LPIT interrupt handler */
		    INT_SYS_InstallHandler(LPIT0_Ch0_IRQn, &LPIT_ISR_0, (isr_t *)0);
		    INT_SYS_EnableIRQ(LPIT0_Ch0_IRQn);
			#endif
		    /* Initialize LPIT channel 1 and configure it as a periodic counter
		     * which is used to enable diagnostic communication during the WD closed
		     * window. */
		    lpit1_ChnConfig1.period = FS65_REPORT_DELAY_MS * 1000;
		    LPIT_DRV_InitChannel(eLPIT0, LPIT_CHANNEL_DIAG, &lpit1_ChnConfig1);
		    INT_SYS_InstallHandler(LPIT0_Ch1_IRQn, &LPIT_ISR_1, (isr_t *)0);
		    INT_SYS_EnableIRQ(LPIT0_Ch1_IRQn);

			break;

		default:
			break;
	}

	return(true);
}

/*=============================================================================================
** @Name      : void BSPLpitStartTime(u32 instance,  u32 mask)
** @Input     : instance:低功耗中断定时器1    mask：1:开启第一通道 2:开启第二通道  3：开启第一、二通道
** @Output    : void
** @Function  : 开始定时器计数
** @The notes :
**===========================================================================================*/
void BSPLpitStartTime(u32 instance,  u32 mask)
{
	LPIT_DRV_StartTimerChannels(instance, mask);
}

/*=============================================================================================
** @Name      : void BSPLpitStopTime(u32 instance,  u32 mask)
** @Input     : instance:低功耗中断定时器1   mask：1:停止第一通道 2:停止第二通道  3：停止第一、二通道
** @Output    : void
** @Function  : 停止定时器计数
** @The notes :
**===========================================================================================*/
void BSPLpitStopTime(u32 instance,  u32 mask)
{
	LPIT_DRV_StopTimerChannels(instance, mask);
}

/*=============================================================================================
** @Name      : void LPIT_ISR_0(void)
** @Input     : void
** @Output    : void
** @Function  : LPIT interrupt handler used for the Watchdog refresh timing.
** @The notes :
**===========================================================================================*/
void LPIT_ISR_0(void)
{
	/* Clear LPIT channel flag */
	LPIT_DRV_ClearInterruptFlagTimerChannels(0, (1 << LPIT_CHANNEL_WD));

	isWdRefreshRequired = true;
	FS65WdRefreshTask();/*电源SBC周期喂狗*/
}

/*=============================================================================================
** @Name      : void LPIT_ISR_1(void)
** @Input     : void
** @Output    : void
** @Function  : LPIT interrupt handler used for the diagnostics and report timing.
** @The notes :
**===========================================================================================*/
void LPIT_ISR_1(void)
{
	/* Clear LPIT channel flag */
	LPIT_DRV_ClearInterruptFlagTimerChannels(0, (1 << LPIT_CHANNEL_DIAG));

	/* Enable diagnostics communication. */
	isCommEnabled = true;
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/



