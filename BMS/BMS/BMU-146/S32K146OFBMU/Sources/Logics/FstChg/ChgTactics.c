/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ChgTactics.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 快充策略处理模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "TacUser.h"
#include "CHGInterface.h"
#include "ChgTactics.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sFstChgStopReqFlag = 0;                           /*快充请求停机标志*/
extern u16 gGSysCmdCtrl_86[];			                    /*系统控制命令信息数组*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void GroupFstChgTacticsTask(void)
** @Input     : void
** @Output    : void
** @Function  : 快充策略执行任务
** @The notes : 500ms调用,遵守快充国标2017流程
**===============================================================================================*/
void GroupFstChgTacticsTask(void)
{
	static u16 sTime = 0;
	static u8 sState = 0;

	switch(sState)
	{
		/*空闲阶段:检测充电枪插入*/
		case 0:
			/*充电枪有连接(电源接入12V或接收到充电桩CHM报文)*/
			if((GetGFstChgFCVoltAPI() >= 900)
				|| (CHGGetRcvChgerInfoAPI(eCHGRCV_CHM) > 0))
			{
				/*进入连接阶段*/
				CHGSetNowStageAPI(eCHGSTAG_LINK);
				sState = 1;
				sTime = 0;
			}
			/*充电枪未连接*/
			else
			{
				/*保持空闲阶段态*/
				CHGSetNowStageAPI(eCHGSTAG_IDLE);
			}
			sFstChgStopReqFlag = 0;
			break;

		/*连接阶段:停止电池系统等待就绪*/
		case 1:
			/*充电枪连接正常(CC2电压就绪变成2.5V(原5V一半))*/
			if(ABS(GetGFstChgCC2VoltAPI(), 250) <= 100)
			{
				/*快充请求停机*/
				sFstChgStopReqFlag = 1;

				/*电池系统故障*/
				if((eWORK_ERR == GetGWorkStateAPI())
					|| (eWORK_OFF == GetGWorkStateAPI()))
				{
					/*进入充电结束阶段*/
					CHGSetNowStageAPI(eCHGSTAG_FIN);
					sState = 6;
				}
				/*电池系统空闲*/
				else if(eWORK_IDLE == GetGWorkStateAPI())
				{
					/*进入握手阶段*/
					CHGSetNowStageAPI(eCHGSTAG_HAND);
					sState = 2;
					sTime = 0;
				}
				/*电池系统未空闲*/
				else
				{
					/*运行状态则停止电池系统*/
					if(eWORK_RUN == GetGWorkStateAPI())
					{
						/*判断5s超时*/
						if(sTime >= 10)
						{
							/*进入充电终止阶段*/
							CHGSetNowStageAPI(eCHGSTAG_STO);
							sState = 5;
							sTime = 0;
						}
						/*未超时*/
						else
						{
							/*正常停止电池系统命令*/
							gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTOP;
							RcvGroupWorkStateCmdAPI(eCMDPORT_FSTC);
							sTime++;
						}
					}
				}
			}
			/*充电枪取出*/
			else if(GetGFstChgFCVoltAPI() < 90)
			{
				/*恢复空闲阶段*/
				CHGSetNowStageAPI(eCHGSTAG_IDLE);
				sState = 0;
			}
			break;

		/*握手阶段:等待通信握手完成*/
		case 2:
			/*接收到握手成功CRM(AA确认)和多包传输BRM接收完成确认*/
			if((CHG_RCV_CRM_CON == CHGGetRcvChgerInfoAPI(eCHGRCV_CRM))
				&& (1 == CHGGetRcvChgerInfoAPI(eCHGRCV_CBRM)))
			{
				/*进入参数配置阶段*/
				CHGSetNowStageAPI(eCHGSTAG_CFG);
				sState = 3;
				sTime = 0;
			}
			/*未收到充电桩CRM握手成功报文(无AA确认)*/
			else
			{
				/*判断60s超时*/
				if(sTime >= 120)
				{
					/*进入充电结束阶段*/
					CHGSetNowStageAPI(eCHGSTAG_FIN);
					sState = 6;
					sTime = 0;
				}
				else
				{
					sTime++;
				}
			}
			break;

		/*配置阶段:等待充电桩参数并电池上高压*/
		case 3:
			/*接收到充电桩参数成功CML(参数与电池一致可充电)*/
			if(CHG_RCV_CML_CON == CHGGetRcvChgerInfoAPI(eCHGRCV_CML))
			{
				/*清除快充请求停机*/
				sFstChgStopReqFlag = 0;

				/*电池系统关机*/
				if(eWORK_OFF == GetGWorkStateAPI())
				{
					/*进入充电结束阶段*/
					CHGSetNowStageAPI(eCHGSTAG_FIN);
					sState = 6;
					sTime = 0;
				}
				/*电池系统运行*/
				else if(eWORK_RUN == GetGWorkStateAPI())
				{
					/*进入配置下半阶段(等待充电桩确认电池参数)*/
					//CHGSetNowStageAPI(eCHGSTAG_CFG);
					sState = 30;
					sTime = 0;
				}
				/*电池系统未就绪*/
				else
				{
					/*判断30s超时(60s内电池必须就绪BRO=AA)*/
					if(sTime >= 60)
					{
						/*进入充电结束阶段*/
						CHGSetNowStageAPI(eCHGSTAG_FIN);
						sState = 6;
						sTime = 0;
					}
					/*未超时*/
					else
					{
						/*空闲状态则启动电池系统*/
						if(eWORK_IDLE == GetGWorkStateAPI())
						{
							/*充放电保护则清除电池系统故障(间隔3s)*/
							if((ePRO_SYSPRO == GetGProtectStateAPI())
								&& (0 == (sTime % 6)))
							{
								/*清除电池系统当前故障命令*/
								gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd] = eCLR_NOWERR;
								RcvGroupSysClrCmdAPI(eCMDPORT_FSTC);
							}
							/*系统正常则高压上电(间隔3s)*/
							else if(1 == (sTime % 6))
							{
								/*正常启动电池系统命令*/
								gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTART;
								RcvGroupWorkStateCmdAPI(eCMDPORT_FSTC);
							}
						}
						/*故障状态则停止电池系统(包括清除系统故障)*/
						else if((eWORK_ERR == GetGWorkStateAPI())
							&& (0 == (sTime % 6)))
						{
							/*强制停止电池系统命令*/
							gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_FORCSTOP;
							RcvGroupWorkStateCmdAPI(eCMDPORT_FSTC);

							/*清除电池系统当前故障命令*/
							gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd] = eCLR_NOWERR;
							RcvGroupSysClrCmdAPI(eCMDPORT_FSTC);
						}

						sTime++;
					}
				}
			}
			/*未接收到充电桩参数CML(或参数与电池需求不符不能充电)*/
			else
			{
				/*判断60s超时*/
				if(sTime >= 120)
				{
					/*进入充电终止阶段*/
					CHGSetNowStageAPI(eCHGSTAG_FIN);
					sState = 6;
					sTime = 0;
				}
				else
				{
					sTime++;
				}
			}
			break;

		/*配置阶段:等待通信配置完成*/
		case 30:
			/*接收到配置成功CRO(AA确认)*/
			if(CHG_RCV_CRO_CON == CHGGetRcvChgerInfoAPI(eCHGRCV_CRO))
			{
				/*进入充电阶段*/
				CHGSetNowStageAPI(eCHGSTAG_CHG);
				sState = 4;
				sTime = 0;
			}
			/*未收到充电桩CRO配置成功报文(无AA确认)*/
			else
			{
				/*判断60s超时*/
				if(sTime >= 120)
				{
					/*进入充电终止阶段*/
					CHGSetNowStageAPI(eCHGSTAG_STO);
					sState = 5;
					sTime = 0;
				}
				else
				{
					sTime++;
				}
			}
			break;

		/*充电阶段:等待充电完成*/
		case 4:
			/*充电截止状态*/
			/*充电桩请求充电终止*/
			/*充电枪取出*/
			if((ePRO_FULL == GetGProtectStateAPI())
				|| (ePRO_SYSPRO == GetGProtectStateAPI())
				|| (1 == CHGGetRcvChgerInfoAPI(eCHGRCV_STOP))
				|| (ABS(GetGFstChgCC2VoltAPI(), 500) <= 100))
			{
				/*进入充电终止阶段*/
				CHGSetNowStageAPI(eCHGSTAG_STO);
				sState = 5;
				sTime = 0;
			}
			break;

		/*充电终止阶段:等待充电桩关闭*/
		case 5:
			/*请求停机*/
			sFstChgStopReqFlag = 1;
			/*充放电空闲状态*/
			if(eCURR_IDLE == GetGChgDhgStateAPI())
			{
				/*等待3s*/
				if(sTime >= 6)
				{
					/*进入充电结束阶段*/
					CHGSetNowStageAPI(eCHGSTAG_FIN);
					sState = 6;
					sTime = 0;
				}
				else
				{
					sTime++;
				}
			}
			break;

		/*充电结束阶段:等待充电枪取出*/
		case 6:
			/*请求停机*/
			sFstChgStopReqFlag = 1;
			if(eCURR_IDLE == GetGChgDhgStateAPI())
			{
				/*运行接触器未全断开*/
				if(FALSE == TacGetGroupRunSwhAllOffStateAPI())
				{
					/*正常停止电池系统命令*/
					gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTOP;
					RcvGroupWorkStateCmdAPI(eCMDPORT_FSTC);
				}

				/*充电枪取出*/
				if((GetGFstChgFCVoltAPI() < 90) && (ABS(GetGFstChgCC2VoltAPI(), 500) <= 100))
				{
					/*返回空闲阶段*/
					CHGSetNowStageAPI(eCHGSTAG_IDLE);
					sState = 0;
				}
			}
			/*充电桩停机超时5s*/
			else if(sTime >= 10)
			{
				/*强制停止电池系统命令*/
				gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_FORCSTOP;
				RcvGroupWorkStateCmdAPI(eCMDPORT_FSTC);
			}
			else
			{
				sTime++;
			}
			break;

		default:
			sState = 0;
			break;
	}
}

/*=================================================================================================
** @Name      : u8 GetFstChgStopReqFlagAPI(void)
** @Input     : void
** @Output    : 0:未请求 1:请求停机
** @Function  : 获取快充请求停机标志
** @The notes :
**===============================================================================================*/
u8 GetFstChgStopReqFlagAPI(void)
{
	return(sFstChgStopReqFlag);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
