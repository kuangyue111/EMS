/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : Fs6503_PowerManag.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 电源管理芯片驱动模块
** @Instructions :
**===========================================================================================*/

#ifndef FUNTION_POWERMANAG_FS6503_POWERMANAG_H_
#define FUNTION_POWERMANAG_FS6503_POWERMANAG_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "sbc_fs65_common.h"
#include "status.h"
/*=============================================================================================
** @Macro definition
**===========================================================================================*/
/* Enable periodic Watchdog refresh. */
#define FS65_WD_ENABLED

#ifdef FS65_WD_ENABLED

/* Watchdog period option.  */
#define WD_PERIOD (FS65_W_FS_WD_WINDOW_512MS)			/*用户可修改喂狗周期*/
/* Watchdog window period in us. */
#define WD_PERIOD_US (128000U)
/* Watchdog timer, set to refresh the WD in 3/4 of the period (open window). */
#define WD_TIMER_PERIOD_US ((WD_PERIOD_US / 4) * 3)

/* Start timer for watchdog refresh. */
#define WD_TIMER_START() BSPLpitStartTime(0, (1 << LPIT_CHANNEL_WD));
/* Stop timer for watchdog refresh. */
#define WD_TIMER_STOP() BSPLpitStopTime(0, (1 << LPIT_CHANNEL_WD));

#endif

/* Start timer for SBC status refresh. */
#define DIAG_TIMER_START() BSPLpitStartTime(0, (1 << LPIT_CHANNEL_DIAG));
/* Stop timer for SBC status refresh. */
#define DIAG_TIMER_STOP() BSPLpitStopTime(0, (1 << LPIT_CHANNEL_DIAG));

/*************************
 * Application settings
 *************************/
/* If defined, the MCU will not wait for the RESET pin to release and
 * INIT_FS phase is skipped. */
#define _FS65_DEBUG_MODE
/* Delay between consequent reports of SBC status in [ms]. */
#define FS65_REPORT_DELAY_MS (2000U)

/* Debug pin (used e.g. for oscilloscope monitoring) */
#define SBC_DEBUG_PORT  PTD
#define DEBUG_PIN       4U

/* SBC FS0b pin */
#define SBC_FS0B_PORT       PORTD
#define SBC_FS0B_PIN        7U

/* LDT */
/* LDT after-run value [seconds]. */
#define SBC_LDT_AFTER_RUN_VALUE 5
/* LDT wake-up value [seconds]. */
#define SBC_LDT_WAKE_UP_VALUE 3

/* SPI */
#define SPI_COMM_TIMEOUT_MS 5

/*=============================================================================================
** @Data type definition
**===========================================================================================*/

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : fs65_status_t Fs6503Init(void)
** @Input     : void
** @Output    : void
** @Function  : 初始化FS6503电源芯片
** @The notes : 该函数在初始化完MCU之后调用
**===========================================================================================*/
fs65_status_t Fs6503Init(void);

/*=============================================================================================
** @Name      : fs65_status_t MCU_SPI_TransferData(uint8_t* txFrame, uint8_t* rxFrame)
** @Input     : txFrame:SPI发送数据      rxFrame：SPI接收数据
** @Output    : void
** @Function  : Implementation of driver function for SPI communication.
** @The notes :
**===========================================================================================*/
fs65_status_t MCU_SPI_TransferData(uint8_t* txFrame, uint8_t* rxFrame);

/*=============================================================================================
** @Name      : void MCU_WaitUs(uint16_t delay)
** @Input     : delay:软件延时时间
** @Output    : void
** @Function  : Implementation of driver function for blocking wait.
** @The notes :
**===========================================================================================*/
void MCU_WaitUs(uint16_t delay);

/*=============================================================================================
** @Name      : status_t sbcTestLdt(void)
** @Input     : void
** @Output    : void
** @Function  : Test of the LDT timer. Uses LDT Function 3(sleep and wake-up).
** @The notes :
**===========================================================================================*/
status_t sbcTestLdt(void);

/*=============================================================================================
** @Name      : fs65_status_t FS65DiagnosticTask(void)
** @Input     : void
** @Output    : void
** @Function  : FS6503诊断任务(上拉故障输出管脚)
** @The notes : 周期调用
**===========================================================================================*/
fs65_status_t FS65DiagnosticTask(void);

/*=============================================================================================
** @Name      : void FS65WdRefreshTask(void)
** @Input     : void
** @Output    : void
** @Function  : FS6503周期喂狗任务
** @The notes :
**===========================================================================================*/
void FS65WdRefreshTask(void);

/*=============================================================================================
** @Name      : void FS65SetUserPwrOffFlagAPI(u8 flag)
** @Input     : flag:用户下电请求标志:0上电请求 1:下电请求
** @Output    : void
** @Function  : 设置FS6503用户下电请求标志
** @The notes : 请求下电时进入软件休眠模式
**===========================================================================================*/
void FS65SetUserPwrOffFlagAPI(u8 flag);

/*=============================================================================================
** @Name      : void OSDiagTimerAddTask(void)
** @Input     : void
** @Output    : void
** @Function  : RTOS诊断计时任务
** @The notes : 500ms定时调用
**===========================================================================================*/
void OSDiagTimerAddTask(void);

#endif
