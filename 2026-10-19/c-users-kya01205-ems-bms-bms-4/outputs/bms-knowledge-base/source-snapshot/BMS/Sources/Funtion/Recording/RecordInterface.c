/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : RecordInterface.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 事件/数据记录模块接口
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
#include "LoadApp.h"
#include "RcdDisplayUser.h"
#include "RecordInterface.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u16 sWaveCVHBuff[50] = {0};  /*单体过压录波缓存区*/
extern u16 gGHisWaveInfo_47[];      /*故障录波查询信息数组(共用作电流录波缓存区)*/

extern u16 gCANDevInfo_1[];         /*设备信息*/

/*=======================================用户自定义信息(通信协议)=================================*/
extern u16 gGSysStateInfo_50[];     /*系统状态信息数组*/
extern u16 gGLimCPInfo_51[];        /*电流功率限值数组*/
extern u16 gGLimVInfo_52[];         /*电压限值信息数组*/
extern u16 gGAlarmInfo_53[];        /*告警信息数组*/
extern u16 gGFaultInfo_54[];        /*故障信息数组*/
extern u16 gGPortStateInfo_56[];    /*输入输出口状态信息数组*/

extern s16 gGSysMainInfo_60[];      /*系统整体信息数组*/
extern u16 gGSysCapInfo_61[];       /*系统电量信息数组*/
extern u16 gGSysCellInfo_62[];      /*系统单体信息数组*/
extern u16 gGVoltCharInfo_63[];     /*电压特征信息数组*/
extern s16 gGTempCharInfo_64[];     /*温度特征信息数组*/
extern u16 gGFstLeakInfo_67[];      /*直流电路信息数组*/
extern u16 gGStatistTime_70[];      /*时间统计信息数组*/
extern u16 gGStatistCapEner_72[];   /*当前能量统计信息数组*/
extern u32 gGStatistHisCap_73[];    /*累计电量统计信息数组*/
extern u32 gGStatistHisEner_74[];   /*累计电能统计信息数组*/
extern u16 gGStatistDodInfo_75[];   /*放电深度统计信息数组*/
extern u16 gGStatistSosInfo_76[];   /*安全等级统计信息数组*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 RcdGetFatWavStoreStopReqHook(void)
** @Input     : void
** @Output    : 故障录波停止数据记录请求状态 0:无请求 1:请求
** @Function  : 获取故障录波停止数据记录请求状态
** @The notes : 用户请求需要暂停缓存当前录波数据
**===============================================================================================*/
u8 RcdGetFatWavStoreStopReqHook(void)
{
	/*电流录波缓存区与故障录波显示区共用数组,读取显示时不能增加缓存数据*/
	if(RcdGetWavRcdStorReadFlagAPI() > 0)
	{
		return(1);
	}

	/*用作缓存区时查询信息清零*/
	gGHisWaveInfo_47[0] = 0;
	gGHisWaveInfo_47[1] = 0;

	return(0);
}

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreCVHBuffPtrHook(u8 *len)
** @Input     : *len:指定单体电压过压故障录波数据缓存区长度
** @Output    : 单体电压过压故障录波数据缓存区起始地址指针
** @Function  : 获取单体电压过压故障录波数据缓存区起始地址指针
** @The notes : 用户需要单体过压故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreCVHBuffPtrHook(u8 *len)
{
	/*用户初始化到可用缓存区长度并返回可用缓存区起始首地址*/
	*len = 50;

	return(sWaveCVHBuff);
}

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreCVHBuffPtrHook(u8 *len)
** @Input     : *len:指定单体电压欠压故障录波数据缓存区长度
** @Output    : 单体电压故障录波数据缓存区起始地址指针
** @Function  : 获取单体电压欠压故障录波数据缓存区起始地址指针
** @The notes : 用户需要单体欠压故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreCVLBuffPtrHook(u8 *len)
{
	/*用户初始化到可用缓存区长度并返回可用缓存区起始首地址*/
	*len = 0;

	return(NULL);
}

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreCVHBuffPtrHook(u8 *len)
** @Input     : *len:指定单体温度过温故障录波数据缓存区长度
** @Output    : 单体电压过温故障录波数据缓存区起始地址指针
** @Function  : 获取单体电压过温故障录波数据缓存区起始地址指针
** @The notes : 用户需要单体过温故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreCTHBuffPtrHook(u8 *len)
{
	/*用户初始化到可用缓存区长度并返回可用缓存区起始首地址*/
	*len = 0;

	return(NULL);
}

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreCVHBuffPtrHook(u8 *len)
** @Input     : *len:指定单体温度低温故障录波数据缓存区长度
** @Output    : 单体电压低温故障录波数据缓存区起始地址指针
** @Function  : 获取单体电压低温故障录波数据缓存区起始地址指针
** @The notes : 用户需要单体低温故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreCTLBuffPtrHook(u8 *len)
{
	/*用户初始化到可用缓存区长度并返回可用缓存区起始首地址*/
	*len = 0;

	return(NULL);
}

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreHVBuffPtrHook(u8 *len)
** @Input     : *len:指定总压过欠压故障录波数据缓存区长度
** @Output    : 总压过欠压故障录波数据缓存区起始地址指针
** @Function  : 获取总压过欠压故障录波数据缓存区起始地址指针
** @The notes : 用户需要总压过欠压故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreHVBuffPtrHook(u8 *len)
{
	/*用户初始化到可用缓存区长度并返回可用缓存区起始首地址*/
	*len = 0;

	return(NULL);
}

