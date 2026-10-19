/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : TacInterface.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池组策略模块用户接口
** @Instructions : 
**===============================================================================================*/
#ifndef _TACINTERFACE_H
#define _TACINTERFACE_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicInfo.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
#define TAC_HS_ON_CURR                  1                   /*高边开关闭合驱动电流 1mA*/
#define TAC_SMG_ONCON_T                 10                  /*烟感电源复位后等待信号确认时间(默认10s) 1s*/

/*根据项目选择调速风扇控制参数*/
#if((0 == PRJ_PARA_NUM_INFO) || (3 == PRJ_PARA_NUM_INFO) || (4 == PRJ_PARA_NUM_INFO))/*标准PACK风扇*/
#define IMLU_FAN_BACK_TYPE              (1)                 /*风扇回检方式(0:不回检 1仅回检且为运行状态 2仅回检且为告警状态 3回检和控制保护且为运行状态 3回检和控制保护且为告警状态)*/
#define IMLU_FAN_MIN_SPEED              (22)                /*风扇最低起转速度[最大50%,默认22%,不同风扇不同,用作风扇刚好转动轻微内循环](1%)*/
#define IMLU_FAN_HOL_SPEED              (50)                /*风扇怠速起转速度[最大80%,默认40%,不同风扇不同,用作默认转速防止热空气回流进入电池箱](1%)*/
#define IMLU_FAN_RUN_SPEED              (60)                /*风扇排风起始速度[最大90%,默认50%,确保需要开始排风时有足够风量](1%)*/
#define IMLU_FAN_CTRL_TIMES            	(7)                 /*风扇控制失败超时次数[每次为7s,默认5次(*7s)，风扇实际相应至少需要约8s](1次)*/
#else                                                       /*CATL PACK风扇*/
#define IMLU_FAN_BACK_TYPE              (0)                 /*风扇回检方式(0:不回检 1仅回检且为运行状态 2仅回检且为告警状态 3回检和控制保护且为运行状态 3回检和控制保护且为告警状态)*/
#define IMLU_FAN_MIN_SPEED              (5)                 /*风扇最低起转速度[最大50%,默认5%,不同风扇不同,用作风扇刚好转动轻微内循环](1%)*/
#define IMLU_FAN_HOL_SPEED              (50)                /*风扇怠速起转速度[最大80%,默认10%,不同风扇不同,用作默认转速防止热空气回流进入电池箱](1%)*/
#define IMLU_FAN_RUN_SPEED              (70)                /*风扇稳定起转速度[最大90%,默认30%,确保需要开始排风时有足够风量](1%)*/
#define IMLU_FAN_CTRL_TIMES             (3)                 /*风扇控制失败超时次数[每次为7s,默认5次(*7s)，风扇实际相应至少需要约8s](1次)*/
#endif
#define FULL_FAN_CHGDHG_CURR            (8)                 /*全速开风扇的大电流充放电电流[默认0.8C](0.1C)*/
#define FULL_FAN_CHGDHG_TIME            (1200)              /*全速开风扇的大电流充放电持续时间[0.8C,默认20min](1s)*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/
/*外围接触器类型定义*/
typedef enum
{
    eGroupSwh_Mai = 0,                  /*主正接触器*/
    eGroupSwh_Pre,                      /*预充接触器*/
    eGroupSwh_Neg,                      /*负极接触器*/
    eGroupSwh_Pwr,                      /*电源接触器*/
    eGroupSwh_Run,                      /*运行灯LED*/
    eGroupSwh_Alm,                      /*故障灯LED*/
    eGroupSwh_Smg,                      /*烟感电源*/
    eGroupSwh_Hot,                      /*加热接触器*/
    eGroupSwh_Fan,                      /*风扇电源*/

    eGroupSwh_Sta,                      /*状态干接点*/
    eGroupSwh_Chg,                      /*充电干接点*/
    eGroupSwh_Dhg,                      /*放电干接点*/

	eGroupSwh_Num                       /*接触器个数*/
}e_GroupSwhSort;

