/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileUserAPI.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控用户接口任务模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "osif.h"
#include "PilePara.h"
#include "PileInfo.h"
#include "PublicPara.h"
#include "PublicInfo.h"
#include "PileParaRd.h"
#include "PileGroupRcv.h"
#include "PileDataCalc.h"
#include "PileEnerStat.h"
#include "PileLimCalc.h"
#include "PileDiagCheck.h"
#include "PileDiagMaker.h"
#include "PileDiagUser.h"
#include "PileWorkState.h"
#include "PileSysState.h"
#include "PileAutoCtrl.h"
#include "PileProtect.h"
#include "PileSysMode.h"
#include "PileSysDebug.h"
#include "PileTactUser.h"
#include "PileTactInterface.h"
#include "PileUpMsg.h"
#include "PileUserAPI.h"

#include "GroupPara.h"
#include "PortCheck.h"
#include "Authority.h"
#include "TacUser.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sPileSysRunFlag = 0;          //扩展堆控系统运行标志 0:未启用 1:运行为并机主控
static u8 sPileButStopReq = 0;          /*用户按钮请求停机*/

extern u16 gSysGenParaRO_100[];			/*系统通用参数设置*/
extern u16 gPSysCmdCtrl_86[];			/*系统控制命令信息数组*/

extern u16 gPSmuGenParaR_102[];  		/*堆控通用参数值*/
extern u16 gGBmuGenPara_102[];  		/*主控通用参数值*/

extern u16 gGAuthorCtrl_85[];			/*权限密码登录信息数组*/
extern u16 gGSysCmdCtrl_86[];	    	/*系统控制命令信息数组*/
extern u16 gGUserCtrl_87[];				/*用户控制命令信息数组*/
extern u16 gGDebugCtrl_88[];	    	/*系统调试命令信息数组*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void PileMainSysSelectTask(void)
** @Input     : void
** @Output    : void
** @Function  : 堆控决策是否启用为堆控并机功能
** @The notes : 10ms调用
**===============================================================================================*/
static void PileMainSysSelectTask(void);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void PileAllUserInit(void)
** @Input     : void
** @Output    : void
** @Function  : 堆控所有任务初始化
** @The notes : 用户初始化调用
**===============================================================================================*/
void PileAllUserInit(void)
{
	//读EEP参数初始化
	ParaReadPileGenParaInit();

	//堆控状态信息初始化
	PileSysStateInit();

	//堆控模式信息初始化
	PileWorkModeInfoInit();

	//堆控系统故障保护初始化
	PileSysProCtrlInit();
}

