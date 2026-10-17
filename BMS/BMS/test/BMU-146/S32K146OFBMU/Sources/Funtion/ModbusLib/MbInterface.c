/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : MbInterface.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : Modbus通信包用户接口函数
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPUserAPI.h"
#include "MbTriSend.h"
#include "MbClient.h"
#include "MbHandle.h"
#include "MbInterface.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
const volatile u8 cSciNum[MBSTATIONNUM] = {0, 1, 2};

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void ModbusStationAllInit(u8 port, u16 bps, u8 stationType, u8 stationID)
** @Input     : port:Modbus通道号, bps:波特率[4800，9600，19200,38400](配置相应的t1.5,t3.5),stationType:站点类型[0从站,1主站,2主&从站] stationID：从站地址
** @Output    : void
** @Function  : 初始化并配置modbus站点相关参数
** @The notes : 用户必须初始化调用,注意串口号与通道号对应
**===============================================================================================*/
void ModbusStationAllInit(u8 port, u16 bps, u8 stationType, u8 stationID)
{
	/*Modbus各端口站参数初始化*/
	ModbusStationInit(port, bps, stationType, stationID);

	/*Modbus各端口触发配置初始化*/
    switch(port)
	{
		case 0:
		    (void)MbTrigSendMsgCfgAPI(0, 1, MbStationReqHandle, (void *)(&cSciNum[0]));
		    break;

		case 1:
		    (void)MbTrigSendMsgCfgAPI(1, 1, MbStationReqHandle, (void *)(&cSciNum[1]));
		    break;

		case 2:
		    (void)MbTrigSendMsgCfgAPI(2, 1, MbStationReqHandle, (void *)(&cSciNum[2]));
		    break;

		default:
			break;
	}
}

/*=================================================================================================
** @Name      : u8 MbClientSendReadReqAPI(u8 stationID, u8 fncCode, u16 address, u16 len, u8 port)
** @Input     : stationID:要读取的从站ID,fncCode:功能码((01读线圈/02读离散量输入/03读保持寄存器/04读输入寄存器),address:要读取的寄存器起始地址,len:要读取的寄存器数量长度,port:Modbus通道号
** @Output    : 读请求插入队列结果:0:不成功1:成功 2:缓冲区溢出
** @Function  : 客户端(主机)发送读数据请求(01和02时可以读取1~2000个长度；03和04时可以读取1~125个寄存器长度的数据)
** @The notes : 作为客户端(主机)用户需要时调用
**===============================================================================================*/
u8 MbClientSendReadReqAPI(u8 stationID, u8 fncCode, u16 address, u16 len, u8 port)
{
	u8 flag = 1;
	u16 dataLen = 0;
	
	switch(fncCode)
	{
		//读请求响应报文格式相同，判断相同
		case MODBUS_READ_COILS:
		case MODBUS_READ_DISCRETEINPUTS:
            dataLen = (len >> 3); 

    	    //如果要读取状态的线圈数不是8的整数倍，则响应报文中响应数据所占字节数加1
	        if (len % 8)
	        {                            
	        	dataLen++;
	        }
	        break;
		
		case MODBUS_READ_INPUTREGISTERS:
		case MODBUS_READ_HOLDREGISTERS:
		    dataLen = len << 1;
		    break;
			
		default:
			break;
	}
	
    if(dataLen + 5 > cMxReqOrRspBuffersize)
	{
	    flag = 2;	
	} 
	else 
	{
        flag = MbClientBuildReadReqAPI(stationID, fncCode, address, len, port);
	}
	return(flag);
}

/*=================================================================================================
** @Name      : u8 MbClientSendWriteSingReqAPI(u8 stationID, u8 fncCode, u16 address, u16 data, u8 port)
** @Input     : stationID:要写入的从站ID,fncCode:功能码(05写单线圈,06写单个寄存器),address:要写入的寄存器地址,data:要写入的数据,port:Modbus通道号
** @Output    : 写请求插入队列结果:0:不成功 1:成功 2:缓冲区溢出
** @Function  : 客户端(主机)发送向指定单个线圈或寄存器写入给定值的写请求
** @The notes : 作为客户端(主机)用户需要时调用，每次只能写入一个值
**===============================================================================================*/
u8 MbClientSendWriteSingReqAPI(u8 stationID, u8 fncCode, u16 address, u16 data, u8 port)
{
	u8 flag = 1;
	
	if(cMxReqOrRspBuffersize < 8)
	{
		flag = 2;
	}
	else 
	{
	    flag = MbClientBuildWriteSingReqAPI(stationID, fncCode, address, data, port);
	}
	
	return (flag);
}

/*=================================================================================================
** @Name      : u8 MbClientSendWriteMultReqAPI(u8 stationID, u16 address, u16 *data, u8 len, u8 port)
** @Input     : stationID:要写入的从站ID,address:要写入的寄存器起始地址,*data:要写入的数据序列首地址指针,len:要写的寄存器数量长度,port:Modbus通道号
** @Output    : 写请求插入队列是否成功0:不成功1:成功 2:缓冲区溢出
** @Function  : 客户端(主机)发送向连续的寄存器中依次写入多个数据值的写请求
** @The notes : 作为客户端(主机)用户需要时调用，每次可以写入多个寄存器(1~123个),功能码16
**===============================================================================================*/
u8 MbClientSendWriteMultReqAPI(u8 stationID, u16 address, u16 *data, u8 len, u8 port)
{
	u8 flag = 1;
	u16 dataLen = 0;
	
	dataLen = len << 1;
	
	if(cMxReqOrRspBuffersize < (dataLen + 8))
	{
	    flag = 2;	
	} 
	else 
	{
	    flag = MbClientBuildWriteMultReqAPI(stationID, address, data, len, port);
	}
	return (flag);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
