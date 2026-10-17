/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : PublicInfo.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     :
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicPara.h"
#include "GroupPara.h"
#include "ParaInterface.h"
#include "EnerInterface.h"
#include "CANUser.h"
#include "PublicInfo.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gCANVerTime_0[];             /*对时信息*/
extern u16 gCANDevInfo_1[];             /*设备信息*/
extern u8 gCANSerialNum_2[];		    /*序列号信息*/
extern u16 gGStatistTime_70[];	        /*时间统计信息数组*/

extern u16 gSysGenParaRO_100[];			/*系统通用参数设置*/
extern u16 gGBmuGenPara_102[];	        /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];		/*主控高级参数值*/

t_SysClock gSysClock = {0};             /*系统时钟(北京时间)*/
u32 gSysSecondTime = 0;                 /*系统运行计时(1s)*/

u16 gSProjecInfo_40[SINDEX40_LEN]  =	/*项目参数信息数组*/
{
	0x0003,		/*设备识别码		H:RTU编号 L:设备类型（0EMS,1PCS，2BSMU，3:BMU)*/
	20010,		/*项目时间		年月-2000（例如18070，最后一位为批次流水号）*/
	0x0000,		/*项目信息		项目代号(识别电池与参数)*/
	0x0000,		/*项目编码		H:安装地点 L:安装地编号*/
	0x0001,		/*系统识别码		H:机组号(0非机组) L:电池组号(0非电池组)*/
	0x0032,		/*系统定位码		H:系统类型（0二级系统 1扩展三级系统 2独立三级系统）  L:系统所在设备号*/
	0x0101,		/*系统信息H		H:系统总数 L:电池组数*/
	0x0d02,		/*系统信息L		H6:从控数目 L10:从控电压数目*/
	0x1004,		/*电池信息		H6:电池类型(0铅酸 1三元 2磷酸铁锂 3钛酸锂 4其它) L10:从控温度数目*/
	0,			/*标称电压(0.1V)*/
	0,			/*标称容量(0.1AH)*/
	0,			/*标称能量(0.1kWh)*/
	0,			/*系统版本号*/
	0,			/*主控版本号*/
	0			/*从控版本号*/
};

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=================================================================================================
** @Name      : static void UpdateMonAndYearAccdDay(t_SysClock *ptm)
** @Input     : ptm :系统时间
** @Output    : void
** @Function  : 根据天数更新月和年
** @The notes :
**===============================================================================================*/
static void UpdateMonAndYearAccdDay(t_SysClock *ptm);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=================================================================================================
** @Name      : void SysPublicInfoInit(void)
** @Input     : void
** @Output    : void
** @Function  : 系统系统通用信息初始化
** @The notes :
**===============================================================================================*/
void SysPublicInfoInit(void)
{
	u8 i = 0;
	t_SysClock rtcClock = {0};
	u8 serialNum[CANINDEX2_LEN + 1] = {0};
	u16 clockTime[CANINDEX0_LEN + 1] = {0};

    /*读系统时间记录(标志/年/月日/时分/毫秒)*/
	if(TRUE == ParaReadStoreSysClock(clockTime, CANINDEX0_LEN + 1))
	{
		for(i = 0; i < CANINDEX0_LEN; i++)
		{
			gCANVerTime_0[i] = clockTime[1 + i];
		}
	}

    /*将校时索引信息更新到系统时钟*/
	SysClockUpdateAPI();

    /*读RTC时间记录*/
	if(TRUE == DEVEXTRTCGetNowClockAPI(&rtcClock))
	{
		/*RTC时间有效*/
		if((((u32)rtcClock.year * 365 + (u32)rtcClock.month * 30 + rtcClock.day) >= ((u32)gSysClock.year * 365 + (u32)gSysClock.month * 30 + gSysClock.day))
			&& (((u32)rtcClock.year * 365 + (u32)rtcClock.month * 30 + rtcClock.day) <= ((u32)2200 * 365 + (u32)gSysClock.month * 30 + gSysClock.day)))
		{
			/*使用RTC时间校时*/
			SysClockRtcVerifyAPI(&rtcClock);
		}
		/*RTC时间无效*/
		else
		{
			/*校准RTC时间*/
			(void)DEVEXTRTCSetNowClockAPI(&gSysClock);
		}
	}

	/*读系统运行时间,单位:1h*/
	if(TRUE == ParaReadStoreSysRunTime(clockTime, 2))
	{
		/*记录系统运行时间1H*/
		gGStatistTime_70[eTime70_SysRun] = clockTime[1];
		/*系统运行时间由1h记录转化为1s*/
		gSysSecondTime = (u32)clockTime[1] * 3600;
	}

    /*读设备序列号(长度20+1标志)*/
	if(TRUE == ParaReadStoreDevSerial(serialNum, CANINDEX2_LEN + 1))
	{
		for(i = 0; i < CANINDEX2_LEN; i++)
		{
	        gCANSerialNum_2[i] = serialNum[1 + i];
		}
	}

    /*初始化项目参数信息*/
	gSProjecInfo_40[eSysProj40_DevCode] = PRJ_PAR_DEV_ID;       //设备识别码 H:RTU编号 L:设备类型（0EMS,1PCS，2BSMU，3:BMU)
	gSProjecInfo_40[eSysProj40_ProjTime] = PRJ_PAR_PRD_DAT;     //设备出厂日期 年月-2000（例如18070，最后一位为批次流水号）
	gSProjecInfo_40[eSysProj40_ProjArea] = PRJ_PARA_NUM_INFO;   //项目信息(编号)
	gSProjecInfo_40[eSysProj40_ProjAddr] = PRJ_PAR_INS_SIT;     //项目编码   H:安装地点 L:安装地编号
	gSProjecInfo_40[eSysProj40_SysCode] = (PRJ_PAR_SYS_ID & 0xff00) | (GetSysSelfGroupIdAPI() + 1);             //系统识别码 H:机组号(0非机组) L:电池组号(0非电池组)
	gSProjecInfo_40[eSysProj40_SysType] = (PRJ_PAR_SYS_TYP & 0xff00) | (gCANDevInfo_1[eCANDev1_Addr] & 0x00ff); //系统定位码 H:系统类型(0二级系统 1扩展三级系统 2独立三级系统) L:系统所在设备号

    //系统信息H  H:系统总数 L:电池组数
	gSProjecInfo_40[eSysProj40_SysInfoH] = (PRJ_PAR_SYS_NUM & 0xff00) | (gSysGenParaRO_100[eSGenPara100_AllGNum] & 0x00ff);

    //系统信息L  H6:从控数目 L10:从控电压总数目
	gSProjecInfo_40[eSysProj40_SysInfoL] = (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] << 10) | (gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] & 0x03ff);

    //电池信息 H6:电池类型(0铅酸 1三元 2磷酸铁锂 3钛酸锂 4其它) L10:从控温度总数目
	gSProjecInfo_40[eSysProj40_BattInfo] = (CELL_CHEM_TYPE << 10) | (gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] & 0x03ff);

    //能量信息
	gSProjecInfo_40[eSysProj40_StandVolt] = (u16)((u32)DEFAULT_STANDCV * gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] / 100);//标称电压 0.1V
	//gSProjecInfo_40[eSysProj40_StandCap] = (u16)(GetGroupStandCapAPI() / 100);    //标称容量 0.1Ah
	//gSProjecInfo_40[eSysProj40_StandEner] = (u16)(GetGroupStandEnerAPI() / 100);  //标称能量 0.1kWh

    //版本信息
	gSProjecInfo_40[eSysProj40_SysVersion] = BMU_SW_VERH;                           //系统版本号 BSMU版本号
	gSProjecInfo_40[eSysProj40_MainVersion] = BMU_SW_VERL;                          //主控版本号 BMU版本号
	gSProjecInfo_40[eSysProj40_SlaveVersion] = 0x0100;                              //从控版本号 BMU版本号

    /*判断自身内部CAN协议是否使能*/
	#if(0 == BMU_SELF_CAN_EN)
	CANSetMsgCycleTxEnAPI(0);
	#endif
}

