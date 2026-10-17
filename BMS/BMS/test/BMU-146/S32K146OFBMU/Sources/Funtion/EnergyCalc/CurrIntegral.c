/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : CurrIntegral.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 电流积分模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "CurrIntegral.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static t_CAPINT sCapInt = 
{
    0,      /*充电电量积分量(单位:10mA1ms)*/
    0,      /*放电电量积分量(单位:10mA1ms)*/
    0,      /*充电电量积分量(单位:1mAH)*/
    0,      /*放电电量积分量(单位:1mAH)*/
    0,      /*充电电量计算量(单位:1AH)*/
    0,      /*放电电量计算量(单位:1AH)*/
    0       /*充放电电量变化量(单位:1mAH)*/
};

static t_ENERINT sEnerInt = 
{
	0,      /*充电电能积分(单位:1w1ms,即1AV1ms)*/
	0,      /*放电电能积分(单位:1w1ms,即1AV1ms)*/
	0,      /*充电电能积分(单位:1WH)*/
	0,      /*放电电能积分(单位:1WH)*/
	0,      /*充电电能计算(单位:0.1KWH)*/
	0,      /*放电电能计算(单位:0.1KWH)*/
    0       /*充放电电能变化量(单位:1WH)*/
};

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void ChgCapIntTask(u32 curr, u8 cycle)
** @Input     : curr:采样电流值(单位10mA) cycle:调用(积分)周期(1ms[1,10])
** @Output    : void
** @Function  : 充电电量积分任务
** @The notes : 最少10ms周期调用
**===========================================================================================*/
void ChgCapIntTask(u32 curr, u8 cycle)
{
    sCapInt.chgCap10ma1ms += (curr * cycle);
    /*计算充电电量变化量(负数)*/
    if(sCapInt.chgCap10ma1ms >= MULT_MAH_TO_10MAMS)                                 /*变化1mAH*/
    {
        sCapInt.changCap1ma1h -= (s32)(sCapInt.chgCap10ma1ms / MULT_MAH_TO_10MAMS); /*充电变化量递减*/
        
        sCapInt.chgCap1ma1h += (sCapInt.chgCap10ma1ms / MULT_MAH_TO_10MAMS);        /*转化为1mAH*/
        
        sCapInt.chgCap10ma1ms %= MULT_MAH_TO_10MAMS;
    }
    
    /*小储能直接用mAH为单位*/
    #if(0 == SMALL_ESS_EN)
    /*计算充电电量*/
    if(sCapInt.chgCap1ma1h >= MULT_AH_TO_MAH)                                       /*到达1AH*/
    {
        sCapInt.chgCap1a1h += (sCapInt.chgCap1ma1h / MULT_AH_TO_MAH);               /*转化为1mAH*/
        
        sCapInt.chgCap1ma1h %= MULT_AH_TO_MAH;
    }
    #endif
}

/*=============================================================================================
** @Name      : void DhgCapIntTask(u32 curr, u8 cycle)
** @Input     : curr:采样电流值(单位10mA) cycle:调用(积分)周期(1ms[1,10])
** @Output    : void
** @Function  : 放电电量积分任务
** @The notes : 最少10ms周期调用
**===========================================================================================*/
void DhgCapIntTask(u32 curr, u8 cycle)
{
    sCapInt.dhgCap10ma1ms += (curr * cycle);
    /*计算放电电量变化量(正数)*/
    if(sCapInt.dhgCap10ma1ms >= MULT_MAH_TO_10MAMS)                                 /*变化1mAH*/
    {
        sCapInt.changCap1ma1h += (s32)(sCapInt.dhgCap10ma1ms / MULT_MAH_TO_10MAMS); /*放电变化量递增*/
        
        sCapInt.dhgCap1ma1h += (sCapInt.dhgCap10ma1ms / MULT_MAH_TO_10MAMS);        /*转化为1mAH*/
        
        sCapInt.dhgCap10ma1ms %= MULT_MAH_TO_10MAMS;
    }
    
    /*小储能直接用mAH为单位*/
    #if(0 == SMALL_ESS_EN)
    /*计算放电电量*/
    if(sCapInt.dhgCap1ma1h >= MULT_AH_TO_MAH)                                       /*到达1AH*/
    {
        sCapInt.dhgCap1a1h += (sCapInt.dhgCap1ma1h / MULT_AH_TO_MAH);               /*转化为1AH*/
        
        sCapInt.dhgCap1ma1h %= MULT_AH_TO_MAH;
    }
    #endif
}

