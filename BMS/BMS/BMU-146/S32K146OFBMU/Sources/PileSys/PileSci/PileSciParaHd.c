/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileSciParaHd.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 串口堆控参数配处理模块接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PilePara.h"
#include "ParaUser.h"
#include "SciInfoPoint.h"
#include "SciCtrlHandle.h"
#include "PileSciParaHd.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gPSmuGenParaR_102[];							  	/*堆控通用参数值*/
extern u16 gPSmuGenParaWR_107[];						  	/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u16* SciGetPtrPileGenParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写堆控通用参数配置寄存器地址指针
** @Function  : 获取读写堆控通用参数配置寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileGenParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
{
	if(index < PINDEX107_LEN)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*写入*/
			gPSmuGenParaWR_107[index] = data;

			/*执行写参数*/
			//RcvPileGenParaCfgAPI(index, eAuthP_Sci0 + sciNum);
		}

		return(&gPSmuGenParaWR_107[index]);
	}

	return(SciGetPtrDefaultValueWR());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileGenParaCfgRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆堆控通用参数值信息寄存器地址指针
** @Function  : 获取读电池堆堆控通用参数值信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileGenParaCfgRO(u8 index)
{
	/*取堆控参数*/
	if(index < PINDEX102_LEN)
	{
		/*读取当前参数值*/
		return(&gPSmuGenParaR_102[index]);
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
