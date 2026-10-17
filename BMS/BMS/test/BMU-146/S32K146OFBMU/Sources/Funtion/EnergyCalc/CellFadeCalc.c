/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : CellFadeCalc.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 电池总容量和SOH计算模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "ParaInterface.h"
#include "CapInfoCalc.h"
#include "EnerInterface.h"
#include "CellFadeCalc.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gGSysCapInfo_61[];
extern u16 gGBmuGenPara_102[];		    /*主控通用参数值*/

static t_FADEFORM sFadeForm =
{
	1000,               		        /*衰减总容量(单位:0.1AH)*/
	1000,              		        	/*额定总容量(单位:0.1AH)*/
	1,              			        /*实际循环次数(单位:1次)*/
	1000             			        /*额定循环次数(单位:1次)*/
};

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CalcGroupCellFadeHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 计算电池组电池衰减状态
** @The notes :
**===========================================================================================*/
static void CalcGroupCellFadeHandle(void);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void GroupFadeSohInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池衰减程度SOH计算初始化
** @The notes :
**===========================================================================================*/
void GroupFadeSohInit(void)
{
	u16 raCap = 0;
	u16 data[2] = {0};

	raCap = gGBmuGenPara_102[eBmuGenPara102_RateCap];

	//额定容量无小数则为标称容量,只取93%
	if(0 == (gGBmuGenPara_102[eBmuGenPara102_RateCap] % 10))
	{
		raCap = (u16)((u32)gGBmuGenPara_102[eBmuGenPara102_RateCap] * 93 / 100);
	}

	/*读取记录的SOH值*/
	if((TRUE == ParaReadStoreNowCapSoh(data, 2))
		&& (data[1] <= 1000))
	{
		//sFadeForm.fadeCap = gGBmuGenPara_102[eBmuGenPara102_RateCap] * data[1] / 1000;
		sFadeForm.fadeCap = raCap * data[1] / 1000;
	}

    sFadeForm.fadeCycle = 0;

    /*获取额定总容量和额定循环次数*/
	//sFadeForm.ratedCap = gGBmuGenPara_102[eBmuGenPara102_RateCap];
	sFadeForm.ratedCap = raCap;
	sFadeForm.ratedCycle = gGBmuGenPara_102[eBmuGenPara102_DampNum];
}

/*=============================================================================================
** @Name      : void GroupFadeSohCalcTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池容量衰减程度SOH计算任务
** @The notes : 100ms调用,测算总容量和SOH
**===========================================================================================*/
void GroupFadeSohCalcTask(void)
{
	static u8 state = 0;

	switch(state)
	{
		case 0:
			CalcGroupCellFadeHandle();	/*计算电池SOH*/
			state = 0;
			break;

		default:
			state = 0;
			break;
	}
}

/*=============================================================================================
** @Name      : void CorrGroupRatedCapAPI(u16 rateCap)
** @Input     : rateCap:电池组额定容量(单位:0.1AH)
** @Output    : void
** @Function  : 修正电池组额定容量
** @The notes :
**===========================================================================================*/
void CorrGroupRatedCapAPI(u16 rateCap)
{
	sFadeForm.ratedCap = (u32)rateCap * 100;

	/*写电池组总容量到EEP*/
	EnerChangEepGRatedCapHook(rateCap);
}

/*=============================================================================================
** @Name      : void CorrGroupRatedCycleAPI(u16 rateCycle)
** @Input     : rateCycle:电池组额定循环次数
** @Output    : void
** @Function  : 修正电池组额定循环次数
** @The notes :
**===========================================================================================*/
void CorrGroupRatedCycleAPI(u16 rateCycle)
{
	sFadeForm.ratedCycle = rateCycle;

	/*写电池组额定循环次数到EEP*/
	EnerChangEepGDampNumHook(rateCycle);
}

/*=============================================================================================
** @Name      : u16 GetGFadeTotalCapAPI(void)
** @Input     : void
** @Output    : 电池组衰减后的总容量值(0.1AH)
** @Function  : 获取电池组衰减后的总容量值
** @The notes :
**===========================================================================================*/
u16 GetGFadeTotalCapAPI(void)
{
    return(sFadeForm.fadeCap);
}

/*=============================================================================================
** @Name      : u16 GetGroupRatedCapAPI(void)
** @Input     : void
** @Output    : 电池组额定容量(单位:0.1AH)
** @Function  : 获取电池组额定容量
** @The notes :
**===========================================================================================*/
u16 GetGroupRatedCapAPI(void)
{
    return(sFadeForm.ratedCap);
}

