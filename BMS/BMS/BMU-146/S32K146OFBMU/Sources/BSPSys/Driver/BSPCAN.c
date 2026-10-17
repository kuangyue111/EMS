/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPCAN.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : CAN驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "string.h"
#include "osif.h"
#include "can_pal.h"
#include "can1_pal.h"
#include "can2_pal.h"
#include "can3_pal.h"
#include "flexcan_driver.h"
#include "flexcan_hw_access.h"
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPQueue.h"
#include "BSPISRHook.h"
#include "BSPUserHook.h"
#include "BSPIRQ.h"
#include "BSPGPIO.h"
#include "BSPCAN.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;

static u8 sCANSendBuff[eCANNUM][QUE_SIZEOF(CAN_QUEUE_SIZE, CAN_QUEUE_NUM)]; /*定义CAN发送缓冲区空间*/
static s_CyRcvBuf sCANRcvCyBuf[eCANNUM];                                    /*CAN接收环形缓冲区*/
static u8 sCANCyUnDealNum[eCANNUM] = {0};                                   /*CAN环形缓冲区未处理信息数目*/

/* ID Filter table */
flexcan_id_table_t filterTable[16];

flexcan_msgbuff_t can0RxMsg;/*CAN报文数据接收结构体*/
flexcan_msgbuff_t can1RxMsg;/*CAN报文数据接收结构体*/
flexcan_msgbuff_t can2RxMsg;/*CAN报文数据接收结构体*/

/* Set information about the data to be received */
flexcan_data_info_t dataInfo =
{
	  .data_length = 8U,
	  .msg_id_type = FLEXCAN_MSG_ID_EXT,
	  .enable_brs  = false,
	  .fd_enable   = false,
	  .fd_padding  = 0U
};

/* Set information about the data to be sended */
can_buff_config_t buffCfg =
{
	.enableFD = false,
	.enableBRS = false,
	.fdPadding = 0U,
	.idType = CAN_MSG_ID_EXT,
	.isRemote = false,
};

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CANRcvCyBufInit(e_CANNUM canNum)
** @Input     : canNum:CAN通道号
** @Output    : void
** @Function  : 环形接收缓冲区初始化
** @The notes :
**===========================================================================================*/
static void CANRcvCyBufInit(e_CANNUM canNum);

/*=============================================================================================
** @Name      : static u8 CANWriteRcvCyBuf(e_CANNUM canNum, flexcan_msgbuff_t *pRxMsg)
** @Input     : canNum:CAN通道号, *pRcvMsg:CAN信息结构体接收数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : CAN接收中断函数将CAN报文写入接收环形缓冲区
** @The notes :
**===========================================================================================*/
static u8 CANWriteRcvCyBuf(e_CANNUM canNum, flexcan_msgbuff_t *pRxMsg);

/*=============================================================================================
** @Name      : static u8 CANReadRcvCyBuf(e_CANNUM canNum, t_CANMSG *pRcvMsg)
** @Input     : canNum:CAN通道号, *pRcvMsg:CAN信息结构体接收数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : CAN接收处理任务读CAN接收环形缓冲区信息出来处理
** @The notes :
**===========================================================================================*/
static u8 CANReadRcvCyBuf(e_CANNUM canNum, t_CANMSG *pRcvMsg);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void flexcan0_Callback(uint8_t instance, flexcan_event_type_t eventType, flexcan_state_t *flexcanState)
** @Input     : instance
** @Output    : void
** @Function  : CAN接收回调函数
** @The notes :
**===========================================================================================*/
void flexcan0_Callback(uint8_t instance, flexcan_event_type_t eventType, flexcan_state_t *flexcanState)
{
	(void)flexcanState;
	(void)instance;

	switch(eventType)
	{
		case FLEXCAN_EVENT_RX_COMPLETE:
			BSPCANRcvIntHandle((e_CANNUM)0);
			break;

		case FLEXCAN_EVENT_RXFIFO_COMPLETE:
			break;

		case FLEXCAN_EVENT_DMA_COMPLETE:
			break;

		case FLEXCAN_EVENT_TX_COMPLETE:
			break;
		default:
			break;
	}
}

