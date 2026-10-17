/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPUART.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : UART驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPQueue.h"
#include "BSPISRHook.h"
#include "BSPUserHook.h"
#include "BSPGPIO.h"
#include "callbacks.h"
//#include "lpuart1.h"
//#include "uart_pal.h"
//#include "uart1_pal.h"
//#include "uart2_pal.h"
//#include "uart3_pal.h"
#include "lpuart_driver.h"
#include "pins_driver.h"
#include "BSPIRQ.h"
#include "BSPUART.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;
//static t_SCISend sSciSendBuf[eSCINUM];                 /*SCI发送缓冲区信息*/
//static u8 uart0ReceiveData[SCI_RCV_DATA_LEN];            /*SCI0接收信息buf*/
//static u8 uart1ReceiveData[SCI_RCV_DATA_LEN];            /*SCI1接收信息buf*/
//static u8 uart2ReceiveData[SCI_RCV_DATA_LEN];            /*SCI2接收信息buf*/
u16 sSciReceiveDataLen = SCI_RCV_DATA_LEN;               /*SCI接收缓冲区信息长度*/
static u8 Sci0bufferIndex = 0;
//static u8 Sci1bufferIndex = 0;
//static u8 Sci2bufferIndex = 0;

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void Uart0_MasterRxCallback(void *driverState, uart_event_t event, void *userData)
** @Input     : driverState:
** @Output    : void
** @Function  : SCI0接收回调函数
** @The notes : UART rx callback for continuous reception, byte by byte
**===========================================================================================*/
void Uart0_rxCallback(void *driverState, uart_event_t event, void *userData)
{
    /* Unused parameters */
    (void)driverState;
    (void)userData;

    /* Check the event type */
    if (event == UART_EVENT_RX_FULL)
    {
        //if(Sci0bufferIndex >= (SCI_RCV_DATA_LEN - 2))/*u8个字节长度*/
        //{
        //	Sci0bufferIndex = 0;
        //}

        /* The reception stops when newline is received or the buffer is full */
        if (/*(uart0ReceiveData[Sci0bufferIndex] != '\n') && */(Sci0bufferIndex != (sSciReceiveDataLen - 2U)))
        {
//            /* Update the buffer index and the rx buffer */
//        	UART_SetRxBuffer(&uart1_pal_instance, &uart0ReceiveData[Sci0bufferIndex], 1U);
//            //SCI_ISR_Rcv_Hook(eSCI0);                       				 //SCI接收处理
//            SCIRcvMsgHandle_Hook(eSCI0, uart0ReceiveData[Sci0bufferIndex]);  //SCI接收处理
            /* Update the buffer index and the rx buffer */

//        	LPUART_DRV_SetRxBuffer(0, &uart0ReceiveData[Sci0bufferIndex], 1);
//            //SCI_ISR_Rcv_Hook(eSCI0);                       				 //SCI接收处理
//            SCIRcvMsgHandle_Hook(eSCI0, uart0ReceiveData[Sci0bufferIndex]);  //SCI接收处理
        }
    	//Sci0bufferIndex++;/*一次只接收一个u8*/
    }
}

/*=============================================================================================
** @Name      : void Uart1_MasterRxCallback(void *driverState, uart_event_t event, void *userData)
** @Input     : driverState:
** @Output    : void
** @Function  : SCI1接收回调函数
** @The notes : UART rx callback for continuous reception, byte by byte
**===========================================================================================*/
void Uart1_rxCallback(void *driverState, uart_event_t event, void *userData)
{
    /* Unused parameters */
    (void)driverState;
    (void)userData;

    /* Check the event type */
    if (event == UART_EVENT_RX_FULL)
    {
        //if(Sci1bufferIndex >= (SCI_RCV_DATA_LEN - 2))/*u8个字节长度*/
        //{
        //	Sci1bufferIndex = 0;
        //}

        /* The reception stops when newline is received or the buffer is full */
//        if ((uart1ReceiveData[Sci1bufferIndex] != '\n') && (Sci1bufferIndex != (sSciReceiveDataLen - 2U)))
//        {
//            /* Update the buffer index and the rx buffer */
//        	UART_SetRxBuffer(&uart2_pal_instance, &uart1ReceiveData[Sci1bufferIndex], 1U);
//            //SCI_ISR_Rcv_Hook(eSCI1);                       				//SCI接收处理
//            SCIRcvMsgHandle_Hook(eSCI1, uart1ReceiveData[Sci1bufferIndex]); //SCI接收处理
//        }
    	//Sci1bufferIndex++;/*一次只接收一个u8*/
    }
}

