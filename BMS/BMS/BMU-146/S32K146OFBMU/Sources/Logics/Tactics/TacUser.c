/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : TacUser.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池策略模块用户接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "Authority.h"
#include "InsuCheck.h"
#include "PortCheck.h"
#include "EventUser.h"
#include "RecordInterface.h"
#include "ParaInterface.h"
#include "CalcInterface.h"
#include "DiagInterface.h"
#include "StaInterface.h"
#include "TacInterface.h"
#include "GroupTactic.h"
#include "GroupDebug.h"
#include "GroupMode.h"
#include "EventUser.h"
#include "GroupMsg.h"
#include "CANUser.h"
#include "TacUser.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gGCtrlResult_81[];						 		/*控制结果状态信息数组*/
extern u16 gGDebugResult_82[];		                        /*调试结果状态信息数组*/
extern u16 gGSysCmdCtrl_86[];			                    /*系统控制命令信息数组*/
extern u16 gGUserCtrl_87[];	    	                        /*用户控制命令信息数组*/
extern u16 gGDebugCtrl_88[];			                    /*系统调试命令信息数组*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void RcvGroupWorkStateCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口 [10,12]按钮或内部口
** @Output    : void
** @Function  : 电池组接收切换工作状态命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvGroupWorkStateCmdAPI(u8 port)
{
    u8 cmd = 0;

    cmd = gGSysCmdCtrl_86[eSysCmdCtrl86_State];

    if((eCTRL_NORSTOP == cmd)                                       //正常停机命令
        || (eCTRL_FORCSTOP == cmd)                                  //强制停机命令
        || (eCTRL_EMERSTOP == cmd)                                  //紧急停机命令
        || (eCTRL_POWEROFF == cmd))                                 //关机命令
    {
    	/*产生事件*/
    	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 1, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), cmd, port);

        TacSetGWorkStateCmdAPI(cmd);                                //设置控制状态命令

        if((eMODE_EMERG == GetGWorkModeAPI())                       //紧急模式
            || (eMODE_DEBUG == GetGWorkModeAPI()))                  //调试模式
        {
            //TacRecoverGroupNorModeAPI();                          //退出到正常模式(历史模式)
        }
    }
    else                                                            //启动命令
    {
    	/*产生事件*/
    	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 1, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), cmd, port);

        TacSetGWorkStateCmdAPI(cmd);                                //设置控制状态命令
    }
}

/*=================================================================================================
** @Name      : void RcvGroupWorkModeCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口 [10,12]按钮或内部口
** @Output    : void
** @Function  : 电池组接收切换工作模式切换命令处理函数
** @The notes : 通信口或按钮或内部命令
**===============================================================================================*/
void RcvGroupWorkModeCmdAPI(u8 port)
{
    u8 cmd = 0;

    cmd = gGSysCmdCtrl_86[eSysCmdCtrl86_Mode];

    /*紧急模式需要高级权限,调试模式需要超级权限*/
    if(((eMODE_EMERG == cmd) && (eAuthL_Service != GetSysNowAuthorityAPI(port)) && (eAuthL_Admin != GetSysNowAuthorityAPI(port)))
		|| ((eMODE_DEBUG == cmd) && (eAuthL_Admin != GetSysNowAuthorityAPI(port))))
	{
    	return;
	}

    if((eWORK_IDLE == GetGWorkStateAPI())                           //待命状态
		|| (eWORK_ERR == GetGWorkStateAPI()))                       //故障状态
	{
    	/*产生事件*/
    	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 2, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), cmd, port);

    	TacSetGroupWorkModeAPI((e_WorkMode)cmd);                    //设置模式命令
	}
    else if(eWORK_RUN == GetGWorkStateAPI())                        //运行状态
	{
    	/*产生事件*/
    	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 2, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), cmd, port);

    	TacSetGroupWorkModeAPI((e_WorkMode)cmd);                    //设置模式命令

    	/*当前处于特殊模式*/
    	if((eMODE_EMERG == GetGWorkModeAPI())
    		|| (eMODE_DEBUG == GetGWorkModeAPI()))
    	{
        	/*切换到正常模式*/
        	if((eMODE_AUTO == cmd) || (eMODE_HAND == cmd))
        	{
            	/*产生状态控制事件*/
            	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 1, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), eCTRL_NORSTOP, port);

                TacSetGWorkStateCmdAPI(eCTRL_NORSTOP);              //设置控制停机状态命令
        	}
    	}
	}
    else if((eWORK_START == GetGWorkStateAPI())                     //启动状态
    	|| (eWORK_STOP == GetGWorkStateAPI()))                      //停机状态
	{
    	/*当前处于特殊模式*/
    	if((eMODE_EMERG == GetGWorkModeAPI())
    		|| (eMODE_DEBUG == GetGWorkModeAPI()))
    	{
        	/*产生模式事件*/
        	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 2, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), cmd, port);

        	TacSetGroupWorkModeAPI((e_WorkMode)cmd);                //设置模式命令

        	/*切换到正常模式*/
        	if((eMODE_AUTO == cmd) || (eMODE_HAND == cmd))
        	{
            	/*产生状态控制事件*/
            	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 1, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), eCTRL_NORSTOP, port);

                TacSetGWorkStateCmdAPI(eCTRL_NORSTOP);              //设置控制停机状态命令
        	}
    	}
	}
}