/*=================================================================================================
** @Name      : void PileAllTimer10msTask(void *p)
** @Input     : *p:输入参数需要
** @Output    : void
** @Function  : 堆控总定时10ms周期任务
** @The notes : 创建10ms任务
**===============================================================================================*/
void PileAllTimer10msTask(void *p)
{
	static u8 sTime1s = 0;
	static u8 sTime100ms = 0;

	for(;;)
	{
		/*决策出堆控主机*/
		PileMainSysSelectTask();

		/*启用为堆控系统才执行任务*/
		if(1 == sPileSysRunFlag)
		{
			switch(sTime100ms)
			{
				/*50ms任务*/
				case 0:
				case 5:
					/*CAN报文发送任务*/
					PileSendUpMsg1sBy50ms_API();
					break;

				/*100ms任务*/
				case 1:
					/*计算电池堆运行主要数据任务*/
					PileMainDataCalcTask();
					/*计算电池堆电流功率限值任务*/
					PileSysCurrLimCalcTask();
					break;

				/*100ms任务*/
				case 6:
					/*电池堆保护状态检测任务*/
					PileProStateCheckTask();
					/*电池堆充放电状态检测任务*/
					PileChgStateCheckTask();
					break;

				/*100ms任务*/
				case 2:
					/*电池组总状态检测任务*/
					PileAllGStateCheckTask();
					/*计算电池堆电压限值任务*/
					PileSysVoltLimCalcTask();
					break;

				/*100ms任务*/
				case 3:
					/*电池堆告警码检测任务*/
					PileWarCodeMakerTask();
					/*电池堆故障码检测任务*/
					PileErrCodeMakerTask();
					break;

				/*100ms任务*/
				case 4:
					/*电池组异常检测任务*/
					PileGroupExpDiagTask();
					#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
					/*电池堆故障告警统计任务*/
					PileErrStatistTask();
					#endif
					break;

				/*100ms任务*/
				case 7:
					/*电池堆故障保护任务*/
					PileSysProCtrlTask();
					/*电池堆动态投切任务*/
					PileAutoCtrlGroupTask();
					break;

				/*100ms任务*/
				case 8:
					/*电池堆工作状态检测任务*/
					PileWorkStateCheckTask();
					/*电池堆工作模式检测任务*/
					PileWorkModeCheckTask();
					break;

				/*100ms任务(驱动1s任务)*/
				case 9:
					if((0 == sTime1s) || (5 == sTime1s))
					{
						/*计算电池堆特征信息任务*/
						PileCharaInfoCalcTask();
					}
					if((2 == sTime1s) || (8 == sTime1s))
					{
						/**/
						;
					}
					else if(1 == sTime1s)
					{
						/*CAN通信状态判断任务*/
						PileAllCommuExpDiagTask();
					}
					else if(3 == sTime1s)
					{
						/*电池堆主控权限控制任务*/
						PileGOperAuthCtrlTask();
					}
					else if(4 == sTime1s)
					{
						/*电池堆按钮启停控制任务*/
						PileSysIoUpDnCtrlTask();
					}
					else if(6 == sTime1s)
					{
						/*电池堆下电关机保护控制任务*/
						PileSysPwrOffProTask();
					}
					else if(7 == sTime1s)
					{
						/*计算电池堆电能量和时间信息任务*/
						PileCapEnerTimeCalcTask();
					}
					else if(9 == sTime1s)
					{
						#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
						/*计算电池堆累计电能量信息任务*/
						PileHisCapEnerCalcTask();
						#endif
					}

					/*100ms计时10次转化为1s周期*/
					sTime1s++;
					if(sTime1s >= 10)
					{
						sTime1s = 0;
					}
					break;

				default:
					sTime100ms = 9;
					break;
			}

			/*电池堆启动控制任务*/
			PileWorkStartCheckTask();

			/*10ms计时10次转化为100ms周期*/
			sTime100ms++;
			if(sTime100ms >= 10)
			{
				sTime100ms = 0;
			}
		}
		/*不做主机堆控系统*/
		else
		{
			sTime1s = 0;
			sTime100ms = 0;
		}

		/*10ms定时*/
		OSIF_TimeDelay(10);
	}
}

