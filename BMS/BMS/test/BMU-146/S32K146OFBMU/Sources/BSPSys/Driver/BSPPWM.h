/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPPWM.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : PWM驱动函数模块
** @Instructions : PWM波输出接口和PWM捕获接口
**===========================================================================================*/
#ifndef _BSPPWM_H
#define _BSPPWM_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/

/*=============================================================================================
** @Macro definition
**===========================================================================================*/

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*PWM模块号*/
typedef enum
{
    ePWM_MOD0 = 0,    /* PWM模块1*/
    ePWM_MOD1 = 1,    /* PWM模块2*/
    ePWM_MOD2 = 2,    /* PWM模块3*/
    ePWM_MOD3 = 3,    /* PWM模块4*/

    ePWM_MODNUM
}e_PWMMod;

/*PWM通道号*/
typedef enum
{
    ePWM_CH0 = 0,    /* PWM通道1*/
    ePWM_CH1 = 1,    /* PWM通道2*/
    ePWM_CH2 = 2,    /* PWM通道3*/
    ePWM_CH3 = 3,    /* PWM通道4*/
    ePWM_CH4 = 4,    /* PWM通道5*/
    ePWM_CH5 = 5,    /* PWM通道6*/
    ePWM_CH6 = 6,    /* PWM通道7*/
    ePWM_CH7 = 7,    /* PWM通道8*/

    ePWM_CHANNUM
}e_PWMChan;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPPWMInit(e_PWMMod module, e_PWMChan chan, u8 dir, u16 duty, u32 freq)
** @Input     : Module:PWM对应FTM模块号 chan:PWM通道号  dir:PWM方向(0:输入 1:输出) duty:初始输出占空比 freq:初始输出频率
** @Output    : 初始化成功或失败：TRUE:成功;FALSE:失败
** @Function  : PWM模块初始化
** @The notes : 设备抽象层调用，上电初始化
**===========================================================================================*/
u8 BSPPWMInit(e_PWMMod module, e_PWMChan chan, u8 dir, u16 duty, u32 freq);

/*=============================================================================================
** @Name      : void BSPPWMSetData(e_PWMMod module, e_PWMChan chan, u16 duty, u32 freq)
** @Input     : Module:PWM对应FTM模块号 chan:PWM通道号  duty:初始输出占空比 freq:初始输出频率
** @Output    : void
** @Function  : 设置输出PWM波的占空比和频率
** @The notes : 用户需要改变输出PWM波占空比和频率时调用
**===========================================================================================*/
void BSPPWMSetData(e_PWMMod module, e_PWMChan chan, u16 duty, u32 freq);

/*=============================================================================================
** @Name      : void BSPPWMRcvISRHandle(e_PWMMod module, e_PWMChan chan)
** @Input     : Module:PWM对应FTM模块号 chan:PWM通道号
** @Output    : void
** @Function  : 输入PWM捕获中断处理函数
** @The notes : 输入PWM中断调用
**===========================================================================================*/
void BSPPWMRcvISRHandle(e_PWMMod module, e_PWMChan chan);

/*=============================================================================================
** @Name      : u32 BSPReadPWMfrequencyAPI(void)
** @Input     : void
** @Output    : PWM周期：1HZ
** @Function  : 读取输入PWM周期
** @The notes : PWM捕获
**===========================================================================================*/
u32 BSPReadPWMfrequencyAPI(void);

#endif

