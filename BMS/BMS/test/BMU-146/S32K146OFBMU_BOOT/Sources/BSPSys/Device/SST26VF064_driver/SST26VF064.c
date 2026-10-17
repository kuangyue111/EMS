/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : SST26VF064.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 外部FLash SST26VF064B驱动模块
** @Instructions : 独立硬件SPI通讯
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "DEVGPIO.h"
#include "BSPSPI.h"
#include "stdio.h"
#include "stdlib.h"
#include "SST26VF064.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u8 sExtFlashErr = 0;     					//外部Flash异常状态 1异常
static u8 sExtFlashReady = 0;   					//外部Flash就绪状态 1就绪
static u8 sExtWRData[EXT_FLA_RO_MAX_SIZE] = {0};	//外部Flash写数据缓存区

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void EXTFlash_SPI_CSUP(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash SPI通讯片选高
** @The notes :
**===========================================================================================*/
static void EXTFlash_SPI_CSUP(void);

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_CSDN(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash SPI通讯片选低
** @The notes :
**===========================================================================================*/
static void EXTFlash_SPI_CSDN(void);

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteEnable(void)
** @Input     : void
** @Output    : void
** @Function  : 发送外部Flash命令写使能
** @The notes : 带片选
**===========================================================================================*/
static void EXTFlash_SPI_WriteEnable(void);

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteDisable(void)
** @Input     : void
** @Output    : void
** @Function  : 发送外部Flash命令写禁止
** @The notes : 带片选
**===========================================================================================*/
static void EXTFlash_SPI_WriteDisable(void);

/*=============================================================================================
** @Name      : static u8 EXTFlash_SPI_SendSST26Cmd(u8 cmd)
** @Input     : void
** @Output    : 发送结果 TRUE:成功 FALSE:失败
** @Function  : SPI发送外部Flash SST26读写命令
** @The notes : 附带检测通讯是否正常,带片选
**===========================================================================================*/
static u8 EXTFlash_SPI_SendSST26Cmd(u8 cmd);
#if 0
/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteStatReg(u8 regData)
** @Input     : regData:写状态寄存器的值
** @Output    : void
** @Function  : 发送外部Flash命令写状态寄存器
** @The notes : 带片选
**===========================================================================================*/
static void EXTFlash_SPI_WriteStatReg(u8 regData);
#endif
/*=============================================================================================
** @Name      : static u8 EXTFlash_SPI_ReadStatReg(void)
** @Input     : void
** @Output    : 状态寄存器状态
** @Function  : 发送外部Flash命令读状态寄存器
** @The notes : 带片选
**===========================================================================================*/
static u8 EXTFlash_SPI_ReadStatReg(void);

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_ReadDevId(void)
** @Input     : regData:写状态寄存器的值
** @Output    : void
** @Function  : 读外部Flash设备ID
** @The notes : 带片选
**===========================================================================================*/
static u32 EXTFlash_SPI_ReadDevId(void);

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteByte(u32 addr, u8 data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) data:所写数据
** @Output    : void
** @Function  : 写外部Flash一个字节数据
** @The notes :
**===========================================================================================*/
static void EXTFlash_SPI_WriteByte(u32 addr, u8 data);

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteSector(u32 addr, u8 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:所写数据块指针 size:页内字节长度(最大一页4kbyte)
** @Output    : void
** @Function  : 写外部Flash一个扇区(一页)内数据
** @The notes : 数据长度不能跨过两个扇区(页)
**===========================================================================================*/
static void EXTFlash_SPI_WriteSector(u32 addr, u8 *pData, u32 size);

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WaitForWriteEnd(void)
** @Input     : void
** @Output    : void
** @Function  : 等待外部Flash操作结束
** @The notes : 带片选
**===========================================================================================*/
static void EXTFlash_SPI_WaitBusyEnd(void);

#if 0
/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteProtect(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash写保护
** @The notes : 写状态寄存器,休眠,带片选
**===========================================================================================*/
static void EXTFlash_SPI_WriteProtect(void);
#endif

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteProClr(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash写保护取消
** @The notes : 写状态寄存器,激活,带片选
**===========================================================================================*/
static void EXTFlash_SPI_WriteProClr(void);

/*=============================================================================================
** @Name      : static u8 EXTFlashCheckIcReadyState(void)
** @Input     : void
** @Output    : 就绪状态 0:未就绪 1:就绪 2:故障
** @Function  : 检测外部Flash就绪状态
** @The notes : 通过读写校验标志判断
**===========================================================================================*/
static u8 EXTFlashCheckIcReadyState(void);

/*=============================================================================================
** @Name      : static u8 EXTFlashWriteBlockProtection(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash写保护操作函数
** @The notes :
**===========================================================================================*/
static u8 EXTFlashWriteBlockProtection(void);

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_BlockProtectUnlock(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash取消所有块保护
** @The notes :
**===========================================================================================*/
static void EXTFlash_SPI_BlockProtectUnlock(void);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void EXTFlashSST26Init(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash初始化
** @The notes : 检测外部Flash芯片是否就绪
**===========================================================================================*/
void EXTFlashSST26Init(void)
{
    u8 state = 0;
    u8 data[4] = {0};
    
    //DEVGPIOSetPortState(ePOut_FlaWPr, ePSTATE_ON);  //取消写保护(低电平写保护)
    //DEVGPIOSetPortState(ePOut_FlaCS, ePSTATE_ON);   //片选上拉
    
    EXTFlash_SPI_WriteProClr();                       //取消写保护(上电默认全片写保护)
    
    //检测外部Flash状态
    state = EXTFlashCheckIcReadyState();
    
    //外部Flash校验未就绪则重写校验码
    if(0 == state)
    {
        //擦除芯片
    	DEVEXTFLASHEraseSector(EXT_FLA_CRC_ADDR);
        
        //取得标志
        data[0] = EXT_FLA_CRC_FLAG & 0xff;
        data[1] = (EXT_FLA_CRC_FLAG >> 8) & 0xff;
        data[2] = (EXT_FLA_CRC_FLAG >> 16) & 0xff;
        data[3] = (EXT_FLA_CRC_FLAG >> 24) & 0xff;
        
        //重写校验标志
        EXTFlash_SPI_WriteSector(EXT_FLA_CRC_ADDR, data, 4);
        
        //重新校验正确则就绪
        if(1 == EXTFlashCheckIcReadyState())
        {
            //外部Flash初始化就绪
            sExtFlashReady = 1;
        }
    }
    //外部Flash校验正确则就绪
    else if(1 == state)
    {
        //外部Flash初始化就绪
        sExtFlashReady = 1;
    }
    //外部Flash校验错误
    else
    {
        //外部Flash初始化故障
        sExtFlashReady = 0;
    }
}

/*=============================================================================================
** @Name      : void EXTFlashSST26Reset(u8 state)
** @Input     : state:复位状态 0休眠 1激活
** @Output    : void
** @Function  : 外部Flash复位
** @The notes : 设置外部Flash芯片是否激活
**===========================================================================================*/
void EXTFlashSST26Reset(u8 state)
{
    //激活
    if(1 == state)
    {
        //重新初始化
        EXTFlashSST26Init();
    }
    //禁用
    else
    {
        //休眠芯片
        sExtFlashReady = 0;
        DEVGPIOSetPortState(ePOut_FlaWPr, ePSTATE_OFF);   //写保护
        DEVGPIOSetPortState(ePOut_FlaCS, ePSTATE_ON);     //片选上拉
    }
}

/*=============================================================================================
** @Name      : void EXTFlashCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash检测任务
** @The notes : 1s
**===========================================================================================*/
void EXTFlashCheckTask(void)
{
    //外部Flash异常
    if(sExtFlashErr > 0)
    {
        //外部Flash未就绪
        sExtFlashReady = 0;
        
        //通讯异常
        if(1 == BitGet(sExtFlashErr, 0))
        {
            //重新初始化
            EXTFlashSST26Init();
        }
        //写异常
        else if(1 == BitGet(sExtFlashErr, 1))
        {
            //重新等待写结束
            BitClr(sExtFlashErr, 1);
            EXTFlash_SPI_WaitBusyEnd();
        }
    }
    else if(0 == sExtFlashReady)
    {
        //重新初始化
        EXTFlashSST26Init();
    }
}

/*=============================================================================================
** @Name      : u8 EXTFlashWriteByte(u32 addr, u8 data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) data:写入Flash数据
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将一个八位数据写入Flash指定地址
** @The notes :
**===========================================================================================*/
u8 EXTFlashWriteByte(u32 addr, u8 data)
{
    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }
    
    //地址超过最大地址容量(64M)
    if(addr >= EXT_FLA_ADDR_SIZE)
    {
        return(FALSE);
    }
    
    //直接写8位数据
    EXTFlash_SPI_WriteByte(addr, data);
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 EXTFlashReadByte(u32 addr, u8 *data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *data:读出的数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 读Flash指定地址的数据
** @The notes : 读出数据是八位
**===========================================================================================*/
u8 EXTFlashReadByte(u32 addr, u8 *data)
{
    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }
    
    //地址超过最大地址容量(64M)
    if(addr >= EXT_FLA_ADDR_SIZE)
    {
        return(FALSE);
    }
    
    /* Select the FLASH: Chip Select low */
    EXTFlash_SPI_CSDN();
    
    /* 发送读命令 */
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_READDATA))
    /* 发送高速读命令 */
    //if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_FST_RDATA))
    {
        //发送失败
        return(FALSE);
    }
    
    /* 发送24位寄存器地址(高位先发) */
    (void)BSPSPISendData(eSPI3, (addr >> 16) & 0xFF);
    (void)BSPSPISendData(eSPI3, (addr >> 8) & 0xFF);
    (void)BSPSPISendData(eSPI3, addr & 0xFF);
    //(void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据(高速读需要)
    
    /* Read a byte from the FLASH */
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据
    *data = BSPSPIGetRcvData(eSPI3);                    //接收数据
    
    /* Deselect the FLASH: Chip Select high */
    EXTFlash_SPI_CSUP();
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 EXTFlashWriteWord(u32 addr, u16 data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) data:写入Flash数据
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将一个16位数据写入Flash指定地址
** @The notes :
**===========================================================================================*/
u8 EXTFlashWriteWord(u32 addr, u16 data)
{
    u8 wrData[2] = {0};
    
    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }
    
    //地址超过最大地址容量(64M)
    if((addr + 1) >= EXT_FLA_ADDR_SIZE)
    {
        return(FALSE);
    }
    
    //转化为8位数据(低位在前[小端模式])
    wrData[0] = data & 0xff;
    wrData[1] = (data >> 8) & 0xff;
    
    //判断写地址是否跨两个扇区
    if((addr / EXT_FLA_PAGE_SIZE) != ((addr + 1) / EXT_FLA_PAGE_SIZE))
    {
        //跨俩扇区分两次写
        EXTFlash_SPI_WriteSector(addr, &wrData[0], 1);
        EXTFlash_SPI_WriteSector(addr + 1, &wrData[1], 1);
    }
    //只写一个扇区
    else
    {
        //直接写完
        EXTFlash_SPI_WriteSector(addr, wrData, 2);
    }
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 ExtFlashReadWord(u32 addr, u16 *data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *data:读出的数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 读Flash指定地址数据
** @The notes : 读出的数据是16位
**===========================================================================================*/
u8 EXTFlashReadWord(u32 addr, u16 *data)
{
    u16 temp0 = 0;
    u16 temp1 = 0;
    
    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }
    
    //地址超过最大地址容量(64M)
    if((addr + 1) >= EXT_FLA_ADDR_SIZE)
    {
        return(FALSE);
    }
    
    /* Select the FLASH: Chip Select low */
    EXTFlash_SPI_CSDN();
    
    /* 发送读命令 */
    //if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_READDATA))
    /* 发送高速读命令 */
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_FST_RDATA))
    {
        //发送失败
        return(FALSE);
    }
    
    /* 发送24位寄存器地址(高位先发) */
    (void)BSPSPISendData(eSPI3, (addr >> 16) & 0xFF);
    (void)BSPSPISendData(eSPI3, (addr >> 8) & 0xFF);
    (void)BSPSPISendData(eSPI3, addr & 0xFF);
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据(高速读需要)
    
    /* Read a byte from the FLASH */
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据
    temp0 = BSPSPIGetRcvData(eSPI3);                    //接收数据
    
    /* Read a byte from the FLASH */
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据
    temp1 = BSPSPIGetRcvData(eSPI3);                    //接收数据
    
    //外部芯片按8位保存，读写16位/32位数据统一按照低位在前[小端模式]
    *data = (temp1 << 8) | temp0;                       //组合数据
    
    /* Deselect the FLASH: Chip Select high */
    EXTFlash_SPI_CSUP();
    
    return TRUE;
}

/*=============================================================================================
** @Name      : u8 EXTFlashWriteLong(u32 addr, u32 data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) data:写入Flash的数据
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将一个32位数据写入Flash指定地址
** @The notes :
**===========================================================================================*/
u8 EXTFlashWriteLong(u32 addr, u32 data)
{
    u8 wrData[4] = {0};
    
    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }
    
    //地址超过最大地址容量(64M)
    if((addr + 3) >= EXT_FLA_ADDR_SIZE)
    {
        return(FALSE);
    }
    
    //转化为8位数据(低位在前[小端模式])
    wrData[0] = data & 0xff;
    wrData[1] = (data >> 8) & 0xff;
    wrData[2] = (data >> 16) & 0xff;
    wrData[3] = (data >> 24) & 0xff;
    
    //判断写地址是否跨两个扇区
    //1-3分成两页
    if((addr / EXT_FLA_PAGE_SIZE) != ((addr + 1) / EXT_FLA_PAGE_SIZE))
    {
        //跨俩扇区分两次写
        EXTFlash_SPI_WriteSector(addr, &wrData[0], 1);
        EXTFlash_SPI_WriteSector(addr + 1, &wrData[1], 3);
    }
    //2-2分成两页
    else if((addr / EXT_FLA_PAGE_SIZE) != ((addr + 2) / EXT_FLA_PAGE_SIZE))
    {
        //跨俩扇区分两次写
        EXTFlash_SPI_WriteSector(addr, &wrData[0], 2);
        EXTFlash_SPI_WriteSector(addr + 2, &wrData[2], 2);
    }
    //3-1分成两页
    else if((addr / EXT_FLA_PAGE_SIZE) != ((addr + 3) / EXT_FLA_PAGE_SIZE))
    {
        //跨俩扇区分两次写
        EXTFlash_SPI_WriteSector(addr, &wrData[0], 3);
        EXTFlash_SPI_WriteSector(addr + 3, &wrData[3], 1);
    }
    //只写一个扇区
    else
    {
        //直接写完
        EXTFlash_SPI_WriteSector(addr, wrData, 4);
    }
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 EXTFlashReadLong(u32 addr, u32 *data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *data:读出的数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 读Flash指定地址的数据
** @The notes : 读出的数据是32位
**===========================================================================================*/
u8 EXTFlashReadLong(u32 addr, u32 *data)
{
    u32 temp0 = 0;
    u32 temp1 = 0;
    u32 temp2 = 0;
    u32 temp3 = 0;
    
    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }
    
    //地址超过最大地址容量(64Mk)
    if((addr + 3) >= EXT_FLA_ADDR_SIZE)
    {
        return(FALSE);
    }
    
    /* Select the FLASH: Chip Select low */
    EXTFlash_SPI_CSDN();
    
    /* 发送读命令 */
    //if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_READDATA))
    /* 发送高速读命令 */
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_FST_RDATA))
    {
        //发送失败
        return(FALSE);
    }
    
    /* 发送24位寄存器地址(高位先发) */
    (void)BSPSPISendData(eSPI3, (addr >> 16) & 0xFF);
    (void)BSPSPISendData(eSPI3, (addr >> 8) & 0xFF);
    (void)BSPSPISendData(eSPI3, addr & 0xFF);
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据(高速读需要)
    
    /* Read a byte from the FLASH */
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据
    temp0 = BSPSPIGetRcvData(eSPI3);                    //接收数据
    
    /* Read a byte from the FLASH */
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据
    temp1 = BSPSPIGetRcvData(eSPI3);                    //接收数据
    
    /* Read a byte from the FLASH */
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据
    temp2 = BSPSPIGetRcvData(eSPI3);                    //接收数据
    
    /* Read a byte from the FLASH */
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据
    temp3 = BSPSPIGetRcvData(eSPI3);                    //接收数据
    
    //外部芯片按8位保存，读写16位/32位数据统一按照低位在前[小端模式]
    *data = (temp3 << 24) | (temp2 << 16) | (temp1 << 8) | temp0;//组合数据
    
    /* Deselect the FLASH: Chip Select high */
    EXTFlash_SPI_CSUP();
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHWriteByte(u32 addr, u8 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:写入Flash数据块 size:数据长度[1,256]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将8位字节数据写入到外部Flash中
** @The notes :
**===========================================================================================*/
u8 DEVEXTFLASHWriteByte(u32 addr, u8 *pData, u32 size)
{
    u8 pageNum = 0;         //需写页数
    u8 pageAddr = 0;        //页内地址
    u8 pageSize = 0;        //页内地址长度
    u8 noWrSize = 0;        //页内未写地址长度
    
    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }
    
    //地址超过最大地址容量或写超过允许长度
    if(((addr + size) > EXT_FLA_ADDR_SIZE)
        || (size > EXT_FLA_WR_MAX_SIZE)
        || (size == 0))
    {
        return(FALSE);
    }
    
    //计算写跨页信息
    pageAddr = addr % EXT_FLA_PAGE_SIZE;
    noWrSize = EXT_FLA_PAGE_SIZE - pageAddr;
    pageNum =  size / EXT_FLA_PAGE_SIZE;
    pageSize = size % EXT_FLA_PAGE_SIZE;
    
    //从页首地址开始写
    if(0 == pageAddr)
    {
        //地址长度不超过一页
        if(0 == pageNum)
        {
            //直接写完
            EXTFlash_SPI_WriteSector(addr, pData, size);
        }
        //跨越多页
        else
        {
            while(pageNum--)
            {
                //从页首地址开始写满每一页
                EXTFlash_SPI_WriteSector(addr, pData, EXT_FLA_PAGE_SIZE);
                
                //计算下一页起始地址
                addr +=  EXT_FLA_PAGE_SIZE;
                pData += EXT_FLA_PAGE_SIZE;
            }
            
            EXTFlash_SPI_WriteSector(addr, pData, pageSize);
        }
    }
    //不是从页首地址开始写
    else
    {
        //地址长度不超过一页
        if(pageNum == 0)
        {
            //需要写的地址长度小于本页内未写地址的长度(一页长度地址本页内写完)
            if(pageSize <= noWrSize)
            {
                //将需写地址直接写完
                EXTFlash_SPI_WriteSector(addr, pData, size);
            }
            //需要写的地址长度超过本页内未写地址的长度(一页长度地址需跨两页才写完)
            else
            {
                //写完本页内未写的地址
                EXTFlash_SPI_WriteSector(addr, pData, noWrSize);
                
                //计算写下一页起始地址
                addr +=  noWrSize;
                pData += noWrSize;
                
                //计算剩余未写的地址
                noWrSize = pageSize - noWrSize;
                
                //把剩余地址写到下一页写完
                EXTFlash_SPI_WriteSector(addr, pData, noWrSize);
            }
        }
        //地址长度超过一页
        else
        {
            //将本页未写地址写完
            EXTFlash_SPI_WriteSector(addr, pData, noWrSize);
            
            //计算下一次起始地址(即为页起始地址)
            addr +=  noWrSize;
            pData += noWrSize;
            
            //计算剩余地址
            size -= noWrSize;
            
            //计算剩余地址所需写页数和最后一页内所需写地址长度
            pageNum =  size / EXT_FLA_PAGE_SIZE;
            pageSize = size % EXT_FLA_PAGE_SIZE;
            
            //所有页数写满
            while(pageNum--)
            {
                //写一页长度
                EXTFlash_SPI_WriteSector(addr, pData, EXT_FLA_PAGE_SIZE);
                
                //计算下一次起始地址(即为页起始地址)
                addr +=  EXT_FLA_PAGE_SIZE;
                pData += EXT_FLA_PAGE_SIZE;
            }
            
            //最后一页有单独地址需要写
            if(0 != pageSize)
            {
                //写页内地址长度
                EXTFlash_SPI_WriteSector(addr, pData, pageSize);
            }
        }
    }
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHWriteWord(u32 addr, u16 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:写入Flash数据块 size:数据长度[1,128]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将16位单字数据写入到外部Flash中
** @The notes :
**===========================================================================================*/
u8 DEVEXTFLASHWriteWord(u32 addr, u16 *pData, u32 size)
{
	u16 i = 0;
	u16 index = 0;
    u8 pageNum = 0;         //需写页数
    u8 pageAddr = 0;        //页内地址
    u8 pageSize = 0;        //页内地址长度
    u8 noWrSize = 0;        //页内未写地址长度

    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }

    //地址超过最大地址容量或写超过允许长度
    if(((addr + (size * 2)) > EXT_FLA_ADDR_SIZE)
        || ((size * 2) > EXT_FLA_WR_MAX_SIZE)
        || (size == 0))
    {
        return(FALSE);
    }

    /*转化为byte数据*/
    for(i = 0; i < size; i++)
    {
        /*小端模式(低位在前(低地址))*/
    	sExtWRData[i * 2] = pData[i] & 0xff;
    	sExtWRData[i * 2 + 1] = (pData[i] >> 8) & 0xff;
    }

    //计算写跨页信息
    pageAddr = addr % EXT_FLA_PAGE_SIZE;
    noWrSize = EXT_FLA_PAGE_SIZE - pageAddr;
    pageNum =  (size * 2) / EXT_FLA_PAGE_SIZE;
    pageSize = (size * 2) % EXT_FLA_PAGE_SIZE;

    //从页首地址开始写
    if(0 == pageAddr)
    {
        //地址长度不超过一页
        if(0 == pageNum)
        {
            //直接写完
            EXTFlash_SPI_WriteSector(addr, sExtWRData, size * 2);
        }
        //跨越多页
        else
        {
            while(pageNum--)
            {
                //从页首地址开始写满每一页
                EXTFlash_SPI_WriteSector(addr, &sExtWRData[index], EXT_FLA_PAGE_SIZE);

                //计算下一页起始地址
                addr +=  EXT_FLA_PAGE_SIZE;
                index += EXT_FLA_PAGE_SIZE;
            }

            EXTFlash_SPI_WriteSector(addr, &sExtWRData[index], pageSize);
        }
    }
    //不是从页首地址开始写
    else
    {
        //地址长度不超过一页
        if(pageNum == 0)
        {
            //需要写的地址长度小于本页内未写地址的长度(一页长度地址本页内写完)
            if(pageSize <= noWrSize)
            {
                //将需写地址直接写完
                EXTFlash_SPI_WriteSector(addr, sExtWRData, size * 2);
            }
            //需要写的地址长度超过本页内未写地址的长度(一页长度地址需跨两页才写完)
            else
            {
                //写完本页内未写的地址
                EXTFlash_SPI_WriteSector(addr, sExtWRData, noWrSize);

                //计算写下一页起始地址
                addr +=  noWrSize;
                index += noWrSize;

                //计算剩余未写的地址
                noWrSize = pageSize - noWrSize;

                //把剩余地址写到下一页写完
                EXTFlash_SPI_WriteSector(addr, &sExtWRData[index], noWrSize);
            }
        }
        //地址长度超过一页
        else
        {
            //将本页未写地址写完
            EXTFlash_SPI_WriteSector(addr, sExtWRData, noWrSize);

            //计算下一次起始地址(即为页起始地址)
            addr +=  noWrSize;
            index += noWrSize;

            //计算剩余地址(转为byte地址)
            size = size * 2 - noWrSize;

            //计算剩余地址所需写页数和最后一页内所需写地址长度
            pageNum =  size / EXT_FLA_PAGE_SIZE;
            pageSize = size % EXT_FLA_PAGE_SIZE;

            //所有页数写满
            while(pageNum--)
            {
                //写一页长度
                EXTFlash_SPI_WriteSector(addr, &sExtWRData[index], EXT_FLA_PAGE_SIZE);

                //计算下一次起始地址(即为页起始地址)
                addr +=  EXT_FLA_PAGE_SIZE;
                index += EXT_FLA_PAGE_SIZE;
            }

            //最后一页有单独地址需要写
            if(0 != pageSize)
            {
                //写页内地址长度
                EXTFlash_SPI_WriteSector(addr, &sExtWRData[index], pageSize);
            }
        }
    }

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHWriteLong(u32 addr, u32 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:写入Flash数据块 size:数据长度[1,64]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 将32位双字数据写入到外部Flash中
** @The notes :
**===========================================================================================*/
u8 DEVEXTFLASHWriteLong(u32 addr, u32 *pData, u32 size)
{
	u16 i = 0;
	u16 index = 0;
    u8 pageNum = 0;         //需写页数
    u8 pageAddr = 0;        //页内地址
    u8 pageSize = 0;        //页内地址长度
    u8 noWrSize = 0;        //页内未写地址长度

    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }

    //地址超过最大地址容量或写超过允许长度
    if(((addr + (size * 4)) > EXT_FLA_ADDR_SIZE)
        || ((size * 4) > EXT_FLA_WR_MAX_SIZE)
        || (size == 0))
    {
        return(FALSE);
    }

    /*转化为byte数据*/
    for(i = 0; i < size; i++)
    {
        /*小端模式(低位在前(低地址))*/
    	sExtWRData[i * 4] = pData[i] & 0xff;
    	sExtWRData[i * 4 + 1] = (pData[i] >> 8) & 0xff;
    	sExtWRData[i * 4 + 2] = (pData[i] >> 16) & 0xff;
    	sExtWRData[i * 4 + 3] = (pData[i] >> 24) & 0xff;
    }

    //计算写跨页信息
    pageAddr = addr % EXT_FLA_PAGE_SIZE;
    noWrSize = EXT_FLA_PAGE_SIZE - pageAddr;
    pageNum =  size * 4 / EXT_FLA_PAGE_SIZE;
    pageSize = (size * 4) % EXT_FLA_PAGE_SIZE;

    //从页首地址开始写
    if(0 == pageAddr)
    {
        //地址长度不超过一页
        if(0 == pageNum)
        {
            //直接写完
            EXTFlash_SPI_WriteSector(addr, sExtWRData, size * 4);
        }
        //跨越多页
        else
        {
            while(pageNum--)
            {
                //从页首地址开始写满每一页
                EXTFlash_SPI_WriteSector(addr, &sExtWRData[index], EXT_FLA_PAGE_SIZE);

                //计算下一页起始地址
                addr +=  EXT_FLA_PAGE_SIZE;
                index += EXT_FLA_PAGE_SIZE;
            }

            EXTFlash_SPI_WriteSector(addr, &sExtWRData[index], pageSize);
        }
    }
    //不是从页首地址开始写
    else
    {
        //地址长度不超过一页
        if(pageNum == 0)
        {
            //需要写的地址长度小于本页内未写地址的长度(一页长度地址本页内写完)
            if(pageSize <= noWrSize)
            {
                //将需写地址直接写完
                EXTFlash_SPI_WriteSector(addr, sExtWRData, size * 4);
            }
            //需要写的地址长度超过本页内未写地址的长度(一页长度地址需跨两页才写完)
            else
            {
                //写完本页内未写的地址
                EXTFlash_SPI_WriteSector(addr, sExtWRData, noWrSize);

                //计算写下一页起始地址
                addr +=  noWrSize;
                index += noWrSize;

                //计算剩余未写的地址
                noWrSize = pageSize - noWrSize;

                //把剩余地址写到下一页写完
                EXTFlash_SPI_WriteSector(addr, &sExtWRData[index], noWrSize);
            }
        }
        //地址长度超过一页
        else
        {
            //将本页未写地址写完
            EXTFlash_SPI_WriteSector(addr, sExtWRData, noWrSize);

            //计算下一次起始地址(即为页起始地址)
            addr +=  noWrSize;
            index += noWrSize;

            //计算剩余地址(转为byte地址)
            size = size * 4 - noWrSize;

            //计算剩余地址所需写页数和最后一页内所需写地址长度
            pageNum =  size / EXT_FLA_PAGE_SIZE;
            pageSize = size % EXT_FLA_PAGE_SIZE;

            //所有页数写满
            while(pageNum--)
            {
                //写一页长度
                EXTFlash_SPI_WriteSector(addr, &sExtWRData[index], EXT_FLA_PAGE_SIZE);

                //计算下一次起始地址(即为页起始地址)
                addr +=  EXT_FLA_PAGE_SIZE;
                index += EXT_FLA_PAGE_SIZE;
            }

            //最后一页有单独地址需要写
            if(0 != pageSize)
            {
                //写页内地址长度
                EXTFlash_SPI_WriteSector(addr, &sExtWRData[index], pageSize);
            }
        }
    }

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHReadByte(u32 addr, u8 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:读出的数据块指针 size:数据长度(最大一块32k)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 从外部Flash指定地址读取任意长度的8位字节数据
** @The notes : 读出的数据是若干个8位数
**===========================================================================================*/
u8 DEVEXTFLASHReadByte(u32 addr, u8 *pData, u32 size)
{
    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }
    
    //地址超过最大地址容量或读超过一块长度(防止耗时过长)
    if(((addr + size) > EXT_FLA_ADDR_SIZE)
        || (size > EXT_FLA_RO_MAX_SIZE)
        || (size == 0))
    {
        return(FALSE);
    }
    
    /* Select the FLASH: Chip Select low */
    EXTFlash_SPI_CSDN();
    
    /* 发送读命令 */
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_READDATA))
    /* 发送高速读命令(读回数据第1个byte无效) */
    //if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_FST_RDATA))
    {
        //发送失败
        return(FALSE);
    }
    
    /* 发送24位寄存器地址(高位先发) */
    (void)BSPSPISendData(eSPI3, (addr >> 16) & 0xFF);
    (void)BSPSPISendData(eSPI3, (addr >> 8) & 0xFF);
    (void)BSPSPISendData(eSPI3, addr & 0xFF);
    //(void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据(高速读需要)
    
    while (size--) /* while there is data to be read */
    {
        /* Read a byte from the FLASH */
        (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据
        *pData = BSPSPIGetRcvData(eSPI3);                   //接收数据
        
        /* Point to the next location where the byte read will be saved */
        pData++;
    }
   
    // Deselect the FLASH: Chip Select high 
    EXTFlash_SPI_CSUP();

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHReadWord(u32 addr, u16 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:读出的数据块指针 size:数据长度(最大一块32k)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 从外部Flash指定地址读取任意长度的16位单字数据
** @The notes : 读出的数据是若干个16位数
**===========================================================================================*/
u8 DEVEXTFLASHReadWord(u32 addr, u16 *pData, u32 size)
{
	u16 i = 0;
	u16 len = 0;
	u16 index = 0;

    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }

    //地址超过最大地址容量或读超过一块长度(防止耗时过长)
    if(((addr + (size * 2)) > EXT_FLA_ADDR_SIZE)
        || ((size * 2) > EXT_FLA_RO_MAX_SIZE)
        || (size == 0))
    {
        return(FALSE);
    }

    /* Select the FLASH: Chip Select low */
    EXTFlash_SPI_CSDN();

    /* 发送读命令 */
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_READDATA))
    /* 发送高速读命令(读回数据第1个byte无效) */
    //if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_FST_RDATA))
    {
        //发送失败
        return(FALSE);
    }

    /* 发送24位寄存器地址(高位先发) */
    (void)BSPSPISendData(eSPI3, (addr >> 16) & 0xFF);
    (void)BSPSPISendData(eSPI3, (addr >> 8) & 0xFF);
    (void)BSPSPISendData(eSPI3, addr & 0xFF);
    //(void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据(高速读需要)

    /*转换为byte长度*/
    len = size * 2;

    while (len--) /* while there is data to be read */
    {
        /* Read a byte from the FLASH */
        (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据
        sExtWRData[index] = BSPSPIGetRcvData(eSPI3);                  //接收数据

        /* Point to the next location where the byte read will be saved */
        index++;
    }

    // Deselect the FLASH: Chip Select high
    EXTFlash_SPI_CSUP();

    /*将byte组合成word*/
    for(i = 0; i < size; i++)
    {
        /*小端模式(低位在前(低地址))*/
    	pData[i] = ((u16)sExtWRData[i * 2 + 1] << 8) | (u16)sExtWRData[i * 2];
    }

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHReadLong(u32 addr, u32 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:读出的数据块指针 size:数据长度(最大一块32k)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 从外部Flash指定地址读取任意长度的16位单字数据
** @The notes : 读出的数据是若干个16位数
**===========================================================================================*/
u8 DEVEXTFLASHReadLong(u32 addr, u32 *pData, u32 size)
{
	u16 i = 0;
	u16 len = 0;
	u16 index = 0;

    //外部Flash未初始化就绪
    if(1 != sExtFlashReady)
    {
        //return(FALSE);
    }

    //地址超过最大地址容量或读超过一块长度(防止耗时过长)
    if(((addr + (size * 4)) > EXT_FLA_ADDR_SIZE)
        || ((size * 4) > EXT_FLA_RO_MAX_SIZE)
        || (size == 0))
    {
        return(FALSE);
    }

    /* Select the FLASH: Chip Select low */
    EXTFlash_SPI_CSDN();

    /* 发送读命令 */
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_READDATA))
    /* 发送高速读命令(读回数据第1个byte无效) */
    //if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_FST_RDATA))
    {
        //发送失败
        return(FALSE);
    }

    /* 发送24位寄存器地址(高位先发) */
    (void)BSPSPISendData(eSPI3, (addr >> 16) & 0xFF);
    (void)BSPSPISendData(eSPI3, (addr >> 8) & 0xFF);
    (void)BSPSPISendData(eSPI3, addr & 0xFF);
    //(void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据(高速读需要)

    /*转换为byte长度*/
    len = size * 4;

    while (len--) /* while there is data to be read */
    {
        /* Read a byte from the FLASH */
        (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节用于接收数据
        sExtWRData[index] = BSPSPIGetRcvData(eSPI3);                  //接收数据

        /* Point to the next location where the byte read will be saved */
        index++;
    }

    // Deselect the FLASH: Chip Select high
    EXTFlash_SPI_CSUP();

    /*将byte组合成Long*/
    for(i = 0; i < size; i++)
    {
        /*小端模式(低位在前(低地址))*/
    	pData[i] = ((u32)sExtWRData[i * 4 + 3] << 24) | ((u32)sExtWRData[i * 4 + 2] << 16) | ((u32)sExtWRData[i * 4 + 1] << 8) | (u32)sExtWRData[i * 4];
    }

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHEraseChip(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 外部Flash擦除整片Flash
** @The notes : 擦除64M
**===========================================================================================*/
u8 DEVEXTFLASHEraseChip(void)
{
    /* Enable the write access to the FLASH */
    EXTFlash_SPI_WriteEnable();
    
    /* Select the FLASH: Chip Select low */
    EXTFlash_SPI_CSDN();
 	//(void)BSPSPISendData(eSPI3, 0xC7);			/* send Chip Erase command (C7h) */
    /*擦除芯片命令*/
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_ERASECHIP))
    {
        //发送失败
        return(FALSE);
    }
    
    /* 发送24位寄存器地址(地址0开始) */
    (void)BSPSPISendData(eSPI3, 0);
    (void)BSPSPISendData(eSPI3, 0);
    (void)BSPSPISendData(eSPI3, 0);
    
    /* Deselect the FLASH: Chip Select high */
    EXTFlash_SPI_CSUP();
    
    /* Wait the end of Flash busy */
    EXTFlash_SPI_WaitBusyEnd();
    
    /* Disable the write access to the FLASH */
    EXTFlash_SPI_WriteDisable();
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHEraseBlock(u32 addr)
** @Input     : addr:Flash地址(0-0x3FFFFFF,必须是32k倍率)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 外部Flash擦除1块Flash
** @The notes : 擦除指定地址起32K
**===========================================================================================*/
u8 DEVEXTFLASHEraseBlock(u32 addr)
{
    //地址超过最大地址容量或不是1块地址倍率
    if(((addr + EXT_FLA_BLOCK_SIZE) >= EXT_FLA_ADDR_SIZE)
        || (0 != (addr % EXT_FLA_BLOCK_SIZE)))
    {
        return(FALSE);
    }
    
    /* Enable the write access to the FLASH */
    EXTFlash_SPI_WriteEnable();
    
    /* Select the FLASH: Chip Select low */
    EXTFlash_SPI_CSDN();
    
    /*擦除1块32k命令*/
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_ERASEBLO))
    {
        //发送失败
        return(FALSE);
    }
    
    /* 发送24位寄存器地址(高位先发) */
    (void)BSPSPISendData(eSPI3, (addr >> 16) & 0xFF);
    (void)BSPSPISendData(eSPI3, (addr >> 8) & 0xFF);
    (void)BSPSPISendData(eSPI3, addr & 0xFF);
    
    /* Deselect the FLASH: Chip Select high */
    EXTFlash_SPI_CSUP();
    
    /* Wait the end of Flash busy */
    EXTFlash_SPI_WaitBusyEnd();
    
    /* Disable the write access to the FLASH */
    EXTFlash_SPI_WriteDisable();
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHEraseSector(u32 addr)
** @Input     : addr:Flash地址(0-0x3FFFFFF,必须是4k倍率)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 外部Flash擦除1扇区Flash(1页)
** @The notes : 擦除指定地址起4K
**===========================================================================================*/
u8 DEVEXTFLASHEraseSector(u32 addr)
{
    //地址超过最大地址容量或不是1块地址倍率
    if(((addr + EXT_FLA_PAGE_SIZE) >= EXT_FLA_ADDR_SIZE)
        || (0 != (addr % EXT_FLA_PAGE_SIZE)))
    {
        return(FALSE);
    }
    
    /* Enable the write access to the FLASH */
    EXTFlash_SPI_WriteEnable();
    
    /* Select the FLASH: Chip Select low */
    EXTFlash_SPI_CSDN();
    
    /*擦除1页4k命令*/
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_ERASEPAG))
    {
        //发送失败
        return(FALSE);
    }
    
    /* 发送24位寄存器地址(高位先发) */
    (void)BSPSPISendData(eSPI3, (addr >> 16) & 0xFF);
    (void)BSPSPISendData(eSPI3, (addr >> 8) & 0xFF);
    (void)BSPSPISendData(eSPI3, addr & 0xFF);
    
    /* Deselect the FLASH: Chip Select high */
    EXTFlash_SPI_CSUP();
    
    /* Wait the end of Flash busy */
    EXTFlash_SPI_WaitBusyEnd();
    
    /* Disable the write access to the FLASH */
    EXTFlash_SPI_WriteDisable();
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 DEVEXTFLASHGetReadyFlag(void)
** @Input     : void
** @Output    : 0:未就绪 1:就绪
** @Function  : 获取外部Flash就绪状态标志
** @The notes : 与外部Flash芯片通讯状态
**===========================================================================================*/
u8 DEVEXTFLASHGetReadyFlag(void)
{
    return(sExtFlashReady);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void EXTFlash_SPI_CSUP(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash SPI通讯片选高
** @The notes :
**===========================================================================================*/
static void EXTFlash_SPI_CSUP(void)
{
	DEVGPIOSetPortState(ePOut_FlaCS, ePSTATE_ON);
}

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_CSDN(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash SPI通讯片选低
** @The notes :
**===========================================================================================*/
static void EXTFlash_SPI_CSDN(void)
{
	DEVGPIOSetPortState(ePOut_FlaCS, ePSTATE_OFF);
}

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteEnable(void)
** @Input     : void
** @Output    : void
** @Function  : 发送外部Flash命令写使能
** @The notes : 带片选
**===========================================================================================*/
static void EXTFlash_SPI_WriteEnable(void)
{
    EXTFlash_SPI_CSDN();                                //片选下拉
    
    (void)BSPSPISendData(eSPI3, SST26_CMD_WRITE_EN);    //SPI发送指定命令
    
    EXTFlash_SPI_CSUP();                                //恢复上拉
}

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteDisable(void)
** @Input     : void
** @Output    : void
** @Function  : 发送外部Flash命令写禁止
** @The notes : 带片选
**===========================================================================================*/
static void EXTFlash_SPI_WriteDisable(void)
{
    EXTFlash_SPI_CSDN();                                //片选下拉
    
    (void)BSPSPISendData(eSPI3, SST26_CMD_WRITE_DIS);   //SPI发送指定命令
    
    EXTFlash_SPI_CSUP();                                //恢复上拉
}

/*=============================================================================================
** @Name      : static u8 EXTFlash_SPI_SendSST26Cmd(u8 cmd)
** @Input     : void
** @Output    : 发送结果 TRUE:成功 FALSE:失败
** @Function  : SPI发送外部Flash SST26读写命令
** @The notes : 附带检测通讯是否正常,带片选
**===========================================================================================*/
static u8 EXTFlash_SPI_SendSST26Cmd(u8 cmd)
{
    u8 time = 0;
    
    /* Send SST26 instruction to SST26 */
    while(FALSE == BSPSPISendData(eSPI3, cmd))
    {
        //连续发送5次失败
        if(time++ >= 5)
        {
            /* Deselect the FLASH: Chip Select high */
            EXTFlash_SPI_CSUP();
            
            //记录读失败返回发送失败
            BitSet(sExtFlashErr, 0);
            return(FALSE);
        }
    }
    
    //读通讯正常清除故障
    BitClr(sExtFlashErr, 0);
    
    return(TRUE);
}
#if 0
/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteStatReg(u8 regData)
** @Input     : regData:写状态寄存器的值
** @Output    : void
** @Function  : 发送外部Flash命令写状态寄存器
** @The notes : 带片选
**===========================================================================================*/
static void EXTFlash_SPI_WriteStatReg(u8 regData)
{
    //Select the FLASH: Chip Select low
    EXTFlash_SPI_CSDN();
    
    //写状态寄存器命令
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_WRITESTATUS))
    {
        //发送失败
        return;
    }
    
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送写状态寄存器地址
    
    (void)BSPSPISendData(eSPI3, regData);               //SPI发送写状态寄存器的值
    
    EXTFlash_SPI_CSUP();                                //恢复上拉
    
    EXTFlash_SPI_WaitBusyEnd();                         //等待写完成
}
#endif
/*=============================================================================================
** @Name      : static u8 EXTFlash_SPI_ReadStatReg(void)
** @Input     : void
** @Output    : 状态寄存器状态
** @Function  : 发送外部Flash命令读状态寄存器
** @The notes : 带片选
**===========================================================================================*/
static u8 EXTFlash_SPI_ReadStatReg(void)
{
    u8 state = 0;
    
    EXTFlash_SPI_CSDN();                                //片选下拉
    
    (void)BSPSPISendData(eSPI3, SST26_CMD_READSTATUS);  //SPI发送指定命令
    
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节获取返回值
    state = BSPSPIGetRcvData(eSPI3);                    //获取状态
    
    EXTFlash_SPI_CSUP();                                //恢复上拉
    
    return(state);
}

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_ReadDevId(void)
** @Input     : regData:写状态寄存器的值
** @Output    : void
** @Function  : 读外部Flash设备ID
** @The notes : 带片选
**===========================================================================================*/
static u32 EXTFlash_SPI_ReadDevId(void)
{
    u32 id = 0;
    
    EXTFlash_SPI_CSDN();                                //片选下拉
    
    //(void)BSPSPISendData(eSPI3, SST26_CMD_READDEVID); //SPI发送指定命令(设备ID)
    (void)BSPSPISendData(eSPI3, SST26_CMD_READJEDID);   //SPI发送指定命令(JEDEC-ID)
    
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节获取返回值
    id = BSPSPIGetRcvData(eSPI3);                       //获取8位ID(共24位)
    
    id = id << 8;                                       //高位先发
    
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节获取返回值
    id |= BSPSPIGetRcvData(eSPI3);                      //获取8位ID(共24位)
    
    id = id << 8;                                       //高位先发
    
    (void)BSPSPISendData(eSPI3, SST26_READ_DUMMY_BYTE); //SPI发送空字节获取返回值
    id |= BSPSPIGetRcvData(eSPI3);                      //获取8位ID(共24位)
    
    EXTFlash_SPI_CSUP();                                //恢复上拉
    
    return(id);
}

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteByte(u32 addr, u8 data)
** @Input     : addr:Flash地址(0-0x3FFFFFF) data:所写数据
** @Output    : void
** @Function  : 写外部Flash一个字节数据
** @The notes :
**===========================================================================================*/
static void EXTFlash_SPI_WriteByte(u32 addr, u8 data)
{
    //地址超过最大地址容量
    if(addr >= EXT_FLA_ADDR_SIZE)
    {
        return;
    }
    
    /* Enable the write access to the FLASH */
    EXTFlash_SPI_WriteEnable();
    
    /* Select the FLASH: Chip Select low */
    EXTFlash_SPI_CSDN();
    
    /*写数据寄存器操作*/
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_WRITEDATA))
    {
        //发送失败
        return;
    }
    
    /* 发送24位寄存器地址(高位先发) */
    (void)BSPSPISendData(eSPI3, (addr >> 16) & 0xFF);
    (void)BSPSPISendData(eSPI3, (addr >> 8) & 0xFF);
    (void)BSPSPISendData(eSPI3, addr & 0xFF);
    
    /* 写数据到FLASH */
    (void)BSPSPISendData(eSPI3, data);
    
    /* Deselect the FLASH: Chip Select high */
    EXTFlash_SPI_CSUP();
    
    /* Wait the end of Flash busy */
    EXTFlash_SPI_WaitBusyEnd();
    
    /* Disable the write access to the FLASH */
    EXTFlash_SPI_WriteDisable();
}

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteSector(u32 addr, u8 *pData, u32 size)
** @Input     : addr:Flash地址(0-0x3FFFFFF) *pData:所写数据块指针 size:页内字节长度(最大一页4kbyte)
** @Output    : void
** @Function  : 写外部Flash一个扇区(一页)内数据
** @The notes : 数据长度不能跨过两个扇区(页)
**===========================================================================================*/
static void EXTFlash_SPI_WriteSector(u32 addr, u8 *pData, u32 size)
{
    //地址超过最大地址容量或读地址超过一页(扇区)长度
    if(((addr + size) > EXT_FLA_ADDR_SIZE)
        || (size > EXT_FLA_PAGE_SIZE)
        || (size == 0))
    {
        return;
    }
    
    //写地址跨两个扇区
    if((addr / EXT_FLA_PAGE_SIZE) != ((addr + size - 1) / EXT_FLA_PAGE_SIZE))
    {
        return;
    }
    
    /* Enable the write access to the FLASH */
    EXTFlash_SPI_WriteEnable();
    
    /* Select the FLASH: Chip Select low */
    EXTFlash_SPI_CSDN();
    
    /*写数据寄存器操作*/
    if(FALSE == EXTFlash_SPI_SendSST26Cmd(SST26_CMD_WRITEDATA))
    {
        //发送失败
        return;
    }
    
    /* 发送24位寄存器地址(高位先发) */
    (void)BSPSPISendData(eSPI3, (addr >> 16) & 0xFF);
    (void)BSPSPISendData(eSPI3, (addr >> 8) & 0xFF);
    (void)BSPSPISendData(eSPI3, addr & 0xFF);
    
    /* 写数据到FLASH */
    while(size--)
    {
        /* Send the current byte */
        (void)BSPSPISendData(eSPI3, *pData);
        /* Point on the next byte to be written */
        pData++;
    }
    
    /* Deselect the FLASH: Chip Select high */
    EXTFlash_SPI_CSUP();
    
    /* Wait the end of Flash busy */
    EXTFlash_SPI_WaitBusyEnd();
    
    /* Disable the write access to the FLASH */
    EXTFlash_SPI_WriteDisable();
}

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WaitForWriteEnd(void)
** @Input     : void
** @Output    : void
** @Function  : 等待外部Flash操作结束
** @The notes : 带片选
**===========================================================================================*/
static void EXTFlash_SPI_WaitBusyEnd(void)
{
    u16 time = 0;
    
   //EXTFlash_SPI_CSDN();                              //片选下拉(读取寄存器状态有下拉)
    
    while((EXTFlash_SPI_ReadStatReg() & SST26_STAREG_BUSY_BIT) != 0x00)//读状态寄存器为1处于忙状态
    {
        time++;
        if(time >= 1000)
        {
            BitSet(sExtFlashErr, 1);                    //记录外部Flash异常
            break;
        }
    }
    
    //EXTFlash_SPI_CSUP();                              //恢复上拉
}

#if 0
/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteProtect(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash写保护
** @The notes : 写状态寄存器,休眠,带片选
**===========================================================================================*/
static void EXTFlash_SPI_WriteProtect(void)
{
    //EXTFlash_SPI_CSDN();                              //片选下拉(读取寄存器状态有下拉)
    
    EXTFlash_SPI_WriteStatReg(SST26_CMD_WRITE_PRO);     //写状态寄存器BPL位为1写保护
    
    //EXTFlash_SPI_CSUP();                              //恢复上拉
    
    DEVGPIOSetOutputState(ePOut_FlaWPr, ePSTATE_OFF);   //写保护
}
#endif

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_WriteProClr(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash写保护取消
** @The notes : 写状态寄存器,激活,带片选
**===========================================================================================*/
static void EXTFlash_SPI_WriteProClr(void)
{
	DEVGPIOSetPortState(ePOut_FlaWPr, ePSTATE_ON);    //取消写保护
	
	EXTFlashWriteBlockProtection();					  //块保护关闭，默认打开(写0无保护)
	
	EXTFlash_SPI_BlockProtectUnlock();				  //清除所有的块保护
}

/*=============================================================================================
** @Name      : static u8 EXTFlashCheckIcReadyState(void)
** @Input     : void
** @Output    : 就绪状态 0:未就绪 1:就绪 2:故障
** @Function  : 检测外部Flash就绪状态
** @The notes : 通过读写校验标志判断
**===========================================================================================*/
static u8 EXTFlashCheckIcReadyState(void)
{
	u32 data = 0;

    /*读取Flash设备ID校验是否正常*/
    if(SST26_DEV_ID == EXTFlash_SPI_ReadDevId())
    {
        /*读取Flash用户参数校验是否正常*/
    	if(TRUE == EXTFlashReadLong(EXT_FLA_CRC_ADDR, &data))
    	{
    	    /*检测用户参数校验是否正常*/
    		if(EXT_FLA_CRC_FLAG == data)
    		{
    	        return(1);
    		}
    		else
    		{
    	        return(0);
    		}
    	}
        else
        {
            return(2);
        }
    }
    //设备ID错误则未就绪
    else
    {
        return(2);
    }
}

/*=============================================================================================
** @Name      : static u8 EXTFlashWriteBlockProtection(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash写保护操作函数
** @The notes :
**===========================================================================================*/
static u8 EXTFlashWriteBlockProtection(void)
{
   	u8 i = 0;
	u8 block_protection_18[18] = {0};

	for(i = 0; i < 18; i++)
	{
		block_protection_18[i] = 0x00;
	}

    /* Enable the write access to the FLASH */
    EXTFlash_SPI_WriteEnable();

	EXTFlash_SPI_CSDN();			  /* enable device */
	(void)BSPSPISendData(eSPI3, 0x42);/* read command */

	for(i = 18; i > 0; i--)
	{
		(void)BSPSPISendData(eSPI3, block_protection_18[i]);
	}

	EXTFlash_SPI_CSUP();			  /* disable device */
    /* Disable the write access to the FLASH */
    EXTFlash_SPI_WriteDisable();

	return TRUE;
}

/*=============================================================================================
** @Name      : static void EXTFlash_SPI_BlockProtectUnlock(void)
** @Input     : void
** @Output    : void
** @Function  : 外部Flash取消所有块保护
** @The notes :
**===========================================================================================*/
static void EXTFlash_SPI_BlockProtectUnlock(void)
{
    EXTFlash_SPI_CSDN();                              //片选下拉(读取寄存器状态有下拉)
    
    (void)BSPSPISendData(eSPI3, SST26_CMD_BLOCK_UNLOCK);
    
    EXTFlash_SPI_CSUP();                              //恢复上拉    
}