/*=================================================================================================
** @Name      : void RcvGroupSysClrCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口 [10,12]按钮或内部口
** @Output    : void
** @Function  : 电池组接收切换工作状态命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvGroupSysClrCmdAPI(u8 port)
{
    if(eCLR_NOWERR == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])        /*清除当前故障命令*/
    {
    	TacClrGCtrlErrFlagAPI();                                    //清除系统控制故障标志
    	ClrInsuAdhesErrFlagAPI();                                   //清除接触器粘连和绝缘检测故障标志
    	DiagClrPerDevExpFlagAPI();                                  //清除外围传感器故障标志
    	TacClrGroupProFlagAPI();                                    //清除故障保护标志

    	DiagClrErrNorStateAPI(0xff);                                //清除所有故障

    	DiagClrGroupSosLevel(0);                                     /*清除SOS级别(非锁定)*/
    }
    else if(eCLR_ERRRCD == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])   /*清除历史故障记录命令*/
    {
		//高级权限未开启
		if(eAuthL_Service > GetSysNowAuthorityAPI(port))
		{
			return;
		}

		RcdClrDateErrRcdUserAPI(0xff);                              /*清除外部EEP或FLASH故障记录*/
		DiagClrHisErrDisplayAPI();                                  /*清除故障记录显示*/
    }
    else if(eCLR_ERRNUM == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])   /*清除故障数目命令*/
    {
		//高级权限未开启
		if(eAuthL_Service > GetSysNowAuthorityAPI(port))
		{
			return;
		}

		ClrGroupHisErrNumAPI();                                     //清除故障计数记录
    }
    else if(eCLR_RUNTIME == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])  /*清除运行计时命令*/
    {
		//高级权限未开启
		if(eAuthL_Service > GetSysNowAuthorityAPI(port))
		{
			return;
		}

		ClrSysSecondTimeAPI();                                      //清除运行计时
    }
    else if(eCLR_HISCAP == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])   /*清除累计电量命令*/
    {
		//高级权限未开启
		if(eAuthL_Service > GetSysNowAuthorityAPI(port))
		{
			return;
		}

		ClrGroupEnerCapStatisInfo();                                //清除累计电量
    }
    else if(eCLR_DODRCD == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])   /*清除历史DOD记录命令*/
    {
		//高级权限未开启
		if(eAuthL_Service > GetSysNowAuthorityAPI(port))
		{
			return;
		}

		RcdClrAllDodRcdUserAPI();                                   /*清除外部EEP或FLASH历史DOD记录*/
		GroupClrDodStatisRcdAPI();                                  /*清除历史DOD记录显示*/
    }
    else if(eCLR_EVENRCD == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])  /*清除事件记录命令*/
    {
		//高级权限未开启
		if(eAuthL_Service > GetSysNowAuthorityAPI(port))
		{
			return;
		}

		RcdClrDateEventRcdUserAPI(0xff);                            /*清除外部EEP或FLASH事件记录*/
		EventClrHisRcdDisplayAPI();                                 /*清除事件记录显示*/
    }
    else if(eCLR_WAVERCD == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])  /*清除故障录波记录命令*/
    {
		//高级权限未开启
		if(eAuthL_Service > GetSysNowAuthorityAPI(port))
		{
			return;
		}

		RcdClrDateWaveRcdUserAPI(0xff);                             /*清除外部EEP或FLASH故障录波记录*/
		DiagClrHisWaveDisplayAPI();                                 /*清除故障录波记录显示*/
    }
    else if(eCLR_HISDATA == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])  /*清除历史数据记录命令*/
    {
		//高级权限未开启
		if(eAuthL_Service > GetSysNowAuthorityAPI(port))
		{
			return;
		}

		RcdClrDateDataRcdUserAPI(0xff);                              /*清除外部EEP或FLASH数据记录*/
		//DataClrHisRcdDisplayAPI();                                 /*清除运行数据记录显示*/
    }
    else if(eCLR_FACTORY == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])  /*恢复出厂设置命令*/
    {
		//超级权限未开启
		if(eAuthL_Admin != GetSysNowAuthorityAPI(port))
		{
			return;
		}

        //调试模式下进行
        if(eMODE_DEBUG == GetGWorkModeAPI())
        {
        	ParaRefactoryUserHandleHook();                          /*清除参数配置标志*/
        }
    }
    else if(eCLR_FALCLIM == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])  /*复位充放电降功率限制命令*/
    {
        ResetCurrLimFallAPI();                                      /*复位充放电降功率限制步骤清零*/
    }
    else if(eCLR_CDHGLIM == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])  /*复位充放电截止限制命令*/
    {
        TacResetGroupBattStateAPI();                                /*复位充放电截止限制状态清零*/
    }
    else if(eCLR_SOSLOCK == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])  /*清除SOS锁定命令*/
    {
		//高级权限未开启
		if(eAuthL_Service > GetSysNowAuthorityAPI(port))
		{
			return;
		}

    	TacClrGCtrlErrFlagAPI();                                    //清除系统控制故障标志
    	ClrInsuAdhesErrFlagAPI();                                   //清除接触器粘连和绝缘检测故障标志
    	DiagClrPerDevExpFlagAPI();                                  //清除外围传感器故障标志
    	TacClrGroupProFlagAPI();                                    //清除故障保护标志

    	DiagClrErrNorStateAPI(0xff);                                //清除所有故障

    	DiagClrGroupSosLevel(0xff);                                 /*清除SOS级别锁定*/
    }

	/*产生事件*/
	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 3, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd], port);
}

/*=================================================================================================
** @Name      : void RcvGroupTrigInfoCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组接收触发信息命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupTrigInfoCmdAPI(u8 port)
{
	if(eAuthP_Can0 == port)                             //只允许CAN0触发
	{
		/*触发单体信息*/
		if(1 == BitGet(gGUserCtrl_87[eUserCtrl87_CellTrig], 0))
		{
			CANSetCellCycleTxEnAPI(1);                  //使能
		}
		else
		{
			CANSetCellCycleTxEnAPI(0);                  //禁止
		}

		/*触发参数信息*/
		if(1 == BitGet(gGUserCtrl_87[eUserCtrl87_CellTrig], 1))
		{
			CANSetParaCycleTxEnAPI(1);                  //使能
		}
		else
		{
			CANSetParaCycleTxEnAPI(0);                  //禁止
		}

    	/*产生事件*/
    	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 4, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_CellTrig], port);
	}
}

