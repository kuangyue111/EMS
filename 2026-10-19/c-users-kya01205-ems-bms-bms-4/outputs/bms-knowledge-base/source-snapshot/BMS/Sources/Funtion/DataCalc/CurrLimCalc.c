/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : CurrLimCalc.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 充放电电流限值计算模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "CalcUtil.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "PublicInfo.h"
#include "EnerInterface.h"
#include "CalcInterface.h"
#include "CharactCalc.h"
#include "CurrLimFall.h"
#include "CurrLimCalc.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
const s16 gTempCurLimitTable[TEMP_CURRLIMIT_NUM] =      //充放电倍率温度表
{
    -10, -5, 0, 5, 10, 15, 20, 25, 35, 40, 45, 50, 55, 60, 70
};

extern u16 gGLimCPInfo_51[];            /*电流功率限值*/
extern u16 gGBmuGenPara_102[];          /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];       /*主控高级参数值*/

extern u8 gGroupParaRO_115[];           /*不同温度充电倍率表*/
extern u8 gGroupParaRO_116[];           /*不同温度放电倍率表*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 CalcTempChgMulCurr(void)
** @Input     : void
** @Output    : 当前温度充电倍率下的电流限值 0.1A
** @Function  : 计算当前温度充电倍率下的电流限值
** @The notes : 查表
**===========================================================================================*/
static u16 CalcTempChgMulCurr(void);

/*=============================================================================================
** @Name      : static u16 CalcTempDhgMulCurr(void)
** @Input     : void
** @Output    : 当前温度放电倍率下的电流限值 0.1A
** @Function  : 计算当前温度放电倍率下的电流限值
** @The notes : 查表
**===========================================================================================*/
static u16 CalcTempDhgMulCurr(void);

/*=============================================================================================
** @Name      : static u16 ChgLimDownByTempH(s16 startTemp, s16 endTemp, s16 nowTemp, u16 currLim)
** @Input     : startTemp:开始降限值起始温度 endTemp:降限值为0的温度 nowTemp:当前温度  currLim:当前限值
** @Output    : 经高温降后的电流限值 0.1A
** @Function  : 根据高温降充电电流限值
** @The notes : 查表
**===========================================================================================*/
static u16 ChgLimDownByTempH(s16 startTemp, s16 endTemp, s16 nowTemp, u16 currLim);

/*=============================================================================================
** @Name      : static u16 DhgLimDownByTempH(s16 startTemp, s16 endTemp, s16 nowTemp, u16 currLim)
** @Input     : startTemp:开始降限值起始温度 endTemp:降限值为0的温度 nowTemp:当前温度  currLim:当前限值
** @Output    : 经高温降后的电流限值 0.1A
** @Function  : 根据高温降放电电流限值
** @The notes : 查表
**===========================================================================================*/
static u16 DhgLimDownByTempH(s16 startTemp, s16 endTemp, s16 nowTemp, u16 currLim);

/*=============================================================================================
** @Name      : static u16 ChgLimDownBySubTemp(u16 currLim)
** @Input     : currLim:当前限值 0.1A
** @Output    : 经温差降后的电流限值 0.1A
** @Function  : 根据温差降充电电流限值
** @The notes : 高温预警前只降1梯度,预警后降2梯度
**===========================================================================================*/
static u16 ChgLimDownBySubTemp(u16 currLim);

/*=============================================================================================
** @Name      : static u16 DhgLimDownBySubTemp(u16 currLim)
** @Input     : currLim:当前限值 0.1A
** @Output    : 经温差降后的电流限值 0.1A
** @Function  : 根据温差降放电电流限值
** @The notes : 高温预警前只降1梯度,预警后降2梯度
**===========================================================================================*/
static u16 DhgLimDownBySubTemp(u16 currLim);

/*=============================================================================================
** @Name      : static u16 ChgLimCurrByPowerLim(u16 chgUpCur)
** @Input     : chgUpCur 当前充电电流限值上限:0.1A
** @Output    : 经过充电功率限制后的充电电流限值:0.1A
** @Function  : 计算通过充电功率限制后的充电电流限值
** @The notes :
**===========================================================================================*/
static u16 ChgLimCurrByPowerLim(u16 chgUpCur);