/*=============================================================================================
** @Name      : void flexcan1_Callback(uint8_t instance, flexcan_event_type_t eventType, flexcan_state_t *flexcanState)
** @Input     : instance
** @Output    : void
** @Function  : CAN接收回调函数
** @The notes :
**===========================================================================================*/
void flexcan1_Callback(uint8_t instance, flexcan_event_type_t eventType, flexcan_state_t *flexcanState)
{
	(void)flexcanState;
	(void)instance;

	switch(eventType)
	{
		case FLEXCAN_EVENT_RX_COMPLETE:
			BSPCANRcvIntHandle((e_CANNUM)1);
			break;

		case FLEXCAN_EVENT_RXFIFO_COMPLETE:
			break;

		case FLEXCAN_EVENT_DMA_COMPLETE:
			break;

		case FLEXCAN_EVENT_TX_COMPLETE:
			break;
		default:
			break;
	}
}

/*=============================================================================================
** @Name      : void flexcan2_Callback(uint8_t instance, flexcan_event_type_t eventType, flexcan_state_t *flexcanState)
** @Input     : instance
** @Output    : void
** @Function  : CAN接收回调函数
** @The notes :
**===========================================================================================*/
void flexcan2_Callback(uint8_t instance, flexcan_event_type_t eventType, flexcan_state_t *flexcanState)
{
	(void)flexcanState;
	(void)instance;

	switch(eventType)
	{
		case FLEXCAN_EVENT_RX_COMPLETE:
			BSPCANRcvIntHandle((e_CANNUM)2);
			break;

		case FLEXCAN_EVENT_RXFIFO_COMPLETE:
			break;

		case FLEXCAN_EVENT_DMA_COMPLETE:
			break;

		case FLEXCAN_EVENT_TX_COMPLETE:
			break;
		default:
			break;
	}
}

/*=============================================================================================
** @Name      : void BSPCanBufInit(void)
** @Input     : void
** @Output    : void
** @Function  : 初始化CAN发送/接收BUF
** @The notes : BSP调用
**===========================================================================================*/
void BSPCanBufInit(void)
{
    u8 canNum = 0;

    /*初始化发送缓冲区队列和接收缓冲区队列*/
    for(canNum = 0;canNum < eCANNUM; canNum++)
    {
        /*环形接收缓冲区初始化*/
        CANRcvCyBufInit((e_CANNUM)canNum);

        /*发送队列缓冲区初始化*/
        (void)QueInit((void *)&sCANSendBuff[canNum], CAN_QUEUE_SIZE, CAN_QUEUE_NUM);
    }
}

/*=============================================================================================
** @Name      : void BSPCANPollRcvAPI(void)
** @Input     : void
** @Output    : void
** @Function  : CAN接收数据轮询接口
** @The notes : task调用(暂时不使用，RX fifo接收错误帧较多)
**===========================================================================================*/
void BSPCANPollRcvAPI(void)
{
	BSPCANRcvIntHandle((e_CANNUM)0);
	BSPCANRcvIntHandle((e_CANNUM)1);
	BSPCANRcvIntHandle((e_CANNUM)2);
}

