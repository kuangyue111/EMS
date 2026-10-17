/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : CharactCalc.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 电池特征信息计算模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "EnerInterface.h"
#include "PublicInfo.h"
#include "CalcInterface.h"
#include "CharactCalc.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern s16 gGSysMainInfo_60[];				/*系统整体信息数组*/
extern u16 gGSysCapInfo_61[];				/*系统电量信息数组*/
extern u16 gGSysCellInfo_62[]; 				/*系统单体信息数组*/
extern u16 gGVoltCharInfo_63[]; 			/*电压特征信息数组*/
extern s16 gGTempCharInfo_64[]; 			/*温度特征信息数组*/

extern u16 gGCellVoltInfo_92[];				/*电池单体电压信息数组*/
extern s8 gGCellTempInfo_94[];				/*电池单体温度信息数组*/

extern s8 gGCellBalanTInfo_201[];			/*均衡温度特征信息*/

extern u16 gGBmuGenPara_102[];  			/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];  			/*主控高级参数值*/
extern u8 gGHardPara_104[]; 		        /*主控硬件参数配置信息*/

static s16 sClusAvgCurr = 0;            	/*电池组平均电流(0.1A)*/

static s32 sAvgCurTbl[CUR_FIT_LEN] = {0};	/*求平均电流的电流表*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void GroupDataInfoInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池数据信息初始化
** @The notes :
**===========================================================================================*/
void GroupDataInfoInit(void)
{
    u16 i = 0;
    
    for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum]; i++)
    {
    	gGCellVoltInfo_92[i] = DEFAULT_STANDCV;     /*初始化单节电压数组*/
    }
    
    for(i = 0; i < (gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_BTSumNum]); i++)
    {
    	gGCellTempInfo_94[i] = 25;                  /*初始化单节温度数组 */
    }
}

/*=============================================================================================
** @Name      : void CheckVoltCharaTask(void)
** @Input     : void
** @Output    : void
** @Function  : 检测电压特征信息任务
** @The notes : 100ms
**===========================================================================================*/
void CheckVoltCharaTask(void)
{
    u16 num = 0;
    u16 maxNum = 0;
    u16 minNum = 0;
    u16 nowVolt = 0;
    u16 maxVolt = 0;
    u16 minVolt = 0xffff;
    u32 sumVolt = 0;

    //记录总电压采样值和预充电压采样值
    SampSumVoltFilterDeal(CalcGetDcSumSampVolt());
    OutPreVoltFilterDeal(CalcGetDcPreSampVolt());
    
    /*系统初始化时信息不齐全不检测最值*/
    if((eWORK_INIT == GetGWorkStateAPI())           /*初始化时信息不齐全不检测*/
        && ((2 != DiagGetRcvAllSlaveFlagAPI())      /*未接收到全部从控信息*/
        || (2 != DiagGetRcvAllCellFlagAPI())))      /*未接收到全部单体信息*/
    {
    	return;
    }

	for(num = 0; num < gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum]; num++)
	{
		nowVolt = gGCellVoltInfo_92[num];

		sumVolt += nowVolt;                         /*计算总电压 */

		if(maxVolt < nowVolt)                       /*求单体最大电压和电池号*/
		{
			maxVolt = nowVolt;
			maxNum = num;
		}

		if(minVolt > nowVolt)                       /*求单体最小电压和电池号*/
		{
			minVolt = nowVolt;
			minNum = num;
		}
	}

	gGVoltCharInfo_63[eVoltCharInfo63_MaxVolt]  = maxVolt;      /*组最大电压*/
	gGVoltCharInfo_63[eVoltCharInfo63_MinVolt] = minVolt;       /*组最小电压*/
	gGVoltCharInfo_63[eVoltCharInfo63_MaxVNum] = maxNum + 1;    /*组电压最大电池号(从1开始)*/
	gGVoltCharInfo_63[eVoltCharInfo63_MinVNum] = minNum + 1;    /*组电压最小电池号(从1开始)*/

	gGSysCellInfo_62[eSysCellInfo62_SumV] = (u16)(sumVolt / 100);/*组总电压单位为0.1V*/

	if(0 == CalcGetGroupCellVExpStateHook())               		/*从控单体正常才计算总电压和平均电压*/
	{
		//gGSysCellInfo_62[eSysCellInfo62_SumV] = (u16)(sumVolt / 100);/*组总电压单位为0.1V*/
		gGSysCellInfo_62[eSysCellInfo62_AvgV] = (u16)(sumVolt / gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum]);/*单节电池平均电压,单位mV*/
	}
	else
	{
		//gGSysCellInfo_62[eSysCellInfo62_SumV] = 0;            /*组总电压单位为0.1V*/
		gGSysCellInfo_62[eSysCellInfo62_AvgV] = 0;		        /*单节电池平均电压,单位mV*/
	}
}

