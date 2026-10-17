/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : GroupTactic.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池控制策略模块接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "GroupPara.h"
#include "TacInterface.h"
#include "TacUser.h"
#include "GroupInfo.h"
#include "GroupState.h"
#include "GroupProtect.h"
#include "DiagInterface.h"
#include "GroupTactic.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sGroupWorkCtrlCmd = 0;                            /*记录接收到的工作状态控制命令[0,8]: 0:正常停机 1:正常预充 2:正常启动 3:充电启动 4:放电启动 5:强制停机 6:紧急停机 7:关机 8:开机*/
static u8 sGroupStartStep = 0xff;                           /*电池组启动步骤(高压上电步骤)*/
static u8 sGroupStopStep = 0xff;                            /*电池组停机步骤(高压下电步骤)*/
static u8 sEmergStopFlag = 0;                               /*电池组急停标志(按位表示):Bit0:系统内部急停 Bit1:外部命令急停 bit2:按钮急停*/

static u8 sGroupStartFailFlag = 0;                          /*电池组启动异常标志(按位表示):Bit0:系统故障 Bit1:启动受限 Bit2:接触器异常 Bit3:预充失败*/
static u8 sGroupStopFailFlag = 0;                           /*电池组停机异常标志(按位表示):Bit0:系统故障 Bit1:等待电流失败 Bit2:接触器异常 Bit3:断开接触器失败*/

static u16 sGSwhOnCtrlFailFlag = 0;                         /*电池组接触器控制闭合失败标志(按位表示):Bit0:主正接触器 Bit1:预充接触器 Bit2:负极接触器 Bit3:电源接触器*/
static u16 sGSwhOffCtrlFailFlag = 0;                        /*电池组接触器控制断开失败标志(按位表示):Bit0:主正接触器 Bit1:预充接触器 Bit2:负极接触器 Bit3:电源接触器*/

#if(1 == BMU_SMG_SWH_EN)									/*烟感电源控制使能(有烟感检测时)*/
static u8 sSmgAlarmState = 0;								/*烟感报警状态(包括电源重启确认真实信号,0:正常 1:正在核实告警 2:已确认告警)*/
#endif

extern u16 gGBmuHigLevPara_103[];							/*主控高级参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void TacSetGWorkStateCmdAPI(u8 cmd)
** @Input     : cmd:电池组工作状态切换控制命令[0,8]:0:正常停止 1:正常预充 2:正常启动 3:充电启动 4:放电启动 5:强制停机 6:紧急停机 7:下电关机 8:上电开机
** @Output    : void
** @Function  : 设置电池组工作状态切换控制命令
** @The notes : 接收到通信命令或按钮命令
**===============================================================================================*/
void TacSetGWorkStateCmdAPI(u8 cmd)
{
	/*分辨命令值*/
    switch(cmd)
    {
        /*正常停机命令*/
        case eCTRL_NORSTOP:
        /*强制停机命令*/
        case eCTRL_FORCSTOP:
            if((eWORK_START == GetGWorkStateAPI())                  /*正在启动*/
                || (eWORK_RUN == GetGWorkStateAPI()))               /*正在运行*/
            {
                sGroupStopStep = 0;                                 /*复位停机步骤*/

                sGroupWorkCtrlCmd = cmd;                            /*记录控制命令*/
                TacClrGCtrlErrFlagAPI();                            /*清除控制失败标志*/
                TacSetGroupWorkStateAPI(eWORK_STOP);                /*切换为正在停止*/

                sGroupStartStep = 0xff;                             /*清除启动步骤*/
            }/*
            else if(eWORK_OFF == GetGWorkStateAPI())                //正在关机
            {
                sGroupStartStep = 0xff;                             //清除启动步骤
                sGroupStopStep = 0xff;                              //清除停机步骤

                sGroupWorkCtrlCmd = eCTRL_POWERON;                  //记录为开机命令
                TacClrGCtrlErrFlagAPI();                            //清除控制失败标志
                //TacSetGroupWorkStateAPI(eWORK_INIT);              //切换为初始化状态(由开机完成后切换)
            }*/
            else if((eWORK_IDLE == GetGWorkStateAPI())              /*系统空闲*/
                || (eWORK_ERR == GetGWorkStateAPI()))               /*系统故障*/
            {
                sGroupWorkCtrlCmd = cmd;                            /*记录控制命令*/
                TacClrGCtrlErrFlagAPI();                            /*清除控制失败标志*/

                sGroupStartStep = 0xff;                             /*清除启动步骤*/
                sGroupStopStep = 0xff;                              /*清除停机步骤*/
            }
            break;

        /*正常预充命令*/
        case eCTRL_PRECHG:
        /*正常启动命令*/
        case eCTRL_NORSTART:
        /*充电启动命令*/
        case eCTRL_CHGSTART:
        /*放电启动命令*/
        case eCTRL_DHGSTART:
            if((eWORK_IDLE == GetGWorkStateAPI())                   /*正空闲待命*/
               && (1 == TacGetGroupFitStartStateHook()))            /*适合启动*/
            {
                sGroupStartStep = 0;                                /*复位启动步骤*/
                sGroupWorkCtrlCmd = cmd;                            /*记录控制命令*/
                TacClrGCtrlErrFlagAPI();                            /*清除控制失败标志*/
                TacSetGroupWorkStateAPI(eWORK_START);               /*切换为正在启动*/

                sGroupStopStep = 0xff;                              /*清除停机步骤*/
            }
            else if((eWORK_START == GetGWorkStateAPI())             /*正在启动*/
                && (eCTRL_PRECHG == sGroupWorkCtrlCmd)              /*正在预充*/
                && (eCTRL_NORSTART == cmd))                         /*只有预充允许变换正常启动命令*/
            {
                sGroupWorkCtrlCmd = cmd;                            /*更新控制命令*/

                sGroupStopStep = 0xff;                              /*清除停机步骤*/
            }
            break;

        /*紧急停机命令*/
        case eCTRL_EMERSTOP:
            sGroupStopStep = 0xff;
            sGroupStartStep = 0xff;

            if((eWORK_START == GetGWorkStateAPI())                  /*正在启动*/
                || (eWORK_RUN == GetGWorkStateAPI())                /*正在运行*/
                || (eWORK_STOP == GetGWorkStateAPI())               /*正在停机*/
                || (eWORK_ERR == GetGWorkStateAPI()))               /*系统故障*/
            {
                sGroupWorkCtrlCmd = cmd;                            /*记录控制命令*/
                TacSetGroupWorkStateAPI(eWORK_ERR);                 /*切换为故障状态*/

                if(1 == TacGetUserButtonEmergStateHook())           /*按钮信号*/
                {
                    TacSetGEmergStopReason(eEmergStopReason_Button);/*记录按钮急停*/
                }
                else                                                /*外部命令*/
                {
                    TacSetGEmergStopReason(eEmergStopReason_ExtCmd);/*记录命令急停*/
                }
            }
            break;

        /*关机机命令*/
        case eCTRL_POWEROFF:
            sGroupStartStep = 0xff;

            if((eWORK_START == GetGWorkStateAPI())                  /*正在启动*/
                || (eWORK_RUN == GetGWorkStateAPI()))               /*正在运行*/
            {
                sGroupStopStep = 0;
                sGroupWorkCtrlCmd = cmd;                            /*记录控制命令*/
                TacSetGroupWorkStateAPI(eWORK_STOP);                /*切换为正在停止状态*/
            }
            else if(eWORK_IDLE == GetGWorkStateAPI())               /*系统空闲待命*/
            {
                sGroupStopStep = 0xff;
                sGroupWorkCtrlCmd = cmd;                            /*记录控制命令*/
                TacSetGroupWorkStateAPI(eWORK_OFF);                 /*切换为关机状态*/
            }
            else if((eWORK_ERR == GetGWorkStateAPI())               /*系统故障*/
                || (eWORK_STOP == GetGWorkStateAPI()))              /*正在停机*/
            {
                sGroupWorkCtrlCmd = cmd;                            /*记录控制命令*/
                /*TacSetGroupWorkStateAPI(eWORK_OFF);*/             /*切换为关机状态(由停机完成后切换)*/
            }
            break;

        /*开机命令*/
        case eCTRL_POWERON:
            if((eWORK_OFF == GetGWorkStateAPI())                    /*系统关机*/
                || (eMODE_DEBUG == GetGWorkModeAPI()))              /*正在调试*/
            {
                sGroupStopStep = 0xff;
                sGroupStartStep = 0xff;

                sGroupWorkCtrlCmd = cmd;                            /*记录控制命令*/
                TacClrGCtrlErrFlagAPI();                            /*清除控制失败标志*/
                /*TacSetGroupWorkStateAPI(eWORK_INIT);*/            /*切换为初始化状态(由开机完成后切换)*/
            }
            break;

        default:
            break;
    }
}

