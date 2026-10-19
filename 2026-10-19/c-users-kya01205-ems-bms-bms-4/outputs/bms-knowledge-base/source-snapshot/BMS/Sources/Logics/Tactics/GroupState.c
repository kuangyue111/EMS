/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : GroupState.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池工作状态信息模块接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "TacUser.h"
#include "TacInterface.h"
#include "GroupProtect.h"
#include "GroupTactic.h"
#include "GroupState.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sGroupBattState = 0;                              /*电池组电池状态[0正常 1充满 2放空]*/

extern u16 gGSysStateInfo_50[];                             /*系统状态信息数组*/

extern u16 gGBmuGenPara_102[];  							/*主控通用参数值*/
extern u8 gGHardPara_104[];                                 /*主控硬件参数配置信息*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void GroupInitStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组初始化工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupInitStateHandle(void);

/*=================================================================================================
** @Name      : static void GroupIdleStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组待命工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupIdleStateHandle(void);

/*=================================================================================================
** @Name      : static void GroupStartStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组启动工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupStartStateHandle(void);

/*=================================================================================================
** @Name      : static void GroupRunStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组运行工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupRunStateHandle(void);

/*=================================================================================================
** @Name      : static void GroupStopStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组停机工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupStopStateHandle(void);

/*=================================================================================================
** @Name      : static void GroupErrStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组故障工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupErrStateHandle(void);

/*=================================================================================================
** @Name      : static void GroupOffStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组关机工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupOffStateHandle(void);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void GroupStateInfoInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组状态信息初始化
** @The notes : 保护状态/工作状态/充放电状态
**===============================================================================================*/
void GroupStateInfoInit(void)
{
    gGSysStateInfo_50[eSysStateInfo50_SysPro] = ePRO_WAIT;
    gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_INIT;
    gGSysStateInfo_50[eSysStateInfo50_SysChg] = eCURR_IDLE;
}

/*=================================================================================================
** @Name      : void GroupProStateTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组保护状态检测任务
** @The notes : 100ms周期调用
**===============================================================================================*/
void GroupProStateTask(void)
{
    /*根据系统状态和故障级别判断保护状态*/
    u16 state = 0;
    u16 syst = 0;
    u16 sysPro = 0;
    u16 askCPro = 0;
    u16 askDPro = 0;
    u16 expSort = 0;

    syst = GetGWorkStateAPI();
    sysPro = TacGetGAskSysProFlagAPI();
    askCPro = TacGetGAskChgProFlagAPI();
    askDPro = TacGetGAskDhgProFlagAPI();
    expSort = TacGetGNowErrLevelHook(0);

    if((eWORK_ERR == syst) || (eErr_SysErr == expSort)          /*系统故障或产生故障类异常*/
        || (eErr_SysPro == expSort) || (eErr_ChgPro == expSort) /*产生充电或放电或充放电保护类故障*/
        || (eErr_DhgPro == expSort) || (sysPro > 0)
        || (askCPro > 0) || (askDPro > 0))                      /*请求充电或放电或充放电保护*/
    {
        state = ePRO_SYSPRO;                                    /*系统保护状态(包括故障保护/充放电保护/充电保护/放电保护)*/
    }
    else if(eErr_SysLim == expSort)                             /*产生请求充放电限制类故障*/
    {
        state = ePRO_SYSLIM;                                    /*充放电限制状态*/
    }
    else if(eErr_DhgLim == expSort)                             /*放电限制故障*/
    {
        if((2 == TacGetGroupBattStateAPI())                     /*电池放空*/
            || (1 == TacGetGroupBattVLStateHook()))             /*电池欠压二级放电截止*/
        {
            state = ePRO_EMPTY;                                 /*放电截止状态*/
        }
        else                                                    /*非电池放空*/
        {
            state = ePRO_SYSLIM;                                /*放电限制状态*/
        }
    }
    else if(eErr_ChgLim == expSort)                             /*充电电限制故障*/
    {
        if((1 == TacGetGroupBattStateAPI())                     /*电池充满*/
            || (1 == TacGetGroupBattVHStateHook()))             /*电池过压二级充电截止*/
        {
            state = ePRO_FULL;                                  /*充电截止状态*/
        }
        else                                                    /*非电池充满*/
        {
            state = ePRO_SYSLIM;                                /*充电限制状态*/
        }
    }
    else if(2 == TacGetGroupBattStateAPI())                     /*电池放空*/
    {
        state = ePRO_EMPTY;                                     /*放电截止状态*/
    }
    else if(1 == TacGetGroupBattStateAPI())                     /*电池充满*/
    {
        state = ePRO_FULL;                                      /*充电截止状态*/
    }
    else if(eWORK_RUN != syst)                                  /*系统未运行*/
    {
        state = ePRO_WAIT;                                      /*未就绪状态*/
    }
    else                                                        /*无充放电限制*/
    {
         state = ePRO_READY;                                    /*正常就绪状态*/
    }

    /*电池保护状态变化记录事件*/
    if(state != GetGProtectStateAPI())
    {
        TacGroupProStateChangDealHook(GetGProtectStateAPI(), state);
    }

    /*电池保护状态更新*/
    gGSysStateInfo_50[eSysStateInfo50_SysPro] = state;
}

