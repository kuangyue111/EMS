/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : MbUserDef.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : Modbus通信包用户补充处理接口函数
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPUserAPI.h"
#include "BSPConfig.h"
#include "BSPUserAPI.h"
#include "MbInterface.h"
#include "ModbusUser.h"
#include "ModbusMap.h"
#include "HeatModbusMap.h"
#include "MbUserDef.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
volatile const u8 cModeSelect = MODESELECT;                 //软件主从功能选择
volatile const u8 cMBStationNum = MBSTATIONNUM;             //ModBus站点数目
volatile const u8 cUnitNum = UNITNUM;                       //队列单元数目
volatile const u8 cUnitSize = UNITSIZE;                     //队列单元大小
volatile const u8 cMxReqOrRspBuffersize = MX_REQ_OR_RSP_BUFFERSIZE; //发送缓冲区

const u16 cMBPollTimeOut = MB_POLL_TIMEOUT;                 //超时临界值
const u16 cMBPollDelay = MB_POLL_DELAY;                     //帧间隔

u8 gMBT15Cfg[MBSTATIONNUM] = {12,12,12};                    //端口0 1.5个字符时间，初始值为9600对应的值
u8 gMBT35Cfg[MBSTATIONNUM] = {29,29,29};                    //端口0 3.5个字符时间，初始值为9600对应的值

/*=================================================================================================
** @相关空间数组定义
**===============================================================================================*/
ModbusStation aModbusStation[MBSTATIONNUM];                 //MB结构体数组
u8 aMbRxBuffer[MBSTATIONNUM][MX_REQ_OR_RSP_BUFFERSIZE];     //接收缓冲区
u8 aMbTxBuffer[MBSTATIONNUM][MX_REQ_OR_RSP_BUFFERSIZE];     //发送缓冲区
u16 aMbReqQueue[MBSTATIONNUM][MX_REQ_TASK_BUF_SIZE];        //请求队列数组
u16 aMbWrBuffer[3][MAX_OUT_NUM];                            //写数据缓存区数组

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : ModbusStation *MbGetStationArrayHook(void)
** @Input     : void
** @Output    : *ModbusStation:Modbus站收发数组首地址指针
** @Function  : 获取Modbus站收发数组
** @The notes : 用户无需修改
**===============================================================================================*/
ModbusStation *MbGetStationArrayHook(void)
{
    return(aModbusStation);
}

/*=================================================================================================
** @Name      : u8 *MbGetRxBufferArrayHook(u8 port)
** @Input     : port:modbus通道号
** @Output    : 接收数据缓存数组首地址指针
** @Function  : 获取指定通道接收数据缓存数组
** @The notes : 用户无需修改
**===============================================================================================*/
u8 *MbGetRxBufferArrayHook(u8 port)
{
    return(aMbRxBuffer[port]);
}

/*=================================================================================================
** @Name      : u8 *MbGetTxBufferArrayHook(u8 port)
** @Input     : port:modbus通道号
** @Output    : 发送数据缓存数组首地址指针
** @Function  : 获取指定通道发送数据缓存数组
** @The notes : 用户无需修改
**===============================================================================================*/
u8 *MbGetTxBufferArrayHook(u8 port)
{
    return(aMbTxBuffer[port]);
}

/*=================================================================================================
** @Name      : u16 *MbGetReqQueueArrayHook(u8 port)
** @Input     : port:modbus通道号
** @Output    : 请求队列缓存数组首地址指针
** @Function  : 获取指定通道请求队列缓存数组
** @The notes : 用户无需修改
**===============================================================================================*/
u16 *MbGetReqQueueArrayHook(u8 port)
{
    return(aMbReqQueue[port]);
}

/*=================================================================================================
** @Name      : u16 *MbGetWrBufferArrayHook(u8 id)
** @Input     : id:数组缓存区id号[0,1,2]
** @Output    : 写数据缓存数组首地址指针
** @Function  : 获取指定通道写数据缓存数组
** @The notes : 用户无需修改
**===============================================================================================*/
u16 *MbGetWrBufferArrayHook(u8 id)
{
    return(aMbWrBuffer[id]);
}

/*=================================================================================================
** @Name      : void MbDisableIRQHook(void)
** @Input     : void
** @Output    : void
** @Function  : modbus通信禁止中断(进入临界区域)
** @The notes : 用户必须完善此函数,调用禁止中断函数
**===============================================================================================*/
void MbDisableIRQHook(void)
{
    ;
}