/*=============================================================================================
** @Name      : void CheckTempCharaTask(void)
** @Input     : void
** @Output    : void
** @Function  : 检测温度特征信息任务
** @The notes : 100ms
**===========================================================================================*/
void CheckTempCharaTask(void)
{
    u8 id = 0;
    u8 cell = 0;
    u16 num = 0;
    u16 maxNum = 0;
    u16 minNum = 0;
    s16 nowTemp = 0;
    s16 maxTemp = -41;
    s16 minTemp = 127;
    s32 sumTemp = 0;
    
    /*系统初始化时信息不齐全不检测最值*/
    if((eWORK_INIT == GetGWorkStateAPI())           /*初始化时信息不齐全不检测*/
        && ((2 != DiagGetRcvAllSlaveFlagAPI())      /*未接收到全部从控信息*/
        || (2 != DiagGetRcvAllCellFlagAPI())))      /*未接收到全部单体信息*/
    {
    	return;
    }

    /*检测单体温度*/
	for(num = 0; num < gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum]; num++)
	{
		nowTemp = gGCellTempInfo_94[num];

		sumTemp += nowTemp;                         /*温度求和*/

		if(maxTemp < nowTemp)                       /*求单体最大温度和电池号*/
		{
			maxTemp = nowTemp;
			maxNum = num;
		}

		if(minTemp > nowTemp)                       /*求单体最小温度和电池号*/
		{
			minTemp = nowTemp;
			minNum = num;
		}
	}

	gGTempCharInfo_64[eTempCharInfo64_MaxTemp] = maxTemp;      /*组最大温度*/
	gGTempCharInfo_64[eTempCharInfo64_MinTemp] = minTemp;      /*组最小温度*/
	gGTempCharInfo_64[eTempCharInfo64_MaxTNum] = maxNum + 1;   /*组温度最大电池号(从1开始)*/
	gGTempCharInfo_64[eTempCharInfo64_MinTNum] = minNum + 1;   /*组温度最小电池号(从1开始)*/

	if(0 == CalcGetGroupCellTExpStateHook())               	   /*从控单体温度正常才计算平均温度*/
	{
		//四舍五入求组平均温度
		gGSysCellInfo_62[eSysCellInfo62_AvgT] = (s16)(((sumTemp * 10 / gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum]) + 5) / 10);
	}
	else
	{
		//显示异常值
		gGSysCellInfo_62[eSysCellInfo62_AvgT] = -41;
	}

    /*检测均衡温度(逐个从控检测)*/
	for(id = 0; id < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; id++)
	{
	    /*用作组内最大温度*/
		sumTemp = -41;
		maxTemp = -41;

	    /*从控内温度*/
		for(cell = 0; cell < gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum]; cell++)
		{
			num = gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] + id * gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum] + cell;

			nowTemp = gGCellTempInfo_94[num];

		    /*求组内最大温度及对应从控号*/
			if(maxTemp < nowTemp)
			{
				maxTemp = nowTemp;
			}
		}

		gGCellBalanTInfo_201[eCellBalanT201_SMaxTemp0 + id] = maxTemp;

	    /*求组内最大温度及对应从控号*/
		if(sumTemp < maxTemp)
		{
			sumTemp = maxTemp;
			maxNum = id;
		}
	}

	gGCellBalanTInfo_201[eCellBalanT201_GMaxTemp] = (s8)sumTemp;
	gGCellBalanTInfo_201[eCellBalanT201_GMaxTSlav] = maxNum + 1;
}

