/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ParaIntEep.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 参数存储模块读写内部EEPROM参数
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
#include "ParaIntEep.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
//extern u16 gSysGenParaRO_100[];  							/*系统通用设置*/
extern u16 gSysHigParaRO_101[];  							/*系统硬件设置*/
extern u16 gGBmuGenPara_102[];  							/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];							/*主控高级参数值*/
extern u8 gGHardPara_104[];  								/*主控硬件参数值*/
extern u16 gSysGenParaRW_105[];								/*系统通用设置*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 ReadIntEepDevSerial(u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取电流零飘修正值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepDevSerial(u8 *data, u8 len)
{
	u8 i = 0;
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEV_SERIAL_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadByte(PARA_DEV_SERIAL_ADDR, data, len);

        if(PARA_CFG_FLAG_U8 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*默认值*/
    for(i = 1; i < len; i++)
    {
    	data[i] = 0;
    }
	data[0] = PARA_CFG_FLAG_U8;

	(void)BSPINTEEPWriteByte(PARA_DEV_SERIAL_ADDR + i, &data[1], (len - 1));
	(void)BSPINTEEPWriteByte(PARA_DEV_SERIAL_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepCorrZeroCurr(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取电流零飘修正值(有符号,单位10mA)
** @The notes :
**===============================================================================================*/
u8 ReadIntEepCorrZeroCurr(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_ZERO_C_CORR_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadWord(PARA_ZERO_C_CORR_ADDR, data, len);

        /*读到参数有效*/
        if((PARA_CFG_FLAG_U16 == data[0])
            && ((0 - (s16)MAX_CURR_CORR_10MA) <= (s16)data[1])
			&& ((s16)data[1] <= (s16)MAX_CURR_CORR_10MA))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*EEP未初始化,未读到有效电流零飘修正值*/
	data[0] = PARA_CFG_FLAG_U16;
	data[1] = 0;

	(void)BSPINTEEPWriteWord(PARA_ZERO_C_CORR_ADDR + 2, &data[1], (len - 1));
	(void)BSPINTEEPWriteWord(PARA_ZERO_C_CORR_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepHalChgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取霍尔充电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepHalChgCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HCHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_HCHGC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	data[1] = 0;
	data[2] = 0;

	(void)BSPINTEEPWriteLong(PARA_HCHGC_CORR_KB_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_HCHGC_CORR_KB_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepHalDhgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取霍尔放电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepHalDhgCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HDHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_HDHGC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	data[1] = 0;
	data[2] = 0;

	(void)BSPINTEEPWriteLong(PARA_HDHGC_CORR_KB_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_HDHGC_CORR_KB_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepShuChgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取分流器充电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepShuChgCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SCHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_SCHGC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	data[1] = 0;
	data[2] = 0;

	(void)BSPINTEEPWriteLong(PARA_SCHGC_CORR_KB_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_SCHGC_CORR_KB_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepShuDhgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取分流器放电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepShuDhgCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SDHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_SDHGC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	data[1] = 0;
	data[2] = 0;

	(void)BSPINTEEPWriteLong(PARA_SDHGC_CORR_KB_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_SDHGC_CORR_KB_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepShuChHCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取分流器充电大电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepShuChHCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SCHHC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_SCHHC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	data[1] = 0;
	data[2] = 0;

	(void)BSPINTEEPWriteLong(PARA_SCHHC_CORR_KB_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_SCHHC_CORR_KB_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepShuDhHCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取分流器放电大电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepShuDhHCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SDHHC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_SDHHC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	data[1] = 0;
	data[2] = 0;

	(void)BSPINTEEPWriteLong(PARA_SDHHC_CORR_KB_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_SDHHC_CORR_KB_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepSumVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取电池总电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepSumVoltCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SUMV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_SUMV_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	data[1] = 0;
	data[2] = 0;

	(void)BSPINTEEPWriteLong(PARA_SUMV_CORR_KB_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_SUMV_CORR_KB_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepPreVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取预充电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepPreVoltCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_PREV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_PREV_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	data[1] = 0;
	data[2] = 0;

	(void)BSPINTEEPWriteLong(PARA_PREV_CORR_KB_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_PREV_CORR_KB_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepLoaVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取负载电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepLoaVoltCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_LOAV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_LOAV_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	data[1] = 0;
	data[2] = 0;

	(void)BSPINTEEPWriteLong(PARA_LOAV_CORR_KB_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_LOAV_CORR_KB_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepPosVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取Link+电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepPosVoltCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_POSV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_POSV_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	data[1] = 0;
	data[2] = 0;

	(void)BSPINTEEPWriteLong(PARA_POSV_CORR_KB_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_POSV_CORR_KB_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepNegVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取Link-电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepNegVoltCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_NEGV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_NEGV_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	data[1] = 0;
	data[2] = 0;

	(void)BSPINTEEPWriteLong(PARA_NEGV_CORR_KB_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_NEGV_CORR_KB_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepSysClock(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取系统时钟(北京时间)
** @The notes :
**===============================================================================================*/
u8 ReadIntEepSysClock(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SYS_CLOCK_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadWord(PARA_SYS_CLOCK_ADDR, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] >= 2000) && (data[1] <= 2200)
            && (data[2] >= 0x0101) && (data[2] <= 0x1f1f) && (data[3] <= 0x3c3c) && (data[4] <= 0xea60))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*未读取到有效参数，写入默认时间2020/01/01 00:00:00*/
    data[0] = PARA_CFG_FLAG_U16;
    data[1] = 2020;
    data[2] = 0x0101;
    data[3] = 0;
    data[4] = 0;
    (void)BSPINTEEPWriteWord(PARA_SYS_CLOCK_ADDR + 2, &data[1], (len - 1));
    (void)BSPINTEEPWriteWord(PARA_SYS_CLOCK_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepMonErrTime(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取月故障数目统计时间(年月)
** @The notes :
**===============================================================================================*/
u8 ReadIntEepMonErrTime(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_MONERR_TIME_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadWord(PARA_MONERR_TIME_ADDR, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] >= 2000))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*未读取到有效参数，写入默认时间20/01*/
    data[0] = PARA_CFG_FLAG_U16;
    data[1] = 0x1401;
    (void)BSPINTEEPWriteWord(PARA_MONERR_TIME_ADDR + 2, &data[1], (len - 1));
    (void)BSPINTEEPWriteWord(PARA_MONERR_TIME_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepNowCapSoh(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取当前总容量SOH值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepNowCapSoh(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_NOW_CAP_SOH_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadWord(PARA_NOW_CAP_SOH_ADDR, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] <= 1000))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*未读取到有效参数，写入默认值100%*/
    data[0] = PARA_CFG_FLAG_U16;
    data[1] = 1000;
    (void)BSPINTEEPWriteWord(PARA_NOW_CAP_SOH_ADDR + 2, &data[1], (len - 1));
    (void)BSPINTEEPWriteWord(PARA_NOW_CAP_SOH_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepChgTimesState(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取充放电次数检测状态
** @The notes :
**===============================================================================================*/
u8 ReadIntEepChgTimesState(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_CHGTIMES_STA_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_CHGTIMES_STA_ADDR, data, len);

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
    (void)BSPINTEEPWriteLong(PARA_CHGTIMES_STA_ADDR + 4, &data[1], (len - 1));
    (void)BSPINTEEPWriteLong(PARA_CHGTIMES_STA_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepCapEnerCorrInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取总容量总能量自动标定信息值
** @The notes :
**===============================================================================================*/
u8 ReadIntEepCapEnerCorrInfo(u32 *data, u8 len)
{
    u8 i = 0;
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_CAPENER_CORR_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_CAPENER_CORR_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*写默认值,防止每次初始化占用太多读时间*/
	data[0] = PARA_CFG_FLAG_U32;
	for(i = 1; i < len; i++)
	{
		data[i] = 0;
	}

	(void)BSPINTEEPWriteLong(PARA_CAPENER_CORR_ADDR + 4, &data[1], (len - 1));
	(void)BSPINTEEPWriteLong(PARA_CAPENER_CORR_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepCapFormInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取电量计算结构信息
** @The notes :
**===============================================================================================*/
u8 ReadIntEepCapFormInfo(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_CAP_FORM_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
    	(void)BSPINTEEPReadLong(PARA_CAP_FORM_ADDR, data, len);

        if((PARA_CFG_FLAG_U32 == data[0]) && (data[1] <= data[3]) && (data[2] <= data[3])
            && (data[3] <= MAX_ALL_CAP_CFG) && (data[4] <= MAX_ALL_CAP_CFG))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*配置标志*/
	data[0] = PARA_CFG_FLAG_U32;
    /*当前电量(SOC)取配置值[0.1AH转1mAH]*/
	data[1] = (u32)gGBmuGenPara_102[eBmuGenPara102_ReaAllCap] * gGBmuGenPara_102[eBmuGenPara102_NowSoc] / 10;
    /*剩余电量1mAH*/
	data[2] = data[1];
    /*总容量[0.1AH转1mAH]*/
	data[3] = (u32)gGBmuGenPara_102[eBmuGenPara102_ReaAllCap] * 100;
    /*标称容量[0.1AH转1mAH]*/
	//data[4] = (u32)gGBmuGenPara_102[eBmuGenPara102_RateCap] * 100;
	//额定容量无小数才为标称容量
	if(0 == (gGBmuGenPara_102[eBmuGenPara102_RateCap] % 10))
	{
		data[4] = gGBmuGenPara_102[eBmuGenPara102_RateCap] * 100;
	}
	//额定容量有小数取实际总容量
	else
	{
		data[4] = gGBmuGenPara_102[eBmuGenPara102_ReaAllCap] * 100;
	}

    /*未读取到有效参数，写入默认值*/
	(void)BSPINTEEPWriteLong(PARA_CAP_FORM_ADDR + 4, &data[1], (len - 1));

	(void)BSPINTEEPWriteLong(PARA_CAP_FORM_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepCapFormInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取电能计算结构信息
** @The notes :
**===============================================================================================*/
u8 ReadIntEepEnerFormInfo(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_ENER_FORM_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
    	(void)BSPINTEEPReadLong(PARA_ENER_FORM_ADDR, data, len);

        if((PARA_CFG_FLAG_U32 == data[0]) && (data[1] <= data[3]) && (data[2] <= data[3])
            && (data[3] <= MAX_ALL_ENER_CFG) && (data[4] <= MAX_ALL_ENER_CFG))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*配置标志*/
	data[0] = PARA_CFG_FLAG_U32;
    /*当前电能(SOE)取配置值[0.1kWh转1Wh]*/
	data[1] = (u32)gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * gGBmuGenPara_102[eBmuGenPara102_NowSoe] / 10;
    /*剩余电能1Wh*/
	data[2] = data[1];
    /*总电能[0.1kWh转1Wh]*/
	//data[3] = (u32)gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 100;
    /*标称电能[0.1kWh转1Wh]*/
	//data[4] = (u32)gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 100;
	//总电能无小数则为标称电能(总电能为97%)
	if(0 == (gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] % 10))
	{
		data[3] = gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 9700 / 100;
		data[4] = (u32)gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 100;
	}
	//有小数,未知标称电能认为总电能为97%
	else
	{
		data[3] = gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 100;
		data[4] = (u32)gGBmuGenPara_102[eBmuGenPara102_ReaAllEner] * 10000 / 97;
	}

    /*未读取到有效参数，写入默认值*/
	(void)BSPINTEEPWriteLong(PARA_ENER_FORM_ADDR + 4, &data[1], (len - 1));

	(void)BSPINTEEPWriteLong(PARA_ENER_FORM_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepSysRunTime(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取系统运行(上电)时长 1Hour
** @The notes :
**===============================================================================================*/
u8 ReadIntEepSysRunTime(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SYS_RUNTIME_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadWord(PARA_SYS_RUNTIME_ADDR, data, len);

        /*读取到有效参数*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*未读取到有效参数，写入默认值*/
    data[0] = PARA_CFG_FLAG_U16;
    data[1] = 0;
    (void)BSPINTEEPWriteWord(PARA_SYS_RUNTIME_ADDR + 2, &data[1], (len - 1));
    (void)BSPINTEEPWriteWord(PARA_SYS_RUNTIME_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepChgDhgTimes(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取充放电次数
** @The notes :
**===============================================================================================*/
u8 ReadIntEepChgDhgTimes(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_CHG_TIMES_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadWord(PARA_CHG_TIMES_ADDR, data, len);

        /*读取到有效参数*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*未读取到有效参数，写入默认值*/
    data[0] = PARA_CFG_FLAG_U16;
    data[1] = 1;
    (void)BSPINTEEPWriteWord(PARA_CHG_TIMES_ADDR + 2, &data[1], (len - 1));
    (void)BSPINTEEPWriteWord(PARA_CHG_TIMES_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepHisErrNum(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取历史故障告警数目
** @The notes :
**===============================================================================================*/
u8 ReadIntEepHisErrNum(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HIS_ERRNUM_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadWord(PARA_HIS_ERRNUM_ADDR, data, len);

        /*读取到有效参数*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*未读取到有效参数，写入默认值*/
    data[0] = PARA_CFG_FLAG_U16;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    (void)BSPINTEEPWriteWord(PARA_HIS_ERRNUM_ADDR + 2, &data[1], (len - 1));
    (void)BSPINTEEPWriteWord(PARA_HIS_ERRNUM_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepDODStaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取放电深度DOD统计信息
** @The notes :
**===============================================================================================*/
u8 ReadIntEepDODStaInfo(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_DOD_INFO_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadWord(PARA_DOD_INFO_ADDR, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] <= 1000)
            && (data[2] <= 1000) && (data[3] <= 1000) && (data[4] <= 1000))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*未读取到有效参数，写入默认值*/
    data[0] = PARA_CFG_FLAG_U16;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    (void)BSPINTEEPWriteWord(PARA_DOD_INFO_ADDR + 2, &data[1], (len - 1));
    (void)BSPINTEEPWriteWord(PARA_DOD_INFO_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepSOSLockInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取安全等级SOS锁定信息
** @The notes :
**===============================================================================================*/
u8 ReadIntEepSOSLockInfo(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SOS_INFO_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadWord(PARA_SOS_INFO_ADDR, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] <= 4) && (data[3] <= 4))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*未读取到有效参数，写入默认值*/
    data[0] = PARA_CFG_FLAG_U16;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    (void)BSPINTEEPWriteWord(PARA_SOS_INFO_ADDR + 2, &data[1], (len - 1));
    (void)BSPINTEEPWriteWord(PARA_SOS_INFO_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepHisChgDhgCap(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取累计充放电电量 1AH
** @The notes :
**===============================================================================================*/
u8 ReadIntEepHisChgDhgCap(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HIS_CHGCAP_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_HIS_CHGCAP_ADDR, data, len);

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
    data[2] = 0;
    (void)BSPINTEEPWriteLong(PARA_HIS_CHGCAP_ADDR + 4, &data[1], (len - 1));
    (void)BSPINTEEPWriteLong(PARA_HIS_CHGCAP_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepHisChgDhgEner(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取累计充放电电能 0.1kWh
** @The notes :
**===============================================================================================*/
u8 ReadIntEepHisChgDhgEner(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HIS_CHGENER_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadLong(PARA_HIS_CHGENER_ADDR, data, len);

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
    data[2] = 0;
    (void)BSPINTEEPWriteLong(PARA_HIS_CHGENER_ADDR + 4, &data[1], (len - 1));
    (void)BSPINTEEPWriteLong(PARA_HIS_CHGENER_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepGroupGenParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取电池组主控通用参数信息
** @The notes :
**===============================================================================================*/
u8 ReadIntEepGroupGenParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEVGEN_PARA_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
    	(void)BSPINTEEPReadWord(PARA_DEVGEN_PARA_ADDR, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)BSPINTEEPReadWord(PARA_DEVGEN_PARA_ADDR + 2, &data[1], (len - 1));

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
		/*未读取到配置标志*/
    	if((0 == trytime) || (0xff == trytime))
		{
			/*写全部默认值*/
			(void)BSPINTEEPWriteWord(PARA_DEVGEN_PARA_ADDR + 2, &gGBmuGenPara_102[1], (len - 1));
		}
		else
		{
			/*写修改后的有效值*/
			(void)BSPINTEEPWriteWord(PARA_DEVGEN_PARA_ADDR + 2, &data[1], (len -1));
		}
		(void)BSPINTEEPWriteWord(PARA_DEVGEN_PARA_ADDR, &gGBmuGenPara_102[0], 1);

		return(FALSE);
    }
}

/*=================================================================================================
** @Name      : u8 ReadIntEepGroupHigParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取电池组主控高级参数信息
** @The notes :
**===============================================================================================*/
u8 ReadIntEepGroupHigParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEVHIG_PARA_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
    	(void)BSPINTEEPReadWord(PARA_DEVHIG_PARA_ADDR, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)BSPINTEEPReadWord(PARA_DEVHIG_PARA_ADDR + 2 , &data[1], (len - 1));

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
		/*未读取到配置标志*/
    	if((0 == trytime) || (0xff == trytime))
		{
			/*写全部默认值*/
			(void)BSPINTEEPWriteWord(PARA_DEVHIG_PARA_ADDR + 2, &gGBmuHigLevPara_103[1], (len - 1));
		}
		else
		{
			/*写修改后的有效值*/
			(void)BSPINTEEPWriteWord(PARA_DEVHIG_PARA_ADDR + 2, &data[1], (len - 1));
		}
		(void)BSPINTEEPWriteWord(PARA_DEVHIG_PARA_ADDR, &gGBmuHigLevPara_103[0], 1);

		return(FALSE);
    }
}

/*=================================================================================================
** @Name      : u8 ReadIntEepGroupHWParaInfo(u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取电池组主控高级参数信息
** @The notes :
**===============================================================================================*/
u8 ReadIntEepGroupHWParaInfo(u8 *data, u8 len)
{
	u8 i = 0;
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEVHW_PARA_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
    	(void)BSPINTEEPReadByte(PARA_DEVHW_PARA_ADDR, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U8 == data[0])
        {
            /*读取所有参数值*/
            (void)BSPINTEEPReadByte(PARA_DEVHW_PARA_ADDR + 1, &data[1], (len - 1));

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
		/*未读取到配置标志*/
    	if((0 == trytime) || (0xff == trytime))
		{
			/*写全部默认值*/
			(void)BSPINTEEPWriteByte(PARA_DEVHW_PARA_ADDR + 1, &gGHardPara_104[1], (len - 1));
		}
		else
		{
			/*写修改后的有效值*/
			(void)BSPINTEEPWriteByte(PARA_DEVHW_PARA_ADDR + 1, &data[1], (len - 1));
		}
		(void)BSPINTEEPWriteByte(PARA_DEVHW_PARA_ADDR, &gGHardPara_104[0], 1);

		return(FALSE);
    }
}

/*=================================================================================================
** @Name      : u8 ReadIntEepSystemWorkMode(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取系统工作模式
** @The notes :
**===============================================================================================*/
u8 ReadIntEepSystemWorkMode(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SYSWORK_MOD_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadWord(PARA_SYSWORK_MOD_ADDR, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] <= 1))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    /*未读取到有效参数，写入默认值*/
    data[0] = PARA_CFG_FLAG_U16;
    data[1] = 0;
    (void)BSPINTEEPWriteWord(PARA_SYSWORK_MOD_ADDR + 2, &data[1], (len - 1));
    (void)BSPINTEEPWriteWord(PARA_SYSWORK_MOD_ADDR, &data[0], 1);

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadIntEepSystemGenParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取系统通用参数信息
** @The notes :
**===============================================================================================*/
u8 ReadIntEepSystemGenParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_SYSGEN_PARA_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
    	(void)BSPINTEEPReadWord(PARA_SYSGEN_PARA_ADDR, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)BSPINTEEPReadWord(PARA_SYSGEN_PARA_ADDR + 2, &data[1], (len - 1));

            /*逐个检测参数是否有效*/
        	for(i = 1; i < len; i++)
        	{
                /*读取到无效参数值*/
                if(FALSE == ParaCheckSGenParaEffect(PARA_SYSGEN_PARA_ADDR + (2 * i), data[i]))
                {
                	/*取默认值,清除配置标志*/
                	data[i] = gSysGenParaRW_105[i];
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
		/*未读取到配置标志*/
    	if((0 == trytime) || (0xff == trytime))
		{
			/*写全部默认值*/
			(void)BSPINTEEPWriteWord(PARA_SYSGEN_PARA_ADDR + 2, &gSysGenParaRW_105[1], (len - 1));
		}
		else
		{
			/*写修改后的有效值*/
			(void)BSPINTEEPWriteWord(PARA_SYSGEN_PARA_ADDR + 2, &data[1], (len - 1));
		}
		(void)BSPINTEEPWriteWord(PARA_SYSGEN_PARA_ADDR, &gSysGenParaRW_105[0], 1);

		return(FALSE);
    }
}

/*=================================================================================================
** @Name      : u8 ReadIntEepSystemHWParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取系统设备硬件参数信息
** @The notes :
**===============================================================================================*/
u8 ReadIntEepSystemHWParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_SYSHW_PARA_LEN))
    {
        return(FALSE);
    }

    /*从内部EEPROM读取参数*/
    while(--trytime)
    {
    	(void)BSPINTEEPReadWord(PARA_SYSHW_PARA_ADDR, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)BSPINTEEPReadWord(PARA_SYSHW_PARA_ADDR + 2, &data[1], (len - 1));

            /*逐个检测参数是否有效*/
        	for(i = 1; i < len; i++)
        	{
                /*读取到无效参数值*/
                if(FALSE == ParaCheckSHWParaEffect(PARA_SYSHW_PARA_ADDR + (2 * i), data[i]))
                {
                	/*取默认值,清除配置标志*/
                	data[i] = gSysHigParaRO_101[i];
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
		/*未读取到配置标志*/
    	if((0 == trytime) || (0xff == trytime))
		{
			/*写全部默认值*/
			(void)BSPINTEEPWriteWord(PARA_SYSHW_PARA_ADDR + 2, &gSysHigParaRO_101[1], (len - 1));
		}
		else
		{
			/*写修改后的有效值*/
			(void)BSPINTEEPWriteWord(PARA_SYSHW_PARA_ADDR + 2, &data[1], (len - 1));
		}
		(void)BSPINTEEPWriteWord(PARA_SYSHW_PARA_ADDR, &gSysHigParaRO_101[0], 1);

		return(FALSE);
    }
}

/*=================================================================================================
** @Name      : u8 WriteIntEepDevSerial(u8 index, u8 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入设备序列号
** @The notes :
**===============================================================================================*/
u8 WriteIntEepDevSerial(u8 index, u8 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_DEV_SERIAL_LEN))
    {
        return(FALSE);
    }

    /*向内部EEPROM写入参数值*/
    (void)BSPINTEEPWriteByte(PARA_DEV_SERIAL_ADDR + index, data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepCorrZeroCurr(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入电流零飘修正值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepCorrZeroCurr(u8 index, u16 *data, u8 len)
{
	u8 i = 0;

    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_ZERO_C_CORR_LEN))
    {
        return(FALSE);
    }

	/*逐个检测参数有效性*/
    for(i = 0; i < len; i++)
    {
		/*写零飘误差值*/
		if(1 == (index + i))
		{
			/*需写入的参数无效*/
			if(((s16)data[i] < (0 - (s16)MAX_CURR_CORR_10MA))
				|| ((s16)data[i] > (s16)MAX_CURR_CORR_10MA))
			{
				return(FALSE);
			}
		}
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_ZERO_C_CORR_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepHalChgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入霍尔充电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepHalChgCurrCorrKB(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_HCHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_HCHGC_CORR_KB_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepHalDhgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入霍尔放电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepHalDhgCurrCorrKB(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_HDHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_HDHGC_CORR_KB_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepShuChgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入分流器充电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepShuChgCurrCorrKB(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SCHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_SCHGC_CORR_KB_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepShuDhgCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入分流器放电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepShuDhgCurrCorrKB(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SDHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_SDHGC_CORR_KB_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepShuChHCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入分流器大充电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepShuChHCurrCorrKB(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SCHHC_CORR_KB_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_SCHHC_CORR_KB_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepShuDhHCurrCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入分流器大放电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepShuDhHCurrCorrKB(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SDHHC_CORR_KB_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_SDHHC_CORR_KB_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepSumVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入电池总电压校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepSumVoltCorrKB(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SUMV_CORR_KB_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_SUMV_CORR_KB_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepPreVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入预充电压校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepPreVoltCorrKB(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_PREV_CORR_KB_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_PREV_CORR_KB_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepLoaVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入负载电压校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepLoaVoltCorrKB(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_LOAV_CORR_KB_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_LOAV_CORR_KB_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepPosVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入Link+电压校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepPosVoltCorrKB(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_POSV_CORR_KB_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_POSV_CORR_KB_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepNegVoltCorrKB(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入Link-电压校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepNegVoltCorrKB(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_NEGV_CORR_KB_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_NEGV_CORR_KB_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepSysClock(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入系统时钟(北京时间)
** @The notes :
**===============================================================================================*/
u8 WriteIntEepSysClock(u8 index, u16 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SYS_CLOCK_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_SYS_CLOCK_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepMonErrTime(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入月历史故障统计时间(年月)
** @The notes :
**===============================================================================================*/
u8 WriteIntEepMonErrTime(u8 index, u16 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_MONERR_TIME_LEN))
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_MONERR_TIME_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepNowCapSoh(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入当前总容量SOH
** @The notes :
**===============================================================================================*/
u8 WriteIntEepNowCapSoh(u8 index, u16 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_NOW_CAP_SOH_LEN))
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_NOW_CAP_SOH_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepChgTimesState(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入充放电次数检测状态
** @The notes :
**===============================================================================================*/
u8 WriteIntEepChgTimesState(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_CHGTIMES_STA_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_CHGTIMES_STA_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepCapEnerCorrInfo(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入总容量总能量自动标定信息值
** @The notes :
**===============================================================================================*/
u8 WriteIntEepCapEnerCorrInfo(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_CAPENER_CORR_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_CAPENER_CORR_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepCapFormInfo(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入电量计算结构信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepCapFormInfo(u8 index, u32 *data, u8 len)
{
	u8 i = 0;

    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_CAP_FORM_LEN))
    {
        return(FALSE);
    }

	/*逐个检测参数有效性*/
    for(i = 0; i < len; i++)
    {
		/*需写入的参数无效*/
		if((0 != (index + i)) && (data[i] > MAX_ALL_CAP_CFG))
		{
			return(FALSE);
		}
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_CAP_FORM_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepEnerFormInfo(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入电能计算结构信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepEnerFormInfo(u8 index, u32 *data, u8 len)
{
	u8 i = 0;

    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_ENER_FORM_LEN))
    {
        return(FALSE);
    }

	/*逐个检测参数有效性*/
    for(i = 0; i < len; i++)
    {
		/*需写入的参数无效*/
    	if((0 != (index + i)) && (data[i] > MAX_ALL_ENER_CFG))
		{
			return(FALSE);
		}
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_ENER_FORM_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepSysRunTime(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入系统运行(上电)时长 1Hour
** @The notes :
**===============================================================================================*/
u8 WriteIntEepSysRunTime(u8 index, u16 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SYS_RUNTIME_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_SYS_RUNTIME_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepChgDhgTimes(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入充放电次数
** @The notes :
**===============================================================================================*/
u8 WriteIntEepChgDhgTimes(u8 index, u16 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_CHG_TIMES_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_CHG_TIMES_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepHisErrNum(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入历史故障告警数目
** @The notes :
**===============================================================================================*/
u8 WriteIntEepHisErrNum(u8 index, u16 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_HIS_ERRNUM_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_HIS_ERRNUM_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepDODStaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入放电深度DOD统计信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepDODStaInfo(u8 index, u16 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_DOD_INFO_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_DOD_INFO_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepSOSLockInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入安全等级SOS锁定信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepSOSLockInfo(u8 index, u16 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SOS_INFO_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_SOS_INFO_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepHisChgDhgCap(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入累计充放电电量 1AH
** @The notes :
**===============================================================================================*/
u8 WriteIntEepHisChgDhgCap(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_HIS_CHGCAP_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_HIS_CHGCAP_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepHisChgDhgEner(u8 index, u32 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入累计充放电电能 0.1kWh
** @The notes :
**===============================================================================================*/
u8 WriteIntEepHisChgDhgEner(u8 index, u32 *data, u8 len)
{
    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_HIS_CHGENER_LEN))
    {
        return(FALSE);
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteLong(PARA_HIS_CHGENER_ADDR + (4 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepGroupGenParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入电池组主控通用参数信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepGroupGenParaInfo(u8 index, u16 *data, u8 len)
{
	u8 i = 0;

    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_DEVGEN_PARA_LEN))
    {
        return(FALSE);
    }

	/*逐个检测参数有效性*/
    for(i = 0; i < len; i++)
    {
		/*参数非有效值*/
		if(FALSE == ParaCheckGGenParaEffect(PARA_DEVGEN_PARA_ADDR + (2 * (index + i)), data[i]))
		{
			return(FALSE);
		}
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_DEVGEN_PARA_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepGroupHigParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入电池组主控高级参数信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepGroupHigParaInfo(u8 index, u16 *data, u8 len)
{
	u8 i = 0;

    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_DEVHIG_PARA_LEN))
    {
        return(FALSE);
    }

	/*逐个检测参数有效性*/
    for(i = 0; i < len; i++)
    {
		/*参数非有效值*/
		if(FALSE == ParaCheckGHigParaEffect(PARA_DEVHIG_PARA_ADDR + (2 * (index + i)), data[i]))
		{
			return(FALSE);
		}
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_DEVHIG_PARA_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepGroupHWParaInfo(u8 index, u8 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入电池组主控硬件参数信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepGroupHWParaInfo(u8 index, u8 *data, u8 len)
{
	u8 i = 0;

    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_DEVHW_PARA_LEN))
    {
        return(FALSE);
    }

	/*逐个检测参数有效性*/
    for(i = 0; i < len; i++)
    {
		/*参数非有效值*/
		if(FALSE == ParaCheckGHWParaEffect(PARA_DEVHW_PARA_ADDR + index + i, data[i]))
		{
			return(FALSE);
		}
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteByte(PARA_DEVHW_PARA_ADDR + index, data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepSystemWorkMode(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入系统工作模式信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepSystemWorkMode(u8 index, u16 *data, u8 len)
{
	u8 i = 0;

    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SYSWORK_MOD_LEN))
    {
        return(FALSE);
    }

	/*逐个检测参数有效性*/
    for(i = 0; i < len; i++)
    {
		/*参数非有效值*/
		if((1 == (index + i)) && (data[i] > 1))
		{
			return(FALSE);
		}
    }

	/*向外部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_SYSWORK_MOD_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepSystemGenParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入系统设备通用参数信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepSystemGenParaInfo(u8 index, u16 *data, u8 len)
{
	u8 i = 0;

    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SYSGEN_PARA_LEN))
    {
        return(FALSE);
    }

	/*逐个检测参数有效性*/
    for(i = 0; i < len; i++)
    {
		/*参数非有效值*/
		if(FALSE == ParaCheckSGenParaEffect(PARA_SYSGEN_PARA_ADDR + (2 * (index + i)), data[i]))
		{
			return(FALSE);
		}
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_SYSGEN_PARA_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepSystemHWParaInfo(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入系统设备硬件参数信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepSystemHWParaInfo(u8 index, u16 *data, u8 len)
{
	u8 i = 0;

    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SYSHW_PARA_LEN))
    {
        return(FALSE);
    }

	/*逐个检测参数有效性*/
    for(i = 0; i < len; i++)
    {
		/*参数非有效值*/
		if(FALSE == ParaCheckSHWParaEffect(PARA_SYSHW_PARA_ADDR + (2 * (index + i)), data[i]))
		{
			return(FALSE);
		}
    }

	/*向内部EEPROM写入参数值*/
	(void)BSPINTEEPWriteWord(PARA_SYSHW_PARA_ADDR + (2 * index), data, len);

    return(TRUE);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

