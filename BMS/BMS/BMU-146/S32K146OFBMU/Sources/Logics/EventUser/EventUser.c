/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : EventUser.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池事件记录用户处理模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "GroupMsg.h"
#include "PortCheck.h"
#include "AuxDevTactic.h"
#include "DiagInterface.h"
#include "TacInterface.h"
#include "TacUser.h"
#include "RecordInterface.h"
#include "EventUser.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gCANVerTime_0[];                                 /*校时信息*/

extern u16 gGHisEventInfo_46[];	                            /*历史事件记录信息查询数组*/
extern u16 gGUserCtrl_87[];	    	                        /*用户控制命令信息数组*/

extern u16 gGBmuGenPara_102[];                              /*主控通用参数值*/
extern u8 gGHardPara_104[]; 			                    /*主控硬件参数配置信息*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void EventRecordAuxDevDeal(void)
** @Input     : void
** @Output    : void
** @Function  : 事件记录辅助设备处理任务(定周期检查辅助设备变化事件)
** @The notes : 100ms周期
**===============================================================================================*/
static void EventRecordAuxDevDeal(void);

/*=================================================================================================
** @Name      : static u8 EventRcdCheckRepeatedState(eEvent_RcdSort sort, u8 id, u16 info2, u16 info3, u16 info4, u16 info5)
** @Input     : sort:事件类型 id:事件码ID info2-5:附加信息2-5
** @Output    : TRUE:重复事件(不需记录) FALSE:非重复事件(需要记录)
** @Function  : 判断事件记录是否重复
** @The notes : 跟上次比较重复则不再记录
**===============================================================================================*/
static u8 EventRcdCheckRepeatedState(eEvent_RcdSort sort, u8 id, u16 info2, u16 info3, u16 info4, u16 info5);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void EventRcdDisplayInit(void)
** @Input     : void
** @Output    : void
** @Function  : 事件记录显示信息初始化
** @The notes : 读取初始显示的历史事件,用户任务单次调用(避免占用太长系统初始化时间)
**===============================================================================================*/
void EventRcdDisplayInit(void)
{
    t_RcdStoreInfo info = {0};

	//读一条事件记录到查询显示区
	if(TRUE == RcdDisplayOneEventRcdUserAPI(0, &info))
	{
		//事件主要信息
		gGHisEventInfo_46[0] = info.codeH & 0x00ff;
		gGHisEventInfo_46[1] = info.codeL;
		gGHisEventInfo_46[2] = info.year;
		gGHisEventInfo_46[3] = info.MD;
		gGHisEventInfo_46[4] = info.HM;
		gGHisEventInfo_46[5] = info.sec;

		//事件附件信息
		gGHisEventInfo_46[6] = info.info1;
		gGHisEventInfo_46[7] = info.info2;
		gGHisEventInfo_46[8] = info.info3;
		gGHisEventInfo_46[9] = info.info4;
		gGHisEventInfo_46[10] = info.info5;
	}
}

