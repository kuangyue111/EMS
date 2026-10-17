/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPUserAPI.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 用户接口函数模块
** @Instructions :
**===========================================================================================*/
#ifndef _BSPUSERAPI_H_
#define _BSPUSERAPI_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPCAN.h"
#include "BSPUART.h"
#include "BSPFTM.h"
#include "BSPADC.h"
#include "BSPGPIO.h"
#include "BSPSPI.h"
#include "BSPI2C.h"
#include "BSPSimI2c.h"
#include "BSPEEPROM.h"
#include "BSPFLASH.h"
#include "BSPIRQ.h"
#include "BSPRTC.h"
#include "BSPWDOG.h"

#include "DEVGPIO.h"
#include "DEVADC.h"
#include "DEVSCI.h"
#include "DEVCAN.h"
#include "RX8010.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/

/*=============================================================================================
** @Data type definition
**===========================================================================================*/

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
//--------------------------------------CAN模块 start------------------------------------------
/*=============================================================================================
** @Name      : u8 BSPCANInit(e_CANNUM canNum, u8 osc, u16 rate)
** @Input     : canNum:CAN通道号 osc:晶振频率 rate:CAN波特率
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : CAN驱动模块初始化F
** @The notes : BSP调用
**===========================================================================================*/
extern u8 BSPCANInit(e_CANNUM canNum, u8 osc, u16 rate);

/*=============================================================================================
** @Name      : u8 BSPCANSetRate(e_CANNUM canNum, u16 rate)
** @Input     : canNum:CAN通道号, rate:CAN通讯波特率(125/250/500/1000Kbps)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 设置CAN通信波特率
** @The notes :
**===========================================================================================*/
extern u8 BSPCANSetRate(e_CANNUM canNum, u16 rate);

/*=============================================================================================
** @Name      : u8 BSPCANSendMsg(e_CANNUM canNum, t_CANMSG *msg)
** @Input     : canNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(直接发送)
** @The notes : 用户调用(尽量少用,会丢帧)
**===========================================================================================*/
extern u8 BSPCANSendMsg(e_CANNUM canNum, t_CANMSG *msg);

/*=============================================================================================
** @Name      : u8 BSPCANSendQin(e_CANNUM canNum, t_CANMSG *msg)
** @Input     : canNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(数据压入队列)
** @The notes : 用户调用(尽量少用,会丢帧)
**===========================================================================================*/
extern u8 BSPCANSendQin(e_CANNUM canNum, t_CANMSG *msg);

/*=============================================================================================
** @Name      : u8 DEVCANResetInit(e_CANNUM canNum)
** @Input     : canNum:CAN通道号
** @Output    : TRUE:成功 FALSE:失败
** @Function  : CAN口复位重新初始化
** @The notes : 用户调用
**===========================================================================================*/
extern u8 DEVCANResetInit(e_CANNUM canNum);
//--------------------------------------CAN模块 end--------------------------------------------

//--------------------------------------UART模块 start-----------------------------------------
/*=============================================================================================
** @Name      : u8 BSPSCIInit(e_SCINUM sciNum, u32 rate, u8 busClock)
** @Input     : sciNum:SCI通道号  rate:通讯波特率 busClock:总线时钟
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : SCI驱动模块初始化
** @The notes : BSP调用,用户调用
**===========================================================================================*/
extern u8 BSPSCIInit(e_SCINUM sciNum, u32 rate, u8 busClock);

/*=============================================================================================
** @Name      : u8 BSPSCISendDataOnIRQ(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : sciNum:SCI通道号     *sendbuf:发送的数据    size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SCI中断方式发送串口数据
** @The notes : 直接发送,暂不实现
**===========================================================================================*/
extern u8 BSPSCISendDataOnIRQ(e_SCINUM sciNum, u8 *sendbuf, u8 size);

/*=============================================================================================
** @Name      : u8 BSPSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : sciNum:SCI通道号     *sendbuf:发送的数据    size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SCI询问方式发送串口数据
** @The notes : 直接发送,用户调用
**===========================================================================================*/
extern u8 BSPSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size);

/*=============================================================================================
** @Name      : u8 DEVSCIResetInit(e_SCINUM sciNum)
** @Input     : sciNum:SCI通道号
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 串口复位重新初始化
** @The notes : 用户调用
**===========================================================================================*/
extern u8 DEVSCIResetInit(e_SCINUM sciNum);

/*=============================================================================================
** @Name      : u8 DEVSCISendDataOnISR(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : sciNum:SCI通道号 *sendbuf:发送的数据 size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : RS485中断方式发送串口数据
** @The notes : 直接发送,暂不实现
**===========================================================================================*/
extern u8 DEVSCISendDataOnISR(e_SCINUM sciNum, u8 *sendbuf, u8 size);

