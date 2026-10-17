/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : UseTimeCalc.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 电池组电能量统计模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "CurrIntegral.h"
#include "ParaInterface.h"
#include "EnerInterface.h"
#include "CalcInterface.h"
#include "EnergyStatis.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gGStatistCapEner_72[];		/*当前能量统计信息数组*/
extern u32 gGStatistHisCap_73[];		/*累计电量统计信息数组*/
extern u32 gGStatistHisEner_74[];		/*累计电能统计信息数组*/

extern u16 gGBmuGenPara_102[];  		/*主控通用参数值*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void GroupEnerCapStatisInit(void)
** @Input     : void
** @Output    : void
** @Function  : 累计充放电电能电量统计初始化
** @The notes :
**===========================================================================================*/
void GroupEnerCapStatisInit(void)
{
	u32 info[3] = {0};

	/*单次充放电量*/
	gGStatistCapEner_72[eCapEner72_ChgCap] = 0;
	gGStatistCapEner_72[eCapEner72_DhgCap] = 0;

	/*累计充放电量*/
	if(TRUE == ParaReadStoreHisChgDhgCap(info, 3))
	{
		gGStatistHisCap_73[eHisCap73_HisChgCap] = info[1];
		gGStatistHisCap_73[eHisCap73_HisDhgCap] = info[2];
	}
	else
	{
		gGStatistHisCap_73[eHisCap73_HisChgCap] = gGBmuGenPara_102[eBmuGenPara102_ReaAllCap] / 20;
		gGStatistHisCap_73[eHisCap73_HisDhgCap] = 0;
	}

	/*累计充放电能*/
	if(TRUE == ParaReadStoreHisChgDhgEner(info, 3))
	{
        gGStatistHisEner_74[eHisEner74_HisChgEner] = info[1];
        gGStatistHisEner_74[eHisEner74_HisDhgEner] = info[2];
	}
	else
	{
        gGStatistHisEner_74[eHisEner74_HisChgEner] = gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] / 2;
        gGStatistHisEner_74[eHisEner74_HisDhgEner] = 0;
	}
}

/*=============================================================================================
** @Name      : void GroupCycleCapStatisTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组单次循环充放电电量统计任务
** @The notes : 400ms
**===========================================================================================*/
void GroupCycleCapStatisTask(void)
{
    u32 chgCap = 0;             /*充电电量(1AH)*/
    u32 dhgCap = 0;             /*放电电量(1AH)*/
    u16 nowState = 0;           /*当前充放电状态*/

    static u32 sHisState = 0;   /*历史充放电状态*/
    static u32 sFstChgCap = 0;  /*记录开始充电时的充电量*/
    static u32 sFstDhgCap = 0;  /*记录开始放电时的放电量*/

    chgCap = GetChgIntCapAPI();
    dhgCap = GetDhgIntCapAPI();
    nowState = GetGChgDhgStateAPI();

    /*计算单次充放电电量*/
    if(sHisState != nowState)                                       /*充放电状态切换*/
    {
        /*开始充电*/
        if(eCURR_CHG == nowState)                                   /*正在充电*/
        {
            sFstChgCap = chgCap;                                    /*记录起始充电电量*/

            sHisState = nowState;                                   /*更新充放电状态记录(充电中断再继续也当做一次)*/
        }

        /*开始放电*/
        if(eCURR_DHG == nowState)                                   /*正在充电*/
        {
            sFstDhgCap = dhgCap;                                    /*记录起始放电电量*/

            sHisState = nowState;                                   /*更新充放电状态记录(放电中断再继续也当做一次)*/
        }
    }

    /*小储能U16只能用10mAH为单位*/
    #if(1 == SMALL_ESS_EN)
    /*显示单次充放电电量*/
    gGStatistCapEner_72[eCapEner72_ChgCap] = (chgCap - sFstChgCap + 5) / 10;/*单次充电电量(1mAH转10mAH)*/
    gGStatistCapEner_72[eCapEner72_DhgCap] = (dhgCap - sFstDhgCap + 5) / 10;/*单次放电电量(1mAH转10mAH)*/
    /*大储能U16直接用1AH为单位*/
    #else
    /*显示单次充放电电量*/
    gGStatistCapEner_72[eCapEner72_ChgCap] = chgCap - sFstChgCap;   /*单次充电电量*/
    gGStatistCapEner_72[eCapEner72_DhgCap] = dhgCap - sFstDhgCap;   /*单次放电电量*/
    #endif
}

