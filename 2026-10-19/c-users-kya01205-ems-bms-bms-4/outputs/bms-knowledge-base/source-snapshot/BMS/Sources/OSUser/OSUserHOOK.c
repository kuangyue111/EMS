/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : OSUserHOOK.c
** @Founder      : ZSY342
** @Date         : 2019.10.31
** @Function     : BSP定时任务服务模块
** @Instructions : 用户周期函数在这里调用
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "SST26VF064.h"
#include "Fs6503_PowerManag.h"
#include "InsuCheck.h"
#include "PortCheck.h"
#include "CellAFESamp.h"
#include "CurrSample.h"
#include "LowADCSamp.h"
#include "HigAFESamp.h"
#include "HigVoltCorr.h"
#include "OutCurrCorr.h"
#include "ParaInterface.h"
#include "DiagInterface.h"
#include "CalcInterface.h"
#include "EnerInterface.h"
#include "StaInterface.h"
#include "TacInterface.h"
#include "CANInterface.h"
#include "CHGInterface.h"
#include "MbInterface.h"
#include "ChgTactics.h"
#include "ModbusUser.h"
#include "BalanTactic.h"
#include "UserTactic.h"
#include "EventUser.h"
#include "SlaveMsg.h"
#include "GTBmmMsg.h"
#include "GroupMsg.h"
#include "CatlMsg.h"
#include "SocUserAPI.h"
#include "LoadApp.h"
#include "RecordInterface.h"
#include "OSUserHOOK.h"

#include "GroupInfo.h"
#include "PublicInfo.h"
#if(1 == PILE_MANAG_EN)
#include "PileUserAPI.h"
#endif

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static TimerHandle_t Timer2ms_Handle = NULL; 		/* 2ms软件定时器句柄  	*/
static TimerHandle_t Timer5ms_Handle = NULL; 		/* 5ms软件定时器句柄  	*/
static TimerHandle_t Timer10ms_Handle = NULL; 		/* 10ms软件定时器句柄  	*/
static TimerHandle_t Timer50ms_Handle = NULL; 		/* 50ms软件定时器句柄 	*/
static TimerHandle_t Timer100ms_Handle = NULL; 		/* 100ms软件定时器句柄 	*/
static TimerHandle_t Timer200ms_Handle = NULL; 		/* 200ms软件定时器句柄 	*/
static TimerHandle_t Timer300ms_Handle = NULL; 		/* 300ms软件定时器句柄 	*/
static TimerHandle_t Timer400ms_Handle = NULL; 		/* 400ms软件定时器句柄 	*/
static TimerHandle_t Timer500ms_Handle = NULL; 		/* 500ms软件定时器句柄 	*/
static TimerHandle_t Timer1000ms_Handle = NULL; 	/* 1000ms软件定时器句柄 	*/
static TimerHandle_t Timer2000ms_Handle = NULL; 	/* 2000ms软件定时器句柄 	*/

/*菊花链单体采样方式*/
#if(0 == BMU_BSU_COM_TYPE)
static TaskHandle_t CellMsgSample_Handler = NULL;   /* 电池信息采样任务句柄	*/
#endif
static TaskHandle_t LowADCSample_Handler = NULL;    /* 低压信息采样任务句柄	*/
static TaskHandle_t HigADCSample_Handler = NULL;    /* 高压信息采样任务句柄	*/
static TaskHandle_t CanSend_Handler = NULL;    		/* CAN信息发送任务句柄	*/
static TaskHandle_t CanRcv_Handler = NULL;    		/* CAN信息接收任务句柄	*/
static TaskHandle_t UartRcv_Handler = NULL;    		/* UART信息接收任务句柄	*/
//创建堆控定时任务:
#if(1 == PILE_MANAG_EN)
static TaskHandle_t PileAll_Handler = NULL;    		/* 堆控总定时任务句柄		*/
#endif

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void Task2ms(void)
** @Input     : void
** @Output    : void
** @Function  : 2ms任务处理
** @The notes : 软件定时器的回调函数，用户自己实现
**===========================================================================================*/
static void Task2ms(void);

/*=============================================================================================
** @Name      : static void Task5ms(void)
** @Input     : void
** @Output    : void
** @Function  : 5ms任务处理
** @The notes : 软件定时器的回调函数，用户自己实现
**===========================================================================================*/
static void Task5ms(void);

