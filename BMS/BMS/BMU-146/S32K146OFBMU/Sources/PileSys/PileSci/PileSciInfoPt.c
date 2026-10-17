/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileSciInfoPt.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 串口堆控信息模块指针接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "PublicPara.h"
#include "PileInfo.h"
#include "GroupInfo.h"
#include "SciInfoPoint.h"
#include "PileSciInfoPt.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u16 sSciROPileDataL[eSCINUM+1] = {0};                /*读端口临时数据信息低位寄存器(只读)*/
static u16 sSciROPileDataH[eSCINUM+1] = {0};                /*读端口临时数据信息高位寄存器(只读)*/

extern u16 gGHisErrInfo_45[];                               /*历史故障记录信息查询数组*/
extern u16 gGHisEventInfo_46[];                             /*历史事件记录信息查询数组*/

extern u16 gPSysStateInfo_50[];								/*系统状态信息数组*/
extern u16 gPLimCPInfo_51[];								/*电流功率限值数组*/
extern u16 gPLimVInfo_52[];									/*电压限值信息数组*/
extern u16 gPAlarmInfo_53[];								/*告警信息数组*/
extern u16 gPFaultInfo_54[]; 								/*故障信息数组*/
extern u16 gPNotesInfo_55[];								/*提示信息数组*/
extern u16 gPGroupState_57[]; 								/*电池组状态信息数组*/

extern s16 gPSysMainInfo_60[];								/*系统整体信息数组*/
extern u16 gPSysCapInfo_61[];								/*系统电量信息数组*/
extern u16 gPVoltCharInfo_62[]; 							/*电压特征信息(最大值)数组*/
extern u16 gPVoltCharInfo_63[];								/*电压特征信息(最小值)数组*/
extern s16 gPTempCharInfo_64[]; 							/*温度特征信息(最大值)数组*/
extern s16 gPTempCharInfo_65[];								/*温度特征信息(最小值)数组*/
extern u16 gPSumVCharInfo_66[];								/*总压特征信息数组*/
extern s16 gPCurrCharInfo_67[];								/*电流特征信息数组*/

extern u16 gPStatistTime_70[]; 								/*时间统计信息数组*/
extern u16 gPStatistErr_71[]; 								/*故障统计信息数组*/
extern u16 gPStatistCapEner_72[];							/*当前能量统计信息数组*/
extern u32 gPStatistHisCap_73[];							/*累计电量统计信息数组*/
extern u32 gPStatistHisEner_74[];							/*累计电能统计信息数组*/
extern u16 gPStatistDhgDeep_75[];							/*放电深度统计信息数组*/

extern u16 gPAuthorInfo_80[]; 								/*权限级别状态信息数组*/
extern u16 gPDebugResult_82[];								/*调试结果状态信息数组*/

extern u16 gPAuthorCtrl_85[];								/*权限密码登录信息数组*/
extern u16 gPSysCmdCtrl_86[];								/*系统控制命令信息数组*/
extern u16 gPDebugCtrl_88[];								/*系统调试命令信息数组*/

/*单组无堆控时使用主控信息*/
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