/*电池组启动失败原因*/
typedef enum
{
    eStartFailReason_SysFault = 0,      /*系统故障*/
    eStartFailReason_StartLim = 1,      /*启动受限*/
    eStartFailReason_PeripExp = 2,      /*外设异常*/
    eStartFailReason_SwtchExp = 3,      /*接触器异常*/
    eStartFailReason_PChgTOvr = 4,      /*预充过温*/
    eStartFailReason_PChgFail = 5,      /*预充未满*/
    eStartFailReason_PChgCErr = 6,      /*预充电流异常*/

    eStartFailReason_Fin
}e_StartFailReason;

/*电池组停机失败原因*/
typedef enum
{
    eStopFailReason_SysFault = 0,       /*系统故障*/
    eStopFailReason_CurrFail = 1,       /*等待安全电流失败*/
    eStopFailReason_SwtchExp = 2,       /*接触器异常*/
    eStopFailReason_OffFail  = 3,       /*接触器断开失败*/

    eStopFailReason_Fin
}e_StopFailReason;

/*电池组紧急停机原因*/
typedef enum
{
    eEmergStopReason_SysInt = 0,        /*系统内部急停*/
    eEmergStopReason_ExtCmd = 1,        /*外部命令急停*/
    eEmergStopReason_Button = 2,        /*按钮命令急停*/

    eEmergStopReason_Fin
}e_EmergStopReason;

/*IMEU2设置空调命令索引值*/
typedef enum
{
    eEuAirCmd_ColdOnT = 0,              /*设置制冷设定温度命令*/
	eEuAirCmd_ColdOffT = 1,             /*设置制冷偏移温度命令*/
	eEuAirCmd_HTWarnT = 2,              /*设置过温告警温度命令*/
	eEuAirCmd_LTWarnT = 3,              /*设置低温告警温度命令*/
	eEuAirCmd_HeatOnT = 4,              /*设置制热设定温度命令*/
	eEuAirCmd_HeatOffT = 5,             /*设置制热偏移温度命令*/
	eEuAirCmd_ForPwr = 6,               /*设置强制开关机命令*/
	eEuAirCmd_ForCold = 7,              /*设置强制制冷模式命令*/
	eEuAirCmd_ForBlow = 8,              /*设置强制送风模式命令*/
	eEuAirCmd_ForIdle = 9,              /*设置强制待机模式命令*/
	eEuAirCmd_ForHeat = 10,             /*设置强制制热模式命令*/
    eEuAirCmd_DehOnT = 11,              /*设置除湿开启温度命令*/
    eEuAirCmd_DehOffT = 12,             /*设置除湿关闭温度命令*/
    eEuAirCmd_DehOnH = 13,              /*设置除湿开启湿度命令*/
    eEuAirCmd_DehOffH = 14,             /*设置除湿关闭湿度命令*/

	eEuAirCmd_DehumDiff = 15,			/*除湿湿度回差,1%*/
	eEuAirCmd_HHWarnH = 16,				/*高湿度告警湿度,1%*/

	eEuAirCmd_Len
}e_EuAirCmdIndex;

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void GroupStateInfoInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组状态信息初始化
** @The notes : 保护状态/工作状态/充放电状态
**===============================================================================================*/
extern void GroupStateInfoInit(void);

/*=================================================================================================
** @Name      : void GroupProStateTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组保护状态检测任务
** @The notes : 100ms周期调用
**===============================================================================================*/
extern void GroupProStateTask(void);

/*=================================================================================================
** @Name      : void void GroupWorkStateTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组工作状态检测任务
** @The notes : 100ms周期调用
**===============================================================================================*/
extern void GroupWorkStateTask(void);

/*=================================================================================================
** @Name      : void void GroupChgDhgStateTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组充放电状态检测任务
** @The notes : 100ms周期调用
**===============================================================================================*/
extern void GroupChgDhgStateTask(void);

/*=================================================================================================
** @Name      : void GroupBattStateCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组电池电压状态检测任务
** @The notes : 100ms周期调用
**===============================================================================================*/
extern void GroupBattStateCheckTask(void);

