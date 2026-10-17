/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileTactInterface.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控电池堆策略接口模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicPara.h"
#include "PilePara.h"
#include "PileInfo.h"
#include "PileDnMsg.h"
#include "PileUserAPI.h"
#include "PileGroupRcv.h"
#include "PileDiagUser.h"
#include "PileDiagCheck.h"
#include "PileTactics.h"
#include "PileSysState.h"
#include "PileProtect.h"
#include "PileTactInterface.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gPDebugResult_82[];			/*调试结果状态信息数组*/
extern u16 gPDebugCtrl_88[];			/*系统调试命令信息数组*/

extern u16 gSysGenParaRO_100[];  		/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];  		/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void PileSetGroupWorkStateCtrlCmdHook(u8 group, u8 cmd)
//输入参数	: group:组号[0,15]  cmd:命令(0:正常停止 1:正常预充 2:正常启动 3:充电启动 4:放电启动 5:充电停机 6:放电停机 7:紧急停机 8:下电关机 9:上电开机)
//输出参数	: void
//函数功能	: 设置控制主控系统工作状态切换控制命令
//注意事项	: 包括发送报文和设置自身所在组
//=============================================================================================
void PileSetGroupWorkStateCtrlCmdHook(u8 group, u8 cmd)
{
    if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])//组号无效
        || (cmd >= eCTRL_SYSCMD))                     	//命令错误
    {
        return;
    }
    
    //如果是自身组直接控制
    if(group == GetSysSelfGroupIdAPI())
    {
    	PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_State, cmd);
    }
    //非自身组发通讯报文控制
    else
    {
        (void)PileSendDnGroupSysCtrlCmdMsg_API(group, eSysCmdCtrl86_State, cmd);
    }
}

//=============================================================================================
//函数名称	: void PileSetGroupWorkModeCtrlCmdHook(u8 group, u8 cmd)
//输入参数	: group:组号[0,15]  cmd:命令(0:自动 1:手动 2:紧急 5:调试)
//输出参数	: void
//函数功能	: 设置控制主控系统工作模式切换控制命令
//注意事项	: 包括发送报文和设置自身所在组
//=============================================================================================
void PileSetGroupWorkModeCtrlCmdHook(u8 group, u8 cmd)
{
    if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])//组号无效
        || (cmd >= eCTRL_SYSCMD))                        //命令错误
    {
        return;
    }
    
    //如果是自身组直接控制
    if(group == GetSysSelfGroupIdAPI())
    {
    	PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_Mode, cmd);
    }
    //非自身组发通讯报文控制
    else
    {
        (void)PileSendDnGroupSysCtrlCmdMsg_API(group, eSysCmdCtrl86_Mode, cmd);
    }
}

//=============================================================================================
//函数名称	: void PileSetGroupSysClrCtrlCmdHook(u8 group, u8 cmd)
//输入参数	: group:组号[0,15] cmd:命令(1:清除当前故障 2:清除当天故障 3:清除故障数目 4:清除运行计时 5:清除累计电量 6:清除故障记录 7:清除事件记录  8:清除故障录波 9:清除数据记录 10:恢复出厂设置 11:消除末端限流 12:消除充放截止)
//输出参数	: void
//函数功能	: 设置控制主控系统清除命令
//注意事项	: 包括发送报文和设置自身所在组
//=============================================================================================
void PileSetGroupSysClrCtrlCmdHook(u8 group, u8 cmd)
{
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])//组号无效
    {
        return;
    }
    
    //如果是自身组直接控制
    if(group == GetSysSelfGroupIdAPI())
    {
    	PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_ClrCmd, cmd);
    }
    //非自身组发通讯报文控制
    else
    {
        (void)PileSendDnGroupSysCtrlCmdMsg_API(group, eSysCmdCtrl86_ClrCmd, cmd);
    }
}

//=============================================================================================
//函数名称	: void PileSetGroupUserCtrlCmdHook(u8 group, u8 index, u8 len, u16 *cmd)
//输入参数	: group:组号[0,15] index:起始子索引[0,5] len:子索引长度[1,6] cmd:控制命令
//输出参数	: void
//函数功能	: 设置控制主控用户控制信息命令
//注意事项	: 包括发送报文和设置自身所在组
//=============================================================================================
void PileSetGroupUserCtrlCmdHook(u8 group, u8 index, u8 len, u16 *cmd)
{
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])//组号无效
    {
        return;
    }

    //如果是自身组直接控制
    if(group == GetSysSelfGroupIdAPI())
    {
    	PileSetSelfGroupUserCtrlCmdAPI(index, len, cmd);
    }
    //非自身组发通讯报文控制
    else
    {
        (void)PileSendDnGroupUserCtrlCmdMsg_API(group, index, len, cmd);
    }
}

