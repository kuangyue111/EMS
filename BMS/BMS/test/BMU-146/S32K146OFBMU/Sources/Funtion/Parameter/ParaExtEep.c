/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ParaExtEep.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 参数存储模块读写外部EEPROM参数
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
#include "ParaExtEep.h"

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
** @Name      : u8 ReadExtEepDevSerial(u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取设备条码序列号值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepDevSerial(u8 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEV_SERIAL_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadByte(PARA_DEV_SERIAL_ADDR, data, len);

        if(PARA_CFG_FLAG_U8 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepCorrZeroCurr(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取电流零飘修正值(有符号,单位10mA)
** @The notes :
**===============================================================================================*/
u8 ReadExtEepCorrZeroCurr(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_ZERO_C_CORR_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadWord(PARA_ZERO_C_CORR_ADDR, data, len);

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
** @Name      : u8 ReadExtEepHalChgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取霍尔充电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepHalChgCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HCHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_HCHGC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepHalDhgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取霍尔放电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepHalDhgCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HDHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_HDHGC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepShuChgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取分流器充电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepShuChgCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SCHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_SCHGC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepShuDhgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取分流器放电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepShuDhgCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SDHGC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_SDHGC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepShuChHCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取分流器充电大电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepShuChHCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SCHHC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_SCHHC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepShuDhHCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取分流器放电大电流校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepShuDhHCurrCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SDHHC_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_SDHHC_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepSumVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取电池总电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepSumVoltCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SUMV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_SUMV_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepPreVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取预充电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepPreVoltCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_PREV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_PREV_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepLoaVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取负载电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepLoaVoltCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_LOAV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_LOAV_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepPosVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取Link+电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepPosVoltCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_POSV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_POSV_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepNegVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取Link-电压校准KB值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepNegVoltCorrKB(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_NEGV_CORR_KB_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_NEGV_CORR_KB_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepSysClock(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取系统时钟(北京时间)
** @The notes :
**===============================================================================================*/
u8 ReadExtEepSysClock(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SYS_CLOCK_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadWord(PARA_SYS_CLOCK_ADDR, data, len);

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
** @Name      : u8 ReadExtEepMonErrTime(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取月故障数目统计时间(年月)
** @The notes :
**===============================================================================================*/
u8 ReadExtEepMonErrTime(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_MONERR_TIME_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadWord(PARA_MONERR_TIME_ADDR, data, len);

        /*读取到有效参数*/
        if((PARA_CFG_FLAG_U16 == data[0]) && (data[1] >= 2000))
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepNowCapSoh(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取当前总容量SOH值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepNowCapSoh(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_NOW_CAP_SOH_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadWord(PARA_NOW_CAP_SOH_ADDR, data, len);

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
** @Name      : u8 ReadExtEepChgTimesState(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取充放电次数检测状态
** @The notes :
**===============================================================================================*/
u8 ReadExtEepChgTimesState(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SYS_CLOCK_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_CHGTIMES_STA_ADDR, data, len);

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
** @Name      : u8 ReadExtEepCapEnerCorrInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取总容量总能量自动标定信息值
** @The notes :
**===============================================================================================*/
u8 ReadExtEepCapEnerCorrInfo(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_CAPENER_CORR_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_CAPENER_CORR_ADDR, data, len);

        if(PARA_CFG_FLAG_U32 == data[0])
        {
            return(TRUE);
        }
        BSPWatchDogOutput();
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepCapFormInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取电量计算结构信息
** @The notes :
**===============================================================================================*/
u8 ReadExtEepCapFormInfo(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_CAP_FORM_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_CAP_FORM_ADDR, data, len);

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
** @Name      : u8 ReadExtEepEnerFormInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取电能计算结构信息
** @The notes :
**===============================================================================================*/
u8 ReadExtEepEnerFormInfo(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_ENER_FORM_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_ENER_FORM_ADDR, data, len);

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
** @Name      : u8 ReadExtEepSysRunTime(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取系统运行(上电)时长 1Hour
** @The notes :
**===============================================================================================*/
u8 ReadExtEepSysRunTime(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SYS_RUNTIME_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadWord(PARA_SYS_RUNTIME_ADDR, data, len);

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
** @Name      : u8 ReadExtEepChgDhgTimes(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取充放电次数
** @The notes :
**===============================================================================================*/
u8 ReadExtEepChgDhgTimes(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_CHG_TIMES_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadWord(PARA_CHG_TIMES_ADDR, data, len);

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
** @Name      : u8 ReadExtEepHisErrNum(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取历史故障告警数目
** @The notes :
**===============================================================================================*/
u8 ReadExtEepHisErrNum(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HIS_ERRNUM_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadWord(PARA_HIS_ERRNUM_ADDR, data, len);

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
** @Name      : u8 ReadExtEepDODStaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取放电深度DOD统计信息
** @The notes :
**===============================================================================================*/
u8 ReadExtEepDODStaInfo(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_DOD_INFO_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadWord(PARA_DOD_INFO_ADDR, data, len);

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
** @Name      : u8 ReadExtEepSOSLockInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取安全等级SOS锁定信息
** @The notes :
**===============================================================================================*/
u8 ReadExtEepSOSLockInfo(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SOS_INFO_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadWord(PARA_SOS_INFO_ADDR, data, len);

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
** @Name      : u8 ReadExtEepHisChgDhgCap(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取累计充放电电量 1AH
** @The notes :
**===============================================================================================*/
u8 ReadExtEepHisChgDhgCap(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HIS_CHGCAP_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_HIS_CHGCAP_ADDR, data, len);

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
** @Name      : u8 ReadExtEepHisChgDhgEner(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取累计充放电电能 0.1kWh
** @The notes :
**===============================================================================================*/
u8 ReadExtEepHisChgDhgEner(u32 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_HIS_CHGENER_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadLong(PARA_HIS_CHGENER_ADDR, data, len);

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
** @Name      : u8 ReadExtEepGroupGenParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取电池组主控通用参数信息
** @The notes :
**===============================================================================================*/
u8 ReadExtEepGroupGenParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEVGEN_PARA_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
    	(void)DEVEXTEEPReadWord(PARA_DEVGEN_PARA_ADDR, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)DEVEXTEEPReadWord(PARA_DEVGEN_PARA_ADDR + 2, &data[1], (len - 1));

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

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepGroupHigParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取电池组主控高级参数信息
** @The notes :
**===============================================================================================*/
u8 ReadExtEepGroupHigParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEVHIG_PARA_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
    	(void)DEVEXTEEPReadWord(PARA_DEVHIG_PARA_ADDR, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)DEVEXTEEPReadWord(PARA_DEVHIG_PARA_ADDR + 2, &data[1], (len - 1));

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

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepGroupHWParaInfo(u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取电池组主控硬件参数信息
** @The notes :
**===============================================================================================*/
u8 ReadExtEepGroupHWParaInfo(u8 *data, u8 len)
{
	u8 i = 0;
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_DEVHW_PARA_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
    	(void)DEVEXTEEPReadByte(PARA_DEVHW_PARA_ADDR, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U8 == data[0])
        {
            /*读取所有参数值*/
            (void)DEVEXTEEPReadByte(PARA_DEVHW_PARA_ADDR + 1, &data[1], (len - 1));

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

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepSystemWorkMode(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取系统工作模式
** @The notes :
**===============================================================================================*/
u8 ReadExtEepSystemWorkMode(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SYSWORK_MOD_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
        (void)DEVEXTEEPReadWord(PARA_SYSWORK_MOD_ADDR, data, 2);

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
** @Name      : u8 ReadExtEepSystemGenParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取系统通用参数信息
** @The notes :
**===============================================================================================*/
u8 ReadExtEepSystemGenParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_SYSGEN_PARA_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
    	(void)DEVEXTEEPReadWord(PARA_SYSGEN_PARA_ADDR, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)DEVEXTEEPReadWord(PARA_SYSGEN_PARA_ADDR + 2, &data[1], (len - 1));

            /*逐个检测参数是否有效*/
        	for(i = 1; i < len; i++)
        	{
                /*读取到无效参数值*/
                if(FALSE == ParaCheckSGenParaEffect(PARA_SYSGEN_PARA_ADDR + (2 * i), data[i]))
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

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ReadExtEepSystemHWParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从外部EEPROM读取系统设备硬件参数信息
** @The notes :
**===============================================================================================*/
u8 ReadExtEepSystemHWParaInfo(u16 *data, u8 len)
{
	u8 i = 0;
    u8 trytime = 5;

    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if((NULL == data) || (len <= 1) || (len > PARA_SYSHW_PARA_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(trytime--)
    {
    	(void)DEVEXTEEPReadWord(PARA_SYSHW_PARA_ADDR, &data[0], 1);

        /*读取到有效参数标志*/
        if(PARA_CFG_FLAG_U16 == data[0])
        {
            /*读取所有参数值*/
            (void)DEVEXTEEPReadWord(PARA_SYSHW_PARA_ADDR + 2, &data[1], (len - 1));

            /*逐个检测参数是否有效*/
        	for(i = 1; i < len; i++)
        	{
                /*读取到无效参数值*/
                if(FALSE == ParaCheckSHWParaEffect(PARA_SYSHW_PARA_ADDR + (2 * i), data[i]))
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

    return(FALSE);
}

/*外部EEPROM单个写会失败*/
#if(0)
/*=================================================================================================
** @Name      : u8 WriteExtEepDevSerial(u8 index, u8 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入设备序列号
** @The notes :
**===============================================================================================*/
u8 WriteExtEepDevSerial(u8 index, u8 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_DEV_SERIAL_LEN)
    {
        return(FALSE);
    }

    /*延时 1ms*/
    BSPSetOSMicrosecDelayAPI(1000);

    /*向外部EEPROM写入参数值*/
    (void)DEVEXTEEPWriteByte(PARA_DEV_SERIAL_ADDR + index, &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepCorrZeroCurr(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入电流零飘修正值
** @The notes :
**===============================================================================================*/
u8 WriteExtEepCorrZeroCurr(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_ZERO_C_CORR_LEN)
    {
        return(FALSE);
    }

    /*写零飘误差值*/
    if(1 == index)
    {
		/*需写入的参数无效*/
		if(((s16)data < (0 - (s16)MAX_CURR_CORR_10MA))
			|| ((s16)data > (s16)MAX_CURR_CORR_10MA))
		{
			return(FALSE);
		}
    }

    /*延时 1ms*/
    BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_ZERO_C_CORR_ADDR + (2 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepHalChgCurrCorrKB(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入霍尔充电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteExtEepHalChgCurrCorrKB(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_HCHGC_CORR_KB_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_HCHGC_CORR_KB_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepHalDhgCurrCorrKB(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入霍尔放电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteExtEepHalDhgCurrCorrKB(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_HDHGC_CORR_KB_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_HDHGC_CORR_KB_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepShuChgCurrCorrKB(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入分流器充电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteExtEepShuChgCurrCorrKB(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_SCHGC_CORR_KB_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_SCHGC_CORR_KB_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepShuDhgCurrCorrKB(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入分流器放电电流校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteExtEepShuDhgCurrCorrKB(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_SDHGC_CORR_KB_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_SDHGC_CORR_KB_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepSumVoltCorrKB(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入电池总电压校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteExtEepSumVoltCorrKB(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_SUMV_CORR_KB_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_SUMV_CORR_KB_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepPreVoltCorrKB(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入预充电压校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteExtEepPreVoltCorrKB(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_PREV_CORR_KB_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_PREV_CORR_KB_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepLoaVoltCorrKB(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入负载电压校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteExtEepLoaVoltCorrKB(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_LOAV_CORR_KB_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_LOAV_CORR_KB_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepPosVoltCorrKB(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入Link+电压校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteExtEepPosVoltCorrKB(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_POSV_CORR_KB_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_POSV_CORR_KB_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepNegVoltCorrKB(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入Link-电压校准KB值
** @The notes :
**===============================================================================================*/
u8 WriteExtEepNegVoltCorrKB(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_NEGV_CORR_KB_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_NEGV_CORR_KB_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepSysClock(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入系统时钟(北京时间)
** @The notes :
**===============================================================================================*/
u8 WriteExtEepSysClock(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_SYS_CLOCK_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_SYS_CLOCK_ADDR + (2 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepMonErrTime(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入月历史故障统计时间(年月)
** @The notes :
**===============================================================================================*/
u8 WriteExtEepMonErrTime(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_MONERR_TIME_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_MONERR_TIME_ADDR + (2 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepChgTimesState(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入充放电次数检测状态
** @The notes :
**===============================================================================================*/
u8 WriteExtEepChgTimesState(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_CHGTIMES_STA_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_CHGTIMES_STA_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepCapEnerCorrInfo(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入总容量总能量自动标定信息值
** @The notes :
**===============================================================================================*/
u8 WriteExtEepCapEnerCorrInfo(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_CAPENER_CORR_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_CAPENER_CORR_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepCapFormInfo(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入电量计算结构信息
** @The notes :
**===============================================================================================*/
u8 WriteExtEepCapFormInfo(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_CAP_FORM_LEN)
    {
        return(FALSE);
    }

    /*需写入的参数无效*/
	if((0 != index) && (data > MAX_ALL_CAP_CFG))
	{
		return(FALSE);
	}

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_CAP_FORM_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepEnerFormInfo(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入电能计算结构信息
** @The notes :
**===============================================================================================*/
u8 WriteExtEepEnerFormInfo(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_ENER_FORM_LEN)
    {
        return(FALSE);
    }

    /*需写入的参数无效*/
	if((0 != index) && (data > MAX_ALL_ENER_CFG))
	{
		return(FALSE);
	}

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_ENER_FORM_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepSysRunTime(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入系统运行(上电)时长 1Hour
** @The notes :
**===============================================================================================*/
u8 WriteExtEepSysRunTime(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_SYS_RUNTIME_LEN)
    {
        return(FALSE);
    }

	/*延时 1ms*/
	BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_SYS_RUNTIME_ADDR + (2 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepChgDhgTimes(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入充放电次数
** @The notes :
**===============================================================================================*/
u8 WriteExtEepChgDhgTimes(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_CHG_TIMES_LEN)
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_CHG_TIMES_ADDR + (2 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepHisErrNum(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入历史故障告警数目
** @The notes :
**===============================================================================================*/
u8 WriteExtEepHisErrNum(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_HIS_ERRNUM_LEN)
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_HIS_ERRNUM_ADDR + (2 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepDODStaInfo(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入放电深度统计信息
** @The notes :
**===============================================================================================*/
u8 WriteExtEepDODStaInfo(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_DOD_INFO_LEN)
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_DOD_INFO_ADDR + (2 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepHisChgDhgCap(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入累计充放电电量 1AH
** @The notes :
**===============================================================================================*/
u8 WriteExtEepHisChgDhgCap(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_HIS_CHGCAP_LEN)
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_HIS_CHGCAP_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepHisChgDhgEner(u8 index, u32 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入累计充放电电能 0.1kWh
** @The notes :
**===============================================================================================*/
u8 WriteExtEepHisChgDhgEner(u8 index, u32 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_HIS_CHGENER_LEN)
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteLong(PARA_HIS_CHGENER_ADDR + (4 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepGroupGenParaInfo(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入电池组主控通用参数信息
** @The notes :
**===============================================================================================*/
u8 WriteExtEepGroupGenParaInfo(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_DEVGEN_PARA_LEN)
    {
        return(FALSE);
    }

    /*参数非有效值*/
    if(FALSE == ParaCheckGGenParaEffect(PARA_DEVGEN_PARA_ADDR + (2 * index), data))
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_DEVGEN_PARA_ADDR + (2 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepGroupHigParaInfo(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入电池组主控高级参数信息
** @The notes :
**===============================================================================================*/
u8 WriteExtEepGroupHigParaInfo(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_DEVHIG_PARA_LEN)
    {
        return(FALSE);
    }

    /*参数非有效值*/
    if(FALSE == ParaCheckGHigParaEffect(PARA_DEVHIG_PARA_ADDR + (2 * index), data))
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_DEVHIG_PARA_ADDR + (2 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepGroupHWParaInfo(u8 index, u8 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入电池组主控硬件参数信息
** @The notes :
**===============================================================================================*/
u8 WriteExtEepGroupHWParaInfo(u8 index, u8 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_DEVHW_PARA_LEN)
    {
        return(FALSE);
    }

    /*参数非有效值*/
    if(FALSE == ParaCheckGHWParaEffect(PARA_DEVHW_PARA_ADDR + index, data))
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteByte(PARA_DEVHW_PARA_ADDR + index, &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepSystemWorkMode(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入系统工作模式信息
** @The notes :
**===============================================================================================*/
u8 WriteExtEepSystemWorkMode(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_SYSWORK_MOD_LEN)
    {
        return(FALSE);
    }

    /*参数非有效值*/
    if((1 == index) && (data > 1))
    {
        return(FALSE);
    }

    /*延时 1ms*/
    BSPSetOSMicrosecDelayAPI(1000);

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_SYSWORK_MOD_ADDR + (2 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepSystemGenParaInfo(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入系统设备通用参数信息
** @The notes :
**===============================================================================================*/
u8 WriteExtEepSystemGenParaInfo(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_SYSGEN_PARA_LEN)
    {
        return(FALSE);
    }

    /*参数非有效值*/
    if(FALSE == ParaCheckSGenParaEffect(PARA_SYSGEN_PARA_ADDR + (2 * index), data))
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_SYSGEN_PARA_ADDR + (2 * index), &data, 1);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 WriteExtEepSystemHWParaInfo(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据
** @Output    : 写入成功或失败
** @Function  : 向外部EEPROM写入系统设备硬件参数信息
** @The notes :
**===============================================================================================*/
u8 WriteExtEepSystemHWParaInfo(u8 index, u16 data)
{
    /*外部EEPROM读写异常*/
    if(FALSE == ParaCheckExtEepReadyHook())
    {
        return(FALSE);
    }

    /*输入参数无效*/
    if(index >= PARA_SYSHW_PARA_LEN)
    {
        return(FALSE);
    }

    /*参数非有效值*/
    if(FALSE == ParaCheckSHWParaEffect(PARA_SYSHW_PARA_ADDR + (2 * index), data))
    {
        return(FALSE);
    }

	/*向外部EEPROM写入参数值*/
	(void)DEVEXTEEPWriteWord(PARA_SYSHW_PARA_ADDR + (2 * index), &data, 1);

    return(TRUE);
}
#endif

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

