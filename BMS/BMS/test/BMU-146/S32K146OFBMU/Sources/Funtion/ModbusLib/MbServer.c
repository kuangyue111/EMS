/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : MbServer.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : Modbus通信包服务端处理函数
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPUserAPI.h"
#include "MbInterface.h"
#include "MbUserDef.h"
#include "MbHandle.h"
#include "MbServer.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u8 MbServerReqAnswerHandle(u8 stationID, u8 port, u8 *pData, u8 Length)
** @Output    : stationID:请求的从站ID,port:Modbus通道号,data:接收到的请求数据,Length:数据长度
** @Function  : 服务端(从站)的主处理函数
** @The notes : 请求应答处理
**===============================================================================================*/
static u8 MbServerReqAnswerHandle(u8 stationID, u8 port, u8 *pData, u8 Length);

/*=================================================================================================
** @Name      : static u8 MbServerCheckRcvMsg(u8 *data, u8 length, u8 port)
** @Input     : *data:接收到的报文指针,length:报文字节长度,port:通道号
** @Output    : 校验结果:1:接收完成 2:接收未完成
** @Function  : 服务器(从机)校验接收到的报文是否无误
** @The notes :
**===============================================================================================*/
static u8 MbServerCheckRcvMsg(u8 *data, u8 length, u8 port);

/*=================================================================================================
** @Name      : static void MbServerParseRcvMsg(u8 *pData, MB_REQ_ADU* pMbReqAdu)
** @Input     : *pData:接收到的报文数据,*pMbReqAdu:解析出的请求报文
** @Output    : viod
** @Function  : 根据功能码解析接收到的请求报文
** @The notes : 从接收缓冲区解析到请求报文记录区
**===============================================================================================*/
static void MbServerParseRcvMsg(u8 *pData, MB_REQ_ADU* pMbReqAdu);

/*=================================================================================================
** @Name      : static u8 MbServerCheckAnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:解析后的请求报文,port:通道号
** @Output    : 应该作出何种应答:0:正常应答,其它:异常应答
** @Function  : 检测从机应作出的应答响应类型
** @The notes : 判断是正常应答还是异常应答
**===============================================================================================*/
static u8 MbServerCheckAnswerType(MB_REQ_ADU *pMbReqAdu, u8 port);

/*=================================================================================================
** @Name      : static void MbServerSelectAnswerMsg(u8 rspType, u8 *pData, u8 port)
** @Input     : rspType:应答种类(0:正常应答,其它:异常应答),*pData:接收数据,port:通道号
** @Output    : void
** @Function  : 根据响应标识作出具体应答,发送应答响应请求报文
** @The notes :
**===============================================================================================*/
static void MbServerSelectAnswerMsg(u8 rspType, u8 *pData, u8 port);

/*=================================================================================================
** @Name      : static void MbServerBuildAnswerPacket(u8 *pData, u8 length, u8 port)
** @Input     : pData:指向不包含效验码CRC的Modbus消息帧的指针,length:不包含效验码CRC的Modbus消息帧字节数,port:发送通道号
** @Output    : void
** @Function  : 服务端(从机)创建和发送应答数据包(发送前将效验位放入应答帧中)
** @The notes : 因封装效验码,length和pDate值在程序中改变
**===============================================================================================*/
static void MbServerBuildAnswerPacket(u8 *pData, u8 length, u8 port);

/*=================================================================================================
** @Name      : static void MbServerAnswerExceptionMsg(u8 *pData, u8 exception, u8 port)
** @Input     : pData:要答复的消息帧数据,exception:描述错误信息的异常代码,port:发送通道号
** @Output    : void
** @Function  : 服务端(从机)创建和发送异常应答报文
** @The notes :
**===============================================================================================*/
static void MbServerAnswerExceptionMsg(u8 *pData, u8 exception, u8 port);

/*=================================================================================================
** @Name      : static u8 mbPrcFncCode01And02Msg(MB_REQ_ADU* pMbReqAdu,u8 sciNum)
** @Input     : *pMbReqAdu:解析出的请求报文,port:通道号
** @Output    : 应该作出何种应答:0:正常应答,其它:异常应答
** @Function  : 检测功能码为01和02类别的请求需作出的应答类型
** @The notes : 需要正常应答则获取相应寄存器地址
**===============================================================================================*/
static u8 MbCheck01And02AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port);