/*=================================================================================================
** @Name      : void RcvGroupTransInfoCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组接收转发报文命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupTransInfoCmdAPI(u8 port)
{
	if(eAuthP_Can0 == port)                             //只允许CAN0设置
	{
		/*触发内部协议*/
		if(1 == gGUserCtrl_87[eUserCtrl87_InTran])
		{
			CANSetMsgCycleTxEnAPI(1);                   //使能内部协议周期信息
		}
		/*转发内部报文*/
		else if(2 == gGUserCtrl_87[eUserCtrl87_InTran])
		{
			CANSetMsgRouteTransEnAPI(1);                //使能报文路由转发功能
		}
		/*触发内部协议和转发内部报文*/
		else if(3 == gGUserCtrl_87[eUserCtrl87_InTran])
		{
			CANSetMsgCycleTxEnAPI(1);                   //使能内部协议周期信息
			CANSetMsgRouteTransEnAPI(1);                //使能报文路由转发功能
		}
		else
		{
			//CANSetMsgCycleTxEnAPI(0);                 //禁止内部协议周期信息
			CANSetMsgRouteTransEnAPI(0);                //禁止报文路由转发功能
		}

    	/*产生事件*/
    	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 5, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_InTran], port);
	}
}

/*=================================================================================================
** @Name      : void RcvGroupDispHisErrCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组接收切换显示历史故障信息命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupDispHisErrCmdAPI(u8 port)
{
	static u8 sHisDate = 0;

	/*切换指定日期记录*/
	if(gGUserCtrl_87[eUserCtrl87_HisErrSw] <= 30)
	{
		/*切换日期才更新显示*/
		if(sHisDate != gGUserCtrl_87[eUserCtrl87_HisErrSw])
		{
			/*记录当前指定日期*/
			sHisDate = gGUserCtrl_87[eUserCtrl87_HisErrSw];

			/*读取指定日期记录显示*/
			DiagChangHisErrDisplayAPI(sHisDate, 0);
		}
	}
	/*切换首页记录(当天最新30条)*/
	else if(90 == gGUserCtrl_87[eUserCtrl87_HisErrSw])
	{
		/*返回读取当前指定日期首页起始记录*/
		DiagChangHisErrDisplayAPI(sHisDate, 1);
	}
	/*切换下一页期记录(下30条)*/
	else if(91 == gGUserCtrl_87[eUserCtrl87_HisErrSw])
	{
		/*继续读取当前指定日期下一条记录*/
		DiagChangHisErrDisplayAPI(sHisDate, 0);
	}

	/*产生事件*/
	//EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 6, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_HisErrSw], port);
}

/*=================================================================================================
** @Name      : void RcvGroupDispHisEventCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组接收切换显示历史事件信息命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupDispHisEventCmdAPI(u8 port)
{
	static u8 sHisDate = 0;

	/*切换指定日期记录*/
	if(gGUserCtrl_87[eUserCtrl87_HisEvnSw] <= 30)
	{
		/*切换日期才更新显示*/
		if(sHisDate != gGUserCtrl_87[eUserCtrl87_HisEvnSw])
		{
			/*记录当前指定日期*/
			sHisDate = gGUserCtrl_87[eUserCtrl87_HisEvnSw];

			/*读取指定日期记录显示*/
			EventChangHisRcdDisplayAPI(sHisDate, 0);
		}
	}
	/*切换首页记录(当天最新30条)*/
	else if(90 == gGUserCtrl_87[eUserCtrl87_HisEvnSw])
	{
		/*返回读取当前指定日期首页最新记录*/
		EventChangHisRcdDisplayAPI(sHisDate, 1);
	}
	/*切换下一页期记录(下30条)*/
	else if(91 == gGUserCtrl_87[eUserCtrl87_HisEvnSw])
	{
		/*继续读取当前指定日期下一条记录*/
		EventChangHisRcdDisplayAPI(sHisDate, 0);
	}

	/*产生事件*/
	//EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 7, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_HisEvnSw], port);
}

/*=================================================================================================
** @Name      : void RcvGroupDispHisWaveRcdCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组接收切换显示历史故障录波信息命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupDispHisWaveRcdCmdAPI(u8 port)
{
	static u8 sHisDate = 0;

	/*切换指定日期记录*/
	if(gGUserCtrl_87[eUserCtrl87_HisWavSw] <= 30)
	{
		/*切换日期才更新显示*/
		if(sHisDate != gGUserCtrl_87[eUserCtrl87_HisWavSw])
		{
			/*记录当前指定日期*/
			sHisDate = gGUserCtrl_87[eUserCtrl87_HisWavSw];

			/*读取指定日期记录显示*/
			DiagChangHisWaveDisplayAPI(sHisDate, 0);
		}
	}
	/*切换首页记录(当天最新30条)*/
	else if(90 == gGUserCtrl_87[eUserCtrl87_HisWavSw])
	{
		/*返回读取当前指定日期首页最新记录*/
		DiagChangHisWaveDisplayAPI(sHisDate, 1);
	}
	/*切换下一条记录*/
	else if(91 == gGUserCtrl_87[eUserCtrl87_HisWavSw])
	{
		/*继续读取当前指定日期下一条记录*/
		DiagChangHisWaveDisplayAPI(sHisDate, 0);
	}

	/*产生事件*/
	//EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 8, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_HisEvnSw], port);
}

/*=================================================================================================
** @Name      : void RcvGroupDispHisDodCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组接收切换显示历史DOD信息命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupDispHisDodCmdAPI(u8 port)
{
	/*切换最新记录*/
	if(0 == gGUserCtrl_87[eUserCtrl87_HisDodSw])
	{
		/*读取首页记录显示*/
		GroupChangHisDodDisplayAPI(1);
	}
	/*切换下一页记录(下100个DOD)*/
	else if(91 == gGUserCtrl_87[eUserCtrl87_HisDodSw])
	{
		/*继续读取下一个记录*/
		GroupChangHisDodDisplayAPI(0);
	}

	/*产生事件*/
	//EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 9, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_HisEvnSw], port);
}