/*=================================================================================================
** @Name      : 16 *RcdGetFatWavStoreCOBuffPtrHook(u8 *len)
** @Input     : *len:指定充放电过流故障录波数据缓存区长度
** @Output    : 充放电过流故障录波数据缓存区起始地址指针
** @Function  : 获取充放电过流故障录波数据缓存区起始地址指针
** @The notes : 用户需要充放电过流故障录波时完善(必须指向有效缓存区)
**===============================================================================================*/
u16 *RcdGetFatWavStoreCOBuffPtrHook(u8 *len)
{
	/*用户初始化到可用缓存区长度并返回可用缓存区起始首地址*/
	*len = 50;

	return(&gGHisWaveInfo_47[6]);
}

/*=================================================================================================
** @Name      : u8 RcdGetStoreStopRcdStateHook(void)
** @Input     : void
** @Output    : 用户停止记录历史记录状态 0:不停止 1:停止
** @Function  : 获取用户停止记录历史记录状态
** @The notes : 用户请求需要暂停记录；历史记录
**===============================================================================================*/
u8 RcdGetStoreStopRcdStateHook(void)
{
	/*在线升级时读写外部FLASH不能读写记录*/
	if(LoadGetRdWrFlashStateAPI() > 0)
	{
		return(1);
	}

	/*正在启停时不能读写记录操作FLASH*/
	if((eWORK_INIT == GetGWorkStateAPI())
		|| (eWORK_START == GetGWorkStateAPI())
		|| (eWORK_STOP == GetGWorkStateAPI()))
	{
		return(1);
	}

	/*每天0点前1分钟后3分钟有其它操作不能读写记录操作FLASH*/
	if(((23 == GetSysDateClockAPI().hour) && (GetSysDateClockAPI().minute >= 59))
		|| ((0 == GetSysDateClockAPI().hour) && (GetSysDateClockAPI().minute < 3)))
	{
		return(1);
	}

	return(0);
}

/*=================================================================================================
** @Name      : u16 RcdGetGroupMaxVoltHook(void)
** @Input     : void
** @Output    : 电池组单体最大电压 1mV
** @Function  : 获取电池组单体最大电压
** @The notes :
**===============================================================================================*/
u16 RcdGetGroupMaxVoltHook(void)
{
    return(GetGCellMaxVoltAPI());
}

/*=================================================================================================
** @Name      : u16 RcdGetGroupMinVoltHook(void)
** @Input     : void
** @Output    : 电池组单体最小电压 1mV
** @Function  : 获取电池组单体最小电压
** @The notes :
**===============================================================================================*/
u16 RcdGetGroupMinVoltHook(void)
{
    return(GetGCellMinVoltAPI());
}

