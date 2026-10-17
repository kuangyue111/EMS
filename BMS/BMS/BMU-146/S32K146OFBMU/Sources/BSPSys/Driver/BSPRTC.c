/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPRTC.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 内部RTC驱动函数模块
** @Instructions : MCU内部按秒计数的寄存器，需要外部纽扣电池
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPConfig.h"
#include "BSPTypeDef.h"
#include "rtc_driver.h"
#include "BSPRTC.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;

/* rtcTimer1 configuration structure */
extern const rtc_init_config_t rtcTimer1_Config0;

/* RTC Initial Time and Date */
extern rtc_timedate_t rtcStartTime;

/* rtcTimer1 Alarm configuration 0 */
extern rtc_alarm_config_t alarmConfig0;

/* RTC Seconds Interrupt Configuration */
extern rtc_seconds_int_config_t rtc_seconds_initconfig;

extern rtc_register_lock_config_t rtc_register_lock_config;

rtc_Currtimedate_t currentTime;

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPRTCInit(void)
** @Input     : void
** @Output    : void
** @Function  : 内部RTC模块初始化
** @The notes :
**===========================================================================================*/
u8 BSPRTCInit(void)
{
    /*模块未使能不进行初始化*/
    if(1 != gBSPFunCfg.Bits.RTCEn)
    {
        return(FALSE);
    }

    /* Call the init function */
    RTC_DRV_Init(0UL, &rtcTimer1_Config0);

    /* Set the time and date */
    RTC_DRV_SetTimeDate(0UL, &rtcStartTime);

    /* Start RTC counter */
    RTC_DRV_StartCounter(0UL);
    
    /* Configure an alarm every 3 seconds */
    RTC_DRV_ConfigureAlarm(0UL, &alarmConfig0);

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPRTCSetCurrSecTime(rtc_Currtimedate_t *time)
** @Input     : time:当前时间(1s)
** @Output    : TRUE:设置成功 FALSE:设置失败
** @Function  : 设置内部RTC当前时间
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPRTCSetCurrSecTime(rtc_Currtimedate_t *time)
{
    /*模块未使能不进行初始化*/
    if(1 != gBSPFunCfg.Bits.RTCEn)
    {
        return(FALSE);
    }
    
    rtcStartTime.year = time->year;
    rtcStartTime.month = time->month;
    rtcStartTime.day = time->day;
    rtcStartTime.hour = time->hour;
    rtcStartTime.minutes = time->minutes;
    rtcStartTime.seconds = time->seconds;

    /* Set the time and date */
    RTC_DRV_SetTimeDate(0UL, &rtcStartTime);
    
    /* Start RTC counter */
    RTC_DRV_StartCounter(0UL);
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : rtc_Currtimedate_t BSPRTCGetCurrSecTime(void)
** @Input     : void
** @Output    : 当前内部RTC时间
** @Function  : 获取内部RTC当前时间
** @The notes : 用户调用
**===========================================================================================*/
rtc_Currtimedate_t BSPRTCGetCurrSecTime(void)
{
    rtc_timedate_t currentTimeTemp;
    
    /*模块未使能不进行初始化*/
    if(1 != gBSPFunCfg.Bits.RTCEn)
    {
    	return(currentTime);
    }
    
    RTC_DRV_GetCurrentTimeDate(0U, &currentTimeTemp);

    currentTime.year = currentTimeTemp.year;
    currentTime.month = currentTimeTemp.month;
    currentTime.day = currentTimeTemp.day;
    currentTime.hour = currentTimeTemp.hour;
    currentTime.minutes = currentTimeTemp.minutes;
    currentTime.seconds = currentTimeTemp.seconds;
    
    return(currentTime);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