/*=================================================================================================
** @Name      : void RcvGroupReqAdhCheckCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组接收请求接触器粘连检测命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupReqAdhCheckCmdAPI(u8 port)
{
	/*请求开始粘连检测*/
	if(1 == gGUserCtrl_87[eUserCtrl87_AdhCheck])
	{
		/*请求重新粘连检测*/
		UserReqSwhAdhesDiagAPI();

		/*记录正在检测*/
		gGCtrlResult_81[eCtrlRes81_AdhCheck] = 1;
	}
	/*请求停止粘连检测*/
	else if(2 == gGUserCtrl_87[eUserCtrl87_AdhCheck])
	{
		/*记录停止检测*/
		gGCtrlResult_81[eCtrlRes81_AdhCheck] = 2;
	}
	/*请求自动进行粘连检测*/
	else
	{
		/*记录自动进行检测*/
		gGCtrlResult_81[eCtrlRes81_AdhCheck] = 0;
	}

	/*产生事件*/
	//EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 10, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_AdhCheck], port);
}

/*=================================================================================================
** @Name      : void RcvGroupReqLeakCheckCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组接收请求绝缘检测命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupReqLeakCheckCmdAPI(u8 port)
{
	/*请求开始绝缘检测*/
	if(1 == gGUserCtrl_87[eUserCtrl87_LeakCheck])
	{
		/*请求重新绝缘检测*/
		UserReqInsuCheckAPI();

		/*记录正在检测*/
		gGCtrlResult_81[eCtrlRes81_LeakCheck] = 1;
	}
	/*请求停止绝缘检测*/
	else if(2 == gGUserCtrl_87[eUserCtrl87_LeakCheck])
	{
		/*记录停止检测*/
		gGCtrlResult_81[eCtrlRes81_LeakCheck] = 2;
	}
	/*请求自动进行绝缘检测*/
	else
	{
		/*记录自动进行检测*/
		gGCtrlResult_81[eCtrlRes81_LeakCheck] = 0;
	}

	/*产生事件*/
	//EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 11, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_LeakCheck], port);
}

/*=================================================================================================
** @Name      : void RcvGroupShutFanCtrlCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组接收百叶风机控制命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupShutFanCtrlCmdAPI(u8 port)
{
	//运维权限未开启
	if(eAuthL_Operater > GetSysNowAuthorityAPI(port))
	{
		return;
	}

	/*命令有效*/
	if(gGUserCtrl_87[eUserCtrl87_ShutFan] <= 1)
	{
		/*输入控制命令*/
		TacSetAuxShutUserCmdAPI((u8)gGUserCtrl_87[eUserCtrl87_ShutFan]);
	}

	/*产生事件*/
	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 12, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_ShutFan], port);
}

/*=================================================================================================
** @Name      : void RcvGroupLightCtrlCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组接收电池柜照明控制命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupLightCtrlCmdAPI(u8 port)
{
	//运维权限未开启
	if(eAuthL_Operater > GetSysNowAuthorityAPI(port))
	{
		return;
	}

	/*命令有效*/
	if(1 == gGUserCtrl_87[eUserCtrl87_Lighting])
	{
		/*输入控制命令*/
		TacSetAuxRackLightCmdAPI(1);
	}
	else if(0 == gGUserCtrl_87[eUserCtrl87_Lighting])
	{
		/*输入控制命令*/
		TacSetAuxRackLightCmdAPI(2);
	}

	/*产生事件*/
	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 13, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_Lighting], port);
}

/*=================================================================================================
** @Name      : void RcvGroupZHSAForceCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组声光报警器强制控制命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupZHSAForceCmdAPI(u8 port)
{
	//运维权限未开启
	if(eAuthL_Operater > GetSysNowAuthorityAPI(port))
	{
		return;
	}

	/*命令有效*/
	if(1 == gGUserCtrl_87[eUserCtrl87_ZHSAForce])
	{
		/*输入控制命令*/
		TacSetAuxZHSAForceCmdAPI(1);
	}
	else if(0 == gGUserCtrl_87[eUserCtrl87_ZHSAForce])
	{
		/*输入控制命令*/
		TacSetAuxZHSAForceCmdAPI(2);
	}

	/*产生事件*/
	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 14, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_ZHSAForce], port);
}

/*=================================================================================================
** @Name      : void RcvGroupFireTrigCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 电池组气溶胶消防器手动触发命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void RcvGroupFireTrigCmdAPI(u8 port)
{
	//运维权限未开启
	if(eAuthL_Operater > GetSysNowAuthorityAPI(port))
	{
		return;
	}

	/*命令有效*/
	if(gGUserCtrl_87[eUserCtrl87_FireTrig] <= 1)
	{
		/*输入控制命令*/
		//TacSetAuxFireTrigCmdAPI((u8)gGUserCtrl_87[eUserCtrl87_FireTrig]);
	}

	/*产生事件*/
	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 15, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGUserCtrl_87[eUserCtrl87_FireTrig], port);
}

/*=================================================================================================
** @Name      : void RcvGroupDebugOperCmdAPI(u8 index)
** @Input     : index:调试命令索引
** @Output    : void
** @Function  : 电池组接收切换调试操作命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvGroupDebugOperCmdAPI(u8 index)
{
    /*调试确认命令处理*/
	if(index < eDbgCmd88_Affirm)
	{
		gGDebugResult_82[index] = gGDebugCtrl_88[index];
	}
}

/*=================================================================================================
** @Name      : void RcvGroupDebugAffirmCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口 10:按钮 11:内部
** @Output    : void
** @Function  : 电池组接收调试确认命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvGroupDebugAffirmCmdAPI(u8 port)
{
	u8 evnId = 0;

    /*调试确认命令处理*/
    TacSetGroupDebugAffirmCmdAPI(gGDebugCtrl_88[eDbgCmd88_Affirm]);

    /*调试主控*/
    if(gGDebugCtrl_88[eDbgCmd88_BMUid] <= 16)
    {
    	evnId = 1;
    }
    /*0xff非调试主控(从控或从设备)*/
    else
    {
    	evnId = 0;
    }

	/*产生事件*/
	EventRcdDisplayUserAPI(eEVN_RCDSORT_DEBUG, evnId, gGDebugCtrl_88[eDbgCmd88_DbgType], gGDebugCtrl_88[eDbgCmd88_BalType], gGDebugCtrl_88[eDbgCmd88_BalStat], port);
}

