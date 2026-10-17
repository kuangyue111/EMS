/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : EnerInfoCalc.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 充放电电量电能计算模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "ParaInterface.h"
#include "CapInfoCalc.h"
#include "EnerInterface.h"
#include "EnerInfoCalc.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static t_ENERFORM sEnerForm =
{
	ENER_ZERO_POINT + 100000,       /*当前能量(单位:1WH)*/
	ENER_ZERO_POINT + 100000,       /*剩余能量(单位:1WH)*/
    ENER_ZERO_POINT,                /*底端能量(单位:1WH)*/
    ENER_ZERO_POINT + 205000,       /*顶端能量(单位:1WH)*/
    215000                          /*标称能量(单位:1WH)*/
};

extern s16 gGSysMainInfo_60[];		/*系统整体信息*/
extern u16 gGSysCapInfo_61[];		/*系统电量信息数组*/
extern u32 gGStatistHisCap_73[];	/*累计电量统计信息*/
extern u32 gGStatistHisEner_74[];	/*累计电能统计信息*/

extern u16 gGBmuGenPara_102[];		/*主控通用参数值*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CalcGroupNowEnerHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 计算电池组当前电能处理
** @The notes : 实际可放电能量
**===========================================================================================*/
static void CalcGroupNowEnerHandle(void);

/*=============================================================================================
** @Name      : static void CalcGroupRemainEnerHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 计算电池组剩余电能
** @The notes : 包含不可用的电能
**===========================================================================================*/
static void CalcGroupRemainEnerHandle(void);

/*=============================================================================================
** @Name      : static void StoreGroupNowEnerToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储当前可用电能到EEP
** @The notes : 判断存储周期
**===========================================================================================*/
static void StoreGroupNowEnerToEEP(void);

/*=============================================================================================
** @Name      : static void StoreGroupRemainEnerToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储当前可用电能到EEP
** @The notes : 判断存储周期
**===========================================================================================*/
static void StoreGroupRemainEnerToEEP(void);

/*=============================================================================================
** @Name      : static void StoreGroupTotalEnerToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储实际总电能到EEP
** @The notes : 直接写存储
**===========================================================================================*/
static void StoreGroupTotalEnerToEEP(void);

/*=============================================================================================
** @Name      : static void StoreGroupStandEnerToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储实际标称电能到EEP
** @The notes : 直接写存储
**===========================================================================================*/
static void StoreGroupStandEnerToEEP(void);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void GroupEnerSoeInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组当前电能SOE计算初始化
** @The notes :
**===========================================================================================*/
void GroupEnerSoeInit(void)
{
	u32 data[5] = {0};

	if(TRUE == ParaReadStoreEnerFormInfo(data, 5))
	{
		sEnerForm.nowEner = ENER_ZERO_POINT + data[1];
		sEnerForm.remEner = ENER_ZERO_POINT + data[2];
		sEnerForm.topEner = ENER_ZERO_POINT + data[3];
		sEnerForm.baseEner = ENER_ZERO_POINT;
		sEnerForm.standEner = data[4];
	}
	else
	{
		sEnerForm.nowEner = ENER_ZERO_POINT + ((u32)gGBmuGenPara_102[eBmuGenPara102_NowSoe] * gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] / 10);
		sEnerForm.remEner = sEnerForm.nowEner;
		//sEnerForm.topEner = ENER_ZERO_POINT + gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 100;
		sEnerForm.baseEner = ENER_ZERO_POINT;
		//sEnerForm.standEner = (u32)gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 100;
		//总电能无小数则为标称电能(总电能为97%)
		if(0 == (gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] % 10))
		{
			sEnerForm.topEner = ENER_ZERO_POINT + (gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 9700 / 100);
			sEnerForm.standEner = (u32)gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 100;
		}
		//有小数,未知标称电能认为总电能为97%
		else
		{
			sEnerForm.topEner = ENER_ZERO_POINT + gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 100;
			sEnerForm.standEner = (u32)gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 10000 / 97;
		}
	}
}