/*=============================================================================================
** @Name      : void GroupHisCapStatisTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组累计历史充放电电量统计任务
** @The notes : 400ms
**===========================================================================================*/
void GroupHisCapStatisTask(void)
{
    u32 chgCap = 0;             /*充电电量(1AH)*/
    u32 dhgCap = 0;             /*放电电量(1AH)*/
    u16 nowState = 0;           /*当前充放电状态*/
    u32 chgChangCap = 0;        /*充电电量变化量(1AH)*/
    u32 dhgChangCap = 0;        /*放电电量变化量(1AH)*/

    static u32 sHisState = 0;   /*历史充放电状态*/
    static u32 sHisChgCap = 0;  /*记录上次充电电量(1AH)*/
    static u32 sHisDhgCap = 0;  /*记录上次放电电量(1AH)*/
    static u32 sEepChgCap = 0;  /*上次写EEP充电电量(1AH)*/
    static u32 sEepDhgCap = 0;  /*上次写EEP放电电量(1AH)*/

    chgCap = GetChgIntCapAPI();
    dhgCap = GetDhgIntCapAPI();
    nowState = GetGChgDhgStateAPI();

    if(chgCap > sHisChgCap)
    {
        chgChangCap = chgCap - sHisChgCap;
    }
    else
    {
        chgChangCap = 0;        /*历史值错误则抛弃本次计算值*/
    }

    if(dhgCap > sHisDhgCap)
    {
        dhgChangCap = dhgCap - sHisDhgCap;
    }
    else
    {
        dhgChangCap = 0;        /*历史值错误则抛弃本次计算值*/
    }

    gGStatistHisCap_73[eHisCap73_HisChgCap] += chgChangCap;
    gGStatistHisCap_73[eHisCap73_HisDhgCap] += dhgChangCap;

    /*保证放电量不能多于充电量一个总容量
    if(gGStatistHisCap_73[eHisCap73_HisDhgCap] > (gGStatistHisCap_73[eHisCap73_HisChgCap] + (gGBmuGenPara_102[eBmuGenPara102_ReaAllCap] / 10)))
    {
    	gGStatistHisCap_73[eHisCap73_HisDhgCap] = gGStatistHisCap_73[eHisCap73_HisChgCap] + (gGBmuGenPara_102[eBmuGenPara102_ReaAllCap] / 10);
    }*/

    if((ABS(gGStatistHisCap_73[eHisCap73_HisChgCap], sEepChgCap) >= HISCAP_CHANG_TO_WRITE_EEP) /*达到写EEP变化值*/
        || ((gGStatistHisCap_73[eHisCap73_HisChgCap] > sEepChgCap)                             /*与EEP值不一致*/
        && (eCURR_IDLE == nowState) && (eCURR_IDLE != sHisState)))                             /*由充放电转为待机*/
    {
        /*电池组历史充电电量写EEP*/
    	ParaWriteStoreHisChgDhgCap(1, &gGStatistHisCap_73[eHisCap73_HisChgCap], 1);
        sEepChgCap = gGStatistHisCap_73[eHisCap73_HisChgCap];
    }

    if((ABS(gGStatistHisCap_73[eHisCap73_HisDhgCap], sEepDhgCap) >= HISCAP_CHANG_TO_WRITE_EEP) /*达到写EEP变化值*/
        || ((gGStatistHisCap_73[eHisCap73_HisDhgCap] > sEepDhgCap)                             /*与EEP值不一致*/
        && (eCURR_IDLE == nowState) && (eCURR_IDLE != sHisState)))                             /*由充放电转为待机*/
    {
        /*电池组历史放电电量写EEP*/
    	ParaWriteStoreHisChgDhgCap(2, &gGStatistHisCap_73[eHisCap73_HisDhgCap], 1);
        sEepDhgCap = gGStatistHisCap_73[eHisCap73_HisDhgCap];
    }

    sHisChgCap = chgCap;
    sHisDhgCap = dhgCap;
    sHisState = nowState;
}