/*=================================================================================================
** @Name      : s16 RcdGetGroupMinTempHook(void)
** @Input     : void
** @Output    : 电池组单体最大温度 1℃
** @Function  : 获取电池组单体最大温度
** @The notes :
**===============================================================================================*/
s16 RcdGetGroupMaxTempHook(void)
{
    return(GetGCellMaxTempAPI());
}

/*=================================================================================================
** @Name      : s16 RcdGetGroupMinTempHook(void)
** @Input     : void
** @Output    : 电池组单体最小温度 1℃
** @Function  : 获取电池组单体最小温度
** @The notes :
**===============================================================================================*/
s16 RcdGetGroupMinTempHook(void)
{
    return(GetGCellMinTempAPI());
}

/*=================================================================================================
** @Name      : u16 RcdGetGroupSumVoltHook(void)
** @Input     : void
** @Output    : 电池组总电压 0.1V
** @Function  : 获取电池组总电压
** @The notes : 采样总压
**===============================================================================================*/
u16 RcdGetGroupSumVoltHook(void)
{
    return(GetGSampSumVoltAPI());
}

/*=================================================================================================
** @Name      : u16 RcdGetGroupOutCurrHook(void)
** @Input     : void
** @Output    : 电池组总电流 0.1A
** @Function  : 获取电池组总电流
** @The notes :
**===============================================================================================*/
s16 RcdGetGroupOutCurrHook(void)
{
    return(GetGSampOutCurrAPI());
}

/*=================================================================================================
** @Name      : u16 RcdGetGroupWorkStateHook(void)
** @Input     : void
** @Output    : 电池组工作状态[0,6]
** @Function  : 获取电池组当前工作状态
** @The notes : e_WorkState
**===============================================================================================*/
u16 RcdGetGroupWorkStateHook(void)
{
    return(GetGWorkStateAPI());
}

/*=================================================================================================
** @Name      : u16 RcdGetGroupProStateHook(void)
** @Input     : void
** @Output    : 电池组保护状态[0,5]
** @Function  : 获取电池组当前保护状态
** @The notes : e_ProState
**===============================================================================================*/
u16 RcdGetGroupProStateHook(void)
{
    return(GetGProtectStateAPI());
}

/*=================================================================================================
** @Name      : u8 RcdGetGroupDevAddrHook(void)
** @Input     : void
** @Output    : 电池组设备地址
** @Function  : 获取电池组设备地址
** @The notes :
**===============================================================================================*/
u8 RcdGetGroupDevAddrHook(void)
{
    /*电池组设备地址*/
    return(gCANDevInfo_1[eCANDev1_Addr]);
}

/*=================================================================================================
** @Name      : u8 RcdGetGRunDataRcdReqHook(void)
** @Input     : void
** @Output    : 电池组运行数据存储请求状态
** @Function  : 获取电池组运行数据存储请求状态
** @The notes : 定周期请求或状态变化时请求
**===============================================================================================*/
u8 RcdGetGRunDataRcdReqHook(void)
{
	static u32 sHisReqTime = 0;
	static u8 sHisproState = 0;
	static u8 sHisworState = 0;
	static u8 sHischgState = 0;

	/*达到存储周期4min*/
	if((0 == sHisReqTime) || (ABS(GetSysSecondTimeAPI(), sHisReqTime) >= RCD_DATWD_CYCLE_T))
	{
		sHisReqTime = GetSysSecondTimeAPI();
		sHisproState = GetGProtectStateAPI();
		sHisworState = GetGWorkStateAPI();
		sHischgState = GetGChgDhgStateAPI();
		return(TRUE);
	}
	/*状态变化*/
	else if((sHisproState != GetGProtectStateAPI()) || (sHisworState != GetGWorkStateAPI())
		|| ((sHischgState != GetGChgDhgStateAPI()) && (ABS(GetSysSecondTimeAPI(), sHisReqTime) >= 60)))
	{
		sHisReqTime = GetSysSecondTimeAPI();
		sHisproState = GetGProtectStateAPI();
		sHisworState = GetGWorkStateAPI();
		sHischgState = GetGChgDhgStateAPI();
		return(TRUE);
	}
	/*日期变化*/
	else if(((GetSysDateClockAPI().hour >= 23) && (GetSysDateClockAPI().minute >= 59)
		&& (GetSysDateClockAPI().second >= 50) && (ABS(GetSysSecondTimeAPI(), sHisReqTime) >= 11))
		|| ((0 == GetSysDateClockAPI().hour) && (0 == GetSysDateClockAPI().minute)
		&& (ABS(GetSysSecondTimeAPI(), sHisReqTime) >= 40)))
	{
		sHisReqTime = GetSysSecondTimeAPI();
		sHisproState = GetGProtectStateAPI();
		sHisworState = GetGWorkStateAPI();
		sHischgState = GetGChgDhgStateAPI();
		return(TRUE);
	}

	return(FALSE);
}

