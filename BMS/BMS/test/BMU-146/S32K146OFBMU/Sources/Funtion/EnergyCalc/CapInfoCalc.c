/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : CapInfoCalc.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : SOC容量计算模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "ParaInterface.h"
#include "EnerInterface.h"
#include "EnerInfoCalc.h"
#include "CurrIntegral.h"
#include "CapInfoCalc.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static t_CAPFORM sCapForm = 
{
    CAP_ZERO_POINT + 135000,        /*当前容量(单位:1mAH)*/
    CAP_ZERO_POINT + 135000,        /*剩余容量(单位:1mAH)*/
    CAP_ZERO_POINT,                 /*底端容量(单位:1mAH)*/
    CAP_ZERO_POINT + 270000,        /*顶端容量(单位:1mAH)*/
    280000                          /*标称容量(单位:1mAH)*/
};

extern s16 gGSysMainInfo_60[];		/*系统整体信息*/
extern u16 gGSysCapInfo_61[];		/*系统电量信息*/
extern u16 gGBmuGenPara_102[];		/*主控通用参数值*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CalcGroupNowCapHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 计算电池组当前容量处理
** @The notes : 实际可放电容量
**===========================================================================================*/
static void CalcGroupNowCapHandle(void);

/*=============================================================================================
** @Name      : static void CalcGroupRemainCapHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 计算电池组剩余容量
** @The notes : 包括不可用的容量
**===========================================================================================*/
static void CalcGroupRemainCapHandle(void);

/*=============================================================================================
** @Name      : static void StoreGroupNowCapToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储电池组当前可用容量到EEP
** @The notes : 判断存储周期
**===========================================================================================*/
static void StoreGroupNowCapToEEP(void);

/*=============================================================================================
** @Name      : static void StoreGroupRemainCapToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储电池组剩余容量到EEP
** @The notes : 判断存储周期
**===========================================================================================*/
static void StoreGroupRemainCapToEEP(void);

/*=============================================================================================
** @Name      : static void StoreGroupTotalCapToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储电池组总容量到EEP
** @The notes : 直接写EEPROM
**===========================================================================================*/
static void StoreGroupTotalCapToEEP(void);

/*=============================================================================================
** @Name      : static void StoreGroupStandCapToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储电池组标称容量到EEP
** @The notes : 直接写EEPROM
**===========================================================================================*/
static void StoreGroupStandCapToEEP(void);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void GroupCapSocInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组当前容量SOC计算初始化
** @The notes :
**===========================================================================================*/
void GroupCapSocInit(void)
{
	u32 data[5] = {0};

	//已初始化EEPROM有效值
	if(TRUE == ParaReadStoreCapFormInfo(data, 5))
	{
		sCapForm.nowCap = CAP_ZERO_POINT + data[1];
		sCapForm.remCap = CAP_ZERO_POINT + data[2];
		sCapForm.topCap = CAP_ZERO_POINT + data[3];
		sCapForm.baseCap = CAP_ZERO_POINT;
		sCapForm.standCap = data[4];
	}
	//首次上电未初始化
	else
	{
		sCapForm.nowCap = CAP_ZERO_POINT + ((u32)gGBmuGenPara_102[eBmuGenPara102_NowSoc] * gGBmuGenPara_102[eBmuGenPara102_ReaAllCap] / 10);
		sCapForm.remCap = sCapForm.nowCap;
		sCapForm.topCap = CAP_ZERO_POINT + gGBmuGenPara_102[eBmuGenPara102_ReaAllCap] * 100;
		sCapForm.baseCap = CAP_ZERO_POINT;
		//sCapForm.standCap = gGBmuGenPara_102[eBmuGenPara102_RateCap] * 100;
		//额定容量无小数才为标称容量
		if(0 == (gGBmuGenPara_102[eBmuGenPara102_RateCap] % 10))
		{
			sCapForm.standCap = gGBmuGenPara_102[eBmuGenPara102_RateCap] * 100;
		}
		//额定容量有小数取实际总容量
		else
		{
			sCapForm.standCap = gGBmuGenPara_102[eBmuGenPara102_ReaAllCap] * 100;
		}
	}
}