/*=============================================================================================
** @Name      : static u16 DhgLimCurrByPowerLim(u16 dhgUpCur)
** @Input     : dhgUpCur 当前放电电流限值上限0.1A
** @Output    : 经过放电功率限制后的放电电流限值0.1A
** @Function  : 计算通过放电功率限制后的放电电流限值
** @The notes :
**===========================================================================================*/
static u16 DhgLimCurrByPowerLim(u16 dhgUpCur);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void CurrLimCalcTask(void)
** @Input     : void
** @Output    : void
** @Function  : 充放电电流限值计算任务
** @The notes : 100ms
**===========================================================================================*/
void CurrLimCalcTask(void)
{
    u16 chgHCurr   = 0;         /*充电过流值*/
    u16 dhgHCurr   = 0;         /*放电过流值*/
    u16 tempMulLim = 0;         /*当前温度充电倍率限值*/
    u16 chUpLim    = 0;         /*充电电流限值*/
    u16 dhUpLim    = 0;         /*放电电流限值*/
    
    s16 nowTemp  = 0;           /*当前温度最大值*/
    s16 fstTemp  = 0;           /*降限值起始温度*/
    s16 lstTemp  = 0;           /*降限值结束温度*/
    
    /*平滑电流限值任务*/
    CurrLimFallTask();                             /*计算平滑的电流限值*/

    /*系统正常才进行充放电电流限值计算*/
    if((GetGWorkStateAPI() != eWORK_RUN)
        && (GetGWorkStateAPI() != eWORK_IDLE)
        && (GetGWorkStateAPI() != eWORK_START)
        && (GetGWorkStateAPI() != eWORK_STOP))
    {
        gGLimCPInfo_51[eLimCPInfo51_ChgC] = 0;
        gGLimCPInfo_51[eLimCPInfo51_DhgC] = 0;
        gGLimCPInfo_51[eLimCPInfo51_ChgP] = 0;
        gGLimCPInfo_51[eLimCPInfo51_DhgP] = 0;
        return;
    }
    
    if(1 == CalcGetGropuNeedSysLimStateHook())      /*产生需充放电限制状态(系统告警或按钮停机)*/
    {
        gGLimCPInfo_51[eLimCPInfo51_ChgC] = 0;
        gGLimCPInfo_51[eLimCPInfo51_DhgC] = 0;
        gGLimCPInfo_51[eLimCPInfo51_ChgP] = 0;
        gGLimCPInfo_51[eLimCPInfo51_DhgP] = 0;
        return;
    }
    
    /*获取过流限值可用比率下最大限流值*/
    chgHCurr = GetChgHReqMaxCurr();                 /*充电过流限值*/
    dhgHCurr = GetDhgHReqMaxCurr();                 /*放电过流限值*/
    
    /*取当前温度下充放电倍率和过流值两者中较小值*/
    tempMulLim = CalcTempChgMulCurr();              /*获得当前温度充电倍率下电流限值*/
    
    if(tempMulLim > chgHCurr)                       /*充电倍率限值大于充电过流值*/
    {
        chUpLim = chgHCurr;                         /*取较小的过流限值*/
    }
    else
    {
        chUpLim = tempMulLim;                       /*取充电倍率限值和过流值中较小值*/
    }
    
    tempMulLim = CalcTempDhgMulCurr();              /*获得当前温度放电倍率下电流限值*/
      
    if(tempMulLim > dhgHCurr)                       /*放电倍率限值大于放电过流值*/
    {
        dhUpLim = dhgHCurr;                         /*取较小的过流限值*/
    }
    else
    {
        dhUpLim = tempMulLim;                       /*取放电倍率限值和过流值中较小值*/
    }
    
    /*充满或充电限制故障时限制充电*/
    if(1 == CalcGetGroupNeedChgLimStateHook())
    {
        chUpLim = 0;                                /*充电限值为0,不允许充电*/
    }
    /*计算温度平滑后充电限值*/
    else
    {
        /*高温降功率*/
        if((GetGCellMaxTempAPI() >= (CalcGetChgCDnFstHTempHook() - 3))  /*到达充电高温降限值起始点温度(回差3℃)*/
            && (1 == gGBmuHigLevPara_103[eBmuHigLevPara103_ChgTHLimEn]))/*充电高温降功率使能*/
        {
            nowTemp = GetGCellMaxTempAPI();
            fstTemp = CalcGetChgCDnFstHTempHook();
            lstTemp = CalcGetChgCDnLstHTempHook();
            
            /*通过高温降充电电流限值*/
            chUpLim = ChgLimDownByTempH(fstTemp, lstTemp, nowTemp, chUpLim);

            /*充电限流仍大于最低电流*/
            if(chUpLim > gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnMinC])
            {
                /*通过温差降充电电流限值*/
                chUpLim = ChgLimDownBySubTemp(chUpLim);
            }

            //温度低时电流不能低于最小大电流
            if((chUpLim < gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnMinC])
                && (nowTemp < ((s16)gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim] - 5)))
            {
                chUpLim = gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnMinC];
            }
        }
        #if(0)
        /*低温降功率*/
        else if((GetGCellMinTempAPI() <= (CalcGetChgCDnFstLTempHook() + 3))/*到达充电低温降限值起始点温度(回差3℃)*/
            && (1 == gGBmuHigLevPara_103[eBmuHigLevPara103_ChgTLLimEn]))/*充电低温降功率使能*/
        {
            nowTemp = GetGCellMinTempAPI();
            fstTemp = CalcGetChgCDnFstLTempHook();
            lstTemp = CalcGetChgCDnLstLTempHook();

            chUpLim = CurrLimDownByTempL(fstTemp, lstTemp, nowTemp, chUpLim);
        }
        #endif

        chUpLim = ChgLimCurrByPowerLim(chUpLim);    /*通过功率限制后获得充电电流限值*/
    }
    
    /*计算电压/SOC平滑后的充电电流限值*/
    tempMulLim = GetFallChgLimCurr();               /*获得平滑计算的充电电流限值*/
    
    if(tempMulLim < chUpLim)                        /*平滑计算的电流小于限值*/
    {
        chUpLim = tempMulLim;                       /*取较小的过流限值*/
    }
    
    /*放空或放电限制故障时限制放电*/
    if(1 == CalcGetGroupNeedDhgLimStateHook())
    {
        dhUpLim = 0;                                /*放电限值为0,不允许放电*/
    }
    /*计算温度平滑后放电限值*/
    else
    {
        /*高温降功率*/
        if((GetGCellMaxTempAPI() >= (CalcGetDhgCDnFstHTempHook() - 3))  /*到达放电高温降限值起始点温度(回差3℃)*/
            && (1 == gGBmuHigLevPara_103[eBmuHigLevPara103_DhgTHLimEn]))/*放电高温降功率使能*/
        {
            nowTemp = GetGCellMaxTempAPI();
            fstTemp = CalcGetDhgCDnFstHTempHook();
            lstTemp = CalcGetDhgCDnLstHTempHook();
            
            dhUpLim = DhgLimDownByTempH(fstTemp, lstTemp, nowTemp, dhUpLim);

            /*放电限流仍大于最低电流*/
            if(dhUpLim > gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnMinC])
            {
                /*通过温差降放电电流限值*/
                dhUpLim = DhgLimDownBySubTemp(dhUpLim);
            }

            //温度低时电流不能低于最小电流
            if((dhUpLim < gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnMinC])
                && (nowTemp < ((s16)gGBmuGenPara_102[eBmuGenPara102_DhgTH2Lim] - 10)))
            {
                dhUpLim = gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnMinC];
            }
        }
        #if(0)
        /*低温降功率*/
        else if((GetGCellMinTempAPI() <= (CalcGetDhgCDnFstLTempHook() + 3))/*到达放电低温降限值起始点温度(回差3℃)*/
            && (1 == gGBmuHigLevPara_103[eBmuHigLevPara103_DhgTLLimEn]))/*放电低温降功率使能*/
        {
            nowTemp = GetGCellMinTempAPI();
            fstTemp = CalcGetDhgCDnFstLTempHook();
            lstTemp = CalcGetDhgCDnLstLTempHook();

            dhUpLim = CurrLimDownByTempL(fstTemp, lstTemp, nowTemp, dhUpLim);
        }
        #endif
        
        dhUpLim = DhgLimCurrByPowerLim(dhUpLim);    /*通过功率限制后获得放电电流限值*/
    }
    
    /*计算电压/SOC平滑后的放电电流限值*/
    tempMulLim = GetFallDhgLimCurr();               /*获得平滑计算的放电电流限值*/
    
    if(tempMulLim < dhUpLim)                        /*平滑计算的电流小于限值*/
    {
        dhUpLim = tempMulLim;                       /*取较小的过流限值*/
    }
    
    gGLimCPInfo_51[eLimCPInfo51_ChgC] = chUpLim;
    gGLimCPInfo_51[eLimCPInfo51_DhgC] = dhUpLim;
    
    /*非运行状态限制功率为0*/
    if(eWORK_RUN == GetGWorkStateAPI())
    {
        gGLimCPInfo_51[eLimCPInfo51_ChgP] = (u16)(((u32)chUpLim * GetGSampSumVoltAPI() + 5000) / 10000);
        gGLimCPInfo_51[eLimCPInfo51_DhgP] = (u16)(((u32)dhUpLim * GetGSampSumVoltAPI() + 5000) / 10000);
    }
    else
    {
        gGLimCPInfo_51[eLimCPInfo51_ChgP] = 0;
        gGLimCPInfo_51[eLimCPInfo51_DhgP] = 0;
    }
}

