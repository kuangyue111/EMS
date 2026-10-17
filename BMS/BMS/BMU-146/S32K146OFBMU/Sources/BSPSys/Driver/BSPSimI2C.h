/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPSimI2c.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 模拟IIC驱动模块
** @Instructions :
**===========================================================================================*/
#ifndef BSPSYS_DRIVER_BSPSIMI2C_H_
#define BSPSYS_DRIVER_BSPSIMI2C_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPGPIO.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
///////////////////////////////////////////////模拟IIC通用宏定义///////////////////////////////////////////////////
#define ACK        0
#define NO_ACK     1

/*端口方向、数据的设置和状态*/
/*
#define IIC_PORT_SET(RegPort, Offset, Direct)  ((Direct) ? (RegPort |= 1 << Offset) : (RegPort &= ~(1 << Offset)))
#define IIC_PORT_STA(RegPort, Offset)          ((RegPort & (1 << Offset)) ? (HIGH) : (LOW))
*/
//延时
#define SimIICWait(time)        {volatile u32 Count = time; while(Count--);}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////多个模拟IIC通道应用宏定义/////////////////////////////////////////////////////
//#define IIC_SCL_DIR(Direct, Channel) {if(1 == Channel) {(void)BSPGPIOInit(ePOR_A, ePIN_3, ePORT_MUX_AS_GPIO, Direct);} else if(2 == Channel) {(void)BSPGPIOInit(ePOR_A, ePIN_3, ePORT_MUX_AS_GPIO, Direct);}}
//#define IIC_SDA_DIR(Direct, Channel) {if(1 == Channel) {(void)BSPGPIOInit(ePOR_A, ePIN_2, ePORT_MUX_AS_GPIO, Direct);} else if(2 == Channel) {(void)BSPGPIOInit(ePOR_A, ePIN_2, ePORT_MUX_AS_GPIO, Direct);}}
//
//#define IIC_SCL_DAT(Level, Channel)  {if(1 == Channel) {(void)BSPGPIOSetPortState(ePOR_A, ePIN_3, Level);} else if(2 == Channel) {(void)BSPGPIOSetPortState(ePOR_A, ePIN_3, Level);}}
//#define IIC_SDA_DAT(Level, Channel)  {if(1 == Channel) {(void)BSPGPIOSetPortState(ePOR_A, ePIN_2, Level);} else if(2 == Channel) {(void)BSPGPIOSetPortState(ePOR_A, ePIN_2, Level);}}
//
//#define IIC_SDA_STA(Channel)         ((1 == Channel) ? (BSPGPIOGetInPortState(ePOR_A, ePIN_2)) : (BSPGPIOGetInPortState(ePOR_A, ePIN_2)))

/*IIC数据传输长度*/
#define IIC_DATA_LEN    20

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*IIC模拟通道*/
typedef enum
{
	Chann_RTC = 1,
	Chann_EEP = 2,

	eSimIIC_NUM
}e_SimI2cChann;

/*模拟IIC方向定义*/
typedef enum
{
	INPUT = 0,		/*初始化IO口为输入*/
	OUTPUT =1		/*初始化IO口为输出*/

}e_SimI2cDirt;

/*模拟IIC数据定义*/
typedef enum
{
	LOW = 0,		/*初始化IO口为低电平*/
	HIGH =1		    /*初始化IO口为高电平*/

}e_SimI2cData;

typedef struct
{
    u8  devID;
    u8  regLen;
    u16 regAddr;
    u8  dataLen;
    u8  dataBuf[IIC_DATA_LEN];
}STR_IICDATA;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPSimIICInit(e_SimI2cChann Channel)
** @Input     : Chan:选择模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : void
** @Function  : 模拟IIC通信初始化
** @The notes :
**===========================================================================================*/
void BSPSimIICInit(e_SimI2cChann Channel);

/*=============================================================================================
** @Name      : u8 BSPSimIICWriteBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
** @Input     : Channel:IIC通道号[1:RTC 2:EEPROM], DevAddr设备地址, RegAddr寄存器地址, *Data需要写入的数据, Length写入数据的长度
** @Output    : IIC写数据成功标志0不成功1成功
** @Function  : 模拟IIC写一块带CRC校验的数据
** @The notes :
**===========================================================================================*/
u8 BSPSimIICWriteBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length);