/*=================================================================================================
** @Name      : void EventRecordUserTask(void)
** @Input     : void
** @Output    : void
** @Function  : 事件记录用户任务(定周期检查变化事件)
** @The notes : 100ms周期
**===============================================================================================*/
void EventRecordUserTask(void)
{
	static u8 sFlag = 0;
	static u8 sPwFlag = 0;
	static u8 sHisProState = 0;
	static u8 sHisChgState = 0;
	static u8 sHisModeState = 0;
	static u8 sHisWorkState = 0;
	static u16 sHisInPorState = 0;
	static u16 sHisOutPorState = 0;
	static u32 sHisOutClock = 0;
	static u32 sHisChgClock = 0;

	static u16 sHisChgCurr = 0;
	static u16 sHisDhgCurr = 0;
	static u16 sHisNegVolt = 0;
	static u16 sHisNegTime = 0;
	static u32 sHisNegClock = 0;

	/*记录显示未初始化*/
    if(0 == sFlag)
	{
    	/*记录存储模块未初始化*/
        if(0 == RcdGetEventRcdEnStateAPI())
    	{
        	/*记录存储读写指针初始化*/
        	RcdStoreStateInit();
    	}
        else
        {
        	/*记录显示初始化*/
        	EventRcdDisplayInit();
        	sFlag = 1;
        }
    	return;
	}

    /*记录开机上电事件(高压采样完成时)*/
    if((0 == sPwFlag) && (1 == SampGetOutHVSampFinStateAPI()))
    {
		/*产生开机上电事件*/
		EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_SysPwrOn, GetGSampOutCurrAPI(), GetGSampSumVoltAPI(), GetGSampPreVoltAPI(), GetGBattSocAPI());
		sPwFlag = 1;
    }

	/*保护状态变化*/
	if(sHisProState != GetGProtectStateAPI())
	{
		/*产生事件*/
		EventRcdDisplayUserAPI(eEVN_RCDSORT_STATE, 0, GetGSampOutCurrAPI(), sHisProState, GetGSampSumVoltAPI(), GetGSampPreVoltAPI());
	}

	/*工作状态变化*/
	if(sHisWorkState != GetGWorkStateAPI())
	{
		/*产生事件*/
		EventRcdDisplayUserAPI(eEVN_RCDSORT_STATE, 1, GetGSampOutCurrAPI(), sHisWorkState, GetGSampSumVoltAPI(), GetGSampPreVoltAPI());
	}

	/*充放电状态变化(至少间隔30min[防止非恒流反复记录])*/
	if((sHisChgState != GetGChgDhgStateAPI()) && ((ABS(GetSysSecondTimeAPI(), sHisChgClock) >= 1800) || ((ABS(GetSysSecondTimeAPI(), sHisChgClock) >= 1200)
		&& ((GetGSampOutCurrAPI() >= 500) || (GetGSampOutCurrAPI() <= -500))) || ((ABS(GetSysSecondTimeAPI(), sHisChgClock) >= 600) && (eCURR_IDLE != sHisChgState) && (eCURR_IDLE != GetGChgDhgStateAPI()))))
	{
		/*产生事件*/
		EventRcdDisplayUserAPI(eEVN_RCDSORT_STATE, 2, GetGSampOutCurrAPI(), sHisChgState, GetGChgDhgStateAPI(), GetGSampPreVoltAPI());
		sHisChgState = GetGChgDhgStateAPI();
		sHisChgClock = GetSysSecondTimeAPI();
	}

	/*工作模式状态变化*/
	if(sHisModeState != GetGWorkModeAPI())
	{
		/*产生事件*/
		EventRcdDisplayUserAPI(eEVN_RCDSORT_STATE, 3, GetGSampOutCurrAPI(), sHisModeState, GetGWorkModeAPI(), GetGSampPreVoltAPI());
	}

	sHisProState = GetGProtectStateAPI();
	sHisWorkState = GetGWorkStateAPI();
	sHisModeState = GetGWorkModeAPI();

	/*非上电初始化状态*/
	if(eWORK_INIT != GetGWorkStateAPI())
	{
		/*输入IO口变化*/
		//if(sHisInPorState != GetGInPortStateAPI())
		//K9异常反馈板级回检会跳变
		if(sHisInPorState != (GetGInPortStateAPI() & 0x7fff))
		{
			/*产生事件*/
			EventRcdDisplayUserAPI(eEVN_RCDSORT_INPUT, 0, GetGSampOutCurrAPI(), sHisInPorState, GetGInPortStateAPI(), GetGWorkModeAPI());
			//sHisInPorState = GetGInPortStateAPI();
			sHisInPorState = GetGInPortStateAPI() & 0x7fff;
		}

		/*输出IO口变化(反复变化的LED控制不记录)*/
		if(sHisOutPorState != (GetGOutPortStateAPI() & EVN_RCD_OUTIO_BIT))
		{
			/*非PGND变化*/
			if((sHisOutPorState & 0x7ffff) != (GetGOutPortStateAPI() & EVN_RCD_OUTIO_BIT & 0x7fff))
			{
				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_OUTPUT, 0, GetGSampOutCurrAPI(), sHisOutPorState, GetGOutPortStateAPI(), GetGWorkModeAPI());

				/*同时PGND变化*/
				if(BitGet(sHisOutPorState, 15) != BitGet(GetGOutPortStateAPI(), 15))
				{
					/*记录PGND变化事件时间*/
					sHisOutClock = GetSysSecondTimeAPI();
				}

				sHisOutPorState = GetGOutPortStateAPI() & EVN_RCD_OUTIO_BIT;
			}
			/*PGND变化至少间隔10min[防止反复记录])*/
			else if(ABS(GetSysSecondTimeAPI(), sHisOutClock) >= 600)
			{
				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_OUTPUT, 0, GetGSampOutCurrAPI(), sHisOutPorState, GetGOutPortStateAPI(), GetGWorkModeAPI());
				sHisOutPorState = GetGOutPortStateAPI() & EVN_RCD_OUTIO_BIT;
				sHisOutClock = GetSysSecondTimeAPI();
			}
		}

		/*充电电流限值变化*/
		if(((0 == sHisChgCurr) && (0 != GetGChgLimCurrAPI()))
			|| ((0 != sHisChgCurr) && (0 == GetGChgLimCurrAPI()))
			|| (ABS(sHisChgCurr, GetGChgLimCurrAPI()) >= 500))
		{
			/*产生事件*/
			EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_CLimChan, GetGSampOutCurrAPI(), sHisChgCurr, GetGChgLimCurrAPI(), GetGCellMaxVoltAPI());
			sHisChgCurr = GetGChgLimCurrAPI();
		}

		/*放电电流限值变化*/
		if(((0 == sHisDhgCurr) && (0 != GetGDhgLimCurrAPI()))
			|| ((0 != sHisDhgCurr) && (0 == GetGDhgLimCurrAPI()))
			|| (ABS(sHisDhgCurr, GetGDhgLimCurrAPI()) >= 500))
		{
			/*产生事件*/
			EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_DLimChan, GetGSampOutCurrAPI(), sHisDhgCurr, GetGDhgLimCurrAPI(), GetGCellMinVoltAPI());
			sHisDhgCurr = GetGDhgLimCurrAPI();
		}

		/*负母线电压采样值变为0*/
		if(((0 == sHisNegVolt) && (0 != GetGPgndNegVoltAPI()))
			|| ((0 != sHisNegVolt) && (0 == GetGPgndNegVoltAPI())))
		{
			/*产生事件*/
			EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_NBusChan, GetGSampOutCurrAPI(), sHisNegVolt, GetGPgndNegVoltAPI(), GetGSampSumVoltAPI());
			sHisNegVolt = GetGPgndNegVoltAPI();
			sHisNegClock = GetSysSecondTimeAPI();
			sHisNegTime = 0;
		}
		/*负母线电压采样值变化50V*/
		else if(ABS(sHisNegVolt, GetGPgndNegVoltAPI()) >= 500)
		{
			/*持续有差值30s(非跳动)*/
			if(sHisNegTime >= 300)
			{
				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_NBusChan, GetGSampOutCurrAPI(), sHisNegVolt, GetGPgndNegVoltAPI(), GetGSampSumVoltAPI());
				sHisNegVolt = GetGPgndNegVoltAPI();
				sHisNegClock = GetSysSecondTimeAPI();
				sHisNegTime = 0;
			}
			else
			{
				sHisNegTime++;
			}
		}
		/*负母线电压采样值变化10V间隔30min*/
		else if((ABS(sHisNegVolt, GetGPgndNegVoltAPI()) >= 100)
			&& (GetSysSecondTimeAPI() >= (sHisNegClock + 1800)))
		{
			/*产生事件*/
			EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_NBusChan, GetGSampOutCurrAPI(), sHisNegVolt, GetGPgndNegVoltAPI(), GetGSampSumVoltAPI());
			sHisNegVolt = GetGPgndNegVoltAPI();
			sHisNegClock = GetSysSecondTimeAPI();
			sHisNegTime = 0;
		}
		/*负母线电压采样值无变化*/
		else
		{
			sHisNegTime = 0;
		}
	}
	/*初始化状态获取初值*/
	else
	{
		sHisChgCurr = GetGChgLimCurrAPI();
		sHisDhgCurr = GetGDhgLimCurrAPI();
		sHisNegVolt = GetGPgndNegVoltAPI();
		sHisNegClock = GetSysSecondTimeAPI();
		sHisNegTime = 0;
	}

	//辅助设备事件记录
	EventRecordAuxDevDeal();
}