/*=================================================================================================
** @Name      : void TacGHeatManagTacticTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组热管理策略任务
** @The notes : 1s周期调用,控制风扇和加热片
**===============================================================================================*/
void TacGHeatManagTacticTask(void)
{
	/*系统调试模式不进行控制*/
    if(eMODE_DEBUG == GetGWorkModeAPI())
    {
        return;
    }

    //定速风扇接触器(有风扇接触器且不是控制空调电源时)
	#if((1 == BMU_FAN_SWH_EN) && (0 == BMU_FAN_PWR_EN))
	//火警或单体温度异常必须开启风扇
	if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//消防告警
		|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//烟雾告警
		|| (FALSE == TacGetGAllSlaveReadyHook()))                       //从控信息异常
	{
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_ON);             //闭合风扇接触器
	}
	//系统开机关机必须关闭风扇
	else if((eWORK_INIT == GetGWorkStateAPI())                          //系统开机
		|| (eWORK_OFF == GetGWorkStateAPI()))                           //系统关机
	{
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_OFF);            //断开风扇接触器
	}
	//最大温度值大于40℃或者平均温度大于35℃吸合风扇控制器
	else if((GetGCellMaxTempAPI() >= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp])
		|| (GetGCellAvgTempAPI() >= ((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp] - 2)))
	{
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_ON);             //吸合风扇接触器
	}
	//最大温度值和平均温度都小于30℃则关闭风扇控制器
	else if((GetGCellMaxTempAPI() <= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp])
		&& (GetGCellAvgTempAPI() <= ((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp] - 1)))
	{
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_OFF);            //断开风扇接触器
	}
	#endif

	/*PWM调速风扇电源控制功能(复用风扇控制管脚)*/
	#if((1 == BMU_FAN_SWH_EN) && (1 == BMU_FAN_PWR_EN))
	//烟感告警或气体浓度异常或单体温度异常必须开启风扇
	if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_CGAS_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_CGAS_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//气体告警
		|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//烟雾告警
		|| (FALSE == TacGetGAllSlaveReadyHook()))                       //从控信息异常
	{
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_ON);             //闭合风扇接触器
	}
	//系统开机关机或已起火必须关闭风扇
	else if((eWORK_INIT == GetGWorkStateAPI()) || (eWORK_OFF == GetGWorkStateAPI())//系统开机关机
		|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))) //消防告警
	{
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_OFF);            //断开风扇接触器
	}
	//最大温度值大于开始调速温度吸合风扇控制器
	else if(GetGCellMaxTempAPI() >= ((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp] - 1))
	{
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_ON);             //吸合风扇接触器
	}
	//最大温度值小于开始调速温度3℃则关闭风扇控制器
	else if(GetGCellMaxTempAPI() <= ((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp] - 3))
	{
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_OFF);            //断开风扇接触器
	}
	#endif

	/*预留空调电源控制功能(复用加热控制管脚)*/
	#if((1 == BMU_HOT_SWH_EN) && (1 == BMU_AIR_PWR_EN))
	//火警或单体温度异常必须开启风扇
	if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//消防告警
		|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//烟雾告警
		|| (FALSE == TacGetGAllSlaveReadyHook()))                       //从控信息异常
	{
		//闭合空调电源
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_ON);
	}
	//系统开机关机必须关闭风扇
	else if((eWORK_INIT == GetGWorkStateAPI())                          //系统开机
		|| (eWORK_OFF == GetGWorkStateAPI()))                           //系统关机
	{
		//断开空调电源
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_OFF);
	}
	else if(GetGCellMaxTempAPI() >= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp])
	{
		//闭合空调电源
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_ON);
	}
	else if(GetGCellMaxTempAPI() <= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp])
	{
		//断开空调电源
		TacCtrlGroupSwhStateAPI(eGroupSwh_Fan, ePSTATE_OFF);
	}
	#endif

    //加热接触器(有加热接触器时)
	#if((1 == BMU_HOT_SWH_EN) && (0 == BMU_AIR_PWR_EN))
	{
		//系统开机关机或单体温度采样异常必须关闭加热
		if((eWORK_INIT == GetGWorkStateAPI())                               //系统开机
			|| (eWORK_OFF == GetGWorkStateAPI())                            //系统关机
			|| (FALSE == TacGetGAllSlaveReadyHook()))                       //温度异常
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Hot, ePSTATE_OFF);            //断开加热接触器
		}
		//最小温度值低于0℃或者平均温度低于2℃吸合加热控制器
		else if((GetGCellMinTempAPI() <= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_HotOpenTemp])
			|| (GetGCellAvgTempAPI() <= ((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_HotOpenTemp] + 2)))
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Hot, ePSTATE_ON);             //吸合风扇接触器
		}
		//最低温度值和平均温度都大于10℃则关闭加热控制器
		else if((GetGCellMinTempAPI() >= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_HotCloseTemp])
			&& (GetGCellAvgTempAPI() >= ((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_HotCloseTemp] + 1)))
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Hot, ePSTATE_OFF);            //断开加热接触器
		}
	}
	#endif
}

