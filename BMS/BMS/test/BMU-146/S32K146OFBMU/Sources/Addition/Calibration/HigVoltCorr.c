/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : HigVoltCorr.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 高压校准模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupInfo.h"
#include "ParaInterface.h"
#include "DiagInterface.h"
#include "TacUser.h"
#include "InsuCheck.h"
#include "HigVoltCorr.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u16 sHVoltCorrType = 0;                              /*高压采样校准类型(按位表示各个通道校准类型[bit0:总压单点校准 bit1:总压双点校准 bit2:预充单点校准 bit3:预充双点校准 bit4:Link电压单点校准 bit5:Link电压双点校准][0未校准 1已校准])*/
static s32 sSumVoltMapK = 0;                                /*电池总压采样通道校准K值*/
static s32 sSumVoltMapB = 0;                                /*电池总压采样通道校准B值*/
static s32 sPreVoltMapK = 0;                                /*预充电压采样通道校准K值*/
static s32 sPreVoltMapB = 0;                                /*预充电压采样通道校准B值*/
static s32 sLoaVoltMapK = 0;                                /*预充电压采样通道校准K值*/
static s32 sLoaVoltMapB = 0;                                /*预充电压采样通道校准B值*/
static s32 sPosVoltMapK = 0;                                /*Link+电压采样通道校准K值*/
static s32 sPosVoltMapB = 0;                                /*Link+电压采样通道校准B值*/
static s32 sNegVoltMapK = 0;                                /*Link-电压采样通道校准K值*/
static s32 sNegVoltMapB = 0;                                /*Link-电压采样通道校准B值*/

extern u16 gGCalibrResult_83[];		                        /*校准结果状态信息数组*/
extern u16 gGCalibrCtrl_89[];			                    /*采样校准命令信息数组*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void HigVoltSampCorrInit(void)
** @Input     : void
** @Output    : void
** @Function  : 高压采样校准初始化
** @The notes :
**===============================================================================================*/
void HigVoltSampCorrInit(void)
{
    u32 kbInfo[3] = {0};

    //读取EEP中总电压KB值
    if((TRUE == ParaReadStoreSumVoltCorrKB(kbInfo, 3)) && (0 != kbInfo[1]))
    {
        sSumVoltMapK = kbInfo[1];
        sSumVoltMapB = kbInfo[2];

        /*单点校准*/
        if(HV_CORR_BLOWUP == sSumVoltMapK)
        {
            BitSet(sHVoltCorrType, eHVCorr_SumVS);
        }
        /*双点校准*/
        else
        {
            BitSet(sHVoltCorrType, eHVCorr_SumVD);
        }
    }
    else
    {
        sSumVoltMapK = HV_CORR_BLOWUP;
        sSumVoltMapB = 0;
    }

    //读取EEP中预充电压KB值
    if((TRUE == ParaReadStorePreVoltCorrKB(kbInfo, 3)) && (0 != kbInfo[1]))
    {
        sPreVoltMapK = kbInfo[1];
        sPreVoltMapB = kbInfo[2];

        /*单点校准*/
        if(HV_CORR_BLOWUP == sPreVoltMapK)
        {
            BitSet(sHVoltCorrType, eHVCorr_PreVS);
        }
        /*双点校准*/
        else
        {
            BitSet(sHVoltCorrType, eHVCorr_PreVD);
        }
    }
    else
    {
    	sPreVoltMapK = HV_CORR_BLOWUP;
    	sPreVoltMapB = 0;
    }

    //读取EEP中负载电压KB值
    if((TRUE == ParaReadStoreLoaVoltCorrKB(kbInfo, 3)) && (0 != kbInfo[1]))
    {
        sLoaVoltMapK = kbInfo[1];
        sLoaVoltMapB = kbInfo[2];

        /*单点校准*/
        if(HV_CORR_BLOWUP == sLoaVoltMapK)
        {
            BitSet(sHVoltCorrType, eHVCorr_LoaVS);
        }
        /*双点校准*/
        else
        {
            BitSet(sHVoltCorrType, eHVCorr_LoaVD);
        }
    }
    else
    {
    	sLoaVoltMapK = HV_CORR_BLOWUP;
    	sLoaVoltMapB = 0;
    }

    //读取EEP中Link+电压KB值
    if((TRUE == ParaReadStorePosVoltCorrKB(kbInfo, 3)) && (0 != kbInfo[1]))
    {
        sPosVoltMapK = kbInfo[1];
        sPosVoltMapB = kbInfo[2];

        /*单点校准*/
        if(HV_CORR_BLOWUP == sPosVoltMapK)
        {
            BitSet(sHVoltCorrType, eHVCorr_PosVS);
        }
        /*双点校准*/
        else
        {
            BitSet(sHVoltCorrType, eHVCorr_PosVD);
        }
    }
    else
    {
    	sPosVoltMapK = HV_CORR_BLOWUP;
    	sPosVoltMapB = 0;
    }

    //读取EEP中Link-电压KB值
    if((TRUE == ParaReadStoreNegVoltCorrKB(kbInfo, 3)) && (0 != kbInfo[1]))
    {
        sNegVoltMapK = kbInfo[1];
        sNegVoltMapB = kbInfo[2];

        /*单点校准*/
        if(HV_CORR_BLOWUP == sNegVoltMapK)
        {
            BitSet(sHVoltCorrType, eHVCorr_NegVS);
        }
        /*双点校准*/
        else
        {
            BitSet(sHVoltCorrType, eHVCorr_NegVD);
        }
    }
    else
    {
    	sNegVoltMapK = HV_CORR_BLOWUP;
    	sNegVoltMapB = 0;
    }
}

