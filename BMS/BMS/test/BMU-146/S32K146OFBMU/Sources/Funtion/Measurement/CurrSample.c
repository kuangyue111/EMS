/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : CurrSample.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 电流采样模块
** @Instructions : 同时使能霍尔和分流器时以分流器为主,霍尔作为参考
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "ParaInterface.h"
#include "CalcInterface.h"
#include "EnerInterface.h"
#include "RecordInterface.h"
#include "TacUser.h"
#include "OutCurrCorr.h"
#include "HigAFESamp.h"
#include "LowADCSamp.h"
#include "CurrSample.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gGCalibrResult_83[];		    /*校准结果状态信息数组*/
extern u16 gGBmuGenPara_102[];          /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];       /*主控高级参数值*/
extern u8 gGHardPara_104[]; 		    /*主控硬件参数配置信息*/

static u8 sCSampExpFlag = 0;            /*电流采样异常标志 bit0:霍尔5V异常 bit1:霍尔回检信号异常 bit2:模拟前端采样异常 bit3:霍尔ADC通道异常 bit4:霍尔AD值异常 bit5:分流器AD值异常 bit6:电流校零异常[0:正常 1:异常]*/
static s32 sRealCurr10mA = 0;           /*实时瞬时电流采样值 10mA*/
static u32 sRealVolt10uV = 0;           /*实时瞬时电压采样值 0.01mV*/

static s16 sCurrZeroMax = 0;            /*电流零点最大值(10mA)*/
static s16 sCurrZeroMin = 0;            /*电流零点最大值(10mA)*/
static s16 sCurrZeroCorr = 0;           /*电流零点修正值(10mA)*/
static u8 sCurrZeroFin = 0;             /*电流零点修正完成标志 1完成 2失败*/

/*同时使能霍尔和分流器时以分流器为主,霍尔作为参考单独记录*/
static s32 sHallRealCurr10mA = 0;       /*霍尔实时瞬时电流采样值 10mA*/
static s32 sHallRealVolt10uV = 0;       /*实时瞬时霍尔电压采样值 0.01mV*/
static s16 sHallZeroCorr = 0;           /*霍尔电流零点修正值(10mA)*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CalcCurrZeroCorr(s32 curr)
** @Input     : curr:空闲时采样电流零飘值 单位:1mA
** @Output    : void
** @Function  : 计算电流零飘值
** @The notes : 必须在高压接触器全断开(无电流)时调用
**===========================================================================================*/
static void CalcCurrZeroCorr(s32 curr);

/*=============================================================================================
** @Name      : static void InputSampRealCurr10mA(s32 curr)
** @Input     : curr:实际电流 10mA
** @Output    : void
** @Function  : 输出记录实际电流
** @The notes : 记录未校准的电流
**===========================================================================================*/
static void InputSampRealCurr10mA(s32 curr);

/*=============================================================================================
** @Name      : static void InputCSampRealVolt10uV(s32 volt)
** @Input     : volt:实际电压 0.01mV
** @Output    : void
** @Function  : 输出记录电流采样实际电压值
** @The notes :
**===========================================================================================*/
static void InputCSampRealVolt10uV(s32 volt);

/*=============================================================================================
** @Name      : static void CheckShuntHallCurrDiff(s32 shuntCurr, s32 hallCurr)
** @Input     : shuntCurr:分流器电流采样值 10mA hallCurr:霍尔电流采样值 10mA
** @Output    : void
** @Function  : 判断分流器和霍尔两个通道采样值是否一致
** @The notes : 排除电流变化俩通道采样不同步采样值可能不同
**===========================================================================================*/
static void CheckShuntHallCurrDiff(s32 shuntCurr, s32 hallCurr);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void CurrSampleInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电流信息采样初始化
** @The notes :
**===========================================================================================*/
void CurrSampleInit(void)
{
	u16 zeroCurr[2] = {0};

	/*读取电流零点误差*/
    if(TRUE == ParaReadStoreCorrZeroCurr(zeroCurr, 2))
    {
    	/*电流零点误差*/
	    sCurrZeroCorr = zeroCurr[1];
    }

    sCurrZeroMax = (s16)gGHardPara_104[eGHardPara104_DhgIntPoint];
    sCurrZeroMin = (0 - (s16)gGHardPara_104[eGHardPara104_ChgIntPoint]);

    sCurrZeroFin = 0;
}

