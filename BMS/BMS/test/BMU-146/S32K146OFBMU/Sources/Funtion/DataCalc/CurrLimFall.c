/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : CurrLimFall.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 充放电电压限值下降调节模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "CalcUtil.h"
//#include "CurrLimPID.h"
#include "CalcInterface.h"
#include "CurrLimFall.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u8  sFallChgCIndex = 0xff;       /*平滑充电电流限值表索引*/
static u16 sFallChgMaxCurr = 0;         /*平滑充电电流最大电流值(恒压开始时的实际电流, 0.1A)*/
static u16 sFallChgCurrLim = 0;         /*平滑计算的充电电流限值*/

static u8  sFallDhgCIndex = 0xff;       /*平滑放电电流限值表索引*/
static u16 sFallDhgMaxCurr = 0;         /*平滑放电电流最大电流值(恒压开始时的实际电流, 0.1A)*/
static u16 sFallDhgCurrLim = 0;         /*平滑计算的放电电流限值*/

extern u16 gGLimCPInfo_51[];            /*电流功率限值*/
extern u16 gGBmuGenPara_102[];          /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];       /*主控高级参数值*/

extern u8 gGroupParaRO_119[];           /*EEP电压/SOC平滑电流限值参数表配置信息*/

//t_PICfg gChgCurrPICfg =
//{30,3,50,4,80,4000,-4000,100000,-100000,100000,50};/*充电电流限值PI调节参数 电流单位0.01A*/
//
//t_PIIO gChgCurrPiIO = {0,0,0,0};        /*用于充电电流环控制*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CVPIDCtrlChgCLim(void)
** @Input     : void
** @Output    : void
** @Function  : PID调节充电电流控制电压稳定
** @The notes :
**===========================================================================================*/
static void CVPIDCtrlChgCLim(void);

/*=============================================================================================
** @Name      : static u8 CVUtilPIDChgCLim(u32 curr)
** @Input     : curr:PID输出的充电电流值 0.01A
** @Output    : 0:输出电流值错误 1输出电流值正常
** @Function  : 限制PID输出充电电流大小
** @The notes :
**===========================================================================================*/
//static u8 CVUtilPIDChgCLim(u32 curr);

/*=============================================================================================
** @Name      : static void CVChgCurrLimPIDReset(void)
** @Input     : void
** @Output    : void
** @Function  : 恒压方式平滑充电电流限值PID参数复位
** @The notes :
**===========================================================================================*/
static void CVChgCurrLimPIDReset(void);

/*=============================================================================================
** @Name      : static u16 CVCalcChgPIDAimVolt(u8 index)
** @Input     : index:电流下标
** @Output    : PID目标电压:1mv
** @Function  : 计算恒压方式平滑充电电流限值PID目标电压
** @The notes : 从配置表查询
**===========================================================================================*/
static u16 CVCalcChgPIDAimVolt(u8 index);

/*=============================================================================================
** @Name      : static u16 CVCalcChgPIDAimCurr(u8 index)
** @Input     : index:电流下标
** @Output    : PID目标电流:0.1A
** @Function  : 计算恒压方式平滑充电电流限值PID目标电流
** @The notes : 从配置表查询
**===========================================================================================*/
static u16 CVCalcChgPIDAimCurr(u8 index);

/*=============================================================================================
** @Name      : static void CVPIDCtrlChgCLim(void)
** @Input     : void
** @Output    : void
** @Function  : 计算放电末端恒压放电电流限值下降步骤
** @The notes :
**===========================================================================================*/
static void CVCalcDhgCLimDnStep(void);

/*=============================================================================================
** @Name      : static void CVDhgCurrLimStepDnReset(void)
** @Input     : void
** @Output    : void
** @Function  : 恒压方式平滑放电电流限值步长降流参数复位
** @The notes :
**===========================================================================================*/
static void CVDhgCurrLimStepDnReset(void);

