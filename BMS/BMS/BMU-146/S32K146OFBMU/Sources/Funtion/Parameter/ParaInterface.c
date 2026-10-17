/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ParaInterface.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 参数存储模块用户处理接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "CANUser.h"
#include "GroupPara.h"
#include "PublicPara.h"
#include "ParaRead.h"
#include "ParaWrite.h"
#include "ParaInterface.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u8 gCANParCfgStat_20[];                              /*通信包参数批量配置状态信息*/
extern u8 gCANParCfgRInfo_21[];                             /*通信包参数批量配置读取信息*/
extern u8 gCANParCfgWInfo_22[];                             /*通信包参数批量配置写入信息*/

extern u16 gSysGenParaRO_100[];  							/*系统通用参数值*/
extern u16 gSysHigParaRO_101[];  							/*系统硬件参数值*/
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
** @Name      : void ParaStoreParaInfoInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组存储的参数初始化
** @The notes : 读取先后顺序:外部EEPROM->内部EEPROM->内部FLASH
**===============================================================================================*/
void ParaStoreParaInfoInit(void)
{
	u8 i = 0;
	u8 para8[GINDEX104_LEN] = {0};
	u16 para16[GINDEX102_LEN] = {0};

	/*初始化系统通用参数*/
	if(TRUE == ParaReadStoreSystemGenParaInfo(para16, SINDEX100_LEN))
	{
		for(i = 0; i < SINDEX100_LEN; i++)
		{
			gSysGenParaRO_100[i] = para16[i];
		}
	}

	/*初始化系统硬件参数*/
	if(TRUE == ParaReadStoreSystemHWParaInfo(para16, SINDEX101_LEN))
	{
		for(i = 0; i < SINDEX101_LEN; i++)
		{
			gSysHigParaRO_101[i] = para16[i];
		}
	}

	/*初始化主控通用参数*/
	if(TRUE == ParaReadStoreGroupGenParaInfo(para16, GINDEX102_LEN))
	{
		for(i = 0; i < GINDEX102_LEN; i++)
		{
			gGBmuGenPara_102[i] = para16[i];
		}
	}

	/*初始化主控高级参数*/
	if(TRUE == ParaReadStoreGroupHigParaInfo(para16, GINDEX103_LEN))
	{
		for(i = 0; i < GINDEX103_LEN; i++)
		{
			gGBmuHigLevPara_103[i] = para16[i];
		}
	}

	/*初始化主控硬件参数*/
	if(TRUE == ParaReadStoreGroupHWParaInfo(para8, GINDEX104_LEN))
	{
		for(i = 0; i < GINDEX104_LEN; i++)
		{
			gGHardPara_104[i] = para8[i];
		}
	}
}