/*=============================================================================================
** @Name      : void GroupCapSocCalcTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组当前容量SOC计算任务
** @The notes : 100ms调用,测算总容量和SOC
**===========================================================================================*/
void GroupCapSocCalcTask(void)
{
    static u8 sState = 0;
    
    switch(sState)
    {
        case 0:
        	CalcGroupNowCapHandle();        /*计算当前容量*/
        	StoreGroupNowCapToEEP();        /*写当前实际容量到EEP*/
            sState = 1;
            break;
            
        case 1:
            CalcGroupRemainCapHandle();     /*计算剩余容量*/
        	StoreGroupRemainCapToEEP();     /*写当前剩余容量到EEP*/
            sState = 0;
            break;
            
        default:
            sState = 0;
            break;
    }
}

/*=============================================================================================
** @Name      : void CorrGNowCapBySocAPI(u16 soc)
** @Input     : soc:目标SOC(0.1%)
** @Output    : void
** @Function  : 根据电池组SOC修正当前容量
** @The notes :
**===========================================================================================*/
void CorrGNowCapBySocAPI(u16 soc)
{
    u32 nowCap = 0;
    u32 lowCap = 0;
    u32 allCap = 0;
    
    lowCap = sCapForm.baseCap;
    allCap = sCapForm.topCap;
    
    if(allCap <= lowCap)
    {
        return;
    }
    
    nowCap = (allCap - lowCap) * soc / 1000 + CAP_ZERO_POINT;
    
    if(nowCap > allCap)
    {
        nowCap = allCap;
    }
    else if(nowCap < lowCap)
    {
        nowCap = lowCap;
    }
    
    sCapForm.nowCap = nowCap;

    /*写当前实际容量到EEP*/
    StoreGroupNowCapToEEP();
}

/*=============================================================================================
** @Name      : void CorrGRemainCapBySocAPI(u32 cap)
** @Input     : soc:目标SOC(0.1%)
** @Output    : void
** @Function  : 根据电池组SOC修正剩余电量
** @The notes : 相对总容量剩余值
**===========================================================================================*/
void CorrGRemainCapBySocAPI(u16 soc)
{
    u32 remCap = 0;
    u32 lowCap = 0;
    u32 allCap = 0;

    lowCap = sCapForm.baseCap;
    allCap = sCapForm.topCap;

    if(allCap <= lowCap)
    {
        return;
    }

    remCap = (allCap - lowCap) * soc / 1000 + CAP_ZERO_POINT;

    if(remCap > allCap)
    {
    	remCap = allCap;
    }
    else if(remCap < lowCap)
    {
    	remCap = lowCap;
    }

    sCapForm.remCap = remCap;

    /*写剩余容量到EEP*/
    StoreGroupRemainCapToEEP();
}

/*=============================================================================================
** @Name      : void CorrGroupTotalCapAPI(u32 cap)
** @Input     : cap:电池组总容量(单位:1mAH)
** @Output    : void
** @Function  : 修正电池组总容量
** @The notes :
**===========================================================================================*/
void CorrGroupTotalCapAPI(u32 cap)
{
    u32 soc = 0;
    u32 nowCap = 0;
    u32 remCap = 0;
    u32 lowCap = 0;
    u32 allCap = 0;
    u32 befCap = 0;

    nowCap = sCapForm.nowCap;
    remCap = sCapForm.remCap;
    lowCap = sCapForm.baseCap;
    allCap = sCapForm.topCap;
    befCap = sCapForm.topCap - CAP_ZERO_POINT;
    
    if(allCap <= lowCap)                /*变量异常*/
    {
        cap = 0;
        nowCap = CAP_ZERO_POINT;
        remCap = CAP_ZERO_POINT;
        allCap = CAP_ZERO_POINT;
    }
    else
    {
    	/*求顶端容量*/
        allCap = cap + CAP_ZERO_POINT;  /*顶端容量*/

    	/*求当前容量*/
        if(nowCap >= allCap)            /*变量异常*/
        {
            soc = 10000;
        }
        else if(nowCap <= lowCap)       /*变量异常*/
        {
            soc = 0;
        }
        else
        {
            soc = (nowCap - lowCap) * 10000 / befCap;
        }

        nowCap = cap * soc / 10000 + CAP_ZERO_POINT;
        if(nowCap > allCap)             /*计算当前容量超过总容量*/
        {
            nowCap = allCap;
        }

    	/*求剩余容量*/
        if(remCap >= allCap)            /*变量异常*/
        {
            soc = 10000;
        }
        else if(remCap <= lowCap)       /*变量异常*/
        {
            soc = 0;
        }
        else
        {
            soc = (remCap - lowCap) * 10000 / befCap;
        }
        
        remCap = cap * soc / 10000 + CAP_ZERO_POINT;
        if(remCap > allCap)             /*计算当前容量超过总容量*/
        {
        	remCap = allCap;
        }
    }

    sCapForm.nowCap = nowCap;
    sCapForm.remCap = remCap;
    sCapForm.topCap = allCap;

    StoreGroupNowCapToEEP();                    /*写当前实际容量到EEP*/
    StoreGroupRemainCapToEEP();                 /*写当前剩余容量到EEP*/
    StoreGroupTotalCapToEEP();                  /*写电池组总容量到EEP*/

    EnerAftTotalCapCorrDealHook(befCap, cap);   /*总容量修正后事件*/
}