/*=================================================================================================
** @Name      : void TacCtrlGroupSwhStateAPI(e_GroupSwhSort swhSort, e_GPIOState state)
** @Input     : swhSort:接触器名称[0,11] state:控制接触器状态 0:断开 1:闭合
** @Output    : void
** @Function  : 控制电池组相应接触器状态
** @The notes : 控制HS/LS/干接点
**===============================================================================================*/
void TacCtrlGroupSwhStateAPI(e_GroupSwhSort swhSort, e_GPIOState state)
{
    u8 flag = 0;

	/*状态有效*/
	if(state < ePSTATE_NUM)
	{
		/*区分接触器名称*/
		switch(swhSort)
		{
			/*主正接触器*/
			case eGroupSwh_Mai:
				/*记录接触器状态改变*/
	            if(state != TacGetGroupSwhCtrlStateAPI(eGroupSwh_Mai))
	            {
	                flag = 1;
	            }

				/*控制LS1*/
				(void)DEVGPIOSetPortState(ePOut_LDRVK0, state);
				/*控制HS1*/
				(void)DEVGPIOSetPortState(ePOut_DRVK0, state);
				break;

			/*预充接触器*/
			case eGroupSwh_Pre:
				/*记录接触器状态改变*/
	            if(state != TacGetGroupSwhCtrlStateAPI(eGroupSwh_Pre))
	            {
	                flag = 1;
	            }

				/*控制HS2*/
				(void)DEVGPIOSetPortState(ePOut_DRVK1, state);
				break;

			/*负极接触器*/
			case eGroupSwh_Neg:
				/*记录接触器状态改变*/
	            if(state != TacGetGroupSwhCtrlStateAPI(eGroupSwh_Neg))
	            {
	                flag = 1;
	            }

				/*控制LS3*/
				(void)DEVGPIOSetPortState(ePOut_LDRVK2, state);
				/*控制HS3*/
				(void)DEVGPIOSetPortState(ePOut_DRVK2, state);
				break;

			/*电源接触器*/
			case eGroupSwh_Pwr:
				/*控制HS4*/
				(void)DEVGPIOSetPortState(ePOut_DRVK3, state);
				break;

			/*运行灯LED*/
			case eGroupSwh_Run:
				/*控制HS5*/
				(void)DEVGPIOSetPortState(ePOut_DRVK4, state);
				break;

			/*故障灯LED*/
			case eGroupSwh_Alm:
				/*控制HS6*/
				(void)DEVGPIOSetPortState(ePOut_DRVK5, state);
				break;

			/*烟感电源*/
			case eGroupSwh_Smg:
				/*控制HS7*/
				(void)DEVGPIOSetPortState(ePOut_DRVK6, state);
				break;

			/*加热接触器*/
			case eGroupSwh_Hot:
				/*控制HS8*/
				(void)DEVGPIOSetPortState(ePOut_DRVK7, state);
				break;

			/*风扇电源*/
			case eGroupSwh_Fan:
				/*控制HS9*/
				(void)DEVGPIOSetPortState(ePOut_DRVK8, state);
				break;

			/*状态干接点*/
			case eGroupSwh_Sta:
				/*控制干接点1*/
				(void)DEVGPIOSetPortState(ePOut_STADRY0, state);
				break;

			/*充电干接点*/
			case eGroupSwh_Chg:
				/*控制干接点2*/
				(void)DEVGPIOSetPortState(ePOut_CHGDRY1, state);
				break;

			/*放电干接点*/
			case eGroupSwh_Dhg:
				/*控制干接点3*/
				(void)DEVGPIOSetPortState(ePOut_DHGDRY2, state);
				break;

			default:
				break;
		}

		/*接触器状态改变则发送一帧报文*/
	    if(1 == flag)
	    {
	    	PortStateCheckTask();           //更新接触器状态信息
	    	SendCanGroupPortStateInfoMsg(); //上传输入输出状态信息
	    }
	}
}

/*=================================================================================================
** @Name      : void TacCtrlGroupRunSwhAllOffAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 控制电池组所有运行接触器均断开
** @The notes : 控制主正/预充/主负接触器
**===============================================================================================*/
void TacCtrlGroupRunSwhAllOffAPI(void)
{
	/*预充接触器,控制HS2*/
	(void)DEVGPIOSetPortState(ePOut_DRVK1, ePSTATE_OFF);

	/*主正接触器,控制HS1/LS1*/
	(void)DEVGPIOSetPortState(ePOut_DRVK0, ePSTATE_OFF);
	(void)DEVGPIOSetPortState(ePOut_LDRVK0, ePSTATE_OFF);

	/*负极接触器,HS3/LS3*/
	(void)DEVGPIOSetPortState(ePOut_DRVK2, ePSTATE_OFF);
	(void)DEVGPIOSetPortState(ePOut_LDRVK2, ePSTATE_OFF);
}

/*=================================================================================================
** @Name      : e_GPIOState TacGetGroupSwhCtrlStateAPI(e_GroupSwhSort swhSort)
** @Input     : swhSort:接触器名称[0,11]
** @Output    : 电池组相应接触器当前控制输出状态 0:断开 1:闭合
** @Function  : 获取电池组相应接触器当前控制输出状态
** @The notes : 控制信号
**===============================================================================================*/
e_GPIOState TacGetGroupSwhCtrlStateAPI(e_GroupSwhSort swhSort)
{
	u8 state = ePSTATE_OFF;

	/*区分接触器名称*/
	switch(swhSort)
	{
		/*主正接触器*/
		case eGroupSwh_Mai:
			/*HS1/LS1均闭合*/
			if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK0)) && (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_LDRVK0)))
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;

		/*预充接触器*/
		case eGroupSwh_Pre:
			/*HS2状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK1);
			break;

		/*负极接触器*/
		case eGroupSwh_Neg:
			/*HS3/LS3均闭合*/
			if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK2)) && (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_LDRVK2)))
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;

		/*电源接触器*/
		case eGroupSwh_Pwr:
			/*HS4状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK3);
			break;

		/*运行灯LED*/
		case eGroupSwh_Run:
			/*HS5状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK4);
			break;

		/*故障灯LED*/
		case eGroupSwh_Alm:
			/*HS6状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK5);
			break;

		/*烟感电源*/
		case eGroupSwh_Smg:
			/*HS7状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK6);
			break;

		/*加热接触器*/
		case eGroupSwh_Hot:
			/*HS8状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK7);
			break;

		/*风扇电源*/
		case eGroupSwh_Fan:
			/*HS9状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK8);
			break;

		/*状态干接点*/
		case eGroupSwh_Sta:
			/*干接点1状态*/
			state = DEVGPIOGetOutPortState(ePOut_STADRY0);
			break;

		/*充电干接点*/
		case eGroupSwh_Chg:
			/*干接点2状态*/
			state = DEVGPIOGetOutPortState(ePOut_CHGDRY1);
			break;

		/*放电干接点*/
		case eGroupSwh_Dhg:
			/*干接点3状态*/
			state = DEVGPIOGetOutPortState(ePOut_DHGDRY2);
			break;

		default:
			return((e_GPIOState)0xff);
	}

	return((e_GPIOState)state);
}