//=============================================================================================
//函数名称	: void PileSetGroupSysDebugCtrlCmdHook(u8 group, u8 index, u8 len, u16 *cmd)
//输入参数	: group:组号[0,15] index:起始子索引[0,7] len:子索引长度[1,8] cmd:调试命令
//输出参数	: void
//函数功能	: 设置控制主控系统调试信息命令
//注意事项	: 包括发送报文和设置自身所在组
//=============================================================================================
void PileSetGroupSysDebugCtrlCmdHook(u8 group, u8 index, u8 len, u16 *cmd)
{
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])//组号无效
    {
        return;
    }

    //如果是自身组直接控制
    if(group == GetSysSelfGroupIdAPI())
    {
    	PileSetSelfGroupSysDebugCmdAPI(index, len, cmd);
    }
    //非自身组发通讯报文控制
    else
    {
        (void)PileSendDnGroupSysDebugCmdMsg_API(group, index, len, cmd);
    }
}

//=============================================================================================
//函数名称	: void PileBroadAllGroupNorStopCmdHook(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 堆控广播所有电池组正常停机命令
//注意事项	: 发送广播地址(包括发送报文和设置自身所在组)
//=============================================================================================
void PileBroadAllGroupNorStopCmdHook(void)
{
    /*
    u8 group = 0;
    
    //单独逐组发送
    for(group = 0; group < gSysGenParaRO_100[eSGenPara100_AllGNum]; group++)
    {
        //如果是自身组直接控制
        if(group == GetSysSelfGroupIdAPI())
        {
            PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_State, eCTRL_NORSTOP);
        }
        //非自身组发通讯报文控制
        else
        {
            (void)PileSendDnGroupSysCtrlCmdMsg_API(group, eSysCmdCtrl86_State, eCTRL_NORSTOP);
        }
    }
    */
    
    //广播报文发送
	PileSendDnGroupSysCtrlCmdMsg_API(0xff, eSysCmdCtrl86_State, eCTRL_NORSTOP);
    
    //停止自身
	PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_State, eCTRL_NORSTOP);
}

//=============================================================================================
//函数名称	: void PileBroadAllGroupForcibStopCmdHook(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 堆控广播所有电池组强制停机命令
//注意事项	: 发送广播地址(包括发送报文和设置自身所在组)
//=============================================================================================
void PileBroadAllGroupForcibStopCmdHook(void)
{
    /*
    u8 group = 0;

    //单独逐组发送
    for(group = 0; group < gSysGenParaRO_100[eSGenPara100_AllGNum]; group++)
    {
        //如果是自身组直接控制
        if(group == GetSysSelfGroupIdAPI())
        {
            PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_State, eCTRL_FORCSTOP);
        }
        //非自身组发通讯报文控制
        else
        {
            (void)PileSendDnGroupSysCtrlCmdMsg_API(group, eSysCmdCtrl86_State, eCTRL_FORCSTOP);
        }
    }
    */

    //广播报文发送
	PileSendDnGroupSysCtrlCmdMsg_API(0xff, eSysCmdCtrl86_State, eCTRL_FORCSTOP);

    //停止自身
	PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_State, eCTRL_FORCSTOP);
}

//=============================================================================================
//函数名称	: void PileBroadAllGroupEmergStopCmdHook(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 堆控广播所有电池组紧急停机命令
//注意事项	: 发送广播地址(包括发送报文和设置自身所在组)
//=============================================================================================
void PileBroadAllGroupEmergStopCmdHook(void)
{
    /*
    u8 group = 0;
    
    //单独逐组发送
    for(group = 0; group < gSysGenParaRO_100[eSGenPara100_AllGNum]; group++)
    {
        //如果是自身组直接控制
        if(group == GetSysSelfGroupIdAPI())
        {
            PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_State, eBSUSYSCMD_EMERGEN);
        }
        //非自身组发通讯报文控制
        else
        {
            (void)PileSendDnGroupSysCtrlCmdMsg_API(group, eSysCmdCtrl86_State, eCTRL_EMERSTOP);
        }
    }
    */
    
    //广播报文发送
	PileSendDnGroupSysCtrlCmdMsg_API(0xff, eSysCmdCtrl86_State, eCTRL_EMERSTOP);
    
    //停止自身
	PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_State, eCTRL_EMERSTOP);
}