/*=================================================================================================
** @Name      : void PileSysIoUpDnCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池堆系统按钮启停控制任务
** @The notes : 1s周期
**===============================================================================================*/
void PileSysIoUpDnCtrlTask(void)
{
    u8 state = 0;
    static u8 sTimer = 0;
    static u8 sOffFlag = 0;
    static u8 sHisState = 0;

    //获取急停干接点回检状态
    state = GetGEmergIoButtonStateAPI();

    if(1 == state)
    {
        sTimer = 0;
        sOffFlag = 0;
        sHisState = 0;

        //设置紧急停机命令
        sPileButStopReq = 1;
        gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_EMERSTOP;
    	RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
        //return;
    }

    //获取按钮干接点回检状态
    state = GetGPwrIoButtonStateAPI();

    //开机3s后松开按钮
    if((0 == state)
        && (1 == sHisState)
        && (BSPGetOSMillSecTimeAPI() >= 3000))
    {
        //记录按钮松开标志
        sOffFlag = 1;
    }
    //开机5s后按钮为松开状态
    else if((0 == state)
        && (BSPGetOSMillSecTimeAPI() >= 5000))
    {
        //当前为关机状态
        if(eWORK_OFF == GetPileWorkStateAPI())
        {
            //清除按钮松开标志
            sOffFlag = 0;
        }
        //当前非关机状态
        else
        {
            //记录按钮松开标志
            sOffFlag = 1;
        }
    }

    //按钮已松开过
    if(1 == sOffFlag)
    {
        //再次按下按钮
        if(1 == state)
        {
            //连续按下4s,切换系统为关机
            if(sTimer >= 4)
            {
                //设置停机请求
                sPileButStopReq = 1;

                //非充放电
                //if((eCURR_IDLE == GetPileChgStateAPI()))
                if((GetPileOutSumCurrAPI() >= -30) && (GetPileOutSumCurrAPI() <= 40))
                {
                    //设置关机命令
                    gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWEROFF;
                	RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
                }
            }
            //连续按下2s,切换系统为停机
            else if(sTimer >= 2)
            {
                //设置停机请求
                sPileButStopReq = 1;

                //非充放电
                //if((eCURR_IDLE == GetPileChgStateAPI()))
                if((GetPileOutSumCurrAPI() >= -20) && (GetPileOutSumCurrAPI() <= 30))
                {
                    //设置正常停机命令
                    gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTOP;
                	RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
                    sTimer++;
                }
            }
            else
            {
                sTimer++;
            }
        }
        //松开按钮
        else
        {
            sTimer = 0;

            //清除停机请求
            sPileButStopReq = 0;
        }
    }
    //开机未松开过按钮
    else
    {
        //按钮按着
        if(1 == state)
        {
            //连续按下30s,系统充放电保护或故障保护则进入紧急模式再启动
            if(sTimer >= 30)
            {
                //系统充放电保护或故障保护
                if((ePRO_SYSPRO == GetPileProStateAPI())
                    || (eWORK_ERR == GetPileWorkStateAPI()))
                {
                    //切换紧急模式命令
                    gPSysCmdCtrl_86[eSysCmdCtrl86_Mode] = eMODE_EMERG;
                    RcvPileWorkModeCmdAPI(eCMDPORT_INTE);
                }
                //系统待机状态
                else if(eWORK_IDLE == GetGWorkStateAPI())
                {
                    //设置正常启动命令
                    gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTART;
                	RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
                }
            }
            //连续按下3s,切换系统为启动
            else if(sTimer >= 3)
            {
                //设置正常启动命令
                gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTART;
            	RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
                sTimer++;
            }
            //连续按下1s,切换系统为开机
            else if(sTimer >= 1)
            {
                //设置开机命令
                gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWERON;
            	RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
                sTimer++;
            }
            else
            {
                sTimer++;
            }
        }
        //松开按钮
        else
        {
            sTimer = 0;
        }

        //清除停机请求
        sPileButStopReq = 0;
    }

    //记录当前IO状态
    sHisState = state;
}