/*=============================================================================================
** @Name      : void ISRCurrCapAddTask(void)
** @Input     : void
** @Output    : void
** @Function  : 定时中断电流积分任务
** @The notes : 必须1ms定时中断调用
**===========================================================================================*/
void ISRCurrCapAddTask(void)
{
    /*使能定时中断积分*/
    #if(1 == CURR_INT_ADD_EN)
    /*放电积分*/
    if((0 == SampGetCurrSampExpFlagAPI(eCCHAN_Num))                 /*电流采样正常*/
        && (sRealCurr10mA > sCurrZeroMax)
        && (sRealCurr10mA >= (s16)gGHardPara_104[eGHardPara104_DhgIntPoint]))
    {
        DhgCapIntTask(sRealCurr10mA, 1);

        /*单体采样正常用单体累计总压*/
        if(ABS(GetGCellSumVoltAPI(), GetGSampSumVoltAPI()) <= gGBmuHigLevPara_103[eBmuHigLevPara103_SumDiffV])
		{
			DhgEnerIntTask(sRealCurr10mA, GetGCellSumVoltAPI(), 1);
		}
        /*单体采样异常用单体采样总压*/
        else
		{
			DhgEnerIntTask(sRealCurr10mA, GetGSampSumVoltAPI(), 1);
		}
    }
    /*充电积分(正值)*/
    else if((0 == SampGetCurrSampExpFlagAPI(eCCHAN_Num))            /*电流采样正常*/
        && (sRealCurr10mA < sCurrZeroMin)
        && (sRealCurr10mA <= (0 - (s16)gGHardPara_104[eGHardPara104_ChgIntPoint])))
    {
        ChgCapIntTask((0 - sRealCurr10mA), 1);

        /*单体采样正常用单体累计总压*/
        if(ABS(GetGCellSumVoltAPI(), GetGSampSumVoltAPI()) <= gGBmuHigLevPara_103[eBmuHigLevPara103_SumDiffV])
		{
        	ChgEnerIntTask((0 - sRealCurr10mA), GetGCellSumVoltAPI(), 1);
		}
        /*单体采样异常用单体采样总压*/
        else
		{
        	ChgEnerIntTask((0 - sRealCurr10mA), GetGSampSumVoltAPI(), 1);
		}
    }
    #endif
}