/*=============================================================================================
** @Name      : void CheckAvgCurrTask(void)
** @Input     : void
** @Output    : void
** @Function  : 检测电池组平均电流任务
** @The notes : 100ms
**===========================================================================================*/
void CheckAvgCurrTask(void)
{
    s16 curr = 0;
    s32 inteCap = 0;
    s32 changCap = 0;
    static u16 sTime = 0;
    static s32 sHisCap = 0;
    
    if((sTime / 10) >= AVG_CURR_TIME)
    {
        inteCap = GetChgDhgChangCapAPI();   /*当前容量变化值*/
        
        changCap = inteCap - sHisCap;       /*间隔时间变化量(1mAH)*/
        
        curr = (s16)((changCap * 360) / (s16)sTime);
        
        sHisCap = inteCap;
        sTime = 0;
    }
    else
    {
        curr = sClusAvgCurr;
        sTime++;
    }
    sClusAvgCurr = curr;
}

/*=============================================================================================
** @Name      : void CurrFilterDeal(s32 curr, u8 flag)
** @Input     : cur:采样电流值(0.1A) flag:立即显示标志(1:立即显示)
** @Output    : void
** @Function  : 对电流进行滤波
** @The notes : 电流采样调用
**===========================================================================================*/
void CurrFilterDeal(s32 curr, u8 flag)
{
    u16 i = 0;
    s32 curSum = 0;
    s32 nowCurr = 0;
    s32 maxCurr = 0x80000000;
    s32 minCurr = 0x7fffffff;
    static u16 sLen = 0;
    static u16 sTime = 0;
    static s32 sAddCur = 0;
    
    /*不滤波*/
    if((0 == gGHardPara_104[eGHardPara104_CFilType]) || (1 == flag))
    {
         sLen = 0;
         sTime = 0;
         sAddCur = 0;
         nowCurr = curr;                    /*电流值*/
    }
    /*采样值滑动求平均滤波*/
    else if(1 == gGHardPara_104[eGHardPara104_CFilType])
    {
		/*滤波次数不合法*/
		if(gGHardPara_104[eGHardPara104_CFilTime] < 1)
		{
			gGHardPara_104[eGHardPara104_CFilTime] = 1;
		}
		else if(gGHardPara_104[eGHardPara104_CFilTime] > CUR_FIT_LEN)
		{
			gGHardPara_104[eGHardPara104_CFilTime] = CUR_FIT_LEN;
		}

        sAvgCurTbl[sTime] = curr;
        sTime++;
        if(sTime < gGHardPara_104[eGHardPara104_CFilTime])
        {
            /*未取满第一遍*/
            if(sLen < gGHardPara_104[eGHardPara104_CFilTime])
            {
                sLen = sTime;
            }
        }
        else
        {
            sLen = gGHardPara_104[eGHardPara104_CFilTime];
            sTime = 0;
        }

        for(i = 0; i < sLen; i++)
        {
            curSum += sAvgCurTbl[i];

            if(sAvgCurTbl[i] > maxCurr)
            {
                maxCurr = sAvgCurTbl[i];
            }

            if(sAvgCurTbl[i] < minCurr)
            {
                minCurr = sAvgCurTbl[i];
            }
        }

        if(sLen >= 4)
        {
        	if(curSum >= 0)
        	{
        		nowCurr = (((curSum - maxCurr - minCurr) * 10 / (sLen - 2)) + 5) / 10;
        	}
        	else
        	{
        		nowCurr = (((curSum - maxCurr - minCurr) * 10 / (sLen - 2)) - 5) / 10;
        	}
        }
        else
        {
        	if(curSum >= 0)
        	{
        		nowCurr = ((curSum * 10 / sLen) + 5) / 10;
        	}
        	else
        	{
        		nowCurr = ((curSum * 10 / sLen) - 5) / 10;
        	}
        }
    }
    /*采样值有效值滤波(暂不实现)*/
    else if(2 == gGHardPara_104[eGHardPara104_CFilType])
    {
        nowCurr = curr;
    }
    /*采样值积分求平均滤波*/
	else /*(3 == CURR_FIL_TYPE)*/
	{
		/*滤波次数不合法*/
		if(gGHardPara_104[eGHardPara104_CFilTime] < 1)
		{
			gGHardPara_104[eGHardPara104_CFilTime] = 1;
		}
		else if(gGHardPara_104[eGHardPara104_CFilTime] > CUR_FIT_LEN)
		{
			gGHardPara_104[eGHardPara104_CFilTime] = CUR_FIT_LEN;
		}

        sAddCur += curr;

        sTime++;
        if(sTime >= gGHardPara_104[eGHardPara104_CFilTime])
        {
            nowCurr = (s16)(((sAddCur * 10) / (s16)sTime + 5) / 10);
            sAddCur = 0;
            sTime = 0;
        }
        else
        {
            nowCurr = gGSysMainInfo_60[eSysMainInfo60_OutC];
        }
    }

    /*在显示零飘值内*/
    if((0 - (s8)gGHardPara_104[eGHardPara104_ChgShowPoint] < nowCurr)
        && (nowCurr < (s8)gGHardPara_104[eGHardPara104_DhgShowPoint]))
    {
        nowCurr = 0;
    }

    gGSysMainInfo_60[eSysMainInfo60_OutC] = (s16)nowCurr;
    //计算功率(0.1KW)
    gGSysMainInfo_60[eSysMainInfo60_OutP] = (s16)(nowCurr * (s32)gGSysCellInfo_62[eSysCellInfo62_SumV] / 10000);
}