/*=================================================================================================
** @Name      : void SysClockCountTask(void)
** @Input     : void
** @Output    : void
** @Function  : 系统时钟(北京时间)计数任务,ms计时
** @The notes : 1ms定时中断调用
**===============================================================================================*/
void SysClockCountTask(void)
{
	static u16 sSysMilSecTime = 0;

	/*上电1ms计时*/
	sSysMilSecTime++;
	if(sSysMilSecTime >= 1000)
	{
		gSysSecondTime++;
		sSysMilSecTime %= 1000;
	}

	/*时钟1ms计时*/
	gSysClock.msecond++;
	if(gSysClock.msecond >= 1000)
	{
		gSysClock.msecond = 0;
		gSysClock.second++;
		if(gSysClock.second >= 60)
		{
			gSysClock.second = 0;
			gSysClock.minute++;
			if(gSysClock.minute >= 60)
			{
				gSysClock.minute = 0;
				gSysClock.hour++;
				if(gSysClock.hour >= 24)
				{
					gSysClock.hour = 0;
					gSysClock.day++;
					UpdateMonAndYearAccdDay(&gSysClock);
				}
			}
		}
	}
}

/*=================================================================================================
** @Name      : void SysClockRecordTask(void)
** @Input     : void
** @Output    : void
** @Function  : 系统时钟时间记录任务(包括记录运行时间)
** @The notes : 2s周期调用
**===============================================================================================*/
void SysClockRecordTask(void)
{
	u8 i = 0;
	u16 time = 0;
	static u8 RcdFlag = 0;
	static u32 sHisTime = 0;
	static u32 sHisWState = 0;
	static u32 sHisCState = 0;
	static u16 sHisClock[CANINDEX0_LEN] = {0};

	/*状态变化且记录间隔10s*/
	if(((GetGWorkStateAPI() != sHisWState)
		|| (GetGChgDhgStateAPI() != sHisCState))
		&& (ABS(GetSysSecondTimeAPI(), sHisTime) >= 10))
	{
		/*更新校时字段*/
		SysClockDisplayAPI();

		/*记录校时字段(不记录ms)*/
		for(i = 0; i < (CANINDEX0_LEN - 1); i++)
		{
			/*校时字段有变化才重写*/
			if(gCANVerTime_0[i] != sHisClock[i])
			{
				ParaWriteStoreSysClock(1 + i, &gCANVerTime_0[i], 1);
				sHisClock[i] = gCANVerTime_0[i];
			}
		}

		sHisTime = GetSysSecondTimeAPI();
		sHisWState = GetGWorkStateAPI();
		sHisCState = GetGChgDhgStateAPI();

		/*运行时间1h计时超过半小时需要四舍五入记录*/
		if(((GetSysSecondTimeAPI() % 3600) >= 1800) && (0 == RcdFlag))
		{
			/*记录系统运行时间四舍五入(确保重新上电计时丢失小于半小时)*/
			time = gGStatistTime_70[eTime70_SysRun] + 1;
			ParaWriteStoreSysRunTime(1, &time, 1);
			RcdFlag = 1;
		}
	}
	/*状态未变化*/
	else
	{
		/*间隔10min或校时改变年月日则记录系统时钟*/
		if((ABS(GetSysSecondTimeAPI(), sHisTime) >= 600)
			|| (gCANVerTime_0[0] != sHisClock[0])
			|| (gCANVerTime_0[1] != sHisClock[1]))
		{
			/*更新校时字段*/
			SysClockDisplayAPI();

			/*记录校时字段*/
			for(i = 0; i < CANINDEX0_LEN; i++)
			{
				/*校时字段有变化才重写*/
				if(gCANVerTime_0[i] != sHisClock[i])
				{
					ParaWriteStoreSysClock(1 + i, &gCANVerTime_0[i], 1);
					sHisClock[i] = gCANVerTime_0[i];
				}
			}

			sHisTime = GetSysSecondTimeAPI();
		}
	}

	/*运行时间1h计时变化则记录*/
	if(gGStatistTime_70[eTime70_SysRun] != (GetSysSecondTimeAPI() / 3600))
	{
		/*单位由1s转化为1h显示*/
		gGStatistTime_70[eTime70_SysRun] = GetSysSecondTimeAPI() / 3600;

		/*记录系统运行时间*/
		ParaWriteStoreSysRunTime(1, &gGStatistTime_70[eTime70_SysRun], 1);
		RcdFlag = 0;
	}
}