/*=============================================================================================
** @Name      : u16 GetGroupFadeCycleAPI(void)
** @Input     : void
** @Output    : 电池实际衰减循环次数(1次)
** @Function  : 获取电池组实际衰减循环次数
** @The notes :
**===========================================================================================*/
u16 GetGroupFadeCycleAPI(void)
{
    return(sFadeForm.fadeCycle);
}

/*=============================================================================================
** @Name      : u16 GetGroupRatedCycleAPI(void)
** @Input     : void
** @Output    : 电池额定循环次数(1次)
** @Function  : 获取电池组额定循环次数
** @The notes :
**===========================================================================================*/
u16 GetGroupRatedCycleAPI(void)
{
    return(sFadeForm.ratedCycle);
}

/*=============================================================================================
** @Name      : u16 GetGRealSohMilliAPI(void)
** @Input     : void
** @Output    : 电池组当前健康度SOH值(0.1%)
** @Function  : 获取电池组当前健康度SOH值
** @The notes : 分辨率千分之一
**===========================================================================================*/
u16 GetGRealSohMilliAPI(void)
{
	u16 soh = 0;

	if(sFadeForm.ratedCap > 0)
	{
		//soh = (u32)sFadeForm.fadeCap * 1000 / sFadeForm.ratedCap;
		soh = ((u32)sFadeForm.fadeCap * 10000 / sFadeForm.ratedCap + 5) / 10;

		if(soh > 1000)
		{
			soh = 1000;
		}

		return(soh);
	}
	else
	{
		return(0);
	}
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CalcGroupCellFadeHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 计算电池组电池衰减状态
** @The notes :
**===========================================================================================*/
static void CalcGroupCellFadeHandle(void)
{
	u16 capSoh = 0;
	u16 timSoh = 0;

	/*充放电次数变化则计算一次SOH*/
	if((0 == sFadeForm.fadeCycle) || (GetGChgCycleTimesAPI() != sFadeForm.fadeCycle))
	{
		sFadeForm.fadeCycle = GetGChgCycleTimesAPI();

		/*通过实际总容量计算的SOH*/
		if(GetGroupRatedCapAPI() > 0)
		{
			/*使用出厂额定容量(0.1AH)*/
			capSoh = (u16)(GetGroupTotalCapAPI() * 10 / GetGroupRatedCapAPI());

			if(capSoh > 1000)
			{
				capSoh = 1000;
			}
		}
		else
		{
			capSoh = 0;
		}

		/*额定充放电循环次数有效*/
		if(sFadeForm.ratedCycle > 2)
		{
			/*通过充放电次数计算的SOH*/
			if(sFadeForm.ratedCycle >= sFadeForm.fadeCycle)
			{
				/*额定充放电次数*/
				timSoh = 1000 - (sFadeForm.fadeCycle * 200 / sFadeForm.ratedCycle);
			}
			else
			{
				timSoh = 800;
			}
		}
		/*额定充放电次数清零*/
		else
		{
			/*仅使用容量计算SOH*/
			timSoh = capSoh;
		}

		/*判断两个方式计算的SOH误差*/
		if(ABS(capSoh, timSoh) <= 50)
		{
			/*误差小则认为当前总容量可信*/
			//sFadeForm.fadeCap = GetGroupTotalCapAPI() / 100;
			/*误差小以充放电次数为准(防止跳变)*/
			capSoh = timSoh;
		}
		else
		{
			/*误差大则求平均值计算衰减容量*/
			capSoh = (capSoh + timSoh) / 2;
		}

		/*额定充放电循环次数有效时防止显示值跳变*/
		if(sFadeForm.ratedCycle > 1)
		{
			/*SOH不能增大*/
			if(capSoh > GetGBattSohAPI())
			{
				capSoh = GetGBattSohAPI();
			}
			/*SOH变化超过0.5%*/
			else if((capSoh + 5) < GetGBattSohAPI())
			{
				/*当前SOH在强制显示100%范围*/
				if(GetGBattSohAPI() >= 994)
				{
					/*防止被强制显示100%*/
					capSoh = 993;
				}
				else
				{
					/*SOH每次最大下降0.5%*/
					capSoh = GetGBattSohAPI() - 5;
				}
			}
		}

		/*防止上位机只显示1%刻度时新电池99.9%显示99%*/
		if(capSoh >= 994)
		{
			/*99.4%以上显示100%*/
			capSoh = 1000;
		}

		/*反算当前SOH对应衰减容量(四舍五入防止精度丢失显示SOH越来越小)*/
		sFadeForm.fadeCap = (GetGroupRatedCapAPI() * capSoh + 500) / 1000;

		/*SOH变化则记录*/
		if(capSoh != GetGBattSohAPI())
		{
			EnerChangEepGNowCapSohHook(capSoh);
		}
	}
}