/*=============================================================================================
** @Name      : void CalcSampleADToCurr(u8 channel, s32 currAD)
** @Input     : channel:采样通道(0主板霍尔 1主板分流器) currAD:电流AD
** @Output    : void
** @Function  : 将采样AD值转化为电流值
** @The notes : 高压模块采电流时输入为电流值0.01A
**===========================================================================================*/
void CalcSampleADToCurr(u8 channel, s32 currAD)
{
    u8 flag = 0;       	        /*电流突变标志*/
    s32 currVolt = 0;       	/*电流对应的采样电压值*/
    s32 curr10mA = 0;       	/*主回路采样电流(带零飘)*/
    s32 realCurr10mA = 0;   	/*主回路实际电流(无零飘)*/
    static s32 sHisCurr10mA = 0;/*上次采样电流值(无零飘)*/

    /*主板BMU采集霍尔电流*/
    if(eCADC_Hall == channel)
    {
        /*使能霍尔采集电流*/
        if(1 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCADC_Hall))
        {
			if(ePSTATE_OFF == DEVGPIOGetInPortState(ePIn_HALFAU))       /*霍尔过载故障(低电平)*/
			{
				SetCurrSampExpFlagAPI(eCExp_HalErr, 1, currAD);         /*记录霍尔过载故障*/

				/*同时使能霍尔和分流器通道时不清零(测试程序要显示异常值不清零)*/
				if(0 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))
				{
					InputGroupLVSideData(eBMU_ADC_HalSIG, 0);           /*霍尔采样值清零*/
					InputSampRealCurr10mA(0);                           /*清除电流采样实际电流值 0.01A*/
					InputCSampRealVolt10uV(0);                          /*清除电流采样实际电压值 0.01mV*/
					CurrFilterDeal(0, 1);                               /*输出电流单位0.1A*/
				}
				return;
			}
			else if((GetGLowHall5VVoltAPI() <= CURR_HALL5V_MINV)        /*霍尔5V供电异常*/
				&& (1 == GetADCLVSampFinState()))                       /*霍尔5V已采样*/
			{
				SetCurrSampExpFlagAPI(eCExp_HalOff, 1, currAD);         /*记录霍尔供电异常*/

				/*同时使能霍尔和分流器通道时不清零(测试程序要显示异常值不清零)*/
				if(0 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))
				{
					InputGroupLVSideData(eBMU_ADC_HalSIG, 0);           /*霍尔采样值清零*/
					InputSampRealCurr10mA(0);                           /*清除电流采样实际电流值 0.01A*/
					InputCSampRealVolt10uV(0);                          /*清除电流采样实际电压值 0.01mV*/
					CurrFilterDeal(0, 1);                               /*输出电流单位0.1A*/
				}
				return;
			}
			//else if((CURR_HALL_EXP_AD == currAD) || (currAD <= 0))    /*采样AD值异常(包括未使能)*/
			else if((currAD <= CURR_HALL_MIN_AD)                        /*采样AD值异常(包括未使能)*/
				|| (currAD >= CURR_HALL_MAX_AD))
			{
				SetCurrSampExpFlagAPI(eCExp_HADErr, 1, currAD);         /*记录AD值异常*/

				/*同时使能霍尔和分流器通道时不清零(测试程序要显示异常值不清零)*/
				if(0 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))
				{
					InputGroupLVSideData(eBMU_ADC_HalSIG, 0);           /*霍尔采样值清零*/
					InputSampRealCurr10mA(0);                           /*清除电流采样实际电流值 0.01A*/
					InputCSampRealVolt10uV(0);                          /*清除电流采样实际电压值 0.01mV*/
					CurrFilterDeal(0, 1);                               /*输出电流单位0.1A*/
				}
				return;
			}
			else if(SampGetCurrSampExpFlagAPI(eCADC_Hall) > 0)          /*已记录霍尔异常*/
			{
				SetCurrSampExpFlagAPI(eCExp_HalOff, 0, currAD);         /*清除霍尔供电异常*/
				SetCurrSampExpFlagAPI(eCExp_HalErr, 0, currAD);         /*清除霍尔过载故障*/
				SetCurrSampExpFlagAPI(eCExp_HADErr, 0, currAD);         /*清除AD值异常*/
			}

			currVolt = (u16)((u32)currAD * 3300 / 4096);                /*AD转化为采样电压(0V~5V,1mv),V=AD*3.3V/4096*/
			//currVolt = currVolt * 136;                                /*Y=37.5*X/27.5, 0.01mV(4.5V量程)*/
			currVolt = currVolt * 150;                                  /*Y=1.5*X, 0.01mV(4.9V量程)*/

			currVolt = CorrRealHalVoltByTemp(currVolt);                 /*通过温度修正电路温升电压偏移*/

			currVolt = CorrRealHalCurrByKB(currVolt);                   /*通过霍尔KB值校准电流对应电压*/

			//curr10mA = (currVolt - 250000) / 10;                      /*电压转化为采样电流(0.5~4.5V,-200~200A),1mV:100mA*/
			curr10mA = (currVolt - ((s32)gGHardPara_104[eGHardPara104_HallSign] * 10000)) * gGHardPara_104[eGHardPara104_HallRang] / gGHardPara_104[eGHardPara104_HallRate] / 10;/*电压转化为采样电流(0.5~4.5V,-200~200A),1mV:100mA*/
        }
        /*禁用霍尔采集电流*/
        else
        {
            return;
        }
    }
    /*主板BMU采集分流器电流*/
    else if(eCAFE_Shunt == channel)
    {
        /*使能分流器采集电流*/
        if(1 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))
        {
			//if((CURR_SHUNT_EXP_AD == currAD) || (0x7fff == currAD))   /*采样AD异常(0x8000负值越界,0x7fff正值最大(断线))*/
			if((currAD <= CURR_SHUNT_MIN_V) || (currAD >= CURR_SHUNT_MAX_V)) /*采样AD值越界异常*/
			{
				SetCurrSampExpFlagAPI(eCExp_SADErr, 1, currAD);         /*记录分流器采样值异常*/

				/*未使能霍尔作为备选或霍尔采样也异常*/
				if((0 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCADC_Hall))
					|| (SampGetCurrSampExpFlagAPI(eCADC_Hall) > 0))
				{
					//采样值清零
					InputGroupHVSideData(HIG_AFE_Shunt, 0);             /*分流器采样值清零(测试程序要显示异常值不清零)*/
					InputSampRealCurr10mA(0);                           /*清除电流采样实际电流值 0.01A*/
					InputCSampRealVolt10uV(0);                          /*清除电流采样实际电压值 0.01mV*/
					CurrFilterDeal(0, 1);                               /*输出电流单位0.1A*/
				}
				return;
			}
			else if(SampGetCurrSampExpFlagAPI(eCAFE_Shunt) > 0)         /*已记录分流器异常*/
			{
				SetCurrSampExpFlagAPI(eCExp_SADErr, 0, currAD);         /*清除AD值异常*/
			}

			//currVolt = currAD * 3 / 4;                                /*输入即为采样电压(-125mV~125mV,0.01mV)*/
			currVolt = currAD;                                          /*输入即为采样电压(-75mV~75mV,0.01mV)*/

			currVolt = CorrRealShuCurrByKB(currVolt);                   /*通过分流器KB值校准电流对应电压*/

			//curr10mA = currVolt * 300 / 75;                           /*电压转化为采样电流((-75~75mV,-300~300A),1mV:4A*/
			curr10mA = currVolt * (s32)gGHardPara_104[eGHardPara104_ShuntRang] * 10 / gGHardPara_104[eGHardPara104_ShuntRate];/*电压转化为采样电流(±75mV对应-300A~300A)*/
        }
        /*禁用分流器采集电流*/
        else
        {
            return;
        }
    }
    /*高压模块HVCU分流器采集电流*/
    else
    {
        /*输入AD值即为电流值*/
        curr10mA = currAD;
    }

    /*外部供电使能自动校准零飘*/
    #if(1 == CURR_AUT_ZERO_EN)

    /*优先分流器采样或无分流器通道则霍尔通道使用*/
    if((eCAFE_Shunt == channel)                                         /*优先分流器使用*/
        || ((eCADC_Hall == channel)                                     /*无分流器采样时霍尔使用*/
        && (0 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))))
    {
		if(((TRUE == TacGetGroupRunSwhAllOffStateAPI())                 /*电池组运行接触器都断开*/
			&& (eMODE_DEBUG != GetGWorkModeAPI())                       /*非调试模式(避免电流校准小电流也校零)*/
			&& ((eWORK_ERR != GetGWorkStateAPI())                       /*非故障状态(避免单板调试小电流也校零)*/
			|| (1 == BitGet(sCSampExpFlag, eCExp_ZroErr))))             /*电流校零失败故障*/
			|| (eCORR_HANDZRO == gGCalibrResult_83[eCorrRes83_CorrType]))/*手动电流校零*/
		{
			CalcCurrZeroCorr(curr10mA * 10);                            /*计算电流零飘值*/

			if(0 == sCurrZeroFin)                                       /*未完成电流零飘值计算*/
			{
				if(eWORK_INIT == GetGWorkStateAPI())                    /*系统自检状态*/
				{
					curr10mA = sCurrZeroCorr;                           /*强制真实电流为0*/
				}
			}
		}

        realCurr10mA = curr10mA - sCurrZeroCorr;                        /*去除电流零飘值*/
    }
    /*使能两个通道增加霍尔通道另外校准*/
    else                                                                /*有分流器时霍尔采样另外使用*/
    {
        if(((1 == TacGetGroupRunSwhAllOffStateAPI())                    /*电池组运行接触器都断开*/
            && (eMODE_DEBUG != GetGWorkModeAPI())                       /*非调试模式(避免电流校准小电流也校零)*/
            && ((eWORK_ERR != GetGWorkStateAPI())                       /*非故障状态(避免单板调试小电流也校零)*/
            || (1 == BitGet(sCSampExpFlag, eCExp_ZroErr))))             /*电流校零失败故障*/
            || (eCORR_HANDZRO == gGCalibrResult_83[eCorrRes83_CorrType]))/*手动电流校零*/
        {
            if(0 == sHallZeroCorr)                                      /*未取霍尔电流零飘值*/
            {
                if(eWORK_INIT == GetGWorkStateAPI())                    /*系统自检状态*/
                {
                    sHallZeroCorr = curr10mA;                           /*取霍尔电流零飘值*/
                }
            }
            else if(eWORK_INIT == GetGWorkStateAPI())                   /*已取零飘值但系统自检状态*/
            {
                sHallZeroCorr = (s16)((((s32)sHallZeroCorr * 20) + ((s32)curr10mA * 12)) / 32);/*取霍尔电流平均零飘值*/
            }
        }

        realCurr10mA = curr10mA - sHallZeroCorr;                        /*去除电流零飘值*/

        sHallRealCurr10mA = realCurr10mA;                               /*另外记录霍尔采样值(只对外显示分流器采样值)*/
        sHallRealVolt10uV = currVolt;                                   /*另外记录霍尔采样值(只对外显示分流器采样值)*/

        /*当前分流器正常则不使用霍尔采样值积分和显示*/
        if(0 == SampGetCurrSampExpFlagAPI(eCAFE_Shunt))
        {
            /*判断两个通道采样值是否一致(排除电流变化俩通道采样不同步采样值可能不同)*/
        	CheckShuntHallCurrDiff(sRealCurr10mA, sHallRealCurr10mA);
        	return;
        }
    }

    /*从电池取电不能自动校准零飘*/
    #else
    sCurrZeroMax = (s16)gGHardPara_104[eGHardPara104_DhgIntPoint];
    sCurrZeroMin = (0 - (s16)gGHardPara_104[eGHardPara104_ChgIntPoint]);
    sCurrZeroFin = 1;

    /*优先分流器采样或无分流器通道则霍尔通道使用*/
	if((eCAFE_Shunt == channel)                                         /*优先分流器使用*/
		|| ((eCADC_Hall == channel)                                     /*无分流器采样时霍尔使用*/
		&& (0 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))))
    {
        realCurr10mA = curr10mA - sCurrZeroCorr;                        /*去除默认电流零飘值*/
    }
	/*双通道使能时霍尔通道*/
	else
	{
        realCurr10mA = curr10mA - sHallZeroCorr;                        /*去除电流零飘值*/

        sHallRealCurr10mA = realCurr10mA;                               /*另外记录霍尔采样值(只对外显示分流器采样值)*/
        sHallRealVolt10uV = currVolt;                                   /*另外记录霍尔采样值(只对外显示分流器采样值)*/

        /*当前分流器正常则不使用霍尔采样值积分和显示*/
        if(0 == SampGetCurrSampExpFlagAPI(eCCHAN_Shunt))
        {
        	/*判断两个通道采样值是否一致(排除电流变化俩通道采样不同步采样值可能不同)*/
        	CheckShuntHallCurrDiff(sRealCurr10mA, sHallRealCurr10mA);
        	return;
        }
	}
    #endif

    /*放电积分*/
    if((0 == SampGetCurrSampExpFlagAPI(eCCHAN_Num))                 /*电流采样正常*/
        && (realCurr10mA > sCurrZeroMax)
        && (realCurr10mA >= (s16)gGHardPara_104[eGHardPara104_DhgIntPoint])  /*当前电流符合积分条件*/
        && ((realCurr10mA >= (s16)gGHardPara_104[eGHardPara104_DhgShowPoint])/*当前电流符合显示条件*/
        || (sHisCurr10mA >= (s16)gGHardPara_104[eGHardPara104_DhgIntPoint])))/*或上次电流符合积分条件(排除偶然跳动的电流)*/
    {
        sHisCurr10mA = realCurr10mA;                                /*记录上次采样值*/

        /*未使能定时中断积分*/
        #if(0 == CURR_INT_ADD_EN)
        DhgCapIntTask(realCurr10mA, 1);
        DhgEnerIntTask(realCurr10mA, GetGSampSumVoltAPI(), 1);
        #endif
    }
    /*充电积分(正值)*/
    else if((0 == SampGetCurrSampExpFlagAPI(eCCHAN_Num))            /*电流采样正常*/
        && (realCurr10mA < sCurrZeroMin)
        && (realCurr10mA <= (0 - (s16)gGHardPara_104[eGHardPara104_ChgIntPoint]))  /*当前电流符合积分条件*/
        && ((realCurr10mA <= (0 - (s16)gGHardPara_104[eGHardPara104_ChgShowPoint]))/*当前电流符合显示条件*/
        || (sHisCurr10mA <= (0 - (s16)gGHardPara_104[eGHardPara104_ChgIntPoint]))))/*或上次电流符合积分条件(排除偶然跳动的电流)*/
    {
        sHisCurr10mA = realCurr10mA;                                /*记录上次采样值*/

        /*未使能定时中断积分*/
        #if(0 == CURR_INT_ADD_EN)
        ChgCapIntTask((0 - realCurr10mA), 1);
        ChgEnerIntTask((0 - realCurr10mA), GetGSampSumVoltAPI(), 1);
        #endif
    }
    else
    {
        sHisCurr10mA = realCurr10mA;                                /*记录上次采样值*/

        if(0 == SampGetCurrSampExpFlagAPI(eCCHAN_Num))              /*电流采样正常*/
        {
            realCurr10mA = 0;
        }
    }

	/*判断是否突变(本次采样超过极限值但上次未过流)*/
	if(((realCurr10mA >= ((s32)BATT_EXEREM_MAXC * 10))
		|| (realCurr10mA <= ((0 - (s32)BATT_EXEREM_MAXC) * 10)))
		&& (sRealCurr10mA > (0 - (s32)gGBmuGenPara_102[eBmuGenPara102_ChgCH1Lim]))
		&& (sRealCurr10mA < gGBmuGenPara_102[eBmuGenPara102_DhgCH1Lim]))
	{
		flag = 1;                                               /*电流突变则直接显示采样值(用于过流诊断)*/

		/*放电*/
		if(realCurr10mA >= ((s32)BATT_EXEREM_MAXC * 10))
		{
			/*放电过流一级告警故障录波*/
			RcdSetFaultWaveTrigAPI(eFatWavSort_DCO1);
		}
		else
		{
			/*充电过流一级告警故障录波*/
			RcdSetFaultWaveTrigAPI(eFatWavSort_CCO1);
		}
	}

	InputSampRealCurr10mA(realCurr10mA);                        /*输入电流采样实际电流值 0.01A*/
	InputCSampRealVolt10uV(currVolt);                           /*输入电流采样实际电压值 0.01mV*/

    /*显示电流采样值(包括滤波显示)*/
    if(realCurr10mA < 0)
    {
        CurrFilterDeal((realCurr10mA - 5) / 10, flag);              /*输出电流单位0.1A*/
    }
    else
    {
        CurrFilterDeal((realCurr10mA + 5) / 10, flag);              /*输出电流单位0.1A*/
    }
}