/*=================================================================================================
** @Name      : void SysDispProjInfoTask(void)
** @Input     : void
** @Output    : void
** @Function  : 系统项目参数更新显示任务
** @The notes : 2s周期调用
**===============================================================================================*/
void SysDispProjInfoTask(void)
{
    /*更新可变的项目参数信息*/
	//gSProjecInfo_40[eSysProj40_DevCode] = PRJ_PAR_DEV_ID;       //设备识别码 H:RTU编号 L:设备类型（0EMS,1PCS，2BSMU，3:BMU)
	//gSProjecInfo_40[eSysProj40_ProjTime] = PRJ_PAR_PRD_DAT;     //设备出厂日期 年月-2000（例如18070，最后一位为批次流水号）
	//gSProjecInfo_40[eSysProj40_ProjArea] = PRJ_PARA_NUM_INFO;   //项目信息(编号)
	//gSProjecInfo_40[eSysProj40_ProjAddr] = PRJ_PAR_INS_SIT;     //项目编码   H:安装地点 L:安装地编号
	gSProjecInfo_40[eSysProj40_SysCode] = (PRJ_PAR_SYS_ID & 0xff00) | (GetSysSelfGroupIdAPI() + 1);             //系统识别码 H:机组号(0非机组) L:电池组号(0非电池组)
	gSProjecInfo_40[eSysProj40_SysType] = (PRJ_PAR_SYS_TYP & 0xff00) | (gCANDevInfo_1[eCANDev1_Addr] & 0x00ff); //系统定位码 H:系统类型(0二级系统 1扩展三级系统 2独立三级系统) L:系统所在设备号

    //系统信息H  H:系统总数 L:电池组数
	gSProjecInfo_40[eSysProj40_SysInfoH] = (PRJ_PAR_SYS_NUM & 0xff00) | (gSysGenParaRO_100[eSGenPara100_AllGNum] & 0x00ff);

    //系统信息L  H6:从控数目 L10:从控电压数目
	gSProjecInfo_40[eSysProj40_SysInfoL] = (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] << 10) | (gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] & 0x03ff);

    //电池信息 H6:电池类型(0铅酸 1三元 2磷酸铁锂 3钛酸锂 4其它) L10:从控温度数目
	gSProjecInfo_40[eSysProj40_BattInfo] = (CELL_CHEM_TYPE << 10) | (gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] & 0x03ff);

    //能量信息
	gSProjecInfo_40[eSysProj40_StandVolt] = (u16)((u32)DEFAULT_STANDCV * gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] / 100);//标称电压 0.1V
	gSProjecInfo_40[eSysProj40_StandCap] = (u16)(GetGroupStandCapAPI() / 100);      //标称容量 0.1Ah
	gSProjecInfo_40[eSysProj40_StandEner] = (u16)(GetGroupStandEnerAPI() / 100);    //标称能量 0.1kWh

    //版本信息
	//gSProjecInfo_40[eSysProj40_SysVersion] = BMU_SW_VERH;                           //系统版本号 BSMU版本号
	//gSProjecInfo_40[eSysProj40_MainVersion] = BMU_SW_VERL;                          //主控版本号 BMU版本号
	//gSProjecInfo_40[eSysProj40_SlaveVersion] = 0x0100;                              //从控版本号 BMU版本号
}