/*=================================================================================================
** @Name      : void void GroupWorkStateTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组工作状态检测任务
** @The notes : 100ms周期调用
**===============================================================================================*/
void GroupWorkStateTask(void)
{
    u16 state = 0;
    static u8 sHisState = 0;

    state = gGSysStateInfo_50[eSysStateInfo50_SysWor];

    /*外部控制电池组工作状态变换处理*/
    if(sHisState != state)
    {
    	TacGroupWorkStateChangDealHook(sHisState, state);
    	sHisState = state;
    }

    switch(state)
    {
        case eWORK_INIT:            /*初始化状态*/
            GroupInitStateHandle();
            break;

        case eWORK_IDLE:            /*待命状态*/
        	GroupIdleStateHandle();
            break;

        case eWORK_START:           /*启动状态*/
        	GroupStartStateHandle();
            break;

        case eWORK_RUN:             /*运行状态*/
        	GroupRunStateHandle();
            break;

        case eWORK_STOP:            /*停机状态*/
        	GroupStopStateHandle();
            break;

        case eWORK_ERR:             /*故障状态*/
        	GroupErrStateHandle();
            break;

        case eWORK_OFF:             /*关机状态*/
        	GroupOffStateHandle();
            break;

        default:
            break;
    }

    /*内部检测电池组工作状态变换处理*/
    if(sHisState != gGSysStateInfo_50[eSysStateInfo50_SysWor])
    {
    	TacGroupWorkStateChangDealHook(sHisState, gGSysStateInfo_50[eSysStateInfo50_SysWor]);
    }
}

/*=================================================================================================
** @Name      : void void GroupChgDhgStateTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组充放电状态检测任务
** @The notes : 100ms周期调用
**===============================================================================================*/
void GroupChgDhgStateTask(void)
{
    s16 curr = 0;
    u16 state = 0;
    static u8 sTimes = 0;
    static u8 sChgCount = 0;
    static u8 sDhgCount = 0;

    curr = GetGSampOutCurrAPI();

    /*电流大于1A放电*/
    if(curr >= gGHardPara_104[eGHardPara104_DhgIdlePoint])
    {
    	sDhgCount++;
    }
    /*电流大于1A充电*/
    else if(curr <= (0 - (s16)gGHardPara_104[eGHardPara104_ChgIdlePoint]))
    {
    	sChgCount++;
    }

    if(sTimes >= 20)              //检测2s
    {
        if(sDhgCount >= 15)       //长时间处于放电状态
        {
            state = 2;
        }
        else
        {
            if(sChgCount >= 15)   //长时间处于充电状态
            {
                state = 1;
            }
            else                //长时间处于停止状态
            {
                state = 0;
            }
        }

        //状态清零
        sTimes = 0;
        sChgCount = 0;
        sDhgCount = 0;
    }
    else
    {
        sTimes++;
        state = gGSysStateInfo_50[eSysStateInfo50_SysChg];
    }

    //充放电状态变化记录事件
    if(state != gGSysStateInfo_50[eSysStateInfo50_SysChg])
    {
    	TacGroupChgStateChangDealHook(gGSysStateInfo_50[eSysStateInfo50_SysChg], state);
    }

    /*电池充放电状态更新*/
    gGSysStateInfo_50[eSysStateInfo50_SysChg] = state;
}

