/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPADC.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : ADC驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "adc_driver.h"
#include "pins_driver.h"
#include "BSPGPIO.h"
#include "BSPADC.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;
static u16 sADCChannelEn = 0;                	/*记录使能的ADC通道[按位表示]*/

/*ADC0初始化结构体*/
adc_converter_config_t Adc0InitConfig =
{
	ADC_CLK_DIVIDE_8,                /*!< Divider of the input clock for the ADC */
	(u8)ADC_DEFAULT_SAMPLE_TIME,	 /*!< Sample time in AD Clocks */
	ADC_RESOLUTION_12BIT,            /*!< ADC resolution (8,10,12 bit) */
	ADC_CLK_ALT_1,                   /*!< Input clock source */
	ADC_TRIGGER_SOFTWARE,            /*!< ADC trigger type (software, hardware) - affects only the first control channel */
	ADC_PRETRIGGER_SEL_SW,           /*!< Pretrigger source selected from Trigger Latching and Arbitration Unit - affects only the first 4 control channels */
	ADC_TRIGGER_SEL_PDB,             /*!< Trigger source selected from Trigger Latching and Arbitration Unit */
	false,                           /*!< Enable DMA for the ADC */
	ADC_VOLTAGEREF_VREF,             /*!< Voltage reference used */
	false,                           /*!< Enable Continuous conversions */
	false                            /*!< Only available for ADC 0. Enable internal supply monitoring - enables measurement of ADC_INPUTCHAN_SUPPLY_ sources. */
};

/*ADC1初始化结构体*/
adc_converter_config_t Adc1InitConfig =
{
	ADC_CLK_DIVIDE_8,                /*!< Divider of the input clock for the ADC */
	(u8)ADC_DEFAULT_SAMPLE_TIME,	 /*!< Sample time in AD Clocks */
	ADC_RESOLUTION_12BIT,            /*!< ADC resolution (8,10,12 bit) */
	ADC_CLK_ALT_1,                   /*!< Input clock source */
	ADC_TRIGGER_SOFTWARE,            /*!< ADC trigger type (software, hardware) - affects only the first control channel */
	ADC_PRETRIGGER_SEL_SW,           /*!< Pretrigger source selected from Trigger Latching and Arbitration Unit - affects only the first 4 control channels */
	ADC_TRIGGER_SEL_PDB,             /*!< Trigger source selected from Trigger Latching and Arbitration Unit */
	false,                           /*!< Enable DMA for the ADC */
	ADC_VOLTAGEREF_VREF,             /*!< Voltage reference used */
	false,                           /*!< Enable Continuous conversions */
	false                            /*!< Only available for ADC 0. Enable internal supply monitoring - enables measurement of ADC_INPUTCHAN_SUPPLY_ sources. */
};

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 ADCChanChangeToChannelx(e_ADCChan channel)
** @Input     : channel:通道号
** @Output    : 通道端口值
** @Function  : 转换对应的通道值
** @The notes : S32K144支持16Chan
**===========================================================================================*/
static u16 ADCChanChangeToChannelx(e_ADCChan channel);