/*=================================================================================================
** @Name      : void TacGPeriManagTacticTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组外围管控策略任务
** @The notes : 1s周期调用,控制显示灯、输出干接点和ACC激活保持信号
**===============================================================================================*/
void TacGPeriManagTacticTask(void)
{
    u16 expSort = 0;
    //static u8 sDely = 0;
    static u8 sTime = 0;
	e_GPIOState pcbLedState = ePSTATE_OFF;
    static e_GPIOState sStateTwin = ePSTATE_OFF;                    //连续快闪
    static e_GPIOState sStateTwin0 = ePSTATE_OFF;                   //连续慢闪

	//显示灯LED接触器(有LED时)
	#if(1 == BMU_LED_SWH_EN)
	e_GPIOState runLedState = ePSTATE_OFF;
	e_GPIOState errLedState = ePSTATE_OFF;
    static e_GPIOState sStateTwin1 = ePSTATE_OFF;                   //1闪1灭
    static e_GPIOState sStateTwin2 = ePSTATE_OFF;                   //2闪1灭
    #endif

    //烟感电源控制使能(有烟感检测时)
	#if(1 == BMU_SMG_SWH_EN)
    static u8 sSmgOffStep = 0;
    static u8 sSmgOffFlag = 0;
    static u8 sSmgOffTime = 0;
	#endif

	/*ACC/快充激活信号控制策略*/
	if(0 == TacGetGroupACCPwrOffReqStateHook())                             /*未请求下电*/
	{
		//sDely = 0;

		FS65SetUserPwrOffFlagAPI(0);                                        /*清除SBC下电请求(正常喂狗)*/
		DEVGPIOSetPortState(ePOut_PwHold, ePSTATE_ON);                      /*控制上电ACC激活信号电源保持*/
	}
	else                                                                    /*ACC电压断开下电*/
	{
		//if(sDely >= 3)                                                    /*延迟3s断开保持下电*/
		{
			if(2 == TacGetGroupACCPwrOffReqStateHook())                     /*需要执行下电*/
			{
				DEVGPIOSetPortState(ePOut_PwHold, ePSTATE_OFF);             /*控制电源保持信号电源切断(低压下电)*/
				FS65SetUserPwrOffFlagAPI(1);                                /*请求SBC下电(停止喂狗)*/
			}
		}
		//else
		{
			//sDely++;
		}
	}

	/*系统调试模式不进行控制*/
    if(eMODE_DEBUG == GetGWorkModeAPI())
    {
    	/*调试模式只自动控制板上LED*/
		if(sTime >= 3)
		{
			sTime = 0;
			DEVGPIOSetPortState(ePOut_LED, ePSTATE_OFF);
		}
		else
		{
			sTime++;
			DEVGPIOSetPortState(ePOut_LED, ePSTATE_ON);
		}
        return;
    }

	/*板上LED/运行LED/故障LED亮灭状态显示控制*/
	if(BSPGetOSMillSecTimeAPI() <= 400)                                 /*开机LED亮400ms*/
	{
		//闭合板上LED、运行LED和故障LED
		pcbLedState = ePSTATE_ON;

		//显示灯LED接触器(有LED时)
		#if(1 == BMU_LED_SWH_EN)
		runLedState = ePSTATE_ON;
		errLedState = ePSTATE_ON;
		#endif
	}
	/*运行状态LED显示*/
	else if(eWORK_RUN == GetGWorkStateAPI())                            //系统运行
	{
		pcbLedState = ePSTATE_ON;                                       //板上LED常亮

		//显示灯LED接触器(有LED时)
		#if(1 == BMU_LED_SWH_EN)
		runLedState = ePSTATE_ON;                                       //运行状态LED常亮

		if(1 == TacGetGAskSysProFlagAPI())                              //请求充放电保护
		{
			errLedState = sStateTwin;                                   //故障状态LED快闪
		}
		else if((GetGProtectStateAPI() >= ePRO_SYSLIM)                  //有充放电限制或保护类异常
			&& (0 == GetGChgLimCurrAPI())                               /*不能充电*/
			&& (0 == GetGDhgLimCurrAPI()))                              /*不能放电*/
		{
			errLedState = sStateTwin1;                                  //故障状态LED一闪一灭
		}
		else if(1 == TacGetGroupExtCommuErrStateHook())                 //外部通讯中断
		{
			errLedState = sStateTwin2;                                  //故障状态LED两闪一灭
		}
		else                                                            //无异常
		{
			errLedState = ePSTATE_OFF;                                  //故障状态LED熄灭
		}
		#endif
	}
	/*故障状态显示*/
	else if(eWORK_ERR == GetGWorkStateAPI())                            //系统故障
	{
		pcbLedState = sStateTwin0;                                      //板上LED慢闪

		//显示灯LED接触器(有LED时)
		#if(1 == BMU_LED_SWH_EN)
		runLedState = ePSTATE_OFF;      		                        //运行状态LED熄灭
		errLedState = ePSTATE_ON;                                       //故障状态LED常亮
		#endif
	}
	else
	{
		pcbLedState = sStateTwin;                                       //板上LED快闪

		//显示灯LED接触器(有LED时)
		#if(1 == BMU_LED_SWH_EN)
		runLedState = sStateTwin;                                       //运行状态LED快闪

	    if((eWORK_IDLE == GetGWorkStateAPI())                           //系统空闲
	        && (0 == TacGetGroupFitStartStateHook()))                   //不能启动
	    {
			errLedState = sStateTwin1;                                  //故障状态LED一闪一灭
	    }
	    else if((eWORK_IDLE == GetGWorkStateAPI())                      //系统空闲
	        && (1 == TacGetGroupExtCommuErrStateHook()))                //外部通讯中断
	    {
			errLedState = sStateTwin2;                                  //故障状态LED两闪一灭
	    }
	    else                                                            //系统正常
	    {
			errLedState = ePSTATE_OFF;                                  //故障状态LED熄灭
	    }
		#endif
	}

	//控制板上LED/运行LED/故障LED
    DEVGPIOSetPortState(ePOut_LED, pcbLedState);

	//显示灯LED接触器(有LED时)
	#if(1 == BMU_LED_SWH_EN)
    TacCtrlGroupSwhStateAPI(eGroupSwh_Run, runLedState);
    TacCtrlGroupSwhStateAPI(eGroupSwh_Alm, errLedState);
    #endif

	sStateTwin ^= ePSTATE_ON;                                           //快闪状态变化(亮0.5s灭0.5s)

	if(0 == (sTime % 4))
	{
		sStateTwin0 = ePSTATE_ON;                                       //慢闪状态变化亮(亮1s灭1s)
	}
	else if(2 == (sTime % 4))
	{
		sStateTwin0 = ePSTATE_OFF;                                      //慢闪状态变化灭(亮1s灭1s)
	}

	//显示灯LED接触器(有LED时)
	#if(1 == BMU_LED_SWH_EN)
	if(4 == (sTime % 5))
	{
		sStateTwin1 = ePSTATE_ON;                                       //一闪一灭状态变化亮(亮0.5s一次灭2s)
	}
	else if(0 == (sTime % 5))
	{
		sStateTwin1 = ePSTATE_OFF;                                      //一闪一灭状态变化灭(亮0.5s一次灭2s)
	}

	if(4 == (sTime % 8))
	{
		sStateTwin2 = ePSTATE_ON;                                       //两闪一灭状态变化亮1次(亮0.5s两次灭2s)
	}
	else if(5 == (sTime % 8))
	{
		sStateTwin2 = ePSTATE_OFF;                                      //两闪一灭状态变化灭j间隔(亮0.5s两次灭2s)
	}
	else if(6 == (sTime % 8))
	{
		sStateTwin2 = ePSTATE_ON;                                       //两闪一灭状态变化亮2次(亮0.5s两次灭2s)
	}
	else if(7 == (sTime % 8))
	{
		sStateTwin2 = ePSTATE_OFF;                                      //两闪一灭状态变化灭(亮0.5s两次灭2s)
	}
	#endif

	sTime++;                                                            //计时
	if(sTime >= 40)                                                     //2/4/5/8最小公倍数时间
	{
		sTime = 0;                                                      //计时复位
	}

	/*输出干接点控制策略(运行/禁充/禁放时闭合)*/
	if(eWORK_RUN == GetGWorkStateAPI())                                     //系统运行
	{
		//系统状态干接点
		TacCtrlGroupSwhStateAPI(eGroupSwh_Sta, ePSTATE_ON);                 //状态干接点1闭合

		//获取故障等级状态
		expSort = TacGetGNowErrLevelHook(1);

		//充电禁止干接点
		if((0 == GetGChgLimCurrAPI())                                       //充电禁止
			|| (eErr_ChgLim == expSort)                                     //充电限制
			|| (eErr_ChgPro == expSort)                                     //充电保护
			|| (eErr_SysLim == expSort)                                     //充放电限制
			|| (eErr_SysPro == expSort)                                     //充放电保护
			|| (DiagGetGroupSosLevel() > 0)                                 //SOS异常保护
			|| (TacGetGAskChgProFlagAPI() > 0))                             //请求充电保护
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Chg, ePSTATE_ON);             //充电禁止干接点2闭合
		}
		else                                                                //允许充电
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Chg, ePSTATE_OFF);            //充电禁止干接点2断开
		}

		//充放电禁止干接点均在DO3
		#if(1 == BMU_CDHG_DO_EN)
		//获取故障等级状态
		expSort = TacGetGNowErrLevelHook(0);

		//充放电禁止干接点
		if(((0 == GetGChgLimCurrAPI())                                      //充电禁止
			&& (0 == GetGDhgLimCurrAPI()))                                  //放电禁止
			|| ((TacGetGAskChgProFlagAPI() > 0)                             //请求充电保护
			&& (eCURR_CHG == GetGChgDhgStateAPI()))                         //仍在充电
			|| ((TacGetGAskDhgProFlagAPI() > 0)                             //请求放电保护
			&& (eCURR_DHG == GetGChgDhgStateAPI()))                         //仍在放电
			|| (eErr_SysLim == expSort)                                     //充放电限制
			|| (eErr_SysPro == expSort)                                     //充放电保护
			|| (DiagGetGroupSosLevel() > 0)                                 //SOS异常保护
			|| (TacGetGAskSysProFlagAPI() > 0))                             //请求充放电保护
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Dhg, ePSTATE_ON);             //充放电禁止干接点3闭合
		}
		else                                                                //允许充电或放电
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Dhg, ePSTATE_OFF);            //充放电禁止干接点3断开
		}
		//仅放电禁止干接点在DO3
		#else
		//获取故障等级状态
		expSort = TacGetGNowErrLevelHook(2);

		//放电禁止干接点
		if((0 == GetGDhgLimCurrAPI())                                       //放电禁止
			|| (eErr_DhgLim == expSort)                                     //放电限制
			|| (eErr_DhgPro == expSort)                                     //放电保护
			|| (eErr_SysLim == expSort)                                     //充放电限制
			|| (eErr_SysPro == expSort)                                     //充放电保护
			|| (DiagGetGroupSosLevel() > 0)                                 //SOS异常保护
			|| (TacGetGAskDhgProFlagAPI() > 0))                             //请求放电保护
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Dhg, ePSTATE_ON);             //放电禁止干接点3闭合
		}
		else                                                                //允许放电
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Dhg, ePSTATE_OFF);            //放电禁止干接点3断开
		}
		#endif
	}
	else                                                                    //系统未运行
	{
		TacCtrlGroupSwhStateAPI(eGroupSwh_Sta, ePSTATE_OFF);                //系统状态干接点1断开
		TacCtrlGroupSwhStateAPI(eGroupSwh_Chg, ePSTATE_ON);                 //充电禁止干接点2闭合
		TacCtrlGroupSwhStateAPI(eGroupSwh_Dhg, ePSTATE_ON);                 //放电禁止干接点3闭合
	}

	//烟感电源(有烟感时)
	#if(1 == BMU_SMG_SWH_EN)
	{
		//常态闭合烟感电源判断是否有告警
		if(0 == sSmgOffStep)
		{
			//发生烟感告警
			if(1 == TacGetUserSmogSignStateHook())
			{
				//未复位过电源
				if(0 == sSmgOffFlag)
				{
					//记录1次事件
					if(1 != sSmgAlarmState)
					{
						DiagUserSelfErrRecord(eUDef_ErrRcd_SmgWarn, eERR_RCDSORT_WARSET, 0,  0);
					}
					sSmgAlarmState = 1;										//正在核实烟感告警
					TacCtrlGroupSwhStateAPI(eGroupSwh_Smg, ePSTATE_OFF);    //断开烟感电源
					sSmgOffStep = 1;										//进行等待2s使烟感完全下电
					sSmgOffFlag = 1;
					sSmgOffTime = 0;
				}
				//已复位过电源
				else
				{
					//记录1次事件
					if(2 != sSmgAlarmState)
					{
						DiagUserSelfErrRecord(eUDef_ErrRcd_SmgWarn, eERR_RCDSORT_WARSET, 0,  0);
					}
					sSmgOffStep = 0;
					sSmgAlarmState = 2;										//确认有烟感告警
					TacCtrlGroupSwhStateAPI(eGroupSwh_Smg, ePSTATE_ON);     //保持闭合烟感电源
				}
			}
			//未发生烟感告警
			else
			{
				sSmgOffStep = 0;
				sSmgOffTime = 0;
				sSmgOffFlag = 0;
				sSmgAlarmState = 0;											//确认无烟感告警
				TacCtrlGroupSwhStateAPI(eGroupSwh_Smg, ePSTATE_ON);         //闭合烟感电源
			}
		}
		//断开电源后等待烟感完全下电
		else if(1 == sSmgOffStep)
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Smg, ePSTATE_OFF);    		//断开烟感电源
			sSmgOffStep = 2;
		}
		//到达烟感完全下电时间
		else if(2 == sSmgOffStep)
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Smg, ePSTATE_OFF);    		//断开烟感电源
			sSmgOffStep = 3;
			sSmgOffFlag = 1;
			sSmgOffTime = 0;
		}
		//断开电源后再次闭合
		else
		{
			TacCtrlGroupSwhStateAPI(eGroupSwh_Smg, ePSTATE_ON);             //重新闭合烟感电源
			if(0 == sSmgOffFlag)                                            //未记录断开电源(错误)
			{
				sSmgOffStep = 0;											//直接返回
				sSmgOffTime = 0;
				sSmgAlarmState = 0;											//无烟感告警
			}
			else															//已记录断开电源(正常)
			{
				if((sSmgOffTime >= TAC_SMG_ONCON_T)							//等待10s仍无告警(电源重新上电后等待实际状态)
					|| ((sSmgOffTime >= 5) && (1 == TacGetUserSmogSignStateHook()))) //等待5s已发生烟雾告警
				{
					if(1 == TacGetUserSmogSignStateHook())					//仍是发生告警状态
					{
						//记录1次事件
						if(2 != sSmgAlarmState)
						{
							DiagUserSelfErrRecord(eUDef_ErrRcd_SmgWarn, eERR_RCDSORT_WARSET, 0,  0);
						}
						sSmgAlarmState = 2;									//确认告警发生
					}
					else
					{
						sSmgAlarmState = 0;									//确认告警消除
						sSmgOffFlag = 0;									//恢复下一周期
					}

					sSmgOffStep = 0;
					sSmgOffTime = 0;
				}
				else
				{
					sSmgOffTime++;
				}
			}
		}
	}
	#endif
}

