/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPCAN.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : CAN驱动函数模块
** @Instructions :
**===========================================================================================*/
#ifndef _BSPCAN_H
#define _BSPCAN_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
//CAN接收发送缓存队列大小
#define CAN_QUEUE_SIZE          sizeof(t_CANMSG)    //CAN发送报文字节空间大小
#define CAN_QUEUE_NUM           15                  //CAN发送报文队列大小
#define CAN_RCV_CYBUF           5                   //CAN接收环形缓冲区大小

#define RX_MSG_ID  (0x01)


/*=============================================================================================
** @Data type definition
**===========================================================================================*/
//CAN接收邮箱定义
typedef enum
{
    eCAN0_RX_BOX = 0,
    eCAN1_RX_BOX = 1,
    eCAN2_RX_BOX = 2,

    eCANRXBOXNUM
}e_CANRXBOXNUM;

//CAN发送邮箱定义
typedef enum
{
    eCAN0_TX_BOX = 8,
    eCAN1_TX_BOX = 9,
    eCAN2_TX_BOX = 10,

    eCANTXBOXNUM
}e_CANTXBOXNUM;

//CAN通道号定义
typedef enum
{
    eCAN0 = 0,
    eCAN1 = 1,
    eCAN2 = 2,
    
    eCANNUM
}e_CANNUM;

//CAN数据结构体
typedef struct
{
    u32 ID;         //帧标识符
    u8 Len;         //数据长度
    u8 Data[8];     //数据
}t_CANMSG;

//环形接收缓冲区
typedef struct
{
    u8 WritePoint;                  //CAN的写指针
    u8 ReadPoint;                   //CAN的读指针
    u8 FullFlag;                    //CAN缓冲区满标志
    t_CANMSG RcvBuf[CAN_RCV_CYBUF]; //CAN接收缓冲区信息
}s_CyRcvBuf;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPCanBufInit(void)
** @Input     : void
** @Output    : void
** @Function  : 初始化CAN发送/接收BUF
** @The notes : BSP调用
**===========================================================================================*/
void BSPCanBufInit(void);

/*=============================================================================================
** @Name      : void BSPCANPollRcvAPI(void)
** @Input     : void
** @Output    : void
** @Function  : CAN接收数据轮询接口
** @The notes : task调用(暂时不使用，RX fifo接收错误帧较多)
**===========================================================================================*/
void BSPCANPollRcvAPI(void);

/*=============================================================================================
** @Name      : u8 BSPCANInit(e_CANNUM canNum, u8 osc, u16 rate)
** @Input     : canNum:CAN通道号 osc:晶振频率 rate:CAN波特率
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : CAN驱动模块初始化F
** @The notes : BSP调用
**===========================================================================================*/
u8 BSPCANInit(e_CANNUM canNum, u8 osc, u16 rate);

/*=============================================================================================
** @Name      : u8 BSPCANSetRate(e_CANNUM canNum, u16 rate)
** @Input     : canNum:CAN通道号, rate:CAN通讯波特率(125/250/500/1000Kbps)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 设置CAN通信波特率
** @The notes :
**===========================================================================================*/
u8 BSPCANSetRate(e_CANNUM canNum, u16 rate);

/*=============================================================================================
** @Name      : u8 BSPCANSendMsg(e_CANNUM canNum, t_CANMSG *msg)
** @Input     : canNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(直接发送)
** @The notes : 用户调用(尽量少用,会丢帧)
**===========================================================================================*/
u8 BSPCANSendMsg(e_CANNUM canNum, t_CANMSG *msg);

/*=============================================================================================
** @Name      : u8 BSPCANSendQin(e_CANNUM canNum, t_CANMSG *msg)
** @Input     : canNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(数据压入队列)
** @The notes : 用户调用(尽量少用,会丢帧)
**===========================================================================================*/
u8 BSPCANSendQin(e_CANNUM canNum, t_CANMSG *msg);

/*=============================================================================================
** @Name      : void BSPCANSendTask(void *p)
** @Input     : *p:任务函数格式需要空指针
** @Output    : void
** @Function  : CAN信息发送任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPCANSendTask(void *p);

/*=============================================================================================
** @Name      : void BSPCANRcvTask(void *p)
** @Input     : *p:任务函数格式需要空指针
** @Output    : void
** @Function  : CAN信息接收任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPCANRcvTask(void *p);

/*=============================================================================================
** @Name      : void BSPCANSendIntHandle(e_CANNUM canNum)
** @Input     : canNum:通道号
** @Output    : void
** @Function  : CAN发送中断处理函数
** @The notes : 底层CAN中断调用
**===========================================================================================*/
void BSPCANSendIntHandle(e_CANNUM canNum);

/*=============================================================================================
** @Name      : void BSPCANRcvIntHandle(e_CANNUM canNum)
** @Input     : canNum:通道号
** @Output    : void
** @Function  : CAN接收中断处理函数
** @The notes : 底层CAN中断调用
**===========================================================================================*/
void BSPCANRcvIntHandle(e_CANNUM canNum);

#endif