/*=============================================================================================
** @Name      : u8 BSPCANInit(e_CANNUM canNum, u8 osc, u16 rate)
** @Input     : canNum:CAN通道号 osc:晶振频率 rate:CAN波特率
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : CAN驱动模块初始化F
** @The notes : BSP调用
**===========================================================================================*/
u8 BSPCANInit(e_CANNUM canNum, u8 osc, u16 rate)
{
	(void)osc;
	u16 id_counter = 0;

    if(canNum >= eCANNUM)
    {
        return(FALSE);
    }
    
    /*模块未使能不进行初始化*/
    if(((eCAN0 == canNum) && (1 != gBSPFunCfg.Bits.CAN0En))
    	|| ((eCAN1 == canNum) && (1 != gBSPFunCfg.Bits.CAN1En))
		|| ((eCAN2 == canNum) && (1 != gBSPFunCfg.Bits.CAN2En)))
    {
        return(FALSE);
    }
    
    switch(canNum)
    {
		case 0:
			CAN_Deinit(&can1_pal_instance);

		    PINS_DRV_SetMuxModeSel(PORTE, ePIN_5, PORT_MUX_ALT5);       //设置PORT管脚为CAN_H功能，复用管脚(具体以硬件设计为主)
		    PINS_DRV_SetMuxModeSel(PORTE, ePIN_4, PORT_MUX_ALT5);       //设置PORT管脚为CAN_L功能，复用管脚

		    //初始化CAN0驱动模块
		    CAN_Init(&can1_pal_instance, &can1_pal_Config);
		    INT_SYS_SetPriority(CAN0_ORed_0_15_MB_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
		    BSPCANSetRate(canNum, rate);
			/* Install callback function */
			FLEXCAN_DRV_InstallEventCallback(canNum, (flexcan_callback_t)flexcan0_Callback, NULL);

        	/* Configure RX message buffer with index RX_MSG_ID and RX_MAILBOX */
        	FLEXCAN_DRV_ConfigRxMb(canNum, eCAN0_RX_BOX, &dataInfo, RX_MSG_ID);
        	// Fill id filter table,
        	for(id_counter=0;id_counter<16;id_counter++)
        	{
        		filterTable[id_counter].isRemoteFrame = false;
        		filterTable[id_counter].isExtendedFrame = (id_counter % 2);
        		filterTable[id_counter].id = id_counter +1 ;
        	}
        	/* Configure RX FIFO ID filter table elements based on filter table defined above*/
        	FLEXCAN_DRV_ConfigRxFifo(canNum, FLEXCAN_RX_FIFO_ID_FORMAT_A, filterTable);
        	/* set individual masking type */
        	FLEXCAN_DRV_SetRxMaskType(canNum, FLEXCAN_RX_MASK_INDIVIDUAL);
        	/* first 10 filter items are masked with RXIMR0-RXIMR9 */
        	for(id_counter=0;id_counter<10;id_counter++)
        	FLEXCAN_DRV_SetRxIndividualMask(canNum, FLEXCAN_MSG_ID_EXT, id_counter, 0);
        	/* rest of filter items are masked with RXFGMASK */
        	FLEXCAN_DRV_SetRxFifoGlobalMask(canNum, FLEXCAN_MSG_ID_EXT, 0);
        	/* set mask affecting MB10 */
        	FLEXCAN_DRV_SetRxIndividualMask(canNum, FLEXCAN_MSG_ID_EXT, eCAN0_RX_BOX, 0);

        	/* Start receiving data in RX_MAILBOX. */
        	FLEXCAN_DRV_Receive(canNum, eCAN0_RX_BOX, &can0RxMsg);
        	//CAN_Receive(&can1_pal_instance, eCAN0_RX_BOX, &can0RxMsg);
        	/* Start receiving data in RX_RXFIFO. */
        	//FLEXCAN_DRV_RxFifo(canNum, &recvBuff2);
			break;

		case 1:
			CAN_Deinit(&can2_pal_instance);

		    PINS_DRV_SetMuxModeSel(PORTA, ePIN_13, PORT_MUX_ALT3);       //设置PORT管脚为CAN_H功能，复用管脚(具体以硬件设计为主)
		    PINS_DRV_SetMuxModeSel(PORTA, ePIN_12, PORT_MUX_ALT3);       //设置PORT管脚为CAN_L功能，复用管脚

		    //初始化CAN2驱动模块
		    CAN_Init(&can2_pal_instance, &can2_pal_Config);
		    INT_SYS_SetPriority(CAN1_ORed_0_15_MB_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
		    BSPCANSetRate(canNum, rate);
			/* Install callback function */
			FLEXCAN_DRV_InstallEventCallback(canNum, (flexcan_callback_t)flexcan1_Callback, NULL);

        	/* Configure RX message buffer with index RX_MSG_ID and RX_MAILBOX */
        	FLEXCAN_DRV_ConfigRxMb(canNum, eCAN1_RX_BOX, &dataInfo, RX_MSG_ID);
        	// Fill id filter table,
        	for(id_counter=0;id_counter<16;id_counter++)
        	{
        		filterTable[id_counter].isRemoteFrame = false;
        		filterTable[id_counter].isExtendedFrame = (id_counter % 2);
        		filterTable[id_counter].id = id_counter +1 ;
        	}
        	/* Configure RX FIFO ID filter table elements based on filter table defined above*/
        	FLEXCAN_DRV_ConfigRxFifo(canNum, FLEXCAN_RX_FIFO_ID_FORMAT_A, filterTable);
        	/* set individual masking type */
        	FLEXCAN_DRV_SetRxMaskType(canNum, FLEXCAN_RX_MASK_INDIVIDUAL);
        	/* first 10 filter items are masked with RXIMR0-RXIMR9 */
        	for(id_counter=0;id_counter<10;id_counter++)
        	FLEXCAN_DRV_SetRxIndividualMask(canNum, FLEXCAN_MSG_ID_EXT, id_counter, 0);
        	/* rest of filter items are masked with RXFGMASK */
        	FLEXCAN_DRV_SetRxFifoGlobalMask(canNum, FLEXCAN_MSG_ID_EXT, 0);
        	/* set mask affecting MB10 */
        	FLEXCAN_DRV_SetRxIndividualMask(canNum, FLEXCAN_MSG_ID_EXT, eCAN1_RX_BOX, 0);

        	/* Start receiving data in RX_MAILBOX. */
        	FLEXCAN_DRV_Receive(canNum, eCAN1_RX_BOX, &can1RxMsg);

        	//CAN_Receive(&can1_pal_instance, eCAN0_RX_BOX, &can1RxMsg);
        	/* Start receiving data in RX_RXFIFO. */
        	//FLEXCAN_DRV_RxFifo(canNum, &recvBuff4);
			break;

		case 2:
		    CAN_Deinit(&can3_pal_instance);

		    PINS_DRV_SetMuxModeSel(PORTC, ePIN_17, PORT_MUX_ALT3);       //设置PORT管脚为CAN_H功能，复用管脚(具体以硬件设计为主)
		    PINS_DRV_SetMuxModeSel(PORTC, ePIN_16, PORT_MUX_ALT3);       //设置PORT管脚为CAN_L功能，复用管脚

		    //初始化CAN3驱动模块
		    CAN_Init(&can3_pal_instance, &can3_pal_Config);
		    INT_SYS_SetPriority(CAN0_ORed_0_15_MB_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
		    BSPCANSetRate(canNum, rate);
			/* Install callback function */
			FLEXCAN_DRV_InstallEventCallback(canNum, (flexcan_callback_t)flexcan2_Callback, NULL);

        	/* Configure RX message buffer with index RX_MSG_ID and RX_MAILBOX */
        	FLEXCAN_DRV_ConfigRxMb(canNum, eCAN2_RX_BOX, &dataInfo, RX_MSG_ID);
        	// Fill id filter table,
        	for(id_counter=0;id_counter<16;id_counter++)
        	{
        		filterTable[id_counter].isRemoteFrame = false;
        		filterTable[id_counter].isExtendedFrame = (id_counter % 2);
        		filterTable[id_counter].id = id_counter +1 ;
        	}
        	/* Configure RX FIFO ID filter table elements based on filter table defined above*/
        	FLEXCAN_DRV_ConfigRxFifo(canNum, FLEXCAN_RX_FIFO_ID_FORMAT_A, filterTable);
        	/* set individual masking type */
        	FLEXCAN_DRV_SetRxMaskType(canNum, FLEXCAN_RX_MASK_INDIVIDUAL);
        	/* first 10 filter items are masked with RXIMR0-RXIMR9 */
        	for(id_counter=0;id_counter<10;id_counter++)
        	FLEXCAN_DRV_SetRxIndividualMask(canNum, FLEXCAN_MSG_ID_EXT, id_counter, 0);
        	/* rest of filter items are masked with RXFGMASK */
        	FLEXCAN_DRV_SetRxFifoGlobalMask(canNum, FLEXCAN_MSG_ID_EXT, 0);
        	/* set mask affecting MB10 */
        	FLEXCAN_DRV_SetRxIndividualMask(canNum, FLEXCAN_MSG_ID_EXT, eCAN2_RX_BOX, 0);

        	/* Start receiving data in RX_MAILBOX. */
        	FLEXCAN_DRV_Receive(canNum, eCAN2_RX_BOX, &can2RxMsg);

        	//CAN_Receive(&can1_pal_instance, eCAN0_RX_BOX, &can2RxMsg);
        	/* Start receiving data in RX_RXFIFO. */
        	//FLEXCAN_DRV_RxFifo(canNum, &recvBuff6);
			break;

		default:
			break;

    }

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPCANSetRate(e_CANNUM canNum, u16 rate)
** @Input     : canNum:CAN通道号, rate:CAN通讯波特率(125/250/500/1000Kbps)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 设置CAN通信波特率
** @The notes :
**===========================================================================================*/
u8 BSPCANSetRate(e_CANNUM canNum, u16 rate)
{
	can_time_segment_t can_time_segment;

	if((125 != rate) && (250 != rate) && (500 != rate) && (1000 != rate))
	{
		return(false);
	}

	if(1000 == rate)
	{
		can_time_segment.propSeg = 6;         /*!< Propagation segment */
		can_time_segment.phaseSeg1 = 3;       /*!< Phase segment 1 */
		can_time_segment.phaseSeg2 = 3;       /*!< Phase segment 2 */
		can_time_segment.preDivider = 0;      /*!< Clock prescaler division factor */
		can_time_segment.rJumpwidth = 1;      /*!< Resync jump width */
	}
	else if(500 == rate)
	{
		can_time_segment.propSeg = 6;         /*!< Propagation segment */
		can_time_segment.phaseSeg1 = 3;       /*!< Phase segment 1 */
		can_time_segment.phaseSeg2 = 3;       /*!< Phase segment 2 */
		can_time_segment.preDivider = 1;      /*!< Clock prescaler division factor */
		can_time_segment.rJumpwidth = 1;      /*!< Resync jump width */
	}
	else if(250 == rate)
	{
		can_time_segment.propSeg = 6;         /*!< Propagation segment */
		can_time_segment.phaseSeg1 = 3;       /*!< Phase segment 1 */
		can_time_segment.phaseSeg2 = 3;       /*!< Phase segment 2 */
		can_time_segment.preDivider = 3;      /*!< Clock prescaler division factor */
		can_time_segment.rJumpwidth = 1;      /*!< Resync jump width */
	}
	else
	{
		can_time_segment.propSeg = 6;         /*!< Propagation segment */
		can_time_segment.phaseSeg1 = 3;       /*!< Phase segment 1 */
		can_time_segment.phaseSeg2 = 3;       /*!< Phase segment 2 */
		can_time_segment.preDivider = 7;      /*!< Clock prescaler division factor */
		can_time_segment.rJumpwidth = 1;      /*!< Resync jump width */
	}

	if(eCAN0 == canNum)
	{
		CAN_SetBitrate(&can1_pal_instance, CAN_NOMINAL_BITRATE, &can_time_segment);
	}
	else if(eCAN1 == canNum)
	{
		CAN_SetBitrate(&can2_pal_instance, CAN_NOMINAL_BITRATE, &can_time_segment);
	}
	else if(eCAN2 == canNum)
	{
		CAN_SetBitrate(&can3_pal_instance, CAN_NOMINAL_BITRATE, &can_time_segment);
	}

	return(true);
}

/*=============================================================================================
** @Name      : u8 BSPCANSendMsg(e_CANNUM canNum, t_CANMSG *msg)
** @Input     : canNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(直接发送)
** @The notes : 用户调用(尽量少用,会丢帧)
**===========================================================================================*/
u8 BSPCANSendMsg(e_CANNUM canNum, t_CANMSG *msg)
{
	can_message_t sendMsg;

    /*模块未使能*/
    if(((eCAN0 == canNum) && (1 != gBSPFunCfg.Bits.CAN0En))
            || ((eCAN1 == canNum) && (1 != gBSPFunCfg.Bits.CAN1En))
        	|| ((eCAN2 == canNum) && (1 != gBSPFunCfg.Bits.CAN2En)))
    {
        return(FALSE);
    }

	sendMsg.cs = 0;
	sendMsg.id = msg->ID;
	sendMsg.length = msg->Len;
	sendMsg.data[0] = msg->Data[0];
	sendMsg.data[1] = msg->Data[1];
	sendMsg.data[2] = msg->Data[2];
	sendMsg.data[3] = msg->Data[3];
	sendMsg.data[4] = msg->Data[4];
	sendMsg.data[5] = msg->Data[5];
	sendMsg.data[6] = msg->Data[6];
	sendMsg.data[7] = msg->Data[7];

	if(eCAN0 == canNum)
	{
		/* Configure TX buffer with index TX_MAILBOX*/
		CAN_ConfigTxBuff(&can1_pal_instance, eCAN0_TX_BOX, &buffCfg);
		CAN_AbortTransfer(&can1_pal_instance, eCAN0_TX_BOX);
		CAN_Send(&can1_pal_instance, eCAN0_TX_BOX, &sendMsg);
	}
	else if(eCAN1 == canNum)
	{
		/* Configure TX buffer with index TX_MAILBOX*/
		CAN_ConfigTxBuff(&can2_pal_instance, eCAN1_TX_BOX, &buffCfg);
		CAN_AbortTransfer(&can2_pal_instance, eCAN1_TX_BOX);
		CAN_Send(&can2_pal_instance, eCAN1_TX_BOX, &sendMsg);
	}
	else
	{
		/* Configure TX buffer with index TX_MAILBOX*/
		CAN_ConfigTxBuff(&can3_pal_instance, eCAN2_TX_BOX, &buffCfg);
		CAN_AbortTransfer(&can3_pal_instance, eCAN2_TX_BOX);
		CAN_Send(&can3_pal_instance, eCAN2_TX_BOX, &sendMsg);
	}

	return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPCANSendQin(e_CANNUM canNum, t_CANMSG *msg)
** @Input     : canNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(数据压入队列)
** @The notes : 用户调用(尽量少用,会丢帧)
**===========================================================================================*/
u8 BSPCANSendQin(e_CANNUM canNum, t_CANMSG *msg)
{
    u8 result = 0;
    
    /*模块未使能不进行初始化*/
    if(((eCAN0 == canNum) && (1 != gBSPFunCfg.Bits.CAN0En))
		|| ((eCAN1 == canNum) && (1 != gBSPFunCfg.Bits.CAN1En))
		|| ((eCAN2 == canNum) && (1 != gBSPFunCfg.Bits.CAN2En)))
    {
        return(FALSE);
    }
    
    if(canNum >= eCANNUM)
    {
        return(FALSE);
    }
    result = QueIn((void *)sCANSendBuff[canNum], (void *)(msg));
    
    return(result);
}

/*=============================================================================================
** @Name      : void BSPCANSendTask(void *p)
** @Input     : *p:任务函数格式需要空指针
** @Output    : void
** @Function  : CAN信息发送任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPCANSendTask(void *p)
{
    u8 index = 0;
    static t_CANMSG txMsg;
    static u8 sHisErr[eCANNUM] = {TRUE, TRUE};
    
    p = p;

    for(;;)
    {
    	BSP_DISABLE_IRQ();
		for(index = 0; index < eCANNUM; index++)
		{
			if(QueGetUnitNum((void *)&sCANSendBuff[index]))                     /*判断CAN发送缓冲区中是否有值*/
			{
				if(FALSE == sHisErr)                                            /*上次发送失败*/
				{
					(void)BSPCANSendMsg((e_CANNUM)index, &txMsg);               /*直接重发一次*/
					sHisErr[index] = TRUE;
				}
				else if(QueOut((void *)&sCANSendBuff[index], (void *)(&txMsg))) /*出对列成功*/
				{
					sHisErr[index] = BSPCANSendMsg((e_CANNUM)index, &txMsg);    /*发送数据*/

					CANSendData_Hook((e_CANNUM)index, txMsg);                   /*CAN发送处理接口函数*/
				}
			}
		}
	    BSP_ENABLE_IRQ();
		OSIF_TimeDelay(2);/*周期2ms*/
    }
}

/*=============================================================================================
** @Name      : void BSPCANRcvTask(void *p)
** @Input     : *p:任务函数格式需要空指针
** @Output    : void
** @Function  : CAN信息接收任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPCANRcvTask(void *p)
{
    u8 index = 0;
    t_CANMSG rcvMsg;
    
    p = p;

    for(;;)
    {
		for(index = 0; index < eCANNUM; index++)
		{
			/*从缓冲区读取接收到的报文提交到用户处理*/
			if(CANReadRcvCyBuf((e_CANNUM)index, &rcvMsg) == 1)
			{
				/*计算缓冲区未处理信息*/
				if(sCANCyUnDealNum[index] >= 1)
				{
					sCANCyUnDealNum[index]--;
				}

				/*CAN接收报文处理函数*/
				(void)CANRcvMsgHandle_Hook((e_CANNUM)index, &rcvMsg);
			}
		}
		OSIF_TimeDelay(1);/*周期1ms,时间越短越好*/
    }
}