/*=============================================================================================
** @Name      : void CorrGroupStandCapAPI(u32 cap)
** @Input     : cap:电池组标称容量(单位:1mAH)
** @Output    : void
** @Function  : 修正电池组标称容量
** @The notes :
**===========================================================================================*/
void CorrGroupStandCapAPI(u32 cap)
{
    sCapForm.standCap = cap;

    StoreGroupStandCapToEEP();                  /*写电池组标称容量到EEP*/
}

/*=============================================================================================
** @Name      : u32 GetGroupNowCapAPI(void)
** @Input     : void
** @Output    : 电池组当前容量(单位:1mAH)
** @Function  : 获取电池组当前电量
** @The notes : 剩余能够使用的电量
**===========================================================================================*/
u32 GetGroupNowCapAPI(void)
{
    return(sCapForm.nowCap - CAP_ZERO_POINT);
}

/*=============================================================================================
** @Name      : u32 GetGroupRemainCapAPI(void)
** @Input     : void
** @Output    : 电池组剩余电量(单位:1mAH)
** @Function  : 获取电池组剩余电量
** @The notes : 包括放不出的电量
**===========================================================================================*/
u32 GetGroupRemainCapAPI(void)
{
    return(sCapForm.remCap - CAP_ZERO_POINT);
}

/*=============================================================================================
** @Name      : u32 GetGroupTotalCapAPI(void)
** @Input     : void
** @Output    : 电池组总容量(单位:1mAH)
** @Function  : 获取电池组总容量
** @The notes :
**===========================================================================================*/
u32 GetGroupTotalCapAPI(void)
{
    return(sCapForm.topCap - CAP_ZERO_POINT);
}

/*=============================================================================================
** @Name      : u32 GetGroupStandCapAPI(void)
** @Input     : void
** @Output    : 电池组标称容量(单位:1mAH)
** @Function  : 获取电池组标称容量
** @The notes :
**===========================================================================================*/
u32 GetGroupStandCapAPI(void)
{
    return(sCapForm.standCap);
}

/*=============================================================================================
** @Name      : u16 GetGRealSocTenThousAPI(void)
** @Input     : void
** @Output    : 电池组真实SOC值(单位:万分之一)
** @Function  : 获取电池组真实SOC值
** @The notes :
**===========================================================================================*/
u16 GetGRealSocTenThousAPI(void)
{
    u32 soc = 0;
    u32 nowCap = 0;
    u32 lowCap = 0;
    u32 allCap = 0;
    
    nowCap = sCapForm.nowCap;
    lowCap = sCapForm.baseCap;
    allCap = sCapForm.topCap;
    
    if(nowCap >= allCap)
    {
        soc = 10000;
    }
    else if((nowCap <= lowCap) || ((allCap <= lowCap)))
    {
        soc = 0;
    }
    else
    {
        soc = (nowCap - lowCap) * 10000 / (allCap - lowCap);
        
        if(soc > 10000)
        {
            soc = 10000;
        }
    }
    
    return((u16)soc);
}

