/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : SST26VF064.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 外部FLash SST26VF064B驱动模块
** @Instructions : 独立硬件SPI通讯
**===========================================================================================*/
#ifndef _SST26VF064_H
#define _SST26VF064_H

//外部FLASH分配说明:
//总64Mbit(8Mbyte/256块/2048扇区,每块8扇区32k,每扇区4k,1Mbyte分为32块):Addr:0~8388607(0x800000);
//1、程序备份使用1M(32块/256扇区):    Addr:0~1048575。
//2、程序下载使用1M(32块/256扇区):    Addr:1048576(0x100000)~2097151。
//3、累记故障记录使用1M(32块/256扇区):Addr:2097152(0x200000)~3145727。
//4、累记事件记录使用1M(32块/256扇区):Addr:3145728(0x300000)~4194303。
//5、累记录波记录使用1M(32块/256扇区):Addr:4194304(0x400000)~5242879。
//6、累记数据记录使用2M(64块/512扇区):Addr:5242880(0x500000)~7340031。
//7、累记DOD记录使用64K(2块/16扇区):  Addr:7340032(0x700000)~7356415。
//8、预留暂未使用960K(30块/240扇区):  Addr:7356416(0x704000)~8384511。
//9、外部FLASH校验使用4Kbyte(1扇区):  Addr:8384512(0x7ff000)~8388607。

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypedef.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define EXT_FLA_PAGE_SIZE               4096            //外部FLASH一个扇区(页)大小(4k)
#define EXT_FLA_BLOCK_SIZE              32768           //外部FLASH划分块大小(32k,8页)
#define EXT_FLA_ADDR_SIZE               8388608         //外部FLASH地址容量大小(8Mbyte)
#define EXT_FLA_WR_MAX_SIZE             256             //外部FLASH一次最多写大小(256byte[芯片要求每写256byte需要重发一次地址])
#define EXT_FLA_RO_MAX_SIZE             1024            //外部FLASH一次最多读大小(1kbyte[需防止读耗时太长])

#define EXT_FLA_CRC_ADDR                8384512         //外部FLASH就绪校验地址(0x7FF000,最后1页)
#define EXT_FLA_CRC_FLAG                0x5a5a5a5a      //外部FLASH就绪校验标志

/* Private define ------------------------------------------------------------*/
#define SST26_CMD_READDATA              0x03            //外部FLASH命令-读数据
#define SST26_CMD_FST_RDATA             0x0b            //外部FLASH命令-高速读数据
#define SST26_CMD_WRITEDATA             0x02            //外部FLASH命令-写数据
#define SST26_CMD_WRITE_EN              0x06            //外部FLASH命令-写使能
#define SST26_CMD_WRITE_DIS             0x04            //外部FLASH命令-写禁止
#define SST26_CMD_WRITE_PRO             0x80            //外部FLASH命令-写保护(写状态寄存器)
#define SST26_CMD_WRPRO_CLR             0x00            //外部FLASH命令-写保护取消(写状态寄存器)
#define SST26_CMD_READSTATUS            0x05            //外部FLASH命令-读状态寄存器(或0x35)
#define SST26_CMD_WRITESTATUS           0x01            //外部FLASH命令-写状态寄存器
#define SST26_CMD_READDEVID             0x90            //外部FLASH命令-读设备ID(或0xab)
#define SST26_CMD_READJEDID             0x9f            //外部FLASH命令-读JEDEC-ID
#define SST26_CMD_ERASEPAG              0x20            //外部FLASH命令-擦除1扇区(页,4k)
#define SST26_CMD_ERASEBLO              0xd8            //外部FLASH命令-擦除1块(8页,32k)
#define SST26_CMD_ERASECHIP             0xc7            //外部FLASH命令-擦除芯片全部(或0x60)(2048块,64M)
#define SST26_CMD_BLOCK_UNLOCK          0x98            //外部FLASH命令-解除所有块保护

#define SST26_READ_DUMMY_BYTE           0xff            //读外部FLASH数据时发送空字节

#define SST26_STAREG_BUSY_BIT           0x81            //外部FLASH状态寄存器表示忙的状态位

#define SST26_DEV_ID                    0x00bf2643      //读外部FLASH设备地址(用于校验)