/*=============================================================================================
** @Name      : void BSPCANSendIntHandle(e_CANNUM canNum)
** @Input     : canNum:通道号
** @Output    : void
** @Function  : CAN发送中断处理函数
** @The notes : 底层CAN中断调用
**===========================================================================================*/
void BSPCANSendIntHandle(e_CANNUM canNum)
{
    //CAN_ISR_Send_Hook(canNum);
}

/*=============================================================================================
** @Name      : void BSPCANRcvIntHandle(e_CANNUM canNum)
** @Input     : canNum:通道号
** @Output    : void
** @Function  : CAN接收中断处理函数
** @The notes : 0.1ms定时器调用
**===========================================================================================*/
void BSPCANRcvIntHandle(e_CANNUM canNum)
{
	u8 i = 0;

    if(canNum >= eCANNUM)
    {
        return;
    }

    /*模块未使能不进行数据查询*/
    if(((eCAN0 == canNum) && (1 != gBSPFunCfg.Bits.CAN0En))
    	|| ((eCAN1 == canNum) && (1 != gBSPFunCfg.Bits.CAN1En))
		|| ((eCAN2 == canNum) && (1 != gBSPFunCfg.Bits.CAN2En)))
    {
        return;
    }

    //BSP_DISABLE_IRQ();  /* 进行临界区保护;关闭中断 */

	if(eCAN0 == canNum)
	{
		/* enable receiving data in RX_MAILBOX again */
		FLEXCAN_DRV_Receive(canNum, eCAN0_RX_BOX, &can0RxMsg);
		//CAN_Receive(&can1_pal_instance, eCAN0_RX_BOX, &can0RxMsg);

		//FLEXCAN_DRV_RxFifo(0, &can0RxMsg);

		if((can0RxMsg.dataLen > 0) && (can0RxMsg.dataLen <= 8))
		{
			/*将数据转存到接收环形缓存BUF中*/
			CANWriteRcvCyBuf(canNum, &can0RxMsg);
			can0RxMsg.cs = 0;
			can0RxMsg.msgId = 0;
			can0RxMsg.dataLen = 0;

			for(i = 0; i < 8; i++)/*只用到前8个数据*/
			{
				can0RxMsg.data[i] = 0;
			}
		}
	}
	else if(eCAN1 == canNum)
	{
		/* enable receiving data in RX_MAILBOX again */
		FLEXCAN_DRV_Receive(canNum, eCAN1_RX_BOX, &can1RxMsg);
		//CAN_Receive(&can2_pal_instance, eCAN0_RX_BOX, &can1RxMsg);

		//FLEXCAN_DRV_RxFifo(1, &can1RxMsg);

		if((can1RxMsg.dataLen > 0) && (can1RxMsg.dataLen <= 8))
		{
			/*将数据转存到接收环形缓存BUF中*/
			CANWriteRcvCyBuf(canNum, &can1RxMsg);
			can1RxMsg.cs = 0;
			can1RxMsg.msgId = 0;
			can1RxMsg.dataLen = 0;

			for(i = 0; i < 8; i++)/*只用到前8个数据*/
			{
				can1RxMsg.data[i] = 0;
			}
		}
	}
	else
	{
		/* enable receiving data in RX_MAILBOX again */
		FLEXCAN_DRV_Receive(canNum, eCAN2_RX_BOX, &can2RxMsg);
		//CAN_Receive(&can2_pal_instance, eCAN0_RX_BOX, &can2RxMsg);

		//FLEXCAN_DRV_RxFifo(2, &can2RxMsg);

		if((can2RxMsg.dataLen > 0) && (can2RxMsg.dataLen <= 8))
		{
			/*将数据转存到接收环形缓存BUF中*/
			CANWriteRcvCyBuf(canNum, &can2RxMsg);
			can2RxMsg.cs = 0;
			can2RxMsg.msgId = 0;
			can2RxMsg.dataLen = 0;

			for(i = 0; i < 8; i++)/*只用到前8个数据*/
			{
				can2RxMsg.data[i] = 0;
			}
		}
	}

    //BSP_ENABLE_IRQ();   /* 退出临界区,开中断 */
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CANRcvCyBufInit(e_CANNUM canNum)
** @Input     : canNum:CAN通道号
** @Output    : void
** @Function  : 环形接收缓冲区初始化
** @The notes :
**===========================================================================================*/
static void CANRcvCyBufInit(e_CANNUM canNum)
{
    if(canNum >= eCANNUM)
    {
        return;
    }
    
    memset((u8 *)(&(sCANRcvCyBuf[canNum])), 0, sizeof(sCANRcvCyBuf[canNum]));
    
    sCANRcvCyBuf[canNum].FullFlag   = 0;   
    sCANRcvCyBuf[canNum].ReadPoint  = 0;
    sCANRcvCyBuf[canNum].WritePoint = 0;
}

/*=============================================================================================
** @Name      : static u8 CANWriteRcvCyBuf(e_CANNUM canNum, flexcan_msgbuff_t *pRxMsg)
** @Input     : canNum:CAN通道号, *pRcvMsg:CAN信息结构体接收数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : CAN接收中断函数将CAN报文写入接收环形缓冲区
** @The notes :
**===========================================================================================*/
static u8 CANWriteRcvCyBuf(e_CANNUM canNum, flexcan_msgbuff_t *pRxMsg)
{
    u8 i = 0;
    u8 result = 0;
    u8 wrtPoint = 0;
    
    if((canNum >= eCANNUM) || (pRxMsg == (void *)0))
    {
        return(FALSE);
    }
    
    /*计算缓冲区未处理信息*/
    if(sCANCyUnDealNum[canNum] < 0xff)
    {
        sCANCyUnDealNum[canNum]++;
    }
    
    /*获得缓冲区写入点*/
    wrtPoint = sCANRcvCyBuf[canNum].WritePoint;
    
    /*缓冲区未满*/
    if((0 == sCANRcvCyBuf[canNum].FullFlag))
    {
        /*写ID*/
//        if(FLEXCAN_MSG_ID_STD == pRxMsg->id)
//        {
//            /*标准帧*/
//            //sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].ID = pRxMsg->msgId;
//            sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].ID = FLEXCAN_MSG_ID_STD;
//        }
//        else
//        {
//            /*扩展帧*/
//            //sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].ID = pRxMsg->msgId;
//            sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].ID = FLEXCAN_MSG_ID_EXT;
//        }
    	sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].ID = pRxMsg->msgId;
    	//sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].ID = pRxMsg->id;
        
        /*写长度*/
        sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].Len = pRxMsg->dataLen;
        //sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].Len = pRxMsg->length;
        
        /*写数据*/
        for (i = 0; i < sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].Len; i++)
        {
            sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].Data[i] = pRxMsg->data[i];
        }
        
        /*判断写缓冲指针是否超过最大值*/
        if(++sCANRcvCyBuf[canNum].WritePoint >= CAN_RCV_CYBUF)
        {
            sCANRcvCyBuf[canNum].WritePoint= 0;
        }
        
        /*判断读指针是否等于写指针*/
        if(sCANRcvCyBuf[canNum].WritePoint == sCANRcvCyBuf[canNum].ReadPoint)
        {
            sCANRcvCyBuf[canNum].FullFlag = 1;
        }
        
        result = TRUE;
    }
    /*缓冲区写满*/
    else
    {
        result = FALSE;
    }
    
    return(result);
}

