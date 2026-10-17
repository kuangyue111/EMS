/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : DodRcdStatis.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 放电深度DOD记录统计模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "ParaInterface.h"
#include "StaInterface.h"
#include "RecordInterface.h"
#include "EventUser.h"
#include "GroupMsg.h"
#include "DodRcdStatis.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u8 sReadNextDodFlag = 0;                         /*读下一页记录标志*/

extern u16 gGHisDodInfo_48[];                           /*历史DOD记录信息查询数组*/

extern u16 gGStatistDodInfo_75[];                       /*放电深度统计信息数组*/

extern u16 gGBmuGenPara_102[];                          /*主控通用参数值*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=================================================================================================
** @Name      : static void ReadDodRcdDisplayInit(void)
** @Input     : void
** @Output    : void
** @Function  : 读取DOD记录显示信息初始化
** @The notes : 读取初始显示的历史DOD值,用户任务单次调用(避免占用太长系统初始化时间)
**===============================================================================================*/
static void ReadDodRcdDisplayInit(void);

/*=================================================================================================
** @Name      : static void GroupDodStatisRcdDeal(u16 *newDodInfo, u8 infoNum)
** @Input     : newDodInfo:新增记录的DOD信息(DOD值/充电量/放电量/充电能/放电能) infoNum:信息个数(5个有效)
** @Output    : void
** @Function  : 计算和记录历史DOD值,增加DOD记录
** @The notes : DOD变化需要记录时调用
**===============================================================================================*/
static void GroupDodStatisRcdDeal(u16 *newDodInfo, u8 infoNum);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void GroupDodRcdStatisInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组放电深度DOD统计记录初始化
** @The notes :
**===========================================================================================*/
void GroupDodStatisRcdInit(void)
{
    u16 info16[5] = {0};

    /*读取放电深度DOD统计*/
    if(TRUE == ParaReadStoreDODStaInfo(info16, 5))
    {
    	gGStatistDodInfo_75[eDodStat75_NowDod] = info16[1];
		gGStatistDodInfo_75[eDodStat75_FstDod] = info16[2];
		gGStatistDodInfo_75[eDodStat75_MaxDod] = info16[3];
		gGStatistDodInfo_75[eDodStat75_AvgDod] = info16[4];
    }
    else
    {
		gGStatistDodInfo_75[eDodStat75_NowDod] = 1000 - gGBmuGenPara_102[eBmuGenPara102_NowSoc];
		gGStatistDodInfo_75[eDodStat75_FstDod] = gGStatistDodInfo_75[eDodStat75_NowDod];
		gGStatistDodInfo_75[eDodStat75_MaxDod] = gGStatistDodInfo_75[eDodStat75_NowDod];
		gGStatistDodInfo_75[eDodStat75_AvgDod] = gGStatistDodInfo_75[eDodStat75_NowDod];
    }
}

