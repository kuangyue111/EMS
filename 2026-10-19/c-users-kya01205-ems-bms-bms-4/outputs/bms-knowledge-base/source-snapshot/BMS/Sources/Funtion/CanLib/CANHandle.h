/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CANHandle.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : CAN通信包规约处理接口
** @Instructions :
**===============================================================================================*/
#ifndef _CANHANDLE_H
#define _CANHANDLE_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "CANInterface.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
#define CAN_MAX_CAN_NUM                 4                       /*最大CAN通道数目(不可修改)*/
#define CAN_TX_UPM_SPA_T                100                     /*发送周期更新长帧帧间间隔时间(1ms)*/
#define CAN_TX_ARQ_SPA_T                10                      /*发送查询回复帧帧间间隔时间(1ms)*/

#define CAN_TX_UPC_SPA_T                50                      /*发送周期更新单体帧间间隔时间(1ms)*/
#define CAN_TX_UPP_SPA_T                300                     /*发送周期更新参数帧间间隔时间(1ms)*/

#define CAN_TX_CRQ_OVR_T                100                     /*等待查询确认帧超时时间(1ms)*/
#define CAN_TX_REQ_OVR_T                500                     /*等待请求查询回复帧超时时间(1ms)*/
#define CAN_TX_RUP_OVR_T                500                     /*等待请求修改回复帧超时时间(1ms)*/
#define CAN_TX_RET_OVR_T                1                       /*无回复重复次数(1次)*/

#define CAN_MAX_COUNT_TIME              0xefffffff              /*CAN通信包计时最大时间数(1ms)*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void CANDevTabEntryInit_API(u8 devAddr, u8 mIndexNum const s_CANIndexTab  *pIndexTab, u8 devId)
** @Input     : devAdd:设备地址 mIndexNum:主索引数目 pIndexTab:设备主动引表 devId:创建设备表序号
** @Output    : void
** @Function  : CAN通信创建设备表初始化
** @The notes : 用户配置CAN通信设备表信息
**===============================================================================================*/
void CANDevTabEntryInit_API(u8 devAddr, u8 mIndexNum, const t_CANIndexTab *pIndexTab, u8 devId);

/*=================================================================================================
** @Name      : u8 CANLibRcvMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : CAN通信报文接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
u8 CANLibRcvMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum);

/*=================================================================================================
** @Name      : void CANLibHandleTask_API(void)
** @Input     : void
** @Output    : void
** @Function  : CAN所有处理任务
** @The notes : 用户必须5ms周期调用
**===============================================================================================*/
void CANLibHandleTask_API(void);

/*=================================================================================================
** @Name      : void CANLibTxRcvHandleTask_API(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信报文收发处理任务(收发通信表处理)
** @The notes : 用户必须1ms周期调用
**===============================================================================================*/
void CANLibTxRcvHandleTask_API(void);

/*=================================================================================================
** @Name      : u8 CANBuildSendVerMsg_API(u8 canNum)
** @Input     : canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送校时报文帧
** @The notes : 用户自行发送校时帧
**===============================================================================================*/
u8 CANBuildSendVerMsg_API(u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendBreMsg_API(u8 canNum)
** @Input     : canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送心跳报文帧
** @The notes : 用户自行发送心跳帧
**===============================================================================================*/
u8 CANBuildSendBreMsg_API(u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendRupMsg_API(u8 destAddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum)
** @Input     : destAddr:目标设备地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 conFlag:是否需要确认 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送请求修改报文帧
** @The notes : 用户自行发送请求修改报文帧
**===============================================================================================*/
u8 CANBuildSendRupMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendAupMsg_API(t_CANFram *canFram, u8 canNum)
** @Input     : *canFram:接收到的CAN通信帧 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送修改回复报文帧
** @The notes : 用户自行发送修改回复报文帧
**===============================================================================================*/
u8 CANBuildSendAupMsg_API(t_CANFram *canFram, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendCupMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : destAddr:目标设备地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 conFlag:是否需要确认 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送修改确认报文帧
** @The notes : 用户自行发送修改确认报文帧
**===============================================================================================*/
u8 CANBuildSendCupMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendReqMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 conFlag:是否需要确认 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送请求查询报文帧
** @The notes : 用户自行发送请求查询报文帧
**===============================================================================================*/
u8 CANBuildSendReqMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendArqMsg_API(t_CANFram *canFram, u8 canNum)
** @Input     : *canFram:接收到的CAN通信帧 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送查询回复报文帧
** @The notes : 用户自行发送查询回复报文帧
**===============================================================================================*/
u8 CANBuildSendArqMsg_API(t_CANFram *canFram, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendCrqMsg_API(t_CANFram *canFram, u8 canNum)
** @Input     : *canFram:接收到的CAN通信帧 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送查询确认报文帧
** @The notes : 用户自行发送查询确认报文帧
**===============================================================================================*/
u8 CANBuildSendCrqMsg_API(t_CANFram *canFram, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendUpmMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目  canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新长帧报文帧
** @The notes : 用户自行发送主动更新长帧报文帧
**===============================================================================================*/
u8 CANBuildSendUpmMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendUpsMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新单帧报文帧
** @The notes : 用户自行发送主动更新单帧报文帧
**===============================================================================================*/
u8 CANBuildSendUpsMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendUpcMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 mapIndex:映射起始主索引号 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新单体报文帧
** @The notes : 用户自行发送主动更新单体报文帧
**===============================================================================================*/
u8 CANBuildSendUpcMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendUppMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 mapIndex:映射起始主索引号 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新参数报文帧
** @The notes : 用户自行发送主动更新参数报文帧
**===============================================================================================*/
u8 CANBuildSendUppMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendErrMsg_API(u8 errReas, t_CANFram *canFram, u8 canNum)
** @Input     : errReas:错误原因 *canFram:接收到的CAN通信帧 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送错误应答报文帧
** @The notes : 请求查询和请求修改命令错误时应答使用
**===============================================================================================*/
u8 CANBuildSendErrMsg_API(u8 errReas, t_CANFram *canFram, u8 canNum);

/*=================================================================================================
** @Name      : u32 CANBuildMsgID_API(e_CANFunCode funCode, u8 mIndex, u8 destAddr, u8 attrib)
** @Input     : funCode:功能码 mIndex:主索引号 destAddr:目标地址 attrib:帧属性
** @Output    : 32位CAN通信帧ID值
** @Function  : 组建32位CAN通信帧ID值
** @The notes :
**===============================================================================================*/
u32 CANBuildMsgID_API(e_CANFunCode funCode, u8 mIndex, u8 destAddr, u8 attrib);

#endif