/*=================================================================================================
** @Name      : void TacClrGCtrlErrFlagAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除电池组控制失败标志
** @The notes :
**===============================================================================================*/
extern void TacClrGCtrlErrFlagAPI(void);

/*=================================================================================================
** @Name      : u8 TacGetGStartFailFlagAPI(void)
** @Input     : void
** @Output    : 启动失败标志(按bit位表示 bit0:系统故障 bit1:启动受限 bit2:外设异常 bit3:接触器异常 bit4:预充过温 bit5:预充未满失败 bit6:预充电流异常)
** @Function  : 获取电池组启动失败标志
** @The notes : 启动失败原因
**===============================================================================================*/
extern u8 TacGetGStartFailFlagAPI(void);

/*=================================================================================================
** @Name      : u8 TacGetGStopFailFlagAPI(void)
** @Input     : void
** @Output    : 停机失败标志(按bit位表示 bit0:系统故障 bit1:等待安全电流失败 bit2:接触器异常 bit3:接触器断开失败)
** @Function  : 获取电池组停机失败标志
** @The notes : 停机失败原因
**===============================================================================================*/
extern u8 TacGetGStopFailFlagAPI(void);

/*=================================================================================================
** @Name      : u8 TacGetGEmergStopFlagAPI(void)
** @Input     : void
** @Output    : 紧急停机标志(按bit位表示 bit0:内部软件急停 bit1:外部命令急停 bit2:按钮急停)
** @Function  : 获取电池组紧急停机标志
** @The notes : 紧急停机原因
**===============================================================================================*/
extern u8 TacGetGEmergStopFlagAPI(void);

/*=================================================================================================
** @Name      : u8 TacGetGSwhCtrlFailFlagAPI(void)
** @Input     : void
** @Output    : 控制接触器闭合失败标志(按bit位表示接触器 bit0:主正断开控制 bit1:预充断开控制 bit2:主负断开控制;bit4:主正闭合控制 bit5:预充闭合控制 bit6:主负闭合控制)
** @Function  : 获取电池组控制接触器闭合失败标志
** @The notes : 高4位控制闭合失败的接触器,低4位控制断开失败的接触器
**===============================================================================================*/
extern u8 TacGetGSwhCtrlFailFlagAPI(void);

/*=================================================================================================
** @Name      : u8 TacGetGSmogAlarmFlagAPI(void)
** @Input     : void
** @Output    : 烟感告警状态标志(0:正常 1:正在核实告警 2:已确认告警)
** @Function  : 获取烟感告警状态标志
** @The notes : 包括电源重启确认真实信号
**===============================================================================================*/
extern u8 TacGetGSmogAlarmFlagAPI(void);

/*=============================================================================================
** @Name      : void TacAuxDevManagTask(void)
** @Input     : void
** @Output    : void
** @Function  : 辅助板管理任务
** @The notes : 500ms驱动1s周期
**===========================================================================================*/
extern void TacAuxDevManagTask(void);

/*使能IMEU2通信交互*/
#if(1 == BMU_IMEU_COM_EN)
/*=============================================================================================
** @Name      : void TacSetAuxDevAirTypeAPI(u8 type)
** @Input     : flag:空调类型 0:黑盾 1:同飞
** @Output    : void
** @Function  : 设置空调类型
** @The notes :
**===========================================================================================*/
extern void TacSetAuxDevAirTypeAPI(u8 type);

/*=================================================================================================
** @Name      : void TacRcvImeuMsgAddAPI(u8 addr)
** @Input     : addr:设备地址
** @Output    : void
** @Function  : 接收IMEU2通信信息地址数目计数
** @The notes : 接收到IMEU2报文时调用
**===============================================================================================*/
extern void TacRcvImeuMsgAddAPI(u8 addr);
#endif

/*=============================================================================================
** @Name      : void TacSetPwmFanUserCmdAPI(u8 id, u8 duty)
** @Input     : id:[0,15]:风扇ID,0xff:所有风扇  duty:控制转速占空比[0,100]1%
** @Output    : void
** @Function  : 设置电池箱调速风扇用户控制命令
** @The notes : 1个电池箱1路风扇(IMLU1控制)
**===========================================================================================*/
extern void TacSetPwmFanUserCmdAPI(u8 id, u8 duty);