/*=============================================================================================
** @Name      : void ChgEnerIntTask(u32 curr, u16 volt, u8 cycle)
** @Input     : curr:采样电流值(单位10mA), volt采样电压值(单位:100mV) cycle:调用(积分)周期(1ms[1,10])
** @Output    : void
** @Function  : 充电电能积分任务
** @The notes : 最少10ms周期调用
**===========================================================================================*/
void ChgEnerIntTask(u32 curr, u16 volt, u8 cycle)
{
    static u32 sChg1mw1ms = 0;
    
    sChg1mw1ms += (curr * (u32)volt * cycle);                               /*10mA*100mV*1ms=1mAVms=1mw1ms*/
    if(sChg1mw1ms >= MULT_WMS_TO_MWMS)                                      /*达到1WMS*/
    {
        sEnerInt.chgEner1w1ms += (sChg1mw1ms / MULT_WMS_TO_MWMS);           /*转化为1WMS*/
        
        sChg1mw1ms %= MULT_WMS_TO_MWMS;
    }
    
    /*计算充电电能变化量(负数)*/
    if(sEnerInt.chgEner1w1ms >= MULT_WH_TO_WMS)                             /*变化1WH*/
    {
        sEnerInt.changEner1w1h -= (s32)(sEnerInt.chgEner1w1ms / MULT_WH_TO_WMS);
        
        sEnerInt.chgEner1w1h += (sEnerInt.chgEner1w1ms / MULT_WH_TO_WMS);   /*转化为1WH*/
        
        sEnerInt.chgEner1w1ms %= MULT_WH_TO_WMS;
    }
    
    /*小储能直接用WH为单位*/
    #if(0 == SMALL_ESS_EN)
    /*计算充电电能*/
    if(sEnerInt.chgEner1w1h >= MULT_100WH_TO_WH)                            /*到达0.1KWH*/
    {
        sEnerInt.chgEner100w1h += (sEnerInt.chgEner1w1h / MULT_100WH_TO_WH);/*转化为0.1KWH*/
        
        sEnerInt.chgEner1w1h %= MULT_100WH_TO_WH;
    }
    #endif
}

/*=============================================================================================
** @Name      : void DhgEnerIntTask(u32 curr, u16 volt, u8 cycle)
** @Input     : curr:采样电流值(单位10mA), volt采样电压值(单位:100mV) cycle:调用(积分)周期(1ms[1,10])
** @Output    : void
** @Function  : 放电电能积分任务
** @The notes : 最少10ms周期调用
**===========================================================================================*/
void DhgEnerIntTask(u32 curr, u16 volt, u8 cycle)
{
    static u32 sDhg1mw1ms = 0;
    
    sDhg1mw1ms += (curr * (u32)volt * cycle);                                  /*10mA*100mV*1ms=1mAVms=1mw1ms*/
    if(sDhg1mw1ms >= MULT_WMS_TO_MWMS)                                         /*达到1WMS*/
    {
        sEnerInt.dhgEner1w1ms += (sDhg1mw1ms / MULT_WMS_TO_MWMS);              /*转化为1WMS*/
        
        sDhg1mw1ms %= MULT_WMS_TO_MWMS;
    }
    
    /*计算放电电能变化量(正数)*/
    if(sEnerInt.dhgEner1w1ms >= MULT_WH_TO_WMS)                                 /*变化1WH*/
    {
        sEnerInt.changEner1w1h += (s32)(sEnerInt.dhgEner1w1ms / MULT_WH_TO_WMS);
        
        sEnerInt.dhgEner1w1h += (sEnerInt.dhgEner1w1ms / MULT_WH_TO_WMS);       /*转化为1WH*/
        
        sEnerInt.dhgEner1w1ms %= MULT_WH_TO_WMS;
    }
    
    /*小储能直接用WH为单位*/
    #if(0 == SMALL_ESS_EN)
    /*计算放电电能*/
    if(sEnerInt.dhgEner1w1h >= MULT_100WH_TO_WH)                                /*到达0.1KWH*/
    {
        sEnerInt.dhgEner100w1h += (sEnerInt.dhgEner1w1h / MULT_100WH_TO_WH);    /*转化为0.1KWH*/
        
        sEnerInt.dhgEner1w1h %= MULT_100WH_TO_WH;
    }
    #endif
}

