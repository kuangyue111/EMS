/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : UserTactic.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 用户自定义策略模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "PortCheck.h"
#include "DiagInterface.h"
#include "TacUser.h"
#include "UserTactic.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sButtonStopReq = 0;                               /*用户按钮请求停机*/
static u8 sSlavMcuErrFlag = 0;                              /*KEA128从MUC异常标志*/

extern u16 gGSysCmdCtrl_86[];                               /*系统状态信息数组*/

extern u16 gGBmuGenPara_102[];		                        /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];                           /*主控高级参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void GroupSysIoUpDnCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 主控系统按钮启停控制任务
** @The notes : 1s周期
**===============================================================================================*/
void GroupSysIoUpDnCtrlTask(void)
{
    static u32 sHisTime = 0;
    /*电池堆未使能时有效*/
    #if(1 != PILE_MANAG_EN)
    //自供电按钮开关机控制
    #if(0 == BMU_DC_PWR_TYPE)
    u8 state = 0;
    static u8 sTimer = 0;
    static u8 sOffFlag = 0;
    static u8 sHisState = 0;
    #endif

    //判断急停干接点回检状态
    if(1 == GetGEmergIoButtonStateAPI())
    {
        //自供电按钮开关机控制
        #if(0 == BMU_DC_PWR_TYPE)
        sTimer = 0;
        sOffFlag = 0;
        sHisState = 0;
        #endif

        //未请求或接触器未断开
        if((0 == sButtonStopReq) || (FALSE == TacGetGroupRunSwhAllOffStateAPI()))
        {
            //设置紧急停机命令
            sButtonStopReq = 1;
            gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_EMERSTOP;
            RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
        }
    }
    //无急停信号
    else
    {
        //清除紧急停机请求
        sButtonStopReq = 0;
    }

    //自供电按钮开关机控制(软件自锁电源接触器)
    #if(0 == BMU_DC_PWR_TYPE)
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
        if(eWORK_OFF == GetGWorkStateAPI())
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
                sButtonStopReq = 1;

                //非充放电
                //if((eCURR_IDLE == GetGChgDhgStateAPI()))
                if((GetGSampOutCurrAPI() >= -20) && (GetGSampOutCurrAPI() <= 30))
                {
                    //设置关机命令
                    gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWEROFF;
                    RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
                }
            }
            //连续按下2s,切换系统为停机
            else if(sTimer >= 2)
            {
                //设置停机请求
                sButtonStopReq = 1;

                //非充放电
                //if((eCURR_IDLE == GetGChgDhgStateAPI()))
                if((GetGSampOutCurrAPI() >= -20) && (GetGSampOutCurrAPI() <= 30))
                {
                    //设置正常停机命令
                    gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTOP;
                    RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
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
            sButtonStopReq = 0;
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
                if((ePRO_SYSPRO == GetGProtectStateAPI())
                    || (eWORK_ERR == GetGWorkStateAPI()))
                {
                    //切换紧急模式命令
                    gGSysCmdCtrl_86[eSysCmdCtrl86_Mode] = eMODE_EMERG;
                    RcvGroupWorkModeCmdAPI(eCMDPORT_INTE);
                }
                //系统待机状态
                else if(eWORK_IDLE == GetGWorkStateAPI())
                {
                    //设置正常启动命令
                    gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTART;
                    RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
                }
            }
            //连续按下3s,切换系统为启动
            else if(sTimer >= 3)
            {
                //设置正常启动命令
                gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_NORSTART;
            	RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
                sTimer++;
            }
            //连续按下1s,切换系统为开机
            else if(sTimer >= 1)
            {
                //设置开机命令
                gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWERON;
            	RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
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
        sButtonStopReq = 0;
    }

    //记录当前IO状态
    sHisState = state;
    #endif
    #endif

    //故障状态(间隔12H自动清除故障)
    if(((eWORK_ERR == GetGWorkStateAPI()) || (ePRO_SYSPRO == GetGProtectStateAPI())
        || (DiagGetGroupSosLevel() >= 2)) && (DiagGetGroupSosLevel() < 4))
    {
    	//超过12H
    	if(GetSysSecondTimeAPI() >= (sHisTime + 43200))
    	{
            //设置清除当前故障命令
            gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd] = eCLR_NOWERR;
            RcvGroupSysClrCmdAPI(eCMDPORT_INTE);
        	//记录时间
        	sHisTime = GetSysSecondTimeAPI();
    	}
    	//时间变小(清除时间)
    	else if((GetSysSecondTimeAPI() + 10) < sHisTime)
    	{
        	//记录时间
        	sHisTime = GetSysSecondTimeAPI();
    	}
    }
    //非故障状态
    else
    {
    	//记录时间
    	sHisTime = GetSysSecondTimeAPI();
    }
}