/*=============================================================================================
** @Name      : void TacSetAuxShutUserCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 百叶风机用户控制命令(0:关闭 1:开启)
** @Function  : 设置百叶风机用户控制命令
** @The notes : IMEU2控制
**===========================================================================================*/
extern void TacSetAuxShutUserCmdAPI(u8 cmd);

/*=============================================================================================
** @Name      : void TacSetAuxRackLightCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 电池柜照明用户控制命令( 0:自动控制 1:手动开启   2:手动关闭)
** @Function  : 设置电池柜照明用户强制控制命令
** @The notes : IMEU2控制
**===========================================================================================*/
extern void TacSetAuxRackLightCmdAPI(u8 cmd);

/*=============================================================================================
** @Name      : void TacSetAuxZHSAForceCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 声光报警器用户控制命令( 0:自动控制 1:强制开启 2:强制关闭)
** @Function  : 设置声光报警器用户强制控制命令
** @The notes : IMEU2控制
**===========================================================================================*/
extern void TacSetAuxZHSAForceCmdAPI(u8 cmd);

/*=============================================================================================
** @Name      : void TacSetAuxFireTrigCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 气溶胶消防器用户触发命令( 0:自动触发 1:手动触发)
** @Function  : 设置气溶胶消防器用户触发命令
** @The notes : IMEU2控制
**===========================================================================================*/
extern void TacSetAuxFireTrigCmdAPI(u8 cmd);

/*=============================================================================================
** @Name      : u8 TacGetAuxDevAirTypeAPI(void)
** @Input     : void
** @Output    : 电池柜空调类型:0黑盾 1同飞
** @Function  : 获取空调类型
** @The notes : IMEU2控制设备
**===========================================================================================*/
extern u8 TacGetAuxDevAirTypeAPI(void);

/*=============================================================================================
** @Name      : u8 TacGetPwmFanCtrlDutyAPI(u8 id)
** @Input     : id:[0,15]:风扇ID(电池箱序号)
** @Output    : 当前控制转速占空比[0,100]1%
** @Function  : 获取电池箱调速风扇当前控制转速占空比
** @The notes : 1个电池箱1路风扇(IMLU1控制)
**===========================================================================================*/
extern u8 TacGetPwmFanCtrlDutyAPI(u8 id);

/*=================================================================================================
** @Name      : u16 TacGetPwmFanErrFlagAPI(void)
** @Input     : void
** @Output    : 调速风扇回检异常标志(按bit位表示1路电池箱风扇,非风扇通道号)
** @Function  : 获取电池箱调速风扇回检异常标志
** @The notes : 1个电池箱1路风扇,与电池箱连续序号对应(IMLU1控制)
**===============================================================================================*/
extern u16 TacGetPwmFanErrFlagAPI(void);

/*=============================================================================================
** @Name      : u16 TacGetAuxDevErrFlagAPI(void)
** @Input     : void
** @Output    : 辅助设备异常标志(bit0:开启百叶异常 bit1:开启风机异常 bit2:空调告警 bit8:关闭百叶异常 bit9:关闭风机异常 bit10:空调通信中断)
** @Function  : 获取辅助设备异常标志
** @The notes : IMEU2控制设备
**===========================================================================================*/
extern u16 TacGetAuxDevErrFlagAPI(void);

/*=============================================================================================
** @Name      : u8 TacGetAuxComErrFlagAPI(void)
** @Input     : void
** @Output    : 辅助设备通信异常标志 bit0:空调 bit1:温湿度传感器1 bit2:温湿度传感器1 bit3:温湿度传感器 bit4:气体传感器1 bit5:气体传感器2 bit6:气体传感器3
** @Function  : 获取辅助设备通讯异常标志
** @The notes : IMEU2控制设备
**===========================================================================================*/
extern u8 TacGetAuxComErrFlagAPI(void);