/*=============================================================================================
** @Name      : u32 GetChgIntCapAPI(void)
** @Input     : void
** @Output    : 充电积分电量值(单位:1AH)
** @Function  : 获取充电积分电量值
** @The notes :
**===========================================================================================*/
u32 GetChgIntCapAPI(void)
{
    /*小储能直接用mAH为单位*/
    #if(1 == SMALL_ESS_EN)
    return(sCapInt.chgCap1ma1h);
    #else
    return(sCapInt.chgCap1a1h);
    #endif
}

/*=============================================================================================
** @Name      : u32 GetDhgIntCapAPI(void)
** @Input     : void
** @Output    : 放电积分电量值(单位:1AH)
** @Function  : 获取放电积分电量值
** @The notes :
**===========================================================================================*/
u32 GetDhgIntCapAPI(void)
{
    /*小储能直接用mAH为单位*/
    #if(1 == SMALL_ESS_EN)
    return(sCapInt.dhgCap1ma1h);
    #else
    return(sCapInt.dhgCap1a1h);
    #endif
}

/*=============================================================================================
** @Name      : s32 GetChgDhgChangCapAPI(void)
** @Input     : void
** @Output    : 充放电电量变化值(单位:1mAH,正数为放电量,负数为充电量)
** @Function  : 获取充放电电量变化值
** @The notes :
**===========================================================================================*/
s32 GetChgDhgChangCapAPI(void)
{
    return(sCapInt.changCap1ma1h);
}

/*=============================================================================================
** @Name      : u32 GetChgIntEnerAPI(void)
** @Input     : void
** @Output    : 充电积分电能值(单位:0.1KWH)
** @Function  : 获取充电积分电能值
** @The notes :
**===========================================================================================*/
u32 GetChgIntEnerAPI(void)
{
    /*小储能直接用WH为单位*/
    #if(1 == SMALL_ESS_EN)
    return(sEnerInt.chgEner1w1h);
    #else
    return(sEnerInt.chgEner100w1h);
    #endif
}

/*=============================================================================================
** @Name      : u32 GetDhgIntEnerAPI(void)
** @Input     : void
** @Output    : 放电积分电能值(单位:0.1KWH)
** @Function  : 获取放电积分电能值
** @The notes :
**===========================================================================================*/
u32 GetDhgIntEnerAPI(void)
{
    /*小储能直接用WH为单位*/
    #if(1 == SMALL_ESS_EN)
    return(sEnerInt.dhgEner1w1h);
    #else
    return(sEnerInt.dhgEner100w1h);
    #endif
}

/*=============================================================================================
** @Name      : s32 GetChgDhgChangEnerAPI(void)
** @Input     : void
** @Output    : 充放电电能变化值(单位:1WH,正数为放电能,负数为充电能)
** @Function  : 获取充放电电能变化值
** @The notes :
**===========================================================================================*/
s32 GetChgDhgChangEnerAPI(void)
{
    return(sEnerInt.changEner1w1h);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
