/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ParaRead.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 参数存储模块读写参数初始化
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "ParaInterface.h"
#include "ParaExtEep.h"
#include "ParaIntEep.h"
#include "ParaIntFlash.h"
#include "ParaCopy.h"
#include "ParaRead.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
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
** @Name      : u8 ParaReadStoreDataByte(u16 addr, u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取指定长度字节数
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreDataByte(u16 addr, u8 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = BSPINTEEPReadByte(addr, data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xff;
    	    result = DEVEXTEEPReadByte(addr, data, len);
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xff;
    	addr = ParaChangEepToFlashAddr(addr);
    	result = BSPINTFLASHReadByte(addr, data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreDevSerial(u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电流零飘修正值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreDevSerial(u8 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepDevSerial(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xff;
    	    result = ReadExtEepDevSerial(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepDevSerial(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_DEV_SERIAL_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xff;
    	result = ReadIntFlashDevSerial(data, len);

        /*读内部FLASH成功*/
	    if(TRUE == result)
	    {
            /*将参数重新写入内部EEPROM*/
	    	WriteIntEepDevSerial(0, data, len);

            /*将参数重新写入外部EEPROM*/
	    	//WriteExtEepDevSerial(0, data, len);

	    	/*请求备份到外部EEPROM*/
	    	ParaSetEepCopyReqFlag(PARA_DEV_SERIAL_ADDR, len, 0);
	    }
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreCorrZeroCurr(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电流零飘修正值初始化(有符号,单位10mA)
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreCorrZeroCurr(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepCorrZeroCurr(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
    	    result = ReadExtEepCorrZeroCurr(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepCorrZeroCurr(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_ZERO_C_CORR_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashCorrZeroCurr(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreHalChgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取霍尔充电电流校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreHalChgCurrCorrKB(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepHalChgCurrCorrKB(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
            result = ReadExtEepHalChgCurrCorrKB(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepHalChgCurrCorrKB(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_HCHGC_CORR_KB_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashHalChgCurrCorrKB(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreHalDhgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取霍尔放电电流校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreHalDhgCurrCorrKB(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepHalDhgCurrCorrKB(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
    	    result = ReadExtEepHalDhgCurrCorrKB(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepHalDhgCurrCorrKB(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_HDHGC_CORR_KB_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashHalDhgCurrCorrKB(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreShuChgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取分流器充电电流校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreShuChgCurrCorrKB(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepShuChgCurrCorrKB(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
    	    result = ReadExtEepShuChgCurrCorrKB(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepShuChgCurrCorrKB(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_SCHGC_CORR_KB_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashShuChgCurrCorrKB(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreShuDhgCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取分流器放电电流校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreShuDhgCurrCorrKB(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepShuDhgCurrCorrKB(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
    	    result = ReadExtEepShuDhgCurrCorrKB(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepShuDhgCurrCorrKB(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_SDHGC_CORR_KB_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashShuDhgCurrCorrKB(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreShuChHCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取分流器充电大电流校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreShuChHCurrCorrKB(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepShuChHCurrCorrKB(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
    	    result = ReadExtEepShuChHCurrCorrKB(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepShuChHCurrCorrKB(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_SCHHC_CORR_KB_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashShuChHCurrCorrKB(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreShuDhHCurrCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取分流器放电大电流校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreShuDhHCurrCorrKB(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepShuDhHCurrCorrKB(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
    	    result = ReadExtEepShuDhHCurrCorrKB(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepShuDhHCurrCorrKB(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_SDHHC_CORR_KB_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashShuDhHCurrCorrKB(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreSumVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电池总电压校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreSumVoltCorrKB(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepSumVoltCorrKB(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
            result = ReadExtEepSumVoltCorrKB(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepSumVoltCorrKB(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_SUMV_CORR_KB_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashSumVoltCorrKB(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStorePreVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取预充电压校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStorePreVoltCorrKB(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepPreVoltCorrKB(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
            result = ReadExtEepPreVoltCorrKB(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepPreVoltCorrKB(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_PREV_CORR_KB_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashPreVoltCorrKB(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreLoaVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取预负载压校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreLoaVoltCorrKB(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepLoaVoltCorrKB(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
            result = ReadExtEepLoaVoltCorrKB(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepLoaVoltCorrKB(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_LOAV_CORR_KB_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashLoaVoltCorrKB(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStorePosVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取Link+电压校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStorePosVoltCorrKB(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepPosVoltCorrKB(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
            result = ReadExtEepPosVoltCorrKB(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepPosVoltCorrKB(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_POSV_CORR_KB_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashPosVoltCorrKB(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreNegVoltCorrKB(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取Link-电压校准KB值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreNegVoltCorrKB(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepNegVoltCorrKB(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
            result = ReadExtEepNegVoltCorrKB(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepNegVoltCorrKB(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_NEGV_CORR_KB_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashNegVoltCorrKB(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreSysClock(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取系统时钟(北京时间)初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreSysClock(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepSysClock(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
            result = ReadExtEepSysClock(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepSysClock(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_SYS_CLOCK_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashSysClock(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreMonErrTime(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取月故障统计时间(年月)初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreMonErrTime(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepMonErrTime(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
            result = ReadExtEepMonErrTime(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepMonErrTime(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_MONERR_TIME_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashMonErrTime(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreNowCapSoh(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取当前容量SOH值初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreNowCapSoh(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepNowCapSoh(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
            result = ReadExtEepNowCapSoh(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepNowCapSoh(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_NOW_CAP_SOH_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashNowCapSoh(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreChgTimesState(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取充放电次数检测状态初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreChgTimesState(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepChgTimesState(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
            result = ReadExtEepChgTimesState(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepChgTimesState(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_CHGTIMES_STA_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashChgTimesState(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreCapEnerCorrInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取总容量总能量自动标定信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreCapEnerCorrInfo(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepCapEnerCorrInfo(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
            result = ReadExtEepCapEnerCorrInfo(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepCapEnerCorrInfo(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_CAPENER_CORR_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashCapEnerCorrInfo(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreCapFormInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电量计算结构信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreCapFormInfo(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepCapFormInfo(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
            result = ReadExtEepCapFormInfo(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepCapFormInfo(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_CAP_FORM_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashCapFormInfo(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreEnerFormInfo(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电能计算结构信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreEnerFormInfo(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepEnerFormInfo(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
            result = ReadExtEepEnerFormInfo(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepEnerFormInfo(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_ENER_FORM_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashEnerFormInfo(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreSysRunTime(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取系统运行(上电)时长初始化 1Hour
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreSysRunTime(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepSysRunTime(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
            result = ReadExtEepSysRunTime(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepSysRunTime(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_SYS_RUNTIME_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashSysRunTime(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreChgDhgTimes(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function(u: 从存储区读取充放电次数初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreChgDhgTimes(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepChgDhgTimes(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
            result = ReadExtEepChgDhgTimes(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepChgDhgTimes(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_CHG_TIMES_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashChgDhgTimes(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreHisErrNum(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取历史故障告警数目初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreHisErrNum(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepHisErrNum(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
            result = ReadExtEepHisErrNum(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepHisErrNum(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_HIS_ERRNUM_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashHisErrNum(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreDODStaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取放电深度统计信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreDODStaInfo(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepDODStaInfo(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
            result = ReadExtEepDODStaInfo(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepDODStaInfo(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_DOD_INFO_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashDODStaInfo(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreSOSLockInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取安全等级SOS锁定信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreSOSLockInfo(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepSOSLockInfo(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
            result = ReadExtEepSOSLockInfo(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepSOSLockInfo(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_SOS_INFO_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashSOSLockInfo(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreHisChgDhgCap(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取累计充放电电量初始化 1AH
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreHisChgDhgCap(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepHisChgDhgCap(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
            result = ReadExtEepHisChgDhgCap(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepHisChgDhgCap(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_HIS_CHGCAP_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashHisChgDhgCap(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreHisChgDhgEner(u32 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取累计充放电电能初始化 0.1kWh
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreHisChgDhgEner(u32 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepHisChgDhgEner(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffffffff;
    	    result = ReadExtEepHisChgDhgEner(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepHisChgDhgEner(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_HIS_CHGENER_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffffffff;
    	result = ReadIntFlashHisChgDhgEner(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreGroupGenParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电池组主控通用参数信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreGroupGenParaInfo(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepGroupGenParaInfo(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
    	    result = ReadExtEepGroupGenParaInfo(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepGroupGenParaInfo(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_DEVGEN_PARA_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashGroupGenParaInfo(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreGroupHigParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电池组主控高级参数信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreGroupHigParaInfo(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepGroupHigParaInfo(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
    	    result = ReadExtEepGroupHigParaInfo(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepGroupHigParaInfo(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_DEVHIG_PARA_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashGroupHigParaInfo(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreGroupHWParaInfo(u8 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取电池组主控硬件参数信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreGroupHWParaInfo(u8 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepGroupHWParaInfo(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xff;
    	    result = ReadExtEepGroupHWParaInfo(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepGroupHWParaInfo(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_DEVHW_PARA_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xff;
    	result = ReadIntFlashGroupHWParaInfo(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreSystemWorkMode(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取系统工作模式
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreSystemWorkMode(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepSystemWorkMode(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
    	    result = ReadExtEepSystemWorkMode(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepSystemWorkMode(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_SYSWORK_MOD_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashSystemWorkMode(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreSystemGenParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取系统通用参数信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreSystemGenParaInfo(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepSystemGenParaInfo(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
    	    result = ReadExtEepSystemGenParaInfo(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepSystemGenParaInfo(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_SYSGEN_PARA_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashSystemGenParaInfo(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaReadStoreSystemHWParaInfo(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取系统硬件参数信息初始化
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStoreSystemHWParaInfo(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepSystemHWParaInfo(data, len);

    /*读内部EEPROM失败*/
    if(FALSE == result)
    {
        /*外部EEPROM读写正常*/
        if(TRUE == ParaCheckExtEepReadyHook())
        {
            /*读外部EEPROM*/
            data[0] = 0xffff;
    	    result = ReadExtEepSystemHWParaInfo(data, len);

            /*读外部EEPROM成功*/
    	    if(TRUE == result)
    	    {
                /*将参数重新写入内部EEPROM*/
    	    	WriteIntEepSystemHWParaInfo(0, data, len);
    	    }
            /*读外部EEPROM失败*/
    	    else
    	    {
    	    	/*请求备份到外部EEPROM*/
    	    	ParaSetEepCopyReqFlag(PARA_SYSHW_PARA_ADDR, len, 0);
    	    }
        }
    }

    /*读内外EEPROM失败*/
    if(FALSE == result)
    {
        /*读内部FLASH*/
    	data[0] = 0xffff;
    	result = ReadIntFlashSystemHWParaInfo(data, len);
    }

    return(result);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

