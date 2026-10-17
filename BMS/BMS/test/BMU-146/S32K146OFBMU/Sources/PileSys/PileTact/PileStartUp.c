/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileStartUp.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控电池堆启动控制任务模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicPara.h"
#include "PilePara.h"
#include "PileInfo.h"
#include "PublicInfo.h"
#include "PileGroupRcv.h"
#include "PileWorkState.h"
#include "PileTactics.h"
#include "PileSysState.h"
#include "PileTactInterface.h"
#include "PileStartUp.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sPileCtrlGroupStep = 0;      //电池堆启停控制电池组步骤

extern t_SYSCTRL_INFO gPileCtrlInfo;   //系统启动停机控制信息
extern u8 gPileSysCtrlCmd;             //记录电池堆系统控制命令[0,9]
extern u8 gPileSysStartStep;           //电池堆系统启动步骤
extern u8 gPileSysStartErrFlag;        //电池堆启动异常标志(每bit表示一个异常原因)[0正常,1异常]

extern u16 gSysGenParaRO_100[];  			/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];  			/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
//=============================================================================================
//函数名称	: static void StartPileGroupPeriDiagStep(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆启动控制电池组外围自诊断
//注意事项	: 检测粘连与绝缘
//=============================================================================================
static void StartPileGroupPeriDiagStep(void);

//=============================================================================================
//函数名称	: static void ResetPileGroupPeriDiagStep(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆复位控制电池组外围自诊断
//注意事项	: 复位所有电池组自行粘连检测,其中一组电池组自行绝缘检测
//=============================================================================================
static void ResetPileGroupPeriDiagStep(void);

//=============================================================================================
//函数名称	: static void ResetPileStartCtrlInfo(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆系统启动控制信息复位
//注意事项	: 仅仅在启动电池堆时调用
//=============================================================================================
static void ResetPileStartCtrlInfo(void);

//=============================================================================================
//函数名称	: static void CheckPileCanStartGroup(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 检测各组电池组性能,判定能够启动的电池组
//注意事项	: 仅仅在启动电池堆时调用
//=============================================================================================
static void CheckPileCanStartGroup(void);

//=============================================================================================
//函数名称	: static void SelectPileFixRunGroup(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池组总压降序排列查找可运行电池组
//注意事项	: 仅在启动电池堆时使用
//=============================================================================================
static void SelectPileFixRunGroup(void);

//=============================================================================================
//函数名称	: static void StartPileGroupPreChgStep(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆启动控制电池组预充
//注意事项	: 控制1组检测1次电流和状态
//=============================================================================================
static void StartPileGroupPreChgStep(void);

//=============================================================================================
//函数名称	: static void StartPileGroupRunStep(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆启动控制电池组运行
//注意事项	: 控制1组检测1次电流和状态
//=============================================================================================
static void StartPileGroupRunStep(void);

//=============================================================================================
//函数名称	: static u8 CheckPileStartGroupPChgSwhState(u16 time)
//输入参数	: time:检测次数
//输出参数	: FALSE:未全闭合 TRUE:全闭合
//函数功能	: 检测启动电池组预充控制状态
//注意事项	: 
//=============================================================================================
static u8 CheckPileStartGroupPChgSwhState(u16 time);