/*=================================================================================================
** @Name      : void TacSetGroupWorkStateAPI(e_WorkState state)
** @Input     : state:电池组工作状态[0,6]
** @Output    : void
** @Function  : 设置电池组工作状态
** @The notes :
**===============================================================================================*/
extern void TacSetGroupWorkStateAPI(e_WorkState state);

/*=================================================================================================
** @Name      : void TacResetGroupBattStateAPI(void)
** @Input     : void
** @Output    : 电池组电池状态: 1充满 2放空
** @Function  : 复位电池组电池状态
** @The notes : 电池电压状态清零(0正常)
**===============================================================================================*/
extern void TacResetGroupBattStateAPI(void);

/*=================================================================================================
** @Name      : u8 TacGetGroupBattStateAPI(void)
** @Input     : void
** @Output    : 电池组电池状态:0正常 1充满 2放空
** @Function  : 获取电池组电池状态
** @The notes : 电压状态
**===============================================================================================*/
extern u8 TacGetGroupBattStateAPI(void);

/*=================================================================================================
** @Name      : void GroupModeInfoInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组模式信息初始化
** @The notes : 读EEPROM写入值
**===============================================================================================*/
extern void GroupModeInfoInit(void);

/*=================================================================================================
** @Name      : void GroupWorkModeTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组工作模式检测任务
** @The notes : 1s周期调用
**===============================================================================================*/
extern void GroupWorkModeTask(void);

/*=================================================================================================
** @Name      : void TacSetGroupWorkModeAPI(e_WorkMode mode)
** @Input     : mode:电池组工作模式[0,,5]
** @Output    : void
** @Function  : 设置电池组工作模式
** @The notes : 非正常模式只能退回正常模式,非正常模式之间不能跳转
**===============================================================================================*/
extern void TacSetGroupWorkModeAPI(e_WorkMode mode);

/*=================================================================================================
** @Name      : void GroupProtectInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组保护策略初始化
** @The notes :
**===============================================================================================*/
extern void GroupProtectInit(void);

/*=================================================================================================
** @Name      : void GroupProtectTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组保护策略执行任务
** @The notes : 100ms周期调用
**===============================================================================================*/
extern void GroupProtectTask(void);

/*=================================================================================================
** @Name      : void TacClrGroupProFlagAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除电池组故障保护标志
** @The notes :
**===============================================================================================*/
extern void TacClrGroupProFlagAPI(void);

/*=================================================================================================
** @Name      : u8 TacGetGAskChgProFlagAPI(void)
** @Input     : void
** @Output    : 电池组请求充电保护标志 0:未请求 1:请求
** @Function  : 获取电池组请求充电保护标志
** @The notes :
**===============================================================================================*/
extern u8 TacGetGAskChgProFlagAPI(void);

/*=================================================================================================
** @Name      : u8 TacGetGAskDhgProFlagAPI(void)
** @Input     : void
** @Output    : 电池组请求放电保护标志 0:未请求 1:请求
** @Function  : 获取电池组请求放电保护标志
** @The notes :
**===============================================================================================*/
extern u8 TacGetGAskDhgProFlagAPI(void);

/*=================================================================================================
** @Name      : u8 TacGetGAskSysProFlagAPI(void)
** @Input     : void
** @Output    : 电池组请求充放电保护标志 0:未请求 1:请求
** @Function  : 获取电池组请求充放电保护标志
** @The notes : 同时需要充放电保护
**===============================================================================================*/
extern u8 TacGetGAskSysProFlagAPI(void);

/*=================================================================================================
** @Name      : u16 TacGetGHappenProFlagAPI(void)
** @Input     : void
** @Output    : 电池组产生充放电保护标志,按位表示:bit0:充电截止保护 bit1:放电截止保护 bit2:充电故障保护 bit3:放电故障保护 bit4:充放电故障保护 bit5:系统故障保护 (0:正常 1:产生保护)
** @Function  : 获取电池组产生充放电保护标志
** @The notes :
**===============================================================================================*/
extern u16 TacGetGHappenProFlagAPI(void);