//=============================================================================================
//函数名称	: void PileBroadAllGroupPowerOnCmdHook(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 堆控广播所有电池组上电开机命令
//注意事项	: 发送广播地址(包括发送报文和设置自身所在组)
//=============================================================================================
void PileBroadAllGroupPowerOnCmdHook(void)
{
    /*
    //单独逐组发送
    for(group = 0; group < gSysGenParaRO_100[eSGenPara100_AllGNum]; group++)
    {
        //如果是自身组直接控制
        if(group == GetSysSelfGroupIdAPI())
        {
            PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_State, eCTRL_POWERON);
        }
        //非自身组发通讯报文控制
        else
        {
            (void)PileSendDnGroupSysCtrlCmdMsg_API(group, eSysCmdCtrl86_State, eCTRL_POWERON);
        }
    }
    */

    //广播报文发送
    PileSendDnGroupSysCtrlCmdMsg_API(0xff, eSysCmdCtrl86_State, eCTRL_POWERON);

    //自身下电关机
    PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_State, eCTRL_POWERON);
}

//=============================================================================================
//函数名称	: void PileBroadAllGroupPowerOffCmdHook(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 堆控广播所有电池组下电关机命令
//注意事项	: 需多次调用(其他组下电才自身下电),发送广播地址(包括发送报文和设置自身所在组)
//=============================================================================================
void PileBroadAllGroupPowerOffCmdHook(void)
{
    u8 num = 0;
    u8 group = 0;
    
    /*
    //单独逐组发送
    for(group = 0; group < gSysGenParaRO_100[eSGenPara100_AllGNum]; group++)
    {
        //如果是自身组直接控制
        if(group == GetSysSelfGroupIdAPI())
        {
            PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_State, eCTRL_POWEROFF);
        }
        //非自身组发通讯报文控制
        else
        {
            (void)PileSendDnGroupSysCtrlCmdMsg_API(group, eSysCmdCtrl86_State, eCTRL_POWEROFF);
        }
    }
    */
    
    //广播报文发送
    PileSendDnGroupSysCtrlCmdMsg_API(0xff, eSysCmdCtrl86_State, eCTRL_POWEROFF);
    
    //计算已关闭电池组数
    for(group = 0; group < gSysGenParaRO_100[eSGenPara100_AllGNum]; group++)
    {
        //未接收到信息
        if((0 == GetPGroupCanRcvStateAPI(group))
            || (group == GetSysSelfGroupIdAPI()))
        {
            num++;
        }
    }
    
    //其他组均下电自身才下电
    if(num >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        //自身下电关机
    	PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_State, eCTRL_POWEROFF);
    }
}

//=============================================================================================
//函数名称	: void PileBroadAllGroupWorkModeCmdHook(void)
//输入参数	: mode:目标模式
//输出参数	: void
//函数功能	: 堆控广播所有电池组切换工作模式命令
//注意事项	: 发送广播地址(包括发送报文和设置自身所在组)
//=============================================================================================
void PileBroadAllGroupWorkModeCmdHook(u8 mode)
{
    /*
    u8 group = 0;

    //单独逐组发送
    for(group = 0; group < gSysGenParaRO_100[eSGenPara100_AllGNum]; group++)
    {
        //如果是自身组直接控制
        if(group == GetSysSelfGroupIdAPI())
        {
            PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_Mode, mode);
        }
        //非自身组发通讯报文控制
        else
        {
            (void)PileSendDnGroupSysCtrlCmdMsg_API(group, eSysCmdCtrl86_Mode, mode);
        }
    }
    */

    //广播报文发送
	PileSendDnGroupSysCtrlCmdMsg_API(0xff, eSysCmdCtrl86_Mode, mode);

    //控制自身
	PileSetSelfGroupSysCtrlCmdAPI(eSysCmdCtrl86_Mode, mode);
}