/*=============================================================================================
** @Name      : void GroupEnerSoeCalcTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组当前电能SOE计算任务
** @The notes : 100ms调用,测算当前能量和SOE
**===========================================================================================*/
void GroupEnerSoeCalcTask(void)
{
    static u8 sState = 0;
    
    switch(sState)
    {
        case 0:
        	CalcGroupNowEnerHandle();   /*计算当前电能*/
        	StoreGroupNowEnerToEEP();   /*写当前可用电能到EEP*/
            sState = 1;
            break;
            
        case 1:
        	CalcGroupRemainEnerHandle();/*计算剩余电能*/
        	StoreGroupRemainEnerToEEP();/*写剩余电能到EEP*/
            sState = 0;
            break;
            
        default:
            sState = 0;
            break;
    }
}

/*=============================================================================================
** @Name      : void CorrGNowEnerBySoeAPI(u16 soe)
** @Input     : soe:目标SOE(0.1%)
** @Output    : void
** @Function  : 根据电池组SOE修正当前电能
** @The notes : 用户需要修正电池组当前电能时调用
**===========================================================================================*/
void CorrGNowEnerBySoeAPI(u16 soe)
{
	u32 nowEner = 0;
    u32 lowEner = 0;
    u32 allEner = 0;

    lowEner = sEnerForm.baseEner;
    allEner = sEnerForm.topEner;

    if(allEner <= lowEner)
    {
        return;
    }

    nowEner = (allEner - lowEner) * soe / 1000 + ENER_ZERO_POINT;

    if(nowEner > allEner)
    {
    	nowEner = allEner;
    }
    else if(nowEner < lowEner)
    {
    	nowEner = lowEner;
    }

    sEnerForm.nowEner = nowEner;

    /*写当前实际电能到EEP*/
    StoreGroupNowEnerToEEP();
}

/*=============================================================================================
** @Name      : void CorrGRemainEnerBySoeAPI(u16 soe)
** @Input     : soe:目标SOE(0.1%)
** @Output    : void
** @Function  : 根据电池组SOE修正剩余电能
** @The notes : 用户需要修正电池组剩余电能时调用
**===========================================================================================*/
void CorrGRemainEnerBySoeAPI(u16 soe)
{
	u32 remEner = 0;
    u32 lowEner = 0;
    u32 allEner = 0;

    lowEner = sEnerForm.baseEner;
    allEner = sEnerForm.topEner;

    if(allEner <= lowEner)
    {
        return;
    }

    remEner = (allEner - lowEner) * soe / 1000 + ENER_ZERO_POINT;

    if(remEner > allEner)
    {
    	remEner = allEner;
    }
    else if(remEner < lowEner)
    {
    	remEner = lowEner;
    }

    sEnerForm.remEner = remEner;

    /*写剩余电能到EEP*/
    StoreGroupRemainEnerToEEP();
}

/*=============================================================================================
** @Name      : void CorrGroupTotalEnerAPI(u32 ener)
** @Input     : ener:电池组总能量(单位:1Wh)
** @Output    : void
** @Function  : 修正电池组总能量
** @The notes :
**===========================================================================================*/
void CorrGroupTotalEnerAPI(u32 ener)
{
    u32 soe = 0;
    u32 nowEner = 0;
    u32 remEner = 0;
    u32 lowEner = 0;
    u32 allEner = 0;
    u32 befEner = 0;

    nowEner = sEnerForm.nowEner;
    remEner = sEnerForm.remEner;
    lowEner = sEnerForm.baseEner;
    allEner = sEnerForm.topEner;
    befEner = sEnerForm.topEner - ENER_ZERO_POINT;

    if(allEner <= lowEner)                      /*变量异常*/
    {
        ener = 0;
        nowEner = ENER_ZERO_POINT;
        remEner = ENER_ZERO_POINT;
        allEner = ENER_ZERO_POINT;
    }
    else
    {
    	/*求顶端容量*/
        allEner = ener + ENER_ZERO_POINT;       /*顶端能量*/

    	/*求当前电能*/
        if(nowEner >= allEner)                  /*变量异常*/
        {
            soe = 10000;
        }
        else if(nowEner <= lowEner)             /*变量异常*/
        {
            soe = 0;
        }
        else
        {
            soe = (nowEner - lowEner) * 10000 / befEner;
        }

        nowEner = ener * soe / 10000 + ENER_ZERO_POINT;
        if(nowEner > allEner)                   /*计算当前能量超过总能量*/
        {
            nowEner = allEner;
        }

    	/*求剩余电能*/
        if(remEner >= allEner)                  /*变量异常*/
        {
            soe = 10000;
        }
        else if(remEner <= lowEner)             /*变量异常*/
        {
            soe = 0;
        }
        else
        {
            soe = (remEner - lowEner) * 10000 / befEner;
        }

        remEner = ener * soe / 10000 + ENER_ZERO_POINT;
        if(remEner > allEner)                   /*计算当前能量超过总能量*/
        {
        	remEner = allEner;
        }
    }

    sEnerForm.nowEner = nowEner;
    sEnerForm.remEner = remEner;
    sEnerForm.topEner = allEner;

    StoreGroupNowEnerToEEP();                   /*写当前实际能量到EEP*/
    StoreGroupRemainEnerToEEP();                /*写当前剩余能量到EEP*/
    StoreGroupTotalEnerToEEP();                 /*写电池组总能量到EEP*/

    EnerAftTotalEnerCorrDealHook(befEner, ener);/*总能量修正后事件*/
}