/*=================================================================================================
** @Name      : void GroupBattStateCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组电池电压状态检测任务
** @The notes : 100ms周期调用
**===============================================================================================*/
void GroupBattStateCheckTask(void)
{
    u32 nowCAP = 0;
    u32 allCAP = 0;
    u8  SOCChang = 0;
    static u8 voltHTimes = 0;
    static u8 voltLTimes = 0;
    static u16 sChgOrDhgCap = 0;

    //从控异常则不检测电池电压状态
    if((eWORK_INIT == GetGWorkStateAPI())
        || (FALSE == TacGetGAllSlaveReadyHook()))
    {
        voltHTimes = 0;
        voltLTimes = 0;
        sChgOrDhgCap = 0;
        sGroupBattState = 0;
        return;
    }

    if((1 == TacGetGroupBattVHStateHook())                  //充电高压
    	&& (0 == TacGetGroupBattVLStateHook()))             //无放电低压
    {
        voltLTimes = 0;                                     //低压判断清零

        if(voltHTimes >= 3)                                 //判断3次高压
        {
            voltHTimes = 3;
            if(2 == sGroupBattState)                        //当前放空未充电
            {
                return;                                     //保持原状态
            }

            sGroupBattState = 1;                            //设置电池组高压标志
            sChgOrDhgCap = TacGetGroupDhgCapHook();         //记录高压时放电容量值
        }
        else
        {
            voltHTimes++;
        }
    }
    else if((1 == TacGetGroupBattVLStateHook())             //放电低压
    	&& (0 == TacGetGroupBattVHStateHook()))             //无充电高压
    {
        voltHTimes = 0;                                     //高压判断次数清零

        if(voltLTimes >= 3)                                 //判断3次为低压
        {
            voltLTimes = 3;
            if(1 == sGroupBattState)                        //当前为充满未放电
            {
                return;                                     //保持原状态
            }
            sGroupBattState = 2;                            //设置电池组低压标志
            sChgOrDhgCap = TacGetGroupChgCapHook();         //记录低压时充电容量值
        }
        else
        {
            voltLTimes++;
        }
    }
    else                                                    //无高低压一般告警
    {
        voltHTimes = 0;
        voltLTimes = 0;

        switch(sGroupBattState)                             //根据电池组状态标志是否恢复正常状态
        {
            case 0:                                         //无高低压,正常计算电流限值
                break;

            case 1:                                         //当前已记录电池组为高压
                nowCAP = TacGetGroupDhgCapHook();           //当前放电量:AH
                allCAP = GetGBattAllCapAPI() / 10;          //电池总容量:AH

                if((GetGCellMaxVoltAPI() <= gGBmuGenPara_102[eBmuGenPara102_ChgLimClrV])
                    && (eCURR_IDLE == GetGChgDhgStateAPI()))//待机时整体电压很低
                {
                	sGroupBattState = 0;                    //强制恢复电池为正常
                    return;
                }
                else if(nowCAP < sChgOrDhgCap)              //计算异常
                {
                    nowCAP = sChgOrDhgCap;
                }

                //放电SOC变化到达3%
                SOCChang = (u8)((nowCAP - sChgOrDhgCap) * 1000 / allCAP);
                if(SOCChang >= gGBmuGenPara_102[eBmuGenPara102_DhgLimSubSoc])
                {
                	sGroupBattState = 0;
                }
                break;

            case 2:                                         //当前已记录为低压
                nowCAP = TacGetGroupChgCapHook();           //当前充电量:AH
                allCAP = GetGBattAllCapAPI() / 10;          //电池总容量:AH

                if((GetGCellMinVoltAPI() >= gGBmuGenPara_102[eBmuGenPara102_DhgLimClrV])
                    && (eCURR_IDLE == GetGChgDhgStateAPI()))//待机时整体电压很高
                {
                	sGroupBattState = 0;                    //强制恢复电池为正常
                    return;
                }
                else if(nowCAP < sChgOrDhgCap)              //计算异常
                {
                    nowCAP = sChgOrDhgCap;
                }

                //充电SOC变化到达3%
                SOCChang = (u8)((nowCAP - sChgOrDhgCap) * 1000 / allCAP);
                if(SOCChang >= gGBmuGenPara_102[eBmuGenPara102_DhgLimSubSoc])
                {
                	sGroupBattState = 0;
                }
                break;

            default:
                break;
        }
    }
}