//=============================================================================================
//函数名称	: void PileBroadAllGroupUserCmdHook(u8 index, u8 len, u16 *cmd)
//输入参数	: index:起始子索引[0,5] len:子索引长度[1,6] cmd:用户控制命令
//输出参数	: void
//函数功能	: 堆控广播所有电池组设置用户控制信息命令
//注意事项	: 发送广播地址(包括发送报文和设置自身所在组)
//=============================================================================================
void PileBroadAllGroupUserCmdHook(u8 index, u8 len, u16 *cmd)
{
    /*
    u8 group = 0;

    //单独逐组发送
    for(group = 0; group < gSysGenParaRO_100[eSGenPara100_AllGNum]; group++)
    {
        //如果是自身组直接控制
        if(group == GetSysSelfGroupIdAPI())
        {
            PileSetSelfGroupUserCtrlCmdAPI(index, len, cmd);
        }
        //非自身组发通讯报文控制
        else
        {
            (void)PileSendDnGroupUserCtrlCmdMsg_API(group, index, len, cmd);
        }
    }
    */

    //广播报文发送
	PileSendDnGroupUserCtrlCmdMsg_API(0xff, index, len, cmd);

    //控制自身
	PileSetSelfGroupUserCtrlCmdAPI(index, len, cmd);
}

//=============================================================================================
//函数名称	: void PileBroadAllGroupDebugCmdHook(u8 index, u8 len, u16 *cmd)
//输入参数	: index:起始子索引[0,7] len:子索引长度[1,8] cmd:调试命令
//输出参数	: void
//函数功能	: 堆控广播所有电池组设置调试信息命令
//注意事项	: 发送广播地址(包括发送报文和设置自身所在组)
//=============================================================================================
void PileBroadAllGroupDebugCmdHook(u8 index, u8 len, u16 *cmd)
{
    /*
    u8 group = 0;

    //单独逐组发送
    for(group = 0; group < gSysGenParaRO_100[eSGenPara100_AllGNum]; group++)
    {
        //如果是自身组直接控制
        if(group == GetSysSelfGroupIdAPI())
        {
            PileSetSelfGroupSysDebugCmdAPI(index, len, cmd);
        }
        //非自身组发通讯报文控制
        else
        {
            (void)PileSendDnGroupSysDebugCmdMsg_API(group, index, len, cmd);
        }
    }
    */

    //广播报文发送
	PileSendDnGroupSysDebugCmdMsg_API(0xff, index, len, cmd);

    //控制自身
	PileSetSelfGroupSysDebugCmdAPI(index, len, cmd);
}

/*=================================================================================================
 ** @Name      : u8 PileGetUserReqStopStateHook(void)
 ** @Input     : void
 ** @Output    : 请求电池堆停机状态 0:未请求 1:请求
 ** @Function  : 获取用户请求电池堆停机状态
 ** @The notes : 运行状态需要停机
 **===============================================================================================*/
u8 PileGetUserReqStopStateHook(void)
 {
	static u32 sCanTime = 0;

	/*外部通信正常*/
	if(0 == GetPilePcsCanOffFlag())
	{
		/*记录时间*/
		sCanTime = GetSysSecondTimeAPI();
	}
	/*外部通信中断超时*/
	else if((gPSmuGenParaR_102[eSmuGenPara102_ComProT] > 0)
		&& (ABS(GetSysSecondTimeAPI(), sCanTime) >= gPSmuGenParaR_102[eSmuGenPara102_ComProT]))
	{
		return(1);
	}

    /*断路器断闸未充放电
    if((0 == GetPileIsoIoReadyStateAPI())
    	&& (eCURR_IDLE == GetPileChgStateAPI()))
	{
		//请求停机
		return(1);
	}*/

    /*发生故障保护*/
    if(TacGetPileHappenProFlagAPI() > 0)
    {
		//请求停机
		return(1);
    }

    return(0);
 }

/*=================================================================================================
 ** @Name      : u8 PileGetSysFitStartStateHook(void)
 ** @Input     : void
 ** @Output    : 电池组适合启动状态 0:不适合 1:适合
 ** @Function  : 获取电池组适合启动状态
 ** @The notes : 空闲状态允许启动
 **===============================================================================================*/
