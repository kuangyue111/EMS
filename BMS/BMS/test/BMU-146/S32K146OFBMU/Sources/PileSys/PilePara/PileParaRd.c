/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileParaRd.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控参数读取模块
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
#include "PileParaRd.h"

#include "ParaIntEep.h"
#include "ParaInterface.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gPGroupState_57[]; 			/*电池组状态信息数组*/

extern u16 gSysGenParaRO_100[];  		/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];  		/*堆控通用参数值*/
extern u16 gGBmuHigLevPara_103[];  		/*主控高级参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 ReadIntEepPileWorkMode(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取系统工作模式
** @The notes :
**===============================================================================================*/
static u8 ReadIntEepPileWorkMode(u16 *data, u8 len);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void ParaReadPileGenParaInit(void)
** @Input     : void
** @Output    : void
** @Function  : 堆控读取常规参数初始化
** @The notes : 用户初始化调用
**===============================================================================================*/
void ParaReadPileGenParaInit(void)
{
    u8 result = 0;
    u16 data[9] = {0};

    //从主控配置值获取EEP参数(其他使用默认值)
    gPSmuGenParaR_102[eSmuGenPara102_SmuDev] = gSysGenParaRO_100[eSGenPara100_ExtAddr];

    gPSmuGenParaR_102[eSmuGenPara102_VSumNum] = gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum];
    gPSmuGenParaR_102[eSmuGenPara102_TSumNum] = gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_BTSumNum];

    /*读取EEP电池组激活使能状态(电池组激活状态在系统统用参数里)*/
	result = ParaReadStoreSystemGenParaInfo(data, 9);

    if((TRUE == result) && (data[8] > 0))
    {
    	gPGroupState_57[ePGroupState57_EnG] = data[8];
    }
    else
    {
        if(gPGroupState_57[eSGenPara100_AllGNum] < 16)
        {
        	gPGroupState_57[ePGroupState57_EnG] = 0xffff >> (16 - gSysGenParaRO_100[eSGenPara100_AllGNum]);
        }
        else
        {
        	gPGroupState_57[ePGroupState57_EnG] = 0xffff;
        }
    }
}

/*=================================================================================================
** @Name      : u8 ParaReadStorePileWorkModeAPI(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从存储区读取堆控系统工作模式
** @The notes : 读取先后顺序:内部EEPROM->外部EEPROM->内部FLASH
**===============================================================================================*/
u8 ParaReadStorePileWorkModeAPI(u16 *data, u8 len)
{
    u8 result = 0;

    /*直接读内部EEPROM*/
	result = ReadIntEepPileWorkMode(data, len);

//    /*读内部EEPROM失败*/
//    if(FALSE == result)
//    {
//        /*外部EEPROM读写正常*/
//        if(TRUE == ParaCheckExtEepReadyHook())
//        {
//            /*读外部EEPROM*/
//            data[0] = 0xffff;
//    	    result = ReadExtEepSystemWorkMode(data, len);
//
//            /*读外部EEPROM成功*/
//    	    if(TRUE == result)
//    	    {
//                /*将参数重新写入内部EEPROM*/
//    	    	WriteIntEepSystemWorkMode(0, data, len);
//    	    }
//            /*读外部EEPROM失败*/
//    	    else
//    	    {
//    	    	/*请求备份到外部EEPROM*/
//    	    	ParaSetEepCopyReqFlag(PARA_SYSWORK_MOD_ADDR, len, 0);
//    	    }
//        }
//    }
//
//    /*读内外EEPROM失败*/
//    if(FALSE == result)
//    {
//        /*读内部FLASH*/
//    	data[0] = 0xffff;
//    	result = ReadIntFlashSystemWorkMode(data, len);
//    }

    return(result);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 ReadIntEepPileWorkMode(u16 *data, u8 len)
** @Input     : data:读取到的数据 len:读取长度
** @Output    : 读取成功或失败
** @Function  : 从内部EEPROM读取系统工作模式
** @The notes :
**===============================================================================================*/
static u8 ReadIntEepPileWorkMode(u16 *data, u8 len)
{
    u8 trytime = 5;

    /*输入参数无效*/
    if((NULL == data) || (len != PARA_SMUWORK_MOD_LEN))
    {
        return(FALSE);
    }

    /*从外部EEPROM读取参数*/
    while(--trytime)
    {
        (void)BSPINTEEPReadWord(PARA_SMUWORK_MOD_ADDR, data, len);

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
    (void)BSPINTEEPWriteWord(PARA_SMUWORK_MOD_ADDR + 2, &data[1], (len - 1));
    (void)BSPINTEEPWriteWord(PARA_SMUWORK_MOD_ADDR, &data[0], 1);

    return(FALSE);
}

#endif
