/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : CalcInterface.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 计算模块用户接口函数
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "PublicInfo.h"
#include "PortCheck.h"
#include "ChgTactics.h"
#include "UserTactic.h"
#include "TacInterface.h"
#include "DiagInterface.h"
#include "CharactCalc.h"
#include "CurrLimCalc.h"
#include "CalcInterface.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gGBmuGenPara_102[];          /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];       /*主控高级参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 CalcGetGroupCellVExpStateHook(void)
** @Input     : void
** @Output    : 电池组单体电压异常状态: 0:正常 1:异常
** @Function  : 获取电池组单体电压异常状态
** @The notes : 从控电压采样失效/断线/CAN断线时从控单节电压异常
**===============================================================================================*/
u8 CalcGetGroupCellVExpStateHook(void)
{
	static u32 sErrTime = 0;

	/*未接收到全部单体电压或采样断线*/
	if((2 != DiagGetRcvAllSlaveFlagAPI())
		|| (2 != DiagGetRcvAllCellFlagAPI())
		|| (DiagGetCellSampErrFlagAPI() > 0)
		|| (DiagGetCellVOffCellBitAPI() > 0)
		|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_SAMP_EXP))
		/*|| (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_SAMP_EXP))*/
		/*|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_INT_LOFF))
		|| (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_INT_LOFF))*/)
	{
		/*连续3s异常或系统已故障*/
		if((GetSysSecondTimeAPI() >= (sErrTime + 3))
			|| (eWORK_ERR == GetGWorkStateAPI()))
		{
			return(1);
		}
		/*首次检测到异常*/
		else if(0 == sErrTime)
		{
			/*记录时间*/
			sErrTime = GetSysSecondTimeAPI();
		}
	}
	else
	{
		sErrTime = 0;
	}

	return(0);
}

/*=============================================================================================
** @Name      : u8 CalcGetGroupCellTExpStateHook(void)
** @Input     : void
** @Output    : 电池组单体温度异常状态 0:正常 1:异常
** @Function  : 获取电池组单体温度异常状态
** @The notes : 从控温度计采样失效/断线/CAN断线时从控单节温度异常
**===========================================================================================*/
u8 CalcGetGroupCellTExpStateHook(void)
{
	static u32 sErrTime = 0;

	/*未接收到全部单体温度或采样断线*/
	if((2 != DiagGetRcvAllSlaveFlagAPI())
		|| (2 != DiagGetRcvAllCellFlagAPI())
		|| (DiagGetCellSampErrFlagAPI() > 0)
		|| (DiagGetCellTOffCellBitAPI() > 0)
		|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_SAMP_EXP))
		/*|| (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_SAMP_EXP))*/
		/*|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_INT_LOFF))
		|| (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_INT_LOFF))*/)
	{
		/*首次检测到异常*/
		if(0 == sErrTime)
		{
			/*记录时间*/
			sErrTime = GetSysSecondTimeAPI();
		}
		/*连续2s异常或系统已故障*/
		else if((GetSysSecondTimeAPI() >= (sErrTime + 2))
			|| (eWORK_ERR == GetGWorkStateAPI()))
		{
			return(1);
		}
	}
	else
	{
		sErrTime = 0;
	}

	return(0);
}

/*=================================================================================================
** @Name      : u8 CalcGetGropuNeedSysLimStateHook(void)
** @Input     : void
** @Output    : 充放电需限制状态:0正常 1需限制充放电
** @Function  : 获取电池组充放电需限制状态
** @The notes : 产生充放电限制故障或按下停机按钮
**===============================================================================================*/
u8 CalcGetGropuNeedSysLimStateHook(void)
{
	e_ErrSort sort = 0;

    //获取当前全部故障级别
	sort = DiagGetGroupNowErrLevelAPI(0);

    //产生充放电限制/故障/或SOS异常或用户需求按钮停机
    if((eErr_SysLim == sort) || (eErr_SysPro == sort)
    	|| (DiagGetGroupSosLevel() > 0)
        || (1 == GetButtonStopReqStateAPI())
        || (1 == GetFstChgStopReqFlagAPI()))
    {
    	return(1);
    }

    return(0);
}