u8 PileGetSysFitStartStateHook(void)
 {
	/*请求停机*/
	if(1 == PileGetUserReqStopStateHook())
	{
		//不适合
		return(0);
	}

	/*充放电保护*/
    if((GetPileAskSysProState() > 0)
	    || (eErr_SysPro == GetPileNowErrSortLevelAPI())
        || (ePGPRO_STA_SYSPRO == GetPileAllGProStateAPI()))
	{
		//不适合
		return(0);
	}

    /*有使能组初始化(直流端未就绪或异常)*/
    if(0 == GetPileAllGSysStateAPI())
    {
		//不适合
		return(0);
    }

    /*电池组间压差过大(不允许自动接入)*/
    if((GetPileGSumVDiffFlag() > 0)
        && (0 == gSysGenParaRO_100[eSGenPara100_AutConn]))
    {
		//不适合
		return(0);
    }

    /*直流端内外压差过大
    if(GetPileOutSumVDiffFlag() > 0)
    {
		//不适合
		return(0);
    }*/

    /*断路器断闸
    if(0 == GetPileIsoIoReadyStateAPI())
	{
		//不适合
		return(0);
	}*/

    /*按钮停机*/
	if(1 == GetPileButStopReqFlagAPI())
	{
		//不适合
		return(0);
	}

    return(1);
 }

//=============================================================================================
//函数名称	: u8 PileCheckInitStateResultHook(void)
//输入参数	: void
//输出参数	: ok:通过 wait等待 fault:异常 
//函数功能	: 判断电池堆开机自检状态结果
//注意事项	: 用户完善
//=============================================================================================
u8 PileCheckInitStateResultHook(void)
{
    //堆控管控粘连和绝缘检测使能
    #if(1 == PILE_DIAG_F_EN)
    //u16 cmd = 2;
    #endif
    u8 index = 0;
    static u8 sState = 0;
    static u16 sWaitTime = 0;
    
    switch(sState)
    {
        //等待所以电池组上电
        case 0:
            if(0 == GetPileGPowerOnStateAPI())
            {
                sWaitTime = 0;
                return(FunR_Wait);
            }

            //广播所有电池组开机(避免电池组误进入关机状态)
            if(0 == sWaitTime)
            {
            	PileBroadAllGroupPowerOnCmdHook();
            }
            //广播所有电池组停机(避免电池组有运行状态)
            else if(1 == sWaitTime)
            {
            	PileBroadAllGroupNorStopCmdHook();
            }
            //堆控管控粘连和绝缘检测使能
            #if(1 == PILE_DIAG_F_EN)
            //广播禁止绝缘检测命令(防止电池组均在检测相互干扰)
            else if(2 == sWaitTime)
            {
                //PileBroadAllGroupUserCmdHook(5, 1, &cmd);
            }
            #endif

            sWaitTime++;
            if((sWaitTime <= gPSmuGenParaR_102[eSmuGenPara102_InitWaitT]) //等待主控信息
                && (FALSE == GetPAllGroupCanRcvStateAPI()))     //未接收到全部主控信息
            {
                if(1 == sWaitTime)
                {
                    for(index = 0; index < gSysGenParaRO_100[eSGenPara100_AllGNum]; index++)
                    {
                    	ClrPileGroupInfoAPI(index);             //初始化各组BCMU信息
                    }
                }
            }
            else                                                //等待组信息完成
            {
                sWaitTime = 0;
                sState = 1;   
            }
            return(FunR_Wait);
            
        //确认接收到所有电池组信息
        case 1:
            sWaitTime++;
            
            //检测使能组是否接收到信息
            for(index = 0; index < gSysGenParaRO_100[eSGenPara100_AllGNum]; index++)
            {
                if((1 == GetPGroupEnStateAPI(index))             //电池组使能
                    && (0 == GetPGroupCanRcvStateAPI(index)))    //未接收到CAN信息
                {
                    //等待判断出CAN断线
                    if((sWaitTime < 3000)
                        && (0 == GetPGroupCanOffStateAPI(index)))//未判断出CAN断线
                    {
                        return(FunR_Wait);
                    }
                }
            }
            
            //收到所有电池组信息或已判断出通讯故障
            sState = 2;
            sWaitTime = 0;
            return(FunR_Wait);
            
        //等待电池组初始化完成及检测是否发生故障
        case 2:
            sWaitTime++;
            
            //系统电池组整体状态故障或有系统有故障或发生需要整体保护的故障
            if((2 == GetPileAllGSysStateAPI())
                || (1 == GetPileGErrSysProStateAPI())
                || (eErr_SysErr == GetPileNowErrSortLevelAPI()))
            {
                sState = 6;                                     //进入故障状态
                sWaitTime = 0;
            }
            else if((0 != GetPileAllGSysStateAPI())             //电池组总状态初始化自检完成
                || (sWaitTime >= gPSmuGenParaR_102[eSmuGenPara102_InitWaitT]))//等待自检超时
            {
                sState = 3;                                     //电池组接触器检测
                sWaitTime = 0;
            }
            return(FunR_Wait);
            
        //判断电池组接触器是否全断开
        case 3:
            //系统电池组整体状态故障或有系统有故障或发生需要整体保护的故障
            if((2 == GetPileAllGSysStateAPI())
                || (1 == GetPileGErrSysProStateAPI())
                || (eErr_SysErr == GetPileNowErrSortLevelAPI()))
            {
                sState = 6;                                     //初始化失败
                sWaitTime = 0;
            }
            else if((0 == GetPAllGroupRunStateAPI())            //无电池组在线运行
            	&& (0 == GetPileGSwhStateErrFlag()))            //接触器状态正常
            {
                sState = 5;                                     //初始化通过
            }
            else                                                //有电池组接触器闭合
            {
                sState = 4;                                     //控制断开接触器
            }
            return(FunR_Wait);
            
        //控制电池组接触器断开
        case 4:
            sWaitTime++;
            if(sWaitTime >= 100)                                //接触器断开超时
            {
                sState = 6;
            }
            else
            {
                if(1 == GetPGroupAllPreSafeCStateAPI())         //预充接触器电流安全
                {
                    for(index = 0; index < gSysGenParaRO_100[eSGenPara100_AllGNum]; index++)
                    {
                        if(0 == GetPGroupMainSafeCStateAPI(index))//组电流不安全
                        {
                            return(FunR_Wait);
                        }
                    }
                    
                    PileBroadAllGroupNorStopCmdHook();          //停止所有电池组
                    sState = 3;  
                }
            }
            return FunR_Wait;
            
        //初始化自检通过
        case 5:
            sState = 0;
            sWaitTime = 0;
            return(FunR_Ok);
            
        //初始化自检失败
        case 6:
            sState = 0;
            sWaitTime = 0;
            return(FunR_Fault);
            
        default:
            sState = 0;
            break;
    }
   
    return(FunR_Ok);
}

