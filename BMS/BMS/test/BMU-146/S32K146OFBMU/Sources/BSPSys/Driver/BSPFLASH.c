/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPFLASH.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : Flash驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPIRQ.h"
#include "status.h"
#include "flash_config.h"
#include "flash_driver.h"
#include "BSPFLASH.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;

/* Declare a FLASH configuration structure which is initialized by FlashInit, and will be used by all
flash APIs */
flash_ssd_config_t flashSSDConfig;

/*U16/U32转化为U8数据再写*/
static u8 sFlashWrData[1024] = {0};

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPINTFLASHInit(u8 oscClock)
** @Input     : oscClock:晶振时钟
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : 内部FLASH驱动模块初始化
** @The notes :
**===========================================================================================*/
u8 BSPINTFLASHInit(u8 oscClock)
{
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }

    FLASH_DRV_Init(&Flash_InitConfig0, &flashSSDConfig);

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseChip(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除整个MCU全部FLASH
** @The notes : 用户慎用
**===========================================================================================*/
u8 BSPINTFLASHEraseChip(void)
{
    status_t FLASHStatus = STATUS_SUCCESS;/* Return code variable */
    
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }
    
    //BSP_DISABLE_IRQ();

    //解锁BANK1,只有解锁后才能擦除和写入
    FLASH_DRV_EraseAllBlockUnsecure(&flashSSDConfig);
    
    //擦除所有flash
    FLASHStatus = FLASH_DRV_EraseAllBlock(&flashSSDConfig);

    /* Verify the erase operation at margin level value of 1 */
    FLASH_DRV_VerifyAllBlock(&flashSSDConfig, 1u);
    
    //操作完成后加锁
    //FLASH_LockBank1();
    
    //BSP_ENABLE_IRQ();

    /*擦除成功*/
    if(STATUS_SUCCESS == FLASHStatus)
    {
        return(TRUE);
    }
    /*擦除失败*/
    else
    {
        return(FALSE);
    }
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseBlock(u16 block)
** @Input     : u16 sector:FLASH块号[0,5]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除FLASH块
** @The notes : 按块擦除(按用途划分1块大小各不同[4K/392K/392K/200K/4K/32K])
**===========================================================================================*/
u8 BSPINTFLASHEraseBlock(u16 block)
{
    u8 i = 0;
    u8 result = 0;
    u8 secNum = 0;
    u32 secAddr = 0;
    status_t FLASHStatus = STATUS_SUCCESS;/* Return code variable */
    
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }
    
    //块号无效
    if(block >= FLASH_BLOCK_NUM)
    {
        return(FALSE);
    }
    
    //不能操作BOOT块(应用程序有效)
    if(block < 1)
    {
       //return(FALSE);
    }
    
    BSP_DISABLE_IRQ();

    //解锁BANK1，只有解锁后才能擦除和写入
    //FLASH_DRV_EraseAllBlockUnsecure(&flashSSDConfig);
    result = TRUE;

    //清除标志,解锁函数中已经操作
    //CLEAR_FTFx_FSTAT_ERROR_BITS;

    switch(block)
    {
        //第一块Flash(4k,BOOT程序块1[向量表])
        case 0:
			secAddr = FLASH_BLOCK0_ADDR_START;
			//擦除一个扇区(一页,4k)FLASH
			FLASHStatus = FLASH_DRV_EraseSector(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE);

			/* Verify the erase operation at margin level value of 1, user read */
			FLASH_DRV_VerifySection(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE, 1u);
			//BSPWatchDogOutput();

			//擦除失败
			if(STATUS_SUCCESS != FLASHStatus)
			{
				result = FALSE;
			}
            break;
            
        //第二块Flash(392k,APP运行程序块)
        case 1:
            secAddr = FLASH_BLOCK1_ADDR_START;
            secNum = (u8)(FLASH_BLOCK1_SIZE / FLASH_PAGE_SIZE);

            for(i = 0; i < secNum; i++)
            {
                //擦除一个扇区(一页,4k)FLASH
                FLASHStatus = FLASH_DRV_EraseSector(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE);
                /* Verify the erase operation at margin level value of 1, user read */
                FLASH_DRV_VerifySection(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE, 1u);
                //计算下一个扇区地址
                secAddr += FLASH_PAGE_SIZE;
                //BSPWatchDogOutput();

                //擦除失败
                if(STATUS_SUCCESS != FLASHStatus)
                {
                    result = FALSE;
                    break;
                }
            }
            break;
            
		//第三块Flash(392k,APP备份程序块)
		case 2:
			secAddr = FLASH_BLOCK2_ADDR_START;
			secNum = (u8)(FLASH_BLOCK2_SIZE / FLASH_PAGE_SIZE);

			for(i = 0; i < secNum; i++)
			{
				//擦除一个扇区(一页,4k)FLASH
				FLASHStatus = FLASH_DRV_EraseSector(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE);
				/* Verify the erase operation at margin level value of 1, user read */
				FLASH_DRV_VerifySection(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE, 1u);
				//计算下一个扇区地址
				secAddr += FLASH_PAGE_SIZE;
				//BSPWatchDogOutput();

				//擦除失败
				if(STATUS_SUCCESS != FLASHStatus)
				{
					result = FALSE;
					break;
				}
			}
			break;

		//第四块Flash(200k,BOOT程序块2[代码块])
		case 3:
			secAddr = FLASH_BLOCK3_ADDR_START;
			secNum = (u8)(FLASH_BLOCK3_SIZE / FLASH_PAGE_SIZE);

			for(i = 0; i < secNum; i++)
			{
				//擦除一个扇区(一页,4k)FLASH
				FLASHStatus = FLASH_DRV_EraseSector(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE);
				/* Verify the erase operation at margin level value of 1, user read */
				FLASH_DRV_VerifySection(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE, 1u);
				//计算下一个扇区地址
				secAddr += FLASH_PAGE_SIZE;
				//BSPWatchDogOutput();

				//擦除失败
				if(STATUS_SUCCESS != FLASHStatus)
				{
					result = FALSE;
					break;
				}
			}
			break;

		//第五块Flash(4k,BOOT参数块)
		case 4:
			secAddr = FLASH_BLOCK4_ADDR_START;
			//擦除一个扇区(一页,4k)FLASH
			FLASHStatus = FLASH_DRV_EraseSector(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE);

			/* Verify the erase operation at margin level value of 1, user read */
			FLASH_DRV_VerifySection(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE, 1u);
			//BSPWatchDogOutput();

			//擦除失败
			if(STATUS_SUCCESS != FLASHStatus)
			{
				result = FALSE;
			}
			break;

        //第六块Flash(32k,APP参数块)
        case 5:
            secAddr = FLASH_BLOCK5_ADDR_START;
            secNum = (u8)(FLASH_BLOCK5_SIZE / FLASH_PAGE_SIZE);
            
            for(i = 0; i < secNum; i++)
            {
                //擦除一个扇区(一页,4k)FLASH
                FLASHStatus = FLASH_DRV_EraseSector(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE);
                /* Verify the erase operation at margin level value of 1, user read */
                FLASH_DRV_VerifySection(&flashSSDConfig, secAddr, FLASH_PAGE_SIZE, 1u);
                //计算下一个扇区地址
                secAddr += FLASH_PAGE_SIZE;
                //BSPWatchDogOutput();

                //擦除失败
                if(STATUS_SUCCESS != FLASHStatus)
                {
                    result = FALSE;
                    break;
                }
            }
            break;
            
        default:
            result = FALSE;
            break;
    }
    
    //操作完成后加锁
    //FLASH_LockBank1();

    BSP_ENABLE_IRQ();
    
    return(result);
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseSector(u32 addr)
** @Input     : addr:FLASH地址[0,1048575](1M)
** @Output    : TRUE:扇区擦除成功 FALSE:扇区擦除错误
** @Function  : 擦除FLASH扇区(一页)
** @The notes : 1个扇区为4096Byte(4K),地址必须是4096倍数
**===========================================================================================*/
u8 BSPINTFLASHEraseSector(u32 addr)
{
    u32 startAddr = 0;
    status_t FLASHStatus = STATUS_SUCCESS;/* Return code variable */

    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }
    
    //地址非扇区(一页)起始地址
    if(0 != (addr % FLASH_PAGE_SIZE))
    {
        return(FALSE);
    }
    
    //转换为实际FLASH地址
    startAddr = addr + FLASH_ADDR_START;
    
    //地址不在Flash地址范围
    /*if((startAddr < FLASH_ADDR_START)
        || (startAddr >= (FLASH_ADDR_START + FLASH_ADDR_SIZE)))*/
    if(startAddr >= FLASH_ADDR_SIZE)
    {
        return(FALSE);
    }
    
    //不能操作BOOT地址(应用程序有效)
    if(startAddr < FLASH_BLOCK1_ADDR_START)
    {
       //return(FALSE);
    }
    
    BSP_DISABLE_IRQ();
    //解锁BANK1，只有解锁后才能擦除和写入
    //FLASH_DRV_EraseAllBlockUnsecure(&flashSSDConfig);
    
    //清除标志,解锁函数中已经操作
    //CLEAR_FTFx_FSTAT_ERROR_BITS;

    //擦除一个扇区(一页)FLASH
    FLASHStatus = FLASH_DRV_EraseSector(&flashSSDConfig, startAddr, FLASH_PAGE_SIZE);

    /* Verify the erase operation at margin level value of 1, user read */
    FLASH_DRV_VerifySection(&flashSSDConfig, startAddr, FLASH_PAGE_SIZE, 1u);

    //操作完成后加锁
    //FLASH_LockBank1();
    //FLASH_Lock();

    BSP_ENABLE_IRQ();
    
    /*擦除成功*/
    if(STATUS_SUCCESS == FLASHStatus)
    {
        return(TRUE);
    }
    /*擦除失败*/
    else
    {
        return(FALSE);
    }
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWriteByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数(必须是8的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数
**===========================================================================================*/
u8 BSPINTFLASHWriteByte(u32 addr, u8 *pData, u32 len)
{
    u8 result = 0;
    u32 startAddr = 0;
    
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }
    
    /*写入地址或长度不合法*/
    if((len == 0) || ((addr + len) > FLASH_ADDR_SIZE))
    {
        return(FALSE);
    }

    /*驱动包函数要求地址和长度必须是8的倍数*/
    if((0 != (addr % 8)) || (0 != (len % 8)) || (pData == (void *)0))
    {
        return(FALSE);
    }
    
    /*转换为实际FLASH地址*/
    startAddr = addr + FLASH_ADDR_START;
    
    BSP_DISABLE_IRQ();
    
    //解锁BANK1，只有解锁后才能擦除和写入
    //FLASH_DRV_EraseAllBlockUnsecure(&flashSSDConfig);

    result = FLASH_DRV_Program(&flashSSDConfig, startAddr, len, pData);

    //操作完成后加锁
    //FLASH_LockBank1();
    
    BSP_ENABLE_IRQ();
    
    /*写入成功*/
    if(STATUS_SUCCESS == result)
    {
        return(TRUE);
    }
    /*写入失败*/
    else
    {
        return(FALSE);
    }
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWriteWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数(必须是4的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字方式写入数据(16位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数,单次最大写1Kbyte.
**===========================================================================================*/
u8 BSPINTFLASHWriteWord(u32 addr, u16 *pData, u32 len)
{
    u32 i = 0;
    u8 result = 0;
    u32 startAddr = 0;
    
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }

    /*写入地址或长度不合法*/
    if((len == 0) || ((addr + (len * 2)) > FLASH_ADDR_SIZE))
    {
        return(FALSE);
    }

    /*驱动包函数要求地址和byte长度必须是8的倍数*/
    if((0 != (addr % 8)) || (0 != (len % 4)) || (pData == (void *)0))
    {
        return(FALSE);
    }

    /*转换为实际FLASH地址*/
    startAddr = addr + FLASH_ADDR_START;
    
    BSP_DISABLE_IRQ();
    
    //解锁BANK1，只有解锁后才能擦除和写入
    //FLASH_DRV_EraseAllBlockUnsecure(&flashSSDConfig);

    /*U16转化为U8,小端模式(低位在前)*/
    for(i = 0; i < len; i++)
    {
    	sFlashWrData[2 * i] = (u8)(pData[i] & 0xff);
    	sFlashWrData[2 * i + 1] = (u8)((pData[i] >> 8) & 0xff);
    }

    result = FLASH_DRV_Program(&flashSSDConfig, startAddr, len * 2, sFlashWrData);

    //操作完成后加锁
    //FLASH_LockBank1();

    BSP_ENABLE_IRQ();
    
    /*写入成功*/
    if(STATUS_SUCCESS == result)
    {
        return(TRUE);
    }
    /*写入失败*/
    else
    {
        return(FALSE);
    }
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWriteLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数(必须是2的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按双字方式写入数据(32位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数,单次最大写1Kbyte.
**===========================================================================================*/
u8 BSPINTFLASHWriteLong(u32 addr, u32 *pData, u32 len)
{
    u32 i = 0;
    u8 result = 0;
    u32 startAddr = 0;
    
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }

    /*写入地址或长度不合法*/
    if((len == 0) || ((addr + (len * 4)) > FLASH_ADDR_SIZE))
    {
        return(FALSE);
    }

    /*驱动包函数要求地址和byte长度必须是8的倍数*/
    if((0 != (addr % 8)) || (0 != (len % 2)) || (pData == (void *)0))
    {
        return(FALSE);
    }

    /*转换为实际FLASH地址*/
    startAddr = addr + FLASH_ADDR_START;
    
    BSP_DISABLE_IRQ();
    
    //解锁BANK1，只有解锁后才能擦除和写入
    //FLASH_DRV_EraseAllBlockUnsecure(&flashSSDConfig);
    
    result = TRUE;

    /*U32转化为U8,小端模式(低位在前)*/
    for(i = 0; i < len; i++)
    {
    	sFlashWrData[4 * i] = (u8)(pData[i] & 0xff);
    	sFlashWrData[4 * i + 1] = (u8)((pData[i] >> 8) & 0xff);
    	sFlashWrData[4 * i + 2] = (u8)((pData[i] >> 16) & 0xff);
    	sFlashWrData[4 * i + 3] = (u8)((pData[i] >> 24) & 0xff);
    }

    result = FLASH_DRV_Program(&flashSSDConfig, startAddr, len * 4, sFlashWrData);

    //操作完成后加锁
    //FLASH_LockBank1();

    BSP_ENABLE_IRQ();
    
    /*写入成功*/
    if(STATUS_SUCCESS == result)
    {
        return(TRUE);
    }
    /*写入失败*/
    else
    {
        return(FALSE);
    }
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadByte(u32 addr, u8 *pData, u32 len)
{
    u32 i = 0;
    u32 startAddr = 0;
    
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }

    /*读取地址或长度不合法*/
    if((len == 0) || ((addr + len) > FLASH_ADDR_SIZE))
    {
        return(FALSE);
    }

    /*输入信息异常*/
    if(pData == (void *)0)
    {
        return(FALSE);
    }

    /*转换为实际FLASH地址*/
    startAddr = addr + FLASH_ADDR_START;

    //BSP_DISABLE_IRQ();

    while((FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK) == 0){}

    for(i = 0; i < len; i++)
    {
        *pData = (*(u32*)startAddr) & 0xff;
        startAddr++;
        pData++;
    }

    //BSP_ENABLE_IRQ();

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadWord(u32 addr, u16 *pData, u32 len)
{
    u32 i = 0;
    u32 startAddr = 0;
    
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }

    /*读取地址或长度不合法*/
    if((len == 0) || ((addr + (len * 2)) > FLASH_ADDR_SIZE))
    {
        return(FALSE);
    }

    /*输入信息异常*/
    if(pData == (void *)0)
    {
        return(FALSE);
    }

    /*转换为实际FLASH地址*/
    startAddr = addr + FLASH_ADDR_START;

    //BSP_DISABLE_IRQ();

    while((FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK) == 0){}

    for(i = 0; i < len; i++)
    {
    	pData[i] = (*(u32*)startAddr) & 0xffff;
        startAddr += 2;
    }

    //BSP_ENABLE_IRQ();

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按双字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadLong(u32 addr, u32 *pData, u32 len)
{
    u32 i = 0;
    u32 startAddr = 0;
    
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }

    /*读取地址或长度不合法*/
    if((len == 0) || ((addr + (len * 4)) > FLASH_ADDR_SIZE))
    {
        return(FALSE);
    }
    
    /*输入信息异常*/
    if(pData == (void *)0)
    {
        return(FALSE);
    }

    /*转换为实际FLASH地址*/
    startAddr = addr + FLASH_ADDR_START;

    //BSP_DISABLE_IRQ();

    while((FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK) == 0){}

    for(i = 0; i < len; i++)
    {
    	pData[i] = *(u32*)startAddr;
        startAddr += 4;
    }

    //BSP_ENABLE_IRQ();

    return(TRUE);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