/*=================================================================================================
** @Name      : void GroupSysPwrOffProTask(void)
** @Input     : void
** @Output    : void
** @Function  : 主控系统关机下电保护任务
** @The notes : 2s周期
**===============================================================================================*/
void GroupSysPwrOffProTask(void)
{
	/*电池堆未使能时有效*/
	#if(1 != PILE_MANAG_EN)
	//自供电(按钮或开关)时才进行下电保护
	#if((0 == BMU_DC_PWR_TYPE) || (1 == BMU_DC_PWR_TYPE))
	{
		static u32 sHL3Timer = 0;
		static u32 sHL4Timer = 0;
		static u32 sErrTimer = 0;
		static u32 sCanTimer = 0;

		//欠压2级
		if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_CEL_VL2))                 	    //单体欠压2级
			&& (GetGCellMinVoltAPI() <= gGBmuGenPara_102[eBmuGenPara102_VL2Lim])    //电压未回升
			&& (0 != gGBmuHigLevPara_103[eBmuHigLevPara103_VL2PwrOffT])             //需要保护
			&& (eErr_A == DiagGetErrFilStateAPI(eERR_ID_LEAK2))                     //未发生绝缘告警
			&& (eCURR_CHG != GetGChgDhgStateAPI())                                  //不充电
			&& ((eMODE_AUTO == GetGWorkModeAPI())                                   //自动模式
			|| (eMODE_HAND == GetGWorkModeAPI())))                                  //手动模式
		{
			//达到延时
			if(sHL3Timer >= gGBmuHigLevPara_103[eBmuHigLevPara103_VL2PwrOffT])
			{
				//设置关机命令
				gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWEROFF;
				RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
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
		if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_CEL_VL2))                 	    //单体欠压1级
			&& (GetGCellMinVoltAPI() <= gGBmuGenPara_102[eBmuGenPara102_VL1Lim])    //电压未回升
			&& (0 != gGBmuHigLevPara_103[eBmuHigLevPara103_VL1PwrOffT])             //需要保护
			&& (eErr_A == DiagGetErrFilStateAPI(eERR_ID_LEAK2))                     //未发生绝缘告警
			&& (eCURR_CHG != GetGChgDhgStateAPI())                                  //不充电
			&& ((eMODE_AUTO == GetGWorkModeAPI())                                   //自动模式
			|| (eMODE_HAND == GetGWorkModeAPI())))                                  //手动模式
		{
			//达到延时
			if(sHL4Timer >= gGBmuHigLevPara_103[eBmuHigLevPara103_VL1PwrOffT])
			{
				//设置关机命令
				gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWEROFF;
				RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
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
		if((eWORK_ERR == GetGWorkStateAPI())                                        //故障状态
			&& (0 != gGBmuHigLevPara_103[eBmuHigLevPara103_ErrPWrOffT])             //需要保护
			&& (eErr_A == DiagGetErrFilStateAPI(eERR_ID_LEAK2))                     //未发生绝缘告警
			&& ((eMODE_AUTO == GetGWorkModeAPI())                                   //自动模式
			|| (eMODE_HAND == GetGWorkModeAPI())))                                  //手动模式
		{
			//达到延时
			if(sErrTimer >= gGBmuHigLevPara_103[eBmuHigLevPara103_ErrPWrOffT])
			{
				//设置关机命令
				gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWEROFF;
				RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
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
	    if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_EXT_COFF))
	    {
	    	/*记录时间*/
	    	sCanTimer = GetSysSecondTimeAPI();
	    }
		/*外部通信中断超时*/
	    else if(ABS(GetSysSecondTimeAPI(), sCanTimer) >= gGBmuHigLevPara_103[eBmuHigLevPara103_CommPwrOffT])
		{
			//外部通讯故障下电保护
			if((0 != gGBmuHigLevPara_103[eBmuHigLevPara103_CommPwrOffT])            //需要保护
				&& (eErr_A == DiagGetErrFilStateAPI(eERR_ID_LEAK2))                 //未发生绝缘告警
				&& (eCURR_IDLE == GetGChgDhgStateAPI())                             //不充放电
				&& ((eMODE_AUTO == GetGWorkModeAPI())                               //自动模式
				|| (eMODE_HAND == GetGWorkModeAPI())))                              //手动模式
			{
				//设置关机命令
				gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_POWEROFF;
				RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
			}
		}
	}
	#endif
	#endif
}