extern u16 gSysGenParaRO_100[];								/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];							  	/*堆控通用参数值*/
extern u16 gPSmuGenParaWR_107[];						  	/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u16* SciGetPtrPileHisErrRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆历史故障信息寄存器地址指针
** @Function  : 获取读电池堆历史故障信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileHisErrRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		/*取堆控信息*/
		//if(index < PINDEX45_LEN)
		//{
			/*读取电池堆历史故障记录*/
			//return(&gPHisErrInfo_45[index]);
		//}

		/*取主控信息*/
		if(index < GINDEX45_LEN)
		{
			/*读取电池组历史故障记录*/
			return(&gGHisErrInfo_45[index]);
		}
	}
	/*单组无堆控*/
	else
	{
		/*取主控信息*/
		if(index < GINDEX45_LEN)
		{
			/*读取电池组历史故障记录*/
			return(&gGHisErrInfo_45[index]);
		}
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileHisEventRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆历史事件态信息寄存器地址指针
** @Function  : 获取读电池堆历史事件信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileHisEventRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		/*取堆控信息*/
		//if(index < PINDEX46_LEN)
		//{
			/*读取电池堆历史故障记录*/
			//return(&gPHisEventInfo_46[index]);
		//}

		/*取主控信息*/
		if(index < GINDEX46_LEN)
		{
			/*读取电池组历史故障记录*/
			return(&gGHisEventInfo_46[index]);
		}
	}
	/*单组无堆控*/
	else
	{
		/*取主控信息*/
		if(index < GINDEX46_LEN)
		{
			/*读取电池组历史故障记录*/
			return(&gGHisEventInfo_46[index]);
		}
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileStateInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆状态信息寄存器地址指针
** @Function  : 获取读电池堆状态信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileStateInfoRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX50_LEN)
		{
			/*读取电池堆信息*/
			return(&gPSysStateInfo_50[index]);
		}
	}
	/*单组无堆控*/
	else
	{
		if(index < GINDEX50_LEN)
		{
			/*读取电池组信息*/
			return(&gGSysStateInfo_50[index]);
		}
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileLimitInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆限值信息寄存器地址指针
** @Function  : 获取读电池堆限值信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileLimitInfoRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX51_LEN)
		{
			/*读取电池堆电流功率限值信息*/
			return(&gPLimCPInfo_51[index]);
		}
		else if(index < (PINDEX51_LEN + PINDEX52_LEN))
		{
			/*读取电池堆电压限值信息*/
			return(&gPLimVInfo_52[index - PINDEX51_LEN]);
		}
	}
	/*单组无堆控*/
	else
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
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileErrInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆故障告警信息寄存器地址指针
** @Function  : 获取读电池堆故障告警信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileErrInfoRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX53_LEN)
		{
			/*读取电池堆告警信息*/
			return(&gPAlarmInfo_53[index]);
		}
		else if(index < (PINDEX53_LEN + PINDEX54_LEN))
		{
			/*读取电池堆故障信息*/
			return(&gPFaultInfo_54[index - PINDEX53_LEN]);
		}
		else if(index < (PINDEX53_LEN + PINDEX54_LEN + PINDEX55_LEN ))
		{
			/*读取电池堆提示信息*/
			return(&gPNotesInfo_55[index - PINDEX53_LEN - PINDEX54_LEN]);
		}
	}
	/*单组无堆控*/
	else
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
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileIoStateInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆输入输出口状态信息寄存器地址指针
** @Function  : 获取读电池堆输入输出口状态信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileIoStateInfoRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX56_LEN)
		{
			/*读取电池堆端口信息*/
			//return(&gPPortStateInfo_56[index]);
		}
		else if(index < (PINDEX56_LEN + PINDEX57_LEN))
		{
			/*读取电池堆各组状态信息*/
			return(&gPGroupState_57[index - PINDEX56_LEN]);
		}
	}
	/*单组无堆控*/
	else
	{
		if(index < GINDEX56_LEN)
		{
			/*读取电池组端口信息*/
			return(&gGPortStateInfo_56[index]);
		}
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileMainInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆主要数据信息寄存器地址指针
** @Function  : 获取读电池堆主要数据信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileMainInfoRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX60_LEN)
		{
			/*读取电池堆主要数据信息*/
			return((u16*)&gPSysMainInfo_60[index]);
		}
	}
	/*单组无堆控*/
	else
	{
		if(index < GINDEX60_LEN)
		{
			/*读取电池组主要数据信息*/
			return((u16*)&gGSysMainInfo_60[index]);
		}
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileCapInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆电量状态信息寄存器地址指针
** @Function  : 获取读电池堆电量状态息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileCapInfoRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX61_LEN)
		{
			/*读取电池堆电量状态信息*/
			return(&gPSysCapInfo_61[index]);
		}
	}
	/*单组无堆控*/
	else
	{
		if(index < GINDEX61_LEN)
		{
			/*读取电池组电量状态信息*/
			return(&gGSysCapInfo_61[index]);
		}
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileCellCharaInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆单体特征信息寄存器地址指针
** @Function  : 获取读电池堆单体特征总体寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileCellCharaInfoRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX62_LEN)
		{
			/*读取电池堆单体电压最大值特征信息*/
			return(&gPVoltCharInfo_62[index]);
		}
		else if(index < (PINDEX62_LEN + PINDEX63_LEN))
		{
			/*读取电池堆单体电压最小值特征信息*/
			return(&gPVoltCharInfo_63[index - PINDEX62_LEN]);
		}
		else if(index < (PINDEX62_LEN + PINDEX63_LEN + PINDEX64_LEN))
		{
			/*读取电池堆单体温度最大值特征信息*/
			return((u16*)&gPTempCharInfo_64[index - PINDEX62_LEN - PINDEX63_LEN]);
		}
		else if(index < (PINDEX62_LEN + PINDEX63_LEN + PINDEX64_LEN + PINDEX65_LEN))
		{
			/*读取电池堆单体温度最小值特征信息*/
			return((u16*)&gPTempCharInfo_65[index - PINDEX62_LEN - PINDEX63_LEN - PINDEX64_LEN]);
		}
	}
	/*单组无堆控*/
	else
	{
		if(0 == index)
		{
			/*读取电池组单体电压最大值组号信息*/
			return(SciGetPtrDefaultValueRO());
		}
		else if(1 == index)
		{
			/*读取电池组单体电压最大值电池号信息*/
			return(&gGVoltCharInfo_63[eVoltCharInfo63_MaxVNum]);
		}
		else if(2 == index)
		{
			/*读取电池组单体电压最大值信息*/
			return(&gGVoltCharInfo_63[eVoltCharInfo63_MaxVolt]);
		}
		else if(3 == index)
		{
			/*读取电池组总电压平均值信息*/
			return(&gGSysCellInfo_62[eSysCellInfo62_SumV]);
		}
		else if(4 == index)
		{
			/*读取电池组单体电压最小值组号信息*/
			return(SciGetPtrDefaultValueRO());
		}
		else if(5 == index)
		{
			/*读取电池组单体电压最小值电池号信息*/
			return(&gGVoltCharInfo_63[eVoltCharInfo63_MinVNum]);
		}
		else if(6 == index)
		{
			/*读取电池组单体电压最小值信息*/
			return(&gGVoltCharInfo_63[eVoltCharInfo63_MinVolt]);
		}
		else if(7 == index)
		{
			/*读取电池组单体电压平均值信息*/
			return(&gGSysCellInfo_62[eSysCellInfo62_AvgV]);
		}
		else if(8 == index)
		{
			/*读取电池组单体温度最大值组号信息*/
			return(SciGetPtrDefaultValueRO());
		}
		else if(9 == index)
		{
			/*读取电池组单体温度最大值电池号信息*/
			return((u16*)&gGTempCharInfo_64[eTempCharInfo64_MaxTNum]);
		}
		else if(10 == index)
		{
			/*读取电池组单体温度最大值信息*/
			return((u16*)&gGTempCharInfo_64[eTempCharInfo64_MaxTemp]);
		}
		else if(11 == index)
		{
			/*读取电池组单体平均温度值信息*/
			return(&gGSysCellInfo_62[eSysCellInfo62_AvgT]);
		}
		else if(12 == index)
		{
			/*读取电池组单体温度最小值组号信息*/
			return(SciGetPtrDefaultValueRO());
		}
		else if(13 == index)
		{
			/*读取电池组单体温度最小值电池号信息*/
			return((u16*)&gGTempCharInfo_64[eTempCharInfo64_MinTNum]);
		}
		else if(14 == index)
		{
			/*读取电池组单体温度最小值信息*/
			return((u16*)&gGTempCharInfo_64[eTempCharInfo64_MinTemp]);
		}
		else if(15 == index)
		{
			/*读取电池组环境温度值信息*/
			return(&gGSysCellInfo_62[eSysCellInfo62_EnvT]);
		}
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileSumVCCharaInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆总压电流特征信息寄存器地址指针
** @Function  : 获取读电池堆总压电流特征信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileSumVCCharaInfoRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX66_LEN)
		{
			/*读取电池堆总压特征信息*/
			return(&gPSumVCharInfo_66[index]);
		}
		else if(index < (PINDEX66_LEN + PINDEX67_LEN))
		{
			/*读取电池堆电流特征信息*/
			return((u16*)&gPCurrCharInfo_67[index - PINDEX66_LEN]);
		}
	}
	/*单组无堆控*/
	else
	{
		if((1 == index) || (3 == index))
		{
			/*读取电池组总压最大最小值信息*/
			return((u16*)&gGSysMainInfo_60[eSysMainInfo60_SumV]);
		}
		else if((5 == index) || (7 == index))
		{
			/*读取电池组电流最大最小值信息*/
			return((u16*)&gGSysMainInfo_60[eSysMainInfo60_OutC]);
		}
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileStatistInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读电池堆统计信息寄存器地址指针
** @Function  : 获取读电池堆统计信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileStatistInfoRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX70_LEN)
		{
			/*读取电池堆时间统计信息*/
			return(&gPStatistTime_70[index]);
		}
		else if(index < (PINDEX70_LEN + PINDEX71_LEN))
		{
			/*读取电池堆故障统计信息*/
			return(&gPStatistErr_71[index - PINDEX70_LEN]);
		}
		else if(index < (PINDEX70_LEN + PINDEX71_LEN + PINDEX72_LEN))
		{
			/*读取电池堆单次电量统计信息*/
			return(&gPStatistCapEner_72[index - PINDEX70_LEN - PINDEX71_LEN]);
		}
	}
	/*单组无堆控*/
	else
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
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileHisCapEnerInfoRO(u8 sciNum, u8 index)
** @Input     : sciNum:端口号 index:索引号
** @Output    : 读电池堆累计电能电量信息寄存器地址指针
** @Function  : 获取读电池堆累计电能电量信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileHisCapEnerInfoRO(u8 sciNum, u8 index)
{
	u8 id = 0;

	id = index / 2;

	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(id < PINDEX73_LEN)
		{
			/*串口读*/
			if(sciNum < eSCINUM)
			{
				/*读取电池堆累计电量信息*/
				sSciROPileDataL[sciNum] = gPStatistHisCap_73[id] & 0xffff;
				sSciROPileDataH[sciNum] = (gPStatistHisCap_73[id] >> 16) & 0xffff;

				/*读高位*/
				if(0 == (index % 2))
				{
					return(&sSciROPileDataH[sciNum]);
				}
				/*读低位*/
				else
				{
					return(&sSciROPileDataL[sciNum]);
				}
			}
			/*网口读*/
			else
			{
				/*读取电池堆累计电量信息*/
				sSciROPileDataL[eSCINUM] = gPStatistHisCap_73[id] & 0xffff;
				sSciROPileDataH[eSCINUM] = (gPStatistHisCap_73[id] >> 16) & 0xffff;

				/*读高位*/
				if(0 == (index % 2))
				{
					return(&sSciROPileDataH[eSCINUM]);
				}
				/*读低位*/
				else
				{
					return(&sSciROPileDataL[eSCINUM]);
				}
			}
		}
		else if(id < (PINDEX73_LEN + PINDEX74_LEN))
		{
			/*串口读*/
			if(sciNum < eSCINUM)
			{
				/*读取电池堆累计电能息*/
				sSciROPileDataL[sciNum] = gPStatistHisEner_74[id - GINDEX73_LEN] & 0xffff;
				sSciROPileDataH[sciNum] = (gPStatistHisEner_74[id - GINDEX73_LEN] >> 16) & 0xffff;

				/*读高位*/
				if(0 == (index % 2))
				{
					return(&sSciROPileDataH[sciNum]);
				}
				/*读低位*/
				else
				{
					return(&sSciROPileDataL[sciNum]);
				}
			}
			/*网口读*/
			else
			{
				/*读取电池堆累计电能息*/
				sSciROPileDataL[eSCINUM] = gPStatistHisEner_74[id - GINDEX73_LEN] & 0xffff;
				sSciROPileDataH[eSCINUM] = (gPStatistHisEner_74[id - GINDEX73_LEN] >> 16) & 0xffff;

				/*读高位*/
				if(0 == (index % 2))
				{
					return(&sSciROPileDataH[eSCINUM]);
				}
				/*读低位*/
				else
				{
					return(&sSciROPileDataL[eSCINUM]);
				}
			}
		}
	}
	/*单组无堆控*/
	else
	{
		if(id < GINDEX73_LEN)
		{
			/*串口读*/
			if(sciNum < eSCINUM)
			{
				/*读取电池组累计电量信息*/
				sSciROPileDataL[sciNum] = gGStatistHisCap_73[id] & 0xffff;
				sSciROPileDataH[sciNum] = (gGStatistHisCap_73[id] >> 16) & 0xffff;

				/*读高位*/
				if(0 == (index % 2))
				{
					return(&sSciROPileDataH[sciNum]);
				}
				/*读低位*/
				else
				{
					return(&sSciROPileDataL[sciNum]);
				}
			}
			/*网口读*/
			else
			{
				/*读取电池组累计电量信息*/
				sSciROPileDataL[eSCINUM] = gGStatistHisCap_73[id] & 0xffff;
				sSciROPileDataH[eSCINUM] = (gGStatistHisCap_73[id] >> 16) & 0xffff;

				/*读高位*/
				if(0 == (index % 2))
				{
					return(&sSciROPileDataH[eSCINUM]);
				}
				/*读低位*/
				else
				{
					return(&sSciROPileDataL[eSCINUM]);
				}
			}
		}
		else if(id < (GINDEX73_LEN + GINDEX74_LEN))
		{
			/*串口读*/
			if(sciNum < eSCINUM)
			{
				/*读取电池组累计电能信息*/
				sSciROPileDataL[sciNum] = gGStatistHisEner_74[id - GINDEX73_LEN] & 0xffff;
				sSciROPileDataH[sciNum] = (gGStatistHisEner_74[id - GINDEX73_LEN] >> 16) & 0xffff;

				/*读高位*/
				if(0 == (index % 2))
				{
					return(&sSciROPileDataH[sciNum]);
				}
				/*读低位*/
				else
				{
					return(&sSciROPileDataL[sciNum]);
				}
			}
			/*网口读*/
			else
			{
				/*读取电池组累计电能信息*/
				sSciROPileDataL[eSCINUM] = gGStatistHisEner_74[id - GINDEX73_LEN] & 0xffff;
				sSciROPileDataH[eSCINUM] = (gGStatistHisEner_74[id - GINDEX73_LEN] >> 16) & 0xffff;

				/*读高位*/
				if(0 == (index % 2))
				{
					return(&sSciROPileDataH[eSCINUM]);
				}
				/*读低位*/
				else
				{
					return(&sSciROPileDataL[eSCINUM]);
				}
			}
		}
	}

	return(SciGetPtrDefaultValueRO());
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
