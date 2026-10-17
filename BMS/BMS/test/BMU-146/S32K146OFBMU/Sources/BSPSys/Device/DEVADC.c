/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : DEVADC.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : ADC采样通道管理函数模块
** @Instructions : 根据PCB外围设备编写管理函数
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPADC.h"
#include "BSPGPIO.h"
#include "pins_driver.h"
#include "DEVADC.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 DEVADCInit(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : ADC端口初始化
** @The notes : 只初始化板级提供的资源
**===========================================================================================*/
u8 DEVADCInit(void)
{
    u8 result = TRUE;
    e_ADCChan ADC0channel[eADC_IdLen] = {eADC0};
    e_ADCChan ADC1channel[eADC_IdLen] = {eADC0};
    
    /*罗列需要使用的ADC通道号*/
	ADC0channel[eADC_ACCV]    = eADC0,    	/*输入电源激活ACC信号,ADC0*/
	ADC0channel[eADC_MUXOut]  = eADC1,      /*电源管理芯片6503 MUX通道电压采集,ADC0*/
	ADC0channel[eADC_PWMV]    = eADC6,		/*PWM电压检测,ADC0*/
	ADC0channel[eADC_TEMP1]   = eADC7,	  	/*环境温度1,ADC0*/

	ADC1channel[eADC_HalSIG]  = eADC6,    	/*霍尔电流信号采集,ADC1*/
	ADC1channel[eADC_PwrV]    = eADC5,    	/*输入供电电源信号,ADC1*/
	ADC1channel[eADC_Hal5V]   = eADC7,    	/*霍尔供电电压采样(5V),ADC1*/
	ADC1channel[eADC_CdcV]    = eADC4,    	/*快充充电枪供入电压采样,ADC1*/
	ADC1channel[eADC_CC2V]    = eADC13,    	/*充电枪CC2信号电压采样,ADC1	*/
    ADC1channel[eADC_MCU5V]   = eADC3,    	/*MCU5V电源信号,ADC1*/
	ADC1channel[eADC_Aux5V]   = eADC2,    	/*电源管理芯片6503输出信号(5V),ADC1*/
	ADC1channel[eADC_TEMP2]   = eADC12,		/*环境温度2,ADC1*/
	ADC1channel[eADC_HS_AD]   = eADC8,      /*高边开关控制回检电流采集,ADC1*/

    /*初始化PORT功能(没有调用PE管脚功能初始化，这里需要初始化ADC管脚)*/
    PINS_DRV_SetMuxModeSel(PORTA, ePIN_0, PORT_PIN_DISABLED);	/*输入电源激活ACC信号,ADC0*/
	PINS_DRV_SetMuxModeSel(PORTA, ePIN_1, PORT_PIN_DISABLED);	/*电源管理芯片6503 MUX通道电压采集,ADC0*/
	PINS_DRV_SetMuxModeSel(PORTB, ePIN_2, PORT_PIN_DISABLED);	/*PWM电压检测,ADC0*/
	PINS_DRV_SetMuxModeSel(PORTB, ePIN_3, PORT_PIN_DISABLED);	/*环境温度1,ADC0*/
	PINS_DRV_SetMuxModeSel(PORTB, ePIN_13, PORT_PIN_DISABLED);	/*高边开关控制回检电流采集,ADC0*/

	PINS_DRV_SetMuxModeSel(PORTD, ePIN_4, PORT_PIN_DISABLED);	/*霍尔电流信号采集,ADC1*/
	PINS_DRV_SetMuxModeSel(PORTC, ePIN_7, PORT_PIN_DISABLED);	/*输入供电电源信号,ADC1*/
	PINS_DRV_SetMuxModeSel(PORTB, ePIN_12, PORT_PIN_DISABLED);	/*霍尔供电电压采样(5V),ADC1*/
	PINS_DRV_SetMuxModeSel(PORTC, ePIN_6, PORT_PIN_DISABLED);	/*快充充电枪供入电压采样,ADC1*/
	PINS_DRV_SetMuxModeSel(PORTA, ePIN_16, PORT_PIN_DISABLED);	/*充电枪CC2信号电压采样,ADC1	*/
	PINS_DRV_SetMuxModeSel(PORTD, ePIN_3, PORT_PIN_DISABLED);	/*MCU5V电源信号,ADC1*/
	PINS_DRV_SetMuxModeSel(PORTD, ePIN_2, PORT_PIN_DISABLED);	/*电源管理芯片6503输出信号(5V),ADC1*/
	PINS_DRV_SetMuxModeSel(PORTA, ePIN_15, PORT_PIN_DISABLED);	/*环境温度2,ADC1*/

	/*ADC通道初始化(S32K146有两个ADC模块)*/
    result = BSPADCInit(eADC_MOD0, ADC0channel, eADC_IdLen);
    result |= BSPADCInit(eADC_MOD1, ADC1channel, eADC_IdLen);
    
    return(result);
}