/*=============================================================================================
** @Name      : u8 CalcGetGroupNeedChgLimStateHook(void)
** @Input     : void
** @Output    : 电池组需要充电限制状态: 0正常 1需充电限制
** @Function  : 获取电池组需要充电限制状态
** @The notes : 充满或充电限制
**===========================================================================================*/
u8 CalcGetGroupNeedChgLimStateHook(void)
{
	e_ErrSort sort = 0;

    //获取当前充电故障级别
	sort = DiagGetGroupNowErrLevelAPI(1);

	//产生充电限制/故障或电池充满
	if((eErr_ChgLim == sort) || (eErr_ChgPro == sort)
		|| (1 == TacGetGroupBattStateAPI())
		|| (TacGetGAskChgProFlagAPI() > 0))
	{
		return(1);
	}

	return(0);
}

/*=============================================================================================
** @Name      : u8 CalcGetGroupNeedDhgLimStateHook(void)
** @Input     : void
** @Output    : 电池组需要放电限制状态: 0正常 1需放电限制
** @Function  : 获取电池组需要放电限制状态
** @The notes : 放空或放电限制
**===========================================================================================*/
u8 CalcGetGroupNeedDhgLimStateHook(void)
{
	e_ErrSort sort = 0;

    //获取当前放电故障级别
	sort = DiagGetGroupNowErrLevelAPI(2);

	//产生放电限制/故障或电池放空
	if((eErr_DhgLim == sort) || (eErr_DhgPro == sort)
		|| (2 == TacGetGroupBattStateAPI())
		|| (TacGetGAskDhgProFlagAPI() > 0))
	{
		return(1);
	}

	return(0);
}

/*=============================================================================================
** @Name      : u16 CalcGetChgLimMaxSumVHook(void)
** @Input     : void
** @Output    : 电池组总压充电截止最大电压值(单位:0.1V)
** @Function  : 获取电池组总压充电截止最大电压
** @The notes : 总压高压2级限值(充电限制)
**===========================================================================================*/
u16 CalcGetChgLimMaxSumVHook(void)
{
    return(gGBmuGenPara_102[eBmuGenPara102_SumVH2Lim]);
}

/*=============================================================================================
** @Name      : u16 CalcGetDhgLimMinSumVHook(void)
** @Input     : void
** @Output    : 电池组总压放电截止最小电压值(单位:0.1V)
** @Function  : 获取电池组总压放电截止最小电压
** @The notes : 总压低压2级限值(放电限制)
**===========================================================================================*/
u16 CalcGetDhgLimMinSumVHook(void)
{
    return(gGBmuGenPara_102[eBmuGenPara102_SumVL2Lim]);
}

/*=============================================================================================
** @Name      : u16 CalcGetChgLimCellVoltHook(void)
** @Input     : void
** @Output    : 电池组充电截止单体电压值(单位:1mV)
** @Function  : 获取电池组充电截止单体电压
** @The notes : 单体高压2级限值(充电限制)
**===========================================================================================*/
u16 CalcGetChgLimCellVoltHook(void)
{
    return(gGBmuGenPara_102[eBmuGenPara102_VH2Lim]);
}

/*=============================================================================================
** @Name      : u16 CalcGetDhgLimCellVoltHook(void)
** @Input     : void
** @Output    : 电池组放电截止单体电压值(单位:1mV)
** @Function  : 获取电池组放电截止单体电压
** @The notes : 单体低压2级限值(放电限制)
**===========================================================================================*/
u16 CalcGetDhgLimCellVoltHook(void)
{
    return(gGBmuGenPara_102[eBmuGenPara102_VL2Lim]);
}

/*=============================================================================================
** @Name      : s16 CalcGetChgCDnFstHTempHook(void)
** @Input     : void
** @Output    : 充电高温降电流限值的起始温度 1℃
** @Function  : 获取充电高温降电流限值的起始温度
** @The notes : 默认45℃开始限充电电流限值
**===========================================================================================*/
s16 CalcGetChgCDnFstHTempHook(void)
{
	s16 temp = 0;

	/*比充电截止提前5℃*/
	temp = (s16)gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim] - 5;

	/*不得低于40℃*/
	if(temp < 40)
	{
		temp = 40;
	}

	//需要提前高温预警降功率
	if(temp > (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnFstT])
	{
		//取高温预警降功率温度
		temp = (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnFstT];
	}

    return(temp);
}