/*=============================================================================================
** @Name      : void CorrGroupStandEnerAPI(u32 ener)
** @Input     : ener:电池组标称能量(单位:1Wh)
** @Output    : void
** @Function  : 修正电池组标称能量
** @The notes :
**===========================================================================================*/
void CorrGroupStandEnerAPI(u32 ener)
{
    sEnerForm.standEner = ener;

    StoreGroupStandEnerToEEP();                 /*写电池组标称能量到EEP*/
}

/*=============================================================================================
** @Name      : u32 GetGroupNowEnerAPI(void)
** @Input     : void
** @Output    : 电池组当前电能,即可用电能(单位:1WH)
** @Function  : 获取电池组当前电能
** @The notes : 可以使用的电能
**===========================================================================================*/
u32 GetGroupNowEnerAPI(void)
{
    return(sEnerForm.nowEner - ENER_ZERO_POINT);
}

/*=============================================================================================
** @Name      : u32 GetGroupRemainEnerAPI(void)
** @Input     : void
** @Output    : 电池组剩余电能 (单位:1WH)
** @Function  : 获取电池组剩余电能
** @The notes : 包括不能使用的电能
**===========================================================================================*/
u32 GetGroupRemainEnerAPI(void)
{
    return(sEnerForm.remEner - ENER_ZERO_POINT);
}

/*=============================================================================================
** @Name      : u32 GetGroupTotalEnerAPI(void)
** @Input     : void
** @Output    : 电池组总电能(单位:1WH)
** @Function  : 获取电池组总电能
** @The notes :
**===========================================================================================*/
u32 GetGroupTotalEnerAPI(void)
{
    return(sEnerForm.topEner - ENER_ZERO_POINT);
}

/*=============================================================================================
** @Name      : u32 GetGroupStandEnerAPI(void)
** @Input     : void
** @Output    : 电池组标称电能(单位:1WH)
** @Function  : 获取电池组标称电能
** @The notes :
**===========================================================================================*/
u32 GetGroupStandEnerAPI(void)
{
    return(sEnerForm.standEner);
}

/*=============================================================================================
** @Name      : u16 GetGRealSoeTenThousAPI(void)
** @Input     : void
** @Output    : 电池组当前真实SOE值(0.01%)
** @Function  : 获取电池组当前真实SOE值
** @The notes : 实际可用值，分辨率万分之一
**===========================================================================================*/
u16 GetGRealSoeTenThousAPI(void)
{
    u32 ener = 0;
    u32 nowEner = 0;
    u32 lowEner = 0;
    u32 allEner = 0;

    nowEner = sEnerForm.nowEner;
    lowEner = sEnerForm.baseEner;
    allEner = sEnerForm.topEner;

    if(nowEner >= allEner)
    {
    	ener = 10000;
    }
    else if((nowEner <= lowEner) || ((allEner <= lowEner)))
    {
    	ener = 0;
    }
    else
    {
    	ener = (nowEner - lowEner) * 10000 / (allEner - lowEner);

        if(ener > 10000)
        {
        	ener = 10000;
        }
    }

    return((u16)ener);
}