/*=============================================================================================
** @Name      : u16 GetChgHReqMaxCurr(void)
** @Input     : void
** @Output    : 充电过流值下允许的最大电流限值 0.1A
** @Function  : 返回充电过流值下允许的最大电流限值
** @The notes : 配置值
**===========================================================================================*/
u16 GetChgHReqMaxCurr(void)
{
    u16 curr = 0;
    
    curr = gGBmuGenPara_102[eBmuGenPara102_ChgCMaxLim];
    
    /*最大允许电流不能超过过流告警值*/
    if(curr >= gGBmuGenPara_102[eBmuGenPara102_ChgCH2Lim])
    {
        if(gGBmuGenPara_102[eBmuGenPara102_ChgCH2Lim] > 20)
        {
            /*允许值比告警值小1A*/
            curr = gGBmuGenPara_102[eBmuGenPara102_ChgCH2Lim] - 10;
            //curr = gGBmuGenPara_102[eBmuGenPara102_ChgCH2Lim];
        }
        else
        {
            /*允许值为限值*/
            curr = gGBmuGenPara_102[eBmuGenPara102_ChgCH2Lim];
        }
    }
    
    return(curr);
}

/*=============================================================================================
** @Name      : u16 GetDhgHReqMaxCurr(void)
** @Input     : void
** @Output    : 放电过流值下允许的最大电流限值 0.1A
** @Function  : 返回放电过流值下允许的最大电流限值
** @The notes : 配置值
**===========================================================================================*/
u16 GetDhgHReqMaxCurr(void)
{
    u16 curr = 0;
    
    curr = gGBmuGenPara_102[eBmuGenPara102_DhgCMaxLim];
    
    /*最大允许电流不能超过过流告警值*/
    if(curr >= gGBmuGenPara_102[eBmuGenPara102_DhgCH2Lim])
    {
        if(gGBmuGenPara_102[eBmuGenPara102_DhgCH2Lim] > 20)
        {
            /*允许值比告警值小1A*/
            curr = gGBmuGenPara_102[eBmuGenPara102_DhgCH2Lim] - 10;
        }
        else
        {
            /*允许值为限值*/
            curr = gGBmuGenPara_102[eBmuGenPara102_DhgCH2Lim];
        }
    }
    
    return(curr);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 CalcTempChgMulCurr(void)
** @Input     : void
** @Output    : 当前温度充电倍率下的电流限值 0.1A
** @Function  : 计算当前温度充电倍率下的电流限值
** @The notes : 查表
**===========================================================================================*/
static u16 CalcTempChgMulCurr(void)
{
    u8 index = 0;
    s16 temp = 0;
    u32 result = 0;
    static u8 sHisIndex = 0;
    
    temp   = GetGCellAvgTempAPI();
    index  = SearchHigFromUpTabs16(temp, gTempCurLimitTable, TEMP_CURRLIMIT_NUM);//取高端(高温度)
    if(index >= TEMP_CURRLIMIT_NUM)
    {
        index = TEMP_CURRLIMIT_NUM - 1;
    }

    //索引变化1则防止温度变化1℃进入另一温度区间导致电流限值反复跳变
    if(((index == (sHisIndex + 1)) && (temp == (gTempCurLimitTable[sHisIndex] + 1)))//温度上升1℃
        /*|| (((index + 1) == sHisIndex) && (temp == gTempCurLimitTable[index]))*/) //温度下降1℃
    {
        //保持上次限值(直到温度变化2℃)
        index = sHisIndex;
    }
    //无变化或变化区间大
    else
    {
        sHisIndex = index;
    }

    result = (u32)gGroupParaRO_115[index] * GetGroupStandCapAPI() / 10000;

    return((u16)result);
}

/*=============================================================================================
** @Name      : static u16 CalcTempDhgMulCurr(void)
** @Input     : void
** @Output    : 当前温度放电倍率下的电流限值 0.1A
** @Function  : 计算当前温度放电倍率下的电流限值
** @The notes : 查表
**===========================================================================================*/
static u16 CalcTempDhgMulCurr(void)
{
    u8 index = 0;
    s16 temp = 0;
    u32 result = 0;
    static u8 sHisIndex = 0;
    
    temp  = GetGCellAvgTempAPI();
    index = SearchHigFromUpTabs16(temp, gTempCurLimitTable, TEMP_CURRLIMIT_NUM);//取高端(高温度)
    if(index >= TEMP_CURRLIMIT_NUM)
    {
        index = TEMP_CURRLIMIT_NUM - 1;
    }

    //索引变化1则防止温度变化1℃进入另一温度区间导致电流限值反复跳变
    if(((index == (sHisIndex + 1)) && (temp == (gTempCurLimitTable[sHisIndex] + 1)))//温度上升1℃
        /*|| (((index + 1) == sHisIndex) && (temp == gTempCurLimitTable[index]))*/) //温度下降1℃
    {
        //保持上次限值(直到温度变化2℃)
        index = sHisIndex;
    }
    //无变化或变化区间大
    else
    {
        sHisIndex = index;
    }

    /*
    if((temp < -20) && (SocSlowlyValueHook() < 50))                             //-20℃以下SOC小于5%
    {
        result = 0;                                                             //禁止放电
    }
    else if((temp < 0) && (SocSlowlyValueHook() < 50))                          //0℃以下SOC小于5%
    {
        result = (u32)gEepCfg_105[TEMP_CURRLIMIT_NUM + 2 + index] * GetGroupStandCapAPI() / 1000;
        
        if(result > 300)                                                        //限最大30A放电
        {
            result = 300;
        }
    }
    else
    {
        result = (u32)gEepCfg_105[TEMP_CURRLIMIT_NUM + 2 + index] * GetGroupStandCapAPI() / 1000;
    }
    */

    result = (u32)gGroupParaRO_116[index] * GetGroupStandCapAPI() / 10000;

    return((u16)result);
}

/*=============================================================================================
** @Name      : static u16 ChgLimDownByTempH(s16 startTemp, s16 endTemp, s16 nowTemp, u16 currLim)
** @Input     : startTemp:开始降限值起始温度 endTemp:降限值为0的温度 nowTemp:当前温度  currLim:当前限值
** @Output    : 经高温降后的电流限值 0.1A
** @Function  : 根据高温降充电电流限值
** @The notes : 查表
**===========================================================================================*/
static u16 ChgLimDownByTempH(s16 startTemp, s16 endTemp, s16 nowTemp, u16 currLim)
{
    u16 dowmCur = 0;                                        /*经高温降后的电流限值*/
    static s16 sHisTemp = 0;                                /*上次温度值*/
    
    if((startTemp >= endTemp)                               /*起止温度错误*/
        || ((nowTemp < startTemp)                           /*当前最大温度未达到降限值温度*/
        && (sHisTemp < startTemp)))                         /*之前温度未达到过降限值温度*/
    {
        sHisTemp = nowTemp;
        return(currLim);                                    /*不降限值*/
    }
    
    if(nowTemp > endTemp)                                   /*当前温度值超过电流限值为零时温度值*/
    {
        nowTemp = endTemp;
    }

    if((nowTemp < sHisTemp) && ((nowTemp + 2) >= sHisTemp)) /*温度跳动在2℃内(回差3℃)*/
    {
        nowTemp = sHisTemp;                                 /*当做温度未变化处理*/
    }
    else if((nowTemp < sHisTemp) && ((nowTemp + 3) >= sHisTemp)) /*温度跳动在3℃内(回差4℃)*/
    {
        nowTemp += 1;
        sHisTemp = nowTemp;                                 /*记录温度值高1℃(防止恢复后温度马上升1度跳变)*/
    }
    else
    {
        sHisTemp = nowTemp;                                 /*记录当次计算温度值*/
    }

    dowmCur = ((u16)(endTemp - nowTemp) * currLim) / (u16)(endTemp - startTemp + 1);  /*按当前温度超过一般过温限值比率降电流限值*/

    return(dowmCur);
}

/*=============================================================================================
** @Name      : static u16 DhgLimDownByTempH(s16 startTemp, s16 endTemp, s16 nowTemp, u16 currLim)
** @Input     : startTemp:开始降限值起始温度 endTemp:降限值为0的温度 nowTemp:当前温度  currLim:当前限值
** @Output    : 经高温降后的电流限值 0.1A
** @Function  : 根据高温降放电电流限值
** @The notes : 查表
**===========================================================================================*/
static u16 DhgLimDownByTempH(s16 startTemp, s16 endTemp, s16 nowTemp, u16 currLim)
{
    u16 dowmCur = 0;                                        /*经高温降后的电流限值*/
    static s16 sHisTemp = 0;                                /*上次温度值*/

    if((startTemp >= endTemp)                               /*起止温度错误*/
        || ((nowTemp < startTemp)                           /*当前最大温度未达到降限值温度*/
        && (sHisTemp < startTemp)))                         /*之前温度未达到过降限值温度*/
    {
        sHisTemp = nowTemp;
        return(currLim);                                    /*不降限值*/
    }

    if(nowTemp > endTemp)                                   /*当前温度值超过电流限值为零时温度值*/
    {
        nowTemp = endTemp;
    }

    if((nowTemp < sHisTemp) && ((nowTemp + 2) >= sHisTemp)) /*温度跳动在2℃内(回差3℃)*/
    {
        nowTemp = sHisTemp;                                 /*当做温度未变化处理*/
    }
    else if((nowTemp < sHisTemp) && ((nowTemp + 3) >= sHisTemp)) /*温度跳动在3℃内(回差4℃)*/
    {
        nowTemp += 1;
        sHisTemp = nowTemp;                                 /*记录温度值高1℃(防止恢复后温度马上升1度跳变)*/
    }
    else
    {
        sHisTemp = nowTemp;                                 /*记录当次计算温度值*/
    }

    dowmCur = ((u16)(endTemp - nowTemp) * currLim) / (u16)(endTemp - startTemp + 1);  /*按当前温度超过一般过温限值比率降电流限值*/

    return(dowmCur);
}

/*=============================================================================================
** @Name      : static u16 ChgLimDownBySubTemp(u16 currLim)
** @Input     : currLim:当前限值 0.1A
** @Output    : 经温差降后的电流限值 0.1A
** @Function  : 根据温差降充电电流限值
** @The notes : 高温预警前只降1梯度,预警后降2梯度
**===========================================================================================*/
static u16 ChgLimDownBySubTemp(u16 currLim)
{
    u16 dowmCur = 0;                                        /*经温差降后的电流限值*/
    s8 nowTemp = 0;                                         /*当前温度值(最高温)*/
    u8 subTemp = 0;                                         /*当前温差值*/
    static s16 sHisTemp = 0;                                /*上次温度值*/
    static u8 sHisSubTemp = 0;                              /*上次温差值*/

    nowTemp = GetGCellMaxTempAPI();
    if((nowTemp < sHisTemp) && ((nowTemp + 2) >= sHisTemp)) /*温度跳动在2℃内(回差3℃)*/
    {
        nowTemp = sHisTemp;                                 /*当做温度未变化处理*/
    }
    else if((nowTemp < sHisTemp) && ((nowTemp + 3) >= sHisTemp)) /*温度跳动在3℃内(回差4℃)*/
    {
        nowTemp += 1;
        sHisTemp = nowTemp;                                 /*记录温度值高1℃(防止恢复后温度马上升1度跳变)*/
    }
    else
    {
        sHisTemp = nowTemp;                                 /*记录当次计算温度值*/
    }

    subTemp = GetGCellMaxTempAPI() - GetGCellMinTempAPI();

    //温差变小
    if((subTemp < sHisSubTemp) && ((subTemp + 2) >= sHisSubTemp)) /*温差跳动在2℃内(回差3℃)*/
    {
        subTemp = sHisSubTemp;                              /*当做温差未变化处理*/
    }
    else if((subTemp < sHisSubTemp) && ((subTemp + 3) >= sHisSubTemp)) /*温差跳动在3℃内(回差4℃)*/
    {
        subTemp += 1;
        sHisSubTemp = subTemp;                              /*记录回差值大1℃(防止恢复后温差马上升1度跳变)*/
    }
    else
    {
        sHisSubTemp = subTemp;                              /*记录当次计算温差值*/
    }

    //达到降限值温差
    if(subTemp >= gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnDifT])
    {
        //超过降限值温度
        if(nowTemp >= (gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnFstT] + 3))
        {
            //降2个梯度
            dowmCur = currLim - ((u32)currLim * gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnRate] * 2 / 1000);
        }
        //达到降限值温度
        else if(nowTemp >= (CalcGetChgCDnFstHTempHook() - 1))
        {
            //只降1个梯度
            dowmCur = currLim - ((u32)currLim * gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnRate] / 1000);
        }
        //未达到降限值温度
        else
        {
            //记录当前温差
            sHisSubTemp = subTemp;
            //无需降限值
            dowmCur = currLim;
        }

        //限流不能低于最低电流
        if(dowmCur < gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnMinC])
        {
            dowmCur = gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnMinC];
        }
    }
    else
    {
        //无需降限值
        dowmCur = currLim;
    }

    return(dowmCur);
}