/*=============================================================================================
** @Name      : u16 GetGRealSocMilliAPI(void)
** @Input     : void
** @Output    : 电池组真实SOC值(单位:千分之一)
** @Function  : 获取电池组真实SOC值
** @The notes :
**===========================================================================================*/
u16 GetGRealSocMilliAPI(void)
{
    u32 soc = 0;
    u32 nowCap = 0;
    u32 lowCap = 0;
    u32 allCap = 0;
    
    nowCap = sCapForm.nowCap;
    lowCap = sCapForm.baseCap;
    allCap = sCapForm.topCap;
    
    if(nowCap >= allCap)
    {
        soc = 1000;
    }
    else if((nowCap <= lowCap) || ((allCap <= lowCap)))
    {
        soc = 0;
    }
    else
    {
        soc = (nowCap - lowCap) * 1000 / (allCap - lowCap);
        
        if(soc > 1000)
        {
            soc = 1000;
        }
    }
    
    return((u16)soc);
}

/*=============================================================================================
** @Name      : u16 GetGRemainSocMilliAPI(void)
** @Input     : void
** @Output    : 电池组剩余SOC值(单位:千分之一)
** @Function  : 获取电池组剩余SOC值
** @The notes :
**===========================================================================================*/
u16 GetGRemainSocMilliAPI(void)
{
    u32 soc = 0;
    u32 remCap = 0;
    u32 lowCap = 0;
    u32 allCap = 0;

    remCap = sCapForm.remCap;
    lowCap = sCapForm.baseCap;
    allCap = sCapForm.topCap;

    if(remCap >= allCap)
    {
        soc = 1000;
    }
    else if((remCap <= lowCap) || ((allCap <= lowCap)))
    {
        soc = 0;
    }
    else
    {
        soc = (remCap - lowCap) * 1000 / (allCap - lowCap);

        if(soc > 1000)
        {
            soc = 1000;
        }
    }

    return((u16)soc);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CalcGroupNowCapHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 计算电池组当前容量处理
** @The notes : 实际可放电容量
**===========================================================================================*/
static void CalcGroupNowCapHandle(void)
{
    s32 nowChangCap = 0;            /*当前充放电电量变化值(单位:1mAH,正数为放电量,负数为充电量)*/
    s32 realChangCap = 0;           /*实际充放电电量变化值(单位:1mAH,正数为放电量,负数为充电量)*/
    
    static s32 sHisChangCap = 0;    /*历史充放电电量变化值(单位:1mAH,正数为放电量,负数为充电量)*/
    
    nowChangCap = GetChgDhgChangCapAPI();
    
    realChangCap = nowChangCap - sHisChangCap;                  /*求间隔时间实际变化电量*/
    
    if((realChangCap > (s32)sCapForm.standCap)
        || (realChangCap < ((s32)0 - (s32)sCapForm.standCap)))  /*计算异常(变化量超过标称容量)*/
    {
        realChangCap = 0;                                       /*抛弃本次计算值*/
    }
    
    sCapForm.nowCap -= realChangCap;                            /*计算当前容量(即剩余容量,放电为正数容量减小)*/
    
    if(sCapForm.nowCap > sCapForm.topCap)                       /*当前容量高端越界*/
    {
        sCapForm.nowCap = sCapForm.topCap;
    }
    
    if(sCapForm.nowCap < sCapForm.baseCap)                      /*当前容量低端越界*/
    {
        sCapForm.nowCap = sCapForm.baseCap;
    }
    
    sHisChangCap = nowChangCap;
}

/*=============================================================================================
** @Name      : static void CalcGroupRemainCapHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 计算电池组剩余容量
** @The notes : 包括不可用的容量
**===========================================================================================*/
static void CalcGroupRemainCapHandle(void)
{
    s32 nowChangCap = 0;            /*当前充放电电量变化值(单位:1mAH,正数为放电量,负数为充电量)*/
    s32 realChangCap = 0;           /*实际充放电电量变化值(单位:1mAH,正数为放电量,负数为充电量)*/

    static s32 sHisChangCap = 0;    /*历史充放电电量变化值(单位:1mAH,正数为放电量,负数为充电量)*/

    nowChangCap = GetChgDhgChangCapAPI();

    realChangCap = nowChangCap - sHisChangCap;                  /*求间隔时间实际变化电量*/

    if((realChangCap > (s32)sCapForm.standCap)
        || (realChangCap < ((s32)0 - (s32)sCapForm.standCap)))  /*计算异常(变化量超过标称容量)*/
    {
        realChangCap = 0;                                       /*抛弃本次计算值*/
    }

    sCapForm.remCap -= realChangCap;                            /*计算当前容量(即剩余容量,放电为正数容量减小)*/

    if(sCapForm.remCap > sCapForm.topCap)                       /*当前容量高端越界*/
    {
        sCapForm.remCap = sCapForm.topCap;
    }

    if(sCapForm.remCap < sCapForm.baseCap)                      /*当前容量低端越界*/
    {
        sCapForm.remCap = sCapForm.baseCap;
    }

    sHisChangCap = nowChangCap;
}

/*=============================================================================================
** @Name      : static void StoreGroupNowCapToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储电池组当前可用容量到EEP
** @The notes : 判断存储周期
**===========================================================================================*/
static void StoreGroupNowCapToEEP(void)
{
    u32 nowCap = 0;
    u16 nowState = 0;
    u16 changeSoc = 0;
    static u32 sHisCap = 0;                                 /*历史电量(单位:1mAH)*/
    static u32 sHisState = 0;                               /*历史充放电状态*/

    nowState = GetGChgDhgStateAPI();
    nowCap = sCapForm.nowCap - sCapForm.baseCap;
    changeSoc = (u16)(ABS(nowCap, sHisCap) * 1000 / GetGroupTotalCapAPI());
    
    if((changeSoc >= SOC_CHANG_TO_WRITE_EEP)                /*达到写EEP变化量*/
        || ((ABS(nowCap, sHisCap) >= 10)                    /*与EEP值不一致*/
        && (eCURR_IDLE == nowState)
        && (eCURR_IDLE != sHisState)))                      /*由充放电转为待机*/
    {
    	EnerChangEepGNowCapHook(nowCap);                    /*写EEP*/
        sHisCap = nowCap;
    }

    sHisState = nowState;
}

/*=============================================================================================
** @Name      : static void StoreGroupRemainCapToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储电池组剩余容量到EEP
** @The notes : 判断存储周期
**===========================================================================================*/
static void StoreGroupRemainCapToEEP(void)
{
    u32 nowCap = 0;
    u16 nowState = 0;
    u16 changeSoc = 0;
    static u32 sHisCap = 0;                                 /*历史电量(单位:1mAH)*/
    static u32 sHisState = 0;                               /*历史充放电状态*/

    nowState = GetGChgDhgStateAPI();
    nowCap = sCapForm.remCap - sCapForm.baseCap;
    changeSoc = (u16)(ABS(nowCap, sHisCap) * 1000 / GetGroupTotalCapAPI());

    if((changeSoc >= SOC_CHANG_TO_WRITE_EEP)                /*达到写EEP变化量*/
        || ((ABS(nowCap, sHisCap) >= 10)                    /*与EEP值不一致*/
        && (eCURR_IDLE == nowState)
        && (eCURR_IDLE != sHisState)))                      /*由充放电转为待机*/
    {
    	EnerChangEepGRemainCapHook(nowCap);                 /*写EEP*/
        sHisCap = nowCap;
    }

    sHisState = nowState;
}

/*=============================================================================================
** @Name      : static void StoreGroupTotalCapToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储电池组总容量到EEP
** @The notes : 直接写EEPROM
**===========================================================================================*/
static void StoreGroupTotalCapToEEP(void)
{
    u32 allCap = 0;
    static u32 sHisCap = 0;                         /*历史总容量(单位:1mAH)*/
    
    allCap = sCapForm.topCap - sCapForm.baseCap;
    
    if(ABS(allCap, sHisCap) >= 10)                  /*总容量变化*/
    {
    	EnerChangEepGTotalCapHook(allCap);          /*写EEP*/
        sHisCap = allCap;
    }
}

/*=============================================================================================
** @Name      : static void StoreGroupStandCapToEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 存储电池组标称容量到EEP
** @The notes : 直接写EEPROM
**===========================================================================================*/
static void StoreGroupStandCapToEEP(void)
{
    static u32 sHisCap = 0;                         /*历史标称容量(单位:1mAH)*/

    if(ABS(sCapForm.standCap, sHisCap) >= 100)      /*标称容量变化*/
    {
    	EnerChangEepGStandCapHook(sCapForm.standCap);/*写EEP*/
        sHisCap = sCapForm.standCap;
    }
}