/*=============================================================================================
** @Name      : u8 BSPSimIICReadBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
** @Input     : Channel:IIC通道号[1:RTC 2:EEPROM], DevAddr是设备地址, RegAddr是寄存器地址, *data是读出的数据, Length是读的数据长度
** @Output    : IIC读数据成功标志0不成功1成功
** @Function  : 模拟IIC读一块带CRC校验的数据
** @The notes :
**===========================================================================================*/
u8 BSPSimIICReadBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length);

/*=============================================================================================
** @Name      : u8 BSPSimIICWriteBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8* Data, u8 Length)
** @Input     : Channel是IIC通道号[1:RTC 2:EEPROM], DevAddr设备地址, RegAddr寄存器地址, *Data需要写入的数据, Length写入数据的长度
** @Output    : IIC写数据成功标志0不成功1成功
** @Function  : 模拟IIC写无CRC校验的数据
** @The notes :
**===========================================================================================*/
u8 BSPSimIICWriteBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8* Data, u8 Length);

/*=============================================================================================
** @Name      : u8 BSPSimIICReadBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
** @Input     : Channel是IIC通道号[1:RTC 2:EEPROM], DevAddr是设备地址, RegAddr是寄存器地址, *data是读出的数据 dataLen是读的数据长度
** @Output    : IIC读数据成功标志0不成功1成功
** @Function  : 模拟IIC读一块无CRC校验的数据
** @The notes :
**===========================================================================================*/
u8 BSPSimIICReadBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length);

/*=============================================================================================
** @Name      : u8 BSPSimIICWriteData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data)
** @Input     : Chan:选择模拟IIC通道[1:RTC 2:EEPROM] devID是设备地址 regLen是寄存器长度 regAddr是寄存器地址 dataLen是写的数据长度  *data是写的数据
** @Output    : 写数据结果:FALSE失败 TRUE:成功
** @Function  : 使用模拟IIC向指定寄存器和地址写入数据
** @The notes : RTC、EEPROM模拟IIC写数据
**===========================================================================================*/
u8 BSPSimIICWriteData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data);

/*=============================================================================================
** @Name      : u8 BSPSimIICReadData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data)
** @Input     : Chan:选择模拟IIC通道[1:RTC 2:EEPROM] devID是设备地址 regLen是寄存器长度 regAddr是寄存器地址 dataLen是读的数据长度  *data是读出的数据
** @Output    : 读数据结果:FALSE失败 TRUE:成功
** @Function  : 模拟IIC向指定寄存器和地址读取数据
** @The notes : RTC、EEPROM模拟IIC读数据
**===========================================================================================*/
u8 BSPSimIICReadData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data);

/*=============================================================================================
** @Name      : void BSPSimIICStart(u8 Channel)
** @Input     : Channel选择模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : void
** @Function  : 模拟IIC启动
** @The notes :
**===========================================================================================*/
void BSPSimIICStart(u8 Channel);

/*=============================================================================================
** @Name      : void BSPSimIICStop(u8 Channel)
** @Input     : Channel选择模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : void
** @Function  : 模拟IIC停止
** @The notes :
**===========================================================================================*/
void BSPSimIICStop(u8 Channel);

/*=============================================================================================
** @Name      : u8 BSPSimIICSendByte(u8 Channel, u8 Data)
** @Input     : Channel:选择模拟IIC通道[1:RTC 2:EEPROM] Data:发送字节
** @Output    : Ack从器件应答值
** @Function  : 模拟IIC向设备发送一个字节(高位先发)
** @The notes : 注意在中断函数里处理的函数不能再次打开全局中断
**===========================================================================================*/
u8 BSPSimIICSendByte(u8 Channel, u8 Data);

/*=============================================================================================
** @Name      : u8 BSPSimIICRecvByte(u8 Ack, u8 Channel)
** @Input     : Ack:是否向从机应答，Channel:选择模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : 读取到的字节
** @Function  : 模拟IIC向设备读取一个字节(先读高位)
** @The notes : 注意在中断函数里处理的函数不能再次打开全局中断
**===========================================================================================*/
u8 BSPSimIICRecvByte(u8 Ack, u8 Channel);

#endif