/*=================================================================================================
** @Name      : void TacGroupStartCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组启动控制策略
** @The notes : 高压上电策略
**===============================================================================================*/
void TacGroupStartCtrlTactic(void)
{
    u8 result = 0;
    static u8 sTime = 0;
    static u8 sWaitT = 0;

    /*电池充放电保护启动受限*/
    if(0 == TacGetGroupFitStartStateHook())                         /*不适合启动*/
    {
        /*等待5s*/
    	if(sWaitT >= 50)
    	{
			sTime = 0;
	    	sWaitT = 0;
			sGroupStartStep = 0xff;

			/*记录启动失败*/
			TacSetGStartFailReason(eStartFailReason_StartLim);
			/*TacSetGroupWorkStateAPI(eWORK_STOP);*/           		/*切换到停机状态(由启动失败故障导致切换为故障状态)*/
    	}
    	else
    	{
    		sWaitT++;
    	}
    }
    else                                                            /*系统正常无故障*/
    {
    	sWaitT = 0;
        switch(sGroupStartStep)
        {
            /*变量复位*/
            case 0:
                sGroupStartStep = 100;                              /*检测外部电路设备*/
                sTime = 0;
                break;

            /*等待外部检测完成(30s)*/
            case 100:
                if(TRUE == TacGetGAllPeriphReadyHook())             /*外部检测完成(电流零点校准/粘连检测/绝缘检测)*/
                {
                    sTime = 0;
                    sGroupStartStep = 1;                            /*检测负极辅助接触器*/
                }
                else if(sTime >= 300)                               /*等待超时*/
                {
                    sTime = 0;
                    sGroupStartStep = 0xff;

                    /*记录启动失败*/
                    TacSetGStartFailReason(eStartFailReason_PeripExp);
                    /*TacSetGroupWorkStateAPI(eWORK_STOP);*/        /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                }
                else
                {
                    sTime++;
                }
                break;

            /*检测负极线路开路状态*/
            case 1:
                if(TRUE == TacGetGNegCirReadyHook())                /*负极线路就绪(GND接外侧采预充电压正常)*/
                {
                    sTime = 0;

                    if(eCTRL_CHGSTART == sGroupWorkCtrlCmd)         /*充电启动*/
                    {
                        sGroupStartStep = 7;                        /*先闭合正极主接触器*/
                    }
                    else                                            /*正常预充或启动*/
                    {
                        sGroupStartStep = 2;                        /*先闭合负极辅助接触器*/
                    }
                }
                else
                {
                    sTime++;
                    /*长时间等待直至产生回检故障*/
                    if(sTime >= 100)
                    {
                        /*记录启动失败*/
                        TacSetGStartFailReason(eStartFailReason_SwtchExp);

                        /*记录负极接触器断开失败*/
                        TacSetGSwhOffCtrlFailFlag(eGroupSwh_Neg);

                        /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                        /*TacSetGroupWorkStateAPI(eWORK_STOP);*/
                    }
                }
                break;

            /*闭合负极接触器*/
            case 2:
                TacCtrlGroupSwhStateAPI(eGroupSwh_Neg, ePSTATE_ON);

                /*负极接触器闭合*/
                if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))
                {
                    sTime = 0;

                    /*进入负极接触器闭合线路检测*/
                    sGroupStartStep = 3;
                }
                /*未闭合*/
                else
                {
                    sTime++;
                    /*长时间等待直至产生回检故障*/
                    if(sTime >= 100)
                    {
                        /*记录启动失败*/
                        TacSetGStartFailReason(eStartFailReason_SwtchExp);

                        /*记录负极接触器闭合失败*/
                        TacSetGSwhOnCtrlFailFlag(eGroupSwh_Neg);

                        /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                        /*TacSetGroupWorkStateAPI(eWORK_STOP);*/
                    }
                }
                break;

            /*检测负极线路闭合状态*/
            case 3:
            	/*负极线路就绪(总压/预充采样正常/正极未粘连)*/
                if(TRUE == TacGetGNegCirReadyHook())
                {
                    sTime = 0;

                    /*充电启动(主->辅助)*/
                    if(eCTRL_CHGSTART == sGroupWorkCtrlCmd)
                    {
                        /*进入启动结果判断*/
                        sGroupStartStep = 9;
                    }
                    /*放电启动(辅助->主)*/
                    else if(eCTRL_DHGSTART == sGroupWorkCtrlCmd)
                    {
                        /*进入主接触器闭合控制*/
                        sGroupStartStep = 7;
                    }
                    /*正常启动(辅助->预充->主)*/
                    else
                    {
                        /*进入预充接触器闭合控制*/
                        sGroupStartStep = 4;
                    }
                }
                /*闭合负极接触器负载电压异常可能主正接触器粘连*/
                else
                {
                    sTime++;
                    /*长时间等待直至产生回检故障*/
                    if(sTime >= 100)
                    {
                        /*记录启动失败*/
                        TacSetGStartFailReason(eStartFailReason_SwtchExp);

                        /*记录负极接触器闭合失败*/
                        //TacSetGSwhOnCtrlFailFlag(eGroupSwh_Neg);
                        /*记录主正接触器异常(未断开)*/
                        TacSetGSwhOffCtrlFailFlag(eGroupSwh_Mai);

                        /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                        /*TacSetGroupWorkStateAPI(eWORK_STOP);*/
                    }
                }
                break;

            /*闭合预充*/
            case 4:
            	TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_ON);

                /*等待必须预充时间*/
                sTime++;
                if(sTime >= gGBmuHigLevPara_103[eBmuHigLevPara103_PChgWaitT])
                {
                    sTime = 0;
                    sGroupStartStep = 5;
                }
                break;

            /*判断是否继续启动完成预充上高压*/
            case 5:
                /*预充接触器断开(未出现异常)*/
                if((ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre))
                    && (0 == TacGetGStartFailFlagAPI()))
                {
                    /*返回上一步闭合预充接触器*/
                    sGroupStartStep = 4;
                }
                /*需要正常启动完成高压上电*/
                else if(eCTRL_NORSTART == sGroupWorkCtrlCmd)
                {
                    sTime = 0;
                    /*进入预充结果判断*/
                    sGroupStartStep = 6;
                }
                /*仅保持预充状态而未出现预充异常*/
                else if(0 == TacGetGStartFailFlagAPI())
                {
                    /*判断预充结果*/
                    result = TacGetGPreChgFinStateHook();

                    /*预充失败[预充电流异常](立刻断开预充接触器)*/
                    if(3 == result)
                    {
                        /*记录启动失败原因*/
                        TacSetGStartFailReason(eStartFailReason_PChgCErr);

                        /*启动失败后处理(事件记录)*/
                        TacGroupPreChgFinDealHook();

                        /*断开预充接触器*/
                        TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_OFF);

                        /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                        /*TacSetGroupWorkStateAPI(eWORK_STOP);*/
                    }
                    /*预充失败[预充温度异常](立刻断开预充接触器)*/
                    else if(2 == result)
                    {
                        /*记录启动失败*/
                        TacSetGStartFailReason(eStartFailReason_PChgTOvr);

                        /*启动失败后处理(事件记录)*/
                        TacGroupPreChgFinDealHook();

                        /*断开预充接触器*/
                        TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_OFF);

                        /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                        /*TacSetGroupWorkStateAPI(eWORK_STOP);*/
                    }
                }
                /*已记录预充异常*/
                else
                {
                    /*断开预充接触器*/
                    TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_OFF);

                    sTime++;
                    /*长时间等待直至产生系统故障*/
                    if(sTime >= 100)
                    {
                        /*超时则切换到停机状态(系统不把启动异常当作故障保护)*/
                        TacSetGroupWorkStateAPI(eWORK_STOP);
                    }
                }
                break;

            /*检测预充完成*/
            case 6:
                /*未出现启动异常*/
                if(0 == TacGetGStartFailFlagAPI())
                {
                    /*判断预充结果*/
                    result = TacGetGPreChgFinStateHook();

                    /*预充成功*/
                    if(1 == result)
                    {
                        sTime = 0;

                        /*启动成功后处理(事件记录)*/
                        TacGroupPreChgFinDealHook();

                        /*启动主接触器*/
                        sGroupStartStep = 7;
                    }
                    /*预充失败[预充电流异常](立刻断开预充接触器)*/
                    else if(3 == result)
                    {
                        /*记录启动失败*/
                        TacSetGStartFailReason(eStartFailReason_PChgCErr);

                        /*启动失败后处理(事件记录)*/
                        TacGroupPreChgFinDealHook();

                        /*断开预充接触器*/
                        TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_OFF);
                        sTime = 0;

                        /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                        /*TacSetGroupWorkStateAPI(eWORK_STOP);*/
                    }
                    /*预充失败[预充温度异常](立刻断开预充接触器)*/
                    else if(2 == result)
                    {
                        /*记录启动失败*/
                        TacSetGStartFailReason(eStartFailReason_PChgTOvr);

                        /*启动失败后处理(事件记录)*/
                        TacGroupPreChgFinDealHook();

                        /*断开预充接触器*/
                        TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_OFF);
                        sTime = 0;

                        /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                        /*TacSetGroupWorkStateAPI(eWORK_STOP);*/
                    }
                    /*预充未完成*/
                    else
                    {
                        sTime++;
                        /*长时间等待预充失败超时(立刻断开预充接触器)/无电流等待多一会等预充电压更新*/
                        if((sTime >= (gGBmuHigLevPara_103[eBmuHigLevPara103_PChgOffT] * 2))
                            || ((sTime >= gGBmuHigLevPara_103[eBmuHigLevPara103_PChgOffT])
                            && ((GetGSampOutCurrAPI() >= 10) || (GetGSampOutCurrAPI() <= -10))))
                        {
                            /*记录启动失败*/
                            TacSetGStartFailReason(eStartFailReason_PChgFail);

                            /*启动失败后处理(事件记录)*/
                            TacGroupPreChgFinDealHook();

                            /*断开预充接触器*/
                            TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_OFF);
                            sTime = 0;

                            /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                            /*TacSetGroupWorkStateAPI(eWORK_STOP);*/
                        }
                    }
                }
                /*出现启动异常*/
                else
                {
                    /*断开预充接触器*/
                    TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_OFF);

                    sTime++;
                    /*长时间等待直至产生系统故障*/
                    if(sTime >= 150)
                    {
                        /*超时则切换到停机状态(系统不把启动异常当作故障保护)*/
                        TacSetGroupWorkStateAPI(eWORK_STOP);
                    }
                }
                break;

            /*闭合主接触器*/
            case 7:
                TacCtrlGroupSwhStateAPI(eGroupSwh_Mai, ePSTATE_ON);

                /*主接触器闭合*/
                if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))
                {
                    sTime = 0;

                    /*充电启动(主->辅助)*/
                    if(eCTRL_CHGSTART == sGroupWorkCtrlCmd)
                    {
                        /*返回控制辅助接触器*/
                        sGroupStartStep = 2;
                    }
                    /*放电启动(辅助->主)*/
                    else if(eCTRL_DHGSTART == sGroupWorkCtrlCmd)
                    {
                        /*进入启动结果判断*/
                        sGroupStartStep = 9;
                    }
                    /*正常启动(辅助->预充->主)*/
                    else
                    {
                        /*进入预充断开控制*/
                        sGroupStartStep = 8;
                    }
                }
                /*未闭合*/
                else
                {
                    sTime++;
                    /*长时间等待直至产生回检故障*/
                    if(sTime >= 100)
                    {
                        /*记录启动失败*/
                        TacSetGStartFailReason(eStartFailReason_SwtchExp);

                        /*记录正极接触器闭合失败*/
                        TacSetGSwhOnCtrlFailFlag(eGroupSwh_Mai);

                        /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                        /*TacSetGroupWorkStateAPI(eWORK_STOP);*/
                    }
                }
                break;

            /*断开预充(延迟1s)*/
            case 8:
                sTime++;
                if(sTime > 10)
                {
                    TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_OFF);
                    sTime = 0;
                    sGroupStartStep = 9;
                }
                break;

            /*启动完成*/
            case 9:
                sGroupWorkCtrlCmd = 0xff;

                /*负极/主接触器闭合、预充接触器断开*/
                if((ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))
                    && (ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))
                    && (ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre)))
                {
                    /*进入运行状态*/
                    sTime = 0;
                    sGroupStartStep = 0xff;
                    TacSetGroupWorkStateAPI(eWORK_RUN);
                }
                /*未闭合*/
                else
                {
                    sTime++;
                    /*长时间等待直至产生回检故障*/
                    if(sTime >= 100)
                    {
                        /*记录启动失败*/
                        TacSetGStartFailReason(eStartFailReason_SwtchExp);

                        if(ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))
                        {
                            /*记录负极接触器闭合失败*/
                            TacSetGSwhOnCtrlFailFlag(eGroupSwh_Neg);
                        }
                        else if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre))
                        {
                            /*记录预充接触器断开失败*/
                            TacSetGSwhOffCtrlFailFlag(eGroupSwh_Pre);
                        }
                        else
                        {
                            /*记录正极接触器闭合失败*/
                            TacSetGSwhOnCtrlFailFlag(eGroupSwh_Mai);
                        }

                        /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                        /*TacSetGroupWorkStateAPI(eWORK_STOP);*/
                    }
                }
                break;

            default:
                break;
        }
    }
}