/*=============================================================================================
** @Name      : void SampSumVoltFilterDeal(u16 volt)
** @Input     : volt:采样电压值(0.1V)
** @Output    : void
** @Function  : 对电池端总电压采样值进行滤波
** @The notes : 电池输出端总电压采样值处理调用
**===========================================================================================*/
void SampSumVoltFilterDeal(u16 volt)
{
    u32 nowVolt = 0;

    //变化值在0.3V—10.0V之间
    if((ABS(gGSysMainInfo_60[eSysMainInfo60_SumV], volt) >= 3)
        && (ABS(gGSysMainInfo_60[eSysMainInfo60_SumV], volt) <= 100))
    {
        nowVolt = ((u32)gGSysMainInfo_60[eSysMainInfo60_SumV] * 12 + (u32)volt * 20) / 32;
    }
    else
    {
        nowVolt = volt;
    }

    gGSysMainInfo_60[eSysMainInfo60_SumV] = (u16)nowVolt;
}

/*=============================================================================================
** @Name      : void OutPreVoltFilterDeal(u16 volt)
** @Input     : volt:采样电压值(0.1V)
** @Output    : void
** @Function  : 对输出端预充电压采样值进行滤波
** @The notes : 电池输出端预充电压采样值处理调用
**===========================================================================================*/
void OutPreVoltFilterDeal(u16 volt)
{
    u32 nowVolt = 0;
    
    //变化值在0.3V—10.0V之间
    if((ABS(gGSysMainInfo_60[eSysMainInfo60_PreV], volt) >= 3)
        && (ABS(gGSysMainInfo_60[eSysMainInfo60_PreV], volt) <= 100))
    {
        nowVolt = ((u32)gGSysMainInfo_60[eSysMainInfo60_PreV] * 12 + (u32)volt * 20) / 32;
    }
    else
    {
        nowVolt = volt;
    }
    
    gGSysMainInfo_60[eSysMainInfo60_PreV] = (u16)nowVolt;
}

/*=============================================================================================
** @Name      : s16 GetGroupAvgCurrAPI(void)
** @Input     : void
** @Output    : 电池组平均电流值(单位:0.1A)
** @Function  : 获取电池组平均电流值
** @The notes :
**===========================================================================================*/
s16 GetGroupAvgCurrAPI(void)
{
    return(sClusAvgCurr);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