/*=================================================================================================
** @Name      : static u8 MbCheck03And04AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:解析出的请求报文,port:通道号
** @Output    : 应该作出何种应答:0:正常应答,其它:异常应答
** @Function  : 检测功能码为03和04类别的请求需作出的应答类型
** @The notes : 需要正常应答则获取相应寄存器地址
**===============================================================================================*/
static u8 MbCheck03And04AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port);

/*=================================================================================================
** @Name      : static u8 MbCheck03And04AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:解析出的请求报文,port:通道号
** @Output    : 应该作出何种应答:0:正常应答,其它:异常应答
** @Function  : 检测功能码为05和06类别的请求需作出的应答类型
** @The notes : 需要正常应答则获取相应寄存器地址
**===============================================================================================*/
static u8 MbCheck05And06AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port);

/*=================================================================================================
** @Name      : static u8 MbCheckFunct16AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:解析出的请求报文,port:通道号
** @Output    : 应该作出何种应答:0:正常应答,其它:异常应答
** @Function  : 检测功能码为16类别的请求需作出的应答类型
** @The notes : 需要正常应答则获取相应寄存器地址
**===============================================================================================*/
static u8 MbCheckFunct16AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port);

/*=================================================================================================
** @Name      : static u8 MbCheckOverFlowTxBuffer(MB_REQ_ADU *pMbReqAdu, u8 dataLen)
** @Input     : *pMbReqAdu:解析出的请求报文,dataLen:要发送的字节数
** @Output    : 检测结果 0:未溢出 1:溢出
** @Function  : 判断接收/发送缓冲区是否溢出
** @The notes :
**===============================================================================================*/
static u8 MbCheckOverFlowTxBuffer(MB_REQ_ADU *pMbReqAdu, u8 dataLen);

/*=================================================================================================
** @Name      : static u8 MbCalcByteFromBitLength(u16 coils)
** @Input     : coils:要读取的逻辑线圈或者寄存器个数
** @Output    : 实际数据所应占用的字节个数
** @Function  : 根据要读取的线圈或寄存器个数计算转化为字节后的数据的字节个数
** @The notes :
**===============================================================================================*/
static u8 MbCalcByteFromBitLength(u16 coils);
 
