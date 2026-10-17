/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : MbHandle.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : Modbus通信包规约处理接口
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPUserAPI.h"
#include "MbInterface.h"
#include "MbUserDef.h"
#include "MbQueue.h"
#include "MbClient.h"
#include "MbServer.h"
#include "MbTriSend.h"
#include "MbHandle.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
ModbusStation *modbusStation;//MB结构体指针

//CRC 循环冗余表
const u8 auchCRCHi_exp[] = {
    0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
	0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
	0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01,
	0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41,
	0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81,
	0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0,
	0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01,
	0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40,
	0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
	0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
	0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01,
	0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,
	0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
	0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
	0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01,
	0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,
	0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
	0x40
};

const  u8 auchCRCLo_exp[] = {
	0x00, 0xc0, 0xc1, 0x01, 0xc3, 0x03, 0x02, 0xc2, 0xc6, 0x06, 0x07, 0xc7, 0x05, 0xc5, 0xc4,
	0x04, 0xcc, 0x0c, 0x0d, 0xcd, 0x0f, 0xcf, 0xce, 0x0e, 0x0a, 0xca, 0xcb, 0x0b, 0xc9, 0x09,
	0x08, 0xc8, 0xd8, 0x18, 0x19, 0xd9, 0x1b, 0xdb, 0xda, 0x1a, 0x1e, 0xde, 0xdf, 0x1f, 0xdd,
	0x1d, 0x1c, 0xdc, 0x14, 0xd4, 0xd5, 0x15, 0xd7, 0x17, 0x16, 0xd6, 0xd2, 0x12, 0x13, 0xd3,
	0x11, 0xd1, 0xd0, 0x10, 0xf0, 0x30, 0x31, 0xf1, 0x33, 0xf3, 0xf2, 0x32, 0x36, 0xf6, 0xf7,
	0x37, 0xf5, 0x35, 0x34, 0xf4, 0x3c, 0xfc, 0xfd, 0x3d, 0xff, 0x3f, 0x3e, 0xfe, 0xfa, 0x3a,
	0x3b, 0xfb, 0x39, 0xf9, 0xf8, 0x38, 0x28, 0xe8, 0xe9, 0x29, 0xeb, 0x2b, 0x2a, 0xea, 0xee,
	0x2e, 0x2f, 0xef, 0x2d, 0xed, 0xec, 0x2c, 0xe4, 0x24, 0x25, 0xe5, 0x27, 0xe7, 0xe6, 0x26,
	0x22, 0xe2, 0xe3, 0x23, 0xe1, 0x21, 0x20, 0xe0, 0xa0, 0x60, 0x61, 0xa1, 0x63, 0xa3, 0xa2,
	0x62, 0x66, 0xa6, 0xa7, 0x67, 0xa5, 0x65, 0x64, 0xa4, 0x6c, 0xac, 0xad, 0x6d, 0xaf, 0x6f,
	0x6e, 0xae, 0xaa, 0x6a, 0x6b, 0xab, 0x69, 0xa9, 0xa8, 0x68, 0x78, 0xb8, 0xb9, 0x79, 0xbb,
	0x7b, 0x7a, 0xba, 0xbe, 0x7e, 0x7f, 0xbf, 0x7d, 0xbd, 0xbc, 0x7c, 0xb4, 0x74, 0x75, 0xb5,
	0x77, 0xb7, 0xb6, 0x76, 0x72, 0xb2, 0xb3, 0x73, 0xb1, 0x71, 0x70, 0xb0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9c, 0x5c,
	0x5d, 0x9d, 0x5f, 0x9f, 0x9e, 0x5e, 0x5a, 0x9a, 0x9b, 0x5b, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4b, 0x8b, 0x8a, 0x4a, 0x4e, 0x8e, 0x8f, 0x4f, 0x8d, 0x4d, 0x4c, 0x8c,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void ModbusStationInit(u8 port, u16 bps, u8 stationType, u8 stationID)
** @Input     : port:modbus通道,bps:波特率[4800，9600，19200，38400](配置相应的t1.5,t3.5),stationType:站点类型[0,2] stationID:从站地址[0,255]
** @Output    : void
** @Function  : 初始化并配置Modbus站点
** @The notes :
**===============================================================================================*/
void ModbusStationInit(u8 port, u16 bps, u8 stationType, u8 stationID)
{  
	//指针变量返回数组
    modbusStation = MbGetStationArrayHook();
    modbusStation[port].RX_Buffer = MbGetRxBufferArrayHook(port);
    modbusStation[port].TX_Buffer = MbGetTxBufferArrayHook(port);
    modbusStation[port].Req_Queue = MbGetReqQueueArrayHook(port);
    
    //接收端口，发送端口初始化	
	modbusStation[port].recPort = port;
	modbusStation[port].sendPort = port;
	
	if(bps == 4800) 
	{
	    gMBT15Cfg[port] = 25;
	    gMBT35Cfg[port] = 58;
	} 
	else if (bps == 9600)
    {
	    gMBT15Cfg[port] = 12;
	    gMBT35Cfg[port] = 29;
	} 
	else if(bps == 19200)
	{
	    gMBT15Cfg[port] = 7;
	    gMBT35Cfg[port] = 15;
	} 
	else 
    {
	    gMBT15Cfg[port] = 7;
	    gMBT35Cfg[port] = 17;
	} 	

	//Modbus 端口模式初始化
	modbusStation[port].clientOrServer = stationType;

	//Modbus 从站ID初始化
	if(cModeSelect == 0)
	{		
		modbusStation[port].stationID = stationID;
	}
	//主站初始化
	else if (cModeSelect == 1)
	{
		//初始化客户端状态为空闲状态
		modbusStation[port].mbstate = Leisure;

		//初始化请求队列
		(void)MBQueInit(modbusStation[port].Req_Queue, cUnitSize, cUnitNum);
	}
	//软件既可以做主站又可以做从站时初始化
	else
	{
		//站点为从站时
		if(modbusStation[port].clientOrServer == eModbus_Server)
		{
			//初始化站点ID
			modbusStation[port].stationID = stationID;
		}
		//站点为主站
		else
		{
			//初始化请求队列
			(void)MBQueInit(modbusStation[port].Req_Queue, cUnitSize, cUnitNum);

			//初始化客户端状态为空闲状态
			modbusStation[port].mbstate = Leisure;
		}			
	}
}

/*=================================================================================================
** @Name      : void MbLibRcvDataHandleAPI(u8 port, u16 data)
** @Input     : port:modbus通道号 data:接收到的寄存器数据
** @Output    : void
** @Function  : Modbus通信包接收处理函数[判别是否一帧数据:连续两个字符的间隔时间小于t1.5则将当前寄存器数据存入接收缓冲区(同一帧数据)]
** @The notes : 串口接收函数处调用,注意串口号与通道号对应
**===============================================================================================*/
void MbLibRcvDataHandleAPI(u8 port, u16 data)
{
	//时钟开始计时标志置1
	modbusStation[port].startMBTmr = TRUE;

	//任务时钟计数器初始化
	modbusStation[port].RevTmrCount = 0;

	// 消息帧内字符间的传输间隔必须小于1.5个字符时间
	if(modbusStation[port].GiveUpRX)
	{
		//间隔大于1.5个字符时间，接收字节数清零
		modbusStation[port].RX_Count = 0;
		modbusStation[port].GiveUpRX = FALSE;
	}

	//SCI接收寄存器数据存入接收缓冲区
	modbusStation[port].RX_Buffer[modbusStation[port].RX_Count] = (u8)data;
	modbusStation[port].RX_Count++;

	//接收字节数大于缓冲区大小，接收字节数清零
	if(modbusStation[port].RX_Count > cMxReqOrRspBuffersize)
	{
		modbusStation[port].RX_Count = 0;
	}
}

/*=================================================================================================
** @Name      : void ModbusRcvTickTask(void)
** @Input     : void
** @Output    : void
** @Function  : Modbus帧接收定时处理函数
** @The notes : 0.1ms周期调用一次
**===============================================================================================*/
void ModbusRcvTickTask(void)
{
	u8 i = 0;

	for(i = 0; i < cMBStationNum; i++)
	{
		//判断时钟是否开始计时
		if(modbusStation[i].startMBTmr)
		{
			modbusStation[i].RevTmrCount++;

			// 消息帧内字符间的传输间隔必须小于1.5个字符时间
			if(modbusStation[i].RevTmrCount > gMBT15Cfg[i])
			{
				//GiveUpRX为true时函数RcvRegisterData会丢弃之前接收到的数据
				modbusStation[i].GiveUpRX = TRUE;
			}

			//消息帧之间的传输时间间隔必须大于3.5个字符时间
			if(modbusStation[i].RevTmrCount > gMBT35Cfg[i])
			{
				modbusStation[i].RevTmrCount = 0;
				modbusStation[i].startMBTmr = FALSE;

				//仅为从站时，启动从端任务
				if (cModeSelect == 0)
				{
					(void)MbSetTrigFlagStateAPI(i, 1);
				}
				//可为主或从站时配置为从站则启动从端任务
				else if (cModeSelect == 2)
				{
					if(modbusStation[i].clientOrServer == 0)
					{
						(void)MbSetTrigFlagStateAPI(i, 1);
					}
				}
			}
		}
	}
}

/*=================================================================================================
** @Name      : void MbStationReqHandle(void *p)
** @Input     : *p:输入参数
** @Output    : void
** @Function  : Modbus站收发请求处理
** @The notes :
**===============================================================================================*/
void MbStationReqHandle(void *p)
{
	u8 flag = 0;
	u8 *sci = (u8*)p;
	
	//软件既可做主端又可做从端
	if(cModeSelect == 2)
	{
		//端口0站点类型为主站
		if(modbusStation[*sci].clientOrServer == 1)
		{
			//实现客户端的请求查询控制
			MbClientReqAndRcvHandleAPI(*sci);
		}
		//端口0站点类型为从站
		else
		{
			//实现服务端处理请求
			flag = MbServerRcvAndAnswerHandleAPI(*sci);
			
			if(flag == STOPTASK)
			{
				(void)MbSetTrigFlagStateAPI(*sci, 0);
			}
		}
	}
	//软件做主端
	else if(cModeSelect == 1)
	{
		//实现客户端的请求查询控制
		MbClientReqAndRcvHandleAPI(*sci);
	}
	//软件做从端
	else
	{	
		//实现服务端处理请求
		flag = MbServerRcvAndAnswerHandleAPI(*sci);
	
		if(flag == STOPTASK)
		{
			(void)MbSetTrigFlagStateAPI(*sci, 0);
		}
	}
}

/*=================================================================================================
** @Name      : void MbClientStartReqPrcTask(u8 port)
** @Input     : port:modbus通道号
** @Output    : void
** @Function  : 客户端(主机)启动请求处理任务
** @The notes : 当主机空闲且有请求数据时启动
**===============================================================================================*/
void MbClientStartReqPrcTask(u8 port)
{
	//数据发送端口为0 ，设置端口0触发标志为1
	if(modbusStation[port].sendPort == port)
	{
	    (void)MbSetTrigFlagStateAPI(port, 1);
	}
}

/*=================================================================================================
** @Name      : void MbClrStationRcvBuffer(u8 port)
** @Input     : port:modbus通道号
** @Output    : u8
** @Function  : 清空modbus接收通道缓冲区
** @The notes : 主机或从机
**===============================================================================================*/
void MbClrStationRcvBuffer(u8 port)
{
	modbusStation[port].RX_Count = 0;
	modbusStation[port].GiveUpRX = 0;
}

/*=================================================================================================
** @Name      : u16 MbCalcCRC16Code(u8 *pData, u8 len)
** @Input     : pData:指向需要计算CRC的数据指针 len:要计算CRC的数据长度
** @Output    : 16位CRC效验码(低位在前)
** @Function  : 计算CRC效验码
** @The notes : 低字节在前
**===============================================================================================*/
u16 MbCalcCRC16Code(u8 *pData, u8 len)
{
	u8   uchCRCHi = 0xff;   //CRC高字节初始化
	u8   uchCRCLo = 0xff;   //CRC低字节初始化
	u16  uIndex;            //查询表索引
	u16  temp_code;

	//Modbus RTU校验CRC计算结果为低字节在前高字节在后
	while (len)
	{
		uIndex = (u16)(uchCRCHi ^ *pData++);
		uchCRCHi = (u8)(uchCRCLo ^ auchCRCHi_exp[uIndex]);
		uchCRCLo = auchCRCLo_exp[uIndex];
		len--;
	}

	temp_code = (u16)uchCRCHi;
	temp_code = (u16)(temp_code << 8);

	return((u16)(temp_code | uchCRCLo));
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