/*=================================================================================================
** @Name      : e_GPIOState TacGetGroupSwhBackStateAPI(e_GroupSwhSort swhSort)
** @Input     : swhSort:接触器名称[0,11]
** @Output    : 电池组相应接触器当前回检状态 0:断开 1:闭合
** @Function  : 获取电池组相应接触器当前回检状态
** @The notes : 回检信号(输出信号/板级回检信号/反馈输入信号)
**===============================================================================================*/
e_GPIOState TacGetGroupSwhBackStateAPI(e_GroupSwhSort swhSort)
{
	u8 state = 0;
	#if(0 != BMU_SWH_BACK_TYPE)
	u8 backState = 0;
	#endif

	/*区分接触器名称*/
	switch(swhSort)
	{
		/*主正接触器*/
		case eGroupSwh_Mai:
			/*HS1/LS1均闭合*/
			/*不回检(控制信号状态)*/
			#if(0 == BMU_SWH_BACK_TYPE)
			if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK0)) && (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_LDRVK0)))
			/*板级信号回检*/
			#elif(1 == BMU_SWH_BACK_TYPE)
			/*低边开关故障回检信号为低时确认闭合故障*/
			if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK1_Sta))
			{
				/*认为未闭合*/
				backState = ePSTATE_OFF;
			}
			/*低边开关故障回检信号为高时无法判定故障*/
			else
			{
				/*取低边开关控制状态*/
				backState = DEVGPIOGetOutPortState(ePOut_LDRVK0);
			}
			if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK0)) && (ePSTATE_ON == backState))
			/*板级采样回检*/
			#elif(2 == BMU_SWH_BACK_TYPE)
			/*低边开关故障回检信号为低时确认闭合故障*/
			if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK1_Sta))
			{
				/*认为未闭合*/
				backState = ePSTATE_OFF;
			}
			/*低边开关故障回检信号为高时无法判定故障*/
			else
			{
				/*取低边开关控制状态*/
				backState = DEVGPIOGetOutPortState(ePOut_LDRVK0);
			}
			if((GetGHSDriverCurrAPI(0) >= TAC_HS_ON_CURR) && (ePSTATE_ON == backState))
			/*外部回检*/
			#else
			/*低边开关故障回检信号为低时确认闭合故障*/
			if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK1_Sta))
			{
				/*认为未闭合*/
				backState = ePSTATE_OFF;
			}
			/*低边开关故障回检信号为高时无法判定故障*/
			else
			{
				/*取低边开关控制状态*/
				backState = DEVGPIOGetOutPortState(ePOut_LDRVK0);
			}
			if((ePSTATE_ON == DEVGPIOGetInPortState(ePIn_DRY2)) && (ePSTATE_ON == backState))
			#endif
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;

		/*预充接触器*/
		case eGroupSwh_Pre:
			/*HS2状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK1);
			break;

		/*负极接触器*/
		case eGroupSwh_Neg:
			/*HS3/LS3均闭合*/
			/*不回检(控制信号状态)*/
			#if(0 == BMU_SWH_BACK_TYPE)
			if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK2)) && (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_LDRVK2)))
			/*板级信号回检*/
			#elif(1 == BMU_SWH_BACK_TYPE)
			/*低边开关故障回检信号为低时确认闭合故障*/
			if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK3_Sta))
			{
				/*认为未闭合*/
				backState = ePSTATE_OFF;
			}
			/*低边开关故障回检信号为高时无法判定故障*/
			else
			{
				/*取低边开关控制状态*/
				backState = DEVGPIOGetOutPortState(ePOut_LDRVK2);
			}
			if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK2)) && (ePSTATE_ON == backState))
			/*板级采样回检*/
			#elif(2 == BMU_SWH_BACK_TYPE)
			/*低边开关故障回检信号为低时确认闭合故障*/
			if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK3_Sta))
			{
				/*认为未闭合*/
				backState = ePSTATE_OFF;
			}
			/*低边开关故障回检信号为高时无法判定故障*/
			else
			{
				/*取低边开关控制状态*/
				backState = DEVGPIOGetOutPortState(ePOut_LDRVK2);
			}
			if((GetGHSDriverCurrAPI(2) >= TAC_HS_ON_CURR) && (ePSTATE_ON == backState))
			/*外部回检*/
			#else
			/*低边开关故障回检信号为低时确认闭合故障*/
			if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK3_Sta))
			{
				/*认为未闭合*/
				backState = ePSTATE_OFF;
			}
			/*低边开关故障回检信号为高时无法判定故障*/
			else
			{
				/*取低边开关控制状态*/
				backState = DEVGPIOGetOutPortState(ePOut_LDRVK2);
			}
			if((ePSTATE_ON == DEVGPIOGetInPortState(ePIn_DRY3)) && (ePSTATE_ON == backState))
			#endif
			{
				state = 1;
			}
			else
			{
				state = 0;
			}
			break;

		/*电源接触器*/
		case eGroupSwh_Pwr:
			/*HS4状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK3);
			break;

		/*运行灯LED*/
		case eGroupSwh_Run:
			/*HS5状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK4);
			break;

		/*故障灯LED*/
		case eGroupSwh_Alm:
			/*HS6状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK5);
			break;

		/*烟感电源*/
		case eGroupSwh_Smg:
			/*HS7状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK6);
			break;

		/*加热接触器*/
		case eGroupSwh_Hot:
			/*HS8状态*/
			state = DEVGPIOGetOutPortState(ePOut_DRVK7);
			break;

		/*风扇电源*/
		case eGroupSwh_Fan:
			/*HS9状态*/
			/*不回检(控制信号状态)*/
			#if(0 == BMU_SWH_BACK_TYPE)
			state = DEVGPIOGetOutPortState(ePOut_DRVK8);
			/*板级信号/采样回检*/
			#elif((1 == BMU_SWH_BACK_TYPE) || (2 == BMU_SWH_BACK_TYPE))
			/*高边开关故障回检信号为低时确认闭合故障*/
			if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_K9_Sta))
			{
				/*认为未闭合*/
				state = ePSTATE_OFF;
			}
			/*高边开关故障回检信号为高时无法判定故障*/
			else
			{
				/*取高边开关控制状态*/
				state = DEVGPIOGetOutPortState(ePOut_DRVK8);
			}
			/*外部回检*/
			#else
			state = DEVGPIOGetInPortState(ePIn_DRY5);
			#endif
			break;

		/*状态干接点*/
		case eGroupSwh_Sta:
			/*干接点1状态*/
			state = DEVGPIOGetOutPortState(ePOut_STADRY0);
			break;

		/*充电干接点*/
		case eGroupSwh_Chg:
			/*干接点2状态*/
			state = DEVGPIOGetOutPortState(ePOut_CHGDRY1);
			break;

		/*放电干接点*/
		case eGroupSwh_Dhg:
			/*干接点3状态*/
			state = DEVGPIOGetOutPortState(ePOut_DHGDRY2);
			break;

		default:
			return((e_GPIOState)0xff);
	}

	return((e_GPIOState)state);
}

