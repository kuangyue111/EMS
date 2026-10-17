/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileSysMode.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控工作模式管控模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicPara.h"
#include "PileParaRd.h"
#include "PileParaWr.h"
#include "PilePara.h"
#include "PublicInfo.h"
#include "PileInfo.h"
#include "PileGroupRcv.h"
#include "PileSysDebug.h"
#include "PileTactUser.h"
#include "PileTactInterface.h"
#include "PileSysMode.h"

#include "ParaInterface.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u16 sPileHisNorMode = 0;         /*原始正常模式 0自动模式 1手动模式*/
static u32 sRcvUnorModeTime = 0;        /*接收进入非正常模式时间 1ms*/

extern u16 gPSysStateInfo_50[];			/*系统状态信息数组*/
extern u16 gPSysCmdCtrl_86[];			/*系统控制命令信息数组*/

extern u16 gSysGenParaRO_100[];  		/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];  		/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
//=============================================================================================
//函数名称	: static u8 CheckGroupWorkModeFitState(u8 mode)
//输入参数	: mode:目标模式
//输出参数	: TRUE:正常 FALSE:异常
//函数功能	: 堆控检测对应模式下电池组工作模式是否正常
//注意事项	: 堆控自动/手动模式下确保主控处于手动模式
//=============================================================================================
static u8 CheckGroupWorkModeFitState(u8 mode);

//=============================================================================================
//函数名称	: static void PileAutoModeSysCtrlDeal(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 堆控自动模式下控制任务处理
//注意事项	: 自启动控制
//=============================================================================================
static void PileAutoModeSysCtrlDeal(void);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void PileWorkModeInfoInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池堆工作模式信息初始化
** @The notes : 读EEPROM写入值
**===============================================================================================*/
void PileWorkModeInfoInit(void)
{
	u16 data[2] = {0};

	/*读存储值设置自动模式还是手动模式*/
	if(TRUE == ParaReadStorePileWorkModeAPI(data, 2))
	{
        gPSysStateInfo_50[ePStateInfo50_SysMod] = data[1];
	}
	/*读设置值失败*/
	else
	{
		/*重写默认值*/
		data[0] = PARA_CFG_FLAG_U16;
		data[1] = gPSysStateInfo_50[ePStateInfo50_SysMod];
		ParaWriteStorePileWorkModeAPI(1, &data[1], 1);
		ParaWriteStorePileWorkModeAPI(0, &data[0], 1);
	}

	sPileHisNorMode = gPSysStateInfo_50[ePStateInfo50_SysMod];
}