/*=================================================================================================
** @Name      : void EventRcdDisplayUserAPI(eEvent_RcdSort sort, u8 id, u16 info2, u16 info3, u16 info4, u16 info5)
** @Input     : sort:事件类型 id:事件码ID info2-5:附加信息2-5
** @Output    : void
** @Function  : 事件记录与显示用户接口
** @The notes : 用户需产生事件的地方调用,事件显示到显示区并记录存储
**===============================================================================================*/
void EventRcdDisplayUserAPI(eEvent_RcdSort sort, u8 id, u16 info2, u16 info3, u16 info4, u16 info5)
{
    t_SysClock sysClock = {0};
    t_RcdCreateInfo rcdInfo = {0};
//  static u16 sHisInfo[6] = {0};
//
//    /*不记录连续重复的事件*/
//    if((sort == sHisInfo[0]) && (id == sHisInfo[1]) /*&& (info2 == sHisInfo[2])*/
//        && (info3 == sHisInfo[3]) && (info4 == sHisInfo[4]) && (info5 == sHisInfo[5]))
//    {
//        return;
//    }
//
//    /*更新上次记录信息*/
//    sHisInfo[0] = sort;
//    sHisInfo[1] = id;
//    sHisInfo[2] = info2;
//    sHisInfo[3] = info3;
//    sHisInfo[4] = info4;
//    sHisInfo[5] = info5;

    /*不记录连续重复的事件*/
    if(TRUE == EventRcdCheckRepeatedState(sort, id, info2, info3, info4, info5))
    {
        return;
    }

    /*正在显示当天事件信息才更新显示内容*/
    if(0 == gGUserCtrl_87[eUserCtrl87_HisEvnSw])
    {
		//记录新历史事件记录
		//记录类型
		gGHisEventInfo_46[0] = sort;

		//输入IO类型
		if(eEVN_RCDSORT_INPUT == sort)
		{
			gGHisEventInfo_46[1] = EVN_RCD_INIOCODE_FST + id;     	//事件记录码
		}
		//输出IO类型
		else if(eEVN_RCDSORT_OUTPUT == sort)
		{
			gGHisEventInfo_46[1] = EVN_RCD_OUTIOCODE_FST + id;     	//事件记录码
		}
		//状态类型
		else if(eEVN_RCDSORT_STATE == sort)
		{
			gGHisEventInfo_46[1] = EVN_RCD_STATECODE_FST + id;     	//事件记录码
		}
		//控制命令类型
		else if(eEVN_RCDSORT_CMD == sort)
		{
			gGHisEventInfo_46[1] = EVN_RCD_CMDCODE_FST + id;    	//事件记录码
		}
		//调试命令类型
		else if(eEVN_RCDSORT_DEBUG == sort)
		{
			gGHisEventInfo_46[1] = EVN_RCD_DBGCODE_FST + id;    	//事件记录码
		}
		//参数配置类型
		else if(eEVN_RCDSORT_CFG == sort)
		{
			gGHisEventInfo_46[1] = EVN_RCD_CFGCODE_FST + id;    	//事件记录码
		}
		//用户自定义类型
		else
		{
			gGHisEventInfo_46[1] = id;                          	//事件记录码
		}

		/*记录时间信息*/
		sysClock = GetSysDateClockAPI();

		gGHisEventInfo_46[2] = sysClock.year;                                 //记录时间-年
		gGHisEventInfo_46[3] = ((u16)sysClock.month << 8) | sysClock.day;     //记录时间-月日
		gGHisEventInfo_46[4] = ((u16)sysClock.hour << 8) | sysClock.minute;   //记录时间-时分
		gGHisEventInfo_46[5] = sysClock.second;                               //记录时间-秒

		//事件附件信息
		gGHisEventInfo_46[6] = ((u16)GetGProtectStateAPI() << 8) | (GetGWorkStateAPI() & 0x00ff);
		gGHisEventInfo_46[7] = info2;
		gGHisEventInfo_46[8] = info3;
		gGHisEventInfo_46[9] = info4;
		gGHisEventInfo_46[10] = info5;

		/*触发发送历史事件信息*/
		SendCanGroupHisEventInfoMsg();
    }

    /*取到相应事件记录附加信息*/
	rcdInfo.type = sort;

    /*附加信息1:当前保护状态和工作状态*/
	rcdInfo.info1 = ((u16)GetGProtectStateAPI() << 8) | (GetGWorkStateAPI() & 0x00ff);

    /*附加信息2-5:用户定义*/
	rcdInfo.info2 = info2;
	rcdInfo.info3 = info3;
	rcdInfo.info4 = info4;
	rcdInfo.info5 = info5;

	//输入IO类型
	if(eEVN_RCDSORT_INPUT == sort)
	{
		rcdInfo.code = EVN_RCD_INIOCODE_FST + id;     	//事件记录码
	}
	//输出IO类型
	else if(eEVN_RCDSORT_OUTPUT == sort)
	{
		rcdInfo.code = EVN_RCD_OUTIOCODE_FST + id;     	//事件记录码
	}
	//状态类型
	else if(eEVN_RCDSORT_STATE == sort)
	{
		rcdInfo.code = EVN_RCD_STATECODE_FST + id;     	//事件记录码
	}
	//控制命令类型
	else if(eEVN_RCDSORT_CMD == sort)
	{
		rcdInfo.code = EVN_RCD_CMDCODE_FST + id;    	//事件记录码
	}
	//调试命令类型
	else if(eEVN_RCDSORT_DEBUG == sort)
	{
		rcdInfo.code = EVN_RCD_DBGCODE_FST + id;    	//事件记录码
	}
	//参数配置类型
	else if(eEVN_RCDSORT_CFG == sort)
	{
		rcdInfo.code = EVN_RCD_CFGCODE_FST + id;    	//事件记录码
	}
	//用户自定义类型
	else
	{
		rcdInfo.code = id;                              //事件记录码
	}

	//事件记录
	RcdCreateEventRcdInfoAPI(&rcdInfo);

	//更新显示事件记录数目
	//ErrEvntRcdUpdateStateAPI();
}

