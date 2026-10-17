//==========================================Copyright(c)=======================================
/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPFTM.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : Timer定时器驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPIRQ.h"
#include "BSPGPIO.h"
#include "BSPISRHook.h"
#include "flexTimer_mc1.h"
#include "ftm_mc_driver.h"
#include "ftm_common.h"
#include "BSPFTM.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
ftm_state_t stateInputCapture;/* The state structure of instance in the input capture mode */
ftm_state_t stateTimer;
ftm_state_t statePwm;

ftm_timer_param_t flexTimer_Config;


u8 sTimecounter1msflag = 0;		/*1ms任务标志*/
u8 sTimecounter5msflag = 0;	    /*5ms任务标志*/
u8 sTimecounter100msflag = 0;	/*100ms任务标志*/
u8 sTimecounter500msflag = 0;	/*500ms任务标志*/
u8 sTimecounter1000msflag = 0;	/*1000ms任务标志*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPSysTickInit(u32 time, u8 busClock)
** @Input     : time:配置时间[1,200000](0.001ms即1us) busClock:总线时钟≥1(M)
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : 系统闹钟定时中断驱动模块初始化
** @The notes : 系统作为RTOS系统心跳源，用户不能使用
**===========================================================================================*/
u8 BSPSysTickInit(u32 time, u8 busClock)
{
	//参数错误
	if((0 == time) || (time > 200000) || (0 == busClock))
	{
		return(FALSE);
	}
	

	return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPFTMInit(e_FTMNum chanel, u32 time, u8 busClock)
** @Input     : chanel:TIMx通道号 time:配置时间≥200(0.001ms即1us) busClock:总线时钟≥1(M)
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : FTM驱动模块初始化
** @The notes : BSP调用
**===========================================================================================*/
u8 BSPFTMInit(e_FTMNum chanel, u32 time, u8 busClock)
{
	(void)busClock;

	switch(chanel)
	{
		/*菊花链PWM输出,定时器0第6通道,由BSPPWM驱动模块完成初始化*/
		case 0:
			/*初始化PWM管脚*/
			//PINS_DRV_SetMuxModeSel(PORTE, ePIN_8, ePORT_MUX_ALT2);
			//flexTimer_pwm1_PwmConfig.nNumCombinedPwmChannels = CHAN6_IDX;

			//FTM_DRV_Init(eFTM0, &flexTimer_pwm1_InitConfig, &statePwm);
			//FTM_DRV_InitPwm(eFTM0, &flexTimer_pwm1_PwmConfig);
			break;

		/*0.1ms定时器*/
		case 1:
			flexTimer_Config.initialValue = 0;
			flexTimer_Config.finalValue = 1000;
			/* Initialize Flex Timer instance as simple timer */
			FTM_DRV_Init(eFTM1, &flexTimer_mc1_InitConfig, &stateTimer);

			/* Install handler for the Timer overflow interrupt and enable it */
			INT_SYS_InstallHandler(FTM1_Ovf_Reload_IRQn, &FTM1TimerISR, (isr_t*) 0U);
			INT_SYS_EnableIRQ(FTM1_Ovf_Reload_IRQn);

			/* Setup the counter to trigger an interrupt every 0.1 ms */
			FTM_DRV_InitCounter(eFTM1, &flexTimer_Config);
			/* Start the counter */
			FTM_DRV_CounterStart(eFTM1);

			break;
//
//		/*菊花链PWM输入捕获,定时器2第0通道*/
//		case 2:
//			/*初始化PWM管脚*/
//			//PINS_DRV_SetMuxModeSel(PORTD, ePIN_10, ePORT_MUX_ALT2);
//			FTM_DRV_Init(eFTM2, &flexTimer_ic1_InitConfig, &stateInputCapture);
//			FTM_DRV_InitInputCapture(eFTM2, &flexTimer_ic1_InputCaptureConfig);
//			/* Install handler for the Timer overflow interrupt and enable it */
//			INT_SYS_InstallHandler(FTM2_Ch0_Ch1_IRQn, &FTM2TimerISR, (isr_t*) 0U);
//			INT_SYS_EnableIRQ(FTM2_Ch0_Ch1_IRQn);
//			break;
//
//		/*1ms定时器*/
//		case 3:
//			flexTimer_Config.initialValue = 0;
//			flexTimer_Config.finalValue = 10000;		/*system clock 80M，8分频*/
//			/* Initialize Flex Timer instance as simple timer */
//			FTM_DRV_Init(eFTM3, &flexTimer_mc1_InitConfig, &stateTimer);
//
//			/* Install handler for the Timer overflow interrupt and enable it */
//			INT_SYS_InstallHandler(FTM3_Ovf_Reload_IRQn, &FTM3TimerISR, (isr_t*) 0U);
//			INT_SYS_EnableIRQ(FTM3_Ovf_Reload_IRQn);
//
//			/* Setup the counter to trigger an interrupt every 1 ms */
//			FTM_DRV_InitCounter(eFTM3, &flexTimer_Config);
//			/* Start the counter */
//			FTM_DRV_CounterStart(eFTM3);
//			break;

		default:
			break;

	}
	return(TRUE);
}

/*=============================================================================================
** @Name      : void FTM0TimerISR(void)
** @Input     : void
** @Output    : void
** @Function  : 定时器0中断
** @The notes : 中断接口调用
**===========================================================================================*/
void FTM0TimerISR(void)
{
    /* Clear FTM Timer Overflow flag */
    //FTM_DRV_ClearStatusFlags(eFTM0, (u32)FTM_TIME_OVER_FLOW_FLAG);

    /*用户完成自己的相关代码*/
}

/*=============================================================================================
** @Name      : void FTM1TimerISR(void)
** @Input     : void
** @Output    : void
** @Function  : 定时器1中断
** @The notes : 中断接口调用
**===========================================================================================*/
void FTM1TimerISR(void)
{
	static u32 sTimecounter1ms = 0;		/*1ms任务标志*/
	static u32 sTimecounter5ms = 0;		/*5ms任务标志*/
	static u32 sTimecounter100ms = 0;	/*100ms任务标志*/
	static u32 sTimecounter500ms = 0;	/*500ms任务标志*/
	static u32 sTimecounter1000ms = 0;	/*1000ms任务标志*/

    /* Clear FTM Timer Overflow flag */
    FTM_DRV_ClearStatusFlags(eFTM1, (u32)FTM_TIME_OVER_FLOW_FLAG);

    if(sTimecounter1ms >= 10)
    {
    	sTimecounter1ms = 0;
    	sTimecounter1msflag = 1;		/*1ms任务标志*/
    }
    else
    {
        sTimecounter1ms++;
    }

    if(sTimecounter5ms >= 50)
    {
    	sTimecounter5ms = 0;
    	sTimecounter5msflag = 1;	    /*5ms任务标志*/
    }
    else
    {
        sTimecounter5ms++;
    }

    if(sTimecounter100ms >= 1000)
    {
    	sTimecounter100ms = 0;
    	sTimecounter100msflag = 1;	    /*100ms任务标志*/
    }
    else
    {
        sTimecounter100ms++;
    }

    if(sTimecounter500ms >= 5000)
    {
    	sTimecounter500ms = 0;
    	sTimecounter500msflag = 1;	    /*500ms任务标志*/
    }
    else
    {
        sTimecounter500ms++;
    }

    if(sTimecounter1000ms >= 10000)
    {
    	sTimecounter1000ms = 0;
    	sTimecounter1000msflag = 1;	    /*1000ms任务标志*/
    }
    else
    {
        sTimecounter1000ms++;
    }

    Time_ISR_Hook(eFTM1);
}

/*=============================================================================================
** @Name      : void FTM2TimerISR(void)
** @Input     : void
** @Output    : void
** @Function  : 定时器2中断
** @The notes : 中断接口调用
**===========================================================================================*/
void FTM2TimerISR(void)
{
//	u16 counter = 0;
//	u16 dutyCycle = 0;
//	u32 frequency = 0;
//
//    /* Clear FTM Timer Overflow flag */
//    FTM_DRV_ClearStatusFlags(eFTM2, (u32)FTM_TIME_OVER_FLOW_FLAG);
//
//    /*读取FTM2捕获寄存器的值，即为PWM周期的计数值*/
//    counter = FTM_DRV_GetInputCaptureMeasurement(eFTM2, CHAN0_IDX);
//
//    if(counter != 0)
//    {
//        //读取IC1捕获寄存器的值，并计算占空比
//        //dutyCycle = (TIM_GetCapture1(TIM4) * 100) / counter;
//
//        //计算PWM频率(主频6分频)
//        //frequency = SystemCoreClock / counter / 6;
//    }
//    else
//    {
//        dutyCycle = 0;
//        frequency = 0;
//    }
//
//    PWM_ISR_Rcv_Hook(dutyCycle, frequency);
}

/*=============================================================================================
** @Name      : void FTM3TimerISR(void)
** @Input     : void
** @Output    : void
** @Function  : 定时器3中断
** @The notes : 中断接口调用
**===========================================================================================*/
void FTM3TimerISR(void)
{
    /* Clear FTM Timer Overflow flag */
    FTM_DRV_ClearStatusFlags(eFTM3, (u32)FTM_TIME_OVER_FLOW_FLAG);

    //Time_ISR_Hook(eFTM3);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
