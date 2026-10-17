/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ParaIntFlash.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 参数存储模块读写内部FLASH参数
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "PublicPara.h"
#include "ParaInterface.h"
#include "ParaIntFlash.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gSysGenParaRO_100[];  							/*系统通用设置*/
extern u16 gSysHigParaRO_101[];  							/*系统硬件设置*/
extern u16 gGBmuGenPara_102[];  							/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];							/*主控高级参数值*/
extern u8 gGHardPara_104[];  								/*主控硬件参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 ReadIntFlashDevSerial(u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取电流零飘修正值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashDevSerial(u8 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEV_SERIAL_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_DEV_SERIAL_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadByte(addr, data, len);

        if(PARA_CFG_FLAG_U8 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashCorrZeroCurr(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取电流零飘修正值(有符号,单位10mA)
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashCorrZeroCurr(u16 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_ZERO_C_CORR_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_ZERO_C_CORR_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadWord(addr, data, len);

        /*读到参数有效*/
        if((PARA_CFG_FLAG_U16 == data[0])
            && ((0 - (s16)MAX_CURR_CORR_10MA) <= (s16)data[1])
			&& ((s16)data[1] <= (s16)MAX_CURR_CORR_10MA))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashHalChgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取霍尔充电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashHalChgCurrCorrKB(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HCHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_HCHGC_CORR_KB_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashHalDhgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取霍尔放电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashHalDhgCurrCorrKB(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HDHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_HDHGC_CORR_KB_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashShuChgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取分流器充电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashShuChgCurrCorrKB(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SCHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_SCHGC_CORR_KB_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashShuDhgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取分流器放电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashShuDhgCurrCorrKB(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SDHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_SDHGC_CORR_KB_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashShuChHCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取分流器充电大电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashShuChHCurrCorrKB(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SCHHC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_SCHHC_CORR_KB_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashShuDhHCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取分流器放电大电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashShuDhHCurrCorrKB(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SDHHC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_SDHHC_CORR_KB_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashSumVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取电池总电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashSumVoltCorrKB(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SUMV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_SUMV_CORR_KB_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashPreVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取预充电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashPreVoltCorrKB(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_PREV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_PREV_CORR_KB_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashLoaVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取负载电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashLoaVoltCorrKB(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_LOAV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_LOAV_CORR_KB_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashPosVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取Link+电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashPosVoltCorrKB(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_POSV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_POSV_CORR_KB_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashNegVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取Link-电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashNegVoltCorrKB(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_NEGV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_NEGV_CORR_KB_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashSysClock(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取系统时钟(北京时间)
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashSysClock(u16 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SYS_CLOCK_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_SYS_CLOCK_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadWord(addr, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] >= 2000) && (data[1] <= 2200)
            && (data[2] >= 0x0101) && (data[2] <= 0x1f1f) && (data[3] <= 0x3c3c) && (data[4] <= 0xea60))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashMonErrTime(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取月故障统计时间(年月)
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashMonErrTime(u16 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_MONERR_TIME_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_MONERR_TIME_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadWord(addr, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] >= 0x1401))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashNowCapSoh(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取当前总容量SOH值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashNowCapSoh(u16 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_NOW_CAP_SOH_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_NOW_CAP_SOH_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadWord(addr, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] <= 1000))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashChgTimesState(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取充放电次数检测状态
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashChgTimesState(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SYS_CLOCK_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_CHGTIMES_STA_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        /*读取到有效参数*/
        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*未读取到有效参数，写入默认值*/
    data[0] = PARA_CFG_FLAG_U32;
    data[1] = 0;
    (void)BSPINTEEPWriteLong(PARA_CHGTIMES_STA_ADDR + 4, &data[1], 1);
    (void)BSPINTEEPWriteLong(PARA_CHGTIMES_STA_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashCapEnerCorrInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取总容量总能量自动标定信息值
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashCapEnerCorrInfo(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_CAPENER_CORR_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_CAPENER_CORR_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashCapFormInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取电量计算结构信息
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashCapFormInfo(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_CAP_FORM_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_CAP_FORM_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
    	(void)BSPINTFLASHReadLong(addr, data, len);

        if((PARA_CFG_FLAG_U32 == data[0]) && (data[1] <= data[3]) && (data[2] <= data[3])
            && (data[3] <= MAX_ALL_CAP_CFG) && (data[4] <= MAX_ALL_CAP_CFG))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashCapFormInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取电能计算结构信息
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashEnerFormInfo(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_ENER_FORM_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_ENER_FORM_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
    	(void)BSPINTFLASHReadLong(addr, data, len);

        if((PARA_CFG_FLAG_U32 == data[0]) && (data[1] <= data[3]) && (data[2] <= data[3])
            && (data[3] <= MAX_ALL_ENER_CFG) && (data[4] <= MAX_ALL_ENER_CFG))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashSysRunTime(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取系统运行(上电)时长 1Hour
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashSysRunTime(u16 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SYS_RUNTIME_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_SYS_RUNTIME_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadWord(addr, data, len);

        /*读取到有效参数*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashChgDhgTimes(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取充放电次数
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashChgDhgTimes(u16 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_CHG_TIMES_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_CHG_TIMES_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadWord(addr, data, len);

        /*读取到有效参数*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashHisErrNum(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取历史故障告警数目
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashHisErrNum(u16 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HIS_ERRNUM_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_HIS_ERRNUM_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadWord(addr, data, len);

        /*读取到有效参数*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashDODStaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取放电深度DOD统计信息
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashDODStaInfo(u16 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_DOD_INFO_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_DOD_INFO_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadWord(addr, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] <= 1000)
            && (data[2] <= 1000) && (data[3] <= 1000) && (data[4] <= 1000))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashSOSLockInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取安全等级SOS锁定信息
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashSOSLockInfo(u16 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SOS_INFO_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_SOS_INFO_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadWord(addr, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] <= 4) && (data[3] <= 4))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashHisChgDhgCap(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取累计充放电电量 1AH
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashHisChgDhgCap(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HIS_CHGCAP_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_HIS_CHGCAP_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        /*读取到有效参数*/
        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashHisChgDhgEner(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取累计充放电电能 0.1kWh
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashHisChgDhgEner(u32 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HIS_CHGENER_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_HIS_CHGENER_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadLong(addr, data, len);

        /*读取到有效参数*/
        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashGroupGenParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取电池组主控通用参数信息
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashGroupGenParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEVGEN_PARA_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_DEVGEN_PARA_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
    	(void)BSPINTFLASHReadWord(addr, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)BSPINTFLASHReadWord(addr + 2, &data[1], (len - 1));

            /*逐个检测参数是否有效*/
        	for(i = 1; i < len; i++)
        	{
                /*读取到无效参数值*/
                if(FALSE == ParaCheckGGenParaEffect(PARA_DEVGEN_PARA_ADDR + (2 * i), data[i]))
                {
                	/*取默认值,清除配置标志*/
                	data[i] = gGBmuGenPara_102[i];
                	data[0] = 0xffff;
                }
        	}
            break;
        }
        BSPWatchDogOutput();
    }

    /*读取到所有参数有效*/
    if(PARA_CFG_FLAG_U16 == data[0])
    {
    	return(TRUE);
    }
	/*未读取到有效参数*/
    else
    {
		return(FALSE);
    }
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashGroupHigParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取电池组主控高级参数信息
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashGroupHigParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEVHIG_PARA_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_DEVHIG_PARA_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
    	(void)BSPINTFLASHReadWord(addr, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)BSPINTFLASHReadWord(addr + 2, &data[1], (len - 1));

            /*逐个检测参数是否有效*/
        	for(i = 1; i < len; i++)
        	{
                /*读取到无效参数值*/
                if(FALSE == ParaCheckGHigParaEffect(PARA_DEVHIG_PARA_ADDR + (2 * i), data[i]))
                {
                	/*取默认值,清除配置标志*/
                	data[i] = gGBmuHigLevPara_103[i];
                	data[0] = 0xffff;
                }
        	}
            break;
        }
        BSPWatchDogOutput();
    }

    /*读取到所有参数有效*/
    if(PARA_CFG_FLAG_U16 == data[0])
    {
    	return(TRUE);
    }
	/*未读取到有效参数*/
    else
    {
		return(FALSE);
    }
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashGroupHWParaInfo(u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取电池组主控高级参数信息
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashGroupHWParaInfo(u8 *data, u8 len)
{
	u8 i = 0;
	u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEVHW_PARA_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_DEVHW_PARA_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
    	(void)BSPINTFLASHReadByte(addr, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U8 == data[0])
        {
            /*读取所有参数值*/
            (void)BSPINTFLASHReadByte(addr + 1, &data[1], (len - 1));

            /*逐个检测参数是否有效*/
        	for(i = 1; i < len; i++)
        	{
                /*读取到无效参数值*/
                if(FALSE == ParaCheckGHWParaEffect(PARA_DEVHW_PARA_ADDR + i, data[i]))
                {
                	/*取默认值,清除配置标志*/
                	data[i] = gGHardPara_104[i];
                	data[0] = 0xff;
                }
        	}
            break;
        }
        BSPWatchDogOutput();
    }

    /*读取到所有参数有效*/
    if(PARA_CFG_FLAG_U8 == data[0])
    {
    	return(TRUE);
    }
	/*未读取到有效参数*/
    else
    {
		return(FALSE);
    }
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashSystemWorkMode(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部FLASH读取系统工作模式
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashSystemWorkMode(u16 *data, u8 len)
{
    u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SYSWORK_MOD_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_SYSWORK_MOD_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
        (void)BSPINTFLASHReadWord(addr, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] <= 1))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashSystemGenParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部Flash读取系统通用参数信息
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashSystemGenParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
	u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_SYSGEN_PARA_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_SYSGEN_PARA_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
    	(void)BSPINTFLASHReadWord(addr, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)BSPINTFLASHReadWord(addr + 2, &data[1], (len - 1));

            /*逐个检测参数是否有效*/
        	for(i = 1; i < len; i++)
        	{
                /*读取到无效参数值*/
                if(FALSE == ParaCheckSGenParaEffect(addr + (2 * i), data[i]))
                {
                	/*取默认值,清除配置标志*/
                	data[i] = gSysGenParaRO_100[i];
                	data[0] = 0xff;
                }
        	}
            break;
        }
        BSPWatchDogOutput();
    }

    /*读取到所有参数有效*/
    if(PARA_CFG_FLAG_U16 == data[0])
    {
    	return(TRUE);
    }
	/*未读取到有效参数*/
    else
    {
		return(FALSE);
    }
}

/*=================================================================================================
** @Name      : u8 ReadIntFlashSystemHWParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部Flash读取系统设备硬件参数信息
** @The notes :
**===============================================================================================*/
u8 ReadIntFlashSystemHWParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
	u32 addr = 0;
    u8 trytime = 10;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_SYSHW_PARA_LEN))
    {
        return(FALSE);
    }

    /*参数所在EEPROM地址转换为对应所在FLASH地址*/
    addr = ParaChangEepToFlashAddr(PARA_SYSHW_PARA_ADDR);

    /*从内部FLASH读取参数*/
    while(trytime--)
    {
    	(void)BSPINTFLASHReadWord(addr, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)BSPINTFLASHReadWord(addr + 2, &data[1], (len - 1));

            /*逐个检测参数是否有效*/
        	for(i = 1; i < len; i++)
        	{
                /*读取到无效参数值*/
                if(FALSE == ParaCheckSHWParaEffect(addr + (2 * i), data[i]))
                {
                	/*取默认值,清除配置标志*/
                	data[i] = gSysHigParaRO_101[i];
                	data[0] = 0xff;
                }
        	}
            break;
        }
        BSPWatchDogOutput();
    }

    /*读取到所有参数有效*/
    if(PARA_CFG_FLAG_U16 == data[0])
    {
    	return(TRUE);
    }
	/*未读取到有效参数*/
    else
    {
		return(FALSE);
    }
}

/*=================================================================================================
** @Name      : u32 ParaChangEepToFlashAddr(u16 addr)
** @Input     : addr:参数EEPROM地址
** @Output    : 参数对应FLASH备份地址
** @Function  : 将参数EEPROM地址转化为FLASH地址
** @The notes :
**===============================================================================================*/
u32 ParaChangEepToFlashAddr(u16 addr)
{
	u32 flash = 0;

    /*EEPROM扇区0(0~497)*/
	if(addr < EEP_SEC0_CRC_ADDR)
	{
		flash = FLA_SEC0_START_ADDR + addr;
	}
    /*EEPROM扇区1(500~997)*/
	else if(addr < EEP_SEC1_CRC_ADDR)
	{
		flash = FLA_SEC1_START_ADDR + addr - 500;
	}
    /*EEPROM扇区2(1000~1497)*/
	else if(addr < EEP_SEC2_CRC_ADDR)
	{
		flash = FLA_SEC2_START_ADDR + addr - 1000;
	}
    /*EEPROM其它地址*/
	else
	{
		flash = 0xffffffff;
	}

    return(flash);
}


/*=================================================================================================
** @Static function definition
**===============================================================================================*/