/*=================================================================================================
** @Name      : void PileSysPwrOffProTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池堆系统关机下电保护任务
** @The notes : 1s周期
**===============================================================================================*/
void PileSysPwrOffProTask(void)
{
	//自供电(按钮或开关)时才进行下电保护
	#if((0 == BMU_DC_PWR_TYPE) || (1 == BMU_DC_PWR_TYPE))
	{
		u8 vlstate = 0;
		u8 lkstate = 0;
		static u8 sHisState = 0;
		static u32 sHL3Timer = 0;
		static u32 sHL4Timer = 0;
		static u32 sErrTimer = 0;
		static u32 sCanTimer = 0;

		//开机或刚退出关机不需要保护
		if((eWORK_INIT == GetPileWorkStateAPI())
			|| ((eWORK_OFF == sHisState)
			&& (eWORK_OFF != GetPileWorkStateAPI())))
		{
			sHL3Timer = 0;
			sHL4Timer = 0;
			sErrTimer = 0;
			sCanTimer = 0;
		}
		sHisState = GetGWorkStateAPI();

		vlstate = GetPAllGroupCellVLStateAPI();
		lkstate = GetPAllGroupLeakStateAPI();

		//欠压2级
		if((2 == vlstate)                 	                                        //单体欠压2级
			&& (GetGCellMinVoltAPI() <= gGBmuGenPara_102[eBmuGenPara102_VL2Lim])    //电压未回升
			&& (0 != gPSmuGenParaR_102[eSmuGenPara102_VL2OffT])             		//需要保护
			&& (0 == lkstate)                                                       //未发生绝缘告警
			&& (eCURR_CHG != GetPileChgStateAPI())                                  //不充电
			&& ((eMODE_AUTO == GetPileWorkModeAPI())                                //自动模式
			|| (eMODE_HAND == GetPileWorkModeAPI())))                               //手动模式
		{
			//达到延时
			if(sHL3Timer >= gPSmuGenParaR_102[eSmuGenPara102_VL2OffT])
			{
				//设置关机命令
                gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWEROFF;
            	RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
			}
			else
			{
				sHL3Timer++;
			}
		}
		else
		{
			sHL3Timer = 0;
		}

		//欠压1级
		if((1 == vlstate)                 	                                        //单体欠压1级
			&& (GetGCellMinVoltAPI() <= gGBmuGenPara_102[eBmuGenPara102_VL1Lim])    //电压未回升
			&& (0 != gPSmuGenParaR_102[eSmuGenPara102_VL1OffT])                     //需要保护
			&& (0 == lkstate)                                                       //未发生绝缘告警
			&& (eCURR_CHG != GetPileChgStateAPI())                                  //不充电
			&& ((eMODE_AUTO == GetPileWorkModeAPI())                                //自动模式
			|| (eMODE_HAND == GetPileWorkModeAPI())))                               //手动模式
		{
			//达到延时
			if(sHL4Timer >= gPSmuGenParaR_102[eSmuGenPara102_VL1OffT])
			{
				//设置关机命令
                gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWEROFF;
            	RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
			}
			else
			{
				sHL4Timer++;
			}
		}
		else
		{
			sHL4Timer = 0;
		}

		//系统故障
		if((eWORK_ERR == GetPileWorkStateAPI())                                     //故障状态
			&& (0 != gPSmuGenParaR_102[eSmuGenPara102_ErrOffT])                     //需要保护
			&& (0 == lkstate)                                                       //未发生绝缘告警
			&& ((eMODE_AUTO == GetPileWorkModeAPI())                                   //自动模式
			|| (eMODE_HAND == GetPileWorkModeAPI())))                                  //手动模式
		{
			//达到延时
			if(sErrTimer >= gPSmuGenParaR_102[eSmuGenPara102_ErrOffT])
			{
				//设置关机命令
                gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWEROFF;
            	RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
			}
			else
			{
				sErrTimer++;
			}
		}
		else
		{
			sErrTimer = 0;
		}

	    /*外部通信正常*/
	    if(0 == GetPilePcsCanOffFlag())
	    {
	    	/*记录时间*/
	    	sCanTimer = GetSysSecondTimeAPI();
	    }
		/*外部通信中断超时*/
	    else if(ABS(GetSysSecondTimeAPI(), sCanTimer) >= gPSmuGenParaR_102[eSmuGenPara102_ComOffT])
		{
			//外部通讯故障下电保护
			if((0 != gPSmuGenParaR_102[eSmuGenPara102_ComOffT])                     //需要保护
				&& (0 == lkstate)                                                   //未发生绝缘告警
				&& (eCURR_IDLE == GetPileChgStateAPI())                             //不充放电
				&& ((eMODE_AUTO == GetPileWorkModeAPI())                            //自动模式
				|| (eMODE_HAND == GetPileWorkModeAPI())))                           //手动模式
			{
				//设置关机命令
                gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWEROFF;
            	RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
			}
		}
	}
	#endif
}

//=============================================================================================
//函数名称	: void PileSetSelfGroupOperAuthCmdAPI(u8 index, u16 password)
//输入参数	: port:设置端口[0,4]CAN口 [5,9]RS485口 index:子索引[0,1] password:操作权限密码
//输出参数	: void
//函数功能	: 堆控设置自身电池组操作权限命令
//注意事项	: 主索引85
//=============================================================================================
void PileSetSelfGroupOperAuthCmdAPI(u8 port, u8 index, u16 password)
{
	if(0 == index)
	{
		gGAuthorCtrl_85[0] = password;
    	//RcvSysAuthorityCmdAPI(port);
	}
	else if(1 == index)
	{
		gGAuthorCtrl_85[1] = password;
    	RcvSysAuthorityCmdAPI(port);
	}
}

//=============================================================================================
//函数名称	: void PileSetSelfGroupSysCtrlCmdAPI(u8 index, u8 cmd)
//输入参数	: index:子索引[0,2] cmd:系统控制命令
//输出参数	: void
//函数功能	: 堆控设置自身电池组系统控制命令
//注意事项	: 主索引86
//=============================================================================================
void PileSetSelfGroupSysCtrlCmdAPI(u8 index, u8 cmd)
{
	if(0 == index)
	{
		gGSysCmdCtrl_86[eSysCmdCtrl86_State] = cmd;
    	RcvGroupWorkStateCmdAPI(eAuthP_Can0);
	}
	else if(1 == index)
	{
		gGSysCmdCtrl_86[eSysCmdCtrl86_Mode] = cmd;
    	RcvGroupWorkModeCmdAPI(eAuthP_Can0);
	}
	else if(2 == index)
	{
		gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd] = cmd;
    	RcvGroupSysClrCmdAPI(eAuthP_Can0);
	}
}