/*=================================================================================================
** @Name      : u8 ParaCheckExtEepReadyHook(void)
** @Input     : void
** @Output    : TRUE:就绪 FALSE:异常
** @Function  : 检测外部EEPROM正常就绪状态
** @The notes : 用户必须补充完善
**===============================================================================================*/
u8 ParaCheckExtEepReadyHook(void)
{
    /*外部EEPROM读写正常*/
	if(1 == DEVEXTEEPGetReadyFlag())
	{
        return(TRUE);
	}

	return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ParaGetStateChangReqCopyHook(void)
** @Input     : void
** @Output    : TRUE:请求备份 FALSE:无请求
** @Function  : 获取状态变换请求参数备份状态
** @The notes : 用户必须补充完善
**===============================================================================================*/
u8 ParaGetStateChangReqCopyHook(void)
{
    static u16 sHisRunState = 0;
    static u16 sHisChgState = 0;

    /*电池组工作状态或充放电状态变换(当前非上下电状态)*/
    if((eWORK_INIT != GetGWorkStateAPI())
		&& (eWORK_START != GetGWorkStateAPI())
		&& (eWORK_STOP != GetGWorkStateAPI())
		&& ((sHisRunState != GetGWorkStateAPI())
    	|| (sHisChgState != GetGChgDhgStateAPI())))
    {
        /*更新状态*/
        sHisRunState = GetGWorkStateAPI();
        sHisChgState = GetGChgDhgStateAPI();

        /*请求备份*/
    	return(TRUE);
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 *ParaGetDnloadStateInfoPtrHook(void)
** @Input     : void
** @Output    : 变量地址指针
** @Function  : 获取参数批量下载状态信息地址指针
** @The notes : 支持参数批量下载时用户必须补充完善
**===============================================================================================*/
u8 *ParaGetDnloadStateInfoPtrHook(void)
{
    /*CAN通信包信息地址指针*/
    return(gCANParCfgStat_20);
}

/*=================================================================================================
** @Name      : u8 *ParaGetDnloadReadInfoPtrHook(void)
** @Input     : void
** @Output    : 变量地址指针
** @Function  : 获取参数配置读取信息地址指针
** @The notes : 支持参数批量下载时用户必须补充完善
**===============================================================================================*/
u8 *ParaGetDnloadReadInfoPtrHook(void)
{
    /*CAN通信包信息地址指针*/
    return(gCANParCfgRInfo_21);
}

/*=================================================================================================
** @Name      : u8 *ParaGetDnloadWriteInfoPtrHook(void)
** @Input     : void
** @Output    : 变量地址指针
** @Function  : 获取参数配置写入信息地址指针
** @The notes : 支持参数批量下载时用户必须补充完善
**===============================================================================================*/
u8 *ParaGetDnloadWriteInfoPtrHook(void)
{
    /*CAN通信包信息地址指针*/
    return(gCANParCfgWInfo_22);
}

/*=================================================================================================
** @Name      : u8 ParaWriteParaUserHandleHook(u16 addr, u8 *para, u8 len)
** @Input     : addr:参数EEPROM地址 para:参数值 len:长度
** @Output    : TRUE:写成功 FALSE:写失败
** @Function  : 参数写入存储区用户处理
** @The notes : 支持参数批量下载时用户必须补充完善
**===============================================================================================*/
u8 ParaWriteParaUserHandleHook(u16 addr, u8 *para, u8 len)
{
	u8 i = 0;
	u8 num = 0;
	u8 index = 0;
	u8 result = TRUE;
	u16 data16 = 0;
	u32 data32 = 0;

    /*参数无效*/
	if((para == NULL) || (0 == len))
	{
		return(FALSE);
	}

    /*根据地址写参数*/
    /*设备序列号(u8)*/
	if((addr >= PARA_DEV_SERIAL_ADDR) && ((addr + len) <= (PARA_DEV_SERIAL_ADDR + PARA_DEV_SERIAL_LEN)))
	{
		index = addr - PARA_DEV_SERIAL_ADDR;
		result = ParaWriteStoreDevSerial(index, para, len);
	}
    /*系统硬件参数(u16)*/
	else if((addr >= PARA_SYSHW_PARA_ADDR) && (len >= 2) && ((addr + len) <= (PARA_SYSHW_PARA_ADDR + PARA_SYSHW_PARA_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_SYSHW_PARA_ADDR) / 2) + i;
			result &= ParaWriteStoreSystemHWParaInfo(index, &data16, 1);
		}
	}
    /*电流零飘修正值参数(u16)*/
	else if((addr >= PARA_ZERO_C_CORR_ADDR) && (len >= 2) && ((addr + len) <= (PARA_ZERO_C_CORR_ADDR + PARA_ZERO_C_CORR_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_ZERO_C_CORR_ADDR) / 2) + i;
			result &= ParaWriteStoreCorrZeroCurr(index, &data16, 1);
		}
	}
    /*霍尔充电电流校准KB值参数(u32)*/
	else if((addr >= PARA_HCHGC_CORR_KB_ADDR) && (len >= 4) && ((addr + len) <= (PARA_HCHGC_CORR_KB_ADDR + PARA_HCHGC_CORR_KB_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_HCHGC_CORR_KB_ADDR) / 4) + i;
			result &= ParaWriteStoreHalChgCurrCorrKB(index, &data32, 1);
		}
	}
    /*霍尔放电电流校准KB值参数(u32)*/
	else if((addr >= PARA_HDHGC_CORR_KB_ADDR) && (len >= 4) && ((addr + len) <= (PARA_HDHGC_CORR_KB_ADDR + PARA_HDHGC_CORR_KB_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_HDHGC_CORR_KB_ADDR) / 4) + i;
			result &= ParaWriteStoreHalDhgCurrCorrKB(index, &data32, 1);
		}
	}
    /*分流器充电电流校准KB值参数(u32)*/
	else if((addr >= PARA_SCHGC_CORR_KB_ADDR) && (len >= 4) && ((addr + len) <= (PARA_SCHGC_CORR_KB_ADDR + PARA_SCHGC_CORR_KB_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_SCHGC_CORR_KB_ADDR) / 4) + i;
			result &= ParaWriteStoreShuChgCurrCorrKB(index, &data32, 1);
		}
	}
    /*分流器放电电流校准KB值参数(u32)*/
	else if((addr >= PARA_SDHGC_CORR_KB_ADDR) && (len >= 4) && ((addr + len) <= (PARA_SDHGC_CORR_KB_ADDR + PARA_SDHGC_CORR_KB_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_SDHGC_CORR_KB_ADDR) / 4) + i;
			result &= ParaWriteStoreShuDhgCurrCorrKB(index, &data32, 1);
		}
	}
    /*电池总电压校准KB值参数(u32)*/
	else if((addr >= PARA_SUMV_CORR_KB_ADDR) && (len >= 4) && ((addr + len) <= (PARA_SUMV_CORR_KB_ADDR + PARA_SUMV_CORR_KB_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_SUMV_CORR_KB_ADDR) / 4) + i;
			result &= ParaWriteStoreSumVoltCorrKB(index, &data32, 1);
		}
	}
    /*预充电压校准KB值参数(u32)*/
	else if((addr >= PARA_PREV_CORR_KB_ADDR) && (len >= 4) && ((addr + len) <= (PARA_PREV_CORR_KB_ADDR + PARA_PREV_CORR_KB_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_PREV_CORR_KB_ADDR) / 4) + i;
			result &= ParaWriteStorePreVoltCorrKB(index, &data32, 1);
		}
	}
    /*负载电压校准KB值参数(u32)*/
	else if((addr >= PARA_LOAV_CORR_KB_ADDR) && (len >= 4) && ((addr + len) <= (PARA_LOAV_CORR_KB_ADDR + PARA_LOAV_CORR_KB_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_LOAV_CORR_KB_ADDR) / 4) + i;
			result &= ParaWriteStoreLoaVoltCorrKB(index, &data32, 1);
		}
	}
    /*Link+电压校准KB值参数(u32)*/
	else if((addr >= PARA_POSV_CORR_KB_ADDR) && (len >= 4) && ((addr + len) <= (PARA_POSV_CORR_KB_ADDR + PARA_POSV_CORR_KB_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_POSV_CORR_KB_ADDR) / 4) + i;
			result &= ParaWriteStorePosVoltCorrKB(index, &data32, 1);
		}
	}
    /*Link-电压校准KB值参数(u32)*/
	else if((addr >= PARA_NEGV_CORR_KB_ADDR) && (len >= 4) && ((addr + len) <= (PARA_NEGV_CORR_KB_ADDR + PARA_NEGV_CORR_KB_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_NEGV_CORR_KB_ADDR) / 4) + i;
			result &= ParaWriteStoreNegVoltCorrKB(index, &data32, 1);
		}
	}
    /*系统时钟(北京时间)参数(u16)*/
	else if((addr >= PARA_SYS_CLOCK_ADDR) && (len >= 2) && ((addr + len) <= (PARA_SYS_CLOCK_ADDR + PARA_SYS_CLOCK_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_SYS_CLOCK_ADDR) / 2) + i;
			result &= ParaWriteStoreSysClock(index, &data16, 1);
		}
	}
    /*月故障统计时间(年月)参数(u16)*/
	else if((addr >= PARA_MONERR_TIME_ADDR) && (len >= 2) && ((addr + len) <= (PARA_MONERR_TIME_ADDR + PARA_MONERR_TIME_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_MONERR_TIME_ADDR) / 2) + i;
			result &= ParaWriteStoreMonErrTime(index, &data16, 1);
		}
	}
    /*当前容量SOH值(u16)*/
	else if((addr >= PARA_NOW_CAP_SOH_ADDR) && (len >= 2) && ((addr + len) <= (PARA_NOW_CAP_SOH_ADDR + PARA_NOW_CAP_SOH_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_NOW_CAP_SOH_ADDR) / 2) + i;
			result &= ParaWriteStoreNowCapSoh(index, &data16, 1);
		}
	}
    /*充放电次数检测状态(u32)*/
	else if((addr >= PARA_CHGTIMES_STA_ADDR) && (len >= 4) && ((addr + len) <= (PARA_CHGTIMES_STA_ADDR + PARA_CHGTIMES_STA_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_CHGTIMES_STA_ADDR) / 4) + i;
			result &= ParaWriteStoreChgTimesState(index, &data32, 1);
		}
	}
    /*总容量总能量自动标定信息(u32)*/
	else if((addr >= PARA_CAPENER_CORR_ADDR) && (len >= 4) && ((addr + len) <= (PARA_CAPENER_CORR_ADDR + PARA_CAPENER_CORR_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_CAPENER_CORR_ADDR) / 4) + i;
			result &= ParaWriteStoreCapEnerCorrInfo(index, &data32, 1);
		}
	}
    /*电量计算结构信息(u32)*/
	else if((addr >= PARA_CAP_FORM_ADDR) && (len >= 4) && ((addr + len) <= (PARA_CAP_FORM_ADDR + PARA_CAP_FORM_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_CAP_FORM_ADDR) / 4) + i;
			result &= ParaWriteStoreCapFormInfo(index, &data32, 1);
		}
	}
    /*电能计算结构信息(u32)*/
	else if((addr >= PARA_ENER_FORM_ADDR) && (len >= 4) && ((addr + len) <= (PARA_ENER_FORM_ADDR + PARA_ENER_FORM_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_ENER_FORM_ADDR) / 4) + i;
			result &= ParaWriteStoreEnerFormInfo(index, &data32, 1);
		}
	}
    /*系统运行(上电)时长 1Hour(u16)*/
	else if((addr >= PARA_SYS_RUNTIME_ADDR) && (len >= 2) && ((addr + len) <= (PARA_SYS_RUNTIME_ADDR + PARA_SYS_RUNTIME_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_SYS_RUNTIME_ADDR) / 2) + i;
			result &= ParaWriteStoreSysRunTime(index, &data16, 1);
		}
	}
    /*充放电次数(u16)*/
	else if((addr >= PARA_CHG_TIMES_ADDR) && (len >= 2) && ((addr + len) <= (PARA_CHG_TIMES_ADDR + PARA_CHG_TIMES_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_CHG_TIMES_ADDR) / 2) + i;
			result &= ParaWriteStoreChgDhgTimes(index, &data16, 1);
		}
	}
    /*历史故障告警数目(u16)*/
	else if((addr >= PARA_HIS_ERRNUM_ADDR) && (len >= 2) && ((addr + len) <= (PARA_HIS_ERRNUM_ADDR + PARA_HIS_ERRNUM_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_HIS_ERRNUM_ADDR) / 2) + i;
			result &= ParaWriteStoreHisErrNum(index, &data16, 1);
		}
	}
    /*放电深度统计信息(u16)*/
	else if((addr >= PARA_DOD_INFO_ADDR) && (len >= 2) && ((addr + len) <= (PARA_DOD_INFO_ADDR + PARA_DOD_INFO_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_DOD_INFO_ADDR) / 2) + i;
			result &= ParaWriteStoreDODStaInfo(index, &data16, 1);
		}
	}
    /*安全状态SOS锁定信息(u16)*/
	else if((addr >= PARA_SOS_INFO_ADDR) && (len >= 2) && ((addr + len) <= (PARA_SOS_INFO_ADDR + PARA_SOS_INFO_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_SOS_INFO_ADDR) / 2) + i;
			result &= ParaWriteStoreSOSLockInfo(index, &data16, 1);
		}
	}
    /*累计充放电电量 1AH(u32)*/
	else if((addr >= PARA_HIS_CHGCAP_ADDR) && (len >= 4) && ((addr + len) <= (PARA_HIS_CHGCAP_ADDR + PARA_HIS_CHGCAP_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_HIS_CHGCAP_ADDR) / 4) + i;
			result &= ParaWriteStoreHisChgDhgCap(index, &data32, 1);
		}
	}
    /*累计充放电电能 0.1kWh(u32)*/
	else if((addr >= PARA_HIS_CHGENER_ADDR) && (len >= 4) && ((addr + len) <= (PARA_HIS_CHGENER_ADDR + PARA_HIS_CHGENER_LEN * 4)))
	{
		num = len / 4;
		for(i = 0; i < num; i++)
		{
			data32 = ((u32)para[3 + (4 * i)] << 24) | ((u32)para[2 + (4 * i)] << 16) | ((u32)para[1 + (4 * i)] << 8) | para[(4 * i)];
			index = ((addr - PARA_HIS_CHGENER_ADDR) / 4) + i;
			result &= ParaWriteStoreHisChgDhgEner(index, &data32, 1);
		}
	}
    /*系统公共通用参数信息参数(u16)*/
	else if((addr >= PARA_SYSGEN_PARA_ADDR) && (len >= 2) && ((addr + len) <= (PARA_SYSGEN_PARA_ADDR + PARA_SYSGEN_PARA_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_SYSGEN_PARA_ADDR) / 2) + i;
			result &= ParaWriteStoreSystemGenParaInfo(index, &data16, 1);
		}
	}
    /*电池组主控通用参数信息参数(u16)*/
	else if((addr >= PARA_DEVGEN_PARA_ADDR) && (len >= 2) && ((addr + len) <= (PARA_DEVGEN_PARA_ADDR + PARA_DEVGEN_PARA_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_DEVGEN_PARA_ADDR) / 2) + i;
			result &= ParaWriteStoreGroupGenParaInfo(index, &data16, 1);
		}
	}
    /*电池组主控高级参数信息参数(u16)*/
	else if((addr >= PARA_DEVHIG_PARA_ADDR) && (len >= 2) && ((addr + len) <= (PARA_DEVHIG_PARA_ADDR + PARA_DEVHIG_PARA_LEN * 2)))
	{
		num = len / 2;
		for(i = 0; i < num; i++)
		{
			data16 = ((u16)para[1 + (2 * i)] << 8) | para[(2 * i)];
			index = ((addr - PARA_DEVHIG_PARA_ADDR) / 2) + i;
			result &= ParaWriteStoreGroupHigParaInfo(index, &data16, 1);
		}
	}
    /*电池组主控硬件参数信息(u8)*/
	else if((addr >= PARA_DEVHW_PARA_ADDR) && ((addr + len) <= (PARA_DEVHW_PARA_ADDR + PARA_DEVHW_PARA_LEN)))
	{
		index = addr - PARA_DEVHW_PARA_ADDR;
		result &= ParaWriteStoreGroupHWParaInfo(index, para, len);
	}
    /*未定义地址*/
	else
	{
		result = FALSE;
	}

    return(result);
}