/*=============================================================================================
** @Name      : u8 DEVSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : sciNum:SCI通道号 *sendbuf:发送的数据 size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : RS485询问方式发送串口数据
** @The notes : 直接发送,用户调用
**===========================================================================================*/
extern u8 DEVSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size);

//--------------------------------------UART模块 end-------------------------------------------

//--------------------------------------EEPROM模块 start---------------------------------------
/*=============================================================================================
** @Name      : u8 BSPINTEEPWriteByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入EEPROM地址([0,4095](4K) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按字节方式写入数据(8位)
** @The notes :
**===========================================================================================*/
extern u8 BSPINTEEPWriteByte(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTEEPWriteWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:写入EEPROM地址([0,4095](4K) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按字方式写入数据(16位)
** @The notes : len必须是4的整数倍
**===========================================================================================*/
extern u8 BSPINTEEPWriteWord(u32 addr, u16 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTEEPWriteLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:写入EEPROM地址([0,4095](4K) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按双字方式写入数据(32位)
** @The notes :
**===========================================================================================*/
extern u8 BSPINTEEPWriteLong(u32 addr, u32 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTEEPReadByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:读取EEPROM地址([0,4095](4K) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按字节方式读出数据
** @The notes :
**===========================================================================================*/
extern u8 BSPINTEEPReadByte(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTEEPReadWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:读取EEPROM地址([0,4095](4K) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按字方式读出数据
** @The notes :
**===========================================================================================*/
extern u8 BSPINTEEPReadWord(u32 addr, u16 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTEEPReadLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:读取EEPROM地址([0,4095](4K) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按双字方式读出数据
** @The notes :
**===========================================================================================*/
extern u8 BSPINTEEPReadLong(u32 addr, u32 *pData, u32 len);

/*=============================================================================================
** @Name      : void EXTEep24M512Init(void)
** @Input     : void
** @Output    : void
** @Function  : 外部EEPROM-M24512初始化
** @The notes :
**===========================================================================================*/
extern void EXTEep24M512Init(void);

/*=============================================================================================
** @Name      : void DEVEXTEEPCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 外部EEPROM检测任务
** @The notes : 4s
**===========================================================================================*/
extern void DEVEXTEEPCheckTask(void);

/*=============================================================================================
** @Name      : u8 DEVEXTEEPWriteByte(u16 addr, u8 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要写入的数据  length:要写入数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 向外部EEPROM指定地址写入length个字节
** @The notes : 尽量不要跨页写(一页128Byte)
**===========================================================================================*/
extern u8 DEVEXTEEPWriteByte(u16 addr, u8 *buffer, u16 length);

/*=============================================================================================
** @Name      : u8 DEVEXTEEPWriteWord(u16 addr, u16 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要写入的数据  length:要写入数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 向外部EEPROM指定地址写入length个单字
** @The notes : 低字节在前(小端模式),尽量不要跨页写(一页128Byte)
**===========================================================================================*/
extern u8 DEVEXTEEPWriteWord(u16 addr, u16 *buffer, u16 length);

/*=============================================================================================
** @Name      : u8 DEVEXTEEPWriteLong(u16 addr, u32 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要写入的数据  length:要写入数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 向外部EEPROM指定地址写入length个双字
** @The notes : 低字节在前(小端模式),尽量不要跨页写(一页128Byte)
**===========================================================================================*/
extern u8 DEVEXTEEPWriteLong(u16 addr, u32 *buffer, u16 length);

/*=============================================================================================
** @Name      : u8 DEVEXTEEPReadByte(u16 addr, u8 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要读取的数据  length:读取数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 从外部EEPROM指定地址读出length个字节
** @The notes :
**===========================================================================================*/
extern u8 DEVEXTEEPReadByte(u16 addr, u8 *buffer, u16 length);

/*=============================================================================================
** @Name      : u8 DEVEXTEEPReadWord(u16 addr, u16 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要读取的数据  length:读取数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 从外部EEPROM指定地址读出length个单字
** @The notes : 低字节在前(小端模式)
**===========================================================================================*/
extern u8 DEVEXTEEPReadWord(u16 addr, u16* buffer, u16 length);

/*=============================================================================================
** @Name      : u8 DEVEXTEEPReadLong(u16 addr, u32 *buffer, u16 length)
** @Input     : addr:EEPROM地址[0,65535] buffer:要读取的数据  length:读取数据的长度
** @Output    : 操作结果 TRUE:成功 FALSE:失败
** @Function  : 从外部EEPROM指定地址读出length个双字
** @The notes : 低字节在前(小端模式)
**===========================================================================================*/
extern u8 DEVEXTEEPReadLong(u16 addr, u32 *buffer, u16 length);

/*=============================================================================================
** @Name      : u8 DEVEXTEEPGetReadyFlag(void)
** @Input     : void
** @Output    : 0:未就绪 1:就绪
** @Function  : 获取外部EEPROm就绪状态标志
** @The notes : 与外部EEPROM芯片通讯状态
**===========================================================================================*/
extern u8 DEVEXTEEPGetReadyFlag(void);

//--------------------------------------EEPROM模块 end-----------------------------------------

//--------------------------------------FLASH模块 start----------------------------------------
/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseChip(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除整个MCU全部FLASH
** @The notes : 用户慎用
**===========================================================================================*/
extern u8 BSPINTFLASHEraseChip(void);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseBlock(u16 block)
** @Input     : u16 sector:FLASH块号[0,5]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除FLASH块
** @The notes : 按块擦除(按用途划分1块大小各不同[4K/392K/392K/200K/4K/32K])
**===========================================================================================*/
extern u8 BSPINTFLASHEraseBlock(u16 block);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseSector(u32 addr)
** @Input     : addr:FLASH地址[0,1048575](1M)
** @Output    : TRUE:扇区擦除成功 FALSE:扇区擦除错误
** @Function  : 擦除FLASH扇区(一页)
** @The notes : 1个扇区为4096Byte(4K),地址必须是4096倍数
**===========================================================================================*/
extern u8 BSPINTFLASHEraseSector(u32 addr);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWriteByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数(必须是8的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数
**===========================================================================================*/
extern u8 BSPINTFLASHWriteByte(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWriteWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数(必须是4的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字方式写入数据(16位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数,单次最大写1Kbyte.
**===========================================================================================*/
extern u8 BSPINTFLASHWriteWord(u32 addr, u16 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWriteLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数(必须是2的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按双字方式写入数据(32位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数,单次最大写1Kbyte.
**===========================================================================================*/
extern u8 BSPINTFLASHWriteLong(u32 addr, u32 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
extern u8 BSPINTFLASHReadByte(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
extern u8 BSPINTFLASHReadWord(u32 addr, u16 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按双字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
extern u8 BSPINTFLASHReadLong(u32 addr, u32 *pData, u32 len);

/*=============================================================================================
** @Name      : void EXTFlashSST26Init(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash初始化
** @The notes : 检测外部Flash芯片是否就绪
**===========================================================================================*/
extern void EXTFlashSST26Init(void);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHWriteByte(u32 addr, u8 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:写入Flash数据块 size:数据长度[1,256]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将8位字节数据写入到外部Flash中
** @The notes :
**===========================================================================================*/
extern u8 DEVEXTFLASHWriteByte(u32 addr, u8 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHWriteWord(u32 addr, u16 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:写入Flash数据块 size:数据长度[1,128]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将16位单字数据写入到外部Flash中
** @The notes :
**===========================================================================================*/
extern u8 DEVEXTFLASHWriteWord(u32 addr, u16 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHWriteLong(u32 addr, u32 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:写入Flash数据块 size:数据长度[1,64]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将32位双字数据写入到外部Flash中
** @The notes :
**===========================================================================================*/
extern u8 DEVEXTFLASHWriteLong(u32 addr, u32 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHReadByte(u32 addr, u8 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:读出的数据块指针 size:数据长度(最大一块32k)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 从外部Flash指定地址读取任意长度的8位字节数据
** @The notes : 读出的数据是若干个8位数
**===========================================================================================*/
extern u8 DEVEXTFLASHReadByte(u32 addr, u8 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHReadWord(u32 addr, u16 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:读出的数据块指针 size:数据长度(最大一块32k)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 从外部Flash指定地址读取任意长度的16位单字数据
** @The notes : 读出的数据是若干个16位数
**===========================================================================================*/
extern u8 DEVEXTFLASHReadWord(u32 addr, u16 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHReadLong(u32 addr, u32 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:读出的数据块指针 size:数据长度(最大一块32k)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 从外部Flash指定地址读取任意长度的16位单字数据
** @The notes : 读出的数据是若干个16位数
**===========================================================================================*/
extern u8 DEVEXTFLASHReadLong(u32 addr, u32 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHEraseChip(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 外部Flash擦除整片Flash
** @The notes : 擦除64M
**===========================================================================================*/
extern u8 DEVEXTFLASHEraseChip(void);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHEraseBlock(u32 addr)
** @Input     : addr:Flash地址(0-0x3FFFFFF,必须是32k倍率)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 外部Flash擦除1块Flash
** @The notes : 擦除指定地址起32K
**===========================================================================================*/
extern u8 DEVEXTFLASHEraseBlock(u32 addr);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHEraseSector(u32 addr)
** @Input     : addr:Flash地址(0-0x3FFFFFF,必须是4k倍率)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 外部Flash擦除1扇区Flash(1页)
** @The notes : 擦除指定地址起4K
**===========================================================================================*/
extern u8 DEVEXTFLASHEraseSector(u32 addr);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHGetReadyFlag(void)
** @Input     : void
** @Output    : 0:未就绪 1:就绪
** @Function  : 获取外部Flash就绪状态标志
** @The notes : 与外部Flash芯片通讯状态
**===========================================================================================*/
extern u8 DEVEXTFLASHGetReadyFlag(void);

//--------------------------------------FLASH模块 end------------------------------------------

//--------------------------------------GPIO模块 start-----------------------------------------
/*=============================================================================================
** @Name      : u8 BSPGPIOInit(eGPIOPORT port, eGPIOPIN pin, eGPIOPORTMUX portmode, u8 dir)
** @Input     : port:端口 pin:第几个引脚  portmode:管脚类型  dir:方向(0输入 1输出)
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : TRUE:初始化成功 FALSE:初始化失败
** @The notes : 用户调用
**===========================================================================================*/
extern u8 BSPGPIOInit(eGPIOPORT port, eGPIOPIN pin, eGPIOPORTMUX portmode, u8 dir);

/*=============================================================================================
** @Name      : u8 BSPGPIOSetPortState(eGPIOPORT port, eGPIOPIN pin, e_IOState data)
** @Input     : port:端口 pin:第几个引脚 data:端口值(0低电平 1高电平)
** @Output    : TRUE:设置成功 FALSE:设置失败
** @Function  : 设置GPIO端口输出电平值
** @The notes : 用户调用
**===========================================================================================*/
extern u8 BSPGPIOSetPortState(eGPIOPORT port, eGPIOPIN pin, e_IOState data);

/*=============================================================================================
** @Name      : u8 BSPGPIOGetOutPortState(eGPIOPORT port, eGPIOPIN pin)
** @Input     : port:端口     pin:第几个引脚
** @Output    : TRUE:设置成功 FALSE:设置失败
** @Function  : GPIO端口当前电平值[0低电平,1高电平] 0xff:参数输入错误
** @The notes : 用户调用
**===========================================================================================*/
extern u8 BSPGPIOGetOutPortState(eGPIOPORT port, eGPIOPIN pin);

/*=============================================================================================
** @Name      : u8 BSPGPIOGetInPortState(eGPIOPORT port, eGPIOPIN pin)
** @Input     : port:端口     pin:第几个引脚
** @Output    : GPIO端口当前电平值[0低电平,1高电平] 0xff:参数输入错误
** @Function  : 查询GPIO输入端口当前电平值
** @The notes : 用户调用
**===========================================================================================*/
extern u8 BSPGPIOGetInPortState(eGPIOPORT port, eGPIOPIN pin);

/*=============================================================================================
** @Name      : u8 DEVGPIOInit(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : GPIO端口初始化
** @The notes : 只初始化板级提供的资源
**===========================================================================================*/
extern u8 DEVGPIOInit(void);

/*=============================================================================================
** @Name      : void DEVGPIOMulPortManagTask(void)
** @Input     : void
** @Output    : void
** @Function  : GPIO复用端口管控任务
** @The notes : 10ms
**===========================================================================================*/
extern void DEVGPIOMulPortManagTask(void);

/*=============================================================================================
** @Name      : u8 DEVGPIOGetInPortState(e_INPUTPORT port)
** @Input     : channel:高边驱动通道序号 curr:回检电流 0.01A
** @Output    : void
** @Function  : 检测高边驱动电流状态状态
** @The notes : 高边驱动电流判断接触器状态
**===========================================================================================*/
extern void DEVGPIOCheckHSCurrState(u8 channel, u16 curr);

/*=============================================================================================
** @Name      : u8 DEVGPIOGetInPortState(e_INPUTPORT port)
** @Input     : port:输入端口号
** @Output    : 端口状态[0:断开 1:闭合]
** @Function  : 获取GPIO输入端口状态
** @The notes :
**===========================================================================================*/
extern u8 DEVGPIOGetInPortState(e_INPUTPORT port);

/*=============================================================================================
** @Name      : u8 DEVGPIOSetPortState(e_OutputPort port, e_GPIOState state)
** @Input     : port:输出端口号 state:输出状态[0:断开 1:闭合]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 设置GPIO输出端口状态
** @The notes :
**===========================================================================================*/
extern u8 DEVGPIOSetPortState(e_OutputPort port, e_GPIOState state);

/*=============================================================================================
** @Name      : u8 DEVGPIOGetOutPortState(e_OutputPort port)
** @Input     : port:输出端口号
** @Output    : 端口状态[0:断开 1:闭合]
** @Function  : 获取GPIO输出端口状态
** @The notes :
**===========================================================================================*/
extern u8 DEVGPIOGetOutPortState(e_OutputPort port);

/*=============================================================================================
** @Name      : u16 DEVGPIOGetInputDryState(void)
** @Input     : void
** @Output    : 按位表示各输入干接点端口状态[0:断开 1:闭合]
** @Function  : 获取所有输入干接点端口状态
** @The notes : 共6路
**===========================================================================================*/
extern u16 DEVGPIOGetInputDryState(void);

/*=============================================================================================
** @Name      : u8 DEVGPIOGetHWVersion(void)
** @Input     : void
** @Output    : 按位表示硬件版本号
** @Function  : 获取硬件版本号
** @The notes : 共3路
**===========================================================================================*/
extern u8 DEVGPIOGetHWVersion(void);

/*=============================================================================================
** @Name      : u8 DEVGPIOGetSwhBackState(void)
** @Input     : void
** @Output    : 按位表示回检信号状态
** @Function  : 获取高低边开关回检信号状态
** @The notes : 共3路(2路低边1路高边K9)
**===========================================================================================*/
extern u8 DEVGPIOGetSwhBackState(void);

/*=============================================================================================
** @Name      : u8 DEVGPIOGetOutputDryState(void)
** @Input     : void
** @Output    : 按位表示出干接点当前状态[0:断开 1:闭合]
** @Function  : 获取输出干接点状态
** @The notes : 共3路
**===========================================================================================*/
extern u8 DEVGPIOGetOutputDryState(void);

/*=============================================================================================
** @Name      : u16 DEVGPIOGetCtrlHSwitchState(void)
** @Input     : void
** @Output    : 按位表示高边开关当前控制状态[0:断开 1:闭合]
** @Function  : 获取控制接触器高边开关状态
** @The notes : 共9路
**===========================================================================================*/
extern u16 DEVGPIOGetCtrlHSwitchState(void);

/*=============================================================================================
** @Name      : u16 DEVGPIOGetCtrlLSwitchState(void)
** @Input     : void
** @Output    : 按位表示低边开关当前控制状态[0:断开 1:闭合]
** @Function  : 获取控制接触器低边开关状态
** @The notes : 共2路
**===========================================================================================*/
extern u16 DEVGPIOGetCtrlLSwitchState(void);

//--------------------------------------GPIO模块 end-------------------------------------------

//--------------------------------------SPI模块 start------------------------------------------
/*=============================================================================================
** @Name      : u8 BSPSPIGetWorkState(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : SPI工作状态 0:空闲 1:忙
** @Function  : 获取SPI工作状态
** @The notes : 用户调用
**===========================================================================================*/
extern u8 BSPSPIGetWorkState(e_SPINum spiNum);

/*=============================================================================================
** @Name      : u8 BSPSPISendData(e_SPINum spiNum, u16 data)
** @Input     : spiNum:SPI通道号 data 需要发送的数据
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SPI发送数据
** @The notes : 用户调用
**===========================================================================================*/
extern u8 BSPSPISendData(e_SPINum spiNum, u16 data);

/*=============================================================================================
** @Name      : u16 BSPSPIGetRcvData(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : void
** @Function  : 获得SPI接收到寄存器的数据
** @The notes : 用户调用
**===========================================================================================*/
extern u16 BSPSPIGetRcvData(e_SPINum spiNum);

//--------------------------------------SPI模块 end--------------------------------------------

//--------------------------------------模拟IIC模块 start--------------------------------------
/*=============================================================================================
** @Name      : void BSPSimIICInit(e_SimI2cChann Channel)
** @Input     : Channel:模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : void
** @Function  : 模拟IIC通信初始化
** @The notes :
**===========================================================================================*/
extern void BSPSimIICInit(e_SimI2cChann Channel);

/*=============================================================================================
** @Name      : u8 BSPSimIICWriteBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
** @Input     : Channel:IIC通道号[1:RTC 2:EEPROM], DevAddr设备地址, RegAddr寄存器地址, *Data需要写入的数据, Length写入数据的长度
** @Output    : IIC写数据成功标志0不成功1成功
** @Function  : 模拟IIC写一块带CRC校验的数据
** @The notes :
**===========================================================================================*/
extern u8 BSPSimIICWriteBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length);

/*=============================================================================================
** @Name      : u8 BSPSimIICReadBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
** @Input     : Channel:IIC通道号[1:RTC 2:EEPROM], DevAddr是设备地址, RegAddr是寄存器地址, *data是读出的数据, Length是读的数据长度
** @Output    : IIC读数据成功标志0不成功1成功
** @Function  : 模拟IIC读一块带CRC校验的数据
** @The notes :
**===========================================================================================*/
extern u8 BSPSimIICReadBlockByCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length);

/*=============================================================================================
** @Name      : u8 BSPSimIICWriteBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
** @Input     : Channel:IIC通道号[1:RTC 2:EEPROM], DevAddr设备地址, RegAddr寄存器地址, *Data需要写入的数据, Length写入数据的长度
** @Output    : IIC写数据成功标志0不成功1成功
** @Function  : 模拟IIC写无CRC校验的数据
** @The notes :
**===========================================================================================*/
extern u8 BSPSimIICWriteBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length);

/*=============================================================================================
** @Name      : u8 BSPSimIICReadBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length)
** @Input     : Channel:IIC通道号[1:RTC 2:EEPROM], DevAddr是设备地址, RegAddr是寄存器地址, *data是读出的数据 dataLen是读的数据长度
** @Output    : IIC读数据成功标志0不成功1成功
** @Function  : 模拟IIC读一块无CRC校验的数据
** @The notes :
**===========================================================================================*/
extern u8 BSPSimIICReadBlockNoCRC(e_SimI2cChann Channel, u8 DevAddr, u8 RegAddr, u8 *Data, u8 Length);

/*=============================================================================================
** @Name      : u8 BSPSimIICWriteData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data)
** @Input     : Chan:选择模拟IIC通道[1:RTC 2:EEPROM] devID是设备地址 regLen是寄存器长度 regAddr是寄存器地址 dataLen是写的数据长度  *data是写的数据
** @Output    : 写数据结果:FALSE失败 TRUE:成功
** @Function  : 使用模拟IIC向指定寄存器和地址写入数据
** @The notes : RTC、EEPROM模拟IIC写数据
**===========================================================================================*/
extern u8 BSPSimIICWriteData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data);

/*=============================================================================================
** @Name      : u8 BSPSimIICReadData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data)
** @Input     : Chan:选择模拟IIC通道[1:RTC 2:EEPROM] devID是设备地址 regLen是寄存器长度 regAddr是寄存器地址 dataLen是读的数据长度  *data是读出的数据
** @Output    : 读数据结果:FALSE失败 TRUE:成功
** @Function  : 模拟IIC向指定寄存器和地址读取数据
** @The notes : RTC、EEPROM模拟IIC读数据
**===========================================================================================*/
extern u8 BSPSimIICReadData(e_SimI2cChann chan, u8 devID, u8 regLen, u16 regAddr, u8 dataLen, u8 *data);

/*=============================================================================================
** @Name      : void BSPSimIICStart(u8 Channel)
** @Input     : Channel选择模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : void
** @Function  : 模拟IIC启动
** @The notes :
**===========================================================================================*/
extern void BSPSimIICStart(u8 Channel);

/*=============================================================================================
** @Name      : void BSPSimIICStop(u8 Channel)
** @Input     : Channel选择模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : void
** @Function  : 模拟IIC停止
** @The notes :
**===========================================================================================*/
extern void BSPSimIICStop(u8 Channel);

/*=============================================================================================
** @Name      : u8 BSPSimIICSendByte(u8 Channel, u8 Data)
** @Input     : Channel:选择模拟IIC通道[1:RTC 2:EEPROM] Data:发送字节
** @Output    : Ack从器件应答值
** @Function  : 模拟IIC向设备发送一个字节(高位先发)
** @The notes : 注意在中断函数里处理的函数不能再次打开全局中断
**===========================================================================================*/
extern u8 BSPSimIICSendByte(u8 Channel, u8 Data);

/*=============================================================================================
** @Name      : u8 BSPSimIICRecvByte(u8 Ack, u8 Channel)
** @Input     : Ack:是否向从机应答，Channel:选择模拟IIC通道[1:RTC 2:EEPROM]
** @Output    : 读取到的字节
** @Function  : 模拟IIC向设备读取一个字节(先读高位)
** @The notes : 注意在中断函数里处理的函数不能再次打开全局中断
**===========================================================================================*/
extern u8 BSPSimIICRecvByte(u8 Ack, u8 Channel);

//--------------------------------------模拟IIC模块 end----------------------------------------

//--------------------------------------ADC模块 start------------------------------------------
/*=============================================================================================
** @Name      : u8 BSPADCInit(e_ADCModule module, e_ADCChan *chan, u8 num)
** @Input     : module: ADC模块 *chan需要初始化的ADC通道序列 num:通道数目
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : ADC驱动模块初始化
** @The notes : 用户调用
**===========================================================================================*/
extern u8 BSPADCInit(e_ADCModule module, e_ADCChan *chan, u8 num);

/*=============================================================================================
** @Name      : void BSPADCStart(e_ADCModule module, e_ADCChan channel)
** @Input     : channel:ADC通道号
** @Output    : void
** @Function  : 启动ADC转换
** @The notes : 用户调用
**===========================================================================================*/
extern void BSPADCStart(e_ADCModule module, e_ADCChan channel);

/*=============================================================================================
** @Name      : u16 BSPADCGetSampData(e_ADCModule module, e_ADCChan channel)
** @Input     : module:ADC模块	channel:ADC通道号
** @Output    : 采样值
** @Function  : 获取ADC采样AD值(已滤波)
** @The notes : 用户调用
**===========================================================================================*/
extern u16 BSPADCGetSampData(e_ADCModule module, e_ADCChan channel);

/*=============================================================================================
** @Name      : u8 BSPADCGetADCState(void)
** @Input     : void
** @Output    : 0:空闲 1:忙
** @Function  : 获取ADC采样通道工作状态
** @The notes : 用户调用
**===========================================================================================*/
extern u8 BSPADCGetWorkState(void);

/*=============================================================================================
** @Name      : void DEVADCStart(e_DEVADCMod module, e_DevADCId channel)
** @Input     : channel:ADC通道序号
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 启动ADC通道转换
** @The notes : 用户调用
**===========================================================================================*/
extern void DEVADCStart(e_DEVADCMod module, e_DevADCId channel);

/*=============================================================================================
** @Name      : u16 DEVADCGetSampAD(e_ADCModule module, e_DevADCId channel)
** @Input     : channel:ADC通道序号
** @Output    : 采样值
** @Function  : 获取ADC采样AD值(已滤波)
** @The notes : 用户调用
**===========================================================================================*/
extern u16 DEVADCGetSampAD(e_DEVADCMod module, e_DevADCId channel);

//--------------------------------------ADC模块 end--------------------------------------------

//--------------------------------------PWM模块 start------------------------------------------
/*=============================================================================================
** @Name      : u32 BSPReadPWMfrequencyAPI(void)
** @Input     : void
** @Output    : PWM周期：1HZ
** @Function  : 读取输入PWM周期
** @The notes : PWM捕获
**===========================================================================================*/
u32 BSPReadPWMfrequencyAPI(void);

//--------------------------------------PWM模块 end--------------------------------------------

//--------------------------------------SD卡模块 start-----------------------------------------

//--------------------------------------SD卡模块 end-------------------------------------------

//--------------------------------------内部RTC模块 start--------------------------------------
/*=============================================================================================
** @Name      : u8 BSPRTCSetCurrSecTime(rtc_Currtimedate_t *time)
** @Input     : time:当前时间(1s)
** @Output    : TRUE:设置成功 FALSE:设置失败
** @Function  : 设置内部RTC当前时间
** @The notes : 用户调用
**===========================================================================================*/
extern u8 BSPRTCSetCurrSecTime(rtc_Currtimedate_t *time);

/*=============================================================================================
** @Name      : rtc_Currtimedate_t BSPRTCGetCurrSecTime(void)
** @Input     : void
** @Output    : 当前内部RTC时间
** @Function  : 获取内部RTC当前时间
** @The notes : 用户调用
**===========================================================================================*/
extern rtc_Currtimedate_t BSPRTCGetCurrSecTime(void);

//--------------------------------------内部RTC模块 end----------------------------------------

//--------------------------------------外部RTC模块 start--------------------------------------
/*=================================================================================================
** @Name      : u8 DEVRTCRX8010Init(void)
** @Input     : void
** @Output    : 初始化结果:TRUE:成功 FALSE:失败
** @Function  : 外部RTC芯片RX8010初始化
** @The notes : 用户调用
**===============================================================================================*/
extern u8 DEVRTCRX8010Init(void);

/*=============================================================================================
** @Name      : void DEVEXTRTCCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 外部RTC检测任务
** @The notes : 4s
**===========================================================================================*/
extern void DEVEXTRTCCheckTask(void);

/*=================================================================================================
** @Name      : void DEVRTCResetRX8010API(void)
** @Input     : void
** @Output    : void
** @Function  : 外部RTC芯片RX8010复位
** @The notes : RX8010总线锁死后复位总线
**===============================================================================================*/
extern void DEVRTCResetRX8010API(void);

/*=================================================================================================
** @Name      : u8 DEVEXTRTCGetNowClockAPI(t_SysClock *rtcClock)
** @Input     : *rtcClock:读取的RTC时钟信息
** @Output    : 读结果:TRUE:成功 FALSE:失败
** @Function  : 从外部RTC芯片RX8010获取当前时钟
** @The notes : 当读取的数据非法时,不更新*rtcClock的数据
**===============================================================================================*/
extern u8 DEVEXTRTCGetNowClockAPI(t_SysClock *rtcClock);

/*=================================================================================================
** @Name      : u8 DEVEXTRTCSetNowClockAPI(t_SysClock *aimClock)
** @Input     : *aimClock:设置的目标时钟
** @Output    : 设置结果:TRUE:成功 FALSE:失败
** @Function  : 设置外部RTC芯片RX8010的当前时钟
** @The notes :
**===============================================================================================*/
extern u8 DEVEXTRTCSetNowClockAPI(t_SysClock *aimClock);

/*=============================================================================================
** @Name      : u8 DEVEXTRTCGetReadyFlagAPI(void)
** @Input     : void
** @Output    : 0:未就绪 1:就绪
** @Function  : 获取外部RTC就绪状态标志
** @The notes : 与外部RTC芯片通讯状态
**===========================================================================================*/
extern u8 DEVEXTRTCGetReadyFlagAPI(void);

//--------------------------------------外部RTC模块 end----------------------------------------

//--------------------------------------喂狗模块 start-----------------------------------------
/*=============================================================================================
** @Name      : void BSPWatchDogOutput(void)
** @Input     : void
** @Output    : void
** @Function  : 直接输出喂狗
** @The notes : 直接设置喂狗
**===========================================================================================*/
extern void BSPWatchDogOutput(void);

//--------------------------------------喂狗模块 end-------------------------------------------

//--------------------------------------中断模块 start-----------------------------------------
/*=============================================================================================
** @Name      : void BSP_DISABLE_IRQ(void)
** @Input     : void
** @Output    : void
** @Function  : 禁止中断
** @The notes : 进入临界区域
**===========================================================================================*/
extern void BSP_DISABLE_IRQ(void);

/*=============================================================================================
** @Name      : void BSP_ENABLE_IRQ(void)
** @Input     : void
** @Output    : void
** @Function  : 使能中断
** @The notes : 退出临界区域
**===========================================================================================*/
extern void BSP_ENABLE_IRQ(void);

//--------------------------------------中断模块 end--------------------------------------------

//--------------------------------------电源SBC模块 start---------------------------------------
/*=============================================================================================
** @Name      : void FS65SetUserPwrOffFlagAPI(u8 flag)
** @Input     : flag:用户下电请求标志:0上电请求 1:下电请求
** @Output    : void
** @Function  : 设置FS6503用户下电请求标志
** @The notes : 请求下电时进入软件休眠模式
**===========================================================================================*/
extern void FS65SetUserPwrOffFlagAPI(u8 flag);

//--------------------------------------电源SBC模块 end-----------------------------------------

/*=============================================================================================
** @Name      : void BSPDisIRQUserInit(void)
** @Input     : void
** @Output    : void
** @Function  : 禁止中断后的用户初始化
** @The notes : 用户添加硬件和任务初始化等
**===========================================================================================*/
void BSPDisIRQUserInit(void);

/*=============================================================================================
** @Name      : void BSPEnIRQUserInit(void)
** @Input     : void
** @Output    : void
** @Function  : 使能中断后的用户初始化
** @The notes : 用户添加功能初始化等
**===========================================================================================*/
void BSPEnIRQUserInit(void);

/*=============================================================================================
** @Name      : u32 BSPGetOSMillSecTimeAPI(void)
** @Input     : void
** @Output    : RTOS毫秒计时时间 1ms
** @Function  : 获取RTOS毫秒计时时间
** @The notes :
**===========================================================================================*/
u32 BSPGetOSMillSecTimeAPI(void);

/*=============================================================================================
** @Name      : void BSPSetOSMicrosecDelayAPI(u32 time)
** @Input     : time:RTOS延时 1微秒
** @Output    : void
** @Function  : 设置RTOS等待延时
** @The notes :
**===========================================================================================*/
void BSPSetOSMicrosecDelayAPI(u32 time);

#endif