/*=================================================================================================
** @Name      : u8 TacGetGroupRunSwhStateAPI()
** @Input     : void
** @Output    : 电池组运行接触器状态[低4位:控制状态 高4位:回检状态](按bit位表示接触器 bit0:主正接触器控制 bit1:预充接触器控制 bit2:主负接触器控制;bit4:主正接触器回检 bit5:预充接触器回检 bit6:主负接触器回检)
** @Function  : 获取电池组运行接触器当前状态
** @The notes : 控制信号和回检信号
**===============================================================================================*/
u8 TacGetGroupRunSwhStateAPI()
{
	u8 state = 0;
	#if(0 != BMU_SWH_BACK_TYPE)
	u8 backState = 0;
	#endif

	/*主正接触器控制:HS1/LS1均闭合*/
	if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK0)) && (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_LDRVK0)))
	{
		BitSet(state, eGroupSwh_Mai);
	}

	/*预充接触器控制:HS2状态*/
	if(ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK1))
	{
		BitSet(state, eGroupSwh_Pre);
	}

	/*负极接触器控制:HS3/LS3均闭合*/
	if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK2)) && (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_LDRVK2)))
	{
		BitSet(state, eGroupSwh_Neg);
	}

	/*主正接触器回检:HS1/LS1均闭合*/
	/*不回检(控制信号状态)*/
	#if(0 == BMU_SWH_BACK_TYPE)
	if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK0)) && (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_LDRVK0)))
	/*板级信号回检*/
	#elif(1 == BMU_SWH_BACK_TYPE)
	/*低边开关故障回检信号为低时确认闭合故障*/
	if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK1_Sta))
	{
		/*认为未闭合*/
		backState = ePSTATE_OFF;
	}
	/*低边开关故障回检信号为高时无法判定故障*/
	else
	{
		/*取低边开关控制状态*/
		backState = DEVGPIOGetOutPortState(ePOut_LDRVK0);
	}
	if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK0)) && (ePSTATE_ON == backState))
	/*板级采样回检*/
	#elif(2 == BMU_SWH_BACK_TYPE)
	/*低边开关故障回检信号为低时确认闭合故障*/
	if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK1_Sta))
	{
		/*认为未闭合*/
		backState = ePSTATE_OFF;
	}
	/*低边开关故障回检信号为高时无法判定故障*/
	else
	{
		/*取低边开关控制状态*/
		backState = DEVGPIOGetOutPortState(ePOut_LDRVK0);
	}
	if((GetGHSDriverCurrAPI(0) >= TAC_HS_ON_CURR) && (ePSTATE_ON == backState))
	/*外部回检*/
	#else
	/*低边开关故障回检信号为低时确认闭合故障*/
	if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK1_Sta))
	{
		/*认为未闭合*/
		backState = ePSTATE_OFF;
	}
	/*低边开关故障回检信号为高时无法判定故障*/
	else
	{
		/*取低边开关控制状态*/
		backState = DEVGPIOGetOutPortState(ePOut_LDRVK0);
	}
	if((ePSTATE_ON == DEVGPIOGetInPortState(ePIn_DRY2)) && (ePSTATE_ON == backState))
	#endif
	{
		BitSet(state, eGroupSwh_Mai + 4);
	}

	/*预充接触器回检:HS2状态*/
	if(DEVGPIOGetOutPortState(ePOut_DRVK1))
	{
		BitSet(state, eGroupSwh_Pre + 4);
	}

	/*负极接触器回检:HS3/LS3均闭合*/
	/*不回检(控制信号状态)*/
	#if(0 == BMU_SWH_BACK_TYPE)
	if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK2)) && (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_LDRVK2)))
	/*板级信号回检*/
	#elif(1 == BMU_SWH_BACK_TYPE)
	/*低边开关故障回检信号为低时确认闭合故障*/
	if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK3_Sta))
	{
		/*认为未闭合*/
		backState = ePSTATE_OFF;
	}
	/*低边开关故障回检信号为高时无法判定故障*/
	else
	{
		/*取低边开关控制状态*/
		backState = DEVGPIOGetOutPortState(ePOut_LDRVK2);
	}
	if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK2)) && (ePSTATE_ON == backState))
	/*板级采样回检*/
	#elif(2 == BMU_SWH_BACK_TYPE)
	/*低边开关故障回检信号为低时确认闭合故障*/
	if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK3_Sta))
	{
		/*认为未闭合*/
		backState = ePSTATE_OFF;
	}
	/*低边开关故障回检信号为高时无法判定故障*/
	else
	{
		/*取低边开关控制状态*/
		backState = DEVGPIOGetOutPortState(ePOut_LDRVK2);
	}
	if((GetGHSDriverCurrAPI(2) >= TAC_HS_ON_CURR) && (ePSTATE_ON == backState))
	/*外部回检*/
	#else
	/*低边开关故障回检信号为低时确认闭合故障*/
	if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK3_Sta))
	{
		/*认为未闭合*/
		backState = ePSTATE_OFF;
	}
	/*低边开关故障回检信号为高时无法判定故障*/
	else
	{
		/*取低边开关控制状态*/
		backState = DEVGPIOGetOutPortState(ePOut_LDRVK2);
	}
	if((ePSTATE_ON == DEVGPIOGetInPortState(ePIn_DRY3)) && (ePSTATE_ON == backState))
	#endif
	{
		BitSet(state, eGroupSwh_Neg + 4);
	}

	return(state);
}

