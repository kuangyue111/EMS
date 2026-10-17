/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : TimesStatis.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 电池时间与次数统计模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "ParaInterface.h"
#include "CurrIntegral.h"
#include "CapInfoCalc.h"
#include "EnerInfoCalc.h"
#include "EnerInterface.h"
#include "CalcInterface.h"
#include "StaInterface.h"
#include "TimesStatis.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u32 sHisChgDhgState  = 0;        /*上次充放电状态/上次充放电量*/
static u16 sHisMonErrTime  = 0;         /*上次月故障统计时间(年月)*/

extern u16 gGStatistTime_70[]; 		    /*时间统计信息数组*/
extern u16 gGStatistErr_71[]; 		    /*故障统计信息数组*/

extern u16 gGBmuGenPara_102[];  		/*主控通用参数值*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void GroupTimesStatisInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组时间次数统计初始化
** @The notes :
**===========================================================================================*/
void GroupTimesStatisInit(void)
{
    u32 info32[2] = {0};
    u16 info16[5] = {0};

	/*计算充放电次数状态*/
    if(TRUE == ParaReadStoreChgTimesState(info32, 2))
    {
		/*通过时间计算充放电次数*/
		#if(1 == CALC_TIMES_BY_TIME)
		if((1 <= info32[1]) && (info32[1] <= 2))
		{
			sHisChgDhgState = info[1];
		}
		else
		{
			sHisChgDhgState = 1;
		}
		/*通过历史电量计算充放电次数*/
		#else
		sHisChgDhgState = info32[1];
		#endif
    }
    else
    {
		/*通过时间计算充放电次数*/
		#if(1 == CALC_TIMES_BY_TIME)
		sHisChgDhgState = 1;
		/*通过历史电量计算充放电次数*/
		#else
		sHisChgDhgState = 0;
		#endif
    }
    
    /*统计充放电次数当前次数*/
    if(TRUE == ParaReadStoreChgDhgTimes(info16, 2))
    {
    	gGStatistTime_70[eTime70_ChgCycle] = info16[1];
    }
    /*未计算次数或清零*/
    else
    {
    	gGStatistTime_70[eTime70_ChgCycle] = 1;
    }

    /*月故障统计时间*/
    if(TRUE == ParaReadStoreMonErrTime(info16, 2))
    {
    	sHisMonErrTime = info16[1];
    }
    /*未计算次数或清零*/
    else
    {
    	sHisMonErrTime = 0;
    }

    /*读取故障告警数目统计*/
    if(TRUE == ParaReadStoreHisErrNum(info16, 5))
    {
		gGStatistErr_71[eErr71_MonWarNum] = info16[1];
		gGStatistErr_71[eErr71_MonErrNum] = info16[2];
		gGStatistErr_71[eErr71_HisWarNum] = info16[3];
		gGStatistErr_71[eErr71_HisErrNum] = info16[4];
    }
    else
    {
		gGStatistErr_71[eErr71_MonWarNum] = 0;
		gGStatistErr_71[eErr71_MonErrNum] = 0;
		gGStatistErr_71[eErr71_HisWarNum] = 0;
		gGStatistErr_71[eErr71_HisErrNum] = 0;
    }
}

