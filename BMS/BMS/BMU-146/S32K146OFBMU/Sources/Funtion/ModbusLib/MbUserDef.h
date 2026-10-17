/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : MbUserDef.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : Modbus通信包用户补充处理接口函数
** @Instructions :
**===============================================================================================*/
#ifndef _MBUSERDEF_H
#define _MBUSERDEF_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "MbInterface.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
//参数配置
#define MB_FAR_EN                       FALSE         //far空间功能使能配置 FALSE：不使能 TRUE：使能  只有ARM平台不使能
#define MODESELECT                      2             //配置软件主从功能，0为只作从端，1只作主端，2 既有主站也有从站
#define MB_POLL_TIMEOUT                 1000          //超时临界值
#define MB_POLL_DELAY                   10            //帧间隔

//相关空间大小定义，用户可配置
#define MBSTATIONNUM                    3             //配置Modbus站点数目
#define UNITNUM                         20            //队列单元数目，最小为1
#define UNITSIZE                        6             //队列单元大小，不可修改
#define MX_REQ_TASK_BUF_SIZE            (UNITNUM * UNITSIZE + 6) //请求队列大小

#define MX_REQ_OR_RSP_BUFFERSIZE        210           //配置接收发送缓冲区大小，配置时注意要使缓冲区不小于
                                                      //要发送和接收的字节个数，最小8个，最大255   2*N+9
#define MAX_OUT_NUM                     30            //存储写操作的寄存器个数。最大123个。最小为1，最大123个

#ifndef TRUE
#define TRUE      1                     //真
#endif

#ifndef FALSE
#define FALSE     0                     //假
#endif

/*=================================================================================================
** @Data type definition
**===============================================================================================*/

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern volatile const u8 cModeSelect;           //软件主从功能选择
extern volatile const u8 cMBStationNum;         //ModBus站点数目
extern volatile const u8 cUnitNum;              //请求队列单元数目
extern volatile const u8 cUnitSize;             //请求队列单元大小
extern volatile const u8 cMxReqOrRspBuffersize; //接收和发送缓冲区大小

extern const u16 cMBPollTimeOut;                //超时临界值
extern const u16 cMBPollDelay;                  //帧间隔

extern u8 gMBT15Cfg[];                          //1.5个字符时间
extern u8 gMBT35Cfg[];                          //3.5个字符时间

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : ModbusStation *MbGetStationArrayHook(void)
** @Input     : void
** @Output    : *ModbusStation:Modbus站收发数组首地址指针
** @Function  : 获取Modbus站收发数组
** @The notes : 用户无需修改
**===============================================================================================*/
ModbusStation *MbGetStationArrayHook(void);

/*=================================================================================================
** @Name      : u8 *MbGetRxBufferArrayHook(u8 port)
** @Input     : port:modbus通道号
** @Output    : 接收数据缓存数组首地址指针
** @Function  : 获取指定通道接收数据缓存数组
** @The notes : 用户无需修改
**===============================================================================================*/
u8 *MbGetRxBufferArrayHook(u8 port);

/*=================================================================================================
** @Name      : u8 *MbGetTxBufferArrayHook(u8 port)
** @Input     : port:modbus通道号
** @Output    : 发送数据缓存数组首地址指针
** @Function  : 获取指定通道发送数据缓存数组
** @The notes : 用户无需修改
**===============================================================================================*/
u8 *MbGetTxBufferArrayHook(u8 port);

/*=================================================================================================
** @Name      : u16 *MbGetReqQueueArrayHook(u8 port)
** @Input     : port:modbus通道号
** @Output    : 请求队列缓存数组首地址指针
** @Function  : 获取指定通道请求队列缓存数组
** @The notes : 用户无需修改
**===============================================================================================*/
u16 *MbGetReqQueueArrayHook(u8 port);