/*=================================================================================================
** @Name      : u8 RcdGetGroupRunDataHook(u16 *data, u8 len)
** @Input     : data:运行数据 len:数据长度
** @Output    : 成功或失败
** @Function  : 获取运行数据(数组)
** @The notes : 需要存储的数据
**===============================================================================================*/
u8 RcdGetGroupRunDataHook(u16 *data, u8 len)
{
	u8 i = 0;
	u8 index = 0;
	t_SysClock sysClock;
	static u16 sNum = 0;

	if(len < 7)
	{
		return(FALSE);
	}

	/*收集数据存储时间(序号/年/月/日/时/分/秒[0,4])*/
	sysClock = GetSysDateClockAPI();

	sNum++;
	data[0] = sNum;                                          //序号
	data[1] = sysClock.year;                                 //记录时间-年
	data[2] = ((u16)sysClock.month << 8) | sysClock.day;     //记录时间-月日
	data[3] = ((u16)sysClock.hour << 8) | sysClock.minute;   //记录时间-时分
	data[4] = sysClock.second;
	index = 5;

	/*收集需要存储的数据(状态信息[5,8])*/
	//for(i = 0; i < GINDEX50_LEN; i++)
	for(i = 0; i < 4; i++)
	{
		data[index] = gGSysStateInfo_50[i];
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(限值信息[9,12])*/
	//for(i = 0; i < GINDEX51_LEN; i++)
	for(i = 0; i < 4; i++)
	{
		data[index] = gGLimCPInfo_51[i];
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(电压限值[13,14])*/
	//for(i = 0; i < GINDEX52_LEN; i++)
	for(i = 0; i < 2; i++)
	{
		data[index] = gGLimVInfo_52[i];
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(告警信息[15,17])*/
	//for(i = 0; i < GINDEX53_LEN; i++)
	for(i = 0; i < 3; i++)
	{
		data[index] = gGAlarmInfo_53[i];
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(故障信息[18,20])*/
	//for(i = 0; i < GINDEX54_LEN; i++)
	for(i = 0; i < 3; i++)
	{
		data[index] = gGFaultInfo_54[i];
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(IO端口信息[21,22])*/
	//for(i = 0; i < GINDEX56_LEN; i++)
	for(i = 0; i < 2; i++)
	{
		data[index] = gGPortStateInfo_56[i];
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(整体信息[23,26])*/
	//for(i = 0; i < GINDEX60_LEN; i++)
	for(i = 0; i < 4; i++)
	{
		data[index] = gGSysMainInfo_60[i];
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(电量信息[27,30])*/
	//for(i = 0; i < GINDEX61_LEN; i++)
	for(i = 0; i < 4; i++)
	{
		data[index] = gGSysCapInfo_61[i];
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(单体整体信息[31,34])*/
	//for(i = 0; i < GINDEX62_LEN; i++)
	for(i = 0; i < 4; i++)
	{
		data[index] = gGSysCellInfo_62[i];
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(电压最值信息[35,38])*/
	//for(i = 0; i < GINDEX63_LEN; i++)
	for(i = 0; i < 4; i++)
	{
		data[index] = gGVoltCharInfo_63[i];
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(温度最值信息[39,42])*/
	//for(i = 0; i < GINDEX64_LEN; i++)
	for(i = 0; i < 4; i++)
	{
		data[index] = gGTempCharInfo_64[i];
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(绝缘值信息[43,44])*/
	data[index] = gGFstLeakInfo_67[eFstLeakInfo67_PasInsu];
	index++;
	if(index >= len)
	{
		return(TRUE);
	}

	data[index] = gGFstLeakInfo_67[eFstLeakInfo67_NegInsu];
	index++;
	if(index >= len)
	{
		return(TRUE);
	}

	/*收集需要存储的数据(循环次数和总容量信息[45,46])*/
	data[index] = gGStatistTime_70[eTime70_ChgCycle];
	index++;
	if(index >= len)
	{
		return(TRUE);
	}
	data[index] = gGStatistCapEner_72[eCapEner72_AllEner];
	index++;
	if(index >= len)
	{
		return(TRUE);
	}

	/*收集需要存储的数据(累计电量信息[47,50])[高位在前]*/
	//for(i = 0; i < GINDEX73_LEN; i++)
	for(i = 0; i < 2; i++)
	{
		data[index] = (gGStatistHisCap_73[i] >> 16) & 0xffff;
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
		data[index] = gGStatistHisCap_73[i] & 0xffff;
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(累计电能信息[51,54])[高位在前]*/
	//for(i = 0; i < GINDEX74_LEN; i++)
	for(i = 0; i < 2; i++)
	{
		data[index] = (gGStatistHisEner_74[i] >> 16) & 0xffff;
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
		data[index] = gGStatistHisEner_74[i] & 0xffff;
		index++;
		if(index >= len)
		{
			return(TRUE);
		}
	}

	/*收集需要存储的数据(放电深度DOD(低位)和安全等级SOS(高位)信息[55,55])*/
	data[index] = (gGStatistSosInfo_76[1] << 13) | ((gGStatistSosInfo_76[0] << 10) & 0x07) | (gGStatistDodInfo_75[0] & 0x3ff);
	index++;
	if(index >= len)
	{
		return(TRUE);
	}

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 RcdReadRcdStoreInfoHook(u8 sort, u32 addr, u16 *data, u8 len)
** @Input     : sort:存储类别(0故障记录 1事件记录 2数据记录 3:故障录波 4:DOD记录)addr:起始地址 data:存储数据 len:数据长度
** @Output    : 读成功或失败
** @Function  : 读取存储记录信息
** @The notes :
**===============================================================================================*/
u8 RcdReadRcdStoreInfoHook(u8 sort, u32 addr, u16 *data, u8 len)
{
	u8 result = 0;

	(void)sort;

	/*外部FLASH读写正常*/
	if(1 == DEVEXTFLASHGetReadyFlag())
	{
		/*从外部FLASH读*/
		result = DEVEXTFLASHReadWord(addr, data, len);
	}

    return(result);
}

/*=================================================================================================
** @Name      : u8 RcdWriteRcdStoreInfoHook(u8 sort, u32 addr, u16 *data, u8 len)
** @Input     : sort:存储类别(0故障记录 1事件记录 2数据记录 3:故障录波 4:DOD记录)addr:起始地址 data:存储数据 len:数据长度
** @Output    : 写成功或失败
** @Function  : 记录信息写存储
** @The notes :
**===============================================================================================*/
u8 RcdWriteRcdStoreInfoHook(u8 sort, u32 addr, u16 *data, u8 len)
{
	u8 result = 0;

	(void)sort;

	/*外部FLASH读写正常*/
	if(1 == DEVEXTFLASHGetReadyFlag())
	{
		/*写入外部FLASH*/
		result = DEVEXTFLASHWriteWord(addr, data, len);
	}

    return(result);
}

/*=================================================================================================
** @Name      : u8 RcdEraseRcdStoreSectorHook(u8 sort, u32 addr)
** @Input     : sort:存储类别(0故障记录 1事件记录 2数据记录 3:故障录波 4:DOD记录)addr:起始地址
** @Output    : 擦除成功或失败
** @Function  : 擦除记录扇区
** @The notes : 擦除FLASH扇区
**===============================================================================================*/
u8 RcdEraseRcdStoreSectorHook(u8 sort, u32 addr)
{
	(void)sort;

	/*外部FLASH读写正常*/
	if(1 == DEVEXTFLASHGetReadyFlag())
	{
		/*擦除外部FLASH*/
		return(DEVEXTFLASHEraseSector(addr));
	}

    return(FALSE);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