/*=============================================================================================
** @Name      : void GroupChgTimesStatisTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组充放电次数统计任务
** @The notes : 2s
**===========================================================================================*/
void GroupChgTimesStatisTask(void)
{
    /*通过时间计算充放电次数*/
    #if(1 == CALC_TIMES_BY_TIME)
    u8 chgState = 0;
    static u8 sBefState = 0;
    static u16 sChgTimers = 0;
    static u16 sDhgTimers = 0;
    
    chgState = (u8)GetGChgDhgStateAPI();            /*当前充放电状态*/
    
    if(chgState == 1)                               /*正在充电*/
    {
        if(chgState == sBefState)                   /*当前状态和上一状态相同(持续充电)*/
        {
            sChgTimers++;
            if(sChgTimers >= CHGDHG_TIMES_ADD_TIME) /*长时间充电10min(默认)*/
            {
                sChgTimers = CHGDHG_TIMES_ADD_TIME;
                
                if(2 == sHisChgDhgState)            /*历史状态是放电*/
                {
                	gGStatistTime_70[eTime70_ChgDchCycle]++;/*充放电增加一次*/
                    sHisChgDhgState = chgState;     /*更新历史值*/
                    ParaWriteStoreChgTimesState(1, &chgState, 1);
                    ParaWriteStoreChgDhgTimes(1, &gGStatistTime_70[eTime70_ChgCycle], 1);
                             
                }
                else if(sHisChgDhgState != 1)       /*历史状态不是充电也不是放电(参数值异常)*/
                {
                    sHisChgDhgState = chgState;     /*更新历史值*/
                    ParaWriteStoreChgTimesState(1, &chgState, 1);
                }       
            }
        }
        else                                        /*状态刚改变为充电*/
        {
            sBefState = chgState;                   /*开始计算充电时间*/
            sChgTimers = 0;
        }
    }
    else if(chgState == 2)                          /*正在放电*/
    {
        if(chgState == sBefState)                   /*当前状态和上一状态相同(持续)*/
        {
            sDhgTimers++;
            if(sDhgTimers >= CHGDHG_TIMES_ADD_TIME) /*长时间放电10min(默认)*/
            {
                sDhgTimers = CHGDHG_TIMES_ADD_TIME;
                
                if(1 == sHisChgDhgState)            /*历史状态是充电*/
                {
                	gGStatistTime_70[eTime70_ChgDchCycle]++;/*充放电增加一次*/
                    sHisChgDhgState = chgState;     /*更新历史值*/
                    ParaWriteStoreChgTimesState(1, &chgState, 1);
                    ParaWriteStoreChgDhgTimes(1, &gGStatistTime_70[eTime70_ChgCycle], 1);
                             
                }
                else if(sHisChgDhgState != 2)       /*历史状态不是充电也不是放电(参数值异常)*/
                {
                    sHisChgDhgState = chgState;     /*更新历史值*/
                    ParaWriteStoreChgTimesState(1, &chgState, 1);
                }
            }
        }
        else                                        /*状态刚改变为放电*/
        {
            sBefState = chgState;                   /*开始计算放电时间*/
            sDhgTimers = 0;
        }
    }
    /*通过历史电量计算充放电次数*/
    #else
    u32 avgCap = 0;
    u32 chgCap = 0;
    u32 dhgCap = 0;
    u32 totalCap = 0;
    
    chgCap = GetGHisChgCapAPI();
    dhgCap = GetGHisDhgCapAPI();
    
    /*计算当前历史充放电量平均值(防止越界)*/
    avgCap = ABS(chgCap, dhgCap) / 2;
    
    if(chgCap >= dhgCap)
    {
        avgCap = avgCap + dhgCap;
    }
    else
    {
        avgCap = avgCap + chgCap;
    }
    
    /*当前历史电量比上次记录值大(历史电量未清零)*/
    if(avgCap >= sHisChgDhgState)
    {
        /*小储能历史电量用mAH为单位*/
        #if(1 == SMALL_ESS_EN)
        totalCap = GetGroupTotalCapAPI();
        #else
        totalCap = GetGroupTotalCapAPI() / 1000;
        #endif
        
        /*充放电累加到容量值*/
        if(avgCap >= (sHisChgDhgState + totalCap))
        {
        	gGStatistTime_70[eTime70_ChgCycle]++;    /*充放电增加一次*/
            sHisChgDhgState = avgCap;                /*更新历史值*/
            ParaWriteStoreChgTimesState(1, &avgCap, 1);
            ParaWriteStoreChgDhgTimes(1, &gGStatistTime_70[eTime70_ChgCycle], 1);
        }
    }
    /*历史电量未清零*/
    else
    {
        /*充放电次数复位*/
    	ClrGroupChgDhgTimesAPI();
        sHisChgDhgState = avgCap;
        ParaWriteStoreChgTimesState(1, &avgCap, 1);
    }
    #endif
}