/*=================================================================================================
** @Name      : void TacGroupNorStopCtrlTactic(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组正常停机控制策略
** @The notes : 高压下电策略
**===============================================================================================*/
void TacGroupNorStopCtrlTactic(void)
{
    static u8 sTime = 0;

    switch(sGroupStopStep)
    {
        /*数据复位*/
        case 0:
            sTime = 0;
            sGroupStopStep = 1;
            TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_OFF);            /*断开预充*/
            break;

        /*检测安全电流*/
        case 1:
            if((1 == TacCheckCurrSafeStateHook())                           /*主接触器电流安全*/
                || (eCTRL_FORCSTOP == sGroupWorkCtrlCmd))                   /*强制停机*/
            {
                /*根据电流方向判定先断哪个接触器*/
                if(eCURR_CHG == GetGChgDhgStateAPI())                       /*正在充电*/
                {
                    sGroupStopStep = 4;                                     /*先断开负极辅助接触器*/
                }
                else                                                        /*正常停机或放电停机*/
                {
                    sGroupStopStep = 2;                                     /*先正极主接触器*/
                }

                sTime = 0;
            }
            else                                                            /*接触器电流过大不能直接切断*/
            {
                sTime++;

                if(sTime >= 100)                                            /*等待PCS停止充放电超时*/
                {
                    sTime = 0;
                    sGroupStopStep = 0xff;
                    TacSetGStopFailReason(eStopFailReason_CurrFail);
                    TacSetGroupWorkStateAPI(eWORK_ERR);                     /*切换到故障状态*/
                }
            }
            break;

        /*断开主接触器*/
        case 2:
            TacCtrlGroupSwhStateAPI(eGroupSwh_Mai, ePSTATE_OFF);

            /*主接触器断开*/
            if(ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))
            {
                sTime = 0;

                /*进入接触器断开状态检测*/
                sGroupStopStep = 3;
            }
            /*未断开*/
            else
            {
                sTime++;

                if(sTime >= 20)
                {
                	TacSetGSwhOffCtrlFailFlag(eGroupSwh_Mai);
                    TacSetGStopFailReason(eStopFailReason_SwtchExp);

                    /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                    /*TacSetGroupWorkStateAPI(eWORK_ERR);*/
                }
            }
            break;

        /*检测主接触器是否断开*/
        case 3:
            sTime++;
            if(((1 == TacCheckCurrCutStateHook())                           /*主接触器断开*/
                && (1 == TacGetGNegCirReadyHook()))                         /*负极线路就绪(GND接外侧采预充电压,负极接触器闭合正常)*/
                || (sTime >= 50))                                           /*控制断开超时*/
            {
                if(sTime >= 50)
                {
                    if(0 == TacCheckCurrCutStateHook())                     /*主接触器未断开*/
                    {
                    	TacSetGSwhOffCtrlFailFlag(eGroupSwh_Mai);           /*记录主接触器控制断开失败*/
                    }

                    if(0 == TacGetGNegCirReadyHook())                       /*负极线路异常(采集总压失效)*/
                    {
                        ;                                                   /*由接触器检测任务产生负极接触器控制闭合失败故障*/
                    }
                }

                if(ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))/*负极接触器已断开(负极->主)*/
                {
                    sGroupStopStep = 6;                                     /*进入停机结果检测*/
                }
                else                                                        /*负极接触器未断开(主->负极)*/
                {
                    sGroupStopStep = 4;                                     /*进入负极接触器控制*/
                }
                sTime = 0;
            }
            break;

        /*断开负极接触器*/
        case 4:
            TacCtrlGroupSwhStateAPI(eGroupSwh_Neg, ePSTATE_OFF);

            /*负极接触器断开*/
            if(ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))
            {
                sTime = 0;

                /*进入接触器断开状态检测*/
                sGroupStopStep = 5;
            }
            /*未闭合*/
            else
            {
                sTime++;
                /*长时间等待直至产生回检故障*/
                if(sTime >= 20)
                {
                	TacSetGSwhOffCtrlFailFlag(eGroupSwh_Neg);
                    TacSetGStopFailReason(eStopFailReason_SwtchExp);

                    /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                    /*TacSetGroupWorkStateAPI(eWORK_ERR);*/
                }
            }
            break;

        /*检测负极接触器是否断开*/
        case 5:
            sTime++;
            if(((1 == TacCheckCurrCutStateHook())                           /*负极接触器断开*/
                && (1 == TacGetGNegCirReadyHook()))                         /*负极线路就绪(GND接外侧采预充电压,接触器未粘连)*/
                || (sTime >= 50))                                           /*等待断开超时*/
            {
                if(sTime >= 50)
                {
                	TacSetGSwhOffCtrlFailFlag(eGroupSwh_Neg);               /*记录负极接触器控制断开失败*/
                }

                if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai)) /*主接触器未断开(辅助->主)*/
                {
                    sGroupStopStep = 2;                                     /*进入主接触器控制*/
                }
                else                                                        /*主接触器已断开(主->辅助)*/
                {
                    sGroupStopStep = 6;                                     /*进入停机结果检测*/
                }
                sTime = 0;
            }
            break;

        /*停机完成检测*/
        case 6:
            /*辅助/主接触器/预充接触器断开*/
            if((ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))
                && (ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))
                && (ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre)))
            {
                /*清除参数*/
                sTime = 0;
                sGroupStopStep = 0xff;

                /*关机命令*/
                if(eCTRL_POWEROFF == sGroupWorkCtrlCmd)
                {
                    /*进入关机状态*/
                    TacSetGroupWorkStateAPI(eWORK_OFF);
                }
                else
                {
                    /*进入空闲状态*/
                    sGroupWorkCtrlCmd = 0xff;
                    TacSetGroupWorkStateAPI(eWORK_IDLE);
                }
            }
            /*有闭合*/
            else
            {
                sTime++;
                /*长时间等待直至产生回检故障*/
                if(sTime >= 100)
                {
                    /*记录停机失败*/
                    TacSetGStartFailReason(eStartFailReason_SwtchExp);

                    if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))
                    {
                        /*记录负极接触器断开失败*/
                        TacSetGSwhOffCtrlFailFlag(eGroupSwh_Neg);
                    }
                    else if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre))
                    {
                        /*记录预充接触器断开失败*/
                        TacSetGSwhOffCtrlFailFlag(eGroupSwh_Pre);
                    }
                    else
                    {
                        /*记录正极接触器断开失败*/
                        TacSetGSwhOffCtrlFailFlag(eGroupSwh_Mai);
                    }

                    /*切换到停机状态(由启动失败故障导致切换为故障状态)*/
                    /*TacSetGroupWorkStateAPI(eWORK_ERR);*/
                }
            }
            break;

        default:
            break;
    }
}