/*=================================================================================================
** @Name      : void SysClockExtVerifyAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 系统时钟外部校时处理
** @The notes : 接收校时报文时调用
**===============================================================================================*/
void SysClockExtVerifyAPI(void)
{
	//校验下发的时间有效
	if((gCANVerTime_0[eCANTim0_Y] >= 2000) && (gCANVerTime_0[eCANTim0_Y] <= 2200)
		&& (gCANVerTime_0[eCANTim0_MD] >= 0x0101) && (gCANVerTime_0[eCANTim0_MD] <= 0x1f1f)
		&& (gCANVerTime_0[eCANTim0_HM] <= 0x3c3c) && (gCANVerTime_0[eCANTim0_MS] <= 0xea60))
	{
		/*从外部校时信息更新系统时钟*/
		SysClockUpdateAPI();

		/*更新RTC时钟*/
		(void)DEVEXTRTCSetNowClockAPI(&gSysClock);
	}
	//校验下发的时间无效
	else
	{
		/*重新更正校时字段*/
		SysClockDisplayAPI();
	}
}

/*=================================================================================================
** @Name      : void SysClockRtcVerifyAPI(t_SysClock *rtcClock)
** @Input     : rtcClock:RTC当前时间
** @Output    : void
** @Function  : 系统时钟RTC校时处理
** @The notes : 发现系统时钟与RTC不一致时调用
**===============================================================================================*/
void SysClockRtcVerifyAPI(t_SysClock *rtcClock)
{
	/*从RTC信息更新系统时钟*/
	gSysClock.year = rtcClock->year;
	gSysClock.month = rtcClock->month;
	gSysClock.day = rtcClock->day;
	gSysClock.hour = rtcClock->hour;
	gSysClock.minute = rtcClock->minute;
	gSysClock.second = rtcClock->second;
	gSysClock.msecond = rtcClock->msecond;

	/*更新外部校时信息显示*/
	SysClockDisplayAPI();
}

