/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPADC.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : ADC驱动函数模块
** @Instructions :
**===========================================================================================*/
#ifndef _BSPADC_H
#define _BSPADC_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define ADC_SAMP_TIMES   8       //ADC通道采样次数(滤波次数)

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
//ADC0通道号定义
typedef enum
{
    eADC0 = 0,      //
    eADC1 = 1,      //
    eADC2 = 2,      //
    eADC3 = 3,      //
    eADC4 = 4,      //
    eADC5 = 5,      //
    eADC6 = 6,      //
    eADC7 = 7,      //
    eADC8 = 8,      //
    eADC9 = 9,      //
    eADC10 = 10,    //
    eADC11 = 11,    //
    eADC12 = 12,    //
    eADC13 = 13,    //
    eADC14 = 14,    //
    eADC15 = 15,    //

    eADCNUM
}e_ADCChan;

//ADC通道类型定义
typedef enum
{
    eADC_PA = 0,    //PA类型
    eADC_PB = 1,    //PB类型
    eADC_PC = 2,    //PC类型
    
    eADCTYPE
}e_ADCType;

//ADC模块类型定义
typedef enum
{
    eADC_MOD0 = 0,    //ADC模块1
    eADC_MOD1 = 1,    //ADC模块2

    eADC_MODNUM
}e_ADCModule;

//ADC通道管脚定义
typedef enum
{
    eADC_P0 = 0,    //P0管脚
    eADC_P1 = 1,    //P1管脚
    eADC_P2 = 2,    //P2管脚
    eADC_P3 = 3,    //P3管脚
    eADC_P4 = 4,    //P4管脚
    eADC_P5 = 5,    //P5管脚
    eADC_P6 = 6,    //P6管脚
    eADC_P7 = 7,    //P7管脚
    eADCPIN_A = 8,  //PA管脚数
    eADCPIN_B = 2,  //PB管脚数
    eADCPIN_C = 6,  //PC管脚数
}e_ADCPin;


/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPADCInit(e_ADCModule module, e_ADCChan *chan, u8 num)
** @Input     : module: ADC模块 *chan需要初始化的ADC通道序列 num:通道数目
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : ADC驱动模块初始化
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPADCInit(e_ADCModule module, e_ADCChan *chan, u8 num);

/*=============================================================================================
** @Name      : void BSPADCStart(e_ADCModule module, e_ADCChan channel)
** @Input     : channel:ADC通道号
** @Output    : void
** @Function  : 启动ADC转换
** @The notes : 用户调用
**===========================================================================================*/
void BSPADCStart(e_ADCModule module, e_ADCChan channel);

/*=============================================================================================
** @Name      : u16 BSPADCGetSampData(e_ADCModule module, e_ADCChan channel)
** @Input     : module:ADC模块	channel:ADC通道号
** @Output    : 采样值
** @Function  : 获取ADC采样AD值(已滤波)
** @The notes : 用户调用
**===========================================================================================*/
u16 BSPADCGetSampData(e_ADCModule module, e_ADCChan channel);

/*=============================================================================================
** @Name      : u8 BSPADCGetADCState(void)
** @Input     : void
** @Output    : 0:空闲 1:忙
** @Function  : 获取ADC采样通道工作状态
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPADCGetWorkState(void);

#endif