/*=================================================================================================
** @Name      : void TacGroupErrStopCtrlTactic(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组故障停机控制策略
** @The notes : 高压下电策略
**===============================================================================================*/
void TacGroupErrStopCtrlTactic(void)
{
    static u8 sTime = 0;
    static u8 sFlag = 0;

    /*无停机命令*/
    if((eCTRL_NORSTOP != sGroupWorkCtrlCmd)
        && (eCTRL_FORCSTOP != sGroupWorkCtrlCmd)
        && (eCTRL_EMERSTOP != sGroupWorkCtrlCmd)
        && (eCTRL_POWEROFF != sGroupWorkCtrlCmd))
    {
        /*不控制(由故障保护执行)*/
    	sTime = 0;
        sFlag = 0;
        sGroupWorkCtrlCmd = 0xff;
        return;
    }

    /*控制预充接触器断开*/
    TacCtrlGroupSwhStateAPI(eGroupSwh_Pre, ePSTATE_OFF);

    if((1 == sFlag) || (eCTRL_EMERSTOP == sGroupWorkCtrlCmd))               /*已控制有接触器先断或需要急停*/
    {
    	TacCtrlGroupRunSwhAllOffAPI();                                      /*断开所有运行接触器*/
    }

    if((eCTRL_FORCSTOP == sGroupWorkCtrlCmd)                                /*强制停机*/
    	|| (1 == TacCheckCurrSafeStateHook())                               /*主接触器电流安全*/
		|| (sGroupStopFailFlag > 0))                                        /*停机失败*/
	{
    	sTime = 0;

        /*检测充放电状态确定断哪个接触器*/
        if(eCURR_CHG == GetGChgDhgStateAPI())                               /*正在充电*/
        {
            if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))     /*负极接触器未断开*/
            {
                sFlag = 1;                                                  /*记录已控制*/
                TacCtrlGroupSwhStateAPI(eGroupSwh_Neg, ePSTATE_OFF);        /*先断开负极接触器*/
            }
            else                                                            /*负极接触器已断开*/
            {
                TacCtrlGroupSwhStateAPI(eGroupSwh_Neg, ePSTATE_OFF);        /*确保断开负极接触器*/
                TacCtrlGroupSwhStateAPI(eGroupSwh_Mai, ePSTATE_OFF);        /*再断开正极主接触器*/
            }
        }
        else                                                                /*正常停机或放电停机*/
        {
            if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))     /*正极主接触器未断开*/
            {
                sFlag = 1;                                                  /*记录已控制*/
                TacCtrlGroupSwhStateAPI(eGroupSwh_Mai, ePSTATE_OFF);        /*先断开正极主接触器*/
            }
            else                                                            /*正极主接触器已断开*/
            {
                TacCtrlGroupSwhStateAPI(eGroupSwh_Mai, ePSTATE_OFF);        /*确保断开正极主接触器*/
                TacCtrlGroupSwhStateAPI(eGroupSwh_Neg, ePSTATE_OFF);        /*再断开负极接触器*/
            }
        }
	}
	else                                                                    /*接触器电流过大不能直接切断*/
	{
		sTime++;

		if(sTime >= 30)                                                     /*等待PCS停止充放电超时*/
		{
			sGroupStopStep = 0xff;
			TacSetGStopFailReason(eStopFailReason_CurrFail);                /*带载超时停机失败*/
		}
	}

    if(TRUE == TacGetGroupRunSwhAllOffStateAPI())                           /*运行接触器全断开*/
    {
    	sTime = 0;
        sFlag = 0;                                                          /*清除已控制*/

        if(eCTRL_POWEROFF == sGroupWorkCtrlCmd)                             /*关机命令*/
        {
            TacSetGroupWorkStateAPI(eWORK_OFF);                             /*进入关机状态*/
        }
        else
        {
            sGroupWorkCtrlCmd = 0xff;
        }
    }
}