/*=============================================================================================
** @Name      : u16 GetGRealSoeMilliAPI(void)
** @Input     : void
** @Output    : 电池组当前真实SOE值(0.1%)
** @Function  : 获取电池组当前真实SOE值
** @The notes : 实际可用值，分辨率千分之一
**===========================================================================================*/
u16 GetGRealSoeMilliAPI(void)
{
    u32 ener = 0;
    u32 nowEner = 0;
    u32 lowEner = 0;
    u32 allEner = 0;

    nowEner = sEnerForm.nowEner;
    lowEner = sEnerForm.baseEner;
    allEner = sEnerForm.topEner;

    if(nowEner >= allEner)
    {
    	ener = 1000;
    }
    else if((nowEner <= lowEner) || ((allEner <= lowEner)))
    {
    	ener = 0;
    }
    else
    {
    	ener = (nowEner - lowEner) * 1000 / (allEner - lowEner);

        if(ener > 1000)
        {
        	ener = 1000;
        }
    }

    return((u16)ener);
}

/*=============================================================================================
** @Name      : u16 GetGRemainSoeMilliAPI(void)
** @Input     : void
** @Output    : 剩余SOE值(0.1%)
** @Function  : 获取剩余SOE值
** @The notes : 相对总能量剩余值，分辨率千分之一
**===========================================================================================*/
u16 GetGRemainSoeMilliAPI(void)
{
    u32 ener = 0;
    u32 remEner = 0;
    u32 lowEner = 0;
    u32 allEner = 0;

    remEner = sEnerForm.remEner;
    lowEner = sEnerForm.baseEner;
    allEner = sEnerForm.topEner;

    if(remEner >= allEner)
    {
    	ener = 1000;
    }
    else if((remEner <= lowEner) || ((allEner <= lowEner)))
    {
    	ener = 0;
    }
    else
    {
    	ener = (remEner - lowEner) * 1000 / (allEner - lowEner);

        if(ener > 1000)
        {
        	ener = 1000;
        }
    }

    return((u16)ener);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CalcGroupNowEnerHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 计算电池组当前电能处理
** @The notes : 实际可放电能量
**===========================================================================================*/
static void CalcGroupNowEnerHandle(void)
{
    s32 nowChangEner = 0;            /*当前充放电电能变化值(单位:1Wh,正数为放电量,负数为充电量)*/
    s32 realChangEner = 0;           /*实际充放电电能变化值(单位:1Wh,正数为放电量,负数为充电量)*/

    static s32 sHisChangEner = 0;    /*历史充放电电能量变化值(单位:1mAH,正数为放电能量,负数为充电能量)*/

    nowChangEner = GetChgDhgChangEnerAPI();

    realChangEner = nowChangEner - sHisChangEner;                   /*求间隔时间实际变化电能*/

    if((realChangEner > (s32)sEnerForm.standEner)
        || (realChangEner < ((s32)0 - (s32)sEnerForm.standEner)))   /*计算异常(变化量超过标称电能)*/
    {
        realChangEner = 0;                                          /*抛弃本次计算值*/
    }

    sEnerForm.nowEner -= realChangEner;                             /*计算当前电能(即剩余电能,放电为正数电能减小)*/

    if(sEnerForm.nowEner > sEnerForm.topEner)                       /*当前能量高端越界*/
    {
        sEnerForm.nowEner = sEnerForm.topEner;
    }

    if(sEnerForm.nowEner < sEnerForm.baseEner)                      /*当前能量低端越界*/
    {
        sEnerForm.nowEner = sEnerForm.baseEner;
    }

    sHisChangEner = nowChangEner;
}

/*=============================================================================================
** @Name      : static void CalcGroupRemainEnerHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 计算电池组剩余电能
** @The notes : 包含不可用的电能
**===========================================================================================*/
static void CalcGroupRemainEnerHandle(void)
{
    s32 nowChangEner = 0;            /*当前充放电电能变化值(单位:1Wh,正数为放电量,负数为充电量)*/
    s32 realChangEner = 0;           /*实际充放电电能变化值(单位:1Wh,正数为放电量,负数为充电量)*/

    static s32 sHisChangEner = 0;    /*历史充放电电能量变化值(单位:1mAH,正数为放电能量,负数为充电能量)*/

    nowChangEner = GetChgDhgChangEnerAPI();

    realChangEner = nowChangEner - sHisChangEner;                   /*求间隔时间实际变化电能*/

    if((realChangEner > (s32)sEnerForm.standEner)
        || (realChangEner < ((s32)0 - (s32)sEnerForm.standEner)))   /*计算异常(变化量超过标称电能)*/
    {
        realChangEner = 0;                                          /*抛弃本次计算值*/
    }

    sEnerForm.remEner -= realChangEner;                             /*计算当前电能(即剩余电能,放电为正数电能减小)*/

    if(sEnerForm.remEner > sEnerForm.topEner)                       /*当前能量高端越界*/
    {
        sEnerForm.remEner = sEnerForm.topEner;
    }

    if(sEnerForm.remEner < sEnerForm.baseEner)                      /*当前能量低端越界*/
    {
        sEnerForm.remEner = sEnerForm.baseEner;
    }

    sHisChangEner = nowChangEner;
}

/*=============================================================================================
** @Name      : static void StoreGroupNowEnerToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储当前可用电能到EEP
** @The notes : 判断存储周期
**===========================================================================================*/
static void StoreGroupNowEnerToEEP(void)
{
    u16 nowState = 0;
    u32 nowEner = 0;
    static u16 sHisState = 0;
    static u32 sHisEner = 0;
    
    nowState = GetGChgDhgStateAPI();
    
    nowEner = sEnerForm.nowEner - sEnerForm.baseEner;
    
    if((ABS(nowEner, sHisEner) >= ENER_CHANG_TO_WRITE_EEP)  /*达到写EEP变化量*/
        || ((ABS(nowEner, sHisEner) >=  10)                 /*与EEP值不一致*/
        && (eCURR_IDLE == nowState)
        && (eCURR_IDLE != sHisState)))                      /*由充放电转为待机*/
    {
    	EnerChangEepGNowEner(nowEner);                      /*写EEP*/
        sHisEner = nowEner;
    }
    
    sHisState = nowState;
}

/*=============================================================================================
** @Name      : static void StoreGroupRemainEnerToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储当前可用电能到EEP
** @The notes : 判断存储周期
**===========================================================================================*/
static void StoreGroupRemainEnerToEEP(void)
{
    u16 nowState = 0;
    u32 nowEner = 0;
    static u16 sHisState = 0;
    static u32 sHisEner = 0;

    nowState = GetGChgDhgStateAPI();

    nowEner = sEnerForm.remEner - sEnerForm.baseEner;

    if((ABS(nowEner, sHisEner) >= ENER_CHANG_TO_WRITE_EEP)  /*达到写EEP变化量*/
        || ((ABS(nowEner, sHisEner) >=  10)                 /*与EEP值不一致*/
        && (eCURR_IDLE == nowState)
        && (eCURR_IDLE != sHisState)))                      /*由充放电转为待机*/
    {
    	EnerChangEepGRemainEner(nowEner);                   /*写EEP*/
        sHisEner = nowEner;
    }

    sHisState = nowState;
}

/*=============================================================================================
** @Name      : static void StoreGroupTotalEnerToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储实际总电能到EEP
** @The notes : 直接写存储
**===========================================================================================*/
static void StoreGroupTotalEnerToEEP(void)
{
    u32 totalEner = 0;
    static u32 sHisTotal = 0;

    totalEner = sEnerForm.topEner - sEnerForm.baseEner;

    if(ABS(totalEner, sHisTotal) >= 10)                     /*总电能变化*/
    {
    	EnerChangEepTotalEner(totalEner);                   /*写EEP*/
        sHisTotal = totalEner;
    }
}

/*=============================================================================================
** @Name      : static void StoreGroupStandEnerToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储实际标称电能到EEP
** @The notes : 直接写存储
**===========================================================================================*/
static void StoreGroupStandEnerToEEP(void)
{
    static u32 sHisTotal = 0;

    if(ABS(sEnerForm.standEner, sHisTotal) >= 100)          /*标称电能变化*/
    {
    	EnerChangEepStandEner(sEnerForm.standEner);         /*写EEP*/
        sHisTotal = sEnerForm.standEner;
    }
}