/*=============================================================================================
** @Name      : static u16 CVCalcDhgStepDnAimVolt(u8 index)
** @Input     : index:电流下标
** @Output    : 降流恒压目标电压:1mv
** @Function  : 计算恒压方式平滑放电电流限值降流恒压目标电压
** @The notes : 从配置表查询
**===========================================================================================*/
static u16 CVCalcDhgStepDnAimVolt(u8 index);

/*=============================================================================================
** @Name      : static u16 CVCalcDhgStepDnAimCurr(u8 index)
** @Input     : index:电流下标
** @Output    : PID目标电流:0.1A
** @Function  : 计算恒压方式平滑放电电流限值PID目标电流
** @The notes : 从配置表查询
**===========================================================================================*/
static u16 CVCalcDhgStepDnAimCurr(u8 index);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void CurrLimFallTask(void)
** @Input     : void
** @Output    : void
** @Function  : 充放电电流限值平滑任务
** @The notes : 100ms
**===========================================================================================*/
void CurrLimFallTask(void)
{
    u16 currLim = 0;
    u16 chgHCurr = 0;         /*充电过流值*/
    u16 dhgHCurr = 0;         /*放电过流值*/
    static u16 sHisMaxChgCurr = 0;
    static u16 sHisMaxDhgCurr = 0;

    /*确保电压采样正常才进行充放电电流限值计算*/
    if((GetGWorkStateAPI() == eWORK_INIT)
        || (GetGWorkStateAPI() == eWORK_OFF)
        || (2 != DiagGetRcvAllSlaveFlagAPI())
        || (2 != DiagGetRcvAllCellFlagAPI()))
    {
        sHisMaxChgCurr = 0;
        sHisMaxDhgCurr = 0;

        sFallChgCIndex = 0xff;
        sFallChgMaxCurr = 0;
        sFallChgCurrLim = GetChgHReqMaxCurr();

        sFallDhgCIndex = 0xff;
        sFallDhgMaxCurr = 0;
        sFallDhgCurrLim = GetDhgHReqMaxCurr();
        return;
    }

    /*获取过流限值可用比率下最大限流值*/
    chgHCurr = GetChgHReqMaxCurr();            /*充电过流限值*/
    dhgHCurr = GetDhgHReqMaxCurr();            /*放电过流限值*/
    
    /*使能充电末端PID恒压平滑电流限值*/
    if(1 == gGBmuHigLevPara_103[eBmuHigLevPara103_ChgFinCVEn])
    {
        if(sHisMaxChgCurr == chgHCurr)                  /*最大限值未改变(防止参数配置)*/
        {
            CVPIDCtrlChgCLim();                         /*PID平滑计算充电电流限值*/
            
            /*未开始恒压*/
            if(0xff == sFallChgCIndex)
            {
                sFallChgCurrLim = chgHCurr;             /*取最大限流值*/
            }
            /*开始恒压*/
            else
            {
                /*PID输出的电流合法 */
                //if(1 == CVUtilPIDChgCLim(gChgCurrPiIO.piOut))
                {
//                  currLim = (u16)gChgCurrPiIO.piOut / 10; /*0.01A转化为0.1A*/
//
//                  /*本次释放限值不能超过当前限流点限值*/
//                  if(currLim >= CVCalcChgPIDAimCurr(sFallChgCIndex))
//                  {
//                      currLim = CVCalcChgPIDAimCurr(sFallChgCIndex);
//                  }

                    //不用PI调节直接按步长下降
                    currLim = CVCalcChgPIDAimCurr(sFallChgCIndex);
                    
                    /*不能超过充电限值*/
                    if(currLim > chgHCurr)
                    {
                        currLim = chgHCurr;
                    }
                    
                    /*未到最后一个恒压点*/
                    if(sFallChgCIndex < (SLOW_CURRLIMIT_NUM - 1))
                    {
                        /*不能小于下一个限流点限值*/
                        if(currLim < (CVCalcChgPIDAimCurr(sFallChgCIndex + 1)- 10))
                        {
                            currLim = CVCalcChgPIDAimCurr(sFallChgCIndex + 1) - 10;
                        }
                    }
                }
                /*PID输出电流值异常*/
//              else
//              {
//                  /*取当前限流点*/
//                  currLim = CVCalcChgPIDAimCurr(sFallChgCIndex);
//              }

                /*不能小于截止电流 0.1A*/
                if(currLim < gGBmuGenPara_102[eBmuGenPara102_ChgCFinLim])
                {
                    currLim = gGBmuGenPara_102[eBmuGenPara102_ChgCFinLim];
                }

                sFallChgCurrLim = currLim;
            }
        }
        else                                            /*充电电流限值配置值变化*/
        {
            /*不平滑充电电流限值*/
            CVChgCurrLimPIDReset();                     /*充电电流限值PID参数复位*/
            sFallChgCurrLim = chgHCurr;
        }
    }
    /*未使能充电电流限值平滑*/
    else
    {
        sFallChgCurrLim = chgHCurr;                     /*充电电流限值取过流值*/
    }
    
    /*使能放电末端PID恒压平滑电流限值*/
    if(1 == gGBmuHigLevPara_103[eBmuHigLevPara103_DhgFinCVEn])
    {
        if(sHisMaxDhgCurr == dhgHCurr)                  /*放电最大限值未改变*/
        {
            CVCalcDhgCLimDnStep();                      /*计算末端降流步骤计算放电电流限值*/

            /*未开始恒压*/
            if(0xff == sFallDhgCIndex)
            {
                sFallDhgCurrLim = dhgHCurr;             /*取最大限流值*/
            }
            /*开始恒压*/
            else
            {
                /*取当前步骤对应的电流限值*/
                currLim  = CVCalcDhgStepDnAimCurr(sFallDhgCIndex);

                /*不能超过充电限值*/
                if(currLim > dhgHCurr)
                {
                    currLim = dhgHCurr;
                }

                /*未到最后一个恒压点*/
                if(sFallDhgCIndex < (SLOW_CURRLIMIT_NUM - 1))
                {
                    /*不能小于下一个限流点限值*/
                    if(currLim < (CVCalcDhgStepDnAimCurr(sFallDhgCIndex + 1)- 10))
                    {
                        currLim = CVCalcDhgStepDnAimCurr(sFallDhgCIndex + 1) - 10;
                    }
                }

                /*不能小于截止电流 0.1A*/
                if(currLim < gGBmuGenPara_102[eBmuGenPara102_DhgCFinLim])
                {
                    currLim = gGBmuGenPara_102[eBmuGenPara102_DhgCFinLim];
                }

                sFallDhgCurrLim = currLim;
            }
        }
        else                                            /*放电电流限值配置值变化*/
        {
            /*不平滑放电电流限值*/
            CVDhgCurrLimStepDnReset();
            sFallDhgCurrLim = dhgHCurr;                 /*放电电流限值取过流值*/
        }
    }
    /*未使能电流限值平滑*/
    else
    {
        sFallDhgCurrLim = dhgHCurr;                     /*放电电流限值取过流值*/
    }
    
    sHisMaxChgCurr = GetChgHReqMaxCurr();               /*充电过流限值*/
    sHisMaxDhgCurr = GetDhgHReqMaxCurr();               /*放电过流限值*/
}