/*=============================================================================================
** @Name      : static void Task10ms(void)
** @Input     : void
** @Output    : void
** @Function  : 10ms任务处理
** @The notes :
**===========================================================================================*/
static void Task10ms(void);

/*=============================================================================================
** @Name      : static void Task50ms(void)
** @Input     : void
** @Output    : void
** @Function  : 50ms任务处理
** @The notes :
**===========================================================================================*/
static void Task50ms(void);

/*=============================================================================================
** @Name      : static void Task100ms(void)
** @Input     : void
** @Output    : void
** @Function  : 100ms任务处理
** @The notes :
**===========================================================================================*/
static void Task100ms(void);

/*=============================================================================================
** @Name      : static void Task200ms(void)
** @Input     : void
** @Output    : void
** @Function  : 200ms任务处理
** @The notes :
**===========================================================================================*/
static void Task200ms(void);

/*=============================================================================================
** @Name      : static void Task300ms(void)
** @Input     : void
** @Output    : void
** @Function  : 300ms任务处理
** @The notes :
**===========================================================================================*/
static void Task300ms(void);

/*=============================================================================================
** @Name      : static void Task400ms(void)
** @Input     : void
** @Output    : void
** @Function  : 400ms任务处理
** @The notes :
**===========================================================================================*/
static void Task400ms(void);

/*=============================================================================================
** @Name      : static void Task500ms(void)
** @Input     : void
** @Output    : void
** @Function  : 500ms任务处理
** @The notes :
**===========================================================================================*/
static void Task500ms(void);

/*=============================================================================================
** @Name      : static void Task1000ms(void)
** @Input     : void
** @Output    : void
** @Function  : 1000ms任务处理
** @The notes :
**===========================================================================================*/
static void Task1000ms(void);