/*=============================================================================================
** @Name      : s16 CalcGetChgCDnLstHTempHook(void)
** @Input     : void
** @Output    : 充电高温降电流限值结束温度 1℃
** @Function  : 获取充电高温降电流限值结束温度
** @The notes : 充电高温2级(充电截止)为降充电电流限值终点
**===========================================================================================*/
s16 CalcGetChgCDnLstHTempHook(void)
{
	s16 temp = 0;

	/*功率下降比率有效*/
	if(gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnRate] > 0)
	{
		/*计算按功率下降比率到0的温度*/
		temp = (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnFstT] - 1 + (1000 / gGBmuHigLevPara_103[eBmuHigLevPara103_ChgHTDnRate]);

		/*不能高于过温限值*/
		if(temp > (s16)gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim])
		{
			temp = (s16)gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim];
		}
	}
	else
	{
		temp = (s16)gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim];
	}

    return(temp);
}

/*=============================================================================================
** @Name      : s16 CalcGetDhgCDnFstHTempHook(void)
** @Input     : void
** @Output    : 放电高温降电流限值的起始温度 1℃
** @Function  : 获取放电高温降电流限值的起始温度
** @The notes : 默认45℃开始限放电电流限值
**===========================================================================================*/
s16 CalcGetDhgCDnFstHTempHook(void)
{
	s16 temp = 0;

	/*取过温末端保护降功率温度(比放电截止提前10℃)*/
	temp = (s16)gGBmuGenPara_102[eBmuGenPara102_DhgTH2Lim] - 10;

	/*不得低于40℃*/
	if(temp < 40)
	{
		temp = 40;
	}

	//需要提前高温预警降功率
	if(temp > (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnFstT])
	{
		//取高温预警降功率温度
		temp = (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnFstT];
	}

    return(temp);
}

/*=============================================================================================
** @Name      : s16 CalcGetDhgCDnLstHTempHook(void)
** @Input     : void
** @Output    : 放电高温降电流限值结束温度 1℃
** @Function  : 获取放电高温降电流限值结束温度
** @The notes : 放电高温2级(放电截止)为降放电电流限值终点
**===========================================================================================*/
s16 CalcGetDhgCDnLstHTempHook(void)
{
	s16 temp = 0;

	/*功率下降比率有效*/
	if(gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnRate] > 0)
	{
		/*计算按功率下降比率到0的温度*/
		temp = (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnFstT] - 1 + (1000 / gGBmuHigLevPara_103[eBmuHigLevPara103_DhgHTDnRate]);

		/*不能高于过温限值*/
		if(temp > (s16)gGBmuGenPara_102[eBmuGenPara102_DhgTH2Lim])
		{
			temp = (s16)gGBmuGenPara_102[eBmuGenPara102_DhgTH2Lim];
		}
	}
	else
	{
		temp = (s16)gGBmuGenPara_102[eBmuGenPara102_DhgTH2Lim];
	}
    return(temp);
}

#if(0)
/*=============================================================================================
** @Name      : s16 CalcGetChgCDnFstLTempHook(void)
** @Input     : void
** @Output    : 充电低温降电流限值的起始温度 1℃
** @Function  : 获取充电低温降电流限值的起始温度
** @The notes : 默认5℃开始限充电电流限值
**===========================================================================================*/
s16 CalcGetChgCDnFstLTempHook(void)
{
	s16 temp = 0;

	/*比充电截止提前5℃*/
	temp = (s16)gGBmuGenPara_102[eBmuGenPara102_ChgTL2Lim] + 5;

	/*不得高于10℃*/
	if(temp > 10)
	{
		temp = 10;
	}

    return(temp);
}

/*=============================================================================================
** @Name      : s16 CalcGetChgCDnLstLTempHook(void)
** @Input     : void
** @Output    : 充电低温降电流限值结束温度 1℃
** @Function  : 获取充电低温降电流限值结束温度
** @The notes : 充电低温2级(充电截止)为降充电电流限值终点
**===========================================================================================*/
s16 CalcGetChgCDnLstLTempHook(void)
{
    return((s16)gGBmuGenPara_102[eBmuGenPara102_ChgTL2Lim]);
}