//=============================================================================================
//函数名称	: u8 PileCheckIdleStateResultHook(void)
//输入参数	: void
//输出参数	: ok:通过 wait等待 fault:异常
//函数功能	: 判断电池堆空闲状态结果
//注意事项	: 用户完善
//=============================================================================================
u8 PileCheckIdleStateResultHook(void)
{
    //系统电池组整体状态故障或有系统有故障或发生需要整体保护的故障
    if((2 == GetPileAllGSysStateAPI())
        || (1 == GetPileGErrSysProStateAPI())
        || (eErr_SysErr == GetPileNowErrSortLevelAPI()))
    {
        return(FunR_Fault);
    }
    
    return(FunR_Wait);
}

//=============================================================================================
//函数名称	: u8 PileCheckStartStateResultHook(void)
//输入参数	: void
//输出参数	: ok:启动完成 wait等待 failed:启动失败
//函数功能	: 判断电池堆启动状态结果
//注意事项	: 用户完善
//=============================================================================================
u8 PileCheckStartStateResultHook(void)
{
    //系统电池组整体状态故障或有系统有故障或发生需要整体保护的故障
    if((2 == GetPileAllGSysStateAPI())
        || (1 == GetPileGErrSysProStateAPI())
        || (eErr_SysErr == GetPileNowErrSortLevelAPI()))
    {
        return(FunR_Fault);
    }
    
    return(FunR_Ok);
}

//=============================================================================================
//函数名称	: u8 PileCheckRunStateResultHook(void)
//输入参数	: void
//输出参数	: ok:通过 wait等待 fault:异常
//函数功能	: 判断电池堆运行状态结果
//注意事项	: 用户完善
//=============================================================================================
u8 PileCheckRunStateResultHook(void)
{
    //系统电池组整体状态故障或有系统有故障或发生需要整体保护的故障
    if((2 == GetPileAllGSysStateAPI())
        || (1 == GetPileGErrSysProStateAPI())
        || (eErr_SysErr == GetPileNowErrSortLevelAPI()))
    {
        return(FunR_Fault);
    }

    //系统电池组保护接触器全断开无运行组则转为空闲状态
    if(0 == GetPileRunGroupNumAPI())
    {
        return(FunR_Ok);
    }

    return(FunR_Wait);
}