/*=================================================================================================
** @Name      : void MbEnableIRQHook(void)
** @Input     : void
** @Output    : void
** @Function  : modbus通信使能中断(进入临界区域)
** @The notes : 用户必须完善此函数,调用使能中断函数
**===============================================================================================*/
void MbEnableIRQHook(void)
{
    ;
}

/*=================================================================================================
** @Name      : u16 *MbClientFindDataPtrHook(u8 stationID, u8 fncCode, u8 port, u16 address)
** @Input     : u8 stationID :从站ID,fncCode功能码,port:Modbus通道号,address:寄存器地址(此地址为请求报文中的寄存器地址)
** @Output    : void
** @Function  : 客户端(主机)根据响应报文中地址，映射到用户空间地址并返回；
** @The notes : 当用作客户端(主机)时，用户必须完善此函数，实现上述功能
**===============================================================================================*/
u16 *MbClientFindDataPtrHook(u8 stationID, u8 fncCode, u8 port, u16 address)
{
    u16* temp = (void *)0;
    
    //(void)stationID;
    //(void)fncCode;
    //(void)port;
    //(void)address;
    
    //接收从机的响应信息
    //MbRcvMsgCountAdd(port);

    //查找接收从机信息寄存器地址指针
    temp = ModbusFindSlaveInfoMaping(fncCode, port, address, stationID);
    
    return temp;
}

/*=================================================================================================
** @Name      : void MbClientRcvAnswerUserHook(u8 stationID, u8 fncCode, u8 port, u16 address)
** @Input     : stationID:从站ID,fncCode:功能码,port:Modbus通道号,address:寄存器地址(此地址为请求报文中的寄存器地址)
** @Output    : void
** @Function  : 客户端(主机)接收到从机应答后用户处理接口函数(应答处理完成后，提供给用户处理的接口)
** @The notes : 用户需要时完善。
**===============================================================================================*/
void MbClientRcvAnswerUserHook(u8 stationID, u8 fncCode, u8 port, u16 address)
{
    (void)stationID;
    (void)fncCode;
    (void)port;
    (void)address;
}

/*=================================================================================================
** @Name      : void MbClientReqWaitOverTimeUserHook(u8 stationID, u8 fncCode, u8 port, u16 address)
** @Input     : stationID:从站ID,fncCode:功能码,port:Modbus通道号,address:寄存器地址(此地址为请求报文中的寄存器地址)
** @Output    : void
** @Function  : 客户端(主机)等待应答超时后用户处理接口函数(等待应答超时后，提供给用户的接口)
** @The notes : 用户需要时完善。
**===============================================================================================*/
void MbClientReqWaitOverTimeUserHook(u8 stationID, u8 fncCode, u8 port, u16 address)
{
    (void)stationID;
    (void)fncCode;
    (void)port;
    (void)address;
}

/*=================================================================================================
** @Name      : u16 *MbServerFindDataPtrHook(u8 fncCode, u8 port, u16 address, u16 data)
** @Input     : fncCode:功能码,port:Modbus通道号,address:寄存器地址 data:写入数据(写入时有效)
** @Output    : void
** @Function  : 服务端(从机)根据请求报文中的地址，映射到用户空间地址并返回；
** @The notes : 当用作服务端(从机)时，用户必须完善此函数，实现上述功能
**===============================================================================================*/
u16 *MbServerFindDataPtrHook(u8 fncCode, u8 port, u16 address, u16 data)
{
    u16* temp = (void *)0;

    /*Modbus信息接收计数*/
    MbRcvMsgCountAdd(port);
    
    temp = ModbusFindAllInfoAddrMap(fncCode, port, address, data);
    
    return(temp);
}

/*=================================================================================================
** @Name      : void ModbusSendDataUserHook(u8 *data, u8 length, u8 port)
** @Input     : data:需要发送的数据,length:数据长度,port:发送通道号
** @Output    : void
** @Function  : Modbus通信包发送数据报文用户接口
** @The notes : 用户完善，调用串口数据发送接口,注意串口号与通道号对应
**===============================================================================================*/
void ModbusSendDataUserHook(u8 *data, u8 length, u8 port)
{
    DEVSCISendDataOnAsk((e_SCINUM)port, data, length);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
