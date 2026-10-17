/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : SciInfoPoint.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 串口信息模块指针接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "CANUser.h"
#include "ModbusMap.h"
#include "SciInfoPoint.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u16 sSciDefaultValueRO = 0;                          /*串口读信息默认值*/
static u16 sSciROPortDataL[eSCINUM+1] = {0};                /*读端口临时数据信息低位寄存器(只读)*/
static u16 sSciROPortDataH[eSCINUM+1] = {0};                /*读端口临时数据信息高位寄存器(只读)*/

extern u16 gCANDevInfo_1[];			                        /*设备信息*/

extern u16 gSProjecInfo_40[];                               /*项目参数信息数组*/

extern u16 gGHisErrInfo_45[];                               /*历史故障记录信息查询数组*/
extern u16 gGHisEventInfo_46[];                             /*历史事件记录信息查询数组*/

extern u16 gGSysStateInfo_50[];                             /*系统状态信息数组*/
extern u16 gGLimCPInfo_51[];                                /*电流功率限值数组*/
extern u16 gGLimVInfo_52[];                                 /*电压限值信息数组*/
extern u16 gGAlarmInfo_53[];                                /*告警信息数组*/
extern u16 gGFaultInfo_54[];                                /*故障信息数组*/
extern u16 gGErrDetail_55[];                                /*故障定位信息数组*/
extern u16 gGPortStateInfo_56[];                            /*输入输出口状态信息数组*/
extern s16 gGSysMainInfo_60[];                              /*系统整体信息数组*/
extern u16 gGSysCapInfo_61[];                               /*系统电量信息数组*/
extern u16 gGSysCellInfo_62[];                              /*系统单体信息数组*/
extern u16 gGVoltCharInfo_63[];                             /*电压特征信息数组*/
extern s16 gGTempCharInfo_64[];                             /*温度特征信息数组*/
extern s16 gGDcCTInfo_65[];                                 /*直流电流温度信息数组*/
extern u16 gGLVSampInfo_66[];                               /*低压采样信息数组*/
extern u16 gGFstLeakInfo_67[];                              /*直流电路信息数组*/
extern s16 gGHsCurrInfo_68[];                               /*高边驱动电流信息数组1*/
extern s16 gGHsCurrInfo_69[];                               /*高边驱动电流信息数组2*/
extern u16 gGStatistTime_70[];                              /*时间统计信息数组*/
extern u16 gGStatistErr_71[];                               /*故障统计信息数组*/
extern u16 gGStatistCapEner_72[];                           /*当前能量统计信息数组*/
extern u32 gGStatistHisCap_73[];                            /*累计电量统计信息数组*/
extern u32 gGStatistHisEner_74[];                           /*累计电能统计信息数组*/
extern u16 gGAuthorInfo_80[];                               /*权限级别状态信息数组*/
extern u16 gGDebugResult_82[];                              /*调试结果状态信息数组*/
extern u16 gGCalibrResult_83[];                             /*校准结果状态信息数组*/
extern u16 gGAuthorCtrl_85[];                               /*权限密码登录信息数组*/
extern u16 gGSysCmdCtrl_86[];                               /*系统控制命令信息数组*/
extern u16 gGUserCtrl_87[];                                 /*用户控制命令信息数组*/
extern u16 gGDebugCtrl_88[];                                /*系统调试命令信息数组*/
extern u16 gGCalibrCtrl_89[];                               /*采样校准命令信息数组*/
extern u16 gGChgBalanInfo_90[];                             /*充电均衡状态信息数组*/
extern u16 gGDhgBalanInfo_91[];                             /*放电均衡状态信息数组*/
extern u16 gGCellVoltInfo_92[];                             /*电池单体电压信息数组*/
extern s8 gGCellTempInfo_94[];                              /*电池单体温度信息数组*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u16 *SciGetPtrDefaultValueRO(void)
** @Input     : void
** @Output    : 只读串口默认值寄存器地址指针
** @Function  : 获取只读串口默认值寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16 *SciGetPtrDefaultValueRO(void)
{
	sSciDefaultValueRO = 0;

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrDeviceInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读设备信息寄存器地址指针
** @Function  : 获取读设备信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrDeviceInfoRO(u8 index)
{
	if(index < CANINDEX1_LEN)
	{
		return(&gCANDevInfo_1[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrProjectInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读项目参数信息寄存器地址指针
** @Function  : 获取读项目参数信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrProjectInfoRO(u8 index)
{
	if(index < SINDEX40_LEN)
	{
		return(&gSProjecInfo_40[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupHisErrRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组历史故障信息寄存器地址指针
** @Function  : 获取读电池组历史故障信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupHisErrRO(u8 index)
{
	if(index < GINDEX45_LEN)
	{
		/*读取电池组历史故障记录*/
		return(&gGHisErrInfo_45[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupHisEventRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组历史事件态信息寄存器地址指针
** @Function  : 获取读电池组历史事件信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupHisEventRO(u8 index)
{
	if(index < GINDEX46_LEN)
	{
		/*读取电池组历史故障记录*/
		return(&gGHisEventInfo_46[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupStateInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组状态信息寄存器地址指针
** @Function  : 获取读电池组状态信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupStateInfoRO(u8 index)
{
	if(index < GINDEX50_LEN)
	{
		/*读取电池组信息*/
		return(&gGSysStateInfo_50[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupLimitInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组限值信息寄存器地址指针
** @Function  : 获取读电池组限值信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupLimitInfoRO(u8 index)
{
	if(index < GINDEX51_LEN)
	{
		/*读取电池组电流功率限值信息*/
		return(&gGLimCPInfo_51[index]);
	}
	else if(index < (GINDEX51_LEN + GINDEX52_LEN))
	{
		/*读取电池组电压限值信息*/
		return(&gGLimVInfo_52[index - GINDEX51_LEN]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupErrInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组故障告警信息寄存器地址指针
** @Function  : 获取读电池组故障告警信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupErrInfoRO(u8 index)
{
	if(index < GINDEX53_LEN)
	{
		/*读取电池组告警信息*/
		return(&gGAlarmInfo_53[index]);
	}
	else if(index < (GINDEX53_LEN + GINDEX54_LEN))
	{
		/*读取电池组故障信息*/
		return(&gGFaultInfo_54[index - GINDEX53_LEN]);
	}
	else if(index < (GINDEX53_LEN + GINDEX54_LEN + GINDEX55_LEN ))
	{
		/*读取电池组故障定位信息*/
		return(&gGErrDetail_55[index - GINDEX53_LEN - GINDEX54_LEN]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupIoStateInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组输入输出口状态信息寄存器地址指针
** @Function  : 获取读电池组输入输出口状态信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupIoStateInfoRO(u8 index)
{
	if(index < GINDEX56_LEN)
	{
		/*读取电池组端口信息*/
		return(&gGPortStateInfo_56[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupMainInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组主要数据信息寄存器地址指针
** @Function  : 获取读电池组主要数据信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupMainInfoRO(u8 index)
{
	if(index < GINDEX60_LEN)
	{
		/*读取电池组主要数据信息*/
		return((u16*)&gGSysMainInfo_60[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupCapInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组电量状态信息寄存器地址指针
** @Function  : 获取读电池组电量状态息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupCapInfoRO(u8 index)
{
	if(index < GINDEX61_LEN)
	{
		/*读取电池组电量状态信息*/
		return(&gGSysCapInfo_61[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupCellInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组单体总体信息寄存器地址指针
** @Function  : 获取读电池组单体总体寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupCellInfoRO(u8 index)
{
	if(index < GINDEX62_LEN)
	{
		/*读取电池组单体总体信息*/
		return(&gGSysCellInfo_62[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupCharaInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组单体特征信息寄存器地址指针
** @Function  : 获取读电池组特征总体寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupCharaInfoRO(u8 index)
{
	if(index < GINDEX63_LEN)
	{
		/*读取电池组单体电压特征信息*/
		return(&gGVoltCharInfo_63[index]);
	}
	else if(index < (GINDEX63_LEN + GINDEX64_LEN))
	{
		/*读取电池组单体温度特征信息*/
		return((u16*)&gGTempCharInfo_64[index - GINDEX63_LEN]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupHigLowInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组高低压侧信息寄存器地址指针
** @Function  : 获取读电池组高低压侧信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupHigLowInfoRO(u8 index)
{
	if(index < GINDEX65_LEN)
	{
		/*读取电池组直流侧信息*/
		return((u16*)&gGDcCTInfo_65[index]);
	}
	else if(index < (GINDEX65_LEN + GINDEX66_LEN))
	{
		/*读取电池组低压采样信息*/
		return(&gGLVSampInfo_66[index - GINDEX65_LEN]);
	}
	else if(index < (GINDEX65_LEN + GINDEX66_LEN + GINDEX67_LEN))
	{
		/*读取电池组快充绝缘信息*/
		return(&gGFstLeakInfo_67[index - GINDEX65_LEN - GINDEX66_LEN]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupStatistInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组统计信息寄存器地址指针
** @Function  : 获取读电池组统计信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupStatistInfoRO(u8 index)
{
	if(index < GINDEX70_LEN)
	{
		/*读取电池组时间统计信息*/
		return(&gGStatistTime_70[index]);
	}
	else if(index < (GINDEX70_LEN + GINDEX71_LEN))
	{
		/*读取电池组故障统计信息*/
		return(&gGStatistErr_71[index - GINDEX70_LEN]);
	}
	else if(index < (GINDEX70_LEN + GINDEX71_LEN + GINDEX72_LEN))
	{
		/*读取电池组单次电量统计信息*/
		return(&gGStatistCapEner_72[index - GINDEX70_LEN - GINDEX71_LEN]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGroupHisCapEnerInfoRO(u8 sciNum, u8 index)
** @Input     : sciNum:端口号 index:索引号
** @Output    : 读电池组累计电能电量信息寄存器地址指针
** @Function  : 获取读电池组累计电能电量信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGroupHisCapEnerInfoRO(u8 sciNum, u8 index)
{
	u8 id = 0;

	id = index / 2;

	if(id < GINDEX73_LEN)
	{
		/*串口读*/
		if(sciNum < eSCINUM)
		{
			/*读取电池组累计电量信息*/
			sSciROPortDataL[sciNum] = gGStatistHisCap_73[id] & 0xffff;
			sSciROPortDataH[sciNum] = (gGStatistHisCap_73[id] >> 16) & 0xffff;

			/*读高位*/
			if(0 == (index % 2))
			{
				return(&sSciROPortDataH[sciNum]);
			}
			/*读低位*/
			else
			{
				return(&sSciROPortDataL[sciNum]);
			}
		}
		/*网口读*/
		else
		{
			/*读取电池组累计电量信息*/
			sSciROPortDataL[eSCINUM] = gGStatistHisCap_73[id] & 0xffff;
			sSciROPortDataH[eSCINUM] = (gGStatistHisCap_73[id] >> 16) & 0xffff;

			/*读高位*/
			if(0 == (index % 2))
			{
				return(&sSciROPortDataH[eSCINUM]);
			}
			/*读低位*/
			else
			{
				return(&sSciROPortDataL[eSCINUM]);
			}
		}
	}
	else if(id < (GINDEX73_LEN + GINDEX74_LEN))
	{
		/*串口读*/
		if(sciNum < eSCINUM)
		{
			/*读取电池组累计电能信息*/
			sSciROPortDataL[sciNum] = gGStatistHisEner_74[id - GINDEX73_LEN] & 0xffff;
			sSciROPortDataH[sciNum] = (gGStatistHisEner_74[id - GINDEX73_LEN] >> 16) & 0xffff;

			/*读高位*/
			if(0 == (index % 2))
			{
				return(&sSciROPortDataH[sciNum]);
			}
			/*读低位*/
			else
			{
				return(&sSciROPortDataL[sciNum]);
			}
		}
		/*网口读*/
		else
		{
			/*读取电池组累计电能信息*/
			sSciROPortDataL[eSCINUM] = gGStatistHisEner_74[id - GINDEX73_LEN] & 0xffff;
			sSciROPortDataH[eSCINUM] = (gGStatistHisEner_74[id - GINDEX73_LEN] >> 16) & 0xffff;

			/*读高位*/
			if(0 == (index % 2))
			{
				return(&sSciROPortDataH[eSCINUM]);
			}
			/*读低位*/
			else
			{
				return(&sSciROPortDataL[eSCINUM]);
			}
		}
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGeroupCellChgBalanRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组单体充电均衡状态信息寄存器地址指针
** @Function  : 获取读电池组单体充电均衡状态信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGeroupCellChgBalanRO(u8 index)
{
	if(index < GINDEX90_LEN)
	{
		/*读取电池组单体均充状态信息*/
		return(&gGChgBalanInfo_90[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGeroupCellDhgBalanRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池组单体放电均衡状态信息寄存器地址指针
** @Function  : 获取读电池组单体放电均衡状态信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGeroupCellDhgBalanRO(u8 index)
{
	if(index < GINDEX91_LEN)
	{
		/*读取电池组单体均放状态信息*/
		return(&gGDhgBalanInfo_91[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGeroupCellVoltStateRO(u16 index)
** @Input     : index:索引号
** @Output    : 读电池组单体电压值信息寄存器地址指针
** @Function  : 获取读电池组单体电压值信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGeroupCellVoltStateRO(u16 index)
{
	if(index < MAX_CELL_NUM)
	{
		/*读取电池组单体电压值信息*/
		return(&gGCellVoltInfo_92[index]);
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrGeroupCellTempInfoRO(u8 sciNum, u16 index)
** @Input     : sciNum:端口号 index:索引号
** @Output    : 读电池组单体温度值信息寄存器地址指针
** @Function  : 获取读电池组单体温度值信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrGeroupCellTempInfoRO(u8 sciNum, u16 index)
{
	if(index < MAX_CELL_NUM)
	{
		/*串口读*/
		if(sciNum < eSCINUM)
		{
			/*读取电池组单体温度值信息*/
			sSciROPortDataL[sciNum] = (u16)((s16)gGCellTempInfo_94[index]);

			return(&sSciROPortDataL[sciNum]);
		}
		/*网口读*/
		else
		{
			/*读取电池组单体温度值信息*/
			sSciROPortDataL[eSCINUM] = (u16)((s16)gGCellTempInfo_94[index]);

			return(&sSciROPortDataL[eSCINUM]);
		}
	}

	return(&sSciDefaultValueRO);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/