/*=================================================================================================
** @Name      : void TacGroupPwrOffCtrlTactic(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组故障停机控制策略
** @The notes : 低压下电策略
**===============================================================================================*/
void TacGroupPwrOffCtrlTactic(void)
{
	TacCtrlGroupRunSwhAllOffAPI();                                          /*断开所有运行接触器*/

    if(TRUE == TacGetGroupRunSwhAllOffStateAPI())                           /*运行接触器全断开*/
    {
        if((eCTRL_POWEROFF == sGroupWorkCtrlCmd)                            /*关机命令*/
            && (ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Fan))   /*风扇已关*/
            && (ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Hot)))  /*加热已关*/
        {
			#if(0 == BMU_DC_PWR_TYPE)                                       /*自供电按钮上下电*/
        	TacCtrlGroupSwhStateAPI(eGroupSwh_Pwr, ePSTATE_OFF);            /*断开电源接触器*/
			#else                                                           /*自供电手动上下电*/
			TacCtrlGroupSwhStateAPI(eGroupSwh_Pwr, ePSTATE_ON);             /*闭合主控电源接触器电源(高电平弹开使断电)*/
			#endif
        }
    }

    if(eCTRL_POWERON == sGroupWorkCtrlCmd)                                  /*开机命令*/
    {
        TacSetGroupWorkStateAPI(eWORK_INIT);                                /*进入初始化状态*/
    }
}