//=============================================================================================
//函数名称	: u8 PileCheckErrStateResultHook(void)
//输入参数	: void
//输出参数	: ok:通过可恢复 wait等待 fault:异常
//函数功能	: 判断电池堆故障状态恢复结果
//注意事项	: 用户完善
//=============================================================================================
u8 PileCheckErrStateResultHook(void)
{
    static u8 sErrTime = 0;

    sErrTime++;
    //故障15s后清除非自动消除故障
    if(sErrTime >= 150)
    {
        //清除启停控制故障
    	TacClrPileCtrlErrFlagAPI();
    }
    
    //系统电池组整体状态故障或有系统有故障或发生需要整体保护的故障
    if((2 == GetPileAllGSysStateAPI())
        || (1 == GetPileGErrSysProStateAPI())
        || (eErr_SysErr == GetPileNowErrSortLevelAPI()))
    {
        return(FunR_Fault);
    }
    
    /*故障状态暂不新增接触器状态异常故障，保持故障状态
    if(CheckGroupSwhExpStateInDTC() == e_GSwhState_ERR) //组接触器状态异常(未全断开)
    {
        return(FunR_Fault);
    }
    */
    
    if(GetPileRunGroupNumAPI() > 0)                     //有电池组在线接触器未断开
    {
        return(FunR_Fault);                             //保持故障状态
    }

    if(sErrTime <= 150)
    {
        return(FunR_Wait);                              //延时15s恢复系统状态
    }
    sErrTime = 0;
    
    return(FunR_Ok);
}

//=============================================================================================
//函数名称	: u8 PileCheckStopStateResultHook(void)
//输入参数	: void
//输出参数	: ok:停止成功 wait等待 fault:异常
//函数功能	: 判断电池堆停机状态
//注意事项	: 用户完善
//=============================================================================================
u8 PileCheckStopStateResultHook(void)
{
    //系统电池组整体状态故障或有系统有故障或发生需要整体保护的故障
    if((2 == GetPileAllGSysStateAPI())
        || (1 == GetPileGErrSysProStateAPI())
        || (eErr_SysErr == GetPileNowErrSortLevelAPI()))
    {
        return(FunR_Fault);
    }

    return(FunR_Ok);
}

//=============================================================================================
//函数名称	: u8 PileCheckOffStateResultHook(void)
//输入参数	: void
//输出参数	: ok:关机成功 wait等待 fault:异常
//函数功能	: 判断电池堆关机状态结果
//注意事项	: 用户完善
//=============================================================================================
u8 PileCheckOffStateResultHook(void)
{
    return(FunR_Wait);
}

/*=================================================================================================
 ** @Name      : void TacPileWorkModeChangDealHook(u8 befMode, u8 aftMode)
 ** @Input     : befState:前置状态 aftState:后置状态
 ** @Output    : void
 ** @Function  : 电池堆工作模式变换处理函数
 ** @The notes :
 **===============================================================================================*/
void TacPileWorkModeChangDealHook(u8 befMode, u8 aftMode)
 {
	/*进入特殊模式需要清除故障1次*/
	if((eMODE_EMERG != befMode) && (eMODE_DEBUG != befMode)           /*前置非特殊模式*/
		&& ((eMODE_EMERG == aftMode) || (eMODE_DEBUG == aftMode)))    /*后置特殊模式*/
	{
		TacClrPileCtrlErrFlagAPI();                                   //清除系统控制故障标志
		TacClrPileProFlagAPI();                                       //清除故障保护标志
	}

    /*触发发送一帧状态信息报文*/
	//SendCanGroupSystemStateInfoMsg();
 }

#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
/*=================================================================================================
 ** @Name      : u8 TacCheckPileGDebugResultHook(void)
 ** @Input     : void
 ** @Output    : 电池组调试结果状态 0:未调试 1:调试成功 2:非调试模式 3:调试参数错误 4:通讯失败
 ** @Function  : 电池堆调试电池组结果状态
 ** @The notes :
 **===============================================================================================*/
