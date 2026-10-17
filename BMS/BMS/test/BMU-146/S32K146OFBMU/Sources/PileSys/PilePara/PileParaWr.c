/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileParaWr.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控参数写入模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicPara.h"
#include "PilePara.h"
#include "GroupPara.h"
#include "PileInfo.h"
#include "ParaInterface.h"
#include "PileParaWr.h"

#include "ParaIntEep.h"
#include "ParaInterface.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 WriteIntEepPileWorkMode(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入堆控工作模式信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepPileWorkMode(u8 index, u16 *data, u8 len);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 ParaWriteStorePileWorkModeAPI(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入堆控系统工作模式信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStorePileWorkModeAPI(u8 index, u16 *data, u8 len)
{
	u8 result  = 0;

    /*直接写入内部EEPROM*/
	result = WriteIntEepPileWorkMode(index, data, len);

    /*外部EEPROM读写正常*/
    if(TRUE == ParaCheckExtEepReadyHook())
    {
        /*直接写入外部EEPROM*/
    	//result = WriteExtEepPileWorkMode(index, data, len);

    	/*请求备份到外部EEPROM*/
    	//ParaSetEepCopyReqFlag(PARA_SMUWORK_MOD_ADDR + (2 * index), len, 0);
    }

    /*写入成功*/
    if(TRUE == result)
    {
        /*请求备份FLASH*/
    	//ParaSetFlashCopyReqFlag(PARA_SMUWORK_MOD_ADDR, 0);
    }

    return(result);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 WriteIntEepPileWorkMode(u8 index, u16 *data, u8 len)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入堆控工作模式信息
** @The notes :
**===============================================================================================*/
u8 WriteIntEepPileWorkMode(u8 index, u16 *data, u8 len)
{
	u8 i = 0;

    /*输入参数无效*/
    if((0 == len) || ((index + len) > PARA_SMUWORK_MOD_LEN))
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
	(void)BSPINTEEPWriteWord(PARA_SMUWORK_MOD_ADDR + (2 * index), data, len);

    return(TRUE);
}

#endif
