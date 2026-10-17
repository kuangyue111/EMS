/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : EventUser.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池事件记录用户处理模块
** @Instructions : 
**===============================================================================================*/
#ifndef _EVENTUSER_H
#define _EVENTUSER_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
#define EVN_RCD_EVNINFO_LEN        6                        /*一条历史事件信息长度(Word)*/
#define EVN_RCD_INIOCODE_FST       0x0400                   /*事件记录输入IO变化事件码前缀*/
#define EVN_RCD_OUTIOCODE_FST      0x0800                   /*事件记录输出IO变化事件码前缀*/
#define EVN_RCD_STATECODE_FST      0x1000                   /*事件记录状态变化事件码前缀*/
#define EVN_RCD_CMDCODE_FST        0x2000                   /*事件记录控制命令类事件码前缀*/
#define EVN_RCD_DBGCODE_FST        0x4000                   /*事件记录控制命令类事件码前缀*/
#define EVN_RCD_CFGCODE_FST        0x8000                   /*事件记录参数配置类事件码前缀*/

#define EVN_RCD_OUTIO_BIT          0xffcf                   /*事件记录输出IO变化检测位(不检测LED控制)*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/
/*事件记录类型*/
typedef enum
{
    eEVN_RCDSORT_INPUT   = 5,               /*输入口变化事件*/
    eEVN_RCDSORT_OUTPUT  = 6,               /*输出口变化事件*/
    eEVN_RCDSORT_STATE   = 7,               /*状态量变化事件*/
    eEVN_RCDSORT_CMD     = 8,               /*接收控制命令事件*/
    eEVN_RCDSORT_DEBUG   = 9,               /*接收调试命令事件*/
    eEVN_RCDSORT_CFG     = 10,              /*接收参数配置事件*/
    //eEVN_RCDSORT_SELF  = 11,              /*自身内部事件(事件记录包使用)*/
    eEVN_RCDSORT_USER100 = 100,             /*用户自定义事件*/

    eEVN_RCDSORT_LEN                        /*事件类型*/
}eEvent_RcdSort;

/*用户自定义事件记录ID*/
typedef enum
{
    eEvn_User_CLimChan = 1,                 /*充电电流限值变化事件*/
    eEvn_User_DLimChan = 2,                 /*放电电流限值变化事件*/
    eEvn_User_NBusChan,                     /*负母线电压变化事件*/
    eEvn_User_DodAdd,                       /*DOD记录增加事件*/
    eEvn_User_SysPwrOn,                     /*系统开机上电事件*/
    eEvn_User_DoorDI,                       /*电池柜门开闭事件*/
    eEvn_User_ShutPwr,                      /*百叶风机电源开闭事件*/
    eEvn_User_ShutBack,                     /*百叶风机回检状态事件*/
    eEvn_User_AirPwr,                       /*空调开关机事件*/
    eEvn_User_AirCold,                      /*空调制冷启停事件*/
    eEvn_User_AirHeat,                      /*空调制热启停事件*/
    eEvn_User_AirDehum,                     /*空调除湿启停事件*/
    eEvn_User_AirInfo,                      /*空调信息变化事件*/
    eEvn_User_IntTChan,                     /*柜内环境温度变化事件*/
    eEvn_User_ExtTChan,                     /*柜外环境温度变化事件*/
    eEvn_User_IntHChan,                     /*柜内环境湿度变化事件*/
    eEvn_User_GasChan,                      /*柜内气体浓度变化事件*/
    eEvn_User_PrChgFin,                     /*电池组预充完成事件*/

    eEvn_User_Len                        	/*用户自定义事件ID长度*/
}eUserEvn_RcdId;

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void EventRcdDisplayInit(void)
** @Input     : void
** @Output    : void
** @Function  : 事件记录显示信息初始化
** @The notes : 读取初始显示的历史事件,用户任务单次调用(避免占用太长系统初始化时间)
**===============================================================================================*/
void EventRcdDisplayInit(void);

/*=================================================================================================
** @Name      : void EventRecordUserTask(void)
** @Input     : void
** @Output    : void
** @Function  : 事件记录用户任务(定周期检查变化事件)
** @The notes : 100ms周期
**===============================================================================================*/
void EventRecordUserTask(void);

/*=================================================================================================
** @Name      : void EventRcdDisplayUserAPI(eEvent_RcdSort sort, u8 id, u16 info2, u16 info3, u16 info4, u16 info5)
** @Input     : sort:事件类型 id:事件码ID info2-5:附加信息2-5
** @Output    : void
** @Function  : 事件记录与显示用户接口
** @The notes : 用户需产生事件的地方调用,事件显示到显示区并记录存储
**===============================================================================================*/
void EventRcdDisplayUserAPI(eEvent_RcdSort sort, u8 id, u16 info2, u16 info3, u16 info4, u16 info5);

/*=================================================================================================
** @Name      : void EventChangHisRcdDisplayAPI(u8 date, u8 home)
** @Input     : date:历史事件日期[0,30]:0-30天前记录 home:读首页标志 0:读当前记录 1:读当日首页记录
** @Output    : void
** @Function  : 切换历史事件显示信息
** @The notes : 按照日期读取指定时间的历史事件显示(读30条)
**===============================================================================================*/
void EventChangHisRcdDisplayAPI(u8 date, u8 home);

/*=================================================================================================
 ** @Name      : void EventClrHisRcdDisplayAPI(void)
 ** @Input     : void
 ** @Output    : void
 ** @Function  : 清除换历史事件显示信息
 ** @The notes : 显示区域清零
 **===============================================================================================*/
 void EventClrHisRcdDisplayAPI(void);

#endif