/*=============================================================================================
** @Name      : void GroupAllowTimeStatisTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组可充放电时间统计任务
** @The notes : 2s
**===========================================================================================*/
void GroupAllowTimeStatisTask(void)
{
    s16 power = 0;
    u16 sumVol = 0;
    u32 chgCap = 0;
    u32 dhgCap = 0;
    u32 calcTime = 0;
    //u16 chgPower = 0;
    //u16 dhgPower = 0;
    u16 chgCurr = 0;
    u16 dhgCurr = 0;

    /*电池组初始化时不检测*/
    if(eWORK_INIT == GetGWorkStateAPI())
    {
        return;
    }

    dhgCap = GetGroupNowCapAPI();
    chgCap = GetGroupTotalCapAPI() - dhgCap;

    sumVol = GetGCellSumVoltAPI() + 1;
    power = GetGSampOutPowerAPI();

    /*过压可充电量为0*/
    if((1 == StaGetVHLimStateHook())
        || (1 == StaGetBattVStateHook()))
    {
        chgCap = 0;
    }

    /*低压可放电量为0*/
    if((1 == StaGetVLLimStateHook())
        || (2 == StaGetBattVStateHook()))
    {
        dhgCap = 0;
    }

    /*通过功率计算
    //充电则放电取额定功率
    if(1 == GetChgDhgState())
    {
        dhgPower = gGBmuGenPara_102[eBmuGenPara102_DhgPRatLim];

        //充电功率大于0.1kW则转为正数
        if(power < -1)
        {
            chgPower = ABS(0, power);
        }
        //充电功率过小取额定功率
        else
        {
            chgPower = gGBmuGenPara_102[eBmuGenPara102_ChgPRatLim];
        }
    }
    //放电则充电取额定功率
    else if(2 == GetChgDhgState())
    {
        chgPower = gGBmuGenPara_102[eBmuGenPara102_ChgPRatLim];

        //放电功率取实际功率
        if(power > 1)
        {
            dhgPower = (u16)power;
        }
        //放电功率过小取额定功率
        else
        {
            dhgPower = gGBmuGenPara_102[eBmuGenPara102_DhgPRatLim];
        }
    }
    //待机则充放电功率取额定功率
    else
    {
        chgPower = gGBmuGenPara_102[eBmuGenPara102_ChgPRatLim];
        dhgPower = gGBmuGenPara_102[eBmuGenPara102_DhgPRatLim];
    }

    calcTime = (u32)sumVol * (chgCap / 100) * 6 / (u32)chgPower / 1000;
    if((calcTime <= 1) && (GetGChgLimCurrAPI() > 0))       //计算无剩余时间但仍可充电
    {
        gGStatistTime_70[eTime70_CanChg] = 1;                 //取剩余1min
    }
    else
    {
        gGStatistTime_70[eTime70_CanChg] = (gGStatistTime_70[eTime70_CanChg] + calcTime) / 2;
    }

    calcTime = (u32)sumVol * (dhgCap / 100) * 6 / (u32)dhgPower / 1000;
    if((calcTime <= 1) && (GetGDhgLimCurrAPI() > 0))       //计算无剩余时间但仍可充电
    {
        gGStatistTime_70[eTime70_CanChg] = 1;                 //取剩余1min
    }
    else
    {
        gGStatistTime_70[eTime70_CanChg] = (gGStatistTime_70[eTime70_CanChg] + calcTime) / 2;
    }
    */

    /*通过平均电流计算*/
    /*充电则放电取额定功率*/
    if(eCURR_CHG == GetGChgDhgStateAPI())
    {
        dhgCurr = (u16)((u32)gGBmuGenPara_102[eBmuGenPara102_DhgPRatLim] * 10000 / sumVol);

        /*充电功率大于0.1kW则转为正数*/
        if((power < -1) && (GetGroupAvgCurrAPI() < -1))
        {
            chgCurr = ABS(0, GetGroupAvgCurrAPI());
        }
        /*充电功率过小取额定功率*/
        else
        {
            chgCurr = (u16)((u32)gGBmuGenPara_102[eBmuGenPara102_ChgPRatLim] * 10000 / sumVol);
        }
    }
    /*放电则充电取额定功率*/
    else if(eCURR_DHG == GetGChgDhgStateAPI())
    {
        chgCurr = (u16)((u32)gGBmuGenPara_102[eBmuGenPara102_ChgPRatLim] * 10000 / sumVol);

        /*放电功率取实际功率*/
        if((power > 1) && (GetGroupAvgCurrAPI() > 1))
        {
            dhgCurr = (u16)GetGroupAvgCurrAPI();
        }
        /*放电功率过小取额定功率*/
        else
        {
            dhgCurr = (u16)((u32)gGBmuGenPara_102[eBmuGenPara102_DhgPRatLim] * 10000 / sumVol);
        }
    }
    /*待机则充放电功率取额定功率*/
    else
    {
        chgCurr = (u16)((u32)gGBmuGenPara_102[eBmuGenPara102_ChgPRatLim] * 10000 / sumVol);
        dhgCurr = (u16)((u32)gGBmuGenPara_102[eBmuGenPara102_DhgPRatLim] * 10000 / sumVol);
    }

    calcTime = (u32)chgCap * 6 / chgCurr / 10;
    if((calcTime <= 1) && (GetGChgLimCurrAPI() > 0))          /*计算无剩余时间但仍可充电*/
    {
    	gGStatistTime_70[eTime70_CanChg] = 1;                 /*取剩余1min*/
    }
    else
    {
    	gGStatistTime_70[eTime70_CanChg] = (gGStatistTime_70[eTime70_CanChg] + calcTime) / 2;
    }

    calcTime = (u32)dhgCap * 6 / dhgCurr / 10;
    if((calcTime <= 1) && (GetGDhgLimCurrAPI() > 0))         /*计算无剩余时间但仍可充电*/
    {
    	gGStatistTime_70[eTime70_CanDhg] = 1;                /*取剩余1min*/
    }
    else
    {
    	gGStatistTime_70[eTime70_CanDhg] = (gGStatistTime_70[eTime70_CanDhg] + calcTime) / 2;
    }
}