/*=================================================================================================
** @Name      : void TacSetGroupWorkStateAPI(e_WorkState state)
** @Input     : state:电池组工作状态[0,6]
** @Output    : void
** @Function  : 设置电池组工作状态
** @The notes :
**===============================================================================================*/
void TacSetGroupWorkStateAPI(e_WorkState state)
{
    if(state >= eWORK_STATE)
    {
        return;
    }

    gGSysStateInfo_50[eSysStateInfo50_SysWor] = state;
}

/*=================================================================================================
** @Name      : void TacResetGroupBattStateAPI(void)
** @Input     : void
** @Output    : 电池组电池状态: 1充满 2放空
** @Function  : 复位电池组电池状态
** @The notes : 电池电压状态清零(0正常)
**===============================================================================================*/
void TacResetGroupBattStateAPI(void)
{
    sGroupBattState = 0;
}

/*=================================================================================================
** @Name      : u8 TacGetGroupBattStateAPI(void)
** @Input     : void
** @Output    : 电池组电池状态:0正常 1充满 2放空
** @Function  : 获取电池组电池状态
** @The notes : 电压状态
**===============================================================================================*/
u8 TacGetGroupBattStateAPI(void)
{
    return(sGroupBattState);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void GroupInitStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组初始化工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupInitStateHandle(void)
{
    u8 result = 0;
    static u8 sState = 0;
    static u16 sTimer = 0;

    switch(sState)
    {
        /*断开所有运行接触器*/
        case 0:
        	TacCtrlGroupRunSwhAllOffAPI();
        	sTimer = 0;
            sState = 1;
            break;

        /*判断接触器回检是否正常*/
        case 1:
            if(1 == TacGetGroupRunSwhAllOffStateAPI())                      /*运行接触器全断开*/
            {
            	#if(0 == BMU_DC_PWR_TYPE)                                   /*自供电按钮上下电*/
            	TacCtrlGroupSwhStateAPI(eGroupSwh_Pwr, ePSTATE_ON);         /*闭合主控电源接触器*/
				#else                                                       /*自供电手动上下电*/
            	TacCtrlGroupSwhStateAPI(eGroupSwh_Pwr, ePSTATE_OFF);        /*断开主控电源接触器电源(高电平弹开使断电)*/
				#endif
                sTimer = 0;
                sState = 2;                                                 /*进入下一步*/
            }
            else                                                            /*有接触器回检异常*/
            {
                if(sTimer >= 10)                                            /*等待接触器回检超时*/
                {
                	sTimer = 0;
                    sState = 10;                                            /*进入等待主控上报故障*/
                }
                else                                                        /*等待未超时*/
                {
                	sTimer++;
                	TacCtrlGroupRunSwhAllOffAPI();                          /*控制运行接触器断开*/
                }
            }
            break;

		/*等待接收到所有从从设备就绪*/
		case 2:
			if((TRUE == TacGetGAllSlaveReadyHook())                         /*所有从控均通信就绪*/
				&& (TRUE == TacGetGAllSubdevReadyHook())       				/*所有子系统均通信就绪*/
			    && (TRUE == TacGetGAllPeriphReadyHook()))                   /*所有外部设备器件就绪*/
			{
				sTimer = 0;
				sState = 10;                                                /*进入下一步*/
			}
			else                                                            /*从控异常*/
			{
				if(sTimer >= 50)                                            /*等待接从控超时*/
				{
					sTimer = 0;
					sState = 10;                                            /*进入等待主控上报故障*/
				}
				else                                                        /*等待未超时*/
				{
					sTimer++;
				}
			}
			break;

        /*电池外部故障检测*/
        case 10:
            result = TacGroupInitStateResultHook();

            if(eSYSRESULT_ERR == result)                                    /*检测故障*/
            {
            	gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_ERR;      /*故障状态*/
                sTimer = 0;
                sState = 0;
            }
            else if(eSYSRESULT_FIN == result)                               /*检测完成*/
            {
            	gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_IDLE;     /*空闲状态*/
                sTimer = 0;
                sState = 0;
            }
            break;

        default:
            break;
    }
}

/*=================================================================================================
** @Name      : static void GroupIdleStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组待命工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupIdleStateHandle(void)
{
    if(eErr_SysErr == TacGetGNowErrLevelHook(0))                            /*检测故障*/
    {
    	gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_ERR;              /*故障状态*/
    }
    else                                                                    /*检测等待*/
    {
        if(TRUE == TacGetGroupRunSwhAllOffStateAPI())                       /*运行接触器全断开*/
        {
        	gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_IDLE;         /*系统空闲*/
        }
        else                                                                /*运行接触器有闭合*/
        {
            if(GetGWorkModeAPI() == eMODE_DEBUG)                            /*正在调试*/
            {
                /*根据接触器状态判断系统状态*/
            	gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_START;    /*正在启动状态*/
            }
            else                                                            /*正常模式无故障*/
            {
            	TacCtrlGroupRunSwhAllOffAPI();                              /*断开所有运行接触器*/
                gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_IDLE;     /*系统空闲*/
            }
        }
    }
}