/*=============================================================================================
** @Name      : static u16 DhgLimDownBySubTemp(u16 currLim)
** @Input     : currLim:当前限值 0.1A
** @Output    : 经温差降后的电流限值 0.1A
** @Function  : 根据温差降放电电流限值
** @The notes : 高温预警前只降1梯度,预警后降2梯度
**===========================================================================================*/
static u16 DhgLimDownBySubTemp(u16 currLim)
{
    u16 dowmCur = 0;                                        /*经温差降后的电流限值*/
    s8 nowTemp = 0;                                         /*当前温度值(最高温)*/
    u8 subTemp = 0;                                         /*当前温差值*/
    static s16 sHisTemp = 0;                                /*上次温度值*/
    static u8 sHisSubTemp = 0;                              /*上次温差值*/

    nowTemp = GetGCellMaxTempAPI();
    if((nowTemp < sHisTemp) && ((nowTemp + 2) >= sHisTemp)) /*温度跳动在2℃内(回差3℃)*/
    {
        nowTemp = sHisTemp;                                 /*当做温度未变化处理*/
    }
    else if((nowTemp < sHisTemp) && ((nowTemp + 3) >= sHisTemp)) /*温度跳动在3℃内(回差4℃)*/
    {
        nowTemp += 1;
        sHisTemp = nowTemp;                                 /*记录温度值高1℃(防止恢复后温度马上升1度跳变)*/
    }
    else
    {
        sHisTemp = nowTemp;                                 /*记录当次计算温度值*/
    }

    subTemp = GetGCellMaxTempAPI() - GetGCellMinTempAPI();

    //温差变小
    if((subTemp < sHisSubTemp) && ((subTemp + 2) >= sHisSubTemp)) /*温差跳动在2℃内(回差3℃)*/
    {
        subTemp = sHisSubTemp;                              /*当做温差未变化处理*/
    }
    else if((subTemp < sHisSubTemp) && ((subTemp + 3) >= sHisSubTemp)) /*温差跳动在3℃内(回差4℃)*/
    {
        subTemp += 1;
        sHisSubTemp = subTemp;                              /*记录回差值大1℃(防止恢复后温差马上升1度跳变)*/
    }
    else
    {
        sHisSubTemp = subTemp;                              /*记录当次计算温差值*/
    }

    //达到降限值温差
    if(subTemp >= gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnDifT])
    {
        //超过降限值温度
        if(nowTemp >= (gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnFstT] + 3))
        {
            //2℃降1个梯度
            dowmCur = currLim - ((u32)currLim * gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnRate] * 2 / 1000);
        }
        //达到降限值温度
        else if(nowTemp >= (CalcGetDhgCDnFstHTempHook() - 1))
        {
            //只降1个梯度
            dowmCur = currLim - ((u32)currLim * gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnRate] / 1000);
        }
        //未达到降限值温度
        else
        {
            //记录当前温差
            sHisSubTemp = subTemp;
            //无需降限值
            dowmCur = currLim;
        }

        //限流不能低于最低电流
        if(dowmCur < gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnMinC])
        {
            dowmCur = gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnMinC];
        }
    }
    else
    {
        //无需降限值
        dowmCur = currLim;
    }

    return(dowmCur);
}