/*=================================================================================================
** @Name      : void EventChangHisRcdDisplayAPI(u8 date, u8 home)
** @Input     : date:历史事件日期[0,30]:0-30天前记录 home:读首页标志 0:读当前记录 1:读当日首页记录
** @Output    : void
** @Function  : 切换历史事件显示信息
** @The notes : 按照日期读取指定时间的历史事件显示(读30条)
**===============================================================================================*/
void EventChangHisRcdDisplayAPI(u8 date, u8 home)
{
    static u8 sHisDate = 0;
    t_RcdStoreInfo info = {0};

    /*不切换日期且都首页记录则跳转切换首页*/
    if((sHisDate == date) && (1 == home))
    {
    	/*切换首页失败*/
    	if(FALSE == RcdJumpDispHomeEventRcdUserAPI())
    	{
    		/*不更新显示*/
    		return;
    	}
    }

	/*读取一条记录成功*/
	if(TRUE == RcdDisplayOneEventRcdUserAPI(date, &info))
	{
		//事件主要信息
		gGHisEventInfo_46[0] = info.codeH & 0x00ff;
		gGHisEventInfo_46[1] = info.codeL;
		gGHisEventInfo_46[2] = info.year;
		gGHisEventInfo_46[3] = info.MD;
		gGHisEventInfo_46[4] = info.HM;
		gGHisEventInfo_46[5] = info.sec;

		//事件附件信息
		gGHisEventInfo_46[6] = info.info1;
		gGHisEventInfo_46[7] = info.info2;
		gGHisEventInfo_46[8] = info.info3;
		gGHisEventInfo_46[9] = info.info4;
		gGHisEventInfo_46[10] = info.info5;
	}
	/*读取已失败*/
	else
	{
		/*切换日期未读取到记录*/
		if(sHisDate != date)
		{
			/*事件记录查询显示区清零*/
			gGHisEventInfo_46[0] = 0;
			gGHisEventInfo_46[1] = 0;
			gGHisEventInfo_46[2] = 0;
			gGHisEventInfo_46[3] = 0;
			gGHisEventInfo_46[4] = 0;
			gGHisEventInfo_46[5] = 0;

			gGHisEventInfo_46[6] = 0;
			gGHisEventInfo_46[7] = 0;
			gGHisEventInfo_46[8] = 0;
			gGHisEventInfo_46[9] = 0;
			gGHisEventInfo_46[10] = 0;
		}
	}
    sHisDate = date;

	/*触发发送历史事件查询显示信息*/
	SendCanGroupHisEventInfoMsg();
}