/*=================================================================================================
** @Name      : static void GroupStartStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组启动工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupStartStateHandle(void)
{
    if(eErr_SysErr == TacGetGNowErrLevelHook(0))                            /*检测故障*/
    {
        /*记录启动失败*/
    	TacSetGStartFailReason(eStartFailReason_SysFault);
        gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_ERR;              /*故障状态*/
    }
    else                                                                    /*检测等待*/
    {
        if(GetGWorkModeAPI() == eMODE_DEBUG)                                /*正在调试*/
        {
            /*根据接触器状态判断系统状态*/
            if(TRUE == TacGetGroupRunSwhAllOffStateAPI())                   /*运行接触器全断开*/
            {
                gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_IDLE;     /*系统空闲*/
            }
            else if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre))/*接触器有闭合且预充接触器闭合*/
            {
                gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_START;    /*正在启动状态*/
            }
            else if((ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))/*主正接触器有闭合且预充接触器断开*/
            	&& (ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg)))/*负极接触器有闭合且预充接触器断开*/
            {
                gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_RUN;      /*运行状态*/
            }
            else                                                            /*接触器有闭合且预充接触器断开*/
            {
                gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_START;    /*正在启动状态*/
            }
        }
        else                                                                /*正常模式无故障*/
        {
        	TacGroupStartCtrlTactic();                                      /*系统启动控制*/
        }
    }
}

/*=================================================================================================
** @Name      : static void GroupRunStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组运行工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupRunStateHandle(void)
{
    if(eErr_SysErr == TacGetGNowErrLevelHook(0))                            /*检测故障*/
    {
        gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_ERR;              /*故障状态*/
    }
    else                                                                    /*检测等待*/
    {
        if(GetGWorkModeAPI() == eMODE_DEBUG)                                /*正在调试*/
        {
            /*根据接触器状态判断系统状态*/
            if(TRUE == TacGetGroupRunSwhAllOffStateAPI())                   /*运行接触器全断开*/
            {
                gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_IDLE;     /*系统空闲*/
            }
            else if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre))/*接触器有闭合且预充接触器闭合*/
            {
                gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_START;    /*正在启动状态*/
            }
            else                                                            /*接触器有闭合且预充接触器断开*/
            {
                gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_RUN;      /*运行状态*/
            }
        }
        else                                                                /*正常模式无故障*/
        {
            if((TRUE == TacGetGroupRunSwhAllOffStateAPI())                  /*运行接触器全断开*/
            	|| (1 == TacGetUserReqStopStateHook()))                     /*请求停机*/
            {
            	TacSetGWorkStateCmdAPI(eCTRL_NORSTOP);                      /*设置正常停止*/
            }
            else
            {
                if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre)) /*预充接触器闭合*/
                {
                	TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_OFF);    /*控制断开*/
                }
            }
        }
    }
}