//=============================================================================================
//函数名称	: void PileSetSelfGroupUserCtrlCmdAPI(u8 index, u8 len, u16 *cmd)
//输入参数	: index:子索引[0,7] len:子索引长度 *cmd:用户控制命令
//输出参数	: void
//函数功能	: 堆控设置自身电池组用户控制命令
//注意事项	: 主索引87
//=============================================================================================
void PileSetSelfGroupUserCtrlCmdAPI(u8 index, u8 len, u16 *cmd)
{
	u8 i = 0;

	for(i = 0; i < len; i++)
	{
		if((index + i) == eUserCtrl87_CellTrig)
		{
			gGUserCtrl_87[eUserCtrl87_CellTrig] = cmd[i];
			RcvGroupTrigInfoCmdAPI(eAuthP_Can0);
		}
		else if((index + i) == eUserCtrl87_InTran)
		{
			gGUserCtrl_87[eUserCtrl87_InTran] = cmd[i];
			RcvGroupTransInfoCmdAPI(eAuthP_Can0);
		}
		else if((index + i) == eUserCtrl87_HisErrSw)
		{
			gGUserCtrl_87[eUserCtrl87_HisErrSw] = cmd[i];
			RcvGroupDispHisErrCmdAPI(eAuthP_Can0);
		}
		else if((index + i) == eUserCtrl87_HisEvnSw)
		{
			gGUserCtrl_87[eUserCtrl87_HisEvnSw] = cmd[i];
			RcvGroupDispHisEventCmdAPI(eAuthP_Can0);
		}
		else if((index + i) == eUserCtrl87_HisWavSw)
		{
			gGUserCtrl_87[eUserCtrl87_HisWavSw] = cmd[i];
			RcvGroupDispHisWaveRcdCmdAPI(eAuthP_Can0);
		}
		else if((index + i) == eUserCtrl87_HisDodSw)
		{
			gGUserCtrl_87[eUserCtrl87_HisDodSw] = cmd[i];
			RcvGroupDispHisDodCmdAPI(eAuthP_Can0);
		}
		else if((index + i) == eUserCtrl87_AdhCheck)
		{
			gGUserCtrl_87[eUserCtrl87_AdhCheck] = cmd[i];
			RcvGroupReqAdhCheckCmdAPI(eAuthP_Can0);
		}
		else if((index + i) == eUserCtrl87_LeakCheck)
		{
			gGUserCtrl_87[eUserCtrl87_LeakCheck] = cmd[i];
			RcvGroupReqLeakCheckCmdAPI(eAuthP_Can0);
		}
	}
}

//=============================================================================================
//函数名称	: void PileSetSelfGroupSysDebugCmdAPI(u8 index, u8 len, u16 *cmd)
//输入参数	: index:子索引[0,7] len:子索引长度 *cmd:系统调试命令
//输出参数	: void
//函数功能	: 堆控设置自身电池组系统调试命令
//注意事项	: 主索引88
//=============================================================================================
void PileSetSelfGroupSysDebugCmdAPI(u8 index, u8 len, u16 *cmd)
{
	u8 i = 0;

	for(i = 0; i < len; i++)
	{
		if((index + i) < eDbgCmd88_Affirm)
		{
			gGDebugCtrl_88[index + i] = cmd[i];
			RcvGroupDebugOperCmdAPI(index + i);
		}
		else if(eDbgCmd88_Affirm == (index + i))
		{
			gGDebugCtrl_88[eDbgCmd88_Affirm] = cmd[i];
			RcvGroupDebugAffirmCmdAPI(eAuthP_Can0);
		}
	}
}

/*=================================================================================================
** @Name      : u8 GetPileSysRunFlagAPI(void)
** @Input     : void
** @Output    : 扩展堆控系统运行标志 0:未启用 1:运行为扩展堆控
** @Function  : 获取扩展堆控系统运行标志
** @The notes : 判断是否为并机主机系统
**===============================================================================================*/
u8 GetPileSysRunFlagAPI(void)
{
	return(sPileSysRunFlag);
}