/*=============================================================================================
** @Name      : void ClrCurrZeroFinFlagAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除电流零点校零标志
** @The notes : 重新校零
**===========================================================================================*/
void ClrCurrZeroFinFlagAPI(void)
{
    sCurrZeroFin = 0;
    sCurrZeroCorr = 0;
    sHallZeroCorr = 0;
}

/*=============================================================================================
** @Name      : void SetCurrSampExpFlagAPI(u8 id, u8 state, s16 currAD)
** @Input     : id:电流采样异常ID(0:霍尔供电5V异常 1:霍尔回检信号异常 2:霍尔ADC通道异常 3:霍尔AD值异常 4:分流器模拟前端异常 5:分流器AD值异常 6:电流校零异常 7:俩通道采样值误差异常 sate:设置状态[0:正常 1:异常]) currAD:读取到的AD值
** @Output    : void
** @Function  : 设置电流采样异常标志
** @The notes :
**===========================================================================================*/
void SetCurrSampExpFlagAPI(u8 id, u8 state, s16 currAD)
{
    if(id >= eCExp_IDLen)
    {
        return;
    }

    /*设置异常*/
    if(1 == state)
    {
    	//新产生异常
    	if(0 == BitGet(sCSampExpFlag, id))
    	{
    		//记录异常
    		BitSet(sCSampExpFlag, id);

    		//俩通道误差异常
    		if(eCExp_HSDiff == id)
    		{
				//记录俩通道误差异常事件
    			DiagUserSelfErrRecord(eUDef_ErrRcd_TWOCDiff, eERR_RCDSORT_WARSET, 0,  0);
    		}
    		//校零异常
    		else if(eCExp_ZroErr == id)
    		{
    			//未使能分流器
    			if(0 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))
    			{
    				//记录霍尔校零异常事件
    				DiagUserSelfErrRecord(eUDef_ErrRcd_HallCErr, eERR_RCDSORT_WARSET, currAD,  0);
    			}
    			else
    			{
    				//记录分流器校零异常事件
    				DiagUserSelfErrRecord(eUDef_ErrRcd_ShunCErr, eERR_RCDSORT_WARSET, currAD,  0);
    			}
    		}
    		//分流器采样异常
    		else if(id >= eCExp_AfeErr)
    		{
				//记录分流器采样异常事件
				DiagUserSelfErrRecord(eUDef_ErrRcd_ShunCErr, eERR_RCDSORT_WARSET, currAD,  0);
    		}
    		//霍尔采样异常
    		else
    		{
				//记录霍尔采样异常事件
				DiagUserSelfErrRecord(eUDef_ErrRcd_HallCErr, eERR_RCDSORT_WARSET, currAD,  0);
    		}
    	}

        /*设置电流校零失败*/
        sCurrZeroFin = 2;
    }
    /*清除异常*/
    else if(0 == state)
    {
        BitClr(sCSampExpFlag, id);
    }
}