/*=============================================================================================
** @Name      : void Uart2_MasterRxCallback(void *driverState, uart_event_t event, void *userData)
** @Input     : driverState:
** @Output    : void
** @Function  : SCI0接收回调函数
** @The notes : UART rx callback for continuous reception, byte by byte
**===========================================================================================*/
void Uart2_rxCallback(void *driverState, uart_event_t event, void *userData)
{
    /* Unused parameters */
    (void)driverState;
    (void)userData;

    /* Check the event type */
    if (event == UART_EVENT_RX_FULL)
    {
        //if(Sci2bufferIndex >= (SCI_RCV_DATA_LEN - 2))/*u8个字节长度*/
        //{
        //	Sci2bufferIndex = 0;
        //}

        /* The reception stops when newline is received or the buffer is full */
//        if ((uart2ReceiveData[Sci2bufferIndex] != '\n') && (Sci2bufferIndex != (sSciReceiveDataLen - 2U)))
//        {
//            /* Update the buffer index and the rx buffer */
//        	UART_SetRxBuffer(&uart3_pal_instance, &uart2ReceiveData[Sci2bufferIndex], 1U);
//            //SCI_ISR_Rcv_Hook(eSCI2);                       				//SCI接收处理
//            SCIRcvMsgHandle_Hook(eSCI2, uart2ReceiveData[Sci2bufferIndex]); //SCI接收处理
//        }
    	//Sci2bufferIndex++;/*一次只接收一个u8*/
    }
}

