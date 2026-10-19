/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : EnerInterface.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 电能量计算模块接口函数
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupInfo.h"
#include "PublicInfo.h"
#include "GroupPara.h"
#include "SocUserAPI.h"
#include "SocInterface.h"
#include "ParaInterface.h"
#include "DiagInterface.h"
#include "CalcInterface.h"
#include "EnerInterface.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gGBmuGenPara_102[];  			/*主控通用参数值*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 EnerGetGroupBattReadyHook(void)
** @Input     : void
** @Output    : 0未就绪 1就绪
** @Function  : 获取电池信息就绪状态
** @The notes :
**===========================================================================================*/
u8 EnerGetGroupBattReadyHook(void)
{
    if((BSPGetOSMillSecTimeAPI() < 5000)                            //上电前5s不能执行防止出错
        || (GetGWorkStateAPI() == eWORK_INIT)                       //系统初始化
        || (GetGWorkStateAPI() == eWORK_ERR)                        //系统故障
        || (GetGWorkStateAPI() == eWORK_OFF)                        //系统关机
		|| (2 != DiagGetRcvAllSlaveFlagAPI())                       //未接收到所有从控信息
		|| (2 != DiagGetRcvAllCellFlagAPI())                        //未接收到所有单体信息
		|| (DiagGetCellSampErrFlagAPI() > 0)                        //单体采样失效
		|| (DiagGetCellVOffCellBitAPI() > 0)                        //单体电压断线
		|| (DiagGetCellTOffCellBitAPI() > 0)                        //单体温度断线
        || (ABS(GetGCellMaxVoltAPI(), GetGCellMinVoltAPI()) >= 400) //单体压差过大
        || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_SAMP_EXP))     //采样芯片异常
        || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_SAMP_VOFF))    //采样电压断线
        || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_SAMP_TOFF))    //采样温度断线
        || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_INT_COFF)))    //主从CAN断线故障
    {
        return(0);
    }

    return(1);
}

/*=============================================================================================
** @Name      : u8 EnerGetGroupVHProStateHook(void)
** @Input     : void
** @Output    : 电池组充电高压保护状态:0正常 1充电高压保护
** @Function  : 获取电池组充电高压保护状态
** @The notes :
**===========================================================================================*/
u8 EnerGetGroupVHProStateHook(void)
{
    u8 state = 0;

    /*严重过压且电压扔超限*/
    if((eErr_B == DiagGetErrLastStateAPI(eERR_ID_CEL_VH1))
        && (GetGCellMaxVoltAPI() >= gGBmuGenPara_102[eBmuGenPara102_VH2Lim]))
    {
        state = 1;
    }
    else
    {
        state = 0;
    }

    return(state);
}

/*=============================================================================================
** @Name      : u8 EnerGetGroupVLProStateHook(void)
** @Input     : void
** @Output    : 电池组放电低压保护状态:0正常 1充电高压保护
** @Function  : 获取电池组放电低压保护状态
** @The notes :
**===========================================================================================*/
u8 EnerGetGroupVLProStateHook(void)
{
    u8 state = 0;

    /*严重欠压且电压扔超限*/
    if((eErr_B == DiagGetErrLastStateAPI(eERR_ID_CEL_VL1))
        && (GetGCellMinVoltAPI() <= gGBmuGenPara_102[eBmuGenPara102_VL2Lim]))
    {
        state = 1;
    }
    else
    {
        state = 0;
    }

    return(state);
}

/*=============================================================================================
** @Name      : u8 EnerGetGroupVHLimStateHook(void)
** @Input     : void
** @Output    : 电池组充电高压限制状态:0正常 1充电高压限制
** @Function  : 获取电池组充电高压限制状态
** @The notes :
**===========================================================================================*/
u8 EnerGetGroupVHLimStateHook(void)
{
    u8 state = 0;

    /*一般过压*/
    if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CEL_VH2))
    {
        state = 1;
    }
    else
    {
        state = 0;
    }

    return(state);
}