/*=============================================================================================
** @Data type definition
**===========================================================================================*/

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void EXTFlashSST26Init(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash初始化
** @The notes : 检测外部Flash芯片是否就绪
**===========================================================================================*/
void EXTFlashSST26Init(void);

/*=============================================================================================
** @Name      : void EXTFlashSST26Reset(u8 state)
** @Input     : state:复位状态 0休眠 1激活
** @Output    : void
** @Function  : 外部Flash复位
** @The notes : 设置外部Flash芯片是否激活
**===========================================================================================*/
void EXTFlashSST26Reset(u8 state);

/*=============================================================================================
** @Name      : void EXTFlashCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash检测任务
** @The notes : 1s
**===========================================================================================*/
void EXTFlashCheckTask(void);

/*=============================================================================================
** @Name      : u8 EXTFlashWriteByte(u32 addr, u8 data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) data:写入Flash数据
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将一个八位数据写入Flash指定地址
** @The notes :
**===========================================================================================*/
u8 EXTFlashWriteByte(u32 addr, u8 data);

/*=============================================================================================
** @Name      : u8 EXTFlashReadByte(u32 addr, u8 *data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *data:读出的数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 读Flash指定地址的数据
** @The notes : 读出数据是八位
**===========================================================================================*/
u8 EXTFlashReadByte(u32 addr, u8 *data);

/*=============================================================================================
** @Name      : u8 EXTFlashWriteWord(u32 addr, u16 data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) data:写入Flash数据
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将一个16位数据写入Flash指定地址
** @The notes :
**===========================================================================================*/
u8 EXTFlashWriteWord(u32 addr, u16 data);

/*=============================================================================================
** @Name      : u8 ExtFlashReadWord(u32 addr, u16 *data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *data:读出的数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 读Flash指定地址数据
** @The notes : 读出的数据是16位
**===========================================================================================*/
u8 EXTFlashReadWord(u32 addr, u16 *data);

/*=============================================================================================
** @Name      : u8 EXTFlashWriteLong(u32 addr, u32 data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) data:写入Flash的数据
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将一个32位数据写入Flash指定地址
** @The notes :
**===========================================================================================*/
u8 EXTFlashWriteLong(u32 addr, u32 data);

/*=============================================================================================
** @Name      : u8 EXTFlashReadLong(u32 addr, u32 *data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *data:读出的数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 读Flash指定地址的数据
** @The notes : 读出的数据是32位
**===========================================================================================*/
u8 EXTFlashReadLong(u32 addr, u32 *data);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHWriteByte(u32 addr, u8 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:写入Flash数据块 size:数据长度[1,256]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将8位字节数据写入到外部Flash中
** @The notes :
**===========================================================================================*/
u8 DEVEXTFLASHWriteByte(u32 addr, u8 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHWriteWord(u32 addr, u16 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:写入Flash数据块 size:数据长度[1,128]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将16位单字数据写入到外部Flash中
** @The notes :
**===========================================================================================*/
u8 DEVEXTFLASHWriteWord(u32 addr, u16 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHWriteLong(u32 addr, u32 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:写入Flash数据块 size:数据长度[1,64]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将32位双字数据写入到外部Flash中
** @The notes :
**===========================================================================================*/
u8 DEVEXTFLASHWriteLong(u32 addr, u32 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHReadByte(u32 addr, u8 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:读出的数据块指针 size:数据长度(最大一块32k)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 从外部Flash指定地址读取任意长度的8位字节数据
** @The notes : 读出的数据是若干个8位数
**===========================================================================================*/
u8 DEVEXTFLASHReadByte(u32 addr, u8 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHReadWord(u32 addr, u16 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:读出的数据块指针 size:数据长度(最大一块32k)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 从外部Flash指定地址读取任意长度的16位单字数据
** @The notes : 读出的数据是若干个16位数
**===========================================================================================*/
u8 DEVEXTFLASHReadWord(u32 addr, u16 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHReadLong(u32 addr, u32 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:读出的数据块指针 size:数据长度(最大一块32k)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 从外部Flash指定地址读取任意长度的16位单字数据
** @The notes : 读出的数据是若干个16位数
**===========================================================================================*/
u8 DEVEXTFLASHReadLong(u32 addr, u32 *pData, u32 size);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHEraseChip(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 外部Flash擦除整片Flash
** @The notes : 擦除64M
**===========================================================================================*/
u8 DEVEXTFLASHEraseChip(void);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHEraseBlock(u32 addr)
** @Input     : addr:Flash地址(0-0x3FFFFFF,必须是32k倍率)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 外部Flash擦除1块Flash
** @The notes : 擦除指定地址起32K
**===========================================================================================*/
u8 DEVEXTFLASHEraseBlock(u32 addr);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHEraseSector(u32 addr)
** @Input     : addr:Flash地址(0-0x3FFFFFF,必须是4k倍率)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 外部Flash擦除1扇区Flash(1页)
** @The notes : 擦除指定地址起4K
**===========================================================================================*/
u8 DEVEXTFLASHEraseSector(u32 addr);

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHGetReadyFlag(void)
** @Input     : void
** @Output    : 0:未就绪 1:就绪
** @Function  : 获取外部Flash就绪状态标志
** @The notes : 与外部Flash芯片通讯状态
**===========================================================================================*/
u8 DEVEXTFLASHGetReadyFlag(void);

#endif