/*=============================================================================================
** @Name      : void GroupErrNumStatisTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组故障告警次数统计任务
** @The notes : 2s
**===========================================================================================*/
void GroupErrNumStatisTask(void)
{
    u16 rcdTime = 0;
    static u16 sMonWarNum = 0;
    static u16 sMonErrNum = 0;
    static u16 sHisWarNum = 0;
    static u16 sHisErrNum = 0;
    static u16 sHisChgState = 0;

    /*获取当前年月份*/
    rcdTime = GetSysDateClockAPI().year - 2000;
    rcdTime = (rcdTime << 8) | GetSysDateClockAPI().month;

    /*月份切换*/
    if(sHisMonErrTime != rcdTime)
    {
        /*月故障数目清零*/
    	gGStatistErr_71[eErr71_MonWarNum] = 0;
    	gGStatistErr_71[eErr71_MonErrNum] = 0;

	    /*重新记录时间*/
	    ParaWriteStoreMonErrTime(1, &rcdTime, 1);
	    sHisMonErrTime = rcdTime;
    }

	/*初始上电*/
	if(0 == sHisWarNum)
	{
		sMonWarNum = gGStatistErr_71[eErr71_MonWarNum];
		sHisWarNum = gGStatistErr_71[eErr71_HisWarNum];

		/*月数目大于累计数目*/
		if(gGStatistErr_71[eErr71_MonWarNum] > gGStatistErr_71[eErr71_HisWarNum])
		{
			gGStatistErr_71[eErr71_MonWarNum] = gGStatistErr_71[eErr71_HisWarNum];
		}
	}
	/*当月新增10条或充放电停止或故障/关机记录一次*/
	else if((ABS(sMonWarNum, gGStatistErr_71[eErr71_MonWarNum]) >= HISERR_TIMES_WR_TIMES)
		|| ((sMonWarNum != gGStatistErr_71[eErr71_MonWarNum])
		&& (((eCURR_IDLE == GetGChgDhgStateAPI())
		&& (eCURR_IDLE != sHisChgState))
		|| (eWORK_ERR == GetGWorkStateAPI())
		|| (eWORK_OFF == GetGWorkStateAPI()))))
	{
		/*记录当月告警数目*/
		ParaWriteStoreHisErrNum(1, &gGStatistErr_71[eErr71_MonWarNum], 1);
		ParaWriteStoreHisErrNum(3, &gGStatistErr_71[eErr71_HisWarNum], 1);
		sMonWarNum = gGStatistErr_71[eErr71_MonWarNum];
	}
	/*累计10条或充放电停止或故障/关机记录一次*/
	else if((ABS(sHisWarNum, gGStatistErr_71[eErr71_HisWarNum]) >= HISERR_TIMES_WR_TIMES)
		|| ((sHisWarNum != gGStatistErr_71[eErr71_HisWarNum])
		&& (((eCURR_IDLE == GetGChgDhgStateAPI())
		&& (eCURR_IDLE != sHisChgState))
		|| (eWORK_ERR == GetGWorkStateAPI())
		|| (eWORK_OFF == GetGWorkStateAPI()))))
	{
		/*记录累计告警数目*/
		ParaWriteStoreHisErrNum(1, &gGStatistErr_71[eErr71_MonWarNum], 1);
		ParaWriteStoreHisErrNum(3, &gGStatistErr_71[eErr71_HisWarNum], 1);
		sHisWarNum = gGStatistErr_71[eErr71_HisWarNum];
	}

	/*初始上电*/
	if(0 == sHisErrNum)
	{
		sMonErrNum = gGStatistErr_71[eErr71_MonErrNum];
		sHisErrNum = gGStatistErr_71[eErr71_HisErrNum];

		/*月数目大于累计数目*/
		if(gGStatistErr_71[eErr71_MonErrNum] > gGStatistErr_71[eErr71_HisErrNum])
		{
			gGStatistErr_71[eErr71_MonErrNum] = gGStatistErr_71[eErr71_HisErrNum];
		}
	}
	/*单月10条或充放电停止或故障记录一次*/
	else if((ABS(sMonErrNum, gGStatistErr_71[eErr71_MonErrNum]) >= HISERR_TIMES_WR_TIMES)
		|| ((sMonErrNum != gGStatistErr_71[eErr71_MonErrNum])
		&& (((eCURR_IDLE == GetGChgDhgStateAPI())
		&& (eCURR_IDLE != sHisChgState))
		|| (eWORK_ERR == GetGWorkStateAPI())
		|| (eWORK_OFF == GetGWorkStateAPI()))))
	{
		/*记录累计故障数目*/
		ParaWriteStoreHisErrNum(2, &gGStatistErr_71[eErr71_MonErrNum], 1);
		ParaWriteStoreHisErrNum(4, &gGStatistErr_71[eErr71_HisErrNum], 1);
		sMonErrNum = gGStatistErr_71[eErr71_MonErrNum];
	}
	/*累计10条或充放电停止或故障记录一次*/
	else if((ABS(sHisErrNum, gGStatistErr_71[eErr71_HisErrNum]) >= HISERR_TIMES_WR_TIMES)
		|| ((sHisErrNum != gGStatistErr_71[eErr71_HisErrNum])
		&& (((eCURR_IDLE == GetGChgDhgStateAPI())
		&& (eCURR_IDLE != sHisChgState))
		|| (eWORK_ERR == GetGWorkStateAPI())
		|| (eWORK_OFF == GetGWorkStateAPI()))))
	{
		/*记录累计故障数目*/
		ParaWriteStoreHisErrNum(2, &gGStatistErr_71[eErr71_MonErrNum], 1);
		ParaWriteStoreHisErrNum(4, &gGStatistErr_71[eErr71_HisErrNum], 1);
		sHisErrNum = gGStatistErr_71[eErr71_HisErrNum];
	}

    sHisChgState = GetGChgDhgStateAPI();
}