/*=============================================================================================
** @Name      : u8 EnerGetGroupVLLimStateHook(void)
** @Input     : void
** @Output    : 电池组放电欠压限制状态:0正常 1放电欠压限制
** @Function  : 获取电池组放电欠压限制状态
** @The notes :
**===========================================================================================*/
u8 EnerGetGroupVLLimStateHook(void)
{
    u8 state = 0;

    /*一般欠压*/
    if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CEL_VL2))
    {
        state = 1;
    }
    else
    {
        state = 0;
    }

    return(state);
}

/*=============================================================================================
** @Name      : u8 EnerGetUserSocStopReqHook(void)
** @Input     : void
** @Output    : TRUE:请求停止SOC FALSE:无请求
** @Function  : 获取用户SOC停止显示请求
** @The notes : 容量自动标定时停止SOC显示,停止在50%
**===========================================================================================*/
u8 EnerGetUserSocStopReqHook(void)
{
	/*非初始化时需要容量标定(长时间未标定)*/
//	if((eWORK_INIT != GetGWorkStateAPI()) && ((GetCapEnerDropExpAPI() >> 4) > 0))
//	{
//		//空闲时SOC保持不变
//		//充电时SOC超过50%不能继续上涨
//		//放电时SOC低于50%不能继续下降
//		if(((0 == GetGSampOutCurrAPI()) && (ePRO_FULL != GetGProtectStateAPI()) && (ePRO_EMPTY != GetGProtectStateAPI()))
//			|| ((GetGSampOutCurrAPI() < 0) && (GetGBattSocAPI() >= 500) && (ePRO_FULL != GetGProtectStateAPI()))
//			|| ((GetGSampOutCurrAPI() > 0) && (GetGBattSocAPI() <= 500) && (ePRO_EMPTY != GetGProtectStateAPI())))
//		{
//			/*需要停止SOC*/
//			return(TRUE);
//		}
//	}

	/*不需要停止SOC*/
	return(FALSE);
}

/*=============================================================================================
** @Name      : u8 EnerGetUserSoeStopReqHook(void)
** @Input     : void
** @Output    : TRUE:请求停止SOE FALSE:无请求
** @Function  : 获取用户SOE停止显示请求
** @The notes : 容量自动标定时停止SOE显示,停止在50%
**===========================================================================================*/
u8 EnerGetUserSoeStopReqHook(void)
{
	/*非初始化时需要容量标定*/
//	if((eWORK_INIT != GetGWorkStateAPI()) && (GetCapEnerDropExpAPI() > 0))
//	{
//		//空闲时SOE保持不变
//		//充电时SOE超过50%不能继续上涨
//		//放电时SOE低于50%不能继续下降
//		if(((0 == GetGSampOutCurrAPI()) && (ePRO_FULL != GetGProtectStateAPI()) && (ePRO_EMPTY != GetGProtectStateAPI()))
//			|| ((GetGSampOutCurrAPI() < 0) && (GetGBattSoeAPI() >= 500) && (ePRO_FULL != GetGProtectStateAPI()))
//			|| ((GetGSampOutCurrAPI() > 0) && (GetGBattSoeAPI() <= 500) && (ePRO_EMPTY != GetGProtectStateAPI())))
//		{
//			/*需要停止SOE*/
//			return(TRUE);
//		}
//	}

	/*不需要停止SOE*/
	return(FALSE);
}

/*=============================================================================================
**以下是输出类函数
**===========================================================================================*/
/*=============================================================================================
** @Name      : void EnerChangEepGNowCapHook(u32 cap)
** @Input     : cap:当前容量 1mAH
** @Output    : void
** @Function  : 修改EEP当前容量值
** @The notes : 可用容量
**===========================================================================================*/
void EnerChangEepGNowCapHook(u32 cap)
{
	/*写EEP存储*/
	(void)ParaWriteStoreCapFormInfo(1, &cap, 1);
}

/*=============================================================================================
** @Name      : void EnerChangEepGRemainCapHook(u32 cap)
** @Input     : cap:目标剩余容量 1mAH
** @Output    : void
** @Function  : 修改EEP电池组剩余容量值
** @The notes : 包括不能放出来的容量
**===========================================================================================*/
void EnerChangEepGRemainCapHook(u32 cap)
{
	/*写EEP存储*/
	(void)ParaWriteStoreCapFormInfo(2, &cap, 1);
}