/*=================================================================================================
** @Name      : u8 GetPileButStopReqFlagAPI(void)
** @Input     : void
** @Output    : 扩展堆控请求停机标志 0:未请求 1:请求停机
** @Function  : 获取扩展堆控请求停机标志
** @The notes : 判断是否需要高压下电停机
**===============================================================================================*/
u8 GetPileButStopReqFlagAPI(void)
{
	return(sPileButStopReq);
}

//=============================================================================================
//函数名称	: u8 GetPileGPowerOnStateAPI(void)
//输入参数	: void
//输出参数	: 堆控控制主控供电电源状态 0:断开 1:闭合
//函数功能	: 获取堆控控制主控供电电源状态
//注意事项	: 堆控控制主控低压供电电源
//=============================================================================================
u8 GetPileGPowerOnStateAPI(void)
{
    return(1);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void PileMainSysSelectTask(void)
** @Input     : void
** @Output    : void
** @Function  : 堆控决策是否启用为堆控并机功能
** @The notes : 10ms调用
**===============================================================================================*/
static void PileMainSysSelectTask(void)
{
    //BSMU系统可变换到其他正常电池组
    #if(1 == PILE_M_MOV_EN)
    u8 i = 0;
    u8 group = 0;
    #endif
    static u16 sTime = 0;

    //无需管理电池组
    if(gSysGenParaRO_100[eSGenPara100_AllGNum] <= 1)
    {
        //不起用堆控主机
        sPileSysRunFlag = 0;
        sTime = 0;
    }
    //堆控自身所在主控为起始电池组
    else if(0 == GetSysSelfGroupIdAPI())
    {
        //本主控已启用扩展BSMU系统
        if(1 == sPileSysRunFlag)
        {
            //本主控保持继续启用扩展BSMU系统(让已运行扩展BSMU系统的其他组退出扩展BSMU系统)
            sPileSysRunFlag = 1;
            sTime = 0;
        }
        //本主控未启用扩展堆控系统且未检测到本电池堆对应的堆控
        else if(0 == GetPileRcvSmuMsgFlag())
        {
            //延时2s未接收到本电池堆BSMU信息
            if(sTime >= 200)
            {
                //设置本BCU为扩展BSMU
                sPileSysRunFlag = 1;
            }
            //等待2s接收同网信息
            else
            {
                sTime++;
            }
        }
        //本主控未启用扩展堆控系统且检测到本电池堆已有对应的BSMU
        else
        {
            //本主控保持不启用扩展堆控系统(让已运行扩展BSMU系统的其他组继续运行扩展BSMU系统)
            sPileSysRunFlag = 0;
            sTime = 0;
        }
    }
    //堆控系统可变换到其他正常电池组
    #if(1 == PILE_M_MOV_EN)
    //本主控不是起始设备地址
    else
    {
        //检测到本电池堆对应的堆控
        if(1 == GetPileRcvSmuMsgFlag())
        {
            //延时3s仍接收到本电池堆BSMU
            if(sTime >= 300)
            {
                //设置本主控不是扩展堆控
                sPileSysRunFlag = 0;
            }
            //等待3s接收同网信息
            else
            {
                sTime++;
            }
        }
        //未检测到本电池堆对应的堆控设备
        else
        {
            //本主控已成为扩展BSMU
            if(1 == sPileSysRunFlag)
            {
                //保持本主控为扩展堆控
                sPileSysRunFlag = 1;
                sTime = 0;
            }
            //本主控与扩展BSMU通讯中断
            else if(0 == GetPileRcvSmuMsgFlag())
            {
                //获取本主控属于第几组
                group = GetSysSelfGroupIdAPI();

                //检测本组之前的电池组是否通讯正常
                for(i = 0; i < group; i++)
                {
                    //发现通讯正常的电池组
                    if(0 == GetPGroupCanOffStateAPI(i))
                    {
                        break;
                    }
                }

                //未发现本BCU之前有正常的电池组
                if(i >= group)
                {
                    //本主控启用为扩展堆控
                    sPileSysRunFlag = 1;
                    sTime = 0;
                }
            }
            //本主控未启用扩展堆控且未判断出与堆控通讯断线则等待断线故障出现或扩展堆控出现
            else
            {
                sTime = 0;
            }
        }
    }
    #endif
}
#endif