//=============================================================================================
//函数名称	: static u8 CheckPileStartGroupRunSwhState(u16 time)
//输入参数	: time:检测次数
//输出参数	: FALSE:未运行 TRUE:全运行
//函数功能	: 检测启动电池组运行状态
//注意事项	: 
//=============================================================================================
static u8 CheckPileStartGroupRunSwhState(u16 time);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void PileSysStartCtrlTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 启动电池堆系统控制任务
//注意事项	: 启动状态10ms调用
//=============================================================================================
void PileSysStartCtrlTask(void)
{
    static u16 sWaitTime = 0;
    static u16 sStartTime = 0;
    
    switch(gPileSysStartStep)
    {
        //初始化电池堆启动信息
        case 0:
            sWaitTime = 0;
            sStartTime = 0;
            sPileCtrlGroupStep = 0;
            ResetPileStartCtrlInfo();

			//堆控管控粘连和绝缘检测使能
			#if(1 == PILE_DIAG_F_EN)
			//进入电池组外围诊断控制
			gPileSysStartStep = 10;
			//主控自行进行粘连和绝缘检测
			#else
			//进入可用电池组选取
			gPileSysStartStep = 11;
			#endif
            break;
            
		//进入电池组外围诊断控制(粘连与绝缘检测)
		case 10:
			StartPileGroupPeriDiagStep();
			sWaitTime = 0;
			break;

		//选取可用电池组
		case 11:
			//检测非故障可启动的电池组
			CheckPileCanStartGroup();

			//有可启动电池组
			if(gPileCtrlInfo.CtrlGNum > 0)
			{
				//进入运行电池组选取
	            sWaitTime = 0;
				gPileSysStartStep = 1;
			}
			//无可启动电池组
			else
			{
				//等待超时
				if(sWaitTime >= 500)
				{
					//进入故障判断
		            sWaitTime = 0;
					gPileSysStartStep = 7;

					//设置可并网电池组不足原因
					BitSet(gPileSysStartErrFlag, e_CtrlExp_LaOKGroup);
				}
				//等待5s
				else
				{
					sWaitTime++;
				}
			}
			break;

        //选取可运行电池组
        case 1:
            SelectPileFixRunGroup();
            
            //有可启动电池组
            if(gPileCtrlInfo.CtrlGNum > 0)
            {
                //进入控制电池组预充控制阶段
	            sWaitTime = 0;
                sStartTime = 0;
                sPileCtrlGroupStep = 0;
                gPileSysStartStep = 2;
            }
            else
            {
				//等待超时
				if(sWaitTime >= 500)
				{
					//进入故障判断
		            sWaitTime = 0;
					gPileSysStartStep = 7;

					//设置组间压差过大原因
					BitSet(gPileSysStartErrFlag, e_CtrlExp_SumVDiff);
				}
				//等待5s
				else
				{
					sWaitTime++;
				}
            }
            break;

        //进入电池组预充控制
        case 2:
            StartPileGroupPreChgStep();
            sWaitTime = 0;
            break;
            
        //判断是否继续启动
        case 3:
            sStartTime++;
            
            //预充电流超过预充接触器安全电流值
            if(0 == GetPGroupAllPreSafeCStateAPI())
            {
                //进入故障判断
                gPileSysStartStep = 7;
                
                //设置预充电流过大原因
                BitSet(gPileSysStartErrFlag, e_CtrlExp_UpCOver);
            }
            //启动电池组未全部启动预充
            else if(0 == CheckPileStartGroupPChgSwhState(sStartTime))
            {
                //超时进入故障判断
                if(sStartTime >= gPSmuGenParaR_102[eSmuGenPara102_UpSwhExpT])
                {
                    gPileSysStartStep = 7;
                
                    //设置预充控制失败原因
                    BitSet(gPileSysStartErrFlag, e_CtrlExp_UpPChgCmd);
                }
                //控制未超时
                else
                {
                    //返回重新控制启动
                    sPileCtrlGroupStep = 0;
                    gPileSysStartStep = 2;
                }
            }
            //非预充命令且预充到达时间
            else if((sWaitTime >= gPSmuGenParaR_102[eSmuGenPara102_WaitPreT])
                && (eCTRL_PRECHG != gPileSysCtrlCmd))
            {
                //进入正常启动运行阶段
                sWaitTime = 0;
                sStartTime = 0;
                sPileCtrlGroupStep = 0;
                gPileSysStartStep = 4;
            }
            //未到达预充时间
            else
            {
                sWaitTime++;
            }
            break;
            
        //进入电池组系统启动
        case 4:
            //吸合主接触器运行
            StartPileGroupRunStep();
            sStartTime = 0;
            sWaitTime = 0;
            break;
            
        //电池组启动结果检测
        case 5:
            sWaitTime++;
            
            //启动无异常并更新运行组信息
            if((1 == CheckPileStartGroupRunSwhState(sWaitTime))
                && (GetPileRunGroupNumAPI() > 0))
            {
    			//堆控管控粘连和绝缘检测使能
    			#if(1 == PILE_DIAG_F_EN)
    			//进入电池组外围诊断复位
                sPileCtrlGroupStep = 0;
    			gPileSysStartStep = 20;
    			//主控自行进行粘连和绝缘检测
    			#else
    			//组启动成功
    			gPileSysStartStep = 6;
    			#endif
            }
            else                                //启动异常
            {
                //等待超时
                if(sWaitTime >= gPSmuGenParaR_102[eSmuGenPara102_UpSwhExpT])
                {
                    sWaitTime = 0;
                    gPileSysStartStep = 7;      //记录预充断开失败原因和进入故障判断
                    BitSet(gPileSysStartErrFlag, e_CtrlExp_DnPChgCmd);
                }
            }
            break;
            
		//进入电池组外围诊断复位(粘连与绝缘检测)
		case 20:
			ResetPileGroupPeriDiagStep();
			sWaitTime = 0;
			break;

        //堆启动成功进入运行状态
        case 6:
            SetPileSysWorkStateAPI(eWORK_RUN);
            gPileSysStartStep = 0xff;
            break;
            
        //启动异常进入系统故障
        case 7:
        	//10s延时等待上报启动失败故障
        	if(sWaitTime >= 1000)
        	{
				SetPileSysWorkStateAPI(eWORK_ERR);
				gPileSysStartStep = 0xff;
				sWaitTime = 0;
        	}
        	else
        	{
        		sWaitTime++;
        	}
            break;
            
        default:
            gPileSysStartStep = 0xff;
            break;
    }
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: static void StartPileGroupPeriDiagStep(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆启动控制电池组外围自诊断
//注意事项	: 检测粘连与绝缘
//=============================================================================================
static void StartPileGroupPeriDiagStep(void)
{
	u8 i = 0;
	u8 errFlag = 0;
	u16 cmd[2] = {0};
    static u8 sDiagCtrlGroup = 0;
    static u16 sWaitFailTime = 0;

    switch(sPileCtrlGroupStep)
    {
        //参数初始化
        case 0:
        	sDiagCtrlGroup = 0;
        	sWaitFailTime = 0;
            sPileCtrlGroupStep = 1;
            break;

		//发送禁止所有电池组自行诊断命令
		case 1:
			//广播禁止绝缘检测和粘连检测命令
			cmd[0] = 2;
			cmd[1] = 2;
			//PileBroadAllGroupUserCmdHook(eUserCtrl87_AdhCheck, 2, cmd);
			PileBroadAllGroupUserCmdHook(6, 2, cmd);
			sPileCtrlGroupStep = 2;
			break;

		//判断是否所有电池组停止诊断
		case 2:
			for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
			{
				//电池组激活使能通信正常
				if((1 == GetPGroupEnStateAPI(i))
					&& (1 == GetPGroupCanRcvStateAPI(i)))
				{
					//未接收到停止检测命令
					if((2 != GetPGroupCtrlResultAPI(i, 0))
						|| (2 != GetPGroupCtrlResultAPI(i, 1)))
					{
						//记录异常
						errFlag = 1;
						break;
					}
				}
			}

			//有异常且未达到延时3s
			if((1 == errFlag)
				&& (sWaitFailTime < 300))
			{
				//返回发送控制命令
				sWaitFailTime++;
				sPileCtrlGroupStep = 1;
			}
			else
			{
				//进入单组依次检测
				sWaitFailTime = 0;
				sPileCtrlGroupStep = 3;
			}
			break;

		//发送单组电池组开始诊断命令
		case 3:
			//发送请求绝缘检测和粘连检测命令
			cmd[0] = 1;
			cmd[1] = 1;
			//PileSetGroupUserCtrlCmdHook(sDiagCtrlGroup, eUserCtrl87_AdhCheck, 2, cmd);
			PileSetGroupUserCtrlCmdHook(sDiagCtrlGroup, 6, 2, cmd);
			sPileCtrlGroupStep = 4;
			break;

		//判断是否指定电池组完成诊断
		case 4:
			//电池组激活使能通信正常
			if((1 == GetPGroupEnStateAPI(sDiagCtrlGroup))
				&& (1 == GetPGroupCanRcvStateAPI(sDiagCtrlGroup)))
			{
				//未接收到停止检测命令
				if(((3 != GetPGroupCtrlResultAPI(sDiagCtrlGroup, 0))
					&& (0 != GetPGroupCtrlResultAPI(sDiagCtrlGroup, 0)))
					|| ((3 != GetPGroupCtrlResultAPI(sDiagCtrlGroup, 1))
					&& (0 != GetPGroupCtrlResultAPI(sDiagCtrlGroup, 1))))
				{
					//有异常且未达到延时5s
					if(sWaitFailTime < 500)
					{
						//返回发送控制命令
						sWaitFailTime++;

						//未接收到控制命令
						if((1 != GetPGroupCtrlResultAPI(i, 0))
							|| (1 != GetPGroupCtrlResultAPI(i, 1)))
						{
							//返回发送控制命令
							sPileCtrlGroupStep = 3;
						}
					}
					//等待检测检测超时
					else
					{
						//切换检测下一组
						sDiagCtrlGroup++;
						sWaitFailTime = 0;
						//返回关闭现有检测
						sPileCtrlGroupStep = 1;
					}
				}
				//检测完成
				else
				{
					//切换检测下一组
					sDiagCtrlGroup++;
					sWaitFailTime = 0;
					//返回关闭现有检测
					sPileCtrlGroupStep = 1;
				}
			}
			else
			{
				//切换检测下一组
				sDiagCtrlGroup++;
				sWaitFailTime = 0;
				//返回关闭现有检测
				sPileCtrlGroupStep = 1;
			}

			//所有电池组检测完成
			if(sDiagCtrlGroup >= gSysGenParaRO_100[eSGenPara100_AllGNum])
			{
				//参数复位
				sDiagCtrlGroup = 0;
				sWaitFailTime = 0;
				//发送退出检测命令
				sPileCtrlGroupStep = 5;
			}
			break;

		//发送所有电池组禁止诊断命令
		case 5:
			//广播禁止绝缘检测和粘连检测命令
			cmd[0] = 2;
			cmd[1] = 2;
			//PileBroadAllGroupUserCmdHook(eUserCtrl87_AdhCheck, 2, cmd);
			PileBroadAllGroupUserCmdHook(6, 2, cmd);
			sPileCtrlGroupStep = 6;
			break;

		//判断是否所有电池组自行诊断
		case 6:
			for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
			{
				//电池组激活使能通信正常
				if((1 == GetPGroupEnStateAPI(i))
					&& (1 == GetPGroupCanRcvStateAPI(i)))
				{
					//未接收到停止检测命令
					if((2 != GetPGroupCtrlResultAPI(i, 0))
						|| (2 != GetPGroupCtrlResultAPI(i, 1)))
					{
						//记录异常
						errFlag = 1;
						break;
					}
				}
			}

			//有异常且未达到延时3s
			if((1 == errFlag)
				&& (sWaitFailTime < 300))
			{
				//返回发送控制命令
				sWaitFailTime++;
				sPileCtrlGroupStep = 6;
			}
			else
			{
				//进入单组依次检测
				sWaitFailTime = 0;
				sPileCtrlGroupStep = 0;
				//进入电池堆启动步骤
                gPileSysStartStep = 11;
			}
			break;

        default:
            sPileCtrlGroupStep = 0;
            break;
    }
}

//=============================================================================================
//函数名称	: static void ResetPileGroupPeriDiagStep(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆复位控制电池组外围自诊断
//注意事项	: 复位所有电池组自行粘连检测,其中一组电池组自行绝缘检测
//=============================================================================================
static void ResetPileGroupPeriDiagStep(void)
{
	u8 i = 0;
	u8 errFlag = 0;
	u16 cmd[2] = {0};
    static u8 sDiagCtrlGroup = 0;
    static u16 sWaitFailTime = 0;

    switch(sPileCtrlGroupStep)
    {
        //参数初始化
        case 0:
        	sDiagCtrlGroup = 0;
        	sWaitFailTime = 0;
            sPileCtrlGroupStep = 1;
            break;

		//发送所有电池组自行诊断粘连和禁止中断绝缘命令
		case 1:
			//广播绝缘检测和粘连检测控制命令
			cmd[0] = 0;
			cmd[1] = 2;
			//PileBroadAllGroupUserCmdHook(eUserCtrl87_AdhCheck, 2, cmd);
			PileBroadAllGroupUserCmdHook(6, 2, cmd);
			sPileCtrlGroupStep = 2;
			break;

		//判断是否所有电池组接收成功
		case 2:
			for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
			{
				//电池组激活使能通信正常
				if((1 == GetPGroupEnStateAPI(i))
					&& (1 == GetPGroupCanRcvStateAPI(i)))
				{
					//未接收到停止检测命令
					if((0 != GetPGroupCtrlResultAPI(i, 0))
						|| (2 != GetPGroupCtrlResultAPI(i, 1)))
					{
						//记录异常
						errFlag = 1;
						break;
					}
				}
			}

			//有异常且未达到延时3s
			if((1 == errFlag)
				&& (sWaitFailTime < 300))
			{
				//返回发送控制命令
				sWaitFailTime++;
				sPileCtrlGroupStep = 1;
			}
			else
			{
				//进入单组检测复位
				sWaitFailTime = 0;
				sDiagCtrlGroup = GetSysSelfGroupIdAPI();
				sPileCtrlGroupStep = 3;
			}
			break;

		//发送堆控所在电池组自行绝缘诊断命令
		case 3:
			//发送请求绝缘检测和粘连检测命令
			cmd[0] = 0;
			cmd[1] = 0;
			//PileSetGroupUserCtrlCmdHook(sDiagCtrlGroup, eUserCtrl87_AdhCheck, 2, cmd);
			PileSetGroupUserCtrlCmdHook(sDiagCtrlGroup, 6, 2, cmd);
			sPileCtrlGroupStep = 4;
			break;

		//判断是否指定电池组完成诊断
		case 4:
			//电池组激活使能通信正常
			if((1 == GetPGroupEnStateAPI(sDiagCtrlGroup))
				&& (1 == GetPGroupCanRcvStateAPI(sDiagCtrlGroup)))
			{
				//未接收到停止检测命令
				if((0 != GetPGroupCtrlResultAPI(sDiagCtrlGroup, 0))
					&& (0 != GetPGroupCtrlResultAPI(sDiagCtrlGroup, 1)))
				{
					//有异常且未达到延时5s
					if(sWaitFailTime < 500)
					{
						//返回发送控制命令
						sWaitFailTime++;
						sPileCtrlGroupStep = 3;
					}
					//等待检测检测超时
					else
					{
						//复位完成
						sWaitFailTime = 0;
						sPileCtrlGroupStep = 0;
						//进入电池堆启动成功状态
		                gPileSysStartStep = 6;
					}
				}
				//检测完成
				else
				{
					//复位完成
					sWaitFailTime = 0;
					sPileCtrlGroupStep = 0;
					//进入电池堆启动成功状态
	                gPileSysStartStep = 6;
				}
			}
			else
			{
				//切换检测下一组
				sDiagCtrlGroup++;

				//所有电池组复位完成
				if(sDiagCtrlGroup >= gSysGenParaRO_100[eSGenPara100_AllGNum])
				{
					//复位完成
					sWaitFailTime = 0;
					sPileCtrlGroupStep = 0;
					//进入电池堆启动成功状态
	                gPileSysStartStep = 6;
				}
				else
				{
					//返回控制复位该电池组
					sWaitFailTime = 0;
					sPileCtrlGroupStep = 3;
				}
			}
			break;

        default:
            sPileCtrlGroupStep = 0;
            break;
    }
}

//=============================================================================================
//函数名称	: static void ResetPileStartCtrlInfo(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆系统启动控制信息复位
//注意事项	: 仅仅在启动电池堆时调用
//=============================================================================================
static void ResetPileStartCtrlInfo(void)
{
    u8 index = 0;
    
    gPileCtrlInfo.CtrlGNum  = 0;
    gPileCtrlInfo.CtrlFailGroup = 0;
    
    for(index = 0; index < gSysGenParaRO_100[eSGenPara100_AllGNum]; index++)
    {
        gPileCtrlInfo.CtrlGId[index] = 0xff;
    }
}

//=============================================================================================
//函数名称	: static void CheckPileCanStartGroup(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 检测各组电池组性能,判定能够启动的电池组
//注意事项	: 仅仅在启动电池堆时调用
//=============================================================================================
static void CheckPileCanStartGroup(void)
{
    u8 i = 0;
    u8 group = 0;
    u8 enable = 0;
    u8 rcvNum = 0;
    u8 norNum = 0;
    
    //控制信息清除
    gPileCtrlInfo.CtrlGNum = 0;
    gPileCtrlInfo.CtrlFailGroup = 0;
    
    norNum = GetPileNorGroupNumAPI();

    //有可启动的电池组
    if(norNum > 0)
    {
		//找出可启动的电池组
		for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
		{
			if(1 == GetPGroupEnStateAPI(i))                         //检测使能组
			{
				enable++;                                           //计算使能组数

				if(1 == GetPGroupCanRcvStateAPI(i))                 //有通信报文
				{
					rcvNum++;                                       //计算上电组数
				}

				if(i < norNum)                                      //仅判断可用电池组
				{
					group = GetPileNorGroupIdAPI(i);                //获取可用电池组号

					//充电启动
					if(eCTRL_CHGSTART == gPileSysCtrlCmd)
					{
						//电池组未充电限制才可启动
						if((ePRO_WAIT == GetPGroupProStateAPI(i))
							|| (GetPGroupChgLimCurrAPI(group) > 0))
						{
							//记录可启动电池组
							gPileCtrlInfo.CtrlGId[gPileCtrlInfo.CtrlGNum] = group;
							gPileCtrlInfo.CtrlGNum++;
						}
					}
					//放电启动
					else if(eCTRL_DHGSTART == gPileSysCtrlCmd)
					{
						//电池组未放电限制才可启动
						if((ePRO_WAIT == GetPGroupProStateAPI(i))
							|| (GetPGroupDhgLimCurrAPI(group) > 0))
						{
							//记录可启动电池组
							gPileCtrlInfo.CtrlGId[gPileCtrlInfo.CtrlGNum] = group;
							gPileCtrlInfo.CtrlGNum++;
						}
					}
					//正常启动或预充
					else
					{
						//电池堆电池组总状态空闲(所有可用组均可启动)
						if(ePGPRO_STA_WAIT == GetPileAllGProStateAPI())
						{
							//电池组未同时放电限制均可启动
							if((ePRO_WAIT == GetPGroupProStateAPI(i))
								|| (GetPGroupChgLimCurrAPI(group) > 0)
								|| (GetPGroupDhgLimCurrAPI(group) > 0))
							{
								//记录可启动电池组
								gPileCtrlInfo.CtrlGId[gPileCtrlInfo.CtrlGNum] = group;
								gPileCtrlInfo.CtrlGNum++;
							}
						}
						//电池堆电池组总状态仅有充电限制或保护
						else if((ePGPRO_STA_FULL == GetPileAllGProStateAPI())
							|| (ePGPRO_STA_CHGLIM == GetPileAllGProStateAPI())
							|| (ePGPRO_STA_CHGPRO == GetPileAllGProStateAPI()))
						{
							//电池组非放电限制才可启动
							if((ePRO_WAIT == GetPGroupProStateAPI(i))
								|| (GetPGroupDhgLimCurrAPI(group) > 0))
							{
								//记录可启动电池组
								gPileCtrlInfo.CtrlGId[gPileCtrlInfo.CtrlGNum] = group;
								gPileCtrlInfo.CtrlGNum++;
							}
						}
						//电池堆电池组总状态仅有放电限制或保护
						else if((ePGPRO_STA_EMPTY == GetPileAllGProStateAPI())
							|| (ePGPRO_STA_DHGLIM == GetPileAllGProStateAPI())
							|| (ePGPRO_STA_DHGPRO == GetPileAllGProStateAPI()))
						{
							//电池组非充电限制才可启动
							if((ePRO_WAIT == GetPGroupProStateAPI(i))
								|| (GetPGroupChgLimCurrAPI(group) > 0))
							{
								//记录可启动电池组
								gPileCtrlInfo.CtrlGId[gPileCtrlInfo.CtrlGNum] = group;
								gPileCtrlInfo.CtrlGNum++;
							}
						}
						//电池堆电池组总状态同时有放电限制(所有可用电池组均可启动)
						else if(ePGPRO_STA_SYSLIM == GetPileAllGProStateAPI())
						{
							//记录可启动电池组
							gPileCtrlInfo.CtrlGId[gPileCtrlInfo.CtrlGNum] = group;
							gPileCtrlInfo.CtrlGNum++;
						}
					}
				}
			}
		}
    }
    
    //非所有使能电池组可用且故障隔离未使能或组数小于最小组数
    if((gPileCtrlInfo.CtrlGNum < enable)
        && ((0 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
		|| ((gPileCtrlInfo.CtrlGNum < gSysGenParaRO_100[eSGenPara100_MinGNum])
        && (gPileCtrlInfo.CtrlGNum < rcvNum))))//上电电池组均正常时允许小于最小运行组数
    {
        gPileCtrlInfo.CtrlGNum = 0;
    }
}

//=============================================================================================
//函数名称	: static void SelectPileFixRunGroup(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池组总压降序排列查找可运行电池组
//注意事项	: 仅在启动电池堆时使用
//=============================================================================================
static void SelectPileFixRunGroup(void)
{
    u8 i = 0;
    u8 j = 0;
    u8 k = 0;
    u8 num = 0;
    u8 maxNum = 0;
    u16 maxSumV = 0;
    u16 minSumV = 0;
    
    //只有组数超过1组时需要总压排序
    if(gPileCtrlInfo.CtrlGNum > 1)
    {
        //统计电池组总电压电池组号降序
        for(i = 0; i < gPileCtrlInfo.CtrlGNum; i++)
        {
            maxSumV = GetPGroupCSumVoltAPI(gPileCtrlInfo.CtrlGId[i]);
            
            for(j = i; j < gPileCtrlInfo.CtrlGNum; j++)
            {
                if(GetPGroupCSumVoltAPI(gPileCtrlInfo.CtrlGId[j]) > maxSumV)
                {
                    maxSumV = GetPGroupCSumVoltAPI(gPileCtrlInfo.CtrlGId[j]);
                    
                    //将最高电压放到最前面
                    k = gPileCtrlInfo.CtrlGId[i];
                    gPileCtrlInfo.CtrlGId[i] = gPileCtrlInfo.CtrlGId[j];
                    gPileCtrlInfo.CtrlGId[j] = k;
                }
            }
        }
    
        //检测总电压降序组号收尾电池组压差是否合适
        maxSumV = GetPGroupCSumVoltAPI(gPileCtrlInfo.CtrlGId[0]);                          //组最大总电压
        minSumV = GetPGroupCSumVoltAPI(gPileCtrlInfo.CtrlGId[gPileCtrlInfo.CtrlGNum - 1]); //组最小总电压
        
        //组总电压最值差在安全压差内则所有组电压适合启动
        if(maxSumV <= (minSumV + gPSmuGenParaR_102[eSmuGenPara102_UpSafeV]))
        {
            //所有可启动电池组均适合启动
            num = gPileCtrlInfo.CtrlGNum;
        }
        else
        {
            //充电启动
            if(eCTRL_CHGSTART == gPileSysCtrlCmd)
            {
                //从最末最低压开始选取电池组
                for(i = 0; i < gPileCtrlInfo.CtrlGNum; i++)
                {
                    //组总电压与最低电压组比较在安全压差内则适合启动
                    if(GetPGroupCSumVoltAPI(gPileCtrlInfo.CtrlGId[gPileCtrlInfo.CtrlGNum - 1 - i]) <= (minSumV + gPSmuGenParaR_102[eSmuGenPara102_UpSafeV]))
                    {
                        num++;
                    }
                }
                
                //非所有电池组则将前面不能运行的电池组去掉
                if(num < gPileCtrlInfo.CtrlGNum)
                {
                    //后面电池组前移
                    for(i = 0; i < num; i++)
                    {
                        //不合适的不要
                        gPileCtrlInfo.CtrlGId[i] = gPileCtrlInfo.CtrlGId[i + (gPileCtrlInfo.CtrlGNum - num)];
                    }
                }
            }
            //放电启动
            else if(eCTRL_DHGSTART == gPileSysCtrlCmd)
            {
                //从头最高压开始选取电池组
                for(i = 0; i < gPileCtrlInfo.CtrlGNum; i++)
                {
                    //组总电压与最高电压组比较在安全压差内则适合启动
                    if(maxSumV <= (GetPGroupCSumVoltAPI(gPileCtrlInfo.CtrlGId[i]) + gPSmuGenParaR_102[eSmuGenPara102_UpSafeV]))
                    {
                        num++;
                    }
                }
            }
            //其它启动方式
            else
            {
                //从从头最高电压组开始选取最大适合运行的电池组数
                for(i = 0; i < gPileCtrlInfo.CtrlGNum; i++) 
                {
                    num = 0;
                    for(j = i; j < gPileCtrlInfo.CtrlGNum; j++) 
                    {
                        //总电压降序组号中查找适合并网的最多的组数和适合并网组起始下标
                        maxSumV = GetPGroupCSumVoltAPI(gPileCtrlInfo.CtrlGId[i]);
                        if(maxSumV <= GetPGroupCSumVoltAPI(gPileCtrlInfo.CtrlGId[j]) + gPSmuGenParaR_102[eSmuGenPara102_UpSafeV])
                        {
                            num++;
                        }
                    }
                    
                    //找到更多适合运行的连续电池组序列
                    if(num > maxNum) 
                    {
                        //记录适合运行的起始索引和组数
                        k = i;
                        maxNum = num;
                    }
                }
                
                //保存选择的可运行最多组
                for(i = 0; i < maxNum; i++) 
                {
                    gPileCtrlInfo.CtrlGId[i] = gPileCtrlInfo.CtrlGId[k + i];     
                }
            }
        }
    }
    //只有一组
    else
    {
        num = gPileCtrlInfo.CtrlGNum;
    }
    
    //非所有电池组可运行且动态接入未使能或组数小于最小组数
    if((num < gPileCtrlInfo.CtrlGNum)
        && ((0 == gSysGenParaRO_100[eSGenPara100_AutConn])
        || (num < gSysGenParaRO_100[eSGenPara100_MinGNum])))
    {
        gPileCtrlInfo.CtrlGNum = 0;
    }
    //电池组可全部运行或允许动态介入
    else
    {
        gPileCtrlInfo.CtrlGNum = num;
    }
}

//=============================================================================================
//函数名称	: static void StartPileGroupPreChgStep(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆启动控制电池组预充
//注意事项	: 控制1组检测1次电流和状态
//=============================================================================================
static void StartPileGroupPreChgStep(void)
{
    static u8 sTurnOnNum = 0;                           //需吸合序号
    static u8 sPickGroup = 0;                           //需吸合组号
    static u8 sCheckCurNum = 0;                         //检测电流合理次数
    static u16 sFailTimeCur = 0;                        //电流判断超时计数
    static u16 sFailTimeTurnOn = 0;                     //接触器吸合超时计数
    
    switch(sPileCtrlGroupStep)
    {
        //参数初始化
        case 0:
            sTurnOnNum = 0;
            sPickGroup = 0;
            sCheckCurNum = 0;
            sFailTimeCur = 0;
            sFailTimeTurnOn = 0;
            sPileCtrlGroupStep = 1;
            break;
            
        //检测电池组预充电流是否安全
        case 1:
            //预充电流在预充接触器安全电流值内
            if(1 == GetPGroupAllPreSafeCStateAPI())
            {
                sCheckCurNum++;
                
                //达到检测次数
                if(sCheckCurNum >= CHECK_CUR_OK_TIMES)
                {
                    sFailTimeCur = 0;
                    sCheckCurNum = 0;
                    sPileCtrlGroupStep = 2;
                }
            }
            //预充电流过大
            else
            {
                sCheckCurNum = 0;
                sFailTimeCur++;
                
                //电流检测超时(10s)
                if(sFailTimeCur >= 1000)
                {
                    sFailTimeCur = 0;
                    sPileCtrlGroupStep = 0;
                    
                    //记录预充电流不安全启动失败原因和进入系统故障判断
                    gPileSysStartStep = 7;
                    BitSet(gPileSysStartErrFlag, e_CtrlExp_UpCOver);
                }  
            }
            break;
            
        //控制一个电池组启动预充
        case 2:
            //从最高电压组吸合预充接触器
            sPickGroup = gPileCtrlInfo.CtrlGId[sTurnOnNum];
            
            //下发组预充启动命令
            PileSetGroupWorkStateCtrlCmdHook(sPickGroup, eCTRL_PRECHG);
            sPileCtrlGroupStep = 3;
            break;
            
        //检测预充控制结果
        case 3:
            if((eWORK_IDLE != GetPGroupWorkStateAPI(sPickGroup))    //该组状态非空闲
                && (1 == GetPGroupPreSwhStateAPI(sPickGroup)))      //预充接触器吸合
            {
                sTurnOnNum++;                                       //准备吸合下一个
                sFailTimeTurnOn = 0;
                
                //所有可运行电池组控制完成
                if(sTurnOnNum >= gPileCtrlInfo.CtrlGNum)
                {
                    sPileCtrlGroupStep = 0;
                    gPileSysStartStep = 3;                          //下一步判断是否启动完成
                }
                //电池组未全部预充
                else
                {
                    sPileCtrlGroupStep = 1;                         //返回检测预充电流是否安全
                }
            }
            else
            {
                sFailTimeTurnOn++;
                
                //控制超时
                if(sFailTimeTurnOn >= gPSmuGenParaR_102[eSmuGenPara102_UpSwhExpT])
                {
                    //放弃控制
                    sFailTimeTurnOn = 0;
                    sPileCtrlGroupStep = 0;
                    
                    //记录预充控制失败启动失败原因和进入系统故障判断
                    gPileSysStartStep = 7;
                    BitSet(gPileCtrlInfo.CtrlFailGroup, sPickGroup);
                    BitSet(gPileSysStartErrFlag, e_CtrlExp_UpPChgCmd);
                }
                //控制未超时
                else
                {
                    //返回再次发送命令,防止报文丢失
                    sPileCtrlGroupStep = 2;
                }
            }
            break;
            
        default:
            sPileCtrlGroupStep = 0;
            break;
    }
}

//=============================================================================================
//函数名称	: static void StartPileGroupRunStep(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆启动控制电池组运行
//注意事项	: 控制1组检测1次电流和状态
//=============================================================================================
static void StartPileGroupRunStep(void)
{
    static u8 sTurnOnNum = 0;                           //需吸合序号
    static u8 sPickGroup = 0;                           //需并网组号
    static u8 sCheckCurNum = 0;                         //检测电流次数
    static u16 sFailTimeCur = 0;                        //电流判断超时计数
    static u16 sFailTimeTurnOn = 0;                     //接触器吸合超时计数
    
    switch(sPileCtrlGroupStep)
    {
        case 0:
            sTurnOnNum = 0;
            sPickGroup = 0;
            sFailTimeCur = 0;
            sFailTimeTurnOn = 0;
            sCheckCurNum = 0;
            sPileCtrlGroupStep = 1;
            break;
            
        case 1:
            //预充电流在预充接触器安全电流值内
            if(1 == GetPGroupAllPreSafeCStateAPI())
            {
                sCheckCurNum++;
                
                //达到检测次数
                if(sCheckCurNum >= CHECK_CUR_OK_TIMES)
                {
                    sFailTimeCur = 0;
                    sCheckCurNum = 0;
                    sPileCtrlGroupStep = 2;
                }
            }
            else
            {
                sCheckCurNum = 0;
                sFailTimeCur++;
                
                //电流检测超时(10s)
                if(sFailTimeCur >= 1000)
                {
                    sFailTimeCur = 0;
                    sPileCtrlGroupStep = 0;
                    
                    //记录预充电流不安全启动失败原因和进入系统故障判断
                    gPileSysStartStep = 7;
                    BitSet(gPileSysStartErrFlag, e_CtrlExp_UpCOver);
                }  
            }
            break;
            
        case 2:
            //获取并网组号
            sPickGroup = gPileCtrlInfo.CtrlGId[sTurnOnNum];
            
            //下发电池组正常启动运行命令
            PileSetGroupWorkStateCtrlCmdHook(sPickGroup, eCTRL_NORSTART);
            sPileCtrlGroupStep = 3;
            break;
            
        case 3:
            //吸合成功
            if(1 == GetPGroupMainSwhStateAPI(sPickGroup))
            {
                sTurnOnNum++;
                sFailTimeTurnOn = 0;
                
                //所有可运行电池组控制完成
                if(sTurnOnNum >= gPileCtrlInfo.CtrlGNum)
                {
                    //下一步判断是否继续启动
                    sPileCtrlGroupStep = 0;
                    gPileSysStartStep = 5;
                }
                //电池组未全部预充
                else
                {
                    //返回检测预充电流是否安全
                    sPileCtrlGroupStep = 1;
                }
            }
            else
            {
                sFailTimeTurnOn++;
                
                //吸合超时
                if(sFailTimeTurnOn >= gPSmuGenParaR_102[eSmuGenPara102_UpSwhExpT])
                {
                    //放弃控制
                    sPileCtrlGroupStep = 0;
                    sFailTimeTurnOn = 0;
                    
                    //记录主接触器控制失败启动失败原因和进入系统故障判断
                    gPileSysStartStep = 7;
                    BitSet(gPileCtrlInfo.CtrlFailGroup, sPickGroup);
                    BitSet(gPileSysStartErrFlag, e_CtrlExp_UpMainCmd);
                }
                else
                {
                    //再次发送，防止信息丢失
                    sPileCtrlGroupStep = 2;
                }
            }
            break;
            
        default:
            sPileCtrlGroupStep = 0;
            break;
    }
}

//=============================================================================================
//函数名称	: static u8 CheckPileStartGroupPChgSwhState(u16 time)
//输入参数	: time:检测次数
//输出参数	: FALSE:未全闭合 TRUE:全闭合
//函数功能	: 检测启动电池组预充控制状态
//注意事项	: 
//=============================================================================================
static u8 CheckPileStartGroupPChgSwhState(u16 time)
{
    u8 temp = 0;
    u8 group = 0;
    u8 result = TRUE;
    
    for(temp = 0; temp < gPileCtrlInfo.CtrlGNum; temp++)
    {
        group = gPileCtrlInfo.CtrlGId[temp];
        
        //判断电池组是否启动预充接触器闭合
        if(((eWORK_START != GetPGroupWorkStateAPI(group))   //该组状态非启动
            || (0 == GetPGroupPreSwhStateAPI(group)))       //预充接触器断开
            && (eWORK_RUN != GetPGroupWorkStateAPI(group))) //该组状态非运行
        {
            //记录控制失败的电池组
            result = FALSE;
            
            //检测超时
            if(time >= gPSmuGenParaR_102[eSmuGenPara102_UpSwhExpT])
            {
                BitSet(gPileCtrlInfo.CtrlFailGroup, group);
            }
        }
    }
    
    return(result);
}

//=============================================================================================
//函数名称	: static u8 CheckPileStartGroupRunSwhState(u16 time)
//输入参数	: time:检测次数
//输出参数	: FALSE:未运行 TRUE:全运行
//函数功能	: 检测启动电池组运行状态
//注意事项	: 
//=============================================================================================
static u8 CheckPileStartGroupRunSwhState(u16 time)
{
    u8 temp = 0;
    u8 group = 0;
    u8 result = TRUE;
    
    for(temp = 0; temp < gPileCtrlInfo.CtrlGNum; temp++)
    {
        group = gPileCtrlInfo.CtrlGId[temp];
        
        //判断电池组是否启动预充接触器闭合
        if((eWORK_RUN != GetPGroupWorkStateAPI(group))      //该组状态非运行
            || (0 == GetPGroupMainSwhStateAPI(group))       //主接触器断开
            || (0 == GetPGroupNegSwhStateAPI(group))        //负极接触器断开
            || (1 == GetPGroupPreSwhStateAPI(group)))       //预充接触器闭合
        {
            //记录控制失败的电池组
            result = FALSE;
            
            //检测超时
            if(time >= gPSmuGenParaR_102[eSmuGenPara102_UpSwhExpT])
            {
                BitSet(gPileCtrlInfo.CtrlFailGroup, group);
            }
        }
    }
    
    return(result);
}

#endif