/*=================================================================================================
** @Name      : void HeartToSlaveMcuTask(void)
** @Input     : void
** @Output    : void
** @Function  : 主MCU与从MCU(KEA128)心跳处理任务
** @The notes : 100ms周期
**===============================================================================================*/
void HeartToSlaveMcuTask(void)
{
	u8 heartState = 0;
	static u8 sTime = 0;
	static u8 sHisState = 0;
	static u8 sHeartErr = 0;

	/*主MCU切换心跳IO到从MCU*/
	if(0 == sTime)
	{
		/*心跳IO口*/
		(void)DEVGPIOSetPortState(ePOut_128EWM_OUT, ePSTATE_OFF);
		sTime = 1;
	}
	else
	{
		/*心跳IO口*/
		(void)DEVGPIOSetPortState(ePOut_128EWM_OUT, ePSTATE_ON);
		sTime = 0;
	}

	/*检测从MCU心跳信号切换*/
	heartState = DEVGPIOGetInPortState(ePIn_128EWM);
	if(sHisState != heartState)
	{
		/*心跳正常*/
		sHeartErr = 0;
		sHisState = heartState;
	}
	/*检测从MCU心跳信号不改变*/
	else
	{
		if(sHeartErr < 200)
		{
			sHeartErr++;
		}
	}

	/*从MCU心跳IO异常*/
	if(sHeartErr >= 10)
	{
		//新产生
		if(0 == BitGet(sSlavMcuErrFlag, 0))
		{
			BitSet(sSlavMcuErrFlag, 0);

    		//记录事件
			DiagUserSelfErrRecord(eUDef_ErrRcd_SmcuErr, eERR_RCDSORT_WARSET, sSlavMcuErrFlag,  0);
		}
	}
	else
	{
		//新消除
		if(1 == BitGet(sSlavMcuErrFlag, 0))
		{
			BitClr(sSlavMcuErrFlag, 0);

    		//记录事件
			DiagUserSelfErrRecord(eUDef_ErrRcd_SmcuErr, eERR_RCDSORT_WARCLR, sSlavMcuErrFlag,  0);
		}
	}
}

/*=================================================================================================
** @Name      : void CommuToSlaveMcuTask(void)
** @Input     : void
** @Output    : void
** @Function  : 主MCU与从MCU(KEA128)通信任务
** @The notes : 400ms周期
**===============================================================================================*/
void CommuToSlaveMcuTask(void)
{
	u8 rcvAddr = 0;
	static u8 sCommuErr = 0;
	static u16 sHisAddr = 0;

	//初始化或设备地址改变时发送SPI
	if((eWORK_INIT == GetGWorkStateAPI())
		|| (sHisAddr != gGBmuGenPara_102[eBmuGenPara102_BmuDev]))
	{
		/*SPI3空闲且未达到*/
		if((0 == BSPSPIGetWorkState(eSPI3)) && (sCommuErr <= 100))
		{
			/*片选从MCU的SPI有效(低电平片选有效)*/
			(void)DEVGPIOSetPortState(ePOut_128SPI_CS, ePSTATE_OFF);

			/*主MCU发送SPI到从MCU*/
			if(TRUE == BSPSPISendData(eSPI3, gGBmuGenPara_102[eBmuGenPara102_BmuDev] & 0xff))
			{
				/*回读从MCU设备地址*/
				rcvAddr = BSPSPIGetRcvData(eSPI3);

				/*从MCU设备地址正确*/
				if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == rcvAddr)
				{
					sCommuErr = 0;
					sHisAddr = rcvAddr;
				}
				else
				{
					if(sCommuErr < 200)
					{
						sCommuErr++;
					}
				}
			}

			/*取消从MCU的SPI片选(低电平片选有效)*/
			(void)DEVGPIOSetPortState(ePOut_128SPI_CS, ePSTATE_ON);
		}
	}

	/*从MCU通信SPI异常*/
	if(sCommuErr >= 5)
	{
		//新产生
		if(0 == BitGet(sSlavMcuErrFlag, 1))
		{
			BitSet(sSlavMcuErrFlag, 1);

    		//记录事件
			DiagUserSelfErrRecord(eUDef_ErrRcd_SmcuErr, eERR_RCDSORT_WARSET, sSlavMcuErrFlag,  0);
		}
	}
	else
	{
		//新消除
		if(1 == BitGet(sSlavMcuErrFlag, 1))
		{
			BitClr(sSlavMcuErrFlag, 1);

    		//记录事件
			DiagUserSelfErrRecord(eUDef_ErrRcd_SmcuErr, eERR_RCDSORT_WARCLR, sSlavMcuErrFlag,  0);
		}
	}
}

/*=================================================================================================
** @Name      : u8 GetButtonStopReqStateAPI(void)
** @Input     : void
** @Output    : 按钮请求停机状态标志:0:未请求 1:请求停机
** @Function  : 获取按钮请求停机状态
** @The notes :
**===============================================================================================*/
u8 GetButtonStopReqStateAPI(void)
{
	return(sButtonStopReq);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