/*=============================================================================================
** @Name      : void GroupHisEnerStatisTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组累计历史充放电电能统计任务
** @The notes : 400ms
**===========================================================================================*/
void GroupHisEnerStatisTask(void)
{
    u32 chgEner = 0;            /*充电电能(0.1KWH)*/
    u32 dhgEner = 0;            /*放电电能(0.1KWH)*/
    u16 nowState = 0;           /*当前充放电状态*/
    u32 chgChangEner = 0;       /*充电电能变化量(0.1KWH)*/
    u32 dhgChangEner = 0;       /*放电电能变化量(0.1KWH)*/
    static u16 sHisState = 0;   /*历史充放电状态*/
    static u32 sHisChgEner = 0; /*记录上次充电电能(0.1KWH)*/
    static u32 sHisDhgEner = 0; /*记录上次放电电能(0.1KWH)*/
    static u32 sEepChgEner = 0; /*上次写EEP充电电能(0.1KWH)*/
    static u32 sEepDhgEner = 0; /*上次写EEP放电电能(0.1KWH)*/

    chgEner = GetChgIntEnerAPI();
    dhgEner = GetDhgIntEnerAPI();
    nowState = GetGChgDhgStateAPI();

    if(chgEner > sHisChgEner)
    {
        chgChangEner = chgEner - sHisChgEner;
    }
    else
    {
        chgChangEner = 0;       /*历史值错误则抛弃本次计算值*/
    }

    if(dhgEner > sHisDhgEner)
    {
        dhgChangEner = dhgEner - sHisDhgEner;
    }
    else
    {
        dhgChangEner = 0;      /*历史值错误则抛弃本次计算值*/
    }

    gGStatistHisEner_74[eHisEner74_HisChgEner] += chgChangEner;
    gGStatistHisEner_74[eHisEner74_HisDhgEner] += dhgChangEner;

    /*保证放电能不能多于充电能多于一个总电能
    if(gGStatistHisEner_74[eHisEner74_HisDhgEner] > (gGStatistHisEner_74[eHisEner74_HisChgEner] + gGBmuGenPara_102[eBmuGenPara102_ReaAllEner]))
    {
    	gGStatistHisEner_74[eHisEner74_HisDhgEner] = gGStatistHisEner_74[eHisEner74_HisChgEner] + gGBmuGenPara_102[eBmuGenPara102_ReaAllEner];
    }*/

    if((ABS(gGStatistHisEner_74[eHisEner74_HisChgEner], sEepChgEner) >= HISENER_CHANG_TO_WRITE_EEP)/*达到写EEP变化值*/
        || ((gGStatistHisEner_74[eHisEner74_HisChgEner] > sEepChgEner)                            /*与EEP值不一致*/
        && (eCURR_IDLE == nowState) && (eCURR_IDLE != sHisState)))                                /*由充放电转为待机*/
    {
        /*电池组历史充电电能写EEP*/
    	ParaWriteStoreHisChgDhgEner(1, &gGStatistHisEner_74[eHisEner74_HisChgEner], 1);
        sEepChgEner = gGStatistHisEner_74[eHisEner74_HisChgEner];
    }

    if((ABS(gGStatistHisEner_74[eHisEner74_HisDhgEner], sEepDhgEner) >= HISENER_CHANG_TO_WRITE_EEP)/*达到写EEP变化值*/
        || ((gGStatistHisEner_74[eHisEner74_HisDhgEner] > sEepDhgEner)                             /*与EEP值不一致*/
        && (eCURR_IDLE == nowState) && (eCURR_IDLE != sHisState)))                                 /*由充放电转为待机*/
    {
        /*电池组历史放电电能写EEP*/
    	ParaWriteStoreHisChgDhgEner(2, &gGStatistHisEner_74[eHisEner74_HisDhgEner], 1);
        sEepDhgEner = gGStatistHisEner_74[eHisEner74_HisDhgEner];
    }

    sHisChgEner = chgEner;
    sHisDhgEner = dhgEner;
    sHisState = nowState;
}

/*=============================================================================================
** @Name      : void ClrGroupEnerCapStatisInfo(void)
** @Input     : void
** @Output    : void
** @Function  : 清除充放电电能电量统计值
** @The notes : 单次/累计/电能/电量
**===========================================================================================*/
void ClrGroupEnerCapStatisInfo(void)
{
    gGStatistHisCap_73[eHisCap73_HisChgCap] = 0;
    gGStatistHisCap_73[eHisCap73_HisDhgCap] = 0;
    gGStatistHisEner_74[eHisEner74_HisChgEner] = 0;
    gGStatistHisEner_74[eHisEner74_HisDhgEner] = 0;
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
