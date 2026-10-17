/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPUART.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : UART驱动函数模块
** @Instructions :
**===========================================================================================*/
#ifndef _BSPUART_H
#define _BSPUART_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define	SCI_DATA_LEN        (210)                /*SCI发送数据最大长度*/

#define	SCI_RCV_DATA_LEN    (255)                /*SCI接收数据最大长度*/
/*=============================================================================================
** @Data type definition
**===========================================================================================*/
//SCI通道定义
typedef enum 
{
    eSCI0 = 0,      // SCI通道1
    eSCI1 = 1,      // SCI通道2
    eSCI2 = 2,      // SCI通道3
    eSCI3 = 3,      // SCI通道4
    
    eSCINUM
}e_SCINUM;

//SCI发送信息结构
typedef struct
{
    u8 data[SCI_DATA_LEN];  //需发送数据值
    u8 len;                 //需发送数据实际长度
    u8 sendNum;             //即将发送数据的索引号
}t_SCISend;

//SCI通道定义
typedef enum
{
    eBAUD_2400 = 2400,
    eBAUD_4800 = 4800,
    eBAUD_9600 = 9600,
    eBAUD_19200 = 19200,
    eBAUD_38400 = 38400,
    eBAUD_56000 = 56000,
    eBAUD_57600 = 57600
    
}e_UARTBAUD;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPSCIInit(e_SCINUM sciNum, u32 rate, u8 busClock)
** @Input     : sciNum:SCI通道号  rate:通讯波特率 busClock:总线时钟
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : SCI驱动模块初始化
** @The notes : BSP调用,用户调用
**===========================================================================================*/
u8 BSPSCIInit(e_SCINUM sciNum, u32 rate, u8 busClock);

/*=============================================================================================
** @Name      : u8 BSPSCISendDataOnIRQ(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : sciNum:SCI通道号     *sendbuf:发送的数据    size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SCI中断方式发送串口数据
** @The notes : 直接发送,暂不实现
**===========================================================================================*/
u8 BSPSCISendDataOnIRQ(e_SCINUM sciNum, u8 *sendbuf, u8 size);

/*=============================================================================================
** @Name      : u8 BSPSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : sciNum:SCI通道号     *sendbuf:发送的数据    size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SCI询问方式发送串口数据
** @The notes : 直接发送,用户调用
**===========================================================================================*/
u8 BSPSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size);

/*=============================================================================================
** @Name      : void BSPUartReceiveData(e_SCINUM sciNum)
** @Input     : sciNum:SCI通道号
** @Output    : void
** @Function  : 接收SCI数据
** @The notes : 该函数不能周期调用，否则会一直等待接收完成导致程序死掉
**===========================================================================================*/
void BSPUartReceiveData(e_SCINUM sciNum);

/*=============================================================================================
** @Name      : void BSPUARTRcvTask(void *p)
** @Input     : *P
** @Output    : void
** @Function  : SCI周期接收数据任务
** @The notes : 周期2ms
**===========================================================================================*/
void BSPUARTRcvTask(void *p);

#endif