u8 TacCheckPileGDebugResultHook(void)
 {
	u8 i = 0;
	u8 j = 0;
	u8 num = 0;

	/*调试全部电池组*/
	if(0 == gPDebugResult_82[eDbgRes82_BMUid])
	{
		for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
		{
			/*只检测调试模式组*/
			if(eMODE_DEBUG == GetPGroupWorkModeAPI(i))
			{
				/*计算调试模式组数*/
				num++;

				/*组调试结果成功*/
				if(eDBGRES_SUCCE == GetPGroupDebugResultAPI(i, eDbgRes82_DbgResult))
				{
					/*只检测写入信息(不检测读取EEPROM值)*/
					for(j = 0; j < eDbgRes82_RDataLW; j++)
					{
						/*组调试参数未更新成功*/
						if(gPDebugResult_82[j] != GetPGroupDebugResultAPI(i, j))
						{
							/*未调试*/
							return(eDBGRES_UNEXE);
						}
					}
				}
				else
				{
					return(GetPGroupDebugResultAPI(i,  eDbgRes82_DbgResult));
				}
			}

			/*调试读EEPROM则更新读取到的值*/
			if(((eDBGCMD_BSUEEP == gPDebugResult_82[eDbgRes82_DbgType])       	//从控配置参数读写调试命令
		        || (eDBGCMD_BMUEEP == gPDebugResult_82[eDbgRes82_DbgType]))    	//主控配置参数读写调试命令
				&& (1 == gPDebugCtrl_88[eDbgCmd88_Affirm]))						//读确认
			{
				gPDebugResult_82[eDbgRes82_RDataLW] = GetPGroupDebugResultAPI(i, eDbgRes82_RDataLW);
				gPDebugResult_82[eDbgRes82_RDataHW] = GetPGroupDebugResultAPI(i, eDbgRes82_RDataHW);
			}
		}

		if(0 == num)
		{
			/*非调试模式*/
			return(eDBGRES_NOMOD);
		}
	}
	/*调试单组电池组*/
	else if(gPDebugResult_82[eDbgRes82_BMUid] <= gSysGenParaRO_100[eSGenPara100_AllGNum])
	{
		/*组处于调试模式*/
		if(eMODE_DEBUG == GetPGroupWorkModeAPI(gPDebugResult_82[eDbgRes82_BMUid] - 1))
		{
			/*组调试结果成功*/
			if(eDBGRES_SUCCE == GetPGroupDebugResultAPI((gPDebugResult_82[eDbgRes82_BMUid] - 1), eDbgRes82_DbgResult))
			{
				/*只检测写入信息(不检测读取EEPROM值)*/
				for(j = 0; j < eDbgRes82_RDataLW; j++)
				{
					/*组调试参数未更新成功*/
					if(gPDebugResult_82[j] != GetPGroupDebugResultAPI((gPDebugResult_82[eDbgRes82_BMUid] - 1), j))
					{
						/*未调试*/
						return(eDBGRES_UNEXE);
					}
				}
			}
			else
			{
				return(GetPGroupDebugResultAPI((gPDebugResult_82[eDbgRes82_BMUid] - 1), eDbgRes82_DbgResult));
			}

			/*调试读EEPROM则更新读取到的值*/
			if(((eDBGCMD_BSUEEP == gPDebugResult_82[eDbgRes82_DbgType])       	//从控配置参数读写调试命令
		        || (eDBGCMD_BMUEEP == gPDebugResult_82[eDbgRes82_DbgType]))    	//主控配置参数读写调试命令
				&& (1 == gPDebugCtrl_88[eDbgCmd88_Affirm]))						//读确认
			{
				gPDebugResult_82[eDbgRes82_RDataLW] = GetPGroupDebugResultAPI((gPDebugResult_82[eDbgRes82_BMUid] - 1), eDbgRes82_RDataLW);
				gPDebugResult_82[eDbgRes82_RDataHW] = GetPGroupDebugResultAPI((gPDebugResult_82[eDbgRes82_BMUid] - 1), eDbgRes82_RDataHW);
			}
		}
		else
		{
			/*非调试模式*/
			return(eDBGRES_NOMOD);
		}
	}
	/*组号错误*/
	else
	{
		/*参数错误*/
		return(eDBGRES_ERRPA);
	}

	/*调试成功*/
	return(eDBGRES_SUCCE);
 }
#endif

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