/*=================================================================================================
** @Name      : u16 *MbGetWrBufferArrayHook(u8 id)
** @Input     : id:数组缓存区id号[0,1,2]
** @Output    : 写数据缓存数组首地址指针
** @Function  : 获取指定通道写数据缓存数组
** @The notes : 用户无需修改
**===============================================================================================*/
u16 *MbGetWrBufferArrayHook(u8 id);

/*=================================================================================================
** @Name      : void MbDisableIRQHook(void)
** @Input     : void
** @Output    : void
** @Function  : modbus通信禁止中断(进入临界区域)
** @The notes : 用户必须完善此函数,调用禁止中断函数
**===============================================================================================*/
void MbDisableIRQHook(void);

/*=================================================================================================
** @Name      : void MbEnableIRQHook(void)
** @Input     : void
** @Output    : void
** @Function  : modbus通信使能中断(进入临界区域)
** @The notes : 用户必须完善此函数,调用使能中断函数
**===============================================================================================*/
void MbEnableIRQHook(void);

/*=================================================================================================
** @Name      : u16 *MbClientFindDataPtrHook(u8 stationID, u8 fncCode, u8 port, u16 address)
** @Input     : u8 stationID :从站ID,fncCode功能码,port:Modbus通道号,address:寄存器地址(此地址为请求报文中的寄存器地址)
** @Output    : void
** @Function  : 客户端(主机)根据响应报文中地址，映射到用户空间地址并返回；
** @The notes : 当用作客户端(主机)时，用户必须完善此函数，实现上述功能
**===============================================================================================*/
u16 *MbClientFindDataPtrHook(u8 stationID, u8 fncCode, u8 port, u16 address);

/*=================================================================================================
** @Name      : void MbClientRcvAnswerUserHook(u8 stationID, u8 fncCode, u8 port, u16 address)
** @Input     : stationID:从站ID,fncCode:功能码,port:Modbus通道号,address:寄存器地址(此地址为请求报文中的寄存器地址)
** @Output    : void
** @Function  : 客户端(主机)接收到从机应答后用户处理接口函数(应答处理完成后，提供给用户处理的接口)
** @The notes : 用户需要时完善。
**===============================================================================================*/
void MbClientRcvAnswerUserHook(u8 stationID, u8 fncCode, u8 port, u16 address);

/*=================================================================================================
** @Name      : void MbClientReqWaitOverTimeUserHook(u8 stationID, u8 fncCode, u8 port, u16 address)
** @Input     : stationID:从站ID,fncCode:功能码,port:Modbus通道号,address:寄存器地址(此地址为请求报文中的寄存器地址)
** @Output    : void
** @Function  : 客户端(主机)等待应答超时后用户处理接口函数(等待应答超时后，提供给用户的接口)
** @The notes : 用户需要时完善。
**===============================================================================================*/
void MbClientReqWaitOverTimeUserHook(u8 stationID, u8 fncCode, u8 port, u16 address);

/*=================================================================================================
** @Name      : u16 *MbServerFindDataPtrHook(u8 fncCode, u8 port, u16 address, u16 data)
** @Input     : fncCode:功能码,port:Modbus通道号,address:寄存器地址 data:写入数据(写入时有效)
** @Output    : void
** @Function  : 服务端(从机)根据请求报文中的地址，映射到用户空间地址并返回；
** @The notes : 当用作服务端(从机)时，用户必须完善此函数，实现上述功能
**===============================================================================================*/
u16 *MbServerFindDataPtrHook(u8 fncCode, u8 port, u16 address, u16 data);

/*=================================================================================================
** @Name      : void ModbusSendDataUserHook(u8 *data, u8 length, u8 port)
** @Input     : data:需要发送的数据,length:数据长度,port:发送通道号
** @Output    : void
** @Function  : Modbus通信包发送数据报文用户接口
** @The notes : 用户完善，调用串口数据发送接口,注意串口号与通道号对应
**===============================================================================================*/
void ModbusSendDataUserHook(u8 *data, u8 length, u8 port);

#endif