/*=============================================================================================
** @Name      : s32 GetGRealCurr10mAAPI(e_CSampChan channel)
** @Input     : void
** @Output    : 电流采样瞬时电流值 10mA
** @Function  : 获取电流采样实时瞬时电流值
** @The notes :
**===========================================================================================*/
s32 GetGRealCurr10mAAPI(e_CSampChan channel)
{
    /*双通道单独记录霍尔通道*/
	if((eCADC_Hall == channel)        /*使能分流器采样时霍尔另外使用*/
		&& (1 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt)))
    {
    	return(sHallRealCurr10mA);     /*返回独立值*/
    }
    else                                /*分流器通道或非测试程序*/
	{
    	return(sRealCurr10mA);          /*返回公共值*/
	}
}

/*=============================================================================================
** @Name      : s32 GetGRealVolt10uVAPI(e_CSampChan channel)
** @Input     : void
** @Output    : 电流采样瞬时电压值 10uV
** @Function  : 获取电流采样实时瞬时电压值
** @The notes :
**===========================================================================================*/
s32 GetGRealVolt10uVAPI(e_CSampChan channel)
{
    /*双通道单独记录霍尔通道*/
	if((eCADC_Hall == channel)      	/*使能分流器采样时霍尔另外使用*/
		&& (1 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt)))
    {
    	return(sHallRealVolt10uV);      /*返回独立值*/
    }
    else                                /*分流器通道或非测试程序*/
	{
    	return(sRealVolt10uV);          /*返回公共值*/
	}
}