//=============================================================================================
//函数名称	: void PileWorkModeCheckTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆工作模式检测任务
//注意事项	: 100ms任务
//=============================================================================================
void PileWorkModeCheckTask(void)
{
	u8 mode = 0;
	static u8 sState = 0;
	static u8 sErrTime = 0;

	mode = gPSysStateInfo_50[ePStateInfo50_SysMod];

    switch(sState)
    {
		//检测电池组是否到目标模式
    	case 0:
        	//主控未全部进入目标模式
			if(FALSE == CheckGroupWorkModeFitState(mode))
    		{
    			sErrTime++;
    			sState = 1;
    		}
        	//主控全部进入目标模式
    		else
    		{
				sState = 2;
    			sErrTime = 0;
    		}
    		break;

    	//控制电池组进入目标模式
        case 1:
        	//堆控自动模式主控也要为手动模式才能控制
        	if(eMODE_AUTO == mode)
        	{
        		mode = eMODE_HAND;
        	}

        	//广播发送命令,确保电池组均处于目标模式
        	PileBroadAllGroupWorkModeCmdHook(mode);

        	//连续控制10次未切换就绪
        	if(sErrTime >= 10)
			{
            	//直接进入相应控制
				sState = 2;
				sErrTime = 10;
				//sErrTime = 0;
			}
			else
			{
            	//返回再次检测
				sState = 0;
			}
            break;

		//对应模式下进行相应控制
		case 2:
			//处于自动模式(自启动控制)
			if(eMODE_AUTO == mode)
			{
				//自启动控制
				PileAutoModeSysCtrlDeal();
			}
			//处于手动模式(无需处理)
			else if(eMODE_HAND == mode)
			{
				;
			}
			//处于紧急模式(自启动控制)
			else if(eMODE_EMERG == mode)
			{
				/*特殊模式保持10min后自动退出*/
				if(ABS(BSPGetOSMillSecTimeAPI(), sRcvUnorModeTime) >= PILE_KEEP_UNORMODE_T)
				{
					//切换到原始正常模式命令
					gPSysCmdCtrl_86[eSysCmdCtrl86_Mode] = sPileHisNorMode;
					RcvPileWorkModeCmdAPI(eCMDPORT_INTE);
				}
				else
				{
					//自启动控制
					PileAutoModeSysCtrlDeal();
				}
			}
			//处于调试模式(主控调试控制)
			else if(eMODE_DEBUG == mode)
			{
				/*特殊模式保持10min后自动退出*/
				if(ABS(BSPGetOSMillSecTimeAPI(), sRcvUnorModeTime) >= PILE_KEEP_UNORMODE_T)
				{
					//切换到原始正常模式命令
					gPSysCmdCtrl_86[eSysCmdCtrl86_Mode] = sPileHisNorMode;
					RcvPileWorkModeCmdAPI(eCMDPORT_INTE);
				}
				else
				{

					#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
					//电池组调试控制
					PileDebugModeGroupCtrlDeal();
					#endif
				}
			}
			//无效模式
			else
			{
				//恢复自动模式
				TacSetPileWorkModeCtrlCmdAPI(eMODE_AUTO);
			}
			sState = 0;
			break;
            
        default:
            sState = 0;
            break;
    }
}