/*=================================================================================================
** @Name      : void SysClockUpdateAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 将显示区校时信息更新到系统时钟信息
** @The notes : 校时信息变化时调用
**===============================================================================================*/
void SysClockUpdateAPI(void)
{
	/*从外部校时信息更新系统时钟*/
	gSysClock.year = gCANVerTime_0[eCANTim0_Y];
	gSysClock.month = (gCANVerTime_0[eCANTim0_MD] >> 8) & 0xff;
	gSysClock.day = gCANVerTime_0[eCANTim0_MD] & 0xff;
	gSysClock.hour = (gCANVerTime_0[eCANTim0_HM] >> 8) & 0xff;
	gSysClock.minute = gCANVerTime_0[eCANTim0_HM] & 0xff;
	gSysClock.second = gCANVerTime_0[eCANTim0_MS] / 1000;
	gSysClock.msecond = gCANVerTime_0[eCANTim0_MS] % 1000;
}

/*=================================================================================================
** @Name      : void SysClockDisplayAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 更新系统时钟信息到显示区
** @The notes : 查询系统时间时调用
**===============================================================================================*/
void SysClockDisplayAPI(void)
{
	gCANVerTime_0[eCANTim0_Y] = gSysClock.year;
	gCANVerTime_0[eCANTim0_MD] = ((u16)gSysClock.month << 8) | (u16)gSysClock.day;
	gCANVerTime_0[eCANTim0_HM] = ((u16)gSysClock.hour << 8) | (u16)gSysClock.minute;
	gCANVerTime_0[eCANTim0_MS] = (u16)gSysClock.second * 1000 + (u16)gSysClock.msecond;
}

