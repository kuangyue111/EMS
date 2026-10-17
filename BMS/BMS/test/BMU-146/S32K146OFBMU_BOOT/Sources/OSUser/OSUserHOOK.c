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
//#include "FreeRTOS.h"
//#include "task.h"
//#include "timers.h"
#include "BSPCAN.h"//测试代码
#include "BSPFTM.h"
#include "LoadApp.h"
#include "Fs6503_PowerManag.h"
#include "SST26VF064.h"
#include "OSUserHOOK.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
//static TimerHandle_t Timer5ms_Handle = NULL; 		    /* 5ms软件定时器句柄  	*/
//static TimerHandle_t Timer10ms_Handle = NULL; 		/* 10ms软件定时器句柄  	*/
//static TimerHandle_t Timer50ms_Handle = NULL; 		/* 50ms软件定时器句柄 	*/
//static TimerHandle_t Timer100ms_Handle = NULL; 		/* 100ms软件定时器句柄 	*/
//static TimerHandle_t Timer200ms_Handle = NULL; 		/* 200ms软件定时器句柄 	*/
//static TimerHandle_t Timer300ms_Handle = NULL; 		/* 300ms软件定时器句柄 	*/
//static TimerHandle_t Timer400ms_Handle = NULL; 		/* 400ms软件定时器句柄 	*/
//static TimerHandle_t Timer500ms_Handle = NULL; 		/* 500ms软件定时器句柄 	*/
//static TimerHandle_t Timer1000ms_Handle = NULL; 	    /* 1000ms软件定时器句柄 	*/
//static TimerHandle_t Timer2000ms_Handle = NULL; 	    /* 2000ms软件定时器句柄 	*/
//
////static TaskHandle_t CellMsgSample_Handler = NULL;   /* 电池信息采样任务句柄	*/
////static TaskHandle_t LowADCSample_Handler = NULL;    /* 低压信息采样任务句柄	*/
////static TaskHandle_t HigADCSample_Handler = NULL;    /* 高压信息采样任务句柄	*/
//static TaskHandle_t CanSend_Handler = NULL;    		/* CAN信息发送任务句柄	*/
//static TaskHandle_t CanRcv_Handler = NULL;    		/* CAN信息接收任务句柄	*/
//static TaskHandle_t UartRcv_Handler = NULL;    		/* UART信息接收任务句柄	*/

extern u8 sTimecounter1msflag;		/*1ms任务标志*/
extern u8 sTimecounter5msflag;	    /*5ms任务标志*/
extern u8 sTimecounter100msflag;	/*100ms任务标志*/
extern u8 sTimecounter500msflag;	/*500ms任务标志*/
extern u8 sTimecounter1000msflag;	/*1000ms任务标志*/

