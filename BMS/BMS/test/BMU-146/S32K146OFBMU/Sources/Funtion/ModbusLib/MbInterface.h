/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : MbInterface.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : Modbus通信包用户接口函数
** @Instructions :
**===============================================================================================*/
#ifndef _MBINTERFACE_H
#define _MBINTERFACE_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
//功能码
#define MODBUS_READ_COILS               0x01	 //读取线圈状态   	取得一组逻辑线圈的当前状态(ON/OFF)
#define MODBUS_READ_DISCRETEINPUTS     	0x02     //读取输入状态    	取得一组开关输入的当前状态(ON/OFF)
#define MODBUS_READ_HOLDREGISTERS  		0x03	 //读取保持寄存器	在一个或多个保持寄存器中取得当前二进制值
#define MODBUS_READ_INPUTREGISTERS      0x04   	 //读取输入寄存器	在一个或多个输入寄存器中取得当前二进制值
#define MODBUS_WRITE_COIL             	0x05	 //预置单线圈		强置一个逻辑线圈的通断状态
#define MODBUS_WRITE_REGISTER           0x06	 //预置单寄存器		把具体二进制值装入一个保持寄存器
#define MODBUS_WRITE_MULT_REG           0x10   	 //预置多寄存器		把具体二进制值装入一串连续的保持寄存器

//用于05功能码
#define COIL_ON                     	0xFF00   //当为此值时置线圈状态为开
#define COIL_OFF                    	0x0000   //当为此值时置线圈状态为关
#define ON                          	1
#define OFF                         	0

//请求任务应答是否完成标识
#define CMPLT_COMPLETE					0x01     //应答完成
#define CMPLT_NO_HOPE   			    0x02     //应答未完成

#define NATURALRSP		                0x00     //正常应答标识   

#define ILLEGAL_FUNCTION_CODE           0x01     //功能码错误
#define ILLEGAL_DATA_ADDRESS        	0x02     //地址错误
#define ILLEGAL_DATA_VALUE          	0x03     //请求数量错误,写单时数据值错误
#define CHILD_DEVICE_MALFUNCTION        0x04     //从设备故障

/*=================================================================================================
** @Data type definition
**===============================================================================================*/
//定义client端循环控制状态
typedef enum 
{
	Leisure = 0,         //空闲状态
	SendReq,             //发送请求状态
	RspWait,             //应答等待状态
	RspParseAndPro,      //解析处理状态
	FrameSpaceSet        //帧间隔设置
}MbState;

//定义更新数据标识modbusStation[i].updataFlag 的值
typedef enum 
{
	HasNotUpdate = 0,     //初始化值，未更新
	HasUpdate,            //数据已更新
	DoNotUpdateForExRsp,  //由于异常响应未更新
	DoNotUpdateForNoRsp   //由于无应答未更新
}UpdateState;

//定义站点类型
typedef enum 
{
	eModbus_Server = 0,          //站点为从站
	eModbus_Client               //站点为主站
}StationType;

//定义MB请求报文结构体
typedef struct
{
	u8	nodeId ;         //从站ID
	u8	function;        //功能码
	u16	address;         //ModBus起始地址	
	u16	length;	         //请求数据长度
	u16	Data;	         //写单个操作时，输出值
	u16* pData;          //写操作时，指向输出值的指针
} MB_REQ_ADU; 

//定义异常应答报文结构体
typedef struct
{
	u8	node_id;        //从站ID
	u8	function;       //异常功能码
	u8	exception;      //异常代码
} MB_EX_RSP_ADU;

//定义写操作时寄存器结构体
typedef struct
{
    u8 putinflag; 						//是否允许放入数组标志，0表示允许，1表示不允许
    u16 *valueArr; 		            	//存储写操作的输出值指针				
}sWritevalueArr;

//定义ModBus站点结构体
typedef struct 
{
	u8 	clientOrServer;		                  //0=从站，1= 主站

	u8 	sendPort;		                      //数据发送端口，远程模式为1，就地模式为0
	u8  recPort;			                  //数据接收端口，远程模式为1，就地模式为0
	u8  stationID;			                  //站点id	
	u8	RX_Count;		                      //接收到的消息字节个数	(单个消息帧的字节个数)
	u8 	*RX_Buffer;                           //接收缓冲区指针
	u8 	*TX_Buffer;                           //发送缓冲区指针
	u16	RevTmrCount;	                      //任务时钟计数器，单位0.1ms
	u8	startMBTmr;			                  //用于判断时钟是否开始计时
	u8 	GiveUpRX;			                  //当modbus消息帧内部传递时间超时时放弃当前帧，清空缓冲区，开始新帧
	
	u16	*Req_Queue;                           //请求队列指针
	u8	updataFlag;		                      //标识应答数据是否已更新到用户空间
	u8	poll_control;	                      //应答是否完成
	u8	mbstate;		                      //客户端循环控制状态 
	u16	reqTaskTmrCnt;	                      //任务时钟计数器，单位ms
	u16	reqTaskStartTmr;                      //时钟启动时间
			  		
}*pModbusStation,ModbusStation;

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void MbLibRcvDataHandleAPI(u8 port, u16 data)
** @Input     : port:modbus通道号 data:接收到的寄存器数据
** @Output    : void
** @Function  : Modbus通信包接收处理函数[判别是否一帧数据:连续两个字符的间隔时间小于t1.5则将当前寄存器数据存入接收缓冲区(同一帧数据)]
** @The notes : 串口接收函数处调用,注意串口号与通道号对应
**===============================================================================================*/
 extern void MbLibRcvDataHandleAPI(u8 port, u16 data);