/*=============================================================================================
** @Name      : s16 CalcGetDhgCDnFstLTempHook(void)
** @Input     : void
** @Output    : 放电低温降电流限值的起始温度 1℃
** @Function  : 获取放电低温降电流限值的起始温度
** @The notes : 默认0℃开始限放电电流限值
**===========================================================================================*/
s16 CalcGetDhgCDnFstLTempHook(void)
{
	s16 temp = 0;

	/*比充电截止提前10℃*/
	temp = (s16)gGBmuGenPara_102[eBmuGenPara102_DhgTL2Lim] + 10;

	/*不得高于5℃*/
	if(temp > 5)
	{
		temp = 5;
	}

    return(temp);
}

/*=============================================================================================
** @Name      : s16 CalcGetDhgCDnLstLTempHook(void)
** @Input     : void
** @Output    : 放电低温降电流限值结束温度 1℃
** @Function  : 获取放电低温降电流限值结束温度
** @The notes : 放电低温2级(放电截止)为降放电电流限值终点
**===========================================================================================*/
s16 CalcGetDhgCDnLstLTempHook(void)
{
    return((s16)gGBmuGenPara_102[eBmuGenPara102_DhgTL2Lim]);
}
#endif

/*=============================================================================================
** @Name      : u16 CalcGetDcSumSampVolt(void)
** @Input     : void
** @Output    : 电池组直流端总电压采样值(0.1V)
** @Function  : 获取电池组直流端总电压采样值
** @The notes : 根据采样方式
**===========================================================================================*/
u16 CalcGetDcSumSampVolt(void)
{
	//通过通讯获取
    #if(2 == BMU_DC_VH_TYPE)
    //有负极接触器
    #if(1 == BMU_NEG_SWH_EN)
    //返回经处理后的总压值
    return(GetGVcuDcSampDealSumVolt());
    //无负极接触器
    #else
    //与HVCU通讯正常
    if((2 == DiagGetRcvAllPeripFlagAPI())
        && (eErr_B == DiagGetErrFilStateAPI(eERR_ID_PER_COFF)))
    {
        //直接返回总压采样值
        return(GetGVcuDcSampRealInfoAPI(eVcuInfo80_BattHV);
    }
    //与HVCU通讯中断
    else
    {
        //返回累加总压值
        return(GetGCellSumVoltAPI());
    }
    #endif
    //自身高压不能采样
    #elif(1 == BMU_DC_VH_TYPE)
    //直接返回总压高压采样值
    return(GetGDcSideSampRealInfoAPI(eReferSampInfo200_BattHV));
    //无高压采样
    #else
    //返回累加总压值
    return(GetGCellSumVoltAPI());
    #endif
}

/*=============================================================================================
** @Name      : u16 CalcGetDcPreSampVolt(void)
** @Input     : void
** @Output    : 电池组直流端预充电压采样值(0.1V)
** @Function  : 获取电池组直流端预充电压采样值
** @The notes : 根据采样方式
**===========================================================================================*/
u16 CalcGetDcPreSampVolt(void)
{
    //通过通讯获取
    #if(2 == BMU_DC_VH_TYPE)
    return(GetGVcuDcSampDealPreVolt());
    //通过自身采样获取
    #elif(1 == BMU_DC_VH_TYPE)
	u16 volt = 0;

	volt = GetGDcSideSampRealInfoAPI(eReferSampInfo200_PChgHV);

    return(volt);
    //无高压采样
    #else
    //有负极接触器
    #if(1 == BMU_NEG_SWH_EN)
    //正负接触器闭合
    if((ePSTATE_ON == TacGetGroupSwhCtrlStateAPI(eGroupSwh_Neg))
        && ((ePSTATE_ON == TacGetGroupSwhCtrlStateAPI(eGroupSwh_Mai))
        || (ePSTATE_ON == TacGetGroupSwhCtrlStateAPI(eGroupSwh_Pre))))
    {
        //返回总电压
        return(GetGCellSumVoltAPI());
    }
    //正负接触器断开
    else
    {
        //返回0
        return(0);
    }
    //无负极接触器
    #else
    //正接触器闭合
    if((ePSTATE_ON == GetSwitchBackStateHook(eSWH_MAIN))
        || (ePSTATE_ON == GetSwitchBackStateHook(eSWH_PCHG)))
    {
        //返回总电压
        return(GetGCellSumVoltAPI());
    }
    //正接触器断开
    else
    {
        //返回0
        return(0);
    }
    #endif
    #endif
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