/*=================================================================================================
** @Name      : void EventClrHisRcdDisplayAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除换历史事件显示信息
** @The notes : 显示区域清零
**===============================================================================================*/
void EventClrHisRcdDisplayAPI(void)
{
	/*事件记录查询显示区清零*/
	gGHisEventInfo_46[0] = 0;
	gGHisEventInfo_46[1] = 0;
	gGHisEventInfo_46[2] = 0;
	gGHisEventInfo_46[3] = 0;
	gGHisEventInfo_46[4] = 0;
	gGHisEventInfo_46[5] = 0;

	gGHisEventInfo_46[6] = 0;
	gGHisEventInfo_46[7] = 0;
	gGHisEventInfo_46[8] = 0;
	gGHisEventInfo_46[9] = 0;
	gGHisEventInfo_46[10] = 0;

	/*触发发送历史事件查询显示信息*/
	SendCanGroupHisEventInfoMsg();
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void EventRecordAuxDevDeal(void)
** @Input     : void
** @Output    : void
** @Function  : 事件记录辅助设备处理任务(定周期检查辅助设备变化事件)
** @The notes : 100ms周期
**===============================================================================================*/
static void EventRecordAuxDevDeal(void)
{
	u16 info3 = 0;
	u16 info4 = 0;
	u16 info5 = 0;
	static u8 sHisTempState = 0;//按位表示历史值是否变化到异常值 bit0:空调温度 bit1:柜内温度 bit2:柜外温度 bit3:柜内湿度 bit4:气体1浓度 bit5:气体2浓度
	static u8 sHisDoorState = 0;//门禁状态 0:关闭 1:开启
	static u8 sHisAirTState = 0;//空调控制状态 0:关闭制冷制热 1:开启制冷 2:开启制热
	static u8 sHisAirHState = 0;//空调除湿状态 0:关闭 1:开启除湿
	#if(1 == BMU_EU_SHUTFAN_EN) //使能控制电池柜百叶风机
	static u8 sHisShutState = 0;//电池柜百叶状态 0:无效 1:开始开启百叶 2:完全开启百叶 3:开始关闭百叶 4:完全关闭百叶
	static u8 sHisFanState = 0;//电池柜风机状态 0:关闭 1:开启
	#endif
	static u16 sHisAirInfo = 0;

	/*非上电初始化状态*/
	if((eWORK_INIT != GetGWorkStateAPI())
		&& (BSPGetOSMillSecTimeAPI() >= 10000))
	{
		//记录温度
		info4 = ((u16)GetGPerInsidMaxTempAPI() << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT);

		//记录湿度
		info5 = ((u16)GetGPerInsidMaxHumAPI() << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH);

		/*门禁状态变化*/
		if(sHisDoorState != BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_Door))
		{
			/*产生事件*/
			EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_DoorDI, GetGSampOutCurrAPI(), BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_Door), info4, info5);
			//记录门禁状态
			sHisDoorState = BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_Door);
		}

		/*IMEU2和空调通讯均正常*/
		if((0 == DiagGetPerImeuOffCommuStateAPI())
			&& (0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_AirCom)))
		{
			//空调达到制冷温度
			if(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirConT) + GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirCoffT)))
			{
				//首次开启空调制冷
				if(1 != sHisAirTState)
				{
					//正在制热
					if(2 == sHisAirTState)
					{
						/*产生事件*/
						EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirHeat, GetGSampOutCurrAPI(), 0, info4, info5);
					}

					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirCold, GetGSampOutCurrAPI(), 1, info4, info5);

					//记录已开启制冷
					sHisAirTState = 1;
				}
			}
			//空调达到制热温度
			else if(((0 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHonT)))
				|| ((1 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHonT) - GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHoffT)))))
			{
				//正在制冷
				if(1 == sHisAirTState)
				{
					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirCold, GetGSampOutCurrAPI(), 0, info4, info5);
				}

				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirHeat, GetGSampOutCurrAPI(), 2, info4, info5);

				//开启制热
				sHisAirTState = 2;
			}
			//达到停止温度
			else if((((0 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirConT)))
				|| ((1 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirConT) - GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirCoffT)))))
				&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHonT) + GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHoffT))))
			{
				//正在制冷
				if(1 == sHisAirTState)
				{
					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirCold, GetGSampOutCurrAPI(), 0, info4, info5);
				}
				//正在制热
				else if(2 == sHisAirTState)
				{
					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirHeat, GetGSampOutCurrAPI(), 0, info4, info5);
				}
				//停止空调
				sHisAirTState = 0;
			}

			//空调达到除湿湿度
			if(((0 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) >= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDonH)))
				|| ((1 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDonH) + GetGPerImeu2TempInfoAPI(eIMEU2Info205_DehumDiff)))))
			{
				//未开启
				if(0 == sHisAirHState)
				{
					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirDehum, GetGSampOutCurrAPI(), 1, info4, info5);
				}
				//开启除湿
				sHisAirHState = 1;
			}
			//达到停止湿度
			else if(((0 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) <= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDoffH)))
				|| ((1 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) <= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDonH) - GetGPerImeu2TempInfoAPI(eIMEU2Info205_DehumDiff)))))
			{
				//未关闭
				if(1 == sHisAirHState)
				{
					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirDehum, GetGSampOutCurrAPI(), 0, info4, info5);
				}
				//停止除湿
				sHisAirHState = 0;
			}

			//记录空调告警
			if(1 == TacGetAuxDevAirTypeAPI())//同飞空调
			{
				info4 = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirAlarm) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirState);
			}
			else
			{
				info4 = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirAlarm) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState);
			}

			/*空调信息变化*/
			if(info4 != sHisAirInfo)
			{
				//记录空调温湿度
				info5 = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH);

				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirInfo, GetGSampOutCurrAPI(), sHisAirInfo, info4, info5);
				//记录空调信息
				sHisAirInfo = info4;
			}

			//检测柜内温度是否达到开启空调(制冷/制热)温度即记录变化事件
			if((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (s8)gGHardPara_104[eGHardPara104_AirColdOn_ET])
				|| (GetGPerInsidMaxTempAPI() >= (s8)gGHardPara_104[eGHardPara104_AirColdOn_ET])
				|| (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET])
				|| (GetGPerInsidMinTempAPI() <= (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]))
			{
				//未记录
				if(0 == BitGet(sHisTempState, 0))
				{
					//记录温度
					info3 = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2);
					info4 = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT3) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT);
					info5 = ((u16)GetGEnvirTempAPI() << 8) | GetGShuntSampTempAPI();

					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_IntTChan, GetGSampOutCurrAPI(), info3, info4, info5);
					BitSet(sHisTempState, 0);
				}
			}
			//柜内温度均降至空调停止温度
			else if((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET])
				&& (GetGPerInsidMaxTempAPI() <= (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET])
				&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET])
				&& (GetGPerInsidMinTempAPI() >= (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]))
			{
				BitClr(sHisTempState, 0);
			}

			//检测柜内温度是否达到过温温度即记录变化事件
			if((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= gGBmuGenPara_102[eBmuGenPara102_Envir1Lim])
				|| (GetGPerInsidMaxTempAPI() >= gGBmuGenPara_102[eBmuGenPara102_Envir1Lim]))
			{
				//记录温度
				info3 = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2);
				info4 = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT3) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT);
				info5 = ((u16)GetGEnvirTempAPI() << 8) | GetGShuntSampTempAPI();

				//未记录
				if(0 == BitGet(sHisTempState, 1))
				{
					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_IntTChan, GetGSampOutCurrAPI(), info3, info4, info5);
					BitSet(sHisTempState, 1);
				}
			}
			else if((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= ((s8)gGBmuGenPara_102[eBmuGenPara102_Envir1Lim] - gGBmuGenPara_102[eBmuGenPara102_Envir1Clr]))
				&& (GetGPerInsidMaxTempAPI() <= ((s8)gGBmuGenPara_102[eBmuGenPara102_Envir1Lim] - gGBmuGenPara_102[eBmuGenPara102_Envir1Clr])))
			{
				BitClr(sHisTempState, 1);
			}

			//检测柜外温度是否达到过温温度即记录变化事件
			if(GetGPerEnvirMaxTempAPI() >= gGBmuGenPara_102[eBmuGenPara102_Envir1Lim])
			{
				//记录温度
				info3 = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_TP10001) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_TP10002);
				info4 = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_TP10003) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_TP10004);
				info5 = ((u16)GetGPerInsidMaxTempAPI() << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT);

				//未记录
				if(0 == BitGet(sHisTempState, 2))
				{
					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_ExtTChan, GetGSampOutCurrAPI(), info3, info4, info5);
					BitSet(sHisTempState, 2);
				}
			}
			else if(GetGPerInsidMaxTempAPI() <= ((s8)gGBmuGenPara_102[eBmuGenPara102_Envir1Lim] - gGBmuGenPara_102[eBmuGenPara102_Envir1Clr]))
			{
				BitClr(sHisTempState, 2);
			}

			//检测柜内温度是否达到开启除湿湿度即记录变化事件
			if((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) >= gGHardPara_104[eGHardPara104_AirDeHumOn_H])
				|| (GetGPerInsidMaxHumAPI() >= gGHardPara_104[eGHardPara104_AirDeHumOn_H]))
			{
				//记录温度
				info3 = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH2);
				info4 = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH3) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH);
				info5 = ((u16)GetGPerInsidMaxTempAPI() << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT);

				//未记录
				if(0 == BitGet(sHisTempState, 3))
				{
					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_IntHChan, GetGSampOutCurrAPI(), info3, info4, info5);
					BitSet(sHisTempState, 3);
				}
			}
			else if((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) <= (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]))
				&& (GetGPerInsidMaxHumAPI() <= (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H])))
			{
				BitClr(sHisTempState, 3);
			}
		}

		#if(1 == BMU_EU_SHUTFAN_EN)
		//仅开报警(关到位)
		if((0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_ShutOff))
			&& (1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_ShutOn)))
		{
			//非到位
			if(4 != sHisShutState)
			{
				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_ShutBack, GetGSampOutCurrAPI(), 4, info4, info5);
				//完全关闭
				sHisShutState = 4;
			}
		}
		//仅关报警(开到位)
		else if((1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_ShutOff))
			&& (0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_ShutOn)))
		{
			//非到位
			if(2 != sHisShutState)
			{
				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_ShutBack, GetGSampOutCurrAPI(), 2, info4, info5);
				//完全开启
				sHisShutState = 2;
			}
		}
		//两个都报警(正在开或关)
		else
		{
			//关到位变为非到位(正在开)
			if(4 == sHisShutState)
			{
				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_ShutBack, GetGSampOutCurrAPI(), 1, info4, info5);
				sHisShutState = 1;
			}
			//开到位变为非到位(正在关)
			else if(2 == sHisShutState)
			{
				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_ShutBack, GetGSampOutCurrAPI(), 3, info4, info5);
				sHisShutState = 3;
			}
			else
			{
				//保持原来状态
				//sHisShutState = sHisShutState;
			}
		}

		//开电池柜风机
		if(GetGPerImeu2SignInfoAPI(eIMEU2Info204_FanSpeed) >= 10)
		{
			//未记录
			if(1 != sHisFanState)
			{
				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_ShutBack, GetGSampOutCurrAPI(), 5, GetGPerImeu2SignInfoAPI(eIMEU2Info204_FanSpeed), info5);
				sHisFanState = 1;
			}
		}
		//关电池柜风机
		else
		{
			//未记录
			if(0 != sHisFanState)
			{
				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_ShutBack, GetGSampOutCurrAPI(), 6, GetGPerImeu2SignInfoAPI(eIMEU2Info204_FanSpeed), info5);
				sHisFanState = 0;
			}
		}
		#endif

		if(0 == BitGet(TacGetAuxComErrFlagAPI(), eAuxComErr_Gas1))/*气体传感器1通讯正常*/
		{
			//检测柜内可燃气体1是否达到告警浓度即记录变化事件
			if((GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1) >= BMU_EU_GAS1_CONC) && (DiagGetPerGasErrFlagAPI() > 0))
			{
				//记录温度
				info3 = GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1);
				info4 = GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2);
				info5 = GetGGasConIoExpStateAPI();

				//未记录
				if(0 == BitGet(sHisTempState, 4))
				{
					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_GasChan, GetGSampOutCurrAPI(), info3, info4, info5);
					BitSet(sHisTempState, 4);
				}
			}
			else if(GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1) <= (BMU_EU_GAS1_CONC / 2))
			{
				BitClr(sHisTempState, 4);
			}
		}

		if(0 == BitGet(TacGetAuxComErrFlagAPI(), eAuxComErr_Gas2))/*气体传感器2通讯正常*/
		{
			//检测柜内可燃气体2是否达到告警浓度即记录变化事件
			if((GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2) >= BMU_EU_GAS2_CONC) && (DiagGetPerGasErrFlagAPI() > 0))
			{
				//记录温度
				info3 = GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1);
				info4 = GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2);
				info5 = GetGGasConIoExpStateAPI();

				//未记录
				if(0 == BitGet(sHisTempState, 5))
				{
					/*产生事件*/
					EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_GasChan, GetGSampOutCurrAPI(), info3, info4, info5);
					BitSet(sHisTempState, 5);
				}
			}
			else if(GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2) <= (BMU_EU_GAS2_CONC / 2))
			{
				BitClr(sHisTempState, 5);
			}
		}
	}
	/*初始化状态获取初值*/
	else
	{
		/*IMEU2和空调通讯均正常*/
		if((0 == DiagGetPerImeuOffCommuStateAPI())
			&& (0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_AirCom)))
		{
			//空调达到制冷温度
			if(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirConT) + GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirCoffT)))
			{
				//开启制冷
				sHisAirTState = 1;
			}
			//空调达到制热温度
			else if(((0 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHonT)))
				|| ((1 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHonT) - GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHoffT)))))
			{
				//开启制热
				sHisAirTState = 2;
			}
			//达到停止温度
			else if((((0 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirConT)))
				|| ((1 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirConT) - GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirCoffT)))))
				&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHonT) + GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHoffT))))
			{
				//停止空调
				sHisAirTState = 0;
			}

			//空调达到除湿湿度
			if(((0 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) >= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDonH)))
				|| ((1 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDonH) + GetGPerImeu2TempInfoAPI(eIMEU2Info205_DehumDiff)))))
			{
				//开启除湿
				sHisAirHState = 1;
			}
			//达到停止温度
			else if(((0 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) <= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDoffH)))
				|| ((1 == TacGetAuxDevAirTypeAPI()) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) <= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDonH) - GetGPerImeu2TempInfoAPI(eIMEU2Info205_DehumDiff)))))
			{
				//停止除湿
				sHisAirHState = 0;
			}
		}

		#if(1 == BMU_EU_SHUTFAN_EN)
		//仅开报警
		if((0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_ShutOff))
			&& (1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_ShutOn)))
		{
			//完全关闭
			sHisShutState = 4;
		}
		//仅关报警
		else if((1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_ShutOff))
			&& (0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_ShutOn)))
		{
			//完全开启
			sHisShutState = 2;
		}
		//两个都报警(正在开或关)
		else
		{
			//关到位变为非到位(正在开)
			if(4 == sHisShutState)
			{
				sHisShutState = 1;
			}
			//开到位变为非到位(正在关)
			else if(2 == sHisShutState)
			{
				sHisShutState = 3;
			}
			else
			{
				//保持原来状态
				//sHisShutState = sHisShutState;
			}
		}

		//开电池柜风机
		if(GetGPerImeu2SignInfoAPI(eIMEU2Info204_FanSpeed) >= 10)
		{
			sHisFanState = 1;
		}
		else
		{
			sHisFanState = 0;
		}
		#endif

		//空调信息
		if(1 == TacGetAuxDevAirTypeAPI())//同飞空调
		{
			sHisAirInfo = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirAlarm) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirState);
		}
		else
		{
			sHisAirInfo = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirAlarm) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState);
		}

		//温湿度浓度历史状态
		sHisTempState = 0;
	}
}