#if 0
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
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
#endif
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
#if 0
	/* 周期模式的软件定时器 1,定时器周期 5ms(tick)*/
	Timer5ms_Handle = xTimerCreate((const char*)"AutoReloadTimer5ms",
								(TickType_t)5,								/*  定时器周期 5(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER5MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task5ms); 			/*  回调函数 */

	/* 周期模式的软件定时器 2,定时器周期 10ms(tick)*/
	Timer10ms_Handle = xTimerCreate((const char*)"AutoReloadTimer10ms",
								(TickType_t)10,								/*  定时器周期 10(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER10MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task10ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 3,定时器周期 50ms(tick)*/
	Timer50ms_Handle = xTimerCreate((const char*)"AutoReloadTimer50ms",
								(TickType_t)50,								/*  定时器周期 50(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER50MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task50ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 4,定时器周期 100ms(tick)*/
	Timer100ms_Handle = xTimerCreate((const char*)"AutoReloadTimer100ms",
								(TickType_t)100,				    		/*  定时器周期 100(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER100MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task100ms);		/*  回调函数 */

	/* 周期模式的软件定时器 5,定时器周期 200ms(tick)*/
	Timer200ms_Handle = xTimerCreate((const char*)"AutoReloadTimer200ms",
								(TickType_t)200,							/*  定时器周期 200(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER200MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task200ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 6,定时器周期 300ms(tick)*/
	Timer300ms_Handle = xTimerCreate((const char*)"AutoReloadTimer300ms",
								(TickType_t)300,							/*  定时器周期 300(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER300MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task300ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 7,定时器周期 400ms(tick)*/
	Timer400ms_Handle = xTimerCreate((const char*)"AutoReloadTimer400ms",
								(TickType_t)400,							/*  定时器周期 400(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER400MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task400ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 8,定时器周期 500ms(tick)*/
	Timer500ms_Handle = xTimerCreate((const char*)"AutoReloadTimer500ms",
								(TickType_t)500,							/*  定时器周期 500(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER500MS,					/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task500ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 9,定时器周期 1000ms(tick)*/
	Timer1000ms_Handle = xTimerCreate((const char*)"AutoReloadTimer1000ms",
								(TickType_t)1000,							/*  定时器周期 1000(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER1000MS,				/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task1000ms); 		/*  回调函数 */

	/* 周期模式的软件定时器 10,定时器周期 2000ms(tick)*/
	Timer2000ms_Handle = xTimerCreate((const char*)"AutoReloadTimer2000ms",
								(TickType_t)2000,							/*  定时器周期 2000(tick) */
								(UBaseType_t)pdTRUE,						/*  周期模式 */
								(void* )RTOS_ID_TIMER2000MS,				/*  为每个计时器分配一个索引的唯一 ID */
								(TimerCallbackFunction_t)Task2000ms); 		/*  回调函数 */

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

	vTaskStartScheduler();                             			/* 开启任务调度器 */

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
#endif
}
#if 0
/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void Task5ms(void)
** @Input     : void
** @Output    : void
** @Function  : 5ms任务处理
** @The notes : 软件定时器的回调函数，用户自己实现
**===========================================================================================*/
static void Task5ms(void)
{
	;
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
	//GPIO复用端口管控任务
	DEVGPIOMulPortManagTask();
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
	/* 软件喂狗定时函数  */
	//BSPWatchDogTask();

	/* 在线升级程序安装和备份出处理  */
	//LoadFlashProgCopyHandleAPI();
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
	;
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
	;
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
	;
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

   if(0 == sTime)
   {
	   DEVGPIOSetPortState(ePOut_LED, ePSTATE_OFF);
	   sTime = 1;
   }
   else
   {
	   DEVGPIOSetPortState(ePOut_LED, ePSTATE_ON);
	   sTime = 0;
   }
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
	;
}
#endif
/*=============================================================================================
** @Name      : void OsAppTask(void)
** @Input     : void
** @Output    : void
** @Function  : 任务处理
** @The notes : 测试阶段
**===========================================================================================*/
void OsAppTask(void)
{
	static u32 sTimeCounter1ms = 0;
	static u32 sTimeCounter50ms = 0;
	static u8 sTick = 0;
	static u8 sTime = 0;
	u8 p = NULL;/*函数指针需要*/

	if(1 == sTimecounter1msflag)		/*1ms任务标志*/
	{
		sTimecounter1msflag = 0;

		if(sTimeCounter1ms >= 2)
		{
			sTimeCounter1ms = 0;
			BSPCANRcvTask(&p);			/*CAN接收周期函数2ms*/
			BSPCANSendTask(&p);			/*CAN发送周期函数2ms*/
		}
		sTimeCounter1ms++;
	}

	if(1 == sTimecounter5msflag)		/*5ms任务标志*/
	{
		sTimecounter5msflag = 0;
		/*电源SBC诊断任务*/
		FS65DiagnosticTask();

		/* 在线升级程序安装和备份出处理  */
		LoadFlashProgCopyHandleAPI();

		/* 50ms上传远程下载信息  */
		if(sTimeCounter50ms >= 10)
		{
			sTimeCounter50ms = 0;

			/* 上传远程下载信息  */
			CanSendRemLoadMsgAPI();
		}
		sTimeCounter50ms++;
	}

	if(1 == sTimecounter100msflag)		/*100ms任务标志*/
	{
		sTimecounter100msflag = 0;

		/*主MCU切换心跳IO到从MCU*/
		if(0 == sTick)
		{
			/*心跳IO口*/
			(void)DEVGPIOSetPortState(ePOut_128EWM_OUT, ePSTATE_OFF);
			sTick = 1;
		}
		else
		{
			/*心跳IO口*/
			(void)DEVGPIOSetPortState(ePOut_128EWM_OUT, ePSTATE_ON);
			sTick = 0;
		}

		/* 在线升级程序擦除FLASH处理  */
		LoadAppFlashErasHandleAPI();
	}

	if(1 == sTimecounter500msflag)		/*500ms任务标志*/
	{
		sTimecounter500msflag = 0;

		//板上LED显示
		if(0 == sTime)
		{
			DEVGPIOSetPortState(ePOut_LED, ePSTATE_OFF);
			sTime = 1;
		}
		else
		{
			DEVGPIOSetPortState(ePOut_LED, ePSTATE_ON);
			sTime = 0;
		}
	}

	if(1 == sTimecounter1000msflag)		/*1000ms任务标志*/
	{
		sTimecounter1000msflag = 0;

		//Flash异常检测
		EXTFlashCheckTask();
	}
}