/*=================================================================================================
** @Name      : u8 ParaRefactoryUserHandleHook(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 参数存储区恢复出厂设置值用户处理
** @The notes : 擦除 各段参数配置标志
**===============================================================================================*/
u8 ParaRefactoryUserHandleHook(void)
{
	u8 result = TRUE;
	u8 default8 = 0xff;
	u16 default16 = 0xffff;
	u32 default32 = 0xffffffff;

    /*根据地址写参数*/
    /*设备序列号(u8)*/
	/*result &= ParaWriteStoreDevSerial(0, &default8, 1);*/

	/*系统硬件参数(u16)*/
	result &= ParaWriteStoreSystemHWParaInfo(0, &default16, 1);

    /*电流零飘修正值参数(u16)*/
	/*result &= ParaWriteStoreCorrZeroCurr(0, &default16, 1);*/
    /*充电电流校准KB值参数(u32)*/
	/*result &= ParaWriteStoreChgCurrCorrKB(0, &default32, 1);*/
    /*放电电流校准KB值参数(u32)*/
	/*result &= ParaWriteStoreDhgCurrCorrKB(0, &default32, 1);*/
    /*电池总电压校准KB值参数(u32)*/
	/*result &= ParaWriteStoreSumVoltCorrKB(0, &default32, 1);*/
    /*预充电压校准KB值参数(u32)*/
	/*result &= ParaWriteStorePreVoltCorrKB(0, &default32, 1);*/
    /*负载电压校准KB值参数(u32)*/
	/*result &= ParaWriteStoreLoaVoltCorrKB(0, &default32, 1);*/
    /*Link+电压校准KB值参数(u32)*/
	/*result &= ParaWriteStorePosVoltCorrKB(0, &default32, 1);*/
    /*Link-电压校准KB值参数(u32)*/
	/*result &= ParaWriteStoreNegVoltCorrKB(0, &default32, 1);*/

    /*系统时钟(北京时间)参数(u16)*/
	result &= ParaWriteStoreSysClock(0, &default16, 1);
    /*月故障统计(年月)参数(u16)*/
	result &= ParaWriteStoreMonErrTime(0, &default16, 1);
    /*当前总容量SOH值(u16)*/
	result &= ParaWriteStoreNowCapSoh(0, &default16, 1);
    /*充放电次数检测状态(u32)*/
	result &= ParaWriteStoreChgTimesState(0, &default32, 1);
    /*总容量总能量自动标定信息(u32)*/
	result &= ParaWriteStoreCapEnerCorrInfo(0, &default32, 1);
    /*电量计算结构信息(u32)*/
	result &= ParaWriteStoreCapFormInfo(0, &default32, 1);
    /*电能计算结构信息(u32)*/
	result &= ParaWriteStoreEnerFormInfo(0, &default32, 1);
    /*系统运行(上电)时长 1Hour(u16)*/
	result &= ParaWriteStoreSysRunTime(0, &default16, 1);
    /*充放电次数(u16)*/
	result &= ParaWriteStoreChgDhgTimes(0, &default16, 1);
    /*历史故障告警数目(u16)*/
	result &= ParaWriteStoreHisErrNum(0, &default16, 1);
    /*放电深度统计(u16)*/
	result &= ParaWriteStoreDODStaInfo(0, &default16, 1);
    /*安全等级SOS锁定(u16)*/
	result &= ParaWriteStoreSOSLockInfo(0, &default16, 1);
    /*累计充放电电量 1AH(u32)*/
	result &= ParaWriteStoreHisChgDhgCap(0, &default32, 1);
    /*累计充放电电能 0.1kWh(u32)*/
	result &= ParaWriteStoreHisChgDhgEner(0, &default32, 1);

    /*系统公共通用参数信息参数(u16)*/
	result &= ParaWriteStoreSystemGenParaInfo(0, &default16, 1);
    /*电池组主控通用参数信息参数(u16)*/
	result &= ParaWriteStoreGroupGenParaInfo(0, &default16, 1);
    /*电池组主控高级参数信息参数(u16)*/
	result &= ParaWriteStoreGroupHigParaInfo(0, &default16, 1);
    /*电池组主控硬件参数信息(u8)*/
	result &= ParaWriteStoreGroupHWParaInfo(0, &default8, 1);

	/*复位软件*/

    return(result);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