/*=============================================================================================
** @Name      : void ResetCurrLimFallAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 复位充放电电流平滑限制
** @The notes : 平滑步骤清零
**===========================================================================================*/
void ResetCurrLimFallAPI(void)
{
    sFallChgCIndex = 0xff;
    sFallDhgCIndex = 0xff;
}

/*=============================================================================================
** @Name      : u8 GetFallChgCIndexStepAPI(void)
** @Input     : void
** @Output    : 放电电流限值
** @Function  : 返回平滑降充电电流限值当前进度步骤
** @The notes : 当前降到第几个点
**===========================================================================================*/
u8 GetFallChgCIndexStepAPI(void)
{
    return(sFallChgCIndex);
}

/*=============================================================================================
** @Name      : u8 GetFallDhgCIndexStepAPI(void)
** @Input     : void
** @Output    : 放电电流限值
** @Function  : 返回平滑降放电电流限值当前进度步骤
** @The notes : 当前降到第几个点
**===========================================================================================*/
u8 GetFallDhgCIndexStepAPI(void)
{
    return(sFallDhgCIndex);
}

/*=============================================================================================
** @Name      : u16 GetFallChgLimCurr(void)
** @Input     : void
** @Output    : 充电电流限值
** @Function  : 返回平滑计算的充电电流限值
** @The notes :
**===========================================================================================*/
u16 GetFallChgLimCurr(void)
{
    return(sFallChgCurrLim);
}