/*=================================================================================================
** @Name      : u8 TacGetGroupRunSwhAllOffStateAPI(void)
** @Input     : void
** @Output    : 电池组运行接触器均断开状态 FALSE:未全断开 TRUE:全断开
** @Function  : 获取电池组运行接触器均断开状态
** @The notes : 回检信号(输出信号/板级回检信号/反馈输入信号)
**===============================================================================================*/
u8 TacGetGroupRunSwhAllOffStateAPI(void)
{
	/*不回检(控制信号状态)*/
	#if(0 == BMU_SWH_BACK_TYPE)
	/*主正接触器HS1/LS1均断开,负极接触器HS3/LS3均断开,预充接触器HS2断开*/
	if((ePSTATE_OFF == DEVGPIOGetOutPortState(ePOut_DRVK0)) && (ePSTATE_OFF == DEVGPIOGetOutPortState(ePOut_LDRVK0))
		&& (ePSTATE_OFF == DEVGPIOGetOutPortState(ePOut_DRVK2)) && (ePSTATE_OFF == DEVGPIOGetOutPortState(ePOut_LDRVK2))
		&& (ePSTATE_OFF == DEVGPIOGetOutPortState(ePOut_DRVK1)))
	/*板级信号回检*/
	#elif(1 == BMU_SWH_BACK_TYPE)
	if((ePSTATE_OFF == DEVGPIOGetOutPortState(ePOut_DRVK0)) && (ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK1_Sta))
		&& (ePSTATE_OFF == DEVGPIOGetOutPortState(ePOut_DRVK2)) && (ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK3_Sta))
		&& (ePSTATE_OFF == DEVGPIOGetOutPortState(ePOut_DRVK1)))
	/*板级采样回检*/
	#elif(2 == BMU_SWH_BACK_TYPE)
	if((GetGHSDriverCurrAPI(0) < TAC_HS_ON_CURR) && (ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK1_Sta))
        && (GetGHSDriverCurrAPI(2) < TAC_HS_ON_CURR) && (ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK3_Sta))
		&& (ePSTATE_OFF == DEVGPIOGetOutPortState(ePOut_DRVK1)))
	/*外部回检*/
	#else
	if((ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_DRY1)) && (ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK1_Sta))
		&& (ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_DRY2)) && (ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_LK3_Sta))
		&& (ePSTATE_OFF == DEVGPIOGetOutPortState(ePOut_DRVK1)))
	#endif
	{
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}

/*=================================================================================================
** @Name      : u8 TacGetGroupRunSwhAllOnStateAPI(void)
** @Input     : void
** @Output    : 电池组运行接触器正负极均闭合状态 FALSE:未全闭合 TRUE:全闭合
** @Function  : 获取电池组运行接触器正负极均闭合状态
** @The notes : 回检信号(输出信号/板级回检信号/反馈输入信号)
**===============================================================================================*/
u8 TacGetGroupRunSwhAllOnStateAPI(void)
{
	/*不回检(控制信号状态)*/
	#if(0 == BMU_SWH_BACK_TYPE)
	/*负极接触器HS3/LS3均闭合,主正接触器HS1/LS1均闭合或预充接触器HS2闭合*/
	if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK2)) && (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_LDRVK2))
		&& (((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK0)) && (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_LDRVK0)))
		|| (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK1))))
	/*板级信号回检*/
	#elif(1 == BMU_SWH_BACK_TYPE)
	if((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK2)) && (ePSTATE_ON == DEVGPIOGetInPortState(ePIn_LK3_Sta))
		&& (((ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK0)) && (ePSTATE_ON == DEVGPIOGetInPortState(ePIn_LK1_Sta)))
		|| (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK1))))
	/*板级采样回检*/
	#elif(2 == BMU_SWH_BACK_TYPE)
	if((GetGHSDriverCurrAPI(2) >= TAC_HS_ON_CURR) && (ePSTATE_ON == DEVGPIOGetInPortState(ePIn_LK3_Sta))
		&& (((GetGHSDriverCurrAPI(0) >= TAC_HS_ON_CURR) && (ePSTATE_ON == DEVGPIOGetInPortState(ePIn_LK1_Sta)))
        || (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK1))))
	/*外部回检*/
	#else
	if((ePSTATE_ON == DEVGPIOGetInPortState(ePIn_DRY2)) && (ePSTATE_ON == DEVGPIOGetInPortState(ePIn_LK3_Sta))
		&& (((ePSTATE_ON == DEVGPIOGetInPortState(ePIn_DRY1)) && (ePSTATE_ON == DEVGPIOGetInPortState(ePIn_LK1_Sta)))
		|| (ePSTATE_ON == DEVGPIOGetOutPortState(ePOut_DRVK1))))
	#endif
	{
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