/*=============================================================================================
** @Name      : static u16 ChgLimCurrByPowerLim(u16 chgUpCur)
** @Input     : chgUpCur 当前充电电流限值上限:0.1A
** @Output    : 经过充电功率限制后的充电电流限值:0.1A
** @Function  : 计算通过充电功率限制后的充电电流限值
** @The notes :
**===========================================================================================*/
static u16 ChgLimCurrByPowerLim(u16 chgUpCur)
{
    u16 chgLimit = 0;                           /*充电电流限值*/
    u32 chgPowerCur = 0;                        /*按功率限值计算的电流限值*/
    u32 sumVolt = 0;                            /*组总电压*/
    
    sumVolt = GetGCellSumVoltAPI();
    
    if(sumVolt > 0)                             /*总电压不为0*/
    {
        chgPowerCur = (u32)gGBmuGenPara_102[eBmuGenPara102_ChgPMaxLim] * 10000 / sumVolt;
    }
    else                                        /*总电压为0V*/
    {
        chgPowerCur = 0;                        /*功率限制电流限值为0*/
    }
    
    if(chgUpCur <= chgPowerCur)
    {
        chgLimit = chgUpCur;                    /*取限值较小者*/
    }
    else
    {
        chgLimit = (u16)chgPowerCur;
    }
    
    return(chgLimit);
}