/*=================================================================================================
** @Name      : static void GroupStopStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组停机工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupStopStateHandle(void)
{
    if(eErr_SysErr == TacGetGNowErrLevelHook(0))                            /*检测故障*/
    {
    	TacSetGStopFailReason(eStopFailReason_SysFault);
        gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_ERR;              /*故障状态*/
    }
    else                                                                    /*检测等待*/
    {
        if(GetGWorkModeAPI() == eMODE_DEBUG)                                /*正在调试*/
        {
            /*根据接触器状态判断系统状态*/
            if(TRUE == TacGetGroupRunSwhAllOffStateAPI())                   /*运行接触器全断开*/
            {
                gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_IDLE;     /*系统空闲*/
            }
            else if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre))/*接触器有闭合且预充接触器闭合*/
            {
                gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_START;    /*正在启动状态*/
            }
            else                                                            /*接触器有闭合且预充接触器断开*/
            {
                gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_RUN;      /*运行状态*/
            }
        }
        else                                                                /*正常模式无故障*/
        {
        	TacGroupNorStopCtrlTactic();                                    /*系统停机控制*/
        }
    }
}

/*=================================================================================================
** @Name      : static void GroupErrStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组故障工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupErrStateHandle(void)
{
    static u8 sTime = 0;

    if(GetGWorkModeAPI() != eMODE_DEBUG)                                    /*非调试模式*/
    {
    	TacGroupErrStopCtrlTactic();                                        /*系统故障停机控制*/
    }

    if(eWORK_OFF == GetGWorkStateAPI())                                     /*切换为关机状态*/
    {
    	sTime = 0;
    	if(eErr_SysErr != TacGetGNowErrLevelHook(0))                        /*故障消除*/
    	{
    		TacClrGCtrlErrFlagAPI();                                        /*清除系统控制故障*/
    	}
    }
    else if((eErr_SysErr != TacGetGNowErrLevelHook(0))                      /*故障消除*/
        && (TRUE == TacGetGroupRunSwhAllOffStateAPI()))                     /*运行接触器全断开*/
    {
        if(sTime >= 50)                                                     /*延迟5s*/
        {
            sTime = 0;
            TacClrGCtrlErrFlagAPI();                                        /*清除系统控制故障*/
        	gGSysStateInfo_50[eSysStateInfo50_SysWor] = eWORK_IDLE;         /*进入空闲待命状态*/
        }
        else                                                                /*等待延时*/
        {
            sTime++;
        }
    }
    else                                                                    /*故障保持*/
    {
        if(TRUE == TacGetGroupRunSwhAllOffStateAPI())                       /*运行接触器全断开*/
        {
            if(sTime >= 50)                                                 /*延迟5s*/
            {
                /*sTime = 0;*/
            	TacClrGCtrlErrFlagAPI();                                    /*清除系统控制故障*/
            }
            else                                                            /*等待延时*/
            {
                sTime++;
            }
        }
        else                                                                /*运行接触器未全断开*/
        {
            sTime = 0;
        }
    }
}

/*=================================================================================================
** @Name      : static void GroupOffStateHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组关机工作状态处理函数
** @The notes : 工作状态检测100ms周期任务调用
**===============================================================================================*/
static void GroupOffStateHandle(void)
{
	/*关机状态不检测故障状态*/
    if(eMODE_DEBUG != GetGWorkModeAPI())                        /*非调试模式*/
    {
        TacGroupPwrOffCtrlTactic();                             /*系统关机控制处理*/
    }
}