/*=============================================================================================
** @Name      : static u8 CANReadRcvCyBuf(e_CANNUM canNum, t_CANMSG *pRcvMsg)
** @Input     : canNum:CAN通道号, *pRcvMsg:CAN信息结构体接收数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : CAN接收处理任务读CAN接收环形缓冲区信息出来处理
** @The notes :
**===========================================================================================*/
static u8 CANReadRcvCyBuf(e_CANNUM canNum, t_CANMSG *pRcvMsg)
{
    u8 result = 0;
    
    if((canNum >= eCANNUM) || (pRcvMsg == (void *)0))
    {
        return(FALSE);
    }
    
    /*有数据则把数据从缓冲区中读出来*/
    if((sCANRcvCyBuf[canNum].ReadPoint != sCANRcvCyBuf[canNum].WritePoint)
        || (sCANRcvCyBuf[canNum].FullFlag != 0))
    {
        *pRcvMsg = sCANRcvCyBuf[canNum].RcvBuf[sCANRcvCyBuf[canNum].ReadPoint];
    
        if(++sCANRcvCyBuf[canNum].ReadPoint >= CAN_RCV_CYBUF)
        {
            sCANRcvCyBuf[canNum].ReadPoint = 0;
        }
        
        sCANRcvCyBuf[canNum].FullFlag = 0;
        
        result = TRUE;
    }
    /*缓冲区无数据*/
    else 
    {
        result = FALSE;
    }
    
    return(result);
}
