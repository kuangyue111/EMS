/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CANTabOper.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : CAN通信包表格读写操作接口
** @Instructions :
**===============================================================================================*/
#ifndef _CANTABOPER_H
#define _CANTABOPER_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "CANInterface.h"

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
** @Name      : const t_CANIndexTab *CanScanIndexTabInfo(u8 devaddr, u8 mindex, u8 sIndex, u8 sIndexNum, u8 *errorCode)
** @Input     : devaddr:索引表设备地址 mindex:主索引号  sindex:子索引号  sIndexNum:子索引数目 *errorCode:存储错误码地址
** @Output    : 对应主索引信息
** @Function  : 查找主索引表信息
** @The notes : 并且判断子索引是否越界
**===============================================================================================*/
const t_CANIndexTab *CanScanIndexTabInfo(u8 devaddr, u8 mindex, u8 sIndex, u8 sIndexNum, u8 *errorCode);

/*=================================================================================================
** @Name      : void CANReadCanIndexData(const t_CANIndexTab *ptrTable, u8 subIndex, u8 sIndexNum, u8 *Data)
** @Input     : *ptrTable:主索引信息  sindex:子索引号  sIndexNum:子索引数目 *Data:读出的数据(多字节则低位在前)
** @Output    : void
** @Function  : 读索引表中对应子索引当前数据值
** @The notes : 必须确保主索引存在且子索引长度合法
**===============================================================================================*/
void CANReadCanIndexData(const t_CANIndexTab *ptrTable, u8 subIndex, u8 sIndexNum, u8 *Data);

/*=================================================================================================
** @Name      : u8 CANWriteCanIndexData(const t_CANIndexTab *ptrTable, u8 subIndex, u8 sIndexNum, u8 *Data, u8 cannum)
** @Input     : *ptrTable:主索引信息  sindex:子索引号  sIndexNum:子索引数目 *Data:需写入的数据(多字节则低位在前) cannum:CAN通道号
** @Output    : 写结果(0:成功 其它:错误码)
** @Function  : 向索引表中对应子索引写入数据值
** @The notes : 必须确保主索引存在且子索引长度合法
**===============================================================================================*/
u8 CANWriteCanIndexData(const t_CANIndexTab *ptrTable, u8 subIndex, u8 sIndexNum, u8 *Data, u8 cannum);

/*=================================================================================================
** @Name      : u8 CANReqIdleTxRcvTab(e_CANFunCode funcode, u8 attrib, void *pdata, u8 dataType, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conflag, u8 daddr, u8 cannum)
** @Input     : funcode:功能码 attrib:帧属性 pdata:数据首地址 dataType:数据类型 mIndex:主索引  sindex:子索引 sindexnum:子索引数目 conflag:确认标志[修改/查询]或映射起始主索引[单体/参数]  daddr:目的地址, cannum:CAN通道
** @Output    : 申请结果(0:成功 其它:错误码)
** @Function  : 请求空闲的收发表记录发送信息
** @The notes : 设置定时发送
**===============================================================================================*/
u8 CANReqIdleTxRcvTab(e_CANFunCode funcode, u8 attrib, void *pdata, u8 dataType, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conflag, u8 daddr, u8 cannum);

/*=================================================================================================
** @Name      : u8 CANSendTxRcvTabMsgPack(u8 id)
** @Input     : id:CAN收发通信表序号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 发送CAN收发通信表中需要发送的数据包
** @The notes : CAN收发通信表有报文需要发送时调用
**===============================================================================================*/
u8 CANSendTxRcvTabMsgPack(u8 id);

/*=================================================================================================
** @Name      : void CANClrTxRcvTabInfo(u8 id)
** @Input     : id:CAN收发通信表序号
** @Output    : void
** @Function  : 清除CAN收发通信表中的记录
** @The notes :
**===============================================================================================*/
void CANClrTxRcvTabInfo(u8 id);

/*=================================================================================================
** @Name      : void CANClrTxRcvTabTimer(u8 id)
** @Input     : id:CAN收发通信表序号
** @Output    : void
** @Function  : 清除CAN收发通信表中的计时
** @The notes : 两帧交互正常计时结束
**===============================================================================================*/
void CANClrTxRcvTabTimer(u8 id);

/*=================================================================================================
** @Name      : void CANMulFramTxTimerCheck(u8 id, u16 timer)
** @Input     : id:通信表序号, timer:发送帧间隔时间(1ms)
** @Output    : void
** @Function  : 多帧发送帧间间隔计时检测(长帧/单帧/单体/参数)
** @The notes : 通讯表状态为等待发送长帧时1ms定时调用
**===============================================================================================*/
void CANMulFramTxTimerCheck(u8 id, u16 timer);

/*=================================================================================================
** @Name      : void CANWaitAnsFramTimerCheck(u8 id, u16 timer, u8 times)
** @Input     : id:通信表序号, timer:发送帧间隔时间(1ms) times:重发次数(1次)
** @Output    : void
** @Function  : 等待回复帧间间隔计时检测
** @The notes : 通讯表状态为等待接收回复时1ms定时调用
**===============================================================================================*/
void CANWaitAnsFramTimerCheck(u8 id, u16 timer, u8 times);

/*=================================================================================================
** @Name      : void CANReadTxRcvTabData(u8 id, u8 readlen, u8 *pdata)
** @Input     : id:CAN收发通信表序号 readlen:读数据长度 *pdata:读到的数据
** @Output    : void
** @Function  : 读取CAN收发通信表中缓存区的数据
** @The notes : 确保读取长度必须有效
**===============================================================================================*/
void CANReadTxRcvTabData(u8 id, u8 readlen, u8 *pdata);

/*=================================================================================================
** @Name      : u8 CANFindTxRcvTabMatching(u8 destAddr, e_CANFunCode funcode, u8 mindex, u8 sindex)
** @Input     : destAddr:源地址 funcode:功能码 mindex:主索引 sindex:子索引
** @Output    : void
** @Function  : 查找CAN收发通信表中与输入参数匹配的序号
** @The notes : 查找是否正处于收发状态
**===============================================================================================*/
u8 CANFindTxRcvTabMatching(u8 destAddr, e_CANFunCode funcode, u8 mindex, u8 sindex);

/*=================================================================================================
** @Name      : u8 CANComputeRequestCRC8(u16 ApplicationId, const u8* Crc_DataPtr, u16 Crc_Length)
** @Input     : ApplicationId:帧ID低16位 Crc_DataPtr:需要计算校验码的数组 Crc_Length:数组长度
** @Output    : void
** @Function  : 按SAE J1850 方式计算CRC值
** @The notes : 请求修改命令帧长度不满8byte但发完8byte时使用
**===============================================================================================*/
u8 CANComputeRequestCRC8(u16 ApplicationId, const u8* Crc_DataPtr, u16 Crc_Length);

#endif