/*=============================================================================================
** @Name      : void GroupHisErrNumAddHandle(u8 type)
** @Input     : type:故障类型 0:告警 1:故障
** @Output    : void
** @Function  : 电池组故障告警累加处理
** @The notes : 确定是故障或告警后调用
**===========================================================================================*/
void GroupHisErrNumAddHandle(u8 type)
{
	/*告警*/
	if(0 == type)
	{
		gGStatistErr_71[eErr71_MonWarNum]++;
	    gGStatistErr_71[eErr71_HisWarNum]++;
	}
	/*故障*/
	else if(1 == type)
	{
		gGStatistErr_71[eErr71_MonErrNum]++;
	    gGStatistErr_71[eErr71_HisErrNum]++;
	}
}

/*=============================================================================================
** @Name      : void ClrGroupChgDhgTimesAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除电池组充放电次数
** @The notes :
**===========================================================================================*/
void ClrGroupChgDhgTimesAPI(void)
{
	gGStatistTime_70[eTime70_ChgCycle] = 1;

	ParaWriteStoreChgDhgTimes(1, &gGStatistTime_70[eTime70_ChgCycle], 1);
}

/*=============================================================================================
** @Name      : void ClrGroupHisErrNumAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除电池组累计故障告警数目
** @The notes : 当月值和累计值
**===========================================================================================*/
void ClrGroupHisErrNumAPI(void)
{
	gGStatistErr_71[eErr71_MonWarNum] = 0;
	gGStatistErr_71[eErr71_MonErrNum] = 0;
	gGStatistErr_71[eErr71_HisWarNum] = 0;
	gGStatistErr_71[eErr71_HisErrNum] = 0;

	ParaWriteStoreHisErrNum(1, gGStatistErr_71, 4);
}