/*=============================================================================================
** @Name      : void GroupDodStatisRcdTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组放电深度DOD统计记录任务
** @The notes : 1s
**===========================================================================================*/
void GroupDodStatisRcdTask(void)
{
    u8 flag = 0;
    u16 dodInfo[5] = {0};
    static u8 sRcdDod = 0;
    static u16 sCTimer = 0;
    static u16 sDTimer = 0;
    static u16 sHisSoc = 0;
    static u16 sEepDod = 0xffff;

	static u32 sHisChgCap = 0;
	static u32 sHisDhgCap = 0;
	static u32 sHisChgEner = 0;
	static u32 sHisDhgEner = 0;

	//初始值异常重新赋值
	if((0 == sHisChgCap) || (sHisChgCap > GetGHisChgCapAPI()))
	{
		sHisChgCap = GetGHisChgCapAPI();
	}
	if((0 == sHisDhgCap) || (sHisDhgCap > GetGHisDhgCapAPI()))
	{
		sHisDhgCap = GetGHisDhgCapAPI();
	}
	if((0 == sHisChgEner) || (sHisChgEner > GetGHisChgEnerAPI()))
	{
		sHisChgEner = GetGHisChgEnerAPI();
	}
	if((0 == sHisDhgEner) || (sHisDhgEner > GetGHisDhgEnerAPI()))
	{
		sHisDhgEner = GetGHisDhgEnerAPI();
	}

    /*系统初始化不计算*/
    if(eWORK_INIT == GetGWorkStateAPI())
    {
    	sCTimer = 0;
    	sDTimer = 0;
        //sRcdDod = 0;
        sHisSoc = GetGBattSocAPI();
        //sEepDod = 0xffff;
    	return;
    }

    /*放电状态*/
    if(eCURR_DHG == GetGChgDhgStateAPI())
    {
    	/*当前放电深度大于统计值*/
		if((1000 - GetGBattSocAPI()) > gGStatistDodInfo_75[eDodStat75_NowDod])
		{
			sDTimer = 0;
			sRcdDod = 1;
			gGStatistDodInfo_75[eDodStat75_NowDod] = 1000 - GetGBattSocAPI();

			/*放电起始DOD不能大于当前DOD*/
			if(gGStatistDodInfo_75[eDodStat75_FstDod] > gGStatistDodInfo_75[eDodStat75_NowDod])
			{
    			gGStatistDodInfo_75[eDodStat75_FstDod] = gGStatistDodInfo_75[eDodStat75_NowDod];
			}
    	}
        /*至少连续放电3min*/
    	else if(sDTimer >= 180)
    	{
    		/*SOC下降5%或SOC低于3%*/
    		if(((GetGBattSocAPI() + 50) <= sHisSoc) || (GetGBattSocAPI() < 30))
    		{
    			sDTimer = 0;
    			sRcdDod = 1;
    			gGStatistDodInfo_75[eDodStat75_FstDod] = 1000 - sHisSoc;
    			gGStatistDodInfo_75[eDodStat75_NowDod] = 1000 - GetGBattSocAPI();
    		}
    	}
    	else
    	{
    		sDTimer++;
    	}

    	sCTimer = 0;
    }
	/*非放电状态*/
    else
	{
    	/*当前放电深度大于统计值*/
		if((1000 - GetGBattSocAPI()) > gGStatistDodInfo_75[eDodStat75_NowDod])
		{
			sRcdDod = 1;
			gGStatistDodInfo_75[eDodStat75_NowDod] = 1000 - GetGBattSocAPI();
    	}

		/*转为充电状态需要更新当前DOD*/
		if((eCURR_CHG == GetGChgDhgStateAPI()) && (1 == sRcdDod) && (0xffff != sEepDod))
		{
			/*至少充电1min*/
			if(sCTimer >= 60)
			{
				/*计算DOD记录相关信息*/
				dodInfo[0] = gGStatistDodInfo_75[eDodStat75_NowDod];
				dodInfo[1] = GetGHisChgCapAPI() - sHisChgCap;
				dodInfo[2] = GetGHisDhgCapAPI() - sHisDhgCap;
				dodInfo[3] = GetGHisChgEnerAPI() - sHisChgEner;
				dodInfo[4] = GetGHisDhgEnerAPI() - sHisDhgEner;

				/*计算和写入一条新DOD记录*/
				GroupDodStatisRcdDeal(dodInfo, 5);

				/*更新历史电能电量记录*/
				sHisChgCap = GetGHisChgCapAPI();
				sHisDhgCap = GetGHisDhgCapAPI();
				sHisChgEner = GetGHisChgEnerAPI();
				sHisDhgEner = GetGHisDhgEnerAPI();

				/*中间变量清零*/
				sCTimer = 0;
				sRcdDod = 0;
				flag = 1;
			}
			else
			{
				/*等待充电延时*/
				sCTimer++;
			}
		}
		/*非充电状态不需要记录DOD*/
		else
		{
			sCTimer = 0;
		}

		/*充电时记录放电起始信息*/
		if(eCURR_CHG == GetGChgDhgStateAPI())
		{
			sDTimer = 0;
			sHisSoc = GetGBattSocAPI();
		}
	}

	/*首次上电*/
    if(0xffff == sEepDod)
    {
    	/*读取显示的一页(50个)DOD记录显示*/
    	ReadDodRcdDisplayInit();

    	/*重新上电则重新记录一个DOD(防止下电前未记录)*/
        sRcdDod = 1;

    	/*记录当前已记录DOD的值*/
    	sEepDod = gGStatistDodInfo_75[eDodStat75_NowDod];
    }
	/*历史DOD变化*/
    else if(1 == flag)
    {
    	/*更新EEPROM中历史DOD值*/
    	ParaWriteStoreDODStaInfo(1, gGStatistDodInfo_75, 4);
    	sEepDod = gGStatistDodInfo_75[eDodStat75_NowDod];
    }
	/*DOD变化超过5%或非放电状态DOD已变化*/
    else if((ABS(gGStatistDodInfo_75[eDodStat75_NowDod], sEepDod) >= 50)
    	|| ((ABS(gGStatistDodInfo_75[eDodStat75_NowDod], sEepDod) >= 1)
    	&& (eCURR_DHG != GetGChgDhgStateAPI())))
    {
    	/*仅更新EEPROM中当前DOD值*/
    	ParaWriteStoreDODStaInfo(1, &gGStatistDodInfo_75[eDodStat75_NowDod], 1);
    	sEepDod = gGStatistDodInfo_75[eDodStat75_NowDod];
    }
}

