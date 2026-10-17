//==========================================Copyright(c)=======================================
/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPPWM.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : PWM驱动函数模块
** @Instructions : PWM波输出接口和PWM捕获接口
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPGPIO.h"
#include "BSPFTM.h"
#include "pwm_pal.h"
#include "pwm_pal1.h"
#include "BSPPWM.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPPWMInit(e_PWMMod module, e_PWMChan chan, u8 dir, u16 duty, u32 freq)
** @Input     : Module:PWM对应FTM模块号 chan:PWM通道号  dir:PWM方向(0:输入 1:输出) duty:初始输出占空比 freq:初始输出频率
** @Output    : 初始化成功或失败：TRUE:成功;FALSE:失败
** @Function  : PWM模块初始化
** @The notes : 设备抽象层调用，上电初始化
**===========================================================================================*/
u8 BSPPWMInit(e_PWMMod module, e_PWMChan chan, u8 dir, u16 duty, u32 freq)
{
    /*模块未使能不进行初始化*/
    if(1 != gBSPFunCfg.Bits.PWMEn)
    {
        return(FALSE);
    }
    
    /*PWM输出初始化*/
	pwm_pal1Configs.pwmChannels->channel = chan;	/*PWM通道*/
	pwm_pal1Configs.pwmChannels->duty = duty;		/*占空比*/
	pwm_pal1Configs.pwmChannels->period = freq;		/*周期*/

	PINS_DRV_SetMuxModeSel(PORTE, ePIN_8, ePORT_MUX_ALT2);/*设置PTE8为PWM输出管脚*/
	PWM_Init(&pwm_pal1Instance, &pwm_pal1Configs);

    return(TRUE);
}

/*=============================================================================================
** @Name      : void BSPPWMSetData(e_PWMMod module, e_PWMChan chan, u16 duty, u32 freq)
** @Input     : Module:PWM对应FTM模块号 chan:PWM通道号  duty:初始输出占空比 freq:初始输出频率
** @Output    : void
** @Function  : 设置输出PWM波的占空比和频率
** @The notes : 用户需要改变输出PWM波占空比和频率时调用
**===========================================================================================*/
void BSPPWMSetData(e_PWMMod module, e_PWMChan chan, u16 duty, u32 freq)
{
	PWM_UpdateDuty(&pwm_pal1Instance, chan, duty);
}

/*=============================================================================================
** @Name      : void BSPPWMRcvISRHandle(e_PWMMod module, e_PWMChan chan)
** @Input     : Module:PWM对应FTM模块号 chan:PWM通道号
** @Output    : void
** @Function  : 输入PWM捕获中断处理函数
** @The notes : 输入PWM中断调用
**===========================================================================================*/
void BSPPWMRcvISRHandle(e_PWMMod module, e_PWMChan chan)
{
    u16 IC2Value = 0;
    //u16 dutyCycle = 0;
    //u32 frequency = 0;
    
    //模块未使能不执行
    if(1 != gBSPFunCfg.Bits.PWMEn)
    {
        return;
    }
    
    //读取IC2捕获寄存器的值，即为PWM周期的计数值
    //IC2Value = TIM_GetCapture2(TIM4);
    
    if(IC2Value != 0)
    {
        //读取IC1捕获寄存器的值，并计算占空比
        //dutyCycle = (TIM_GetCapture1(TIM4) * 100) / IC2Value;
        
        //计算PWM频率(主频6分频)
        //frequency = SystemCoreClock / IC2Value / 6;
    }
    else
    {
        //dutyCycle = 0;
        //frequency = 0;
    }
    
    //PWM波捕获处理函数
    //PWM_ISR_Rcv_Hook(dutyCycle, frequency);
}

/*=============================================================================================
** @Name      : u32 BSPReadPWMfrequencyAPI(void)
** @Input     : void
** @Output    : PWM周期：1HZ
** @Function  : 读取输入PWM周期
** @The notes : PWM捕获
**===========================================================================================*/
u32 BSPReadPWMfrequencyAPI(void)
{
	u16 counter = 0;
	u32 frequency = 0;
	u32 sysfrequency = 0;
	static u8 sTime = 0;
	static u8 sHisCoubt = 0;

    /*读取FTM2捕获寄存器的值，即为PWM周期的计数值*/
    counter = FTM_DRV_GetInputCaptureMeasurement(eFTM2, CHAN0_IDX);

    sysfrequency = FTM_DRV_GetFrequency(eFTM2);

    /*计数值不变*/
    if(counter == sHisCoubt)
    {
        /*连续10次*/
    	if(sTime >= 10)
    	{
            /*计数清零(实际PWM输入终止,SDK未清零)*/
    		counter = 0;
    	}
    	else
    	{
        	sTime++;
    	}
    }
	else
	{
    	sTime = 0;
    	sHisCoubt = counter;
	}

    if(counter != 0)
    {
        //计算PWM频率
        frequency = sysfrequency / counter;
    }
    else
    {
        frequency = 0;
    }

    return(frequency);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/