/*=================================================================================================
** @Name      : void HigVoltSampCorrProcess(void)
** @Input     : void
** @Output    : void
** @Function  : 高压校准处理过程
** @The notes : 1s
**===============================================================================================*/
void HigVoltSampCorrProcess(void)
{
    u8 j = 0;
    u8 step = 0;
    u32 tempV = 0;
    s32 calcK = 0;
    s32 calcB = 0;
    static u16 sErrTime = 0;
    static u16 sFilTime = 0;
    static s32 sLowStandV = 0;
    static s32 sHighStandV = 0;
    static s32 sFilSumVolt = {0};
    static s32 sLowVolt[HV_CORR_FIL_T] = {0};
    static s32 sHighVolt[HV_CORR_FIL_T] = {0};

    //非调试模式不校准
    if(eMODE_DEBUG != GetGWorkModeAPI())
    {
        //校准数据清零
        gGCalibrResult_83[eCorrRes83_CorrType] = eCORR_INVALID;
        gGCalibrResult_83[eCorrRes83_CorrState] = 0;
    }
//    //有故障则校准失败
//    else if((SampGetHigVSampExpFlagAPI() > 0)                               //高压采样电路异常
//		&& ((eCORR_SUMVOLS == gGCalibrResult_83[eCorrRes83_CorrType])       //正在总压单点校准
//		|| (eCORR_SUMVOLD == gGCalibrResult_83[eCorrRes83_CorrType])        //正在总压双点校准
//		|| (eCORR_PREVOLS == gGCalibrResult_83[eCorrRes83_CorrType])        //正在预充单点校准
//		|| (eCORR_PREVOLD == gGCalibrResult_83[eCorrRes83_CorrType])        //正在预充单点校准
//		|| (eCORR_LINVOLS == gGCalibrResult_83[eCorrRes83_CorrType])        //正在Link单点校准
//		|| (eCORR_LINVOLD == gGCalibrResult_83[eCorrRes83_CorrType])))      //正在Link双点校准
//    {
//    	/*校准类型切换*/
//    	if((gGCalibrCtrl_89[eCorrCmd89_Type] > eCORR_INVALID)
//    		&& (gGCalibrResult_83[eCorrRes83_CorrType] != gGCalibrCtrl_89[eCorrCmd89_Type]))
//    	{
//    		gGCalibrResult_83[eCorrRes83_CorrType] = gGCalibrCtrl_89[eCorrCmd89_Type];
//    	}
//
//        //校准成功则恢复为未校准
//        if(100 == gGCalibrResult_83[eCorrRes83_CorrState])
//        {
//            gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
//
//            gGCalibrResult_83[eCorrRes83_CorrType] = eCORR_INVALID;
//            gGCalibrResult_83[eCorrRes83_CorrState] = 0;
//        }
//        //正在校准切换为校准失败
//        else if(gGCalibrResult_83[eCorrRes83_CorrState] > 0)
//        {
//            gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
//
//            gGCalibrResult_83[eCorrRes83_CorrState] = 101;
//        }
//    }
    //接收停止命令则失败
    else if(0xff == gGCalibrCtrl_89[eCorrCmd89_Type])                       //终止命令
    {
        if((eCORR_SUMVOLS == gGCalibrResult_83[eCorrRes83_CorrType])        /*正在总压单点校准*/
            || (eCORR_SUMVOLD == gGCalibrResult_83[eCorrRes83_CorrType])    /*正在总压双点校准*/
            || (eCORR_PREVOLS == gGCalibrResult_83[eCorrRes83_CorrType])    /*正在预充单点校准*/
            || (eCORR_PREVOLD == gGCalibrResult_83[eCorrRes83_CorrType])    /*正在预充双点校准*/
    		|| (eCORR_LINVOLS == gGCalibrResult_83[eCorrRes83_CorrType])    /*正在Link单点校准*/
    		|| (eCORR_LINVOLD == gGCalibrResult_83[eCorrRes83_CorrType]))   /*正在Link双点校准*/
        {
            //处于校准中途则校准失败
            if((0 != gGCalibrResult_83[eCorrRes83_CorrState])
                && (100 != gGCalibrResult_83[eCorrRes83_CorrState]))
            {
                gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;

                gGCalibrResult_83[eCorrRes83_CorrState] = 101;
            }
        }
    }
    //正在电池总压单点校准接收其他校准命令
    else if((eCORR_SUMVOLS == gGCalibrResult_83[eCorrRes83_CorrType])       //正在总压单点校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] > 0)                    //已开始校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] < 100)                  //未完成校准
        && (eCORR_SUMVOLS != gGCalibrCtrl_89[eCorrCmd89_Type]))             //非总压单点校准命令
    {
        //校准失败
        gGCalibrResult_83[eCorrRes83_CorrState] = 101;
    }
    //正在电池总压双点校准接收其他校准命令
    else if((eCORR_SUMVOLD == gGCalibrResult_83[eCorrRes83_CorrType])       //正在总压双点校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] > 0)                    //已开始校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] < 100)                  //未完成校准
        && (eCORR_SUMVOLD != gGCalibrCtrl_89[eCorrCmd89_Type]))             //非总压双点校准命令
    {
        //校准失败
        gGCalibrResult_83[eCorrRes83_CorrState] = 101;
    }
    //正在预充电压单点校准接收其他校准命令
    else if((eCORR_PREVOLS == gGCalibrResult_83[eCorrRes83_CorrType])       //正在预充电压单点校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] > 0)                    //已开始校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] < 100)                  //未完成校准
        && (eCORR_PREVOLS != gGCalibrCtrl_89[eCorrCmd89_Type]))             //非预充电压单点校准命令
    {
        //校准失败
        gGCalibrResult_83[eCorrRes83_CorrState] = 101;
    }
    //正在预充电压双点校准接收其他校准命令
    else if((eCORR_PREVOLD == gGCalibrResult_83[eCorrRes83_CorrType])       //正在预充电压双点校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] > 0)                    //已开始校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] < 100)                  //未完成校准
        && (eCORR_PREVOLD != gGCalibrCtrl_89[eCorrCmd89_Type]))             //非预充电压双点校准命令
    {
        //校准失败
        gGCalibrResult_83[eCorrRes83_CorrState] = 101;
    }
    //正在Link电压单点校准接收其他校准命令
    else if((eCORR_LINVOLS == gGCalibrResult_83[eCorrRes83_CorrType])       //正在Link电压单点校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] > 0)                    //已开始校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] < 100)                  //未完成校准
        && (eCORR_LINVOLS != gGCalibrCtrl_89[eCorrCmd89_Type]))             //非Link电压单点校准命令
    {
        //校准失败
        gGCalibrResult_83[eCorrRes83_CorrState] = 101;
    }
    //正在Link电压双点校准接收其他校准命令
    else if((eCORR_LINVOLD == gGCalibrResult_83[eCorrRes83_CorrType])       //正在Link电压双点校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] > 0)                    //已开始校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] < 100)                  //未完成校准
        && (eCORR_LINVOLD != gGCalibrCtrl_89[eCorrCmd89_Type]))             //非Link电压双点校准命令
    {
        //校准失败
        gGCalibrResult_83[eCorrRes83_CorrState] = 101;
    }
    /*命令正确*/
    else
    {
    	/*正在高压校准*/
		if((gGCalibrCtrl_89[eCorrCmd89_Type] >= eCORR_SUMVOLS)
            && (gGCalibrCtrl_89[eCorrCmd89_Type] <= eCORR_LINVOLD))
		{
	    	/*当前高压校准状态为步骤*/
			step = gGCalibrResult_83[eCorrRes83_CorrState];
		}
    	/*非高压校准*/
		else
		{
			/*保持在第一步*/
			step = 0;
		}
    }

    //根据校准状态检测信息
    switch(step)
    {
        //未校准
        case 0:
            /*更新高压校准结果*/
        	UpdateHVoltCorrResult();

            //下发开始校准高压命令(总压/预充/Link+-单点或双点校准)
            if((gGCalibrCtrl_89[eCorrCmd89_Type] >= eCORR_SUMVOLS)
            	&& (gGCalibrCtrl_89[eCorrCmd89_Type] <= eCORR_LINVOLD))
            {
                gGCalibrResult_83[eCorrRes83_CorrState] = 1;
                gGCalibrResult_83[eCorrRes83_CorrType] = gGCalibrCtrl_89[eCorrCmd89_Type];
                BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], gGCalibrCtrl_89[eCorrCmd89_Type]);
            }
            break;

        //清除校准参数
        case 1:
            /*电池总压校准*/
        	if((eCORR_SUMVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_SUMVOLD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
        		//复位KB参数
                BitClr(sHVoltCorrType, eHVCorr_SumVS);
                BitClr(sHVoltCorrType, eHVCorr_SumVD);
                ClrEEPHVoltCorrKBInfo(0);

                sSumVoltMapK = HV_CORR_BLOWUP;
                sSumVoltMapB = 0;
        	}
            /*预充/负载电压校准*/
        	else if((eCORR_PREVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
            	|| (eCORR_PREVOLD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
        		//接触器未全断开或接触器粘连
        		if((FALSE == TacGetGroupRunSwhAllOffStateAPI())
        			|| (GetSwhAdhesErrFlagAPI() > 0))
        		{
					//复位预充电压KB参数
					BitClr(sHVoltCorrType, eHVCorr_PreVS);
					BitClr(sHVoltCorrType, eHVCorr_PreVD);
					ClrEEPHVoltCorrKBInfo(1);

					sPreVoltMapK = HV_CORR_BLOWUP;
					sPreVoltMapB = 0;
        		}
        		//接触器全断开且无接触器粘连
        		else
        		{
					//复位负载电压KB参数
					BitClr(sHVoltCorrType, eHVCorr_LoaVS);
					BitClr(sHVoltCorrType, eHVCorr_LoaVD);
					ClrEEPHVoltCorrKBInfo(2);

					sLoaVoltMapK = HV_CORR_BLOWUP;
					sLoaVoltMapB = 0;
        		}
        	}
            /*Link电压校准(两路只校准其中一路)*/
        	else
        	{
        		//复位KB参数
        		/*Link-有电压则优先校准link-*/
        		if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link2) > 100)
        		{
					BitClr(sHVoltCorrType, eHVCorr_NegVS);
					BitClr(sHVoltCorrType, eHVCorr_NegVD);
					ClrEEPHVoltCorrKBInfo(4);

					sNegVoltMapK = HV_CORR_BLOWUP;
					sNegVoltMapB = 0;
        		}
        		/*Link-无电压默认校准link+*/
        		else
        		{
					BitClr(sHVoltCorrType, eHVCorr_PosVS);
					BitClr(sHVoltCorrType, eHVCorr_PosVD);
					ClrEEPHVoltCorrKBInfo(3);

					sPosVoltMapK = HV_CORR_BLOWUP;
					sPosVoltMapB = 0;
        		}
        	}

            for(j = 0; j < HV_CORR_FIL_T; j++)
            {
                sLowVolt[j] = 0;
                sHighVolt[j] = 0;
            }

            sErrTime = 0;
            sFilTime = 0;
            gGCalibrCtrl_89[eCorrCmd89_Value] = 0;
            gGCalibrResult_83[eCorrRes83_CorrState] = 2;
            break;

        //等待第一个基准高压
        case 2:
            //下发基准高压
            if(0 != gGCalibrCtrl_89[eCorrCmd89_Value])
            {
                //等待1s再采集
                sErrTime++;
                if(sErrTime >= 1)
                {
                    sErrTime = 0;
                    sFilTime = 0;
                    sLowStandV = 0;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 3;
                }
            }
            break;

        //采集第1个基准电压
        case 3:
            //记录基准电压(0.1V转10mV)
            sLowStandV = (s32)gGCalibrCtrl_89[eCorrCmd89_Value] * 10;

            /*电池总压校准*/
        	if((eCORR_SUMVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_SUMVOLD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
                /*取电池总压采样值*/
        		tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_BattHV) * 10;
        	}
            /*预充电压校准*/
        	else if((eCORR_PREVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
                || (eCORR_PREVOLD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
                /*取预充电压采样值*/
        		tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_PChgHV) * 10;
        	}
            /*Link电压校准*/
        	else
        	{
        		/*Link-有电压则优先校准link-*/
        		if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link2) > 100)
        		{
					/*取Link-电压采样值*/
					//tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link2) * 10;
					tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link2);
        		}
        		/*Link-无电压默认校准link+*/
        		else
        		{
					/*取Link+电压采样值*/
					//tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link1) * 10;
					tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link1);
        		}
        	}

            //误差50V内(或10%V内)
            if((ABS(tempV, sLowStandV) < 5000)
                || (ABS(tempV, sLowStandV) < (sLowStandV / 10)))
            {
               sLowVolt[sFilTime] = (s32)tempV;
            }
            //采样误差过大或模拟电池输出误差过大
            else
            {
                //等待3s
                sErrTime++;
                if(sErrTime >= 3)
                {
                    //等待超时则校准失败
                    sErrTime = 0;
                    gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 101;
                }
                return;
            }

            sFilTime++;
            sErrTime = 0;

            //达到滤波采样次数则求平均值并进入下一步
            if(sFilTime >= HV_CORR_FIL_T)
            {
                //计算采样通道平均值
                sFilSumVolt = 0;

                for(j = 0; j < HV_CORR_FIL_T; j++)
                {
                    sFilSumVolt += sLowVolt[j];
                }

                sLowVolt[0] =  sFilSumVolt / HV_CORR_FIL_T;

                sFilTime = 0;

                //判断是单点校准还是双点校准
                if((eCORR_SUMVOLS == gGCalibrResult_83[eCorrRes83_CorrType])
                    || (eCORR_PREVOLS == gGCalibrResult_83[eCorrRes83_CorrType])
	                || (eCORR_LINVOLS == gGCalibrResult_83[eCorrRes83_CorrType]))
                {
                    //单点校准进入KB值计算
                    gGCalibrCtrl_89[eCorrCmd89_Value] = 0;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 6;
                }
                else
                {
                    //双点校准进入第2点采集
                    gGCalibrCtrl_89[eCorrCmd89_Value] = 0;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 4;
                }
            }
            break;

        //等待第2个基准电压
        case 4:
            //下发基准电压(必须比第1个基准点电压大)
            if((0 != gGCalibrCtrl_89[eCorrCmd89_Value]) && (gGCalibrCtrl_89[eCorrCmd89_Value] > (sLowStandV / 10)))
            {
                //等待1s再采集
                sErrTime++;
                if(sErrTime >= 1)
                {
                    sErrTime = 0;
                    sFilTime = 0;
                    sHighStandV = 0;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 5;
                }
            }
            break;

        //采集第2个基准电压
        case 5:
            //记录基准电压(0.1V转10mV)
            sHighStandV = (s32)gGCalibrCtrl_89[eCorrCmd89_Value] * 10;

            /*电池总压校准*/
        	if((eCORR_SUMVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_SUMVOLD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
                /*取电池总压采样值*/
        		tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_BattHV) * 10;
        	}
            /*预充电压校准*/
        	else if((eCORR_PREVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
                || (eCORR_PREVOLD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
                /*取预充电压采样值*/
        		tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_PChgHV) * 10;
        	}
            /*Link电压校准*/
        	else
        	{
        		/*Link-有电压则优先校准link-*/
        		if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link2) > 100)
        		{
					/*取Link-电压采样值*/
					//tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link2) * 10;
					tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link2);
        		}
        		/*Link-无电压默认校准link+*/
        		else
        		{
					/*取Link+电压采样值*/
					//tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link1) * 10;
					tempV = GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link1);
        		}
        	}

            //误差50V内(或10%V内)
            if((ABS(tempV, sHighStandV) < 5000)
                || (ABS(tempV, sHighStandV) < (sHighStandV / 10)))
            {
                sHighVolt[sFilTime] =(s32)tempV;
            }
            //采样误差过大或模拟电池输出误差过大
            else
            {
                //等待3s
                sErrTime++;
                if(sErrTime >= 3)
                {
                    //等待超时则校准失败
                    sErrTime = 0;
                    gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 101;
                }
                return;
            }

            sFilTime++;
            sErrTime = 0;

            //达到滤波采样次数则求平均值并进入下一步
            if(sFilTime >= HV_CORR_FIL_T)
            {
                //计算每个采样通道平均值
                sFilSumVolt = 0;

                for(j = 0; j < HV_CORR_FIL_T; j++)
                {
                    sFilSumVolt += sHighVolt[j];
                }

                sHighVolt[0] =  sFilSumVolt / HV_CORR_FIL_T;

                sFilTime = 0;

                //进入KB值计算
                gGCalibrCtrl_89[eCorrCmd89_Value] = 0;
                gGCalibrResult_83[eCorrRes83_CorrState] = 6;
            }
            break;

        //校准计算处理
        case 6:
            //判断是单点校准还是双点校准
            if((eCORR_SUMVOLS == gGCalibrResult_83[eCorrRes83_CorrType])
               	|| (eCORR_PREVOLS == gGCalibrResult_83[eCorrRes83_CorrType])
	            || (eCORR_LINVOLS == gGCalibrResult_83[eCorrRes83_CorrType]))
            {
                sHighStandV = sLowStandV;
                calcK = HV_CORR_BLOWUP;

                calcB = ((s32)sLowStandV - sLowVolt[0]) * HV_CORR_BLOWUP;
            }
            else
            {
                calcK = (((s32)sHighStandV - (s32)sLowStandV) * 1000) / (((s32)sHighVolt[0] - (s32)sLowVolt[0]));
                //calcB = ((((s32)sHighVolt[0] * sLowStandV * 1000) - ((s32)sLowVolt[0] * sHighStandV * 1000))/((s32)sHighVolt[0] - (s32)sLowVolt[0]));
                calcB = (s32)sHighStandV * 1000 - (s32)sHighVolt[0] * calcK;
            }

            /*电池总压校准*/
        	if((eCORR_SUMVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_SUMVOLD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
        		//检测电池总压采样校准是否正确
        	    tempV = ((u32)GetGDcSideSampRealInfoAPI(eReferSampInfo200_BattHV) * 10 * calcK  + calcB) / HV_CORR_BLOWUP;

                //误差超过2V
                if(ABS(sHighStandV, tempV) > 200)
                {
                    //校准失败;
                    gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 101;
                }
                //校准正确
                else
                {
                    //记录电池总压校准KB值
                    sSumVoltMapK = calcK;
                    sSumVoltMapB = calcB;
                    RcdEEPHVoltCorrKBInfo(0);

                    /*单点校准*/
                    if(eCORR_SUMVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
                    {
                        BitSet(sHVoltCorrType, eHVCorr_SumVS);
                    }
                    /*双点校准*/
                    else
                    {
                        BitSet(sHVoltCorrType, eHVCorr_SumVD);
                    }

                    gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 100;
                }
        	}
            /*预充电压校准*/
        	else if((eCORR_PREVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
                || (eCORR_PREVOLD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
        		//检测预充电压采样校准是否正确
        	    tempV = ((u32)GetGDcSideSampRealInfoAPI(eReferSampInfo200_PChgHV) * 10 * calcK  + calcB) / HV_CORR_BLOWUP;

                //误差超过5V
                if(ABS(sHighStandV, tempV) > 500)
                {
                    //校准失败;
                    gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 101;
                }
                //校准正确
                else
                {
            		//接触器未全断开或接触器粘连
            		if((FALSE == TacGetGroupRunSwhAllOffStateAPI())
            			|| (GetSwhAdhesErrFlagAPI() > 0))
            		{
                        //记录预充电压校准KB值
                        sPreVoltMapK = calcK;
                        sPreVoltMapB = calcB;
                        RcdEEPHVoltCorrKBInfo(1);

                        /*单点校准*/
                        if(eCORR_PREVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
                        {
                            BitSet(sHVoltCorrType, eHVCorr_PreVS);
                        }
                        /*双点校准*/
                        else
                        {
                            BitSet(sHVoltCorrType, eHVCorr_PreVD);
                        }
            		}
            		//接触器全断开且无接触器粘连
            		else
            		{
                        //记录负载电压校准KB值
                        sLoaVoltMapK = calcK;
                        sLoaVoltMapB = calcB;
                        RcdEEPHVoltCorrKBInfo(2);

                        /*单点校准*/
                        if(eCORR_PREVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
                        {
                            BitSet(sHVoltCorrType, eHVCorr_LoaVS);
                        }
                        /*双点校准*/
                        else
                        {
                            BitSet(sHVoltCorrType, eHVCorr_LoaVD);
                        }
            		}

                    gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 100;
                }
        	}
            /*Link电压校准*/
        	else
        	{
        		/*Link-有电压则优先校准link-*/
        		if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link2) > 100)
        		{
					//检测Link-电压采样校准是否正确
					//tempV = ((u32)GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link2) * 10 * calcK  + calcB) / HV_CORR_BLOWUP;
					tempV = ((u32)GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link2) * calcK  + calcB) / HV_CORR_BLOWUP;

					//误差超过0.5V
					//if(ABS(sHighStandV, tempV) > 50)
					//误差超过10mV
					if(ABS(sHighStandV, tempV) > 10)
					{
						//校准失败;
						gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
						gGCalibrResult_83[eCorrRes83_CorrState] = 101;
					}
					//校准正确
					else
					{
						//记录Link-电压校准KB值
						sNegVoltMapK = calcK;
						sNegVoltMapB = calcB;
						RcdEEPHVoltCorrKBInfo(4);

						/*单点校准*/
						if(eCORR_LINVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
						{
							BitSet(sHVoltCorrType, eHVCorr_NegVS);
						}
						/*双点校准*/
						else
						{
							BitSet(sHVoltCorrType, eHVCorr_NegVD);
						}

						gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
						gGCalibrResult_83[eCorrRes83_CorrState] = 100;
					}
        		}
        		/*Link-无电压默认校准link+*/
        		else
        		{
					//检测Link+电压采样校准是否正确
					//tempV = ((u32)GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link1) * 10 * calcK  + calcB) / HV_CORR_BLOWUP;
					tempV = ((u32)GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link1) * calcK  + calcB) / HV_CORR_BLOWUP;

					//误差超过0.5V
					//if(ABS(sHighStandV, tempV) > 50)
					//误差超过10mV
					if(ABS(sHighStandV, tempV) > 10)
					{
						//校准失败;
						gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
						gGCalibrResult_83[eCorrRes83_CorrState] = 101;
					}
					//校准正确
					else
					{
						//记录Link-电压校准KB值
						sPosVoltMapK = calcK;
						sPosVoltMapB = calcB;
						RcdEEPHVoltCorrKBInfo(3);

						/*单点校准*/
						if(eCORR_LINVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
						{
							BitSet(sHVoltCorrType, eHVCorr_PosVS);
						}
						/*双点校准*/
						else
						{
							BitSet(sHVoltCorrType, eHVCorr_PosVD);
						}

						gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
						gGCalibrResult_83[eCorrRes83_CorrState] = 100;
					}
        		}
        	}
            break;

        //校准成功
        case 100:
        //校准失败
        case 101:
            /*更新高压校准结果*/
        	UpdateHVoltCorrResult();

            //下发开始高压校准命令
            if((eCORR_SUMVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
            	|| (eCORR_SUMVOLD == gGCalibrCtrl_89[eCorrCmd89_Type])
            	|| (eCORR_PREVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
            	|| (eCORR_PREVOLD == gGCalibrCtrl_89[eCorrCmd89_Type])
            	|| (eCORR_LINVOLS == gGCalibrCtrl_89[eCorrCmd89_Type])
            	|| (eCORR_LINVOLD == gGCalibrCtrl_89[eCorrCmd89_Type]))
            {
                gGCalibrResult_83[eCorrRes83_CorrType] = gGCalibrCtrl_89[eCorrCmd89_Type];
                gGCalibrResult_83[eCorrRes83_CorrState] = 1;
            }
            break;

        default:
            break;
    }
}

/*=================================================================================================
** @Name      : s32 CorrGetVoltCorrKBValue(e_HVCorrValue index)
** @Input     : index:高压校准KB值索引
** @Output    : 当前高压校准KB值(系数0.1)
** @Function  : 获取当前高压校准KB值
** @The notes : 放大1000(OC_CORR_BLOWUP)倍率后的值
**===============================================================================================*/
s32 CorrGetVoltCorrKBValue(e_HVCorrValue index)
{
    s32 value = 0;

    if(eHVCorr_SumVK == index)
    {
    	value =sSumVoltMapK;
    }
    else if(eHVCorr_SumVB == index)
    {
    	value = sSumVoltMapB;
    }
    else if(eHVCorr_PreVK == index)
    {
    	value = sPreVoltMapK;
    }
    else if(eHVCorr_PreVB == index)
    {
    	value = sPreVoltMapB;
    }
    else if(eHVCorr_PosVK == index)
    {
    	value = sPosVoltMapK;
    }
    else if(eHVCorr_PosVB == index)
    {
    	value = sPosVoltMapB;
    }
    else if(eHVCorr_NegVK == index)
    {
    	value = sNegVoltMapK;
    }
    else if(eHVCorr_NegVB == index)
    {
    	value = sNegVoltMapB;
    }
    else
    {
        return(0);
    }

    return(value);
}

/*=================================================================================================
** @Name      : u16 CorrRealSumVoltByKB(u32 volt)
** @Input     : volt:修正前实际高压值 10mV
** @Output    : 修正后实际电压  0.1V
** @Function  : 通过KB值修正实际电池总压采样值
** @The notes :
**===============================================================================================*/
u16 CorrRealSumVoltByKB(u32 volt)
{
    s32 realVolt = 0;

    //高压通道已校准
    if((1 == BitGet(sHVoltCorrType, eHVCorr_SumVS))
    	|| (1 == BitGet(sHVoltCorrType, eHVCorr_SumVD)))
    {
        //按KB值计算,10mV转0.1V
        realVolt = ((s32)volt * sSumVoltMapK + sSumVoltMapB) / HV_CORR_BLOWUP;

        //不能为负数
        if(realVolt <= 0)
        {
            realVolt = 0;
        }
        else
        {
            //10mV转0.1V
            realVolt = (realVolt + 5) / 10;
        }
    }
    //高压通道未校准
    else
    {
        //10mV转0.1V
        realVolt = (volt + 5) / 10;

        //不能为负数
        if(realVolt <= 0)
        {
            realVolt = 0;
        }
    }

    return((u16)realVolt);
}

/*=================================================================================================
** @Name      : u16 CorrRealPreVoltByKB(u32 volt)
** @Input     : volt:修正前实际高压值 10mV
** @Output    : 修正后实际电压  0.1V
** @Function  : 通过KB值修正实际预充电压采样值
** @The notes :
**===============================================================================================*/
u16 CorrRealPreVoltByKB(u32 volt)
{
    s32 realVolt = 0;

    //高压通道已校准
    if((1 == BitGet(sHVoltCorrType, eHVCorr_PreVS))
    	|| (1 == BitGet(sHVoltCorrType, eHVCorr_PreVD)))
    {
    	if(volt > 0)
    	{
    		//按KB值计算,10mV转0.1V
    		realVolt = ((s32)volt * sPreVoltMapK + sPreVoltMapB) / HV_CORR_BLOWUP;
    	}
    	else
    	{
    		realVolt = 0;
    	}

        //不能为负数
        if(realVolt <= 0)
        {
            realVolt = 0;
        }
        else
        {
            //10mV转0.1V
            realVolt = (realVolt + 5) / 10;
        }
    }
    //高压通道未校准
    else
    {
        //10mV转0.1V
        realVolt = (volt + 5) / 10;

        //不能为负数
        if(realVolt <= 0)
        {
            realVolt = 0;
        }
    }

    return((u16)realVolt);
}

/*=================================================================================================
** @Name      : u16 CorrRealLoaVoltByKB(u32 volt)
** @Input     : volt:修正前实际高压值 10mV
** @Output    : 修正后实际电压  0.1V
** @Function  : 通过KB值修正实际负载电压采样值
** @The notes :
**===============================================================================================*/
u16 CorrRealLoaVoltByKB(u32 volt)
{
    s32 realVolt = 0;

    //高压通道已校准
    if((1 == BitGet(sHVoltCorrType, eHVCorr_LoaVS))
    	|| (1 == BitGet(sHVoltCorrType, eHVCorr_LoaVD)))
    {
    	if(volt > 0)
    	{
    		//按KB值计算,10mV转0.1V
    		realVolt = ((s32)volt * sLoaVoltMapK + sLoaVoltMapB) / HV_CORR_BLOWUP;
    	}
    	else
    	{
    		realVolt = 0;
    	}

        //不能为负数
        if(realVolt <= 0)
        {
            realVolt = 0;
        }
        else
        {
            //10mV转0.1V
            realVolt = (realVolt + 5) / 10;
        }
    }
    //高压通道未校准
    else
    {
        //10mV转0.1V
        realVolt = (volt + 5) / 10;

        //不能为负数
        if(realVolt <= 0)
        {
            realVolt = 0;
        }
    }

    return((u16)realVolt);
}

/*=================================================================================================
** @Name      : u16 CorrRealLinkPosVoltByKB(u32 volt)
** @Input     : volt:修正前实际高压值 1mV
** @Output    : 修正后实际电压  1mV
** @Function  : 通过KB值修正实际Link+电压采样值
** @The notes :
**===============================================================================================*/
u16 CorrRealLinkPosVoltByKB(u32 volt)
{
    s32 realVolt = 0;

    //Link+通道已校准
    if((1 == BitGet(sHVoltCorrType, eHVCorr_PosVS))
    	|| (1 == BitGet(sHVoltCorrType, eHVCorr_PosVD)))
    {
        //按KB值计算,10mV转0.1V
        realVolt = ((s32)volt * sPosVoltMapK + sPosVoltMapB) / HV_CORR_BLOWUP;

        //不能为负数
        if(realVolt <= 0)
        {
            realVolt = 0;
        }
        else
        {
            //10mV转0.1V
            //realVolt = (realVolt + 5) / 10;
        }
    }
    //高压通道未校准
    else
    {
        //10mV转0.1V
        //realVolt = (volt + 5) / 10;
        realVolt = volt;

        //不能为负数
        if(realVolt <= 0)
        {
            realVolt = 0;
        }
    }

    return((u16)realVolt);
}

/*=================================================================================================
** @Name      : u16 CorrRealLinkNegVoltByKB(u32 volt)
** @Input     : volt:修正前实际高压值 1mV
** @Output    : 修正后实际电压  1mV
** @Function  : 通过KB值修正实际Link-电压采样值
** @The notes :
**===============================================================================================*/
u16 CorrRealLinkNegVoltByKB(u32 volt)
{
    s32 realVolt = 0;

    //Link-通道已校准
    if((1 == BitGet(sHVoltCorrType, eHVCorr_NegVS))
    	|| (1 == BitGet(sHVoltCorrType, eHVCorr_NegVD)))
    {
        //按KB值计算,10mV转0.1V
        realVolt = ((s32)volt * sNegVoltMapK + sNegVoltMapB) / HV_CORR_BLOWUP;

        /*不能为负数*/
        if(realVolt <= 0)
        {
            realVolt = 0;
        }
        else
        {
            //10mV转0.1V
            //realVolt = (realVolt + 5) / 10;
        }
    }
    //高压通道未校准
    else
    {
        //10mV转0.1V
        //realVolt = (volt + 5) / 10;
        realVolt = volt;

        /*不能为负数*/
        if(realVolt <= 0)
        {
            realVolt = 0;
        }
    }

    return((u16)realVolt);
}

/*=================================================================================================
** @Name      : void ClrEEPHVoltCorrKBInfo(u8 chan)
** @Input     : chan:高压通道 0:电池总压 1:预充电压 2:负载电压 3:link+电压 4:link-电压
** @Output    : void
** @Function  : 清除EEP高压采样校准KB值
** @The notes :
**===============================================================================================*/
void ClrEEPHVoltCorrKBInfo(u8 chan)
{
	u32 flag[3] = {0xffffffff, 0xffffffff, 0xffffffff};

	/*电池总压*/
	if(0 == chan)
	{
        /*清除EEP值*/
        ParaWriteStoreSumVoltCorrKB(0, flag, 3);
	}
	/*预充电压*/
	else if(1 == chan)
	{
		/*清除EEP值*/
        ParaWriteStorePreVoltCorrKB(0, flag, 3);
	}
	/*负载电压*/
	else if(2 == chan)
	{
		/*清除EEP值*/
        ParaWriteStoreLoaVoltCorrKB(0, flag, 3);
	}
	/*Link+电压*/
	else if(3 == chan)
	{
		/*清除EEP值*/
        ParaWriteStorePosVoltCorrKB(0, flag, 3);
	}
	/*Link-电压*/
	else if(4 == chan)
	{
		/*清除EEP值*/
        ParaWriteStoreNegVoltCorrKB(0, flag, 3);
	}
}

/*=================================================================================================
** @Name      : void RcdEEPHVoltCorrKBInfo(u8 chan)
** @Input     : chan:高压通道 0:电池总压 1:预充电压  2:负载电压 3:link+电压 4:link-电压
** @Output    : void
** @Function  : 记录高压校准KB值到EEP
** @The notes :
**===============================================================================================*/
void RcdEEPHVoltCorrKBInfo(u8 chan)
{
	u32 flag = PARA_CFG_FLAG_U32;

	/*电池总压*/
	if(0 == chan)
	{
		/*修改EEP值*/
        ParaWriteStoreSumVoltCorrKB(1, (u32*)&sSumVoltMapK, 1);
        ParaWriteStoreSumVoltCorrKB(2, (u32*)&sSumVoltMapB, 1);
        ParaWriteStoreSumVoltCorrKB(0, &flag, 1);
	}
	/*预充电压*/
	else if(1 == chan)
	{
		/*修改EEP值*/
        ParaWriteStorePreVoltCorrKB(1, (u32*)&sPreVoltMapK, 1);
        ParaWriteStorePreVoltCorrKB(2, (u32*)&sPreVoltMapB, 1);
        ParaWriteStorePreVoltCorrKB(0, &flag, 1);
	}
	/*负载电压*/
	else if(2 == chan)
	{
		/*修改EEP值*/
        ParaWriteStoreLoaVoltCorrKB(1, (u32*)&sLoaVoltMapK, 1);
        ParaWriteStoreLoaVoltCorrKB(2, (u32*)&sLoaVoltMapB, 1);
        ParaWriteStoreLoaVoltCorrKB(0, &flag, 1);
	}
	/*Link+电压*/
	else if(3 == chan)
	{
		/*修改EEP值*/
        ParaWriteStorePosVoltCorrKB(1, (u32*)&sPosVoltMapK, 1);
        ParaWriteStorePosVoltCorrKB(2, (u32*)&sPosVoltMapB, 1);
        ParaWriteStorePosVoltCorrKB(0, &flag, 1);
	}
	/*Link-电压*/
	else if(4 == chan)
	{
		/*修改EEP值*/
        ParaWriteStoreNegVoltCorrKB(1, (u32*)&sNegVoltMapK, 1);
        ParaWriteStoreNegVoltCorrKB(2, (u32*)&sNegVoltMapB, 1);
        ParaWriteStoreNegVoltCorrKB(0, &flag, 1);
	}
}

/*=================================================================================================
** @Name      : void UpdateHVoltCorrResult(void)
** @Input     : void
** @Output    : void
** @Function  : 更新高压校准结果
** @The notes : 根据校准标志位
**===============================================================================================*/
void UpdateHVoltCorrResult(void)
{
    /*电池总压已单点校准*/
	if(1 == BitGet(sHVoltCorrType, eHVCorr_SumVS))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_SUMVOLS);
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_SUMVOLS);
	}

    /*电池总压已双点校准*/
	if(1 == BitGet(sHVoltCorrType, eHVCorr_SumVD))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_SUMVOLD);
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_SUMVOLD);
	}

    /*预充/负载电压已单点校准*/
	if((1 == BitGet(sHVoltCorrType, eHVCorr_PreVS))
		|| (1 == BitGet(sHVoltCorrType, eHVCorr_LoaVS)))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_PREVOLS);
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_PREVOLS);
	}

    /*预充/负载电压已双点校准*/
	if((1 == BitGet(sHVoltCorrType, eHVCorr_PreVD))
		|| (1 == BitGet(sHVoltCorrType, eHVCorr_LoaVD)))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_PREVOLD);
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_PREVOLD);
	}

    /*Link电压已单点校准*/
	if((1 == BitGet(sHVoltCorrType, eHVCorr_PosVS))
		|| (1 == BitGet(sHVoltCorrType, eHVCorr_NegVS)))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_LINVOLS);
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_LINVOLS);
	}

    /*Link电压已双点校准*/
	if((1 == BitGet(sHVoltCorrType, eHVCorr_PosVD))
			|| (1 == BitGet(sHVoltCorrType, eHVCorr_NegVD)))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_LINVOLD);
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_LINVOLD);
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