/*=============================================================================================
** @Name      : u16 GetFallDhgLimCurr(void)
** @Input     : void
** @Output    : 放电电流限值
** @Function  : 返回平滑计算的放电电流限值
** @The notes :
**===========================================================================================*/
u16 GetFallDhgLimCurr(void)
{
    return(sFallDhgCurrLim);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CVPIDCtrlChgCLim(void)
** @Input     : void
** @Output    : void
** @Function  : PID调节充电电流控制电压稳定
** @The notes :
**===========================================================================================*/
static void CVPIDCtrlChgCLim(void)
{
    //s32 aimVolt = 0;          /*目标电压*/
    u16 maxVolt = 0;            /*当前最大电压*/
    static u16 sHisMaxVolt = 0; /*上次最大电压*/
    
    /*当前最大电压*/
    maxVolt = GetGCellMaxVoltAPI();
    
    /*最大电压下降至严重小于起始压差或电压异常则重新开始恒压*/
    if(((maxVolt + CHG_FIN_CV_CLR_OFFV) < CVCalcChgPIDAimVolt(0))
        || (maxVolt >= (CalcGetChgLimCellVoltHook() + 500)))
    {
        sFallChgCIndex = 0xff;
    }
    else
    {
        /*未开始降流恒压*/
        if(0xff == sFallChgCIndex)
        {
            /*电压到达*/
            if(maxVolt >= CVCalcChgPIDAimVolt(0))
            {
                /*开始降流恒压*/
                sFallChgCIndex = 0;
                
                /*记录开始恒压时的电流值*/
                if(GetGSampOutCurrAPI() < 0)
                {
                    //当前实际电流小，降流起始点至少0.7C
                    if((0 - GetGSampOutCurrAPI()) <= (GetGBattAllCapAPI() * 7 / 10))
                    {
                    	//0.7C低于最大请求电流
                    	//if((GetGBattAllCapAPI() * 7 / 10) <= GetChgHReqMaxCurr())
                    	//{
                    		//降流点取0.7C
                    		sFallChgMaxCurr = GetGBattAllCapAPI() * 7 / 10;
                    	//}
                    	//else
                    	//{
                    		//降流点取最大请求电流值
                    	//	sFallChgMaxCurr = GetChgHReqMaxCurr();
                    	//}
                    }
                    //实际电流超过0.7C
                    else
                    {
                    	//起始点取实际电流
                        sFallChgMaxCurr = 0 - GetGSampOutCurrAPI();
                    }
                }
                else
                {
                    sFallChgMaxCurr = 0;
                }
            }
        }
        /*已经开始恒压*/
        else
        {
            /*未调节到最小值*/
            if(sFallChgCIndex < (SLOW_CURRLIMIT_NUM - 1))
            {
                /*恒压电流下降至目标电流且电压超过当前恒压点则抬升下一个恒压点*/
                //if((ABS(GetGSampOutCurrAPI(), 0) <= CVCalcChgPIDAimCurr(sFallChgCIndex))
                //    && (maxVolt >= (CVCalcChgPIDAimVolt(sFallChgCIndex) + 5)))
                /*当前电压超过下一个恒压点则抬升下一个恒压点*/
                if((maxVolt >= CVCalcChgPIDAimVolt(sFallChgCIndex + 1))
                    && (sHisMaxVolt >= CVCalcChgPIDAimVolt(sFallChgCIndex + 1)))
                {
                    sFallChgCIndex++;
                }
            }
            /*调节到最后一个恒压电*/
            else
            {
                /*保持*/
                sFallChgCIndex = SLOW_CURRLIMIT_NUM - 1;
            }
        }
    }
    
    sHisMaxVolt = maxVolt;

//    /*未开始恒压*/
//    if(0xff == sFallChgCIndex)
//    {
//        /*目标电压取第1个点*/
//        aimVolt = CVCalcChgPIDAimVolt(0);
//    }
//    /*已经开始恒压*/
//    else
//    {
//        /*计算目标电压*/
//        //aimVolt = CVCalcChgPIDAimVolt(sFallChgCIndex);/*恒压(根据电压PI调节电流)*/
//        aimVolt = CVCalcChgPIDAimVolt(sFallChgCIndex + 1);/*恒流(不调节电流)*/
//    }
//
//    /*检测电压差值值*/
//    gChgCurrPiIO.piIn = aimVolt - (s32)maxVolt;
//
//    /*根据电压差值进行PID调节电流限值*/
//    (void)UtilPIAdjust(&gChgCurrPiIO, &gChgCurrPICfg);
}

/*=============================================================================================
** @Name      : static u8 CVUtilPIDChgCLim(u32 curr)
** @Input     : curr:PID输出的充电电流值 0.01A
** @Output    : 0:输出电流值错误 1输出电流值正常
** @Function  : 限制PID输出充电电流大小
** @The notes :
**===========================================================================================*/
//static u8 CVUtilPIDChgCLim(u32 curr)
//{
//    if(curr < gChgCurrPICfg.piOutMin)
//    {
//        return(0);
//    }
//
//    if(curr > gChgCurrPICfg.piOutMax)
//    {
//        return(0);
//    }
//
//    return(1);
//}

/*=============================================================================================
** @Name      : static void CVChgCurrLimPIDReset(void)
** @Input     : void
** @Output    : void
** @Function  : 恒压方式平滑充电电流限值PID参数复位
** @The notes :
**===========================================================================================*/
static void CVChgCurrLimPIDReset(void)
{
//    s32 currLim = 0;
//
//    currLim = (u32)GetChgHReqMaxCurr() * 10;
//
//    /*重新初始化参数*/
//    gChgCurrPICfg.piSumMax = currLim;
//    gChgCurrPICfg.piSumMin = 0 - currLim;
//    gChgCurrPICfg.piOutMax = currLim;
//    gChgCurrPICfg.piOutMin = 10;
//
//    gChgCurrPiIO.piIn = 0;
//    gChgCurrPiIO.piOut = 0;
//    gChgCurrPiIO.piSum = 0;
//    gChgCurrPiIO.piInBak = 0;

    sFallChgCIndex = 0xff;
    sFallChgMaxCurr = 0;
    sFallChgCurrLim = 0;
}

/*=============================================================================================
** @Name      : static u16 CVCalcChgPIDAimVolt(u8 index)
** @Input     : index:电流下标
** @Output    : PID目标电压:1mv
** @Function  : 计算恒压方式平滑充电电流限值PID目标电压
** @The notes : 从配置表查询
**===========================================================================================*/
static u16 CVCalcChgPIDAimVolt(u8 index)
{
    u8 tabNum = 0;
    u16 volt = 0;
    
    if(index >= SLOW_CURRLIMIT_NUM)
    {
        index = SLOW_CURRLIMIT_NUM - 1;
    }
    
    /*计算所在数组中对应下标*/
    tabNum = (SLOW_CURRLIMIT_NUM + 1) * 2 + 1 + index;
    
    /*根据压差计算目标电压*/
    volt = CalcGetChgLimCellVoltHook() - (gGroupParaRO_119[tabNum] * 10);
    
    return(volt);
}

/*=============================================================================================
** @Name      : static u16 CVCalcChgPIDAimCurr(u8 index)
** @Input     : index:电流下标
** @Output    : PID目标电流:0.1A
** @Function  : 计算恒压方式平滑充电电流限值PID目标电流
** @The notes : 从配置表查询
**===========================================================================================*/
static u16 CVCalcChgPIDAimCurr(u8 index)
{
    u8 tabNum = 0;
    u16 curr = 0;
    
    if(index >= SLOW_CURRLIMIT_NUM)
    {
        index = SLOW_CURRLIMIT_NUM - 1;
    }
    
    /*计算所在数组中对应下标*/
    tabNum = 1 + index;
    
    /*记录恒压起始电流有效(必须大于最小限流值)*/
    if(sFallChgMaxCurr >= ((u32)GetChgHReqMaxCurr() * gGroupParaRO_119[SLOW_CURRLIMIT_NUM] / 100))
    {
        /*根据恒压起始电流和当前限制倍率计算目标电流*/
        curr = (u16)((u32)sFallChgMaxCurr * gGroupParaRO_119[tabNum] / 100);
    }
    /*未记录恒压起始电流或恒压起始点电流很小*/
    else
    {
        /*根据最大允许电流和当前限制倍率计算目标电流*/
        curr = (u16)((u32)GetChgHReqMaxCurr() * gGroupParaRO_119[tabNum] / 100);
    }
    
    /*最小限1A*/
    if(curr < 10)
    {
        curr = 10;
    }
    
    return(curr);
}

/*=============================================================================================
** @Name      : static void CVPIDCtrlChgCLim(void)
** @Input     : void
** @Output    : void
** @Function  : 计算放电末端恒压放电电流限值下降步骤
** @The notes :
**===========================================================================================*/
static void CVCalcDhgCLimDnStep(void)
{
    u16 minVolt = 0;           /*当前最小电压*/
    static u16 sHisMinVolt = 0;/*上次最小电压*/

    /*当前最低电压*/
    minVolt = GetGCellMinVoltAPI();

    /*最小电压上升至严重大于起始压差或电压异常则重新开始恒压*/
    if((minVolt > (CVCalcDhgStepDnAimVolt(0) + DHG_FIN_CV_CLR_OFFV))
        || ((minVolt + 500) <= CalcGetDhgLimCellVoltHook()))
    {
        sFallDhgCIndex = 0xff;
    }
    else
    {
        /*未开始降流恒压*/
        if(0xff == sFallDhgCIndex)
        {
            /*电压到达*/
            if(minVolt <= CVCalcDhgStepDnAimVolt(0))
            {
                /*开始降流恒压*/
                sFallDhgCIndex = 0;

                /*记录开始恒压时的电流值*/
                if(GetGSampOutCurrAPI() > 0)
                {
                    //当前实际电流小，降流起始点至少0.7C
                    if(GetGSampOutCurrAPI() <= (GetGBattAllCapAPI() * 7 / 10))
                    {
                    	//0.7C低于最大请求电流
                    	//if((GetGBattAllCapAPI() * 7 / 10) <= GetDhgHReqMaxCurr())
                    	//{
                    		//降流点取0.7C
                    		sFallDhgMaxCurr = GetGBattAllCapAPI() * 7 / 10;
                    	//}
                    	//else
                    	//{
                    		//降流点取最大请求电流值
                    	//	sFallDhgMaxCurr = GetDhgHReqMaxCurr();
                    	//}
                    }
                    //实际电流超过0.7C
                    else
                    {
                    	//起始点取实际电流
                    	sFallDhgMaxCurr = GetGSampOutCurrAPI();
                    }
                }
                else
                {
                    sFallDhgMaxCurr = 0;
                }
            }
        }
        /*已经开始恒压*/
        else
        {
            /*未调节到最小值*/
            if(sFallDhgCIndex < (SLOW_CURRLIMIT_NUM - 1))
            {
                /*恒压电流下降至目标电流且电压下降至当前恒压点则抬升下一个恒压点*/
                //if((ABS(GetGSampOutCurrAPI(), 0) <= CVCalcDhgPIDAimCurr(sFallDhgCIndex))
                //    && (minVolt <= (CVCalcDhgStepDnAimVolt(sFallDhgCIndex) + 5)))
                /*当前电压下降至下一个恒压点则抬升下一个恒压点*/
                if((minVolt <= CVCalcDhgStepDnAimVolt(sFallDhgCIndex + 1))
                    && (sHisMinVolt <= CVCalcDhgStepDnAimVolt(sFallDhgCIndex + 1)))
                {
                    sFallDhgCIndex++;
                }
            }
            /*调节到最后一个恒压电*/
            else
            {
                /*保持*/
                sFallDhgCIndex = SLOW_CURRLIMIT_NUM - 1;
            }
        }
    }

    sHisMinVolt = minVolt;
}

/*=============================================================================================
** @Name      : static void CVDhgCurrLimStepDnReset(void)
** @Input     : void
** @Output    : void
** @Function  : 恒压方式平滑放电电流限值步长降流参数复位
** @The notes :
**===========================================================================================*/
static void CVDhgCurrLimStepDnReset(void)
{
    sFallDhgCIndex = 0xff;
    sFallDhgMaxCurr = 0;
    sFallDhgCurrLim = 0;
}

/*=============================================================================================
** @Name      : static u16 CVCalcDhgStepDnAimVolt(u8 index)
** @Input     : index:电流下标
** @Output    : 降流恒压目标电压:1mv
** @Function  : 计算恒压方式平滑放电电流限值降流恒压目标电压
** @The notes : 从配置表查询
**===========================================================================================*/
static u16 CVCalcDhgStepDnAimVolt(u8 index)
{
    u8 tabNum = 0;
    u16 volt = 0;

    if(index >= SLOW_CURRLIMIT_NUM)
    {
        index = SLOW_CURRLIMIT_NUM - 1;
    }

    /*计算所在数组中对应下标*/
    tabNum = (SLOW_CURRLIMIT_NUM + 1) * 3 + 1 + index;

    /*根据压差计算目标电压*/
    volt = CalcGetDhgLimCellVoltHook() + (gGroupParaRO_119[tabNum] * 10);

    return(volt);
}

/*=============================================================================================
** @Name      : static u16 CVCalcDhgStepDnAimCurr(u8 index)
** @Input     : index:电流下标
** @Output    : PID目标电流:0.1A
** @Function  : 计算恒压方式平滑放电电流限值PID目标电流
** @The notes : 从配置表查询
**===========================================================================================*/
static u16 CVCalcDhgStepDnAimCurr(u8 index)
{
    u8 tabNum = 0;
    u16 curr = 0;

    if(index >= SLOW_CURRLIMIT_NUM)
    {
        index = SLOW_CURRLIMIT_NUM - 1;
    }

    /*计算所在数组中对应下标*/
    tabNum = SLOW_CURRLIMIT_NUM + 2 + index;

    /*记录恒压起始电流有效(必须大于最小限流值)*/
    if(sFallDhgMaxCurr >= ((u32)GetDhgHReqMaxCurr() * gGroupParaRO_119[SLOW_CURRLIMIT_NUM * 2 + 1 ] / 100))
    {
        /*根据恒压起始电流和当前限制倍率计算目标电流*/
        curr = (u16)((u32)sFallDhgMaxCurr * gGroupParaRO_119[tabNum] / 100);
    }
    /*未记录恒压起始电流或恒压起始点电流很小*/
    else
    {
        /*根据最大允许电流和当前限制倍率计算目标电流*/
        curr = (u16)((u32)GetDhgHReqMaxCurr() * gGroupParaRO_119[tabNum] / 100);
    }

    /*最小限1A*/
    if(curr < 10)
    {
        curr = 10;
    }

    return(curr);
}