/*=============================================================================================
** @Name      : void DEVADCStart(e_DEVADCMod module, e_DevADCId channel)
** @Input     : channel:ADC通道序号
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 启动ADC通道转换
** @The notes : 用户调用
**===========================================================================================*/
void DEVADCStart(e_DEVADCMod module, e_DevADCId channel)
{
    /*根据通道序号启动对应AD通道*/
    switch((u8)channel)
    {
    	/*霍尔电流信号采集*/
		case eADC_HalSIG:
			BSPADCStart((e_ADCModule)module, eADC6);
			break;

		/*输入电源信号*/
		case eADC_PwrV:
			BSPADCStart((e_ADCModule)module, eADC5);
			break;

		/*输入电源激活信号*/
		case eADC_ACCV:
			BSPADCStart((e_ADCModule)module, eADC0);
			break;

		/*霍尔供电电压采样(5V)*/
		case eADC_Hal5V:
			BSPADCStart((e_ADCModule)module, eADC7);
			break;

		/*快充充电枪供入电压采样*/
		case eADC_CdcV:
			BSPADCStart((e_ADCModule)module, eADC4);
			break;

		/*充电枪CC2信号电压采样	*/
		case eADC_CC2V:
			BSPADCStart((e_ADCModule)module, eADC13);
			break;

		/*5V电源信号*/
		case eADC_MCU5V:
			BSPADCStart((e_ADCModule)module, eADC3);
			break;

		/*电源管理芯片6503输出信号(5V)*/
		case eADC_Aux5V:
			BSPADCStart((e_ADCModule)module, eADC2);
			break;

		/*MUX通道电压采集*/
		case eADC_MUXOut:
			BSPADCStart((e_ADCModule)module, eADC1);
			break;

		/*PWM电压检测*/
		case eADC_PWMV:
			BSPADCStart((e_ADCModule)module, eADC6);
			break;

		/*环境温度1*/
		case eADC_TEMP1:
			BSPADCStart((e_ADCModule)module, eADC7);
			break;

		/*环境温度2*/
		case eADC_TEMP2:
			BSPADCStart((e_ADCModule)module, eADC12);
			break;

		/*高边开关控制回检电流采集*/
		case eADC_HS_AD:
			BSPADCStart((e_ADCModule)module, eADC8);
			break;
        
        /*采样通道错误*/
        default:
            break;
    }
}

/*=============================================================================================
** @Name      : u16 DEVADCGetSampAD(e_DEVADCMod module, e_DevADCId channel)
** @Input     : channel:ADC通道序号
** @Output    : 采样值
** @Function  : 获取ADC采样AD值(已滤波)
** @The notes : 用户调用
**===========================================================================================*/
u16 DEVADCGetSampAD(e_DEVADCMod module, e_DevADCId channel)
{
    u16 adData = 0;
    
    /*根据通道序号获取对应通道AD值*/
    switch((u8)channel)
    {
		/*霍尔电流信号采集*/
		case eADC_HalSIG:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC6);
			break;

		/*输入电源信号*/
		case eADC_PwrV:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC5);
			break;

		/*输入电源激活信号*/
		case eADC_ACCV:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC0);
			break;

		/*霍尔供电电压采样(5V)*/
		case eADC_Hal5V:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC7);
			break;

		/*快充充电枪供入电压采样*/
		case eADC_CdcV:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC4);
			break;

		/*充电枪CC2信号电压采样	*/
		case eADC_CC2V:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC13);
			break;

		/*5V电源信号*/
		case eADC_MCU5V:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC3);
			break;

		/*电源管理芯片6503输出信号(5V)*/
		case eADC_Aux5V:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC2);
			break;

		/*MUX通道电压采集*/
		case eADC_MUXOut:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC1);
			break;

		/*PWM电压检测*/
		case eADC_PWMV:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC6);
			break;

		/*环境温度1*/
		case eADC_TEMP1:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC7);
			break;

		/*环境温度2*/
		case eADC_TEMP2:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC12);
			break;

		/*高边开关控制回检电流采集*/
		case eADC_HS_AD:
			adData = BSPADCGetSampData((e_ADCModule)module, eADC8);
			break;

        /*采样通道错误*/
        default:
            adData = 0xffff;
            break;
    }
    
    return(adData);
}