/*=================================================================================================
** @Name      : void ModbusRcvTickTask(void)
** @Input     : void
** @Output    : void
** @Function  : Modbus帧接收定时处理函数
** @The notes : 0.1ms周期调用一次
**===============================================================================================*/
extern void ModbusRcvTickTask(void);

/*=================================================================================================
** @Name      : void MbClientTimerTask(void)
** @Input     : void
** @Output    : void
** @Function  : 客户端(主机)请求任务定时器
** @The notes : 1ms调用周期调用一次
**===============================================================================================*/
extern void MbClientTimerTask(void);

/*=================================================================================================
** @Name      : void MbTrigSendTimerTask(void)
** @Input     : void
** @Output    : void
** @Function  : 触发信息发送计时任务
** @The notes : 2ms周期调用一次
**===============================================================================================*/
extern void MbTrigSendTimerTask(void);

/*=================================================================================================
 ** @Name      : u8 MbClientGetRcvUpdateFlagAPI(u8 port)
 ** @Input     : port:Modbus通道号
 ** @Output    : 接收应答数据更新标志 1:已更新,0:未更新
 ** @Function  : 获得客户端(主机)接收应答数据是否已更新到用户指定的空间中的标志
 ** @The notes : 客户端(主机)接收服务端(主机)应答完成标志
 **===============================================================================================*/
extern u8 MbClientGetRcvUpdateFlagAPI(u8 port);

/*=================================================================================================
** @Name      : void ModbusStationAllInit(u8 port, u16 bps, u8 stationType, u8 stationID)
** @Input     : port:Modbus通道号, bps:波特率[4800，9600，19200,38400](配置相应的t1.5,t3.5),stationType:站点类型[0从站,1主站,2主&从站] stationID：从站地址
** @Output    : void
** @Function  : 初始化并配置modbus站点相关参数
** @The notes : 用户必须初始化调用,注意串口号与通道号对应
**===============================================================================================*/
void ModbusStationAllInit(u8 port, u16 bps, u8 stationType, u8 stationID);

/*=================================================================================================
** @Name      : u8 MbClientSendReadReqAPI(u8 stationID, u8 fncCode, u16 address, u16 len, u8 port)
** @Input     : stationID:要读取的从站ID,fncCode功能码((01读线圈/02读离散量输入/03读保持寄存器/04读输入寄存器),address:要读取的寄存器起始地址,len:要读取的寄存器数量长度,port:Modbus通道号
** @Output    : 读请求插入队列结果:0:不成功1:成功 2:缓冲区溢出
** @Function  : 客户端(主机)发送读数据请求(01和02时可以读取1~2000个长度；03和04时可以读取1~125个寄存器长度的数据)
** @The notes : 作为客户端(主机)用户需要时调用
**===============================================================================================*/
u8 MbClientSendReadReqAPI(u8 stationID, u8 fncCode, u16 address, u16 len, u8 port);

/*=================================================================================================
** @Name      : u8 MbClientSendWriteSingReqAPI(u8 stationID, u8 fncCode, u16 address, u16 data, u8 port)
** @Input     : stationID:要写入的从站ID,fncCode:功能码(05写单线圈,06写单个寄存器),address:要写入的寄存器地址,data:要写入的数据,port:Modbus通道号
** @Output    : 写请求插入队列结果:0:不成功 1:成功 2:缓冲区溢出
** @Function  : 客户端(主机)发送向指定单个线圈或寄存器写入给定值的写请求
** @The notes : 作为客户端(主机)用户需要时调用，每次只能写入一个值
**===============================================================================================*/
u8 MbClientSendWriteSingReqAPI(u8 stationID, u8 fncCode, u16 address, u16 data, u8 port);

/*=================================================================================================
** @Name      : u8 MbClientSendWriteMultReqAPI(u8 stationID, u16 address, u16 *data, u8 len, u8 port)
** @Input     : stationID:要写入的从站ID,address:要写入的寄存器起始地址,*data:要写入的数据序列首地址指针,len:要写的寄存器数量长度,port:Modbus通道号
** @Output    : 写请求插入队列是否成功0:不成功1:成功 2:缓冲区溢出
** @Function  : 客户端(主机)发送向连续的寄存器中依次写入多个数据值的写请求
** @The notes : 作为客户端(主机)用户需要时调用，每次可以写入多个寄存器(1~123个),功能码16
**===============================================================================================*/
u8 MbClientSendWriteMultReqAPI(u8 stationID, u16 address, u16 *data, u8 len, u8 port);

#endif