/*=================================================================================================
** @Name      : void TacSetGWorkStateCmdAPI(u8 cmd)
** @Input     : cmd:电池组工作状态切换控制命令[0,8]:0:正常停止 1:正常预充 2:正常启动 3:充电启动 4:放电启动 5:强制停机 6:紧急停机 7:下电关机 8:上电开机
** @Output    : void
** @Function  : 设置电池组工作状态切换控制命令
** @The notes : 接收到通信命令或按钮命令
**===============================================================================================*/
extern void TacSetGWorkStateCmdAPI(u8 cmd);

/*=================================================================================================
** @Name      : void TacGHeatManagTacticTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组热管理策略任务
** @The notes : 1s周期调用,控制风扇和加热片
**===============================================================================================*/
extern void TacGHeatManagTacticTask(void);

/*=================================================================================================
** @Name      : void TacGPeriManagTacticTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组外围管控策略任务
** @The notes : 1s周期调用,控制显示灯、输出干接点和ACC激活保持信号
**===============================================================================================*/
extern void TacGPeriManagTacticTask(void);

/*=============================================================================================
** @Name      : u8 GetGroupSosDiagLevel(void)
** @Input     : void
** @Output    : void
** @Function  : 获取电池组总安全等级SOS
** @The notes : 包括当前等级和推断等级
**===========================================================================================*/
extern u8 GetGroupSosDiagLevel(void);

/*=================================================================================================
** @Name      : e_ErrSort TacGetGNowErrLevelHook(u8 type)
** @Input     : type:故障类别 0:全部故障 1:充电类故障 2:放电类故障
** @Output    : 电池组当前故障级别:0无故障 1仅提示 2充电限制 3放电限制 4充放电限制 5充电保护 6放电保护 7充放电保护 8系统故障保护
** @Function  : 获取电池组当前故障级别
** @The notes :
**===============================================================================================*/
e_ErrSort TacGetGNowErrLevelHook(u8 type);

/*=================================================================================================
** @Name      : u8 TacGetGAllSlaveReadyHook(void)
** @Input     : void
** @Output    : 电池组所有从机就绪状态 TRUE:就绪 FALSE:未就绪
** @Function  : 获取电池组所有从机就绪状态
** @The notes : 包括从控通信状态和采样状态
**===============================================================================================*/
u8 TacGetGAllSlaveReadyHook(void);

/*=================================================================================================
** @Name      : u8 TacGetGAllSubdevReadyHook(void)
** @Input     : void
** @Output    : 电池组所有子系统设备就绪状态 TRUE:就绪 FALSE:未就绪
** @Function  : 获取电池组所有子系统设备就绪状态
** @The notes : 包括IMLU1和IMEU2
**===============================================================================================*/
u8 TacGetGAllSubdevReadyHook(void);

/*=================================================================================================
** @Name      : u8 TacGetGAllPeriphReadyHook(void)
** @Input     : void
** @Output    : 电池组所有外设就绪状态 TRUE:就绪 FALSE:未就绪
** @Function  : 获取电池组所有外围设备就绪状态
** @The notes : 包括电流传感器和接触器、绝缘等
**===============================================================================================*/
u8 TacGetGAllPeriphReadyHook(void);

/*=================================================================================================
** @Name      : u8 TacGetGNegCirReadyHook(void)
** @Input     : void
** @Output    : 电池组所有从机就绪状态 TRUE:就绪 FALSE:未就绪
** @Function  : 获取电池组府机电路就绪状态
** @The notes : GND接外侧采预充电压正常
**===============================================================================================*/
u8 TacGetGNegCirReadyHook(void);

/*=================================================================================================
** @Name      : u8 TacGetGPreChgFinStateHook(void)
** @Input     : void
** @Output    : 电池组当前直流侧预充状态 0:未预充 1:预充成功 2:预充失败
** @Function  : 获取电池组直流侧预充状态
** @The notes :
**===============================================================================================*/
u8 TacGetGPreChgFinStateHook(void);