/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPADCInit(e_ADCModule module, e_ADCChan *chan, u8 num)
** @Input     : module: ADC模块 *chan需要初始化的ADC通道序列 num:通道数目
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : ADC驱动模块初始化(只初始化ADC模块，不初始化AD通道)
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPADCInit(e_ADCModule module, e_ADCChan *chan, u8 num)
{
	(void)num;
	(void)chan;

    /*模块未使能不进行初始化*/
    if(1 != gBSPFunCfg.Bits.ADCEn)
    {
        return(FALSE);
    }
    
    /*输入参数错误*/
    if((chan == NULL) || (0 == num) || (num > eADCNUM) || (module > eADC_MODNUM))
    {
        return(FALSE);
    }


    if(eADC_MOD0 == module)
    {
        ADC_DRV_ConfigConverter(module, &Adc0InitConfig);/*ADC0初始化*/
        //ADC_DRV_GetConverterConfig(module, &watchtemp0);
        ADC_DRV_AutoCalibration(module);
    }
    else
    {
        ADC_DRV_ConfigConverter(module, &Adc1InitConfig);/*ADC1初始化*/
        //ADC_DRV_GetConverterConfig(module, &watchtemp1);
        ADC_DRV_AutoCalibration(module);
    }
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : void BSPADCStart(e_ADCModule module, e_ADCChan channel)
** @Input     : channel:ADC通道号
** @Output    : void
** @Function  : 启动ADC转换
** @The notes : 用户调用
**===========================================================================================*/
void BSPADCStart(e_ADCModule module, e_ADCChan channel)
{
	u16 ADCchan = 0;
	adc_chan_config_t AdcChanConfig;		/*ADC通道初始化,ADC0与ADC1通道数一样*/

    /*该通道未初始化使能*/
    if((channel >= eADCNUM) || (0 == BitGet(sADCChannelEn, channel)) || (module > eADC_MODNUM))
    {
        //return;
    }
    
    //通道转换
    ADCchan = ADCChanChangeToChannelx(channel);

    AdcChanConfig.interruptEnable = false;  /*该ADC通道中断使能*/
    AdcChanConfig.channel = (adc_inputchannel_t)ADCchan;/*选择ADC通道*/

    if(eADC_MOD0 == module)/*防止ADC0和ADC1复用管脚功能不一致*/
    {
        ADC_DRV_ConfigChan(module, 0, &AdcChanConfig);
        //ADC_DRV_GetChanConfig(module, 0, &watchChanConfig0);
    }
    else
    {
    	ADC_DRV_ConfigChan(module, 0, &AdcChanConfig);
        //ADC_DRV_GetChanConfig(module, 0, &watchChanConfig1);
    }
}

/*=============================================================================================
** @Name      : u16 BSPADCGetSampData(e_ADCModule module, e_ADCChan channel)
** @Input     : module:ADC模块	channel:ADC通道号
** @Output    : 采样值
** @Function  : 获取ADC采样AD值(已滤波)
** @The notes : 用户调用
**===========================================================================================*/
u16 BSPADCGetSampData(e_ADCModule module, e_ADCChan channel)
{
	//u16 ADCchan = 0;
    u16 tempData = 0;
    
    /*该通道未初始化使能*/
    if((channel >= eADCNUM) || (0 == BitGet(sADCChannelEn, channel)) || (module > eADC_MODNUM))
    {
        //return(0xffff);
    }
    
    /*通道转换*/
    //ADCchan = ADCChanChangeToChannelx(channel);

	/*等待AD转换完成*/
	ADC_DRV_WaitConvDone(module);
	/*获取AD值*/
	//ADC_DRV_GetChanResult(module, channel, &tempData);
	ADC_DRV_GetChanResult((uint32_t)module, 0, (uint16_t *)&tempData);

    return(tempData);
}

/*=============================================================================================
** @Name      : u8 BSPADCGetADCState(void)
** @Input     : void
** @Output    : 0:空闲 1:忙
** @Function  : 获取ADC采样通道工作状态
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPADCGetWorkState(void)
{
    return(0);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 ADCChanChangeToChannelx(e_ADCChan channel)
** @Input     : channel:通道号
** @Output    : 通道端口值
** @Function  : 转换对应的通道值
** @The notes : S32K144支持16Chan
**===========================================================================================*/
static u16 ADCChanChangeToChannelx(e_ADCChan channel)
{
    u16 data = 0;
    
    switch(channel)
    {
        case eADC0:
            data = ADC_INPUTCHAN_EXT0;
            break;
            
        case eADC1:
            data = ADC_INPUTCHAN_EXT1;
            break;
            
        case eADC2:
            data = ADC_INPUTCHAN_EXT2;
            break;
            
        case eADC3:
            data = ADC_INPUTCHAN_EXT3;
            break;
            
        case eADC4:
            data = ADC_INPUTCHAN_EXT4;
            break;
            
        case eADC5:
            data = ADC_INPUTCHAN_EXT5;
            break;
            
        case eADC6:
            data = ADC_INPUTCHAN_EXT6;
            break;
            
        case eADC7:
            data = ADC_INPUTCHAN_EXT7;
            break;
            
        case eADC8:
            data = ADC_INPUTCHAN_EXT8;
            break;
            
        case eADC9:
            data = ADC_INPUTCHAN_EXT9;
            break;
            
        case eADC10:
            data = ADC_INPUTCHAN_EXT10;
            break;
            
        case eADC11:
            data = ADC_INPUTCHAN_EXT11;
            break;
            
        case eADC12:
            data = ADC_INPUTCHAN_EXT12;
            break;
            
        case eADC13:
            data = ADC_INPUTCHAN_EXT13;
            break;
            
        case eADC14:
            data = ADC_INPUTCHAN_EXT14;
            break;
            
        case eADC15:
            data = ADC_INPUTCHAN_EXT15;
            break;
            
        default:
            data = 0;
            break;
    }
    
    return(data);
}