//=============================================================================================
//函数名称	: void TacSetPileWorkModeCtrlCmdAPI(u8 mode)
//输入参数	: mode:目标模式 0:自动 1:手动 2:紧急 5:调试
//输出参数	: void
//函数功能	: 设置系统工作模式切换控制命令
//注意事项	:
//=============================================================================================
void TacSetPileWorkModeCtrlCmdAPI(u8 mode)
{
	u16 data[2] = {0};

	if(mode >= eMODE_STATE)
    {
        return;
    }

    /*正处于调试模式或紧急模式*/
    if((eMODE_DEBUG == gPSysStateInfo_50[ePStateInfo50_SysMod])
    	|| (eMODE_EMERG == gPSysStateInfo_50[ePStateInfo50_SysMod]))
    {
        /*退出调试模式或紧急模式到原来模式*/
    	if(eMODE_AUTO == mode)
    	{
    		/*模式切换用户处理*/
    		TacPileWorkModeChangDealHook((u8)gPSysStateInfo_50[ePStateInfo50_SysMod], (u8)sPileHisNorMode);

    		/*只能由特殊模式切换至原来正常模式*/
    		gPSysStateInfo_50[ePStateInfo50_SysMod] = sPileHisNorMode;
    	}
        /*退出调试模式或紧急模式到手动模式*/
    	else if(eMODE_HAND == mode)
    	{
    		/*原来非手动模式*/
    		if(eMODE_HAND != sPileHisNorMode)
    		{
    			/*记录新模式*/
    			data[1] = mode;
    			data[0] = PARA_CFG_FLAG_U16;
    			ParaWriteStorePileWorkModeAPI(1, &data[1], 1);
    			ParaWriteStorePileWorkModeAPI(0, &data[0], 1);
    		}

    		/*模式切换用户处理*/
    		TacPileWorkModeChangDealHook((u8)gPSysStateInfo_50[ePStateInfo50_SysMod], (u8)mode);

    		/*只能由特殊模式切换至正常模式*/
    		gPSysStateInfo_50[ePStateInfo50_SysMod] = mode;
    	}
    }
    /*正处于手动模式或自动模式*/
    else
    {
        /*模式切换*/
    	if( mode != gPSysStateInfo_50[ePStateInfo50_SysMod])
    	{
    		/*手动/自动切换*/
        	if((eMODE_HAND == mode) || (eMODE_AUTO == mode))
        	{
        		/*记录新模式*/
    			data[1] = mode;
    			data[0] = PARA_CFG_FLAG_U16;
    			ParaWriteStorePileWorkModeAPI(1, &data[1], 1);
    			ParaWriteStorePileWorkModeAPI(0, &data[0], 1);
        	}

        	/*记录原始模式和模式切换时间*/
        	sPileHisNorMode = gPSysStateInfo_50[ePStateInfo50_SysMod];
        	sRcvUnorModeTime = BSPGetOSMillSecTimeAPI();

    		/*模式切换用户处理*/
    		TacPileWorkModeChangDealHook((u8)gPSysStateInfo_50[ePStateInfo50_SysMod], (u8)mode);

        	/*切换当前模式接收模式切换时间*/
    		gPSysStateInfo_50[ePStateInfo50_SysMod] = mode;
    	}
    }
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: static u8 CheckGroupWorkModeFitState(u8 mode)
//输入参数	: mode:目标模式
//输出参数	: TRUE:正常 FALSE:异常
//函数功能	: 堆控检测对应模式下电池组工作模式是否正常
//注意事项	: 堆控自动/手动模式下确保主控处于手动模式
//=============================================================================================
static u8 CheckGroupWorkModeFitState(u8 mode)
{
    u8 i = 0;

    for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
    {
        if((0 == GetPGroupCanOffStateAPI(i))                //组通讯正常
            && (1 == GetPGroupCanRcvStateAPI(i))            //组有通信报文
	        && (eWORK_INIT != GetPGroupWorkStateAPI(i)))    //组非初始化
        {
        	//堆控处于自动模式或手动模式
        	if((eMODE_AUTO == GetPileWorkModeAPI())
                || (eMODE_HAND == GetPileWorkModeAPI()))
        	{
        		//主控非手动模式
				if((eMODE_HAND != GetPGroupWorkModeAPI(i)))
				{
	        		//主控模式异常
					return(FALSE);
				}
        	}
        	//堆控处于紧急模式
        	else if(eMODE_EMERG == GetPileWorkModeAPI())
        	{
        		//主控非紧急模式
				if((eMODE_EMERG != GetPGroupWorkModeAPI(i)))
				{
	        		//主控模式异常
					return(FALSE);
				}
        	}
        	//堆控处于调试模式
        	else if(eMODE_DEBUG == GetPileWorkModeAPI())
        	{
        		//主控非调试模式
				if((eMODE_DEBUG != GetPGroupWorkModeAPI(i)))
				{
	        		//主控模式异常
					return(FALSE);
				}
        	}
        }
    }

    return(TRUE);
}

//=============================================================================================
//函数名称	: static void PileAutoModeSysCtrlDeal(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 堆控自动模式下控制任务处理
//注意事项	: 自启动控制
//=============================================================================================
static void PileAutoModeSysCtrlDeal(void)
{
	static u16 sTimer = 0;

    //系统处于空闲且适合启动
    if((eWORK_IDLE == GetPileWorkStateAPI())
        && (1 == PileGetSysFitStartStateHook()))
    {
        //达到启动等待延时(200ms执行一次)或上电首次启动
        if(((sTimer * 2) >= gPSmuGenParaR_102[eSmuGenPara102_UpWaitT])
            || (BSPGetOSMillSecTimeAPI() <= 30000))
        {
            //设置正常启动命令
        	gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTART;
            RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
        }
        //未达到启动延时
        else
        {
            sTimer++;
        }
    }
    //系统正在启动时不适合启动(无电池组启动则停机否则故障停机)
    else if((eWORK_START == GetPileWorkStateAPI())
        && (0 == PileGetSysFitStartStateHook())
        && (0 == GetPAllGroupRunStateAPI()))
    {
        //立即停止系统
        gPSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTOP;
        RcvPileWorkStateCmdAPI(eCMDPORT_INTE);
        sTimer = 0;
    }
    //系统非空闲或充放电保护
    else
    {
        //启动延时清零
        sTimer = 0;
    }
}

#endif
