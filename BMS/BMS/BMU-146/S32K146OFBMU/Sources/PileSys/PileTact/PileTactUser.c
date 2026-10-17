/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileTactUser.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控电池堆策略接口模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PilePara.h"
#include "PublicInfo.h"
#include "PileInfo.h"
#include "PileDnMsg.h"
#include "DiagCheck.h"
#include "PileDiagCheck.h"
#include "PileUserAPI.h"
#include "PileTactics.h"
#include "PileSysMode.h"
#include "PileSysDebug.h"
#include "PileProtect.h"
#include "PileTactUser.h"

#include "Authority.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gPDebugResult_82[];			/*调试结果状态信息数组*/
extern u16 gPSysCmdCtrl_86[];			/*系统控制命令信息数组*/
extern u16 gPDebugCtrl_88[];			/*系统调试命令信息数组*/

extern u16 gPSmuGenParaR_102[];  		/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void PileGOperAuthCtrlTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆各主控操作权限控制任务(确保组权限与堆权限一致)
//注意事项	: 1s
//=============================================================================================
void PileGOperAuthCtrlTask(void)
{
	u16 auth = 0;
	static u8 sTime = 0;
	static u16 sHisAuth = eAuthL_User;

	auth = GetSysAuthorityInfoAPI(0);

	/*权限级别变化*/
	if(sHisAuth != auth)
	{
		//控制组切换权限
		PileSendDnGroupOperAuthCmdMsg_API(0xff, auth);

		//控制自身切换权限
		//PileSetSelfGroupOperAuthCmdAPI(auth);

		sTime++;
		if(sTime >= 3)
		{
			sHisAuth = auth;
		}
	}
	else
	{
		sTime = 0;
	}

	/*向组下发心跳帧*/
	PileSendDnGroupBreathMsg_API();

    //启用为扩展堆控
	if(1 == GetPileSysRunFlagAPI())
	{
		//模拟自身所在组接收到堆控心跳报文
		DiagRcvDevMsgAddAPI(gPSmuGenParaR_102[eSmuGenPara102_SmuDev]);
	}
}

/*=================================================================================================
** @Name      : void RcvPileWorkStateCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口 [10,12]按钮或内部口
** @Output    : void
** @Function  : 电池堆接收切换工作状态命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvPileWorkStateCmdAPI(u8 port)
{
    u8 cmd = 0;

    cmd = gPSysCmdCtrl_86[eSysCmdCtrl86_State];

    if((eCTRL_NORSTOP == cmd)                                       //正常停机命令
        || (eCTRL_FORCSTOP == cmd)                                  //强制停机命令
        || (eCTRL_EMERSTOP == cmd)                                  //紧急停机命令
        || (eCTRL_POWEROFF == cmd))                                 //关机命令
    {
    	/*产生事件*/
    	//EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 1, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), cmd, port);

    	TacSetPileWorkStateCtrlCmdAPI(cmd);                         //设置控制状态命令

        if((eMODE_EMERG == GetPileWorkModeAPI())                    //紧急模式
            || (eMODE_DEBUG == GetPileWorkModeAPI()))               //调试模式
        {
            //TacRecoverPileNorModeCtrlCmdAPI();                    //退出到正常模式(历史模式)
        }
    }
    else                                                            //启动命令
    {
    	/*产生事件*/
    	//EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 1, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), cmd, port);

    	TacSetPileWorkStateCtrlCmdAPI(cmd);                         //设置控制状态命令
    }
}

/*=================================================================================================
** @Name      : void RcvPileWorkModeCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口 [10,12]按钮或内部口
** @Output    : void
** @Function  : 电池堆接收切换工作模式切换命令处理函数
** @The notes : 通信口或按钮或内部命令
**===============================================================================================*/
void RcvPileWorkModeCmdAPI(u8 port)
{
    u8 cmd = 0;

    cmd = gPSysCmdCtrl_86[eSysCmdCtrl86_Mode];

    /*紧急模式需要高级权限,调试模式需要超级权限*/
    if(((eMODE_EMERG == cmd) && (eAuthL_Service != GetSysNowAuthorityAPI(port)) && (eAuthL_Admin != GetSysNowAuthorityAPI(port)))
		|| ((eMODE_DEBUG == cmd) && (eAuthL_Admin != GetSysNowAuthorityAPI(port))))
	{
    	return;
	}

    if((eWORK_IDLE == GetPileWorkStateAPI())                        //待命状态
		|| (eWORK_RUN == GetPileWorkStateAPI())                     //运行状态
		|| (eWORK_ERR == GetPileWorkStateAPI()))                    //故障状态
	{
    	/*产生事件*/
    	//EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 2, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), cmd, port);

    	TacSetPileWorkModeCtrlCmdAPI((e_WorkMode)cmd);              //设置模式命令
	}
}

/*=================================================================================================
** @Name      : void RcvPileSysClrCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口 [10,12]按钮或内部口
** @Output    : void
** @Function  : 电池堆接收切换工作状态命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvPileSysClrCmdAPI(u8 port)
{
    if(eCLR_NOWERR == gPSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])          /*清除当前故障命令*/
    {
    	TacClrPileCtrlErrFlagAPI();                                   //清除系统控制故障标志
    	TacClrPileProFlagAPI();                                       //清除故障保护标志
    }
//    else if(eCLR_FALCLIM == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])  /*复位充放电降功率限制命令*/
//    {
//        ResetCurrLimFallAPI();                                      /*复位充放电降功率限制步骤清零*/
//    }
//    else if(eCLR_CDHGLIM == gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd])  /*复位充放电截止限制命令*/
//    {
//        TacResetGroupBattStateAPI();                                /*复位充放电截止限制状态清零*/
//    }

	/*产生事件*/
	//EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 3, GetGSampOutCurrAPI(), GetSysNowAuthorityAPI(port), gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd], port);
}

#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
/*=================================================================================================
** @Name      : void RcvPileDebugOperCmdAPI(u8 index)
** @Input     : index:调试命令索引
** @Output    : void
** @Function  : 电池堆接收切换调试操作命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvPileDebugOperCmdAPI(u8 index)
{
    /*调试参数设置处理(上一条调试未执行完不能修改调试参数)*/
	if((0 == GetPileSysDebugStepAPI())
		&& (index < eDbgCmd88_Affirm))
	{
		gPDebugResult_82[index] = gPDebugCtrl_88[index];
	}
}

/*=================================================================================================
** @Name      : void RcvPileDebugAffirmCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口 10:按钮 11:内部
** @Output    : void
** @Function  : 电池堆接收调试确认命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvPileDebugAffirmCmdAPI(u8 port)
{
	//u8 id = 0;

    /*调试确认命令处理*/
    TacSetPileDebugAffirmCmdAPI(gPDebugCtrl_88[eDbgCmd88_Affirm]);

    /*if(gGDebugCtrl_88[eDbgCmd88_BMUid] <= 16)
    {
    	id = 1;
    }
    else
    {
    	id = 0;
    }*/

	/*产生事件*/
	//EventRcdDisplayUserAPI(eEVN_RCDSORT_DEBUG, id, gGDebugCtrl_88[eDbgCmd88_DbgType], gGDebugCtrl_88[eDbgCmd88_BalType], gGDebugCtrl_88[eDbgCmd88_BalStat], port);
}
#endif

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
