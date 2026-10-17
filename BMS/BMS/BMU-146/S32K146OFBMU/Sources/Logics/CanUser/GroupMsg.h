/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : GroupMsg.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池组主控CAN报文处理模块
** @Instructions : 
**===============================================================================================*/
#ifndef _GEOUPMSG_H
#define _GROUPMSG_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "CANUser.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 RcvCanDeviceSerialCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收设备序列号配置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(2)处理调用
**===============================================================================================*/
u8 RcvCanDeviceSerialCfgDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanDnloadParaWRDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收电池组参数批量配置写数据CAN报文处理
** @The notes : CAN通信包接收请求修改(22)处理调用
**===============================================================================================*/
u8 RcvCanDnloadParaWRDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanErrRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收查询故障记录详细信息读取CAN报文处理
** @The notes : CAN通信包接收请求查询(31)处理调用
**===============================================================================================*/
u8 RcvCanErrRcdReadInfoDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanEventRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收查询事件记录详细信息读取CAN报文处理
** @The notes : CAN通信包接收请求查询(32)处理调用
**===============================================================================================*/
u8 RcvCanEventRcdReadInfoDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanWaveRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收查询故障录波详细信息读取CAN报文处理
** @The notes : CAN通信包接收请求查询(33)处理调用
**===============================================================================================*/
u8 RcvCanWaveRcdReadInfoDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanDodRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收查询DOD记录详细信息读取CAN报文处理
** @The notes : CAN通信包接收请求查询(34)处理调用
**===============================================================================================*/
u8 RcvCanDodRcdReadInfoDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanDataRcdDisplayInfoDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收查询数据记录详细信息显示CAN报文处理
** @The notes : CAN通信包接收请求查询(37)处理调用
**===============================================================================================*/
u8 RcvCanDataRcdDisplayInfoDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanHisRecordRWDateDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收历史记录读写日期处理
** @The notes : CAN通信包接收请求修改(39)处理调用
**===============================================================================================*/
u8 RcvCanHisRecordRWDateDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanGroupPasswordDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收电池组权限密码命令CAN报文处理
** @The notes : CAN通信包接收请求修改(85)处理调用
**===============================================================================================*/
u8 RcvCanGroupPasswordDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanGroupCtrlCmdDeal(u8 saddr, u8 index, u8 len, u8 canNum)
** @Input     : saddr:命令源地址 index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收电池组控制命令CAN报文处理
** @The notes : CAN通信包接收请求修改(86)处理调用
**===============================================================================================*/
u8 RcvCanGroupCtrlCmdDeal(u8 saddr, u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanUserCtrlCmdDeal(u8 index, u8 len)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收用户控制命令CAN报文处理
** @The notes : CAN通信包接收请求修改(87)处理调用
**===============================================================================================*/
u8 RcvCanUserCtrlCmdDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanGroupDebugCmdDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收电池组调试命令CAN报文处理
** @The notes : CAN通信包接收请求修改(88)处理调用
**===============================================================================================*/
u8 RcvCanGroupDebugCmdDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanSystemGenParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收系统通用参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(105)处理调用
**===============================================================================================*/
u8 RcvCanSystemGenParaCfgDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanSystemHWParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收系统硬件参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(106)处理调用
**===============================================================================================*/
u8 RcvCanSystemHWParaCfgDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanGroupGenParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收主控通用参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(107)处理调用
**===============================================================================================*/
u8 RcvCanGroupGenParaCfgDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanGroupHigParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收主控高级参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(108)处理调用
**===============================================================================================*/
u8 RcvCanGroupHigParaCfgDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : u8 RcvCanGroupHWParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收主控硬件参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(109)处理调用
**===============================================================================================*/
u8 RcvCanGroupHWParaCfgDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : void GroupSendUpMsg1sBy50ms_API(void)
** @Input     : void
** @Output    : void
** @Function  : 主控上传1s周期CAN通信报文任务
** @The notes : 用户50ms周期接口调用
**===============================================================================================*/
void GroupSendUpMsg1sBy50ms_API(void);

/*=================================================================================================
** @Name      : void SendCanGroupSystemStateInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送系统整体状态信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupSystemStateInfoMsg(void);

/*=================================================================================================
** @Name      : void SendCanGroupPortStateInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送输入输出端口状态信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupPortStateInfoMsg(void);

/*=================================================================================================
** @Name      : void SendCanGroupAuthorityInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送当前权限状态信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupAuthorityInfoMsg(void);

/*=================================================================================================
** @Name      : void SendCanGroupHisErrInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送历史故障信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupHisErrInfoMsg(void);

/*=================================================================================================
** @Name      : void SendCanGroupHisEventInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送历史事件信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupHisEventInfoMsg(void);

/*=================================================================================================
** @Name      : void SendCanGroupHisWaveInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送历史故障录波信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupHisWaveInfoMsg(void);

/*=================================================================================================
** @Name      : void SendCanGroupHisDodInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送历史DOD记录信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupHisDodInfoMsg(void);

/*使能自研协议,禁止CATL协议时CAN#1发送单体信息(非CAN#1监控)*/
#if((1 == BMU_SELF_CAN_EN) && (0 == BMU_CATL_CAN_EN) && (eCAN0 != CAN_UPDATE_PORT))
/*=================================================================================================
** @Name      : void SendCanGroupCellVoltInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送单体电压信息CAN报文
** @The notes : 用户需要发送时调用
**===============================================================================================*/
void SendCanGroupCellVoltInfoMsg(void);

/*=================================================================================================
** @Name      : void SendCanGroupCellTempInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送单体温度和均衡温度信息CAN报文
** @The notes : 用户需要发送时调用
**===============================================================================================*/
void SendCanGroupCellTempInfoMsg(void);

/*=================================================================================================
** @Name      : void SendCanGroupCellBalanInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送单体均衡状态信息CAN报文
** @The notes : 用户需要发送时调用
**===============================================================================================*/
void SendCanGroupCellBalanInfoMsg(void);

//使能自研从控BSU CAN通信信息才测内阻
#if((1 == BMU_BSU_COM_TYPE) && (0 == BMU_BSU_CAN_TYPE))
/*=================================================================================================
** @Name      : void SendCanGroupCellResisInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送单体内阻信息CAN报文
** @The notes : 用户需要发送时调用
**===============================================================================================*/
void SendCanGroupCellResisInfoMsg(void);
#endif
#endif

#endif