/*=================================================================================================
** @Name      : void TacSetGStartFailReason(e_StartFailReason reason)
** @Input     : reason:失败原因[0,3]
** @Output    : void
** @Function  : 设置电池组启动失败原因
** @The notes : 设置启动失败标志
**===============================================================================================*/
void TacSetGStartFailReason(e_StartFailReason reason)
{
	/*原因有效*/
	if(reason < eStartFailReason_Fin)
	{
		BitSet(sGroupStartFailFlag, reason);
	}
}

/*=================================================================================================
** @Name      : void TacSetGStopFailReason(e_StopFailReason reason)
** @Input     : reason:失败原因[0,3]
** @Output    : void
** @Function  : 设置电池组停机失败原因
** @The notes : 设置停机失败标志
**===============================================================================================*/
void TacSetGStopFailReason(e_StopFailReason reason)
{
	/*原因有效*/
	if(reason < eStopFailReason_Fin)
	{
		BitSet(sGroupStopFailFlag, reason);
	}
}

/*=================================================================================================
** @Name      : void TacSetGEmergStopReason(e_EmergStopReason reason)
** @Input     : reason:失败原因[0,1]
** @Output    : void
** @Function  : 设置电池组紧急停机原因
** @The notes : 设置紧急停机标志
**===============================================================================================*/
void TacSetGEmergStopReason(e_EmergStopReason reason)
{
	/*原因有效*/
	if(reason < eEmergStopReason_Fin)
	{
		BitSet(sEmergStopFlag, reason);
	}
}

/*=================================================================================================
** @Name      : void TacSetGSwhOnCtrlFailFlag(e_GroupSwhSort swhSort)
** @Input     : swhSort:接触器名称[0,11]
** @Output    : void
** @Function  : 电池组接触器控制闭合失败标志
** @The notes : 控制后回检电压电流
**===============================================================================================*/
void TacSetGSwhOnCtrlFailFlag(e_GroupSwhSort swhSort)
{
	/*接触器号有效*/
	if(swhSort < eGroupSwh_Num)
	{
		BitSet(sGSwhOnCtrlFailFlag, swhSort);
	}
}

/*=================================================================================================
** @Name      : void TacSetGSwhOffCtrlFailFlag(e_GroupSwhSort swhSort)
** @Input     : swhSort:接触器名称[0,11]
** @Output    : void
** @Function  : 电池组接触器控制断开失败标志
** @The notes : 控制后回检电压电流
**===============================================================================================*/
void TacSetGSwhOffCtrlFailFlag(e_GroupSwhSort swhSort)
{
	/*接触器号有效*/
	if(swhSort < eGroupSwh_Num)
	{
		BitSet(sGSwhOffCtrlFailFlag, swhSort);
	}
}

/*=================================================================================================
** @Name      : void TacClrGCtrlErrFlagAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除电池组控制失败标志
** @The notes :
**===============================================================================================*/
void TacClrGCtrlErrFlagAPI(void)
{
	/*接触器控制失败*/
	sGSwhOnCtrlFailFlag = 0;
	sGSwhOffCtrlFailFlag = 0;

	/*启停控制失败*/
	sGroupStartFailFlag = 0;
	sGroupStopFailFlag = 0;

	/*取消急停按钮才清除急停故障*/
	if(0 == TacGetUserButtonEmergStateHook())
	{
		sEmergStopFlag = 0;
	}
}

/*=================================================================================================
** @Name      : u8 TacGetGStartFailFlagAPI(void)
** @Input     : void
** @Output    : 启动失败标志(按bit位表示 bit0:系统故障 bit1:启动受限 bit2:外设异常 bit3:接触器异常 bit4:预充过温 bit5:预充未满失败 bit6:预充电流异常)
** @Function  : 获取电池组启动失败标志
** @The notes : 启动失败原因
**===============================================================================================*/
u8 TacGetGStartFailFlagAPI(void)
{
	return(sGroupStartFailFlag);
}

/*=================================================================================================
** @Name      : u8 TacGetGStopFailFlagAPI(void)
** @Input     : void
** @Output    : 停机失败标志(按bit位表示 bit0:系统故障 bit1:等待安全电流失败 bit2:接触器异常 bit3:接触器断开失败)
** @Function  : 获取电池组停机失败标志
** @The notes : 停机失败原因
**===============================================================================================*/
u8 TacGetGStopFailFlagAPI(void)
{
	return(sGroupStopFailFlag);
}

/*=================================================================================================
** @Name      : u8 TacGetGEmergStopFlagAPI(void)
** @Input     : void
** @Output    : 紧急停机标志(按bit位表示 bit0:内部软件急停 bit1:外部命令急停 bit2:按钮急停)
** @Function  : 获取电池组紧急停机标志
** @The notes : 紧急停机原因
**===============================================================================================*/
u8 TacGetGEmergStopFlagAPI(void)
{
	return(sEmergStopFlag);
}

/*=================================================================================================
** @Name      : u8 TacGetGSwhCtrlFailFlagAPI(void)
** @Input     : void
** @Output    : 控制接触器闭合失败标志(按bit位表示接触器 bit0:主正断开控制 bit1:预充断开控制 bit2:主负断开控制;bit4:主正闭合控制 bit5:预充闭合控制 bit6:主负闭合控制)
** @Function  : 获取电池组控制接触器闭合失败标志
** @The notes : 高4位控制闭合失败的接触器,低4位控制断开失败的接触器
**===============================================================================================*/
u8 TacGetGSwhCtrlFailFlagAPI(void)
{
	return((sGSwhOnCtrlFailFlag << 4) | (sGSwhOffCtrlFailFlag & 0x0f));
}

/*=================================================================================================
** @Name      : u8 TacGetGSmogAlarmFlagAPI(void)
** @Input     : void
** @Output    : 烟感告警状态标志(0:正常 1:正在核实告警 2:已确认告警)
** @Function  : 获取烟感告警状态标志
** @The notes : 包括电源重启确认真实信号
**===============================================================================================*/
u8 TacGetGSmogAlarmFlagAPI(void)
{
	/*烟感电源控制使能(有烟感检测时)*/
	#if(1 == BMU_SMG_SWH_EN)
	return(sSmgAlarmState);
	#else
	return(0);
	#endif
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