/*=============================================================================================
** @Name      : void EnerChangEepGTotalCapHook(u32 cap)
** @Input     : cap:目标总容量 1mAH
** @Output    : void
** @Function  : 修改EEP电池组总容量值
** @The notes : 实测容量
**===========================================================================================*/
void EnerChangEepGTotalCapHook(u32 cap)
{
	u16 temp = 0;

	/*写EEP存储*/
	(void)ParaWriteStoreCapFormInfo(3, &cap, 1);

	/*转化为0.1AH*/
	temp = cap / 100;

	/*参数有变化*/
	if(temp != gGBmuGenPara_102[eBmuGenPara102_ReaAllCap])
	{
		/*写入配置区成功*/
		if(TRUE == ParaWriteStoreGroupGenParaInfo(eBmuGenPara102_ReaAllCap, &temp, 1))
		{
			/*显示区索引*/
			gGBmuGenPara_102[eBmuGenPara102_ReaAllCap] = temp;
		}
	}
}

/*=============================================================================================
** @Name      : void EnerChangEepGStandCapHook(u32 cap)
** @Input     : cap:目标标称容量 1mAH
** @Output    : void
** @Function  : 修改EEP电池组标称容量值
** @The notes : 实测容量
**===========================================================================================*/
void EnerChangEepGStandCapHook(u32 cap)
{
	/*写EEP存储*/
	(void)ParaWriteStoreCapFormInfo(4, &cap, 1);
}

/*=============================================================================================
** @Name      : void EnerChangEepGNowSocHook(u16 soc)
** @Input     : soc:当前显示SOC 0.1%
** @Output    : void
** @Function  : 修改EEP当前显示SOC值
** @The notes : 显示SOC
**===========================================================================================*/
void EnerChangEepGNowSocHook(u16 soc)
{
	/*参数有变化*/
	if(soc != gGBmuGenPara_102[eBmuGenPara102_NowSoc])
	{
		/*写入配置区成功*/
		if(TRUE == ParaWriteStoreGroupGenParaInfo(eBmuGenPara102_NowSoc, &soc, 1))
		{
			/*显示区索引*/
			gGBmuGenPara_102[eBmuGenPara102_NowSoc] = soc;
		}
	}
}

/*=============================================================================================
** @Name      : void EnerChangEepGNowEner(u32 ener)
** @Input     : ener:当前能量 1Wh
** @Output    : void
** @Function  : 修改EEP电池组当前能量值
** @The notes : 可用电能
**===========================================================================================*/
void EnerChangEepGNowEner(u32 ener)
{
	/*写EEP存储*/
	(void)ParaWriteStoreEnerFormInfo(1, &ener, 1);
}

/*=============================================================================================
** @Name      : void EnerChangEepGRemainEner(u32 ener)
** @Input     : ener:剩余能量 1Wh
** @Output    : void
** @Function  : 修改EEP电池组当前剩余能量值
** @The notes : 包括不能放出来的电能
**===========================================================================================*/
void EnerChangEepGRemainEner(u32 ener)
{
	/*写EEP存储*/
	(void)ParaWriteStoreEnerFormInfo(2, &ener, 1);
}

/*=============================================================================================
** @Name      : void EnerChangEepTotalEner(u32 ener)
** @Input     : ener:总能量 1Wh
** @Output    : void
** @Function  : 修改EEP总能量值
** @The notes : 实测总电能
**===========================================================================================*/
void EnerChangEepTotalEner(u32 ener)
{
	u16 temp = 0;

	/*写EEP存储*/
	(void)ParaWriteStoreEnerFormInfo(3, &ener, 1);

	/*转化为0.1kWh*/
	temp = ener / 100;

	/*参数有变化*/
	if(temp != gGBmuGenPara_102[eBmuGenPara102_ReaAllEner])
	{
		/*写入配置区成功*/
		if(TRUE == ParaWriteStoreGroupGenParaInfo(eBmuGenPara102_ReaAllEner, &temp, 1))
		{
			/*显示区索引*/
			gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] = temp;
		}
	}
}