/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 MbServerRcvAndAnswerHandleAPI(u8 port)
** @Input     : port:Modbus通道号
** @Output    : 处理结果:0:正常 1:停止应答
** @Function  : 服务端(从机)接收请求和应答处理
** @The notes :
**===============================================================================================*/
u8 MbServerRcvAndAnswerHandleAPI(u8 port)
{
	u8 flag = 0;

	flag = MbServerReqAnswerHandle(modbusStation[port].stationID, port, modbusStation[port].RX_Buffer, modbusStation[port].RX_Count);

	//清空接收缓冲区
	MbClrStationRcvBuffer(port);

	return(flag);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u8 MbServerReqAnswerHandle(u8 stationID, u8 port, u8 *pData, u8 Length)
** @Output    : stationID:请求的从站ID,port:Modbus通道号,data:接收到的请求数据,Length:数据长度
** @Function  : 服务端(从站)的主处理函数
** @The notes : 请求应答处理
**===============================================================================================*/
static u8 MbServerReqAnswerHandle(u8 stationID, u8 port, u8 *pData, u8 Length)
{
	u8 rspSelection = 0;
	u8 isMbMsgRight = 0;
	static MB_REQ_ADU mbReqAdu;
	static u16 outValue[123];
	
	mbReqAdu.pData = outValue;

	//从站ID与消息帧中目标地址相同
	if(*pData == stationID)
	{
		//核对接收信息功能码,长度和校验码是否正确
		isMbMsgRight = MbServerCheckRcvMsg(pData, Length, port);

		//接收信息正确
		if(CMPLT_COMPLETE == isMbMsgRight)
		{
			//解析请求报文
			MbServerParseRcvMsg(pData, &mbReqAdu);

			//获得应答响应需求(是否需要应答)
			rspSelection = MbServerCheckAnswerType(&mbReqAdu, port);

			//根据应答响应需求做出具体响应,并发送响应报文
			MbServerSelectAnswerMsg(rspSelection, modbusStation[port].TX_Buffer, port);

			return(0);
		}
		//接收信息不正确
		else
		{
			//停止任务
			return(STOPTASK);
		}
	}
	//从站ID与消息帧中目标地址不相同
	else
	{
		return(STOPTASK);
	}
}

/*=================================================================================================
** @Name      : static u8 MbServerCheckRcvMsg(u8 *data, u8 length, u8 port)
** @Input     : *data:接收到的报文指针,length:报文字节长度,port:通道号
** @Output    : 校验结果:1:接收完成 2:接收未完成
** @Function  : 服务器(从机)校验接收到的报文是否无误
** @The notes :
**===============================================================================================*/
static u8 MbServerCheckRcvMsg(u8 *data, u8 length, u8 port)
{
	u8	target = 0;    //请求报文长度(不包括校验码)
	u16	temp_code = 0;
	u16	checkRtu = 0;

	//RTU模式下消息帧最小长度4，ascii模式下最小3
	if (length < 4)
	{
		return(CMPLT_NO_HOPE);
	}
	//消息帧长度>= 3
	else
	{
		switch(data[1])
		{
			//读请求和写单线圈和写单寄存器
			case MODBUS_READ_COILS :
			case MODBUS_READ_DISCRETEINPUTS :
			case MODBUS_READ_INPUTREGISTERS :
			case MODBUS_READ_HOLDREGISTERS:
			case MODBUS_WRITE_COIL :
			case MODBUS_WRITE_REGISTER :
				target = 6; //以上case对应的请求报文长度为6
				break;

			//写多寄存器
			case MODBUS_WRITE_MULT_REG:
				target = data[6] + 7; //data[6]为输出的寄存器值所占用的字节个数,外加7个字节
				break;

			//功能码错误，返回应答未完成
			default :
			    modbusStation[port].TX_Buffer[0] = data[0];
			    modbusStation[port].TX_Buffer[1] = data[1];
			    MbServerAnswerExceptionMsg(modbusStation[port].TX_Buffer, ILLEGAL_FUNCTION_CODE, port);
				return(CMPLT_NO_HOPE);
		}

		//接收信息长度和请求报文理论长度不相等
		if(length != (target + 2))
		{
			//返回应答未完成
			return(CMPLT_NO_HOPE);
		}

		//接收信息长度和请求报文理论长度相等
		else
		{
			//获取计算校验码(低字节在前)
			checkRtu = MbCalcCRC16Code(data, target);

			//获取消息帧中的效验码(低位在前先发)
			temp_code = (u16)(data[target] << 8);
			temp_code = (u16)(temp_code | data[target + 1]);

			//判断接收端计算得到的效验码与消息帧效验码是否相同(同时支持高位在前或地位在前)
			if(checkRtu == temp_code)
			{
				//返回应答完成
				return(CMPLT_COMPLETE);
			}
			else
			{
			    //获取消息帧中的效验码(高位在前先发)
			    temp_code = (u16)(data[target + 1] << 8);
			    temp_code = (u16)(temp_code | data[target]);

			    //判断接收端计算得到的效验码与消息帧效验码是否相同(同时支持高位在前或地位在前)
			    if(checkRtu == temp_code)
			    {
			    	//返回应答完成
			        return(CMPLT_COMPLETE);
			    }
				else
				{
				    //返回应答未完成
				    return(CMPLT_NO_HOPE);
				}
			}
		}
	}
}

/*=================================================================================================
** @Name      : static void MbServerParseRcvMsg(u8 *pData, MB_REQ_ADU* pMbReqAdu)
** @Input     : *pData:接收到的报文数据,*pMbReqAdu:解析出的请求报文
** @Output    : viod
** @Function  : 根据功能码解析接收到的请求报文
** @The notes : 从接收缓冲区解析到请求报文记录区
**===============================================================================================*/
static void MbServerParseRcvMsg(u8 *pData, MB_REQ_ADU* pMbReqAdu)
{
	u8 i = 0;
	u8 index = 0;
	u16* pOutValue = (void*)0;
	u16 highTemp = 0;
	u16 lowTemp = 0;

	switch(pData[1])
	{
		//解析01-04功能码的请求报文，格式相同，故解析相同
		case MODBUS_READ_COILS :
		case MODBUS_READ_DISCRETEINPUTS :
		case MODBUS_READ_INPUTREGISTERS :
		case MODBUS_READ_HOLDREGISTERS:
			pMbReqAdu->nodeId = pData[0];
			pMbReqAdu->function = pData[1];
			pMbReqAdu->address = (u16)(((u16)(pData[2]) << 8) | ((u16)(pData[3])));
			pMbReqAdu->length = (u16)(((u16)(pData[4]) << 8) | ((u16)(pData[5])));
			break;

		//解析05-06功能码报文，格式相同，故解析相同
		case MODBUS_WRITE_COIL :
		case MODBUS_WRITE_REGISTER :
			pMbReqAdu->nodeId = pData[0];
			pMbReqAdu->function = pData[1];
			pMbReqAdu->address = (u16)(((u16)(pData[2]) << 8) | ((u16)(pData[3])));
			pMbReqAdu->Data = (u16)(((u16)(pData[4]) << 8) | ((u16)(pData[5])));
			break;

		//解析16功能码报文，预置多寄存器
		case MODBUS_WRITE_MULT_REG:
			pMbReqAdu->nodeId = pData[0];
			pMbReqAdu->function = pData[1];
			pMbReqAdu->address = (u16)(((u16)(pData[2]) << 8) | ((u16)(pData[3])));
			pMbReqAdu->length  = (u16)(((u16)(pData[4]) << 8) | ((u16)(pData[5])));
			pOutValue = pMbReqAdu->pData;
			index = 7;
			for(i = 0; i < pMbReqAdu->length; i++)
			{
				highTemp = (u16)(pData[index++]);
				lowTemp = (u16)(pData[index++]);
				*pOutValue = (highTemp << 8) | lowTemp;
				pOutValue++;
			}
			break;

		default :
			break;
	}
}

/*=================================================================================================
** @Name      : static u8 MbServerCheckAnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:解析后的请求报文,port:通道号
** @Output    : 应该作出何种应答:0:正常应答,其它:异常应答
** @Function  : 检测从机应作出的应答响应类型
** @The notes : 判断是正常应答还是异常应答
**===============================================================================================*/
static u8 MbServerCheckAnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
{
	u8 rspType = 0;

	//根据功能码获取当前是正常应答还是异常应答
	switch(pMbReqAdu->function)
	{
		//读线圈和离散量输入
		case MODBUS_READ_COILS :
		case MODBUS_READ_DISCRETEINPUTS :
			rspType = MbCheck01And02AnswerType(pMbReqAdu, port);
			return(rspType);

		//读输入寄存器和保持寄存器
		case MODBUS_READ_INPUTREGISTERS :
		case MODBUS_READ_HOLDREGISTERS:
			rspType = MbCheck03And04AnswerType(pMbReqAdu, port);
			return(rspType);

		//预置单线圈和单寄存器
		case MODBUS_WRITE_COIL :
		case MODBUS_WRITE_REGISTER :
			rspType = MbCheck05And06AnswerType(pMbReqAdu, port);
			return(rspType);

		//预置多寄存器
		case MODBUS_WRITE_MULT_REG:
			rspType = MbCheckFunct16AnswerType(pMbReqAdu, port);
			return(rspType);

		default:
			rspType = ILLEGAL_FUNCTION_CODE;
			return(rspType);
	}
}

/*=================================================================================================
** @Name      : static void MbServerSelectAnswerMsg(u8 rspType, u8 *pData, u8 port)
** @Input     : rspType:应答种类(0:正常应答,其它:异常应答),*pData:接收数据,port:通道号
** @Output    : void
** @Function  : 根据响应标识作出具体应答,发送应答响应请求报文
** @The notes :
**===============================================================================================*/
static void MbServerSelectAnswerMsg(u8 rspType, u8 *pData, u8 port)
{
   	ModbusStation *momodbusStation;

    momodbusStation = &modbusStation[port];

   	switch(pData[1])
	{
		//功能码为1，2，3，4时响应格式相同
		case MODBUS_READ_COILS:
		case MODBUS_READ_DISCRETEINPUTS:
		case MODBUS_READ_HOLDREGISTERS:
		case MODBUS_READ_INPUTREGISTERS:

			//正常应答
			if(NATURALRSP == rspType)
			{
				//封装校验码，发送响应报文
				MbServerBuildAnswerPacket(momodbusStation->TX_Buffer, (u8)(3 + momodbusStation->TX_Buffer[2]), port);
			}
			//异常应答
			else
			{
				//发送异常响应报文
				MbServerAnswerExceptionMsg(momodbusStation->TX_Buffer, rspType, port);
			}
			break;

		//功能码为5，6，16时响应格式相同
		case MODBUS_WRITE_COIL:
		case MODBUS_WRITE_REGISTER:
		case MODBUS_WRITE_MULT_REG:

			//正常应答
			if(NATURALRSP == rspType)
			{
				//封装校验码，发送响应报文
				MbServerBuildAnswerPacket(momodbusStation->TX_Buffer, 6, port);
			}
			//异常应答
			else
			{
				//发送异常响应报文
				MbServerAnswerExceptionMsg(momodbusStation->TX_Buffer, rspType, port);
			}
			break;

		default:
			break;
	}
}

/*=================================================================================================
** @Name      : static void MbServerBuildAnswerPacket(u8 *pData, u8 length, u8 port)
** @Input     : pData:指向不包含效验码CRC的Modbus消息帧的指针,length:不包含效验码CRC的Modbus消息帧字节数,port:发送通道号
** @Output    : void
** @Function  : 服务端(从机)创建和发送应答数据包(发送前将效验位放入应答帧中)
** @The notes : 因封装效验码,length和pDate值在程序中改变
**===============================================================================================*/
static void MbServerBuildAnswerPacket(u8 *pData, u8 length, u8 port)
{
	u16	checkRtu = 0;

	//获得校验码(低字节在前)
	checkRtu = MbCalcCRC16Code(pData, length);

	//将效验码CRC封装到响应报文尾部,先高8位(低字节),后低8位(高字节)
	pData[length++] = (u8)(checkRtu >> 8);
	pData[length++] = (u8)(checkRtu & 0xff);

	//发送响应报文
	ModbusSendDataUserHook(pData, length, port);
}

/*=================================================================================================
** @Name      : static void MbServerAnswerExceptionMsg(u8 *pData, u8 exception, u8 port)
** @Input     : pData:要答复的消息帧数据,exception:描述错误信息的异常代码,port:发送通道号
** @Output    : void
** @Function  : 服务端(从机)创建和发送异常应答报文
** @The notes :
**===============================================================================================*/
static void MbServerAnswerExceptionMsg(u8 *pData, u8 exception, u8 port)
{
	//异常应答功能码为原功能码或0X80
	pData[1] = 0x80 | pData[1];

	//异常代码
	pData[2] = exception;

	//发送异常响应
	MbServerBuildAnswerPacket(pData, 3, port);
}

/*=================================================================================================
** @Name      : static u8 mbPrcFncCode01And02Msg(MB_REQ_ADU* pMbReqAdu,u8 sciNum)
** @Input     : *pMbReqAdu:解析出的请求报文,port:通道号
** @Output    : 应该作出何种应答:0:正常应答,其它:异常应答
** @Function  : 检测功能码为01和02类别的请求需作出的应答类型
** @The notes : 需要正常应答则获取相应寄存器地址
**===============================================================================================*/
static u8 MbCheck01And02AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
{
	u16	i = 0;
	u16 t = 0;
	u8 	isOutTxBuffer = 0;
	u8	bool_val = 0;
	u8	byte_store = 0;
	u8	temp_bool = 0;
	u16  currentAddress = 0;
	u16  endAddress = 0;
	u16  *temp_md = (void*)0;

	//封装 从站id，功能码，数据域占用的字节数到发送缓冲区
	modbusStation[port].TX_Buffer[0] = pMbReqAdu->nodeId;
    modbusStation[port].TX_Buffer[1] = pMbReqAdu->function;
    modbusStation[port].TX_Buffer[2] = MbCalcByteFromBitLength(pMbReqAdu->length);

	//判断输入数量是否超出允许范围
	if( pMbReqAdu->length > 2000)
	{
		//返回请求数量错误
		return(ILLEGAL_DATA_VALUE);
	}

	//发送缓冲区溢出
	isOutTxBuffer = MbCheckOverFlowTxBuffer(pMbReqAdu, modbusStation[port].TX_Buffer[2]);
	if(isOutTxBuffer)
	{
		//返回请求数量错误
		return(ILLEGAL_DATA_VALUE);
	}

	//当前地址
	currentAddress = pMbReqAdu->address;

	//结束地址
	endAddress = pMbReqAdu->address + pMbReqAdu->length - 1;

	//每8次循环完成一个字节数据的存储,每个字节存储8个逻辑线圈或者输入寄存器的状态值
	for(i = 0; i < pMbReqAdu->length; i++)
	{
		//查找用户空间地址
		temp_md = MbServerFindDataPtrHook(pMbReqAdu->function, port, currentAddress, 0);

		//地址合法
		if(temp_md != 0)
		{
			t = i >> 3;
			bool_val  = (u8)((*temp_md) & 0x01);

			//安排当前寄存器的状态在字节中的存放位置
			temp_bool = (u8)(bool_val << (i % 8));

			//已经获取的寄存器状态按位置存放于字节byte_store中
			byte_store = (u8)(byte_store|temp_bool);

			//完成了8个寄存器数据的读取
			if(i % 8 == 7)
			{
				modbusStation[port].TX_Buffer[3 + t] = byte_store;
				byte_store = 0;
				currentAddress++;
				continue;
			}

			//指向下一寄存器
			currentAddress++;

			//所有要读的寄存器已读完
			if (currentAddress > endAddress)
			{
				//将剩余的寄存器数据放入发送缓冲区
				modbusStation[port].TX_Buffer[3 + t] = byte_store;
				break;
			}
		}

		//地址不合法
		else
		{
			return(ILLEGAL_DATA_ADDRESS);
		}
	}

	return(NATURALRSP);
}

/*=================================================================================================
** @Name      : static u8 MbCheck03And04AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:解析出的请求报文,port:通道号
** @Output    : 应该作出何种应答:0:正常应答,其它:异常应答
** @Function  : 检测功能码为03和04类别的请求需作出的应答类型
** @The notes : 需要正常应答则获取相应寄存器地址
**===============================================================================================*/
static u8 MbCheck03And04AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
{
	u16	bgnAddress;
	u16	endAddress;
	u16	*pDataSpc;	                         //存放单元数据指针
	u8	i;			                         //发送缓冲区下标索引
	u8 	isOutTxBuffer = 0;

	bgnAddress = pMbReqAdu->address;
	endAddress = bgnAddress+pMbReqAdu->length-1;

	//封装从站ID、功能码、读取的数据占用的字节数到发送缓冲区
	modbusStation[port].TX_Buffer[0] = pMbReqAdu->nodeId;
	modbusStation[port].TX_Buffer[1] = pMbReqAdu->function;
	modbusStation[port].TX_Buffer[2] = (u8)(pMbReqAdu->length << 1);

	//判断输入数量是否超出允许范围
	if(pMbReqAdu->length > 125)
	{
		//超出允许范围，返回请求数量错误异常码
		return(ILLEGAL_DATA_VALUE);
	}

	//发送缓冲区溢出
	isOutTxBuffer = MbCheckOverFlowTxBuffer(pMbReqAdu, modbusStation[port].TX_Buffer[2]);
	if(isOutTxBuffer)
	{
		//返回请求数量错误异常码
		return(ILLEGAL_DATA_VALUE);
	}

	//按功能码要求读取寄存器数据，并把读到的数据封装到发送缓冲区
	i = 3;
	for ( ; bgnAddress <= endAddress; bgnAddress++)
	{
		//获取当前寄存器的数据地址
		pDataSpc = MbServerFindDataPtrHook(pMbReqAdu->function, port, bgnAddress, 0);

		//判断地址是否合法
		if(pDataSpc != 0)
		{
			modbusStation[port].TX_Buffer[i++] = (u8)(*pDataSpc >> 8);
			modbusStation[port].TX_Buffer[i++] = (u8)(*pDataSpc & 0xff);
		}
		//寄存器地址不存在，返回地址错误异常码
		else
		{
			return(ILLEGAL_DATA_ADDRESS);
		}
	}

	//都正常，返回正常应答标识
	return(NATURALRSP);
}

/*=================================================================================================
** @Name      : static u8 MbCheck03And04AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:解析出的请求报文,port:通道号
** @Output    : 应该作出何种应答:0:正常应答,其它:异常应答
** @Function  : 检测功能码为05和06类别的请求需作出的应答类型
** @The notes : 需要正常应答则获取相应寄存器地址
**===============================================================================================*/
static u8 MbCheck05And06AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
{
	u16	*pDataSpc;
	ModbusStation *momodbusStation;

    momodbusStation = &modbusStation[port];

	//封装从站ID和功能码到发送缓冲区
	modbusStation[port].TX_Buffer[0] = pMbReqAdu->nodeId;
	modbusStation[port].TX_Buffer[1] = pMbReqAdu->function;

	//获取当前寄存器的数据地址
	pDataSpc = MbServerFindDataPtrHook(pMbReqAdu->function, port, pMbReqAdu->address, pMbReqAdu->Data);

	//地址合法
	if(pDataSpc != 0)
	{
		//请求报文为预置单线圈
		if(pMbReqAdu->function == MODBUS_WRITE_COIL)
		{
			//写入数据为0xFF00，则置线圈状态为开
			if((pMbReqAdu->Data) == (u16)COIL_ON)
			{
				*pDataSpc = ON;
			}

			//写入数据为0，置线圈状态为关
			else if((pMbReqAdu->Data) == (u16)COIL_OFF)
			{
				*pDataSpc = OFF;
			}

			//写入数据不是0xFF00或0，返回数据错误异常码
			else
			{
				return(ILLEGAL_DATA_VALUE);
			}
		}

		//请求报文为预置单寄存器
		else
		{
			*pDataSpc = (pMbReqAdu->Data);
		}

		//成功写入数据后封装地址和数据到发送缓冲区
		momodbusStation->TX_Buffer[2] = (u8)(pMbReqAdu->address >> 8);
        momodbusStation->TX_Buffer[3] = (u8)(pMbReqAdu->address & 0xff);
        momodbusStation->TX_Buffer[4] = (u8)((pMbReqAdu->Data) >> 8);
        momodbusStation->TX_Buffer[5] = (u8)((pMbReqAdu->Data) & 0xff);

		//返回正常应答标识
		return(NATURALRSP);
	}
	else
	{
		//寄存器地址不存在或操作权限错误，返回地址错误异常码
		return(ILLEGAL_DATA_ADDRESS);
	}
}

/*=================================================================================================
** @Name      : static u8 MbCheckFunct16AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
** @Input     : *pMbReqAdu:解析出的请求报文,port:通道号
** @Output    : 应该作出何种应答:0:正常应答,其它:异常应答
** @Function  : 检测功能码为16类别的请求需作出的应答类型
** @The notes : 需要正常应答则获取相应寄存器地址
**===============================================================================================*/
static u8 MbCheckFunct16AnswerType(MB_REQ_ADU *pMbReqAdu, u8 port)
{
	u16	bgnAddress;
	u16	endAddress;
	u16	*pDataSpc;            //指向用户单元数据指针

    ModbusStation *momodbusStation;
    momodbusStation = &modbusStation[port];

	//封装从站地址和功能码到发送缓冲区
	modbusStation[port].TX_Buffer[0] = pMbReqAdu->nodeId;
	modbusStation[port].TX_Buffer[1] = pMbReqAdu->function;

	bgnAddress = pMbReqAdu->address;
	endAddress = bgnAddress + pMbReqAdu->length - 1;

	//判断输入数量是否超出允许范围
	if(pMbReqAdu->length > 123)
	{
		//超出允许范围，返回请求数量错误异常代码
		return(ILLEGAL_DATA_VALUE);
	}

	//按报文要求将数据写入寄存器，成功后封装应答报文到发送缓冲区
	for ( ; bgnAddress <= endAddress; bgnAddress++)
	{
		//获取当前寄存器的数据地址
		pDataSpc = MbServerFindDataPtrHook(pMbReqAdu->function, port, bgnAddress, *(pMbReqAdu->pData));

		//地址合法
		if(pDataSpc != 0)
		{
	        ;
		}
		//寄存器地址不存在或操作权限错误，封装异常代码到发送缓冲区
		else
		{
			break;
		}
	}

	//所有地址都合法，则写入数据
	if(bgnAddress > endAddress)
	{
	    for (bgnAddress = pMbReqAdu->address; bgnAddress <= endAddress; bgnAddress++)
	    {
		    //获取当前寄存器的数据地址
		    pDataSpc = MbServerFindDataPtrHook(pMbReqAdu->function, port, bgnAddress, *(pMbReqAdu->pData));

		    //地址合法
		    if(pDataSpc != 0)
		    {
		    	*pDataSpc = *(pMbReqAdu->pData);
			    pMbReqAdu->pData++;
		    }
	    }

		//封装起始地址和寄存器数量到发送缓冲区
        momodbusStation->TX_Buffer[2] = (u8)(pMbReqAdu->address >> 8);
        momodbusStation->TX_Buffer[3] = (u8)(pMbReqAdu->address & 0xff);
        momodbusStation->TX_Buffer[4] = (u8)(pMbReqAdu->length >> 8);
        momodbusStation->TX_Buffer[5] = (u8)(pMbReqAdu->length & 0xff);

        //应答成功，返回正常应答标识
	    return(NATURALRSP);
	}
	else
	{
		return(ILLEGAL_DATA_ADDRESS);
	}
}

/*=================================================================================================
** @Name      : static u8 MbCheckOverFlowTxBuffer(MB_REQ_ADU *pMbReqAdu, u8 dataLen)
** @Input     : *pMbReqAdu:解析出的请求报文,dataLen:要发送的字节数
** @Output    : 检测结果 0:未溢出 1:溢出
** @Function  : 判断接收/发送缓冲区是否溢出
** @The notes :
**===============================================================================================*/
static u8 MbCheckOverFlowTxBuffer(MB_REQ_ADU *pMbReqAdu, u8 dataLen)
{
	switch(pMbReqAdu->function)
	{
		//读请求响应报文格式相同，判断相同
		case MODBUS_READ_COILS:
		case MODBUS_READ_DISCRETEINPUTS:
		case MODBUS_READ_INPUTREGISTERS:
		case MODBUS_READ_HOLDREGISTERS:
			if(dataLen + 5 > cMxReqOrRspBuffersize)
			{
				return(OVERFLOW);
			}
			break;

		//写单线圈和寄存器响应报文格式相同，判断相同
		case MODBUS_WRITE_COIL:
		case MODBUS_WRITE_REGISTER:
		case MODBUS_WRITE_MULT_REG:
		    if(8 > cMxReqOrRspBuffersize)
			{
				return(OVERFLOW);
			}
			break;

		default:
			return(0);
	}

    return(0);
}

/*=================================================================================================
** @Name      : static u8 MbCalcByteFromBitLength(u16 coils)
** @Input     : coils:要读取的逻辑线圈或者寄存器个数
** @Output    : 实际数据所应占用的字节个数
** @Function  : 根据要读取的线圈或寄存器个数计算转化为字节后的数据的字节个数
** @The notes :
**===============================================================================================*/
static u8 MbCalcByteFromBitLength(u16 coils)
{
	//响应报文的数据所占字节数 = 要读取状态的线圈数除以8
	u8 len = (u8)(coils >> 3);

	//如果要读取状态的线圈数不是8的整数倍，则响应报文中响应数据所占字节数加1
	if(coils % 8)
	{
		len++;
	}

	return(len);
}