/*=============================================================================================
** @Name      : u8 SampGetCurrZeroFinFlagAPI(void)
** @Input     : void
** @Output    : 电流采样零飘校准完成标志[0未校准 1校准成功 2校准失败]
** @Function  : 获取电流采样零飘校准完成标志
** @The notes :
**===========================================================================================*/
u8 SampGetCurrZeroFinFlagAPI(void)
{
    return(sCurrZeroFin);
}

/*=============================================================================================
** @Name      : u8 SampGetCurrSampExpFlagAPI(u8 channel)
** @Input     : channel:采样通道(0:霍尔通道 1:分流器通道 2:两个通道综合(一个正常则正常)) 0xff:原标志
** @Output    : 按位表示采样异常标志(bit0:霍尔供电5V异常 bit1:霍尔回检信号异常 bit2:霍尔ADC通道异常 bit3:霍尔AD值异常 bit4:分流器模拟前端采样异常 bit5:分流器AD值异常 bit6:电流校零异常 bit7:两通道采样误差异常[0正常 1异常])
** @Function  : 获取指定采样通道电流采样异常标志
** @The notes :
**===========================================================================================*/
u8 SampGetCurrSampExpFlagAPI(u8 channel)
{
    /*霍尔通道*/
    if(eCADC_Hall == channel)
    {
        return(sCSampExpFlag & 0x0f);
    }
    /*分流器通道*/
    else if(eCAFE_Shunt == channel)
    {
        return(sCSampExpFlag & 0x70);
    }
    /*两个通道(其中一个正常则正常)*/
    else if(eCCHAN_Num == channel)
    {
    	//只使能一个通道则返回实际标志
    	if((0 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCADC_Hall))
    		|| (0 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCADC_Hall)))
    	{
    		return(sCSampExpFlag);
    	}
    	//两个通道异常或两个通道误差过大
    	else if((((sCSampExpFlag & 0x0f) > 0) && ((sCSampExpFlag & 0x70) > 0))
    		|| (1 == BitGet(sCSampExpFlag, eCExp_HSDiff)))
    	{
    		return(sCSampExpFlag);
    	}
    	//其中一个通道正常
    	else
    	{
        	return(0);
    	}
    }
    /*原标志*/
    else if(0xff == channel)
    {
        return(sCSampExpFlag);
    }
    /*参数错误*/
    else
    {
    	return(0);
    }
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CalcCurrZeroCorr(s32 curr)
** @Input     : curr:空闲时采样电流零飘值 单位:1mA
** @Output    : void
** @Function  : 计算电流零飘值
** @The notes : 必须在高压接触器全断开(无电流)时调用
**===========================================================================================*/
static void CalcCurrZeroCorr(s32 curr)
{
    u16 flag = 0;           //EEPROM标志
    s32 curZero = 0;        //零飘值 1mA
    static u16 num = 0;     //积分次数
    static u16 time = 0;    //误差次数
    static s32 curSum = 0;  //零飘积分
    static s32 curMax = 0;  //零飘最大值
    static s32 curMin = 0;  //零飘最小值

    //电流校零完成
    if(1 == sCurrZeroFin)
    {
        //校零后误差仍超过0.3A则计算是否连续误差
        //if(ABS(curr, (sCurrZeroCorr * 10)) >= 300)
        if(ABS(curr, (sCurrZeroCorr * 10)) >= 500)
        {
            time++;
        }
        else
        {
            time = 0;
        }

        //校零后误差仍超过0.5A则重新初始化和校零
        //if((ABS(curr, (sCurrZeroCorr * 10)) >= 500)
        if((ABS(curr, (sCurrZeroCorr * 10)) >= 800)
            || (time >= 50))
        {
            time = 0;
            ///CurrSampleInit();
            //sCurrZeroFin = 0;
        }

        curSum = 0;
        num = 0;
        return;
    }

    //计算1s电流积分求平均
    if(num < 50)
    {
        if((curr > curMax) || (0 == num))
        {
            curMax = curr;
        }

        if((curr < curMin) || (0 == num))
        {
            curMin = curr;
        }

        curSum += curr;
        num++;
    }
    else
    {
        curZero = curSum / num;

        if(((0 - (s16)MAX_CURR_CORR_10MA) <= (curZero / 10))
            && ((curZero / 10) <= (s16)MAX_CURR_CORR_10MA)) //计算正确±10A
        {
            if(curZero < 0)                                 //负数
            {
                sCurrZeroCorr = (s16)((curZero - 5) / 10);  //零飘值 10mA
            }
            else                                            //正数
            {
                sCurrZeroCorr = (s16)((curZero + 5) / 10);  //零飘值 10mA
            }

            sCurrZeroMax = (s16)((curMax - curZero) / 10);  //零飘最大值 10mA
            sCurrZeroMin = (s16)((curMin - curZero) / 10);  //零飘最小值 10mA

            ParaWriteStoreCorrZeroCurr(1, (u16*)&sCurrZeroCorr, 1);//修改EEP值
            flag = PARA_CFG_FLAG_U16;
            ParaWriteStoreCorrZeroCurr(0, &flag, 1);        //修改EEP标志

            SetCurrSampExpFlagAPI(eCExp_ZroErr, 0, (curZero / 100));//清除电流校零异常标志
            sCurrZeroFin = 1;                               //电流校零完成
        }
        else                                                //计算错误重新计算
        {
        	SetCurrSampExpFlagAPI(eCExp_ZroErr, 1, (curZero / 100));//记录电流校零异常标志
            sCurrZeroFin = 2;                               //电流校零失败
        }

        curSum = 0;                                         //零飘求和值
        curMax = 0;                                         //零飘最大值
        curMin = 0;                                         //零飘最小值
        time = 0;
        num = 0;
    }
}