/*=============================================================================================
** @Name      : static void Task2000ms(void)
** @Input     : void
** @Output    : void
** @Function  : 2000ms任务处理
** @The notes :
**===========================================================================================*/
static void Task2000ms(void);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void RtosTaskStart(void)
** @Input     : void
** @Output    : void
** @Function  : RTOS定时任务
** @The notes :
**===========================================================================================*/
void RtosTaskStart(void)
{
	/* 周期模式的软件定时器 1,定时器周期 2ms(tick)*/
	Timer2ms_Handle = xTimerCreate((const char*)"AutoReloadTimer2ms",
								(TickType_t)2,								/*  定时器周期 2(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER2MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task2ms); 			/*  回调函数 */

	/* 周期模式的软件定时器 2,定时器周期 5ms(tick)*/
	Timer5ms_Handle = xTimerCreate((const char*)"AutoReloadTimer5ms",
								(TickType_t)5,								/*  定时器周期 5(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER5MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task5ms); 			/*  回调函数 */

	/* 周期模式的软件定时器 3,定时器周期 10ms(tick)*/
	Timer10ms_Handle = xTimerCreate((const char*)"AutoReloadTimer10ms",
								(TickType_t)10,								/*  定时器周期 10(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER10MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task10ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 4,定时器周期 50ms(tick)*/
	Timer50ms_Handle = xTimerCreate((const char*)"AutoReloadTimer50ms",
								(TickType_t)50,								/*  定时器周期 50(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER50MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task50ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 5,定时器周期 100ms(tick)*/
	Timer100ms_Handle = xTimerCreate((const char*)"AutoReloadTimer100ms",
								(TickType_t)100,				    		/*  定时器周期 100(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER100MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task100ms);		/*  回调函数 */

	/* 周期模式的软件定时器 6,定时器周期 200ms(tick)*/
	Timer200ms_Handle = xTimerCreate((const char*)"AutoReloadTimer200ms",
								(TickType_t)200,							/*  定时器周期 200(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER200MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task200ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 7,定时器周期 300ms(tick)*/
	Timer300ms_Handle = xTimerCreate((const char*)"AutoReloadTimer300ms",
								(TickType_t)300,							/*  定时器周期 300(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER300MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task300ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 8,定时器周期 400ms(tick)*/
	Timer400ms_Handle = xTimerCreate((const char*)"AutoReloadTimer400ms",
								(TickType_t)400,							/*  定时器周期 400(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER400MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task400ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 9,定时器周期 500ms(tick)*/
	Timer500ms_Handle = xTimerCreate((const char*)"AutoReloadTimer500ms",
								(TickType_t)500,							/*  定时器周期 500(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER500MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task500ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 10,定时器周期 1000ms(tick)*/
	Timer1000ms_Handle = xTimerCreate((const char*)"AutoReloadTimer1000ms",
								(TickType_t)1000,							/*  定时器周期 1000(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER1000MS,				/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task1000ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 11,定时器周期 2000ms(tick)*/
	Timer2000ms_Handle = xTimerCreate((const char*)"AutoReloadTimer2000ms",
								(TickType_t)2000,							/*  定时器周期 2000(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER2000MS,				/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task2000ms); 		/*  回调函数 */

	if(Timer2ms_Handle != NULL)
	{
		xTimerStart(Timer2ms_Handle, 0);      /* 开启2ms周期定时器 ,0表示不阻塞*/
	}

	if(Timer5ms_Handle != NULL)
	{
		xTimerStart(Timer5ms_Handle, 0);      /* 开启5ms周期定时器 ,0表示不阻塞*/
	}

	if(Timer10ms_Handle != NULL)
	{
		xTimerStart(Timer10ms_Handle, 0);     /* 开启10ms周期定时器 ,0表示不阻塞*/
	}

	if(Timer50ms_Handle != NULL)
	{
		xTimerStart(Timer50ms_Handle, 0);     /* 开启50ms周期定时器 ,0表示不阻塞 */
	}

	if(Timer100ms_Handle != NULL)
	{
		xTimerStart(Timer100ms_Handle, 0);    /* 开启100ms周期定时器 ,0表示不阻塞 */
	}

	if(Timer200ms_Handle != NULL)
	{
		xTimerStart(Timer200ms_Handle, 0);    /* 开启200ms周期定时器 ,0表示不阻塞 */
	}

	if(Timer300ms_Handle != NULL)
	{
		xTimerStart(Timer300ms_Handle, 0);    /* 开启300ms周期定时器 ,0表示不阻塞 */
	}

	if(Timer400ms_Handle != NULL)
	{
		xTimerStart(Timer400ms_Handle, 0);    /* 开启400ms周期定时器 ,0表示不阻塞 */
	}

	if(Timer500ms_Handle != NULL)
	{
		xTimerStart(Timer500ms_Handle, 0);    /* 开启500ms周期定时器 ,0表示不阻塞 */
	}

	if(Timer1000ms_Handle != NULL)
	{
		xTimerStart(Timer1000ms_Handle, 0);   /* 开启1000ms周期定时器 ,0表示不阻塞 */
	}

	if(Timer2000ms_Handle != NULL)
	{
		xTimerStart(Timer2000ms_Handle, 0);   /* 开启2000ms周期定时器 ,0表示不阻塞 */
	}

	//创建高压信息采样任务:
	xTaskCreate((TaskFunction_t)    HigInfoSampleTask,          /* 任务函数名 */
				(const char *)      "HigInfoSampleTask",        /* 任务名称 */
				(u16)          		HV_SAMP_TASK_SIZE,       	/* 任务堆栈大小 */
				(void *)            NULL,              			/* 任务参数，一般为空 */
				(UBaseType_t)       RTOS_ID_HV_SAMP,      		/* 任务优先级 */
				(TaskHandle_t *)    &HigADCSample_Handler);     /* 任务句柄 */

	//创建电池信息采样任务:
	#if(0 == BMU_BSU_COM_TYPE)/*菊花链单体采样方式*/
	xTaskCreate((TaskFunction_t)    CellInfoSampleTask,         /* 任务函数名 */
				(const char *)      "CellInfoSampleTask",       /* 任务名称 */
				(u16)          		CELL_SAMP_TASK_SIZE,        /* 任务堆栈大小 */
				(void *)            NULL,              			/* 任务参数，一般为空 */
				(UBaseType_t)       RTOS_ID_CELL_SAMP,      	/* 任务优先级 */
				(TaskHandle_t *)    &CellMsgSample_Handler);    /* 任务句柄 */
	#endif

	//创建低压信息采样任务:
	xTaskCreate((TaskFunction_t)    LowInfoSampleTask,          /* 任务函数名 */
				(const char *)      "LowInfoSampleTask",        /* 任务名称 */
				(u16)          		LV_SAMP_TASK_SIZE,       	/* 任务堆栈大小 */
				(void *)            NULL,              			/* 任务参数，一般为空 */
				(UBaseType_t)       RTOS_ID_LV_SAMP,      		/* 任务优先级 */
				(TaskHandle_t *)    &LowADCSample_Handler);     /* 任务句柄 */

	//创建CAN信息发送任务:
	xTaskCreate((TaskFunction_t)    BSPCANSendTask,          	/* 任务函数名 */
				(const char *)      "BSPCANSendTask",        	/* 任务名称 */
				(u16)          		CAN_SEND_TASK_SIZE,       	/* 任务堆栈大小 */
				(void *)            NULL,              			/* 任务参数，一般为空 */
				(UBaseType_t)       RTOS_ID_CAN_SEND,      		/* 任务优先级 */
				(TaskHandle_t *)    &CanSend_Handler);     		/* 任务句柄 */

	//创建CAN信息接收任务:
	xTaskCreate((TaskFunction_t)    BSPCANRcvTask,          	/* 任务函数名 */
				(const char *)      "BSPCANRcvTask",        	/* 任务名称 */
				(u16)          		CAN_RCV_TASK_SIZE,       	/* 任务堆栈大小 */
				(void *)            NULL,              			/* 任务参数，一般为空 */
				(UBaseType_t)       RTOS_ID_CAN_RCV,      		/* 任务优先级 */
				(TaskHandle_t *)    &CanRcv_Handler);     		/* 任务句柄 */

	//创建UART信息接收任务:
	xTaskCreate((TaskFunction_t)    BSPUARTRcvTask,          	/* 任务函数名 */
				(const char *)      "BSPUARTRcvTask",        	/* 任务名称 */
				(u16)          		UART_RCV_TASK_SIZE,       	/* 任务堆栈大小 */
				(void *)            NULL,              			/* 任务参数，一般为空 */
				(UBaseType_t)       RTOS_ID_UART_RCV,      		/* 任务优先级 */
				(TaskHandle_t *)    &UartRcv_Handler);     		/* 任务句柄 */

	//用户创建自己的任务ID，参照以上例子

	//创建堆控定时任务:
	#if(1 == PILE_MANAG_EN)
	xTaskCreate((TaskFunction_t)    PileAllTimer10msTask,       /* 任务函数名 */
				(const char *)      "PileAll10msTask",        	/* 任务名称 */
				(u16)          		PILE_ALL_TASK_SIZE,       	/* 任务堆栈大小 */
				(void *)            NULL,              			/* 任务参数，一般为空 */
				(UBaseType_t)       RTOS_ID_PILE_ALL,      		/* 任务优先级 */
				(TaskHandle_t *)    &PileAll_Handler);     		/* 任务句柄 */
	#endif

	vTaskStartScheduler();                             			/* 开启任务调度器 */

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void Task2ms(void)
** @Input     : void
** @Output    : void
** @Function  : 2ms任务处理
** @The notes : 软件定时器的回调函数，用户自己实现
**===========================================================================================*/
static void Task2ms(void)
{
	/*Modbus通信包定时触发任务(2ms)*/
	MbTrigSendTimerTask();
}

/*=============================================================================================
** @Name      : static void Task5ms(void)
** @Input     : void
** @Output    : void
** @Function  : 5ms任务处理
** @The notes : 软件定时器的回调函数，用户自己实现
**===========================================================================================*/
static void Task5ms(void)
{
	/*CAN通信包定时发送任务*/
	CANLibHandleTask_API();

	/*电源SBC诊断任务*/
	FS65DiagnosticTask();

	/*兼容CATL电池协议定时发送任务(原协议周期)*/
	#if(1 == BMU_CATL_CAN_EN)
	//CanSendUpCatlMsgCycleTask();
	/*并机激活上传并机汇总信息*/
	#if(1 == PILE_MANAG_EN)
	//PileCanUpCatlMsgCycleTask();
	#endif
	#endif
}

/*=============================================================================================
** @Name      : static void Task10ms(void)
** @Input     : void
** @Output    : void
** @Function  : 10ms任务处理
** @The notes : 软件定时器的回调函数，用户自己实现
**===========================================================================================*/
static void Task10ms(void)
{
	static u8 sTime = 0;

	//GPIO复用端口管控任务
	DEVGPIOMulPortManagTask();
	PortStateCheckTask();

	//使能兼容高特从控BMM通信信息
	#if((1 == BMU_BSU_COM_TYPE) && (1 == BMU_BSU_CAN_TYPE))
	/*主控下发GT从控BMM协议CAN报文*/
	GroupSendDnGTBmmMsg_API();
	#endif

	/*兼容CATL电池协议定时发送任务(放慢1倍周期)*/
	#if(1 == BMU_CATL_CAN_EN)
	CanSendUpCatlMsgCycleTask();
	/*并机激活上传并机汇总信息*/
	#if(1 == PILE_MANAG_EN)
	PileCanUpCatlMsgCycleTask();
	#endif
	#endif

	/*10ms驱动100ms任务*/
	switch(sTime)
	{
		/*电流限值计算任务*/
		case 0:
			CurrLimCalcTask();
			CurrLimFallTask();
			break;

		/*电压温度电流计算任务*/
		case 1:
			CheckVoltCharaTask();
			CheckTempCharaTask();
			CheckAvgCurrTask();
			break;

		/*故障诊断任务1*/
		case 2:
			DiagVoltLimFilMakerTask();
			DiagTempLimFilMakerTask();
			DiagCurrLimFilMakerTask();
			break;

		/*故障诊断任务2*/
		case 3:
			DiagPortExpFilMakerTask();
			DiagInfoExpFilMakerTask();
			break;

		/*故障诊断任务3*/
		case 4:
			DiagStateExpFilMakerTask();
			DiagErrDisplayTask();
			break;

		/*电能量计算任务*/
		case 5:
			GroupCapSocCalcTask();
			GroupEnerSoeCalcTask();
			break;

		/*电能量显示任务*/
		case 6:
			GroupFadeSohCalcTask();
			GroupEnerDisplayTask();
			break;

		/*电池组电池状态检测与均衡控制任务*/
		case 7:
			PwmInfoCaptureTask();
			GroupBattStateCheckTask();
			//菊花链通信从控或自研CAN通信从控
			#if((0 == BMU_BSU_COM_TYPE) || (0 == BMU_BSU_CAN_TYPE))
			GroupVoltBalanTacticTask();
			#endif
			break;

		/*绝缘检测/接触器粘连检测和电池组保护状态检测*/
		case 8:
			InsuAdhesMeasurTask();
			GroupProStateTask();
			EventRecordUserTask();
			break;

		/*其它任务*/
		case 9:
			DiagSwhChangCStateCheckTask();
			CHGLibSendMsgTask();
			ParaStoreCopyTask();
			break;

		default:
			break;
	}

	sTime++;
	if(sTime >= 10)
	{
		sTime = 0;
	}
}

/*=============================================================================================
** @Name      : static void Task50ms(void)
** @Input     : void
** @Output    : void
** @Function  : 50ms任务处理
** @The notes :
**===========================================================================================*/
static void Task50ms(void)
{
    /*故障列表滤波检测*/
	DiagErrFilterTask(eERR_ID_CEL_VH2, eERR_ID_FINISH);

    /*SOC、SOE算法包总任务*/
	SocSoeLibAllTask();

	/*主控用户发送CAN报文*/
	GroupSendUpMsg1sBy50ms_API();

	//使能自研从控BSU CAN通信信息
	#if((1 == BMU_BSU_COM_TYPE) && (0 == BMU_BSU_CAN_TYPE))
	/*主控下发报文和判断从控信息采集完成*/
	GroupCheckRcvBsuMsgAPI();
	#endif
}

/*=============================================================================================
** @Name      : static void Task100ms(void)
** @Input     : void
** @Output    : void
** @Function  : 100ms任务处理
** @The notes :
**===========================================================================================*/
static void Task100ms(void)
{
	/*软件喂狗定时函数  */
	//BSPWatchDogTask();

    /*电池组工作状态检测*/
	GroupWorkStateTask();

    /*电池组充放电状态检测*/
	GroupChgDhgStateTask();

    /*电池组安全等级诊断*/
	DiagGroupSosTask();

    /*电池组故障保护控制*/
	GroupProtectTask();

    /*故障录波数据处理(非数据存储)*/
	RcdFaultWaveHandleTask();
}

/*=============================================================================================
** @Name      : static void Task200ms(void)
** @Input     : void
** @Output    : void
** @Function  : 200ms任务处理
** @The notes :
**===========================================================================================*/
static void Task200ms(void)
{
	static u8 sTime = 0;

	/*200ms驱动1s任务*/
	switch(sTime)
	{
		/*热管理控制任务*/
		case 0:
			TacGHeatManagTacticTask();
			break;

		/*电压限值计算任务*/
		case 1:
			VoltLimCalcTask();
			break;

		/*工作模式检测任务*/
		case 2:
			GroupWorkModeTask();
			break;

		/*通信诊断任务*/
		case 3:
			DiagCommuCheckTask();
			break;

		/*高压采样/电流采样校准任务*/
		case 4:
			HigVoltSampCorrProcess();
			OutCurrSampCorrProcess();
			break;

		default:
			break;
	}

	sTime++;
	if(sTime >= 5)
	{
		sTime = 0;
	}
}

/*=============================================================================================
** @Name      : static void Task300ms(void)
** @Input     : void
** @Output    : void
** @Function  : 300ms任务处理
** @The notes :
**===========================================================================================*/
static void Task300ms(void)
{
	static u8 sTime  = 0;

	/*单次循环充放电量统计*/
	GroupCycleCapStatisTask();

	/*在线升级擦除外部FLASH程序下载区任务*/
	LoadAppFlashErasHandleAPI();

	/*远程升级状态信息报文发送任务*/
	CanSendRemLoadMsgAPI();

	//转化600ms周期
	if(0 == sTime)
	{
		/*历史记录处理任务*/
		RcdStoreHandleTask();
		sTime = 1;
	}
	else
	{
		sTime = 0;
	}
}

/*=============================================================================================
** @Name      : static void Task400ms(void)
** @Input     : void
** @Output    : void
** @Function  : 400ms任务处理
** @The notes :
**===========================================================================================*/
static void Task400ms(void)
{
	/*累计充放电量统计*/
	GroupHisCapStatisTask();

	/*累计充放电能统计*/
	GroupHisEnerStatisTask();

	/*与辅助MCU通信处理任务(收发SPI传递设备地址)*/
	CommuToSlaveMcuTask();
}

/*=============================================================================================
** @Name      : static void Task500ms(void)
** @Input     : void
** @Output    : void
** @Function  : 500ms任务处理
** @The notes :
**===========================================================================================*/
static void Task500ms(void)
{
	/*外围故障检测任务*/
	DiagPerDevCheckTask();

	/*快充策略任务*/
	GroupFstChgTacticsTask();

	/*RTOS诊断计时任务*/
	OSDiagTimerAddTask();

	/*辅助设备管理任务*/
	TacAuxDevManagTask();
}

/*=============================================================================================
** @Name      : static void Task1000ms(void)
** @Input     : void
** @Output    : void
** @Function  : 1000ms任务处理
** @The notes :
**===========================================================================================*/
static void Task1000ms(void)
{
	static u8 sTime = 0;

	/*外围输出口控制任务*/
	TacGPeriManagTacticTask();

	/*主控系统输入IO口启停控制任务*/
	GroupSysIoUpDnCtrlTask();

	if(0 == sTime)
	{
		/*充放电次数统计任务*/
		GroupChgTimesStatisTask();
		sTime = 1;
	}
	else
	{
		/*充放电剩余时间计算*/
		GroupAllowTimeStatisTask();
		sTime = 0;
	}

	/*放电深度DOD统计记录任务*/
	GroupDodStatisRcdTask();

	/*外部Flash检测任务*/
	EXTFlashCheckTask();
}

/*=============================================================================================
** @Name      : static void Task2000ms(void)
** @Input     : void
** @Output    : void
** @Function  : 2000ms任务处理
** @The notes :
**===========================================================================================*/
static void Task2000ms(void)
{
	static u8 sState = 0;

	/*主控系统下电关机保护任务*/
	GroupSysPwrOffProTask();

	/*故障告警次数统计任务*/
	GroupErrNumStatisTask();

	/*Modbus通信检测复位任务*/
	//MbCheckRcvOffResetTask();

	/*系统时间记录任务*/
	SysClockRecordTask();

	/*系统项目信息更新显示任务*/
	SysDispProjInfoTask();

	/*4s任务*/
	if(0 == sState)
	{
		/*外部RTC检测任务*/
		DEVEXTRTCCheckTask();
		sState = 1;
	}
	else
	{
		/*外部EEPROM检测任务*/
		DEVEXTEEPCheckTask();
		sState = 0;
	}
}
