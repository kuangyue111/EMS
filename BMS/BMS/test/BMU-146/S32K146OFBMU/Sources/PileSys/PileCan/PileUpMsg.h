/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileUpMsg.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控与上层设备通信CAN报文处理模块
** @Instructions : 
**===============================================================================================*/
#ifndef _PILEUPMSG_H
#define _PILEUPMSG_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicInfo.h"

#if(1 == PILE_MANAG_EN)
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
** @Name      : void PileRcvUpMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 堆控上层(PCS)CAN通信报文接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
void PileRcvUpMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum);

/*=================================================================================================
** @Name      : void PileSendUpMsg500By50ms_API(void)
** @Input     : void
** @Output    : void
** @Function  : 堆控上传1s周期CAN通信报文任务
** @The notes : 用户50ms周期接口调用
**===============================================================================================*/
void PileSendUpMsg1sBy50ms_API(void);

/*=================================================================================================
** @Name      : void PileRcvCanReqUpdateCmdDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:主索引号 *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 接收请求修改命令CAN报文处理
** @The notes : CAN通信包接收请求修改处理调用
**===============================================================================================*/
void PileRcvCanReqUpdateCmdDeal(u8 index, u8 *pdata, u8 DLC, u8 canNum);

/*=================================================================================================
** @Name      : void RcvCanPilePasswordDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : void
** @Function  : 接收电池堆权限密码命令CAN报文处理
** @The notes : CAN通信包接收请求修改处理调用
**===============================================================================================*/
void RcvCanPilePasswordDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : void RcvCanPileCtrlCmdDeal(u8 index, u8 len)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : void
** @Function  : 接收电池堆控制命令CAN报文处理
** @The notes : CAN通信包接收请求修改处理调用
**===============================================================================================*/
void RcvCanPileCtrlCmdDeal(u8 index, u8 len, u8 canNum);

#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
/*=================================================================================================
** @Name      : void RcvCanPileDebugCmdDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : void
** @Function  : 接收电池堆调试命令CAN报文处理
** @The notes : CAN通信包接收请求修改处理调用
**===============================================================================================*/
void RcvCanPileDebugCmdDeal(u8 index, u8 len, u8 canNum);

/*=================================================================================================
** @Name      : void RcvCanPileGenParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : void
** @Function  : 接收堆控通用参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改处理调用
**===============================================================================================*/
void RcvCanPileGenParaCfgDeal(u8 index, u8 len, u8 canNum);
#endif

/*=================================================================================================
** @Name      : void SendCanPileSystemStateInfoMsg50(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统整体状态信息CAN报文
** @The notes : 主索引50
**===============================================================================================*/
void SendCanPileSystemStateInfoMsg50(void);

/*=================================================================================================
** @Name      : void SendCanPileLimCurrPwrInfoMsg51(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统电流功率限值信息CAN报文
** @The notes : 主索引51
**===============================================================================================*/
void SendCanPileLimCurrPwrInfoMsg51(void);

/*=================================================================================================
** @Name      : void SendCanPileLimSumVoltInfoMsg52(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统总压限值信息CAN报文
** @The notes : 主索引52
**===============================================================================================*/
void SendCanPileLimSumVoltInfoMsg52(void);

/*=================================================================================================
** @Name      : void SendCanPileAlarmInfoMsg53(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统告警信息CAN报文
** @The notes : 主索引53
**===============================================================================================*/
void SendCanPileAlarmInfoMsg53(void);

/*=================================================================================================
** @Name      : void SendCanPileFaultInfoMsg54(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统故障信息CAN报文
** @The notes : 主索引54
**===============================================================================================*/
void SendCanPileFaultInfoMsg54(void);

/*=================================================================================================
** @Name      : void SendCanPileNotesInfoMsg55(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统提醒信息CAN报文
** @The notes : 主索引55
**===============================================================================================*/
void SendCanPileNotesInfoMsg55(void);

/*=================================================================================================
** @Name      : void SendCanPileGroupStateInfoMsg57(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统各电池组状态信息CAN报文
** @The notes : 主索引57
**===============================================================================================*/
void SendCanPileGroupStateInfoMsg57(void);

/*=================================================================================================
** @Name      : void SendCanPileSysMainInfoMsg60(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统主要信息CAN报文
** @The notes : 主索引60
**===============================================================================================*/
void SendCanPileSysMainInfoMsg60(void);

/*=================================================================================================
** @Name      : void SendCanPileSysCapInfoMsg61(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统电能量信息CAN报文
** @The notes : 主索引61
**===============================================================================================*/
void SendCanPileSysCapInfoMsg61(void);

/*=================================================================================================
** @Name      : void SendCanPileVoltCharInfoMsg62(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统电压特征信息1CAN报文
** @The notes : 主索引62
**===============================================================================================*/
void SendCanPileVoltCharInfoMsg62(void);

