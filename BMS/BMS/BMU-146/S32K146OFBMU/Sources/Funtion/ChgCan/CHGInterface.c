/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CHGInterface.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 快充通信包用户接口
** @Instructions : CAN通信
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "DiagInterface.h"
#include "CHGInterface.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gSProjecInfo_40[];                                       /*项目参数信息数组*/
extern u16 gGBmuGenPara_102[];                                      /*主控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void CHGSendMsgUser_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域 DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : CAN通信包发送报文处理
** @The notes : 用户必须补充完善
**===============================================================================================*/
void CHGSendMsgUser_Hook(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
	u8 i = 0;
	t_CANMSG msg;

	if(DLC > 8)
	{
		return;
	}

	msg.ID = ID;
	msg.Len = DLC;

	for(i = 0; i < DLC; i++)
	{
		msg.Data[i] = pdata[i];
	}

    /*发送CAN报文*/
	BSPCANSendQin((e_CANNUM)canNum, &msg);
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupChgVoltLim_Hook(void)
** @Input     : void
** @Output    : 充电最高电压:0.1V
** @Function  : 获取充电最高电压(握手阶段/配置阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupChgVoltLim_Hook(void)
{
    /*总压高压限值(二级告警配置值)*/
    return(gGBmuGenPara_102[eBmuGenPara102_SumVH2Lim]);
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupRatedCap_Hook(void)
** @Input     : void
** @Output    : 电池额定容量:0.1AH
** @Function  : 获取电池组额定容量(握手阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupRatedCap_Hook(void)
{
    /*标称容量*/
    return(gSProjecInfo_40[eSysProj40_StandCap]);
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupRatedVolt_Hook(void)
** @Input     : void
** @Output    : 电池额定电压:0.1V
** @Function  : 获取电池组额定电压(握手阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupRatedVolt_Hook(void)
{
    /*标称总压*/
    return(gSProjecInfo_40[eSysProj40_StandVolt]);
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupChgCellVLim_Hook(void)
** @Input     : void
** @Output    : 充电单体电压限值:1mV
** @Function  : 获取充电电池单体电压限值(配置阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupChgCellVLim_Hook(void)
{
    /*单体高压告警限值*/
    return(gGBmuGenPara_102[eBmuGenPara102_VH2Lim]);
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupChgCurrLim_Hook(void)
** @Input     : void
** @Output    : 充电电流限值:0.1A
** @Function  : 获取充电电流限值(配置阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupChgCurrLim_Hook(void)
{
    /*充电电流限值(配置值)*/
    return(gGBmuGenPara_102[eBmuGenPara102_ChgCH2Lim]);
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupStandEner_Hook(void)
** @Input     : void
** @Output    : 电池组标称电能:0.1kWh
** @Function  : 获取电池组标称电能值(配置阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupStandEner_Hook(void)
{
    /*标称电能*/
    return(gSProjecInfo_40[eSysProj40_StandEner]);
}

/*=================================================================================================
** @Name      : s8 CHGGetGroupChgTempHigLim_Hook(void)
** @Input     : void
** @Output    : 充电最高温度限值:1℃
** @Function  : 获取电池组充电最高温度限值(配置阶段)
** @The notes : 用户补充
**===============================================================================================*/
s8 CHGGetGroupChgTempHigLim_Hook(void)
{
    /*充电过温限值*/
    return(gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim]);
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupSoc_Hook(void)
** @Input     : void
** @Output    : 电池组当前SOC:0.1%
** @Function  : 获取电池组当前SOC值(配置阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupSoc_Hook(void)
{
    /*当前SOC*/
    return(GetGBattSocAPI());
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupSumVolt_Hook(void)
** @Input     : void
** @Output    : 电池组当前总电压:0.1V
** @Function  : 获取电池组当前总电压值(配置阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupSumVolt_Hook(void)
{
    /*当前总电压*/
    return(GetGCellSumVoltAPI());
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupReqVoltLim_Hook(void)
** @Input     : void
** @Output    : 电池组请求电压需求:0.1V
** @Function  : 获取电池组电池组请求电压需求值(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupReqVoltLim_Hook(void)
{
    /*最大总压限值(实时值)*/
    return(GetGChgLimSumVAPI());
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupReqCurrLim_Hook(void)
** @Input     : void
** @Output    : 电池组请求电流需求:0.1A
** @Function  : 获取电池组电池组请求电流需求值(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupReqCurrLim_Hook(void)
{
    /*充电电流限值(实时值)*/
    return(GetGChgLimCurrAPI());
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupChgInputVolt_Hook(void)
** @Input     : void
** @Output    : 充电电压测量值(输入值):0.1V
** @Function  : 获取充电电压测量值(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupChgInputVolt_Hook(void)
{
    /*预充电压值*/
    return(GetGSampPreVoltAPI());
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupChgInputCurr_Hook(void)
** @Input     : void
** @Output    : 充电电流测量值(输入值):0.1A
** @Function  : 获取充电电流测量值(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupChgInputCurr_Hook(void)
{
    /*电池电流值(正值)*/
    if(GetGSampOutCurrAPI() < 0)
    {
    	return((u16)(0 - GetGSampOutCurrAPI()));
    }
    else
    {
    	return(0);
    }
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupMaxCellVolt_Hook(void)
** @Input     : void
** @Output    : 最高电池电压:0.01V
** @Function  : 获取最高电池电压值(充电阶段/结束阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupMaxCellVolt_Hook(void)
{
    /*单体最高电压*/
    return(GetGCellMaxVoltAPI());
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupMaxCellVNum_Hook(void)
** @Input     : void
** @Output    : 最高电压电池号
** @Function  : 获取最高电压电池号(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupMaxCellVNum_Hook(void)
{
    /*单体最高电压电池号*/
    return(GetGCellMaxVNumAPI());
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupMinCellVolt_Hook(void)
** @Input     : void
** @Output    : 最低电池电压:0.01V
** @Function  : 获取最低电池电压值(结束阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupMinCellVolt_Hook(void)
{
    /*单体最低电压*/
    return(GetGCellMinVoltAPI());
}

/*=================================================================================================
** @Name      : u16 CHGGetGroupChgRemainTime_Hook(void)
** @Input     : void
** @Output    : 充电剩余时间 1min
** @Function  : 获取充电剩余时间(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u16 CHGGetGroupChgRemainTime_Hook(void)
{
    /*充电剩余时间*/
    return(GetGCanChgTimeAPI());
}

/*=================================================================================================
** @Name      : s8 CHGGetGroupMaxCellTemp_Hook(void)
** @Input     : void
** @Output    : 最高电池温度:1℃
** @Function  : 获取最高电池温度值(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
s8 CHGGetGroupMaxCellTemp_Hook(void)
{
    /*单体最高温度*/
    return(GetGCellMaxTempAPI());
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupMaxCellTNum_Hook(void)
** @Input     : void
** @Output    : 最高温度电池号
** @Function  : 获取最高温度电池号(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupMaxCellTNum_Hook(void)
{
    /*单体最高温度电池号*/
    return(GetGCellMaxTNumAPI());
}

/*=================================================================================================
** @Name      : s8 CHGGetGroupMinCellTemp_Hook(void)
** @Input     : void
** @Output    : 最低电池温度:1℃
** @Function  : 获取最低电池温度值(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
s8 CHGGetGroupMinCellTemp_Hook(void)
{
    /*单体最低温度*/
    return(GetGCellMinTempAPI());
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupMinCellTNum_Hook(void)
** @Input     : void
** @Output    : 最低温度电池号
** @Function  : 获取最低温度电池号(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupMinCellTNum_Hook(void)
{
    /*单体最低温度电池号*/
    return(GetGCellMinTNumAPI());
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupCellVAlarmState_Hook(void)
** @Input     : void
** @Output    : 电池单体电压告警状态 0:正常 1:过高 2:过低
** @Function  : 获取电池单体电压告警状态(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupCellVAlarmState_Hook(void)
{
    /*单体高低压告警状态*/
    if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CEL_VH2))
    {
    	return(1);
    }
    else if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CEL_VL2))
    {
    	return(2);
    }

    return(0);
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupSocAlarmState_Hook(void)
** @Input     : void
** @Output    : 电池SOC告警状态 0:正常 1:过高 2:过低
** @Function  : 获取电池SOC告警状态(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupSocAlarmState_Hook(void)
{
    /*SOC告警状态*/
    if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CAP_SH1))
    {
    	return(1);
    }
    else if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CAP_SL1))
    {
    	return(2);
    }

    return(0);
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupChgCAlarmState_Hook(void)
** @Input     : void
** @Output    : 电池充电过流告警状态 0:正常 1:过流
** @Function  : 获取电池充电过流告警状态(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupChgCAlarmState_Hook(void)
{
    /*充电过流告警状态*/
    if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CHG_CO2))
    {
    	return(1);
    }

    return(0);
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupTHAlarmState_Hook(void)
** @Input     : void
** @Output    : 电池温度过高告警状态 0:正常 1:过温
** @Function  : 获取电池温度过高告警状态(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupTHAlarmState_Hook(void)
{
    /*充电过温告警状态*/
    if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CHG_TH2))
    {
    	return(1);
    }

    return(0);
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupLeakAlarmState_Hook(void)
** @Input     : void
** @Output    : 电池绝缘告警状态 0:正常 1:不正常
** @Function  : 获取电池绝缘告警状态(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupLeakAlarmState_Hook(void)
{
    /*绝缘告警状态*/
    if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_LEAK2))
    {
    	return(1);
    }

    return(0);
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupLeakAlarmState_Hook(void)
** @Input     : void
** @Output    : 电池连接器连接告警状态 0:正常 1:不正常
** @Function  : 获取电池连接器连接告警状态(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupSwitchAlarmState_Hook(void)
{
    /*接触器告警状态*/
    if((eErr_B == DiagGetErrLastStateAPI(eERR_ID_K0RE_EXP))
        || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_K1RE_EXP))
	    || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_K2RE_EXP))
	    || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_K0MAI_EXP))
	    || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_K1PRE_EXP))
	    || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_K2NEG_EXP)))
    {
    	return(1);
    }

    return(0);
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupChgAllowState_Hook(void)
** @Input     : void
** @Output    : 电池充电允许状态 0:禁止 1:允许
** @Function  : 获取电池充电允许状态(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupChgAllowState_Hook(void)
{
    /*充电允许状态*/
    if((eWORK_RUN == GetGWorkStateAPI())
    	&& (GetGChgLimPowerAPI() > 0))
    {
        return(1);
    }

    return(0);
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupChgFinState_Hook(void)
** @Input     : void
** @Output    : 电池充电终止状态 0:未终止 1:终止
** @Function  : 获取电池充电终止状态(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupChgFinState_Hook(void)
{
    /*充电截止状态*/
    if((ePRO_FULL == GetGProtectStateAPI())
    	|| (ePRO_SYSPRO == GetGProtectStateAPI()))
    {
    	return(1);
    }

    return(0);
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupSumVAlarmState_Hook(void)
** @Input     : void
** @Output    : 电池总压告警状态 0:正常 1:过高 2:过低
** @Function  : 获取电池总压告警状态(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupSumVAlarmState_Hook(void)
{
    /*总压高低压告警状态*/
    if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_SUM_VH2))
    {
    	return(1);
    }
    else if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_SUM_VL2))
    {
    	return(2);
    }

    return(0);
}

/*=================================================================================================
** @Name      : u8 CHGGetGroupHWTHAlarmState_Hook(void)
** @Input     : void
** @Output    : 电池BMS元器件/输出连接器过温告警状态 0:正常 1:过高
** @Function  : 获取电池BMS元器件/输出连接器过温告警状态(充电阶段)
** @The notes : 用户补充
**===============================================================================================*/
u8 CHGGetGroupHWTHAlarmState_Hook(void)
{
    /*分流器过温告警状态*/
    if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_PER_TH2))
    {
    	return(1);
    }

    return(0);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