/*=================================================================================================
** @Name      : void ChangHisDodRcdDisplayAPI(u8 home)
** @Input     : home:读首页标志 0:读下一页记录 1:读首页记录
** @Output    : void
** @Function  : 切换历史DOD显示信息
** @The notes : 读取历史DOD到显示区(读50条)
**===============================================================================================*/
void GroupChangHisDodDisplayAPI(u8 home)
{
    u8 i = 0;
    u8 num = 0;
    u8 flag = 0;
    t_DodStoreInfo info = {0};

    /*不切换日期且都首页记录则跳转切换首页*/
    if(1 == home)
    {
    	/*已切换首页记录*/
    	sReadNextDodFlag = 0;

    	/*切换首页失败*/
    	if(FALSE == RcdJumpDispHomeDodRcdUserAPI())
    	{
    		/*不更新显示*/
    		return;
    	}
    }
    else
    {
    	/*记录非首页记录*/
    	sReadNextDodFlag = 1;
    }

	/*有效记录数清零*/
    num = gGHisDodInfo_48[eHisDodInfo48_Num];
	gGHisDodInfo_48[eHisDodInfo48_Num] = 0;

	/*读50条记录*/
	for(i = 0; i < HISDOD_DISP_LEN; i++)
	{
		/*未读失败过则读取一条记录且成功*/
		if((0 == flag) && (TRUE == RcdDisplayOneDodRcdUserAPI(&info)))
		{
			//第一条最新记录
			if(0 == i)
			{
				//仅显示最新记录时间信息
				gGHisDodInfo_48[eHisDodInfo48_Id] = info.id;
				gGHisDodInfo_48[eHisDodInfo48_Y] = info.year;
				gGHisDodInfo_48[eHisDodInfo48_MD] = info.MD;
				gGHisDodInfo_48[eHisDodInfo48_HM] = info.HM;
				gGHisDodInfo_48[eHisDodInfo48_S] = info.sec;
			}

			//计算显示个数和记录显示值
			gGHisDodInfo_48[eHisDodInfo48_Num]++;
			gGHisDodInfo_48[i+eHisDodInfo48_Dod] = info.dod;
		}
		/*读取已失败*/
		else
		{
			flag = 1;
			/*有读取记录成功*/
			if(i > 0)
			{
				/*剩余显示清零*/
				gGHisDodInfo_48[i+eHisDodInfo48_Dod] = 0;
			}
			/*第1条就读取失败(没有更多记录)*/
			else
			{
				/*不再读取(保持原来记录)*/
				gGHisDodInfo_48[eHisDodInfo48_Num] = num;
				break;
			}
		}
	}

	/*触发发送历史DOD信息*/
	SendCanGroupHisDodInfoMsg();
}