/*=================================================================================================
** @Name      : u8 TacCheckCurrSafeStateHook(void)
** @Input     : void
** @Output    : 电池组当前电流安全状态 0:不安全 1:安全
** @Function  : 检测电池组当前电流安全状态
** @The notes :
**===============================================================================================*/
u8 TacCheckCurrSafeStateHook(void);

/*=================================================================================================
** @Name      : u8 TacCheckCurrCutStateHook(void)
** @Input     : void
** @Output    : 电池组当前电流切断状态 0:未切断 1:切断
** @Function  : 检测电池组当前电流切断状态
** @The notes :
**===============================================================================================*/
u8 TacCheckCurrCutStateHook(void);

/*=================================================================================================
** @Name      : u8 TacGetUserReqStopStateHook(void)
** @Input     : void
** @Output    : 请求电池组停机状态 0:未请求 1:请求
** @Function  : 获取用户请求电池组停机状态
** @The notes : 运行状态需要停机
**===============================================================================================*/
u8 TacGetUserReqStopStateHook(void);

/*=================================================================================================
** @Name      : u8 TacGetGroupFitStartStateHook(void)
** @Input     : void
** @Output    : 电池组适合启动状态 0:不适合 1:适合
** @Function  : 获取电池组适合启动状态
** @The notes : 空闲状态允许启动
**===============================================================================================*/
u8 TacGetGroupFitStartStateHook(void);

/*=================================================================================================
** @Name      : u8 TacGetGroupACCPwrOffReqStateHook(void)
** @Input     : void
** @Output    : 请求状态:0:上电保持 1:下电等待 2:执行下电
** @Function  : 获取电池组ACC低压下电请求状态
** @The notes : ACC和快充均掉电时需要关闭SBC;""即下电前处理 "执行下电"即关闭
**===============================================================================================*/
u8 TacGetGroupACCPwrOffReqStateHook(void);

/*=================================================================================================
** @Name      : u8 TacGetUserSmogSignStateHook(void)
** @Input     : void
** @Output    : 电池组烟感报警状态 0:未报警 1:报警
** @Function  : 获取用户请求电池组烟感报警状态
** @The notes :
**===============================================================================================*/
u8 TacGetUserSmogSignStateHook(void);

/*=================================================================================================
** @Name      : u8 TacGetUserButtonEmergStateHook(void)
** @Input     : void
** @Output    : 电池组按钮急停状态 0:未请求 1:请求
** @Function  : 获取用户请求电池组按钮急停状态
** @The notes :
**===============================================================================================*/
u8 TacGetUserButtonEmergStateHook(void);

/*=================================================================================================
** @Name      : u8 TacGetGroupExtCommuErrStateHook(void)
** @Input     : void
** @Output    : 电池组当前外部通信异常状态 0:正常 1:异常
** @Function  : 获取电池组当前外部通信异常状态
** @The notes :
**===============================================================================================*/
u8 TacGetGroupExtCommuErrStateHook(void);

/*=================================================================================================
** @Name      : u8 TacGetGroupBattVHStateHook(void)
** @Input     : void
** @Output    : 电池组当前高压状态 0:正常 1:高压
** @Function  : 获取电池组高压状态
** @The notes :
**===============================================================================================*/
u8 TacGetGroupBattVHStateHook(void);

/*=================================================================================================
** @Name      : u8 TacGetGroupBattVLStateHook(void)
** @Input     : void
** @Output    : 电池组当前低压状态 0:正常 1:低压
** @Function  : 获取电池组低压状态
** @The notes :
**===============================================================================================*/
u8 TacGetGroupBattVLStateHook(void);

/*=================================================================================================
** @Name      : u16 TacGetGroupChgCapHook(void)
** @Input     : void
** @Output    : 电池组充电电量 1AH
** @Function  : 获取电池组充电电量
** @The notes :
**===============================================================================================*/
u16 TacGetGroupChgCapHook(void);

/*=================================================================================================
** @Name      : u16 TacGetGroupDhgCapHook(void)
** @Input     : void
** @Output    : 电池组放电电量 1AH
** @Function  : 获取电池组放电电量
** @The notes :
**===============================================================================================*/
u16 TacGetGroupDhgCapHook(void);