/*=============================================================================================
** @Name      : void EnerChangEepStandEner(u32 ener)
** @Input     : ener:标称能量 1Wh
** @Output    : void
** @Function  : 修改EEP标称能量值
** @The notes : 标准电能值
**===========================================================================================*/
void EnerChangEepStandEner(u32 ener)
{
	/*写EEP存储*/
	(void)ParaWriteStoreEnerFormInfo(4, &ener, 1);
}

/*=============================================================================================
** @Name      : void EnerChangEepGNowSocHook(u16 soe)
** @Input     : soe:当前显示SOE 0.1%
** @Output    : void
** @Function  : 修改EEP当前显示SOE值
** @The notes : 显示SOE
**===========================================================================================*/
void EnerChangEepGNowSoeHook(u16 soe)
{
	/*参数有变化*/
	if(soe != gGBmuGenPara_102[eBmuGenPara102_NowSoe])
	{
		/*写入配置区成功*/
		if(TRUE == ParaWriteStoreGroupGenParaInfo(eBmuGenPara102_NowSoe, &soe, 1))
		{
			/*显示区索引*/
			gGBmuGenPara_102[eBmuGenPara102_NowSoe] = soe;
		}
	}
}

/*=============================================================================================
** @Name      : void EnerChangEepGRatedCapHook(u16 rateCap)
** @Input     : rateCap:电池组额定容量(0.1AH)
** @Output    : void
** @Function  : 修改电池组额定容量
** @The notes : 出厂有效容量
**===========================================================================================*/
void EnerChangEepGRatedCapHook(u16 rateCap)
{
	/*参数有变化*/
	if(rateCap != gGBmuGenPara_102[eBmuGenPara102_RateCap])
	{
		/*写入配置区成功*/
		if(TRUE == ParaWriteStoreGroupGenParaInfo(eBmuGenPara102_RateCap, &rateCap, 1))
		{
			/*显示区索引*/
			gGBmuGenPara_102[eBmuGenPara102_RateCap] = rateCap;
		}
	}
}

/*=============================================================================================
** @Name      : void EnerChangEepGDampNumHook(u16 times)
** @Input     : times:电池额定循环次数
** @Output    : void
** @Function  : 修改电池额定循环次数
** @The notes : 有效衰减次数
**===========================================================================================*/
void EnerChangEepGDampNumHook(u16 times)
{
	/*参数有变化*/
	if(times != gGBmuGenPara_102[eBmuGenPara102_DampNum])
	{
		/*写入配置区成功*/
		if(TRUE == ParaWriteStoreGroupGenParaInfo(eBmuGenPara102_DampNum, &times, 1))
		{
			/*显示区索引*/
			gGBmuGenPara_102[eBmuGenPara102_DampNum] = times;
		}
	}
}

/*=============================================================================================
** @Name      : void EnerChangEepGNowCapSohHook(u16 soh)
** @Input     : soh:电池SOH(0.1%)
** @Output    : void
** @Function  : 修改电池当前总容量SOH值
** @The notes : 有效衰减次数
**===========================================================================================*/
void EnerChangEepGNowCapSohHook(u16 soh)
{
	/*写EEP存储*/
	(void)ParaWriteStoreNowCapSoh(1, &soh, 1);
}

/*=============================================================================================
** @Name      : void EnerAftTotalCapCorrDealHook(u16 befcap, u16 aftcap)
** @Input     : befcap:前置总容量 aftcap:修正后总容量
** @Output    : void
** @Function  : 总容量修正后处理
** @The notes :
**===========================================================================================*/
void EnerAftTotalCapCorrDealHook(u16 befcap, u16 aftcap)
{
	/*事件记录*/
	;
}

/*=============================================================================================
** @Name      : void EnerAftTotalEnerCorrDealHook(u16 befener, u16 aftener)
** @Input     : befener:前置总能量 aftener:修正后总能量
** @Output    : void
** @Function  : 总能量修正后处理
** @The notes :
**===========================================================================================*/
void EnerAftTotalEnerCorrDealHook(u16 befener, u16 aftener)
{
	/*事件记录*/
	;
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/