/*=============================================================================================
** @Name      : void GroupClrDodStatisRcdAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除电池组放电深度DOD统计记录信息
** @The notes :
**===========================================================================================*/
void GroupClrDodStatisRcdAPI(void)
{
    u8 i = 0;

	//清除显示最新记录时间信息
	gGHisDodInfo_48[eHisDodInfo48_Id] = 0;
	gGHisDodInfo_48[eHisDodInfo48_Num] = 0;
	gGHisDodInfo_48[eHisDodInfo48_Y] = 0;
	gGHisDodInfo_48[eHisDodInfo48_MD] = 0;
	gGHisDodInfo_48[eHisDodInfo48_HM] = 0;
	gGHisDodInfo_48[eHisDodInfo48_S] = 0;

	//清除显示的50个记录
	for(i = 0; i < HISDOD_DISP_LEN; i++)
	{
		gGHisDodInfo_48[i+eHisDodInfo48_Dod] = 0;
	}

	//清除DOD统计值
	gGStatistDodInfo_75[eDodStat75_NowDod] = 0;
	gGStatistDodInfo_75[eDodStat75_FstDod] = 0;
	gGStatistDodInfo_75[eDodStat75_MaxDod] = 0;
	gGStatistDodInfo_75[eDodStat75_AvgDod] = 0;

	ParaWriteStoreDODStaInfo(1, gGStatistDodInfo_75, 4);

	/*触发发送历史DOD信息*/
	SendCanGroupHisDodInfoMsg();
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void ReadDodRcdDisplayInit(void)
** @Input     : void
** @Output    : void
** @Function  : 读取DOD记录显示信息初始化
** @The notes : 读取初始显示的历史DOD值,用户任务单次调用(避免占用太长系统初始化时间)
**===============================================================================================*/
static void ReadDodRcdDisplayInit(void)
{
    u8 i = 0;
    t_DodStoreInfo info = {0};

	//逐条读取50个记录
	for(i = 0; i < HISDOD_DISP_LEN; i++)
	{
		//读一条记录成功
		if(TRUE == RcdDisplayOneDodRcdUserAPI(&info))
		{
			//第一条最新记录
			if(0 == i)
			{
				//仅显示最新记录时间信息
				gGHisDodInfo_48[eHisDodInfo48_Id] = info.id;
				gGHisDodInfo_48[eHisDodInfo48_Y] = info.year;
				gGHisDodInfo_48[eHisDodInfo48_MD] = info.MD;
				gGHisDodInfo_48[eHisDodInfo48_HM] = info.HM;
				gGHisDodInfo_48[eHisDodInfo48_S] = info.sec;
			}

			//计算显示个数和记录显示值
			gGHisDodInfo_48[eHisDodInfo48_Num]++;
			gGHisDodInfo_48[i+eHisDodInfo48_Dod] = info.dod;
		}
		/*读取失败则停止*/
		else
		{
			return;
		}
	}
}

/*=================================================================================================
** @Name      : static void GroupDodStatisRcdDeal(u16 *newDodInfo, u8 infoNum)
** @Input     : newDodInfo:新增记录的DOD信息(DOD值/充电量/放电量/充电能/放电能) infoNum:信息个数(5个有效)
** @Output    : void
** @Function  : 计算和记录历史DOD值,增加DOD记录
** @The notes : DOD变化需要记录时调用
**===============================================================================================*/
static void GroupDodStatisRcdDeal(u16 *newDodInfo, u8 infoNum)
{
	u8 i = 0;
	u8 len = 0;
	u8 num = 0;
	u16 maxDod = 0;
	u32 sumDod = 0;
	t_DodStoreInfo dodInfo;

	/*输入值无效*/
	if((NULL == newDodInfo) || (5 != infoNum))
	{
		return;
	}

	/*DOD记录变化*/
	//if(gGHisDodInfo_48[eHisDodInfo48_Dod] != newDodInfo[0])
	{
		maxDod = newDodInfo[0];
		sumDod = newDodInfo[0];
		num = 1;

		/*非显示首页记录*/
		if(1 == sReadNextDodFlag)
		{
			/*恢复到显示首页记录*/
			GroupChangHisDodDisplayAPI(1);
		}

		/*至少2个值*/
		if(gGHisDodInfo_48[eHisDodInfo48_Num] >= 2)
		{
			/*未满50个记录*/
			if(gGHisDodInfo_48[eHisDodInfo48_Num] < HISDOD_DISP_LEN)
			{
				/*全部记录(实际n+1个)*/
				len = gGHisDodInfo_48[eHisDodInfo48_Num];
			}
			/*满50个记录*/
			else
			{
				/*去掉最后1个(实际n个)*/
				len = gGHisDodInfo_48[eHisDodInfo48_Num] - 1;
			}

			/*求49个DOD中最大值和平均值*/
			for(i = (len - 1); i > 0; i--)
			{
				if(gGHisDodInfo_48[eHisDodInfo48_Dod + i - 1] > maxDod)
				{
					maxDod = gGHisDodInfo_48[eHisDodInfo48_Dod + i - 1];
				}
				sumDod += gGHisDodInfo_48[eHisDodInfo48_Dod + i - 1];
				num++;

				/*显示历史DOD值后移一个位置*/
				gGHisDodInfo_48[eHisDodInfo48_Dod + i] = gGHisDodInfo_48[eHisDodInfo48_Dod + i - 1];
			}
		}
		else
		{
			/*只有一个值*/
			if(1 == gGHisDodInfo_48[eHisDodInfo48_Num])
			{
				/*求新旧两值的最大值*/
				if(gGHisDodInfo_48[eHisDodInfo48_Dod] > maxDod)
				{
					maxDod = gGHisDodInfo_48[eHisDodInfo48_Dod];
				}
				sumDod += gGHisDodInfo_48[eHisDodInfo48_Dod];
				num++;

				/*显示历史DOD值后移一个位置*/
				gGHisDodInfo_48[eHisDodInfo48_Dod + 1] = gGHisDodInfo_48[eHisDodInfo48_Dod];
			}
		}

		/*更新显示统计记录*/
		gGStatistDodInfo_75[eDodStat75_MaxDod] = maxDod;
		gGStatistDodInfo_75[eDodStat75_AvgDod] = sumDod / num;

		/*增加一条DOD记录*/
		RcdCreateDodRcdInfoAPI(newDodInfo, 5);

		/*增加一条日志事件记录*/
		EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_DodAdd, GetGSampOutCurrAPI(), gGStatistDodInfo_75[eDodStat75_NowDod], gGStatistDodInfo_75[eDodStat75_FstDod], gGHisDodInfo_48[eHisDodInfo48_Dod + 1]);

		/*获取最新缓存信息*/
		if(TRUE == RcdGetDodBuffInfoAPI(&dodInfo))
		{
			/*更新显示历史记录和时间*/
			gGHisDodInfo_48[eHisDodInfo48_Dod] = dodInfo.dod;
			gGHisDodInfo_48[eHisDodInfo48_Id] = dodInfo.id;
			gGHisDodInfo_48[eHisDodInfo48_Y] = dodInfo.year;
			gGHisDodInfo_48[eHisDodInfo48_MD] = dodInfo.MD;
			gGHisDodInfo_48[eHisDodInfo48_HM] = dodInfo.HM;
			gGHisDodInfo_48[eHisDodInfo48_S] = dodInfo.sec;

			/*更新显示历史记录数目*/
			if(gGHisDodInfo_48[eHisDodInfo48_Num] < HISDOD_DISP_LEN)
			{
				gGHisDodInfo_48[eHisDodInfo48_Num]++;
			}
		}

		//更新显示DOD记录数目
		//ErrEvntRcdUpdateStateAPI();
	}
}