/*=================================================================================================
** @Name      : static u8 EventRcdCheckRepeatedState(eEvent_RcdSort sort, u8 id, u16 info2, u16 info3, u16 info4, u16 info5)
** @Input     : sort:事件类型 id:事件码ID info2-5:附加信息2-5
** @Output    : TRUE:重复事件(不需记录) FALSE:非重复事件(需要记录)
** @Function  : 判断事件记录是否重复
** @The notes : 跟上次比较重复则不再记录
**===============================================================================================*/
static u8 EventRcdCheckRepeatedState(eEvent_RcdSort sort, u8 id, u16 info2, u16 info3, u16 info4, u16 info5)
{
    static u16 sHisAuth[3] = {0};
    static u16 sHisStat[3] = {0};
    static u16 sHisMode[3] = {0};
    static u16 sHisClr[3] = {0};
    static u16 sHisShut[3] = {0};
    static u16 sHisSDbg[3] = {0};
    static u16 sHisMDbg[3] = {0};
    static u16 sHisPara[5] = {0};
    static u16 sHisInfo[6] = {0};

    /*外部触发的事件(控制)单独处理*/
    if(eEVN_RCDSORT_CMD == sort)
    {
    	//权限切换控制
    	if(0 == id)
    	{
    		//信息完全一样不记录
    		if((info3 == sHisAuth[0]) && (info4 == sHisAuth[1]) && (info5 == sHisAuth[2]))
    		{
    			return(TRUE);
    		}

    		sHisAuth[0] = info3;
    		sHisAuth[1] = info4;
    		sHisAuth[2] = info5;
    	}
    	//工作状态切换控制
    	else if(1 == id)
    	{
    		//信息完全一样不记录
    		if((info3 == sHisStat[0]) && (info4 == sHisStat[1]) && (info5 == sHisStat[2]))
    		{
    			return(TRUE);
    		}

    		//空闲状态非启动命令不记录
    		if((eWORK_IDLE == GetGWorkStateAPI()) && (eCTRL_PRECHG != info4) && (eCTRL_NORSTART != info4)
    			&& (eCTRL_CHGSTART != info4) && (eCTRL_DHGSTART != info4))
    		{
    			return(TRUE);
    		}
    		//运行状态非停机命令不记录
    		else if((eWORK_RUN == GetGWorkStateAPI()) && (eCTRL_NORSTOP != info4) && (eCTRL_FORCSTOP != info4)
    			&& (eCTRL_EMERSTOP != info4) && (eCTRL_POWEROFF != info4))
    		{
    			return(TRUE);
    		}
    		//故障状态启动命令不记录
    		else if((eWORK_ERR == GetGWorkStateAPI()) && ((eCTRL_PRECHG == info4) || (eCTRL_NORSTART == info4)
        		|| (eCTRL_CHGSTART == info4) || (eCTRL_DHGSTART == info4)))
    		{
    			return(TRUE);
    		}
    		//故障状态接触器全断开时停止命令不记录
    		else if((eWORK_ERR == GetGWorkStateAPI()) && (TRUE == TacGetGroupRunSwhAllOffStateAPI()) && ((eCTRL_NORSTOP == info4)
    			|| (eCTRL_FORCSTOP == info4) || (eCTRL_EMERSTOP == info4) || (eCTRL_POWEROFF == info4)))
    		{
    			return(TRUE);
    		}

    		sHisStat[0] = info3;
    		sHisStat[1] = info4;
    		sHisStat[2] = info5;
    	}
    	//工作模式切换控制
    	else if(2 == id)
    	{
    		//信息完全一样不记录
    		if((info3 == sHisMode[0]) && (info4 == sHisMode[1]) && (info5 == sHisMode[2]))
    		{
    			return(TRUE);
    		}

    		sHisMode[0] = info3;
    		sHisMode[1] = info4;
    		sHisMode[2] = info5;
    	}
    	//系统清除命令控制
    	else if(3 == id)
    	{
    		//信息完全一样不记录
    		if((info3 == sHisClr[0]) && (info4 == sHisClr[1]) && (info5 == sHisClr[2]))
    		{
    			return(TRUE);
    		}

    		sHisClr[0] = info3;
    		sHisClr[1] = info4;
    		sHisClr[2] = info5;
    	}
    	//百叶控制
    	else if(12 == id)
    	{
    		//信息完全一样不记录
    		if((info3 == sHisShut[0]) && (info4 == sHisShut[1]) && (info5 == sHisShut[2]))
    		{
    			return(TRUE);
    		}

    		sHisShut[0] = info3;
    		sHisShut[1] = info4;
    		sHisShut[2] = info5;
    	}
    	//其它控制
    	else
    	{
    		//不记录
    		//return(TRUE);

            /*所有信息均一样才不记录(类型/序号/电流/其它信息)*/
            if((sort == sHisInfo[0]) && (id == sHisInfo[1]) /*&& (info2 == sHisInfo[2])*/
                && (info3 == sHisInfo[3]) && (info4 == sHisInfo[4]) && (info5 == sHisInfo[5]))
            {
                return(TRUE);
            }

    		/*更新上次记录信息*/
    		sHisInfo[0] = sort;
    		sHisInfo[1] = id;
    		sHisInfo[2] = info2;
    		sHisInfo[3] = info3;
    		sHisInfo[4] = info4;
    		sHisInfo[5] = info5;
    	}
    }
    /*外部触发的事件(调试)单独处理*/
    else if(eEVN_RCDSORT_DEBUG == sort)
    {
    	//从控调试
    	if(0 == id)
    	{
    		//信息完全一样不记录
    		if((info3 == sHisSDbg[0]) && (info4 == sHisSDbg[1]) && (info5 == sHisSDbg[2]))
    		{
    			return(TRUE);
    		}

    		sHisSDbg[0] = info3;
    		sHisSDbg[1] = info4;
    		sHisSDbg[2] = info5;
    	}
    	//主控调试
    	else if(1 == id)
    	{
    		//信息完全一样不记录
    		if((info3 == sHisMDbg[0]) && (info4 == sHisMDbg[1]) && (info5 == sHisMDbg[2]))
    		{
    			return(TRUE);
    		}

    		sHisMDbg[0] = info3;
    		sHisMDbg[1] = info4;
    		sHisMDbg[2] = info5;
    	}
    	//其它调试
    	else
    	{
    		//不记录
    		//return(TRUE);

            /*所有信息均一样才不记录(类型/序号/电流/其它信息)*/
            if((sort == sHisInfo[0]) && (id == sHisInfo[1]) /*&& (info2 == sHisInfo[2])*/
                && (info3 == sHisInfo[3]) && (info4 == sHisInfo[4]) && (info5 == sHisInfo[5]))
            {
                return(TRUE);
            }

    		/*更新上次记录信息*/
    		sHisInfo[0] = sort;
    		sHisInfo[1] = id;
    		sHisInfo[2] = info2;
    		sHisInfo[3] = info3;
    		sHisInfo[4] = info4;
    		sHisInfo[5] = info5;
        }
    }
    /*外部触发的事件(参数配置)单独处理*/
    else if(eEVN_RCDSORT_CFG == sort)
    {
		//信息完全一样不记录
		if((id == sHisPara[0]) && (info3 == sHisPara[1]) && (info4 == sHisPara[2]) && (info5 == sHisPara[3]))
		{
			return(TRUE);
		}

		sHisPara[0] = id;
		sHisPara[1] = info3;
		sHisPara[2] = info4;
		sHisPara[3] = info5;
    }
    /*其它事件统一处理*/
    else
    {
        /*所有信息均一样才不记录(类型/序号/电流/其它信息)*/
        if((sort == sHisInfo[0]) && (id == sHisInfo[1]) /*&& (info2 == sHisInfo[2])*/
            && (info3 == sHisInfo[3]) && (info4 == sHisInfo[4]) && (info5 == sHisInfo[5]))
        {
            return(TRUE);
        }

		/*更新上次记录信息*/
		sHisInfo[0] = sort;
		sHisInfo[1] = id;
		sHisInfo[2] = info2;
		sHisInfo[3] = info3;
		sHisInfo[4] = info4;
		sHisInfo[5] = info5;
    }

    return(FALSE);
}