/*=============================================================================================
** @Name      : static u16 DhgLimCurrByPowerLim(u16 dhgUpCur)
** @Input     : dhgUpCur 当前放电电流限值上限0.1A
** @Output    : 经过放电功率限制后的放电电流限值0.1A
** @Function  : 计算通过放电功率限制后的放电电流限值
** @The notes :
**===========================================================================================*/
static u16 DhgLimCurrByPowerLim(u16 dhgUpCur)
{
    u16 dhgLimit = 0;                       /*放电电流限值*/
    u32 dhgPowerCur = 0;                    /*按功率限值计算的电流限值*/
    u32 sumVolt = 0;                        /*组总电压*/
    
    sumVolt = GetGCellSumVoltAPI();
    
    if(sumVolt > 0)                         /*总电压不为0*/
    {
        dhgPowerCur = (u32)gGBmuGenPara_102[eBmuGenPara102_DhgPMaxLim] * 10000 / sumVolt;
    }
    else                                    /*总电压为0V*/
    {
        dhgPowerCur = 0;                    /*功率限制电流限值为0*/
    }
    
    if(dhgUpCur <= dhgPowerCur)
    {
        dhgLimit = dhgUpCur;                /*取限值较小者*/
    }
    else
    {
        dhgLimit = (u16)dhgPowerCur;
    }
    
    return(dhgLimit);
}