/*=============================================================================================
** @Name      : u8 BSPSCIInit(e_SCINUM sciNum, u32 rate, u8 busClock)
** @Input     : sciNum:SCI通道号  rate:通讯波特率 busClock:总线时钟
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : SCI驱动模块初始化
** @The notes : BSP调用,用户调用
**===========================================================================================*/
u8 BSPSCIInit(e_SCINUM sciNum, u32 rate, u8 busClock)
{
    /*模块未使能不进行初始化*/
    if(((eSCI0 == sciNum) && (1 != gBSPFunCfg.Bits.SCI0En))
        || ((eSCI1 == sciNum) && (1 != gBSPFunCfg.Bits.SCI1En))
        || ((eSCI2 == sciNum) && (1 != gBSPFunCfg.Bits.SCI2En)))
    {
        return(FALSE);
    }

    switch(sciNum)
    {
        case eSCI0:
			/* Disable LPUART1 pins */
			PINS_DRV_SetMuxModeSel(PORTC, ePIN_2, PORT_PIN_DISABLED);
			PINS_DRV_SetMuxModeSel(PORTC, ePIN_3, PORT_PIN_DISABLED);
        	/*初始化UART1管脚*/
        	PINS_DRV_SetMuxModeSel(PORTC, ePIN_2, PORT_MUX_ALT4);//UART1_RX
        	PINS_DRV_SetMuxModeSel(PORTC, ePIN_3, PORT_MUX_ALT4);//UART1_TX
        	/*初始化UART1*/
//        	UART_Init(&uart1_pal_instance, &uart1_pal_Config0);
//        	UART_SetBaudRate(&uart1_pal_instance, rate);
//        	/*初始化完SCI之后需要告诉driver监听一个简单的byte以便后面接收更多的SCI数据(callback)*/
//        	UART_ReceiveData(&uart1_pal_instance, uart0ReceiveData, 1U);

//        	LPUART_DRV_Init(0, &lpuart1_State, &lpuart1_InitConfig0);
//        	LPUART_DRV_SetBaudRate(0, rate);
            break;

//        case eSCI1:
//			/* Disable LPUART2 pins */
//			PINS_DRV_SetMuxModeSel(PORTD, ePIN_13, PORT_PIN_DISABLED);
//			PINS_DRV_SetMuxModeSel(PORTD, ePIN_14, PORT_PIN_DISABLED);
//
//        	/*初始化UART2管脚*/
//        	PINS_DRV_SetMuxModeSel(PORTD, ePIN_13, PORT_MUX_ALT3);//UART2_RX
//        	PINS_DRV_SetMuxModeSel(PORTD, ePIN_14, PORT_MUX_ALT3);//UART2_TX
//        	/*初始化UART2*/
//        	UART_Init(&uart2_pal_instance, &uart2_pal_Config0);
//        	UART_SetBaudRate(&uart2_pal_instance, rate);
//        	/*初始化完SCI之后需要告诉driver监听一个简单的byte以便后面接收更多的SCI数据(callback)*/
//        	UART_ReceiveData(&uart2_pal_instance, uart1ReceiveData, 1U);
//            break;
//
//        case eSCI2:
//			/* Disable LPUART3 pins */
//			PINS_DRV_SetMuxModeSel(PORTD, ePIN_6, PORT_PIN_DISABLED);
//			PINS_DRV_SetMuxModeSel(PORTD, ePIN_7, PORT_PIN_DISABLED);
//        	/*初始化UART3管脚*/
//        	PINS_DRV_SetMuxModeSel(PORTD, ePIN_6, PORT_MUX_ALT2);//UART3_RX
//        	PINS_DRV_SetMuxModeSel(PORTD, ePIN_7, PORT_MUX_ALT2);//UART3_TX
//        	/*初始化UART3*/
//        	UART_Init(&uart3_pal_instance, &uart3_pal_Config0);
//        	UART_SetBaudRate(&uart3_pal_instance, rate);
//        	/*初始化完SCI之后需要告诉driver监听一个简单的byte以便后面接收更多的SCI数据(callback)*/
//        	UART_ReceiveData(&uart3_pal_instance, uart2ReceiveData, 1U);
//            break;

        default:
            break;
    }

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPSCISendDataOnIRQ(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : sciNum:SCI通道号     *sendbuf:发送的数据    size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SCI中断方式发送串口数据
** @The notes : 直接发送,暂不实现
**===========================================================================================*/
u8 BSPSCISendDataOnIRQ(e_SCINUM sciNum, u8 *sendbuf, u8 size)
{
    //u8 i = 0;
    
    /*模块未使能不进行初始化*/
    if(((eSCI0 == sciNum) && (1 != gBSPFunCfg.Bits.SCI0En))
        || ((eSCI1 == sciNum) && (1 != gBSPFunCfg.Bits.SCI1En))
        || ((eSCI2 == sciNum) && (1 != gBSPFunCfg.Bits.SCI2En)))
    {
        return(FALSE);
    }
    
    if((sciNum >= eSCINUM) || (size > SCI_DATA_LEN))
    {
        return(FALSE);
    }
    
    BSP_DISABLE_IRQ();
    /*
    //获取发送数据信息
    for(i = 0; i < size; i++)
    {
        sSciSendBuf[sciNum].data[i] = sendbuf[i];
    }
    sSciSendBuf[sciNum].len = size;
     */

	//中断发送
    if(eSCI0 == sciNum)
    {
    	//UART_SendData(&uart_pal1_instance, sendbuf, size);
    }
    else if(eSCI1 == sciNum)
    {
        //UART_SendData(&uart_pal2_instance, sendbuf, size);
    }
    else
    {
    	//UART_SendData(&uart_pal3_instance, sendbuf, size);
    }

    BSP_ENABLE_IRQ();

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : sciNum:SCI通道号     *sendbuf:发送的数据    size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SCI询问方式发送串口数据
** @The notes : 直接发送,用户调用
**===========================================================================================*/
u8 BSPSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size)
{
    /*模块未使能不进行初始化*/
    if(((eSCI0 == sciNum) && (1 != gBSPFunCfg.Bits.SCI0En))
        || ((eSCI1 == sciNum) && (1 != gBSPFunCfg.Bits.SCI1En))
        || ((eSCI2 == sciNum) && (1 != gBSPFunCfg.Bits.SCI2En)))
    {
        return(FALSE);
    }

    if((sciNum >= eSCINUM) || (size > SCI_DATA_LEN))
    {
        return(FALSE);
    }

    //BSP_ENTER_CRITICAL();

    /*开始询问发送数据*/
    if(eSCI0 == sciNum)
    {
    	//LPUART_DRV_SendDataPolling(sciNum, sendbuf, size);
    }
    else if(eSCI1 == sciNum)
    {
    	//LPUART_DRV_SendDataPolling(sciNum, sendbuf, size);
    }
    else
    {
    	//LPUART_DRV_SendDataPolling(sciNum, sendbuf, size);
    }

    //BSP_EXIT_CRITICAL();

    return(TRUE);
}

/*=============================================================================================
** @Name      : void BSPUARTRcvTask(void *p)
** @Input     : *P
** @Output    : void
** @Function  : SCI周期接收数据任务
** @The notes : 周期2ms
**===========================================================================================*/
void BSPUARTRcvTask(void *p)
{
	u8 sciNum = 0;

	for(;;)
	{
		for(sciNum = eSCI0; sciNum < eSCINUM; sciNum++)
		{
			BSPUartReceiveData((e_SCINUM)sciNum);
		}
		OSIF_TimeDelay(2);/*周期2ms*/
	}
}

/*=============================================================================================
** @Name      : void BSPUartReceiveData(e_SCINUM sciNum)
** @Input     : sciNum:SCI通道号
** @Output    : void
** @Function  : 接收SCI数据
** @The notes : 该函数周期调用
**===========================================================================================*/
void BSPUartReceiveData(e_SCINUM sciNum)
{
    /* Receive and store data byte by byte until new line character is received,
     * or the buffer becomes full (256 characters received)
     */
	if(eSCI0 == sciNum)
	{
		//UART_ReceiveData(&uart1_pal_instance, uart0ReceiveData, 1U);
		//LPUART_DRV_ReceiveData(0, uart0ReceiveData, 1);
	}
	else if(eSCI1 == sciNum)
	{
		//UART_ReceiveData(&uart2_pal_instance, uart1ReceiveData, 1U);
	}
	else
	{
		//UART_ReceiveData(&uart3_pal_instance, uart2ReceiveData, 1U);
	}
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/