/*=============================================================================================
** @Name      : static void InputSampRealCurr10mA(s32 curr)
** @Input     : curr:实际电流 10mA
** @Output    : void
** @Function  : 输出记录电流采样实际电流值
** @The notes :
**===========================================================================================*/
static void InputSampRealCurr10mA(s32 curr)
{
    sRealCurr10mA = curr;
}

/*=============================================================================================
** @Name      : static void InputCSampRealVolt10uV(s32 volt)
** @Input     : volt:实际电压 0.01mV
** @Output    : void
** @Function  : 输出记录电流采样实际电压值
** @The notes :
**===========================================================================================*/
static void InputCSampRealVolt10uV(s32 volt)
{
    sRealVolt10uV = volt;
}

/*=============================================================================================
** @Name      : static void CheckShuntHallCurrDiff(s32 shuntCurr, s32 hallCurr)
** @Input     : shuntCurr:分流器电流采样值 10mA hallCurr:霍尔电流采样值 10mA
** @Output    : void
** @Function  : 判断分流器和霍尔两个通道采样值是否一致
** @The notes : 排除电流变化俩通道采样不同步采样值可能不同
**===========================================================================================*/
static void CheckShuntHallCurrDiff(s32 shuntCurr, s32 hallCurr)
{
	static u8 sNum = 0;
	static s32 sAvgSCurr = 0;
	static s32 sAvgHCurr = 0;

	//累计电流求平均电流
	sAvgSCurr += shuntCurr;
	sAvgHCurr += hallCurr;
	sNum++;

	//50次求平均电流
	if(sNum >= 50)
	{
		//求平均电流
		sAvgSCurr = sAvgSCurr / sNum;
		sAvgHCurr = sAvgHCurr / sNum;

		/*俩通道当前电流采样值与平均电流值均一样(误差2A内,确保恒流)*/
		if((ABS(shuntCurr, sAvgSCurr) <= 200) && (ABS(hallCurr, sAvgHCurr) <= 200))
		{
			if((ABS(shuntCurr, hallCurr) >= 1500)           /*两个采样通道误差超过15A*/
				&& (ABS(sAvgSCurr, sAvgHCurr) >= 1500)
				&& (ABS(shuntCurr, sAvgHCurr) >= 1500)
				&& (ABS(sAvgSCurr, hallCurr) >= 1500))
			{
				SetCurrSampExpFlagAPI(eCExp_HSDiff, 1, 0);  /*记录采样误差异常*/
			}
			else if((ABS(shuntCurr, hallCurr) <= 1000)      /*两个采样通道误差小于10A*/
				&& (ABS(sAvgSCurr, sAvgHCurr) <= 1000)
				&& (ABS(shuntCurr, sAvgHCurr) <= 1000)
				&& (ABS(sAvgSCurr, hallCurr) <= 1000))
			{
				SetCurrSampExpFlagAPI(eCExp_HSDiff, 0, 0);  /*清除采样误差异常*/
			}
		}

		sAvgSCurr = 0;
		sAvgHCurr = 0;
		sNum= 0;
	}
}
