/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : SciParaHandle.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 串口参数配处理模块接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "PublicPara.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "ParaUser.h"
#include "Authority.h"
#include "SciInfoPoint.h"
#include "SciCtrlHandle.h"
#include "SciParaHandle.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u16 sSciParaCfgGroupId = 0;                          /*串口配置参数组号(可读写)*/
static u16 sSciParaCfgHMIValue = 0;                         /*触摸屏串口配置参数设置值(可读写)*/
static u16 sSciParaCfgHMIAffirm = 0;                        /*触摸屏串口配置参数设置确认(可读写)*/

extern u16 gSysGenParaRO_100[];			                    /*系统通用参数设置结果*/
extern u16 gSysHigParaRO_101[];			                    /*系统硬件参数设置结果*/
extern u16 gGBmuGenPara_102[];  							/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];							/*主控高级参数值*/
extern u8 gGHardPara_104[];

extern u16 gSysGenParaRW_105[];			                    /*系统通用参数设置*/
extern u16 gSysHigParaRW_106[];			                    /*系统硬件参数设置*/
extern u16 gGBmuGenParaWR_107[];							/*主控通用参数配置*/
extern u16 gGBmuHigLevParaWR_108[];							/*主控高级参数配置*/
extern u8 gGHardParaWR_109[];								/*主控硬件参数配置*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u16 *SciGetPtrParaCfgGroupId(void)
** @Input     : void
** @Output    : 当前读写参数配置组号寄存器地址指针
** @Function  : 获取当前读写参数配置组号寄存器地址指针
** @The notes : 取寄存器地址
**===============================================================================================*/
u16 *SciGetPtrParaCfgGroupId(void)
{
    return(&sSciParaCfgGroupId);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrSysGenParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写系统通用参数配置寄存器地址指针
** @Function  : 获取读写系统通用参数配置寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrSysGenParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
{
	if(index < SINDEX105_LEN)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*写入*/
			gSysGenParaRW_105[index] = data;

			/*执行写参数*/
			(void)RcvSystemGenParaCfgAPI(index, eAuthP_Sci0 + sciNum);
		}

		return(&gSysGenParaRW_105[index]);
	}

	return(SciGetPtrDefaultValueWR());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrSysHWParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写系统通用参数配置寄存器地址指针
** @Function  : 获取读写系统通用参数配置寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrSysHWParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
{
	if(index < SINDEX106_LEN)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*写入*/
			gSysHigParaRW_106[index] = data;

			/*执行写参数*/
			(void)RcvSystemHWParaCfgAPI(index, eAuthP_Sci0 + sciNum);
		}

		return(&gSysHigParaRW_106[index]);
	}

	return(SciGetPtrDefaultValueWR());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupGenParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写主控通用参数配置寄存器地址指针
** @Function  : 获取读写主控通用参数配置寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupGenParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
{
	if(index < GINDEX107_LEN)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*写入*/
			gGBmuGenParaWR_107[index] = data;

			/*执行写参数*/
			(void)RcvGroupGenParaCfgAPI(index, eAuthP_Sci0 + sciNum);
		}

		return(&gGBmuGenParaWR_107[index]);
	}

	return(SciGetPtrDefaultValueWR());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupHigParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写主控高级参数配置寄存器地址指针
** @Function  : 获取读写主控高级参数配置寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupHigParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
{
	if(index < GINDEX108_LEN)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*写入*/
			gGBmuHigLevParaWR_108[index] = data;

			/*执行写参数*/
			(void)RcvGroupHigParaCfgAPI(index, eAuthP_Sci0 + sciNum);
		}

		return(&gGBmuHigLevParaWR_108[index]);
	}

	return(SciGetPtrDefaultValueWR());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupHWParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写主控硬件参数配置寄存器地址指针
** @Function  : 获取读写主控硬件参数配置寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupHWParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
{
	if(index < GINDEX109_LEN)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*写入*/
			gGHardParaWR_109[index] = data & 0xff;

			/*执行写参数*/
			(void)RcvGroupHWParaCfgAPI(index, eAuthP_Sci0 + sciNum);
		}

		return(SciGetPtrDefaultValueWR());
	}

	return(SciGetPtrDefaultValueWR());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupHMIParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写主控触摸屏配置参数配置寄存器地址指针
** @Function  : 获取读写主控触摸屏配置参数配置寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupHMIParaCfgWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
{
	/*写入参数值*/
	if(0 == index)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*写入*/
			sSciParaCfgHMIValue = data;
		}

		return(&sSciParaCfgHMIValue);

	}
	/*写入确认*/
	else if(1 == index)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*写入*/
			sSciParaCfgHMIAffirm = data;

			/*执行写参数*/
			;
		}

		return(&sSciParaCfgHMIAffirm);
	}

	return(SciGetPtrDefaultValueWR());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrSystemGenParaCfgRO(u8 index)
** @Input     : index:索引号
** @Output    : 读系统通用参数值信息寄存器地址指针
** @Function  : 获取读系统通用参数值信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrSystemGenParaCfgRO(u8 index)
{
	if(index < SINDEX100_LEN)
	{
		/*读取当前参数值*/
		return(&gSysGenParaRO_100[index]);
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrSystemHWParaCfgRO(u8 index)
** @Input     : index:索引号
** @Output    : 读系统硬件参数值信息寄存器地址指针
** @Function  : 获取读系统硬件参数值信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrSystemHWParaCfgRO(u8 index)
{
	if(index < SINDEX101_LEN)
	{
		/*读取当前参数值*/
		return(&gSysHigParaRO_101[index]);
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupGenParaCfgRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组主控通用参数值信息寄存器地址指针
** @Function  : 获取读电池组主控通用参数值信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupGenParaCfgRO(u8 index)
{
	if(index < GINDEX102_LEN)
	{
		/*读取当前参数值*/
		return(&gGBmuGenPara_102[index]);
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupHigParaCfgRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组主控高级参数值信息寄存器地址指针
** @Function  : 获取读电池组主控高级参数值信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupHigParaCfgRO(u8 index)
{
	if(index < GINDEX103_LEN)
	{
		/*读取当前参数值*/
		return(&gGBmuHigLevPara_103[index]);
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupHWParaCfgRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组主控硬件参数值信息寄存器地址指针
** @Function  : 获取读电池组主控硬件参数值信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupHWParaCfgRO(u8 index)
{
	if(index < GINDEX104_LEN)
	{
		/*读取当前参数值*/
		sSciParaCfgHMIValue = gGHardPara_104[index];

		return(&sSciParaCfgHMIValue);
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