/*=================================================================================================
** @Name      : e_SysStateResult TacGroupInitStateResultHook(void)
** @Input     : void
** @Output    : 检测结果:0:等待 1:完成 2:异常
** @Function  : 电池组初始化状态检测结果
** @The notes :
**===============================================================================================*/
e_SysStateResult TacGroupInitStateResultHook(void);

/*=================================================================================================
** @Name      : void TacGroupProStateChangDealHook(u8 befState, u16 aftState)
** @Input     : befState:前置状态 aftState:后置状态
** @Output    : void
** @Function  : 电池组保护状态变换处理函数
** @The notes :
**===============================================================================================*/
void TacGroupProStateChangDealHook(u8 befState, u16 aftState);

/*=================================================================================================
** @Name      : void TacGroupWorkStateChangDealHook(u8 befState, u16 aftState)
** @Input     : befState:前置状态 aftState:后置状态
** @Output    : void
** @Function  : 电池组工作状态变换处理函数
** @The notes :
**===============================================================================================*/
void TacGroupWorkStateChangDealHook(u8 befState, u16 aftState);

/*=================================================================================================
** @Name      : void TacGroupChgStateChangDealHook(u8 befState, u16 aftState)
** @Input     : befState:前置状态 aftState:后置状态
** @Output    : void
** @Function  : 电池组充放电状态变换处理函数
** @The notes :
**===============================================================================================*/
void TacGroupChgStateChangDealHook(u8 befState, u16 aftState);

/*=================================================================================================
** @Name      : void TacGroupWorkModeChangDealHook(u8 befMode, u8 aftMode)
** @Input     : befState:前置状态 aftState:后置状态
** @Output    : void
** @Function  : 电池组工作模式变换处理函数
** @The notes :
**===============================================================================================*/
void TacGroupWorkModeChangDealHook(u8 befMode, u8 aftMode);

/*=================================================================================================
** @Name      : void TacGroupDebugSuccessDealHook(u8 type, u8 dev, u8 id)
** @Input     : type:调试类型 dev:调试设备 id:调试通道ID
** @Output    : void
** @Function  : 电池组调试成功处理函数
** @The notes :
**===============================================================================================*/
void TacGroupDebugSuccessDealHook(u8 type, u8 dev, u8 id);

/*=================================================================================================
** @Name      : void TacGroupDebugSlaveBalanHook(u8 ecu, u32 cell, u8 type, u8 state)
** @Input     : ecu:从控节点号(0:所有从控) cell:需均衡电池(按位表示) type:均衡方式(0禁止均衡 1主动均衡 2被动均衡) state:均衡状态(0关闭 1充电 2放电)
** @Output    : void
** @Function  : 电池组调试从控均衡状态处理函数
** @The notes :
**===============================================================================================*/
void TacGroupDebugSlaveBalanHook(u8 ecu, u32 cell, u8 type, u8 state);

/*=================================================================================================
** @Name      : void TacGroupDebugSlaveSwitchHook(u8 ecu, u16 id, u8 state)
** @Input     : ecu:从控节点号(0:所有从控) id:接触器号(0:所有接触器) state:控制状态(0断开 1闭合)
** @Output    : void
** @Function  : 电池组调试从控接触器状态处理函数
** @The notes :
**===============================================================================================*/
void TacGroupDebugSlaveSwitchHook(u8 ecu, u16 id, u8 state);

/*=================================================================================================
** @Name      : void TacGroupDebugSlaveOutDryHook(u8 ecu, u16 id, u8 state)
** @Input     : ecu:从控节点号(0:所有从控) id:接触器号(0:所有接触器) state:控制状态(0断开 1闭合)
** @Output    : void
** @Function  : 电池组调试从控输出干接点状态处理函数
** @The notes :
**===============================================================================================*/
void TacGroupDebugSlaveOutDryHook(u8 ecu, u16 id, u8 state);

/*=================================================================================================
** @Name      : void TacGroupPreChgFinDealHook(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组预充完成处理函数
** @The notes :
**===============================================================================================*/
void TacGroupPreChgFinDealHook(void);

#endif