/*=================================================================================================
** @Name      : void SendCanPileVoltCharInfoMsg63(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统电压特征信息2CAN报文
** @The notes : 主索引63
**===============================================================================================*/
void SendCanPileVoltCharInfoMsg63(void);

/*=================================================================================================
** @Name      : void SendCanPileTempCharInfoMsg64(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统温度特征信息1CAN报文
** @The notes : 主索引64
**===============================================================================================*/
void SendCanPileTempCharInfoMsg64(void);

/*=================================================================================================
** @Name      : void SendCanPileTempCharInfoMsg65(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统温度特征信息2CAN报文
** @The notes : 主索引65
**===============================================================================================*/
void SendCanPileTempCharInfoMsg65(void);

/*=================================================================================================
** @Name      : void SendCanPileSumVCharInfoMsg66(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统总压特征信息CAN报文
** @The notes : 主索引66
**===============================================================================================*/
void SendCanPileSumVCharInfoMsg66(void);

/*=================================================================================================
** @Name      : void SendCanPileCurrCharInfoMsg67(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统电流特征信息CAN报文
** @The notes : 主索引67
**===============================================================================================*/
void SendCanPileCurrCharInfoMsg67(void);

/*=================================================================================================
** @Name      : void SendCanPileStatistTimeInfoMsg70(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统时间统计信息CAN报文
** @The notes : 主索引70
**===============================================================================================*/
void SendCanPileStatistTimeInfoMsg70(void);

#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
/*=================================================================================================
** @Name      : void SendCanPileStatistErrInfoMsg71(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统故障数目统计信息CAN报文
** @The notes : 主索引71
**===============================================================================================*/
void SendCanPileStatistErrInfoMsg71(void);

/*=================================================================================================
** @Name      : void SendCanPileStatistCapInfoMsg72(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统单次电量统计信息CAN报文
** @The notes : 主索引72
**===============================================================================================*/
void SendCanPileStatistCapInfoMsg72(void);

/*=================================================================================================
** @Name      : void SendCanPileStatistHisCapInfoMsg73(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统累计电量统计信息CAN报文
** @The notes : 主索引73
**===============================================================================================*/
void SendCanPileStatistHisCapInfoMsg73(void);

/*=================================================================================================
** @Name      : void SendCanPileStatistHisEnerInfoMsg74(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统累计电能统计信息CAN报文
** @The notes : 主索引74
**===============================================================================================*/
void SendCanPileStatistHisEnerInfoMsg74(void);

/*=================================================================================================
** @Name      : void SendCanPileStatistDodInfoMsg75(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统放电深度统计信息CAN报文
** @The notes : 主索引75
**===============================================================================================*/
void SendCanPileStatistDodInfoMsg75(void);
#endif

/*=================================================================================================
** @Name      : void SendCanPileSysAuthorInfoMsg80(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统权限信息CAN报文
** @The notes : 主索引80
**===============================================================================================*/
void SendCanPileSysAuthorInfoMsg80(void);

#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
/*=================================================================================================
** @Name      : void SendCanPileDebugResultInfoMsg82(void)
** @Input     : void
** @Output    : void
** @Function  : 发送系统调试结果信息CAN报文
** @The notes : 主索引82
**===============================================================================================*/
void SendCanPileDebugResultInfoMsg82(void);

/*=================================================================================================
** @Name      : void PileRcvCanReqReadCmdDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:主索引号 *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 接收请求查询命令CAN报文处理
** @The notes : CAN通信包接收请求查询处理调用
**===============================================================================================*/
void PileRcvCanReqReadCmdDeal(u8 index, u8 *pdata, u8 DLC, u8 canNum);

/*=================================================================================================
** @Name      : void AnswCanPileDevInfoMsg1(void)
** @Input     : void
** @Output    : void
** @Function  : 应答查询堆控设备信息CAN报文
** @The notes : 主索引1
**===============================================================================================*/
void AnswCanPileDevInfoMsg1(u8 index, u8 len);

/*=================================================================================================
** @Name      : void SendCanPileDebugResultInfoMsg82(void)
** @Input     : void
** @Output    : void
** @Function  : 应答查询系统通用参数信息CAN报文
** @The notes : 主索引100
**===============================================================================================*/
void AnswCanPileSysGenParaMsg100(u8 index, u8 len);

/*=================================================================================================
** @Name      : void AnswCanPileSysHWParaMsg101(void)
** @Input     : void
** @Output    : void
** @Function  : 应答查询系统硬件参数信息CAN报文
** @The notes : 主索引101
**===============================================================================================*/
void AnswCanPileSysHWParaMsg101(u8 index, u8 len);

#endif
#endif
#endif
