/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPEEPROM.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : EEPROM驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPISRHook.h"
#include "flash_config.h"
#include "flash_driver.h"
#include "BSPEEPROM.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;

/* Declare a FLASH configuration structure which is initialized by FlashInit, and will be used by all
flash APIs */
extern flash_ssd_config_t flashSSDConfig;

/*U16/U32转化为U8数据再写*/
static u8 sEepWrData[1024] = {0};

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPINTEEPInit(u8 oscClock)
** @Input     : oscClock:晶振时钟
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : 内部EEPROM驱动模块初始化
** @The notes :
**===========================================================================================*/
u8 BSPINTEEPInit(u8 oscClock)
{
    if(1 != gBSPFunCfg.Bits.EEPEn)
    {
        return(FALSE);
    }

    /* Disable interrupts */
    INT_SYS_DisableIRQGlobal();

    FLASH_DRV_Init(&Flash_InitConfig0, &flashSSDConfig);

    /* Configure FlexRAM as EEPROM and FlexNVM as EEPROM backup region,
     * DEFlashPartition will be failed if the IFR region isn't blank.
     * Refer to the device document for valid EEPROM Data Size Code
     * and FlexNVM Partition Code. For example on S32K148:
     * - EEEDataSizeCode = 0x02u: EEPROM size = 4 Kbytes
     * - DEPartitionCode = 0x04u: EEPROM backup size = 64 Kbytes */
    FLASH_DRV_DEFlashPartition(&flashSSDConfig, 0x02u, 0x04u, 0x0u, false, true);

    FLASH_DRV_Init(&Flash_InitConfig0, &flashSSDConfig);

    /* Make FlexRAM available for EEPROM */
    FLASH_DRV_SetFlexRamFunction(&flashSSDConfig, EEE_ENABLE, 0x00u, NULL);

    /* Make FlexRAM available for EEPROM, make sure that FlexNVM and FlexRAM
     * are already partitioned successfully before */
    FLASH_DRV_SetFlexRamFunction(&flashSSDConfig, EEE_ENABLE, 0x00u, NULL);

    /* Enable interrupts */
    INT_SYS_EnableIRQGlobal();

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPINTEEPWriteByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入EEPROM地址([0,4095](4K) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按字节方式写入数据(8位)
** @The notes :
**===========================================================================================*/
u8 BSPINTEEPWriteByte(u32 addr, u8 *pData, u32 len)
{
    u8 result = 0;
    u32 startAddr = 0;

    if(1 != gBSPFunCfg.Bits.EEPEn)
    {
        return(FALSE);
    }

    /*写入地址或长度不合法*/
    if((len == 0) || ((addr + len) > EEPROM_ADDR_SIZE))
    {
        return(FALSE);
    }

    /*输入信息异常*/
    if(pData == (void *)0)
    {
        return(FALSE);
    }

    /*转换为实际FLASH地址*/
    startAddr = addr + EEPROM_ADDR_START;

    //BSP_DISABLE_IRQ();

    result = FLASH_DRV_EEEWrite(&flashSSDConfig, startAddr, len, pData);

    //BSP_ENABLE_IRQ();

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
** @Name      : u8 BSPINTEEPWriteWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:写入EEPROM地址([0,4095](4K) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按字方式写入数据(16位)
** @The notes : len必须是4的整数倍
**===========================================================================================*/
u8 BSPINTEEPWriteWord(u32 addr, u16 *pData, u32 len)
{
    u32 i = 0;
    u8 result = 0;
    u32 startAddr = 0;

    if(1 != gBSPFunCfg.Bits.EEPEn)
    {
        return(FALSE);
    }

    if((len == 0) || ((addr + (len * 2)) > EEPROM_ADDR_SIZE))
    {
        return(FALSE);
    }

    /*输入信息异常*/
    if(pData == (void *)0)
    {
        return(FALSE);
    }

    /*转换为实际FLASH地址*/
    startAddr = addr + EEPROM_ADDR_START;

    //BSP_DISABLE_IRQ();

    /*U16转化为U8,小端模式(低位在前)*/
    for(i = 0; i < len; i++)
    {
    	sEepWrData[i * 2] = (u8)(pData[i] & 0xff);
    	sEepWrData[i * 2 + 1] = (u8)((pData[i] >> 8) & 0xff);
    }

    result = FLASH_DRV_EEEWrite(&flashSSDConfig, startAddr, len * 2, sEepWrData);

    //BSP_ENABLE_IRQ();

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
** @Name      : u8 BSPINTEEPWriteLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:写入EEPROM地址([0,4095](4K) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按双字方式写入数据(32位)
** @The notes :
**===========================================================================================*/
u8 BSPINTEEPWriteLong(u32 addr, u32 *pData, u32 len)
{
    u32 i = 0;
    u8 result = 0;
    u32 startAddr = 0;

    if(1 != gBSPFunCfg.Bits.EEPEn)
    {
        return(FALSE);
    }

    /*写入地址或长度不合法*/
    if((len == 0) || ((addr + (len * 4)) > EEPROM_ADDR_SIZE))
    {
        return(FALSE);
    }

    /*输入信息异常*/
    if(pData == (void *)0)
    {
        return(FALSE);
    }

    /*转换为实际FLASH地址*/
    startAddr = addr + EEPROM_ADDR_START;

    /*U32转化为U8,小端模式(低位在前)*/
    for(i = 0; i < len; i++)
    {
    	sEepWrData[i * 4] = (u8)(pData[i] & 0xff);
    	sEepWrData[i * 4 + 1] = (u8)((pData[i] >> 8) & 0xff);
    	sEepWrData[i * 4 + 2] = (u8)((pData[i] >> 16) & 0xff);
    	sEepWrData[i * 4 + 3] = (u8)((pData[i] >> 24) & 0xff);
    }
    //BSP_DISABLE_IRQ();

    result = FLASH_DRV_EEEWrite(&flashSSDConfig, startAddr, len * 4, sEepWrData);

    //BSP_ENABLE_IRQ();

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
** @Name      : u8 BSPINTEEPReadByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:读取EEPROM地址([0,4095](4K) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按字节方式读出数据
** @The notes :
**===========================================================================================*/
u8 BSPINTEEPReadByte(u32 addr, u8 *pData, u32 len)
{
    u32 i = 0;
    u32 startAddr = 0;

    if(1 != gBSPFunCfg.Bits.EEPEn)
    {
        return(FALSE);
    }

    /*读地址或长度不合法*/
    if((len == 0) || ((addr + len) > EEPROM_ADDR_SIZE))
    {
        return(FALSE);
    }

    /*输入信息异常*/
    if(pData == (void *)0)
    {
        return(FALSE);
    }

    /*转换为实际FLASH地址*/
    startAddr = addr + EEPROM_ADDR_START;

    while ((FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK) == 0){}

    for(i = 0; i < len; i++)
    {
        *pData = (*(u32*)startAddr) & 0xff;
        startAddr++;
        pData++;
    }

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPINTEEPReadWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:读取EEPROM地址([0,4095](4K) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按字方式读出数据
** @The notes :
**===========================================================================================*/
u8 BSPINTEEPReadWord(u32 addr, u16 *pData, u32 len)
{
    u32 i = 0;
    u32 startAddr = 0;

    if(1 != gBSPFunCfg.Bits.EEPEn)
    {
        return(FALSE);
    }

    /*读地址或长度不合法*/
    if((len == 0) || ((addr + (len * 2)) > EEPROM_ADDR_SIZE))
    {
        return(FALSE);
    }

    /*输入信息异常*/
    if(pData == (void *)0)
    {
        return(FALSE);
    }

    /*转换为实际FLASH地址*/
    startAddr = addr + EEPROM_ADDR_START;

    while ((FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK) == 0){}

    for(i = 0; i < len; i++)
    {
    	pData[i] = (*(u32*)startAddr) & 0xffff;
        startAddr += 2;
    }

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPINTEEPReadLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:读取EEPROM地址([0,4095](4K) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Eeprom按双字方式读出数据
** @The notes :
**===========================================================================================*/
u8 BSPINTEEPReadLong(u32 addr, u32 *pData, u32 len)
{
    u32 i = 0;
    u32 startAddr = 0;

    if(1 != gBSPFunCfg.Bits.EEPEn)
    {
        return(FALSE);
    }

    /*读地址或长度不合法*/
    if((len == 0) || ((addr + (len * 4)) > EEPROM_ADDR_SIZE))
    {
        return(FALSE);
    }

    /*输入信息异常*/
    if(pData == (void *)0)
    {
        return(FALSE);
    }

    /*转换为实际FLASH地址*/
    startAddr = addr + EEPROM_ADDR_START;

    while ((FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK) == 0){}

    for(i = 0; i < len; i++)
    {
        pData[i] = *(u32*)startAddr;
        startAddr += 4;
    }

    return(TRUE);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
