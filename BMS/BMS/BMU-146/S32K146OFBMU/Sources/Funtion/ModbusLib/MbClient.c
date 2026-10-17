/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : MbClient.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : Modbus通信包客户端处理函数
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPUserAPI.h"
#include "MbInterface.h"
#include "MbUserDef.h"
#include "MbHandle.h"
#include "MbTriSend.h"
#include "MbQueue.h"
#include "MbClient.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
sWritevalueArr WritevalueArr[3];        //定义写操作数组

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u8 MbClientAddReqToQueue(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:请求报文,port:通道号
** @Output    : 标识添加新的请求报文是否成功
** @Function  : 往请求报文队列中添加新的请求报文
** @The notes :
**===============================================================================================*/
static u8 MbClientAddReqToQueue(MB_REQ_ADU *pMbReqAdu, u8 port);

/*=================================================================================================
** @Name      : static void MbClientPackAndPollRdWrReq(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:请求报文结构体,port:通道号
** @Output    : void
** @Function  : 封装client端读写请求到发送缓存区数组,并发送请求信息
** @The notes :
**===============================================================================================*/
static void MbClientPackAndPollRdWrReq(MB_REQ_ADU *pMbReqAdu, u8 port);

/*=================================================================================================
** @Name      : static void MbClientPackAndPollReadReq(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:请求报文结构体,port:通道号
** @Output    : void
** @Function  : 封装client端查询请求到发送缓存区数组,并发送请求信息
** @The notes :
**===============================================================================================*/
static void MbClientPackAndPollReadReq(MB_REQ_ADU *pMbReqAdu, u8 port);

/*=================================================================================================
** @Name      : static void MbClientPackAndPollWriteSingReq(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:请求报文结构体,port:通道号
** @Output    : void
** @Function  : 封装client端写单个寄存器请求到发送缓存区数组,并发送请求信息
** @The notes :
**===============================================================================================*/
static void MbClientPackAndPollWriteSingReq(MB_REQ_ADU *pMbReqAdu, u8 port);

/*=================================================================================================
** @Name      : static void MbClientPackAndPollWriteMultReq(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:请求报文结构体,port:通道号
** @Output    : void
** @Function  : 封装client端写多个寄存器请求到发送缓存区数组,并发送请求信息
** @The notes :
**===============================================================================================*/
static void MbClientPackAndPollWriteMultReq(MB_REQ_ADU *pMbReqAdu, u8 port);

/*=================================================================================================
** @Name      : static u8 MbCheckReqAnswerFinState(u8 port)
** @Input     : port:通道号
** @Output    : 请求读写任务应答完成状态:1:完成 2:未完成
** @Function  : 检测客户端(主机)请求读写任务的应答完成状态
** @The notes :
**===============================================================================================*/
static u8 MbCheckReqAnswerFinState(u8 port);

/*=================================================================================================
** @Name      : static void MbParseAndProcessReqAnswer(u8 port)
** @Input     : port:通道号
** @Output    : void
** @Function  : 客户端(主机)解析并处理来自从机的应答数据
** @The notes : 应答完成后执行
**===============================================================================================*/
static void MbParseAndProcessReqAnswer(u8 port);

/*=================================================================================================
** @Name      : static void MbClientStore01or02ReqAnswerData(u8 stationID, u8 fncCode, u8 port, u16 address, u8 *data, u8 len)
** @Input     : stationID:从站id,fncCode:功能码,port:通道号,address:起始地址,*data:接收到的数据(字节),len:数据字节长度
** @Output    : void
** @Function  : 客户端(主机)将01/02功能码的应答数据存入用户寄存器
** @The notes : 调用用户寄存器地址映射接口
**===============================================================================================*/
static void MbClientStore01or02ReqAnswerData(u8 stationID, u8 fncCode, u8 port, u16 address, u8 *data, u8 len);

/*=================================================================================================
** @Name      : static void MbParse03And04ReqAnswer(u16 *pData, u8 port)
** @Input     : *pData:接收到的应答数据,port:通道号
** @Output    : void
** @Function  : 客户端(主机)解析来自从机的03/04功能码应答数据,存入pData中
** @The notes :
**===============================================================================================*/
static void MbParse03And04ReqAnswer(u16 *pData, u8 port);

/*=================================================================================================
** @Name      : static void MbClientStore03or04ReqAnswerData(u8 stationID, u8 fncCode, u8 port, u16 address, u16 *data, u8 len)
** @Input     : stationID:从站id,fncCode:功能码,port:通道号,address:起始地址,*data:接收到的数据(字节),len:数据字节长度
** @Output    : void
** @Function  : 客户端(主机)将03/04功能码的应答数据存入用户寄存器
** @The notes : 调用用户寄存器地址映射接口
**===============================================================================================*/
static void MbClientStore03or04ReqAnswerData(u8 stationID, u8 fncCode, u8 port, u16 address, u16 *data, u8 len);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void MbClientTimerTask(void)
** @Input     : void
** @Output    : void
** @Function  : 客户端(主机)请求任务定时器
** @The notes : 1ms调用周期调用一次
**===============================================================================================*/
void MbClientTimerTask(void)
{
	u8 i = 0;
	
	for(i = 0; i < cMBStationNum; i++)
	{
		if(cModeSelect == 2)
		{
			if(modbusStation[i].clientOrServer == 1)
			{
				modbusStation[i].reqTaskTmrCnt++;
			}
		}
		else if(cModeSelect == 1)
		{
			modbusStation[i].reqTaskTmrCnt++;
		}
	}
}

/*=================================================================================================
** @Name      : u8 MbClientBuildReadReqAPI(u8 stationID, u8 fncCode, u16 address, u16 len, u8 port)
** @Input     : stationID:要读取的从站ID,fncCode:功能码(01读线圈/02读离散量输入/03读保持寄存器/04读输入寄存器),address:要读取的寄存器起始地址,len:要读取的寄存器数量长度,port:Modbus通道号
** @Output    : 创建请求插入队列结果 0:失败 1:成功
** @Function  : 客户端(主机)创建读请求并发送
** @The notes : 对应01/02/03/04功能码
**===============================================================================================*/
u8 MbClientBuildReadReqAPI(u8 stationID, u8 fncCode, u16 address, u16 len, u8 port)
{
    MB_REQ_ADU mbReqAdu;
    
    mbReqAdu.nodeId = stationID;
    mbReqAdu.function = fncCode;
    mbReqAdu.address = address;
    mbReqAdu.length = len;
    mbReqAdu.pData = 0;

    return(MbClientAddReqToQueue(&mbReqAdu, port));
}

/*=================================================================================================
** @Name      : u8 MbClientSendWriteSingReqAPI(u8 stationID, u8 fncCode, u16 address, u16 data, u8 port)
** @Input     : stationID:要写入的从站ID,fncCode:功能码(05写单线圈,06写单个寄存器),address:要写入的寄存器地址,data:要写入的数据,port:Modbus通道号
** @Output    : 创建请求插入队列结果:0:失败 1:成功
** @Function  : 客户端(主机)创建向指定单个线圈或寄存器写入给定值的写请求并发送
** @The notes : 对应05/06功能码
**===============================================================================================*/
u8 MbClientBuildWriteSingReqAPI(u8 stationID, u8 fncCode, u16 address, u16 data, u8 port)
{
    MB_REQ_ADU mbReqAdu;
    
    mbReqAdu.nodeId = stationID;
    mbReqAdu.function = fncCode;
    mbReqAdu.address = address;
    mbReqAdu.Data = data;
    //mbReqAdu.pData = &data;

    return(MbClientAddReqToQueue(&mbReqAdu, port));
}

/*=================================================================================================
** @Name      : u8 MbClientSendWriteMultReqAPI(u8 stationID, u16 address, u16 *data, u8 len, u8 port)
** @Input     : stationID:要写入的从站ID,address:要写入的寄存器起始地址,*data:要写入的数据序列首地址指针,len:要写的寄存器数量长度,port:Modbus通道号
** @Output    : 创建请求插入队列结果:0:失败 1:成功
** @Function  : 客户端(主机)创建向连续的寄存器中依次写入多个数据值的写请求并发送
** @The notes : 对应16功能码
**===============================================================================================*/
u8 MbClientBuildWriteMultReqAPI(u8 stationID, u16 address, u16 *data, u8 len, u8 port)
{
    MB_REQ_ADU mbReqAdu;
    
    mbReqAdu.nodeId = stationID;
    mbReqAdu.function = MODBUS_WRITE_MULT_REG;
    mbReqAdu.address = address;
    mbReqAdu.length = len;
    mbReqAdu.pData = data;

    return(MbClientAddReqToQueue(&mbReqAdu, port));
}

/*=================================================================================================
** @Name      : void MbClientReqAndRcvHandleAPI(u8 port)
** @Input     : port:Modbus通道号
** @Output    : void
** @Function  : 实现客户端(主机)的读写请求和接收处理
** @The notes :
**===============================================================================================*/
void MbClientReqAndRcvHandleAPI(u8 port)
{
	u8 j = 0;
    static u8 outFlag = 0;
	static MB_REQ_ADU pMbReqAdu[5]; //存放从请求队列中取出的当前请求任务
    
	switch(modbusStation[port].mbstate)
	{
		//主站空闲状态
		case Leisure:
			//判断请求队列是否有请求
			if(MBQueGetUnitNum(modbusStation[port].Req_Queue))
			{
				//从请求队列中取出新的请求任务
				outFlag = MBQueOut(modbusStation[port].Req_Queue, &pMbReqAdu[port]);
				
				//取队列成功，进入发送请求状态
				if(outFlag)
				{
					modbusStation[port].mbstate = SendReq;
					modbusStation[port].updataFlag = HasNotUpdate;
				}
				//取队列不成功，还在空闲状态
				else
				{
					modbusStation[port].mbstate = Leisure;
				}
				MbClientStartReqPrcTask(port);
			}
		    else
		    {
		        (void)MbSetTrigFlagStateAPI(port, 0);
		    }
			break;
			
		//发送请求状态
		case SendReq:
			
			//封装和发送请求读写信息
			MbClientPackAndPollRdWrReq(&pMbReqAdu[port],port);
			
			if((outFlag) && (pMbReqAdu[port].function == MODBUS_WRITE_MULT_REG))
			{
		    	for(j = 0; j < 3; j++) 
			    {
				    if(pMbReqAdu[port].pData == WritevalueArr[j].valueArr)
				    {
		                WritevalueArr[j].putinflag = 0;
		                break; 
				    }
	            }
			}
			
			modbusStation[port].stationID = pMbReqAdu[port].nodeId;
			
			//开始超时定时
			modbusStation[port].reqTaskTmrCnt = 0;
			modbusStation[port].reqTaskStartTmr = modbusStation[port].reqTaskTmrCnt;
			
			//进入应答等待状态
			modbusStation[port].mbstate = RspWait;
			MbClientStartReqPrcTask(port);
			break;
			
		//应答等待状态
		case RspWait:
			
			//判断请求查询任务应答是否完成
			modbusStation[port].poll_control = MbCheckReqAnswerFinState(port);
			
			//完成则转到解析处理状态
			if(modbusStation[port].poll_control == (u8)CMPLT_COMPLETE)
			{
				modbusStation[port].mbstate = RspParseAndPro;
				MbClientStartReqPrcTask(port);
				break;
			}
			
			//判断请求查询是否超时，是则置主节点空闲状态
			MbDisableIRQHook();
			if((modbusStation[port].reqTaskTmrCnt - modbusStation[port].reqTaskStartTmr) >= cMBPollTimeOut)
			{
				modbusStation[port].updataFlag = DoNotUpdateForNoRsp;
				modbusStation[port].mbstate = Leisure;
				
				/*等待应答超时后用户处理*/
				MbClientReqWaitOverTimeUserHook(pMbReqAdu[port].nodeId, pMbReqAdu[port].function, port, pMbReqAdu[port].address);
			}
			MbDisableIRQHook();
			
			MbClientStartReqPrcTask(port);
			break;
			
		//解析处理状态
		case RspParseAndPro:
			
			//解析并处理来自从机的应答数据
			MbParseAndProcessReqAnswer(port);
			
			//清空接收缓冲区
			MbClrStationRcvBuffer(modbusStation[port].recPort);
			
			//进入请求帧间隔设置状态
			modbusStation[port].mbstate = FrameSpaceSet;
			MbClientStartReqPrcTask(port);
			break;
			
		//请求帧间隔设置状态
		case FrameSpaceSet:
			/*禁止中断*/
			MbDisableIRQHook();
			
			//如果时间到设置的间隔时间，就进入空闲状态
			if((modbusStation[port].reqTaskTmrCnt - modbusStation[port].reqTaskStartTmr) >= (u32)cMBPollDelay)
			{            
				modbusStation[port].mbstate = Leisure;
			}
			MbDisableIRQHook();
			
			MbClientStartReqPrcTask(port);
			break;
			
		default:
			break;
	}
}

/*=================================================================================================
 ** @Name      : u8 MbClientGetRcvUpdateFlagAPI(u8 port)
 ** @Input     : port:Modbus通道号
 ** @Output    : 接收应答数据更新标志 1:已更新,0:未更新
 ** @Function  : 获得客户端(主机)接收应答数据是否已更新到用户指定的空间中的标志
 ** @The notes : 客户端(主机)接收服务端(主机)应答完成标志
 **===============================================================================================*/
 u8 MbClientGetRcvUpdateFlagAPI(u8 port)
{
    return(modbusStation[port].updataFlag);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
 ** @Name      : static u8 MbClientAddReqToQueue(MB_REQ_ADU *pMbReqAdu, u8 port)
 ** @Input     : *pMbReqAdu:请求报文,port:通道号
 ** @Output    : 标识添加新的请求报文是否成功
 ** @Function  : 往请求报文队列中添加新的请求报文
 ** @The notes :
 **===============================================================================================*/
 static u8 MbClientAddReqToQueue(MB_REQ_ADU *pMbReqAdu, u8 port)
 {
 	u8 i = 0;
 	u8 j = 0;
 	MB_REQ_ADU mbReqAdu;
 	u8 addflag = 0;//标志往队列中添加数据是否成功

 	//初始指针为数组首地址
 	for(j = 0; j < 3; j++)
 	{
 	    WritevalueArr[j].valueArr = MbGetWrBufferArrayHook(j);
 	}

 	switch(pMbReqAdu->function)
 	{
 		//读线圈或读输入量状态
 		case MODBUS_READ_COILS:
 		case MODBUS_READ_DISCRETEINPUTS:

 			//判断请求数量是否在1-2000之间
 			if((pMbReqAdu->length > 0) && (pMbReqAdu->length <= 2000))
 			{
 				//将新的请求报文压入请求报文队列；
 				addflag = MBQueIn(modbusStation[port].Req_Queue, pMbReqAdu);
 				break;
 			}

 			//请求数量超出允许范围，返回错误数量信息
 			else
 			{
 				addflag = WRONGNUM;
 				return(addflag);
 			}

 		//读保持寄存器和输入寄存器
 		case MODBUS_READ_HOLDREGISTERS:
 		case MODBUS_READ_INPUTREGISTERS:

 			//请求数量在1-125之间
 			if((pMbReqAdu->length > 0) && (pMbReqAdu->length < 126))
 			{
 				//将新的请求报文压入请求报文队列；
 				addflag = MBQueIn(modbusStation[port].Req_Queue, pMbReqAdu);
 				break;
 			}

 			//请求数量超出允许范围，返回错误数量信息
 			else
 			{
 				addflag = WRONGNUM;
 				return(addflag);
 			}

 		//预置单线圈和单寄存器
 		case MODBUS_WRITE_COIL:
 		case MODBUS_WRITE_REGISTER:
 			mbReqAdu.nodeId = pMbReqAdu->nodeId;
 			mbReqAdu.function = pMbReqAdu->function;
 			mbReqAdu.address = pMbReqAdu->address;
 			mbReqAdu.Data = pMbReqAdu->Data;

 			addflag = MBQueIn(modbusStation[port].Req_Queue, &mbReqAdu);
 			break;

 		//预置多寄存器
 		case MODBUS_WRITE_MULT_REG:

 			//请求写寄存器数量在1-123之间
 			if((pMbReqAdu->length > 0) && (pMbReqAdu->length < MAX_OUT_NUM))
 			{
 				mbReqAdu.nodeId = pMbReqAdu->nodeId;
 				mbReqAdu.function = pMbReqAdu->function;
 				mbReqAdu.address = pMbReqAdu->address;
 				mbReqAdu.length = pMbReqAdu->length;

 				//判断写寄存器是否有空闲
 			    for(j = 0; j < 3; j++)
 			    {
 			        if(WritevalueArr[j].putinflag == 0)
 			        {
 			            break;
 			        }

 		    	}

 			    //有空闲，把要写入的数据存入写寄存器，请求压队列
 			    if(j < 3)
 			    {
 			        for(i = 0; i < pMbReqAdu->length; i++)
 				    {
 				    	WritevalueArr[j].valueArr[i] = *(pMbReqAdu->pData);
 					    (pMbReqAdu->pData)++;
 				    }
 				    mbReqAdu.pData = WritevalueArr[j].valueArr;
 			    	addflag = MBQueIn(modbusStation[port].Req_Queue, &mbReqAdu);//将新的请求报文压入请求报文队列；
 			        if(addflag == 1)
                     {
                         WritevalueArr[j].putinflag = 1;
                     }
 			    }
 			    //无空闲，不允许压队列
 		    	else
 			    {
 			        addflag = FALSE;
 			    }
 				break;
 			}
 			//请求数量超出允许范围，返回错误数量信息
 			else
 			{
 				addflag = WRONGNUM;
 				return(addflag);
 			}

 		//功能码错误返回错误的功能码信息
 		default:
 			addflag = WRONGFUNCTION;
 			return(addflag);
 	}

 	//压入成功
 	if(addflag)
 	{
 		addflag = ADDSUCESSFULLY;
 	}
 	//压入不成功，返回压入不成功信息
 	else
 	{
 		addflag = ADDUNSUCESSFULLY;
 	}

 	//尝试启动请求任务
 	MbClientStartReqPrcTask(port);

 	return(addflag);
 }

 /*=================================================================================================
 ** @Name      : static void MbClientPackAndPollRdWrReq(MB_REQ_ADU *pMbReqAdu, u8 port)
 ** @Input     : *pMbReqAdu:请求报文结构体,port:通道号
 ** @Output    : void
 ** @Function  : 封装client端读写请求到发送缓存区数组,并发送请求信息
 ** @The notes :
 **===============================================================================================*/
 static void MbClientPackAndPollRdWrReq(MB_REQ_ADU *pMbReqAdu, u8 port)
 {
 	//功能码不同，封装到发送缓冲区中不同位置上的内容不同
 	switch (pMbReqAdu->function)
 	{
 		//读请求
 		case MODBUS_READ_COILS:
 		case MODBUS_READ_DISCRETEINPUTS:
 		case MODBUS_READ_HOLDREGISTERS:
 		case MODBUS_READ_INPUTREGISTERS:

 			//封装读请求到发送缓冲区并发送
 			MbClientPackAndPollReadReq(pMbReqAdu, port);
 			break;

 		//预置单线圈和单寄存器
 		case MODBUS_WRITE_COIL:
 		case MODBUS_WRITE_REGISTER:

 			//封装请求到发送缓冲区并发送
 			MbClientPackAndPollWriteSingReq(pMbReqAdu, port);
 			break;

 		//预置多寄存器
 		case MODBUS_WRITE_MULT_REG:

 		    //封装请求到发送缓冲区并发送
 			MbClientPackAndPollWriteMultReq(pMbReqAdu, port);
 			break;

 		default:
 			break;
 	}
 }

 /*=================================================================================================
 ** @Name      : static void MbClientPackAndPollReadReq(MB_REQ_ADU *pMbReqAdu, u8 port)
 ** @Input     : *pMbReqAdu:请求报文结构体,port:通道号
 ** @Output    : void
 ** @Function  : 封装client端查询请求到发送缓存区数组,并发送请求信息
 ** @The notes :
 **===============================================================================================*/
 static void MbClientPackAndPollReadReq(MB_REQ_ADU *pMbReqAdu, u8 port)
 {
 	u8 length = 0;
 	u16 crc = 0;

 	//封装目标从站id, 功能码，和寄存器起始地址
 	modbusStation[port].TX_Buffer[0] = pMbReqAdu->nodeId;
 	modbusStation[port].TX_Buffer[1] = pMbReqAdu->function;
 	modbusStation[port].TX_Buffer[2] = (u8)((pMbReqAdu->address >> 8) & 0xff) ;	//地址域高8位
 	modbusStation[port].TX_Buffer[3] = (u8)((pMbReqAdu->address) & 0xff);		//地址域低8位

 	//封装要读取的寄存器数量
 	modbusStation[port].TX_Buffer[4] = (u8)((pMbReqAdu->length >> 8) & 0xff);
 	modbusStation[port].TX_Buffer[5] = (u8)((pMbReqAdu->length) & 0xff);

 	//计算要校验的ModBus数据字节数(不包括校验码)
 	length = 6;

 	//获得校验码并封装进发送缓冲区(低字节在前)
 	crc = MbCalcCRC16Code(modbusStation[port].TX_Buffer, length) ;

 	//低位在前先发
 	modbusStation[port].TX_Buffer[length++] = (u8)((crc >> 8) & 0xff);
 	modbusStation[port].TX_Buffer[length++] = (u8)((crc) & 0xff);

 	//高位在前先发
 	//modbusStation[sciNum].TX_Buffer[length++] = (u8)((crc) & 0xff);
 	//modbusStation[sciNum].TX_Buffer[length++] = (u8)((crc >> 8) & 0xff);

 	//将发送缓冲区中的报文发送出去
 	ModbusSendDataUserHook(modbusStation[port].TX_Buffer, length, modbusStation[port].sendPort);
 }

 /*=================================================================================================
 ** @Name      : static void MbClientPackAndPollWriteSingReq(MB_REQ_ADU *pMbReqAdu, u8 port)
 ** @Input     : *pMbReqAdu:请求报文结构体,port:通道号
 ** @Output    : void
 ** @Function  : 封装client端写单个寄存器请求到发送缓存区数组,并发送请求信息
 ** @The notes :
 **===============================================================================================*/
 static void MbClientPackAndPollWriteSingReq(MB_REQ_ADU *pMbReqAdu, u8 port)
 {
 	u8 length = 0;
 	u16 crc = 0;

 	ModbusStation *momodbusStation;
    momodbusStation = &modbusStation[port];

 	//封装目标从站id, 功能码，和寄存器起始地址
    momodbusStation->TX_Buffer[0] = pMbReqAdu->nodeId;
 	momodbusStation->TX_Buffer[1] = pMbReqAdu->function;
    momodbusStation->TX_Buffer[2] = (u8)((pMbReqAdu->address >> 8) & 0xff);//地址域高8位
 	momodbusStation->TX_Buffer[3] = (u8)((pMbReqAdu->address) & 0xff);	   //地址域低8位

 	//预置单线圈值时，输入为0，封装
 	if(pMbReqAdu->function == MODBUS_WRITE_COIL)
    {
         if(pMbReqAdu->Data == 0)
         {
             pMbReqAdu->Data = (u16)COIL_OFF;//请求线圈处于off状态
         }
         else
         {
             pMbReqAdu->Data = (u16)COIL_ON;//请求线圈处于on状态
         }
     }

     //为预置寄存器时，直接封装进发送缓冲区
 	else
 	{
         ;
 	}
 	momodbusStation->TX_Buffer[4] = (u8)((pMbReqAdu->Data >> 8) & 0xff);
    momodbusStation->TX_Buffer[5] = (u8)((pMbReqAdu->Data) & 0xff);

 	//计算要校验的ModBus数据字节数
 	length = 6;

 	//获得校验码并封装进发送缓冲区(低字节在前)
 	crc = MbCalcCRC16Code(modbusStation[port].TX_Buffer, length);

 	//记录校验码(低位在前先发)
 	momodbusStation->TX_Buffer[length++] = (u8)((crc >> 8) & 0xff);
 	momodbusStation->TX_Buffer[length++] = (u8)((crc) & 0xff);

 	//记录校验码(高位在前先发)
 	//momodbusStation->TX_Buffer[length++] = (u8)((crc) & 0xff);
 	//momodbusStation->TX_Buffer[length++] = (u8)((crc >> 8) & 0xff);

 	//将发送缓冲区中的报文发送出去
 	ModbusSendDataUserHook(modbusStation[port].TX_Buffer, length, modbusStation[port].sendPort);
 }

 /*=================================================================================================
 ** @Name      : static void MbClientPackAndPollWriteMultReq(MB_REQ_ADU *pMbReqAdu, u8 port)
 ** @Input     : *pMbReqAdu:请求报文结构体,port:通道号
 ** @Output    : void
 ** @Function  : 封装client端写多个寄存器请求到发送缓存区数组,并发送请求信息
 ** @The notes :
 **===============================================================================================*/
 static void MbClientPackAndPollWriteMultReq(MB_REQ_ADU *pMbReqAdu, u8 port)
 {
 	u8 i = 0;
 	u8 index = 0;
 	u8 length = 0;
 	u16 crc = 0;
 	u16 *data;

 	data = pMbReqAdu->pData;

 	//封装目标从站id, 功能码，和寄存器起始地址
 	modbusStation[port].TX_Buffer[0] = pMbReqAdu->nodeId;
 	modbusStation[port].TX_Buffer[1] = pMbReqAdu->function;
 	modbusStation[port].TX_Buffer[2] = (u8)((pMbReqAdu->address >> 8) & 0xff);	//地址域高8位
 	modbusStation[port].TX_Buffer[3] = (u8)((pMbReqAdu->address) & 0xff);		//地址域低8位

 	//封装要写入的寄存器数量
 	modbusStation[port].TX_Buffer[4] = (u8)((pMbReqAdu->length >> 8) & 0xff);
 	modbusStation[port].TX_Buffer[5] = (u8)((pMbReqAdu->length) & 0xff);

 	//封装要写入的寄存器值所占用的字节数
 	modbusStation[port].TX_Buffer[6] = (u8)((pMbReqAdu->length) << 1);

 	index = 7;
 	//封装要写入的寄存器值
 	for(i = 0; i < pMbReqAdu->length; i++)
 	{
 		modbusStation[port].TX_Buffer[index++] = (*(pMbReqAdu->pData)) >> 8;
 		modbusStation[port].TX_Buffer[index++] = (*(pMbReqAdu->pData)) & 0xff;
 		(pMbReqAdu->pData)++;
 	}
 	pMbReqAdu->pData = data;

 	//计算要校验的ModBus数据字节数
 	length = modbusStation[port].TX_Buffer[6] + 7;

 	//获得校验码并封装进发送缓冲区(低字节在前)
 	crc = MbCalcCRC16Code(modbusStation[port].TX_Buffer, length);

 	//记录校验码(低位在前先发)
 	modbusStation[port].TX_Buffer[length++] = (u8)((crc >> 8) & 0xff);
 	modbusStation[port].TX_Buffer[length++] = (u8)((crc) & 0xff);

 	//记录校验码(高位在前先发)
 	//modbusStation[port].TX_Buffer[length++] = (u8)((crc) & 0xff);
 	//modbusStation[port].TX_Buffer[length++] = (u8)((crc >> 8) & 0xff);

 	//将发送缓冲区中的报文发送出去
 	ModbusSendDataUserHook(modbusStation[port].TX_Buffer, length, modbusStation[port].sendPort);
 }

 /*=================================================================================================
 ** @Name      : static u8 MbCheckReqAnswerFinState(u8 port)
 ** @Input     : port:通道号
 ** @Output    : 请求读写任务应答完成状态:1:完成 2:未完成
 ** @Function  : 检测客户端(主机)请求读写任务的应答完成状态
 ** @The notes :
 **===============================================================================================*/
 static u8 MbCheckReqAnswerFinState(u8 port)
 {
 	u16 crc = 0;
 	u8 target = 0;
 	u8 fonction = 0;
 	u8 length = 0;

 	//获取原请求报文中要操作的功能码
 	fonction = modbusStation[port].TX_Buffer[1];
 	length = modbusStation[port].TX_Buffer[5] * 2;

     //判断应答是否为本请求的应答
 	if((modbusStation[port].stationID == modbusStation[port].RX_Buffer[0])
 	    && (fonction == (modbusStation[port].RX_Buffer[1] & 0x7f)))
 	{
 		//如果为正常应答
 		if(!(modbusStation[port].RX_Buffer[1] & 0x80))
 		{
 			switch(modbusStation[port].RX_Buffer[1])
 			{
 				//读请求应答
 				case MODBUS_READ_COILS:
 				case MODBUS_READ_DISCRETEINPUTS:

 				    //对以上读操作响应报文长度为RX_Buffer[2] + 3个字节，未包含CRC
 					target = (u8)(modbusStation[port].RX_Buffer[2] + 3);
 					break;

 				case MODBUS_READ_HOLDREGISTERS:
 				case MODBUS_READ_INPUTREGISTERS:

                     //判断长度是否正确
                     if(length == modbusStation[port].RX_Buffer[2])
 					{
 					    target = (u8)(modbusStation[port].RX_Buffer[2] + 3);
 					}
 					else
 					{
 					    return(CMPLT_NO_HOPE);
 					}
 					break;

 				//写请求应答
 				case MODBUS_WRITE_COIL:
 				case MODBUS_WRITE_REGISTER:

 				    if((modbusStation[port].TX_Buffer[2] == modbusStation[port].RX_Buffer[2])
 				        && (modbusStation[port].TX_Buffer[3] == modbusStation[port].RX_Buffer[3]))
 				    {
 				        target = 6;
 				    }
 				    else
 				    {
 				        return(CMPLT_NO_HOPE);
 				    }
 					break;

 				case MODBUS_WRITE_MULT_REG:
 				    if((modbusStation[port].TX_Buffer[2] == modbusStation[port].RX_Buffer[2])
 				        && (modbusStation[port].TX_Buffer[3] == modbusStation[port].RX_Buffer[3])
 				        && (modbusStation[port].TX_Buffer[4] == modbusStation[port].RX_Buffer[4])
 				        && (modbusStation[port].TX_Buffer[5] == modbusStation[port].RX_Buffer[5]))
 				    {
 				        target = 6;
 				    }
 				    else
 				    {
 				        return(CMPLT_NO_HOPE);
 				    }
 					break;

 				default:
 					break;
 			}
 		}
 		//异常应答
 		else
 		{
 			//异常应答 1字节地址 1字节功能码 1字节异常代码，未包含CRC
 			target = 3;
 		}

 		//判断接收到消息的字节个数和理论应答报文字节个数是否相等
 		if(modbusStation[port].RX_Count == (target + 2))
 		{
 			//计算得到校验码(低字节在前)
 			crc = MbCalcCRC16Code(modbusStation[port].RX_Buffer, target);

 			//判断接收到的CRC与计算得到的CRC是否相等(CRC高位在前或低位在前均可以),相等返回应答完成
 			if(((modbusStation[port].RX_Buffer[modbusStation[port].RX_Count - 2] == ((u8)(crc >> 8) & 0xff))
 				&& ((u8)modbusStation[port].RX_Buffer[modbusStation[port].RX_Count -1] == (u8)(crc & 0xff)))
 			    || ((modbusStation[port].RX_Buffer[modbusStation[port].RX_Count - 2] == (u8)(crc & 0xff))
 				&& ((u8)modbusStation[port].RX_Buffer[modbusStation[port].RX_Count -1] == ((u8)(crc >> 8) & 0xff))))
 			{
 				return(CMPLT_COMPLETE);
 			}

 			//不相等返回应答未完成
 			else
 			{
 				return(CMPLT_NO_HOPE);
 			}
 		}

 		//字节个数不相等，返回应答未完成
 		else
 		{
 			return(CMPLT_NO_HOPE);
 		}
 	}
 	//接收从站ID和目标从站ID不相同，返回应答未完成
 	else
 	{
 		return(CMPLT_NO_HOPE);
 	}
 }

 /*=================================================================================================
 ** @Name      : static void MbParseAndProcessReqAnswer(u8 port)
 ** @Input     : port:通道号
 ** @Output    : void
 ** @Function  : 客户端(主机)解析并处理来自从机的应答数据
 ** @The notes : 应答完成后执行
 **===============================================================================================*/
 static void MbParseAndProcessReqAnswer(u8 port)
 {
 	u8 	function = 0;
 	u8 	byteCnt = 0;
 	u16 address = 0;
 	u16 highTemp = 0;
 	u16	lowTemp = 0;
 	u16	data[125] = {0};
 	u8  coildada[250] = {0};
 	u8  i = 0;

 	//获取原请求报文中要操作的寄存器首地址
 	highTemp = (u16)(modbusStation[port].TX_Buffer[2]);
 	lowTemp = (u16)(modbusStation[port].TX_Buffer[3]);
 	address = (highTemp << 8) | (lowTemp);

 	//获取应答数据中的功能码
 	function = modbusStation[port].RX_Buffer[1];

 	//正常应答
 	if(!(modbusStation[port].RX_Buffer[1] & 0x80))
 	{
 		switch(function)
 		{
 			//读请求查询的应答处理
 			case MODBUS_READ_COILS :
 			case MODBUS_READ_DISCRETEINPUTS :

 				//解析应答数据，将数据存入数组coildada[]中
 				byteCnt = modbusStation[port].RX_Buffer[2];
 				for(i = 0; i < byteCnt; i++)
 				{
 					coildada[i] = modbusStation[port].RX_Buffer[i + 3];
 				}

                 //查找数据空间首地址,并将数据存入地址中
 				MbClientStore01or02ReqAnswerData(modbusStation[port].RX_Buffer[0], function, port, address, coildada, byteCnt);
 				break;

 			case MODBUS_READ_INPUTREGISTERS:
 			case MODBUS_READ_HOLDREGISTERS:
 				byteCnt = modbusStation[port].RX_Buffer[2] >> 1;

 				//解析应答数据，将数据存入数组data[]中
 				MbParse03And04ReqAnswer(data, port);

 				//查找数据空间首地址,并将数据存入地址中
 				MbClientStore03or04ReqAnswerData(modbusStation[port].RX_Buffer[0], function, port, address, data, byteCnt);
 				break;

 			default:
 				break;
 		}

 		//通知用户写操作已正常应答
 		modbusStation[port].updataFlag = HasUpdate;

 		//提供给用户使用的接口
 		MbClientRcvAnswerUserHook(modbusStation[port].RX_Buffer[0], function, port, address);
 	}
 	else
 	{
 		// 代表应答已完成，但为异常应答
 		modbusStation[port].updataFlag  = DoNotUpdateForExRsp;
 	}
 }

 /*=================================================================================================
 ** @Name      : static void MbClientStore01or02ReqAnswerData(u8 stationID, u8 fncCode, u8 port, u16 address, u8 *data, u8 len)
 ** @Input     : stationID:从站id,fncCode:功能码,port:通道号,address:起始地址,*data:接收到的数据(字节),len:数据字节长度
 ** @Output    : void
 ** @Function  : 客户端(主机)将01/02功能码的应答数据存入用户寄存器
 ** @The notes : 调用用户寄存器地址映射接口
 **===============================================================================================*/
 static void MbClientStore01or02ReqAnswerData(u8 stationID, u8 fncCode, u8 port, u16 address, u8 *data, u8 len)
 {
    u16 i = 0;
    u16* pData = (void*)0;

    //查找数据空间首地址
 	pData = MbClientFindDataPtrHook(stationID, fncCode, port, address);

 	//向用户空间写入读取到的数据
 	if(pData != 0)
 	{
 		for(i = 0; i < len; i++)
 		{
 			*pData = (u16)data[i];
 			pData++;
 		}
 	}
 }

 /*=================================================================================================
 ** @Name      : static void MbParse03And04ReqAnswer(u16 *pData, u8 port)
 ** @Input     : *pData:接收到的应答数据,port:通道号
 ** @Output    : void
 ** @Function  : 客户端(主机)解析来自从机的03/04功能码应答数据,存入pData中
 ** @The notes :
 **===============================================================================================*/
 static void MbParse03And04ReqAnswer(u16 *pData, u8 port)
 {
 	u8	i = 0;
 	u8	arrIndex = 0;
 	u8	byteCnt = 0;
 	u16	highTemp = 0;
 	u16	lowTemp = 0;

 	//数据个数等于字节数除以2
 	byteCnt = modbusStation[port].RX_Buffer[2] >> 1;
 	arrIndex = 3;

 	//将数据存入pData指向的空间中
 	for(i = 0; i < byteCnt; i++)
 	{
 		highTemp = (u16)(modbusStation[port].RX_Buffer[arrIndex++]);
 		lowTemp = (u16)(modbusStation[port].RX_Buffer[arrIndex++]);
 		pData[i] = (highTemp << 8) | (lowTemp);
 	}
 }

 /*=================================================================================================
 ** @Name      : static void MbClientStore03or04ReqAnswerData(u8 stationID, u8 fncCode, u8 port, u16 address, u16 *data, u8 len)
 ** @Input     : stationID:从站id,fncCode:功能码,port:通道号,address:起始地址,*data:接收到的数据(字节),len:数据字节长度
 ** @Output    : void
 ** @Function  : 客户端(主机)将03/04功能码的应答数据存入用户寄存器
 ** @The notes : 调用用户寄存器地址映射接口
 **===============================================================================================*/
 static void MbClientStore03or04ReqAnswerData(u8 stationID, u8 fncCode, u8 port, u16 address, u16 *data, u8 len)
 {
    u16 i = 0;
    u16* pData = (void*)0;

    //查找数据空间首地址
 	pData = MbClientFindDataPtrHook(stationID, fncCode, port, address);

 	//向用户空间写入读取到的数据
 	if(pData != 0)
 	{
 		for(i = 0; i < len; i++)
 		{
 			*pData = (u16)data[i];
 			pData++;
 		}
 	}
 }