/*=================================================================================================
** @Name      : void SysDevSerialDispayAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 读取系统设备条码序列号进行显示
** @The notes : 从存储区读取
**===============================================================================================*/
void SysDevSerialDispayAPI(void)
{
	u8 i = 0;
	u8 serialNum[CANINDEX2_LEN + 1] = {0};

    /*读设备序列号(长度21+1标志)*/
	if(TRUE == ParaReadStoreDevSerial(serialNum, CANINDEX2_LEN + 1))
	{
		for(i = 0; i < CANINDEX2_LEN; i++)
		{
	        gCANSerialNum_2[i] = serialNum[1 + i];
		}
	}
	else
	{
		for(i = 0; i < CANINDEX2_LEN; i++)
		{
	        gCANSerialNum_2[i] = 0;
		}
	}
}

/*=================================================================================================
** @Name      : void ClrSysSecondTimeAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除电池组系统运行计时
** @The notes :
**===============================================================================================*/
void ClrSysSecondTimeAPI(void)
{
	gSysSecondTime = 0;
}

/*=================================================================================================
** @Name      : u32 GetSysSecondTimeAPI(void)
** @Input     : void
** @Output    : 系统计时 1s
** @Function  : 获取电池组系统运行计时
** @The notes : 上电开始计时,考虑可能会外部清零复位
**===============================================================================================*/
u32 GetSysSecondTimeAPI(void)
{
	return(gSysSecondTime);
}

/*=================================================================================================
** @Name      : t_SysClock GetSysDateClockAPI(void)
** @Input     : void
** @Output    : 系统时钟 年/月/日/时/分/秒/毫秒
** @Function  : 获取电池组系统时钟
** @The notes : 北京时间
**===============================================================================================*/
t_SysClock GetSysDateClockAPI(void)
{
	return(gSysClock);
}

/*=================================================================================================
** @Name      : u8 GetSysSelfGroupIdAPI(void)
** @Input     : void
** @Output    : 系统自身所在电池组组号
** @Function  : 获取系统自身所在电池组组号
** @The notes : 自身主控序号
**===============================================================================================*/
u8 GetSysSelfGroupIdAPI(void)
{
    u8 group = 0;

    if((gGBmuGenPara_102[eBmuGenPara102_BmuDev] >= BMU_BAS_DEVADDR)
    	&& (gGBmuGenPara_102[eBmuGenPara102_BmuDev] < SMU_BAS_DEVADDR))
    {
    	//group = (u8)((gGBmuGenPara_102[eBmuGenPara102_BmuDev] - BMU_BAS_DEVADDR) % gSysGenParaRO_100[eSGenPara100_AllGNum]);
    	group = (u8)(gGBmuGenPara_102[eBmuGenPara102_BmuDev] - BMU_BAS_DEVADDR);
    }
	else
	{
		group = 0xff;
	}

    return(group);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=================================================================================================
** @Name      : static void UpdateMonAndYearAccdDay(t_SysClock *ptm)
** @Input     : ptm :系统时间
** @Output    : void
** @Function  : 根据天数更新月和年
** @The notes :
**===============================================================================================*/
static void UpdateMonAndYearAccdDay(t_SysClock *ptm)
{
    switch(ptm->month)
    {
        /*如果当前月份是31天*/
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if(ptm->day >= 32)
            {
                ptm->day = 1;
                ptm->month++;

                /*更新年份*/
                if(ptm->month >= 13)
                {
                    ptm->month = 1;
                    ptm->year++;
                }
            }
            break;

        case 2:
            /*判断当前是否为闰年*/
            if(((ptm->year % 4 == 0) && (ptm->year % 100 != 0)) || (ptm->year % 400 == 0))
            {
                /*为闰年有29天*/
                if(ptm->day >= 30)
                {
                    ptm->day = 1;
                    ptm->month++;
                }
            }
            else
            {
                if(ptm->day >= 29)
                {
                    ptm->day = 1;
                    ptm->month++;
                }
            }
            break;

        default:
            /*其他月份只有30天*/
            if(ptm->day >= 31)
            {
                ptm->day = 1;
                ptm->month++;
            }
            break;
    }
}
