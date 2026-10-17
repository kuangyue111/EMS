/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPFLASH.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : Flash驱动函数模块
** @Instructions :
**===========================================================================================*/
#ifndef _BSPFLASH_H
#define _BSPFLASH_H

//=============================================================================================
//Flash分配说明:
//总Flash(S32K144): 512kbyte[0x0000000, 0x0080000),一个扇区(页)4Kbyte,块起始地址必须是4K(0x1000)倍数;
//其中:
//第1块:BOOT progr1:4kbyte   [0x0000000, 0x0001000);
//第2块:APP  progr :372kbyte [0x0001000, 0x005E000);
//第3块:BOOT progr2:100kbyte [0x005E000, 0x0077000);
//第4块:BOOT Param :4kbyte   [0x0077000, 0x0078000);
//第5块:APP  Param:32kbyte   [0x0078000, 0x0080000);

//总Flash(S32K146): 1Mkbyte[0x0000000, 0x0100000),一个扇区(页)4Kbyte,块起始地址必须是4K(0x1000)倍数;
//其中:
//第1块:BOOT progr1:4kbyte   [0x0000000, 0x0001000);
//第2块:APP  progr :392kbyte [0x0001000, 0x0063000);
//第3块:APP  progr :392kbyte [0x0063000, 0x00C5000);
//第4块:BOOT progr2:200kbyte [0x00C5000, 0x00F7000);
//第5块:BOOT Param :4kbyte   [0x00F7000, 0x00F8000);
//第6块:APP  Param :32kbyte  [0x00F8000, 0x0100000);
//=============================================================================================

/*=============================================================================================
** @Header file definition
**===========================================================================================*/

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define FLASH_BLOCK_NUM             (6)                                 /*flash块数目(每块Flash大小见说明)*/

#define FLASH_BLOCK0_ADDR_START     (0x0000000)                         /*flash块0起始地址*/
#define FLASH_BLOCK0_SIZE           (0x01000)                           /*flash块0地址大小(共4K,用于BOOT代码1)*/

#define FLASH_BLOCK1_ADDR_START     (0x0001000)                         /*flash块1起始地址[在线升级要求APP起始地址必须小于0xffff]*/
#define FLASH_BLOCK1_SIZE           (0x62000)                           /*flash块1地址大小(共392K,用于APP运行代码)*/

#define FLASH_BLOCK2_ADDR_START     (0x0063000)                         /*flash块2起始地址*/
#define FLASH_BLOCK2_SIZE           (0x62000)                           /*flash块2地址大小(共392K,用于APP备份代码)*/

#define FLASH_BLOCK3_ADDR_START     (0x00C5000)                         /*flash块3起始地址*/
#define FLASH_BLOCK3_SIZE           (0x32000)                           /*flash块3地址大小(共200K,用于BOOT代码2)*/

#define FLASH_BLOCK4_ADDR_START     (0x00F7000)                         /*flash块4起始地址*/
#define FLASH_BLOCK4_SIZE           (0x1000)                            /*flash块4地址大小(共4K,用于BOOT参数)*/

#define FLASH_BLOCK5_ADDR_START     (0x00F8000)                         /*flash块5起始地址*/
#define FLASH_BLOCK5_SIZE           (0x8000)                            /*flash块5大小(共32K,用于APP参数)*/

#define FLASH_ADDR_START            (0x0000000)                         /*flash起始地址*/
#define FLASH_ADDR_SIZE             (FLASH_BLOCK0_SIZE + FLASH_BLOCK1_SIZE + FLASH_BLOCK2_SIZE + FLASH_BLOCK3_SIZE + FLASH_BLOCK4_SIZE + FLASH_BLOCK5_SIZE)/*flash地址总大小*/

#define FLASH_PAGE_SIZE             ((u16)0x1000)                       /*flash一个扇区(一页)的大小为0x1000字节(4K)*/

/*FLASH解锁键值*/
//#define RDP_Key                   ((uint16_t)0x00A5)
//#define FLASH_KEY1                ((uint32_t)0x45670123)
//#define FLASH_KEY2                ((uint32_t)0xCDEF89AB)

/*=============================================================================================
** @Data type definition
**===========================================================================================*/

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPINTFLASHInit(u8 oscClock)
** @Input     : oscClock:晶振时钟
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : 内部FLASH驱动模块初始化
** @The notes :
**===========================================================================================*/
u8 BSPINTFLASHInit(u8 oscClock);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseChip(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除整个MCU全部FLASH
** @The notes : 用户慎用
**===========================================================================================*/
u8 BSPINTFLASHEraseChip(void);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseBlock(u16 block)
** @Input     : u16 sector:FLASH块号[0,5]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除FLASH块
** @The notes : 按块擦除(按用途划分1块大小各不同[4K/392K/392K/200K/4K/32K])
**===========================================================================================*/
u8 BSPINTFLASHEraseBlock(u16 block);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseSector(u32 addr)
** @Input     : addr:FLASH地址[0,1048575](1M)
** @Output    : TRUE:扇区擦除成功 FALSE:扇区擦除错误
** @Function  : 擦除FLASH扇区(一页)
** @The notes : 1个扇区为4096Byte(4K),地址必须是4096倍数
**===========================================================================================*/
u8 BSPINTFLASHEraseSector(u32 addr);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWriteByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数(必须是8的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数
**===========================================================================================*/
u8 BSPINTFLASHWriteByte(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWriteWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数(必须是4的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字方式写入数据(16位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数,单次最大写1Kbyte.
**===========================================================================================*/
u8 BSPINTFLASHWriteWord(u32 addr, u16 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWriteLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数(必须是2的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按双字方式写入数据(32位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数,单次最大写1Kbyte.
**===========================================================================================*/
u8 BSPINTFLASHWriteLong(u32 addr, u32 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadByte(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadWord(u32 addr, u16 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按双字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadLong(u32 addr, u32 *pData, u32 len);

#endif
