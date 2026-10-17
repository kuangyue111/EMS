/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : OutCurrCorr.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电流校准模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupInfo.h"
#include "CurrSample.h"
#include "ParaInterface.h"
#include "DiagInterface.h"
#include "OutCurrCorr.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sCurrCorrType = 0;                                /*电流采样校准类型(按位表示各个通道校准类型(bit0:霍尔电流单点校准 bit1:霍尔电流双点校准  bit2:分流器充电电流单点校准 bit3:分流器充电电流双点校准  bit4:分流器放电电流单点校准 bit5:分流器放电电流双点校准[0未校准 1校准])*/
static s32 sHalOutCMapK = 0;                                /*霍尔输出电流采样通道校准K值*/
static s32 sHalOutCMapB = 0;                                /*霍尔输出电流采样通道校准B值*/
static s32 sShuChgCMapK = 0;                                /*分流器充电电流采样通道校准K值*/
static s32 sShuChgCMapB = 0;                                /*分流器充电电流采样通道校准B值*/
static s32 sShuDhgCMapK = 0;                                /*分流器放电电流采样通道校准K值*/
static s32 sShuDhgCMapB = 0;                                /*分流器放电电流采样通道校准B值*/

static s32 sShuChHCMapK = 0;                                /*分流器大充电电流采样通道校准K值*/
static s32 sShuChHCMapB = 0;                                /*分流器大充电电流采样通道校准B值*/
static s32 sShuDhHCMapK = 0;                                /*分流器大放电电流采样通道校准K值*/
static s32 sShuDhHCMapB = 0;                                /*分流器大放电电流采样通道校准B值*/

extern u16 gGCalibrResult_83[];		                        /*校准结果状态信息数组*/
extern u16 gGCalibrCtrl_89[];			                    /*采样校准命令信息数组*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void OutCurrSampCorrInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电流采样校准初始化
** @The notes :
**===============================================================================================*/
void OutCurrSampCorrInit(void)
{
    u32 kbInfo[3] = {0};

    //读取EEP中霍尔输出电流KB值
    if((TRUE == ParaReadStoreHalChgCurrCorrKB(kbInfo, 3)) && (0 != kbInfo[1]))
    {
        sHalOutCMapK = kbInfo[1];
        sHalOutCMapB = kbInfo[2];

        /*单点校准*/
        if(OC_CORR_BLOWUP == sHalOutCMapK)
        {
            BitSet(sCurrCorrType, eOCCorr_HalOutCS);
        }
        /*双点校准*/
        else
        {
            BitSet(sCurrCorrType, eOCCorr_HalOutCD);
        }
    }
    else
    {
    	sHalOutCMapK = OC_CORR_BLOWUP;
    	sHalOutCMapB = 0;
    }

    //读取EEP中分流器充电电流KB值
    if((TRUE == ParaReadStoreShuChgCurrCorrKB(kbInfo, 3)) && (0 != kbInfo[1]))
    {
        sShuChgCMapK = kbInfo[1];
        sShuChgCMapB = kbInfo[2];

        /*单点校准*/
        if(OC_CORR_BLOWUP == sShuChgCMapK)
        {
            BitSet(sCurrCorrType, eOCCorr_ShuChgCS);
        }
        /*双点校准*/
        else
        {
            BitSet(sCurrCorrType, eOCCorr_ShuChgCD);
        }
    }
    else
    {
    	sShuChgCMapK = OC_CORR_BLOWUP;
    	sShuChgCMapB = 0;
    }

    //读取EEP中分流器放电电流KB值
    if((TRUE == ParaReadStoreShuDhgCurrCorrKB(kbInfo, 3)) && (0 != kbInfo[1]))
    {
        sShuDhgCMapK = kbInfo[1];
        sShuDhgCMapB = kbInfo[2];

        /*单点校准*/
        if(OC_CORR_BLOWUP == sShuDhgCMapK)
        {
            BitSet(sCurrCorrType, eOCCorr_ShuDhgCS);
        }
        /*双点校准*/
        else
        {
            BitSet(sCurrCorrType, eOCCorr_ShuDhgCD);
        }
    }
    else
    {
    	sShuDhgCMapK = OC_CORR_BLOWUP;
    	sShuDhgCMapB = 0;
    }

    //读取EEP中分流器充电大电流KB值
    if((TRUE == ParaReadStoreShuChHCurrCorrKB(kbInfo, 3)) && (0 != kbInfo[1]))
    {
        sShuChHCMapK = kbInfo[1];
        sShuChHCMapB = kbInfo[2];

        /*单点校准*/
        if(OC_CORR_BLOWUP == sShuChHCMapK)
        {
            BitSet(sCurrCorrType, eOCCorr_ShuChgCS);
        }
        /*双点校准*/
        else
        {
            BitSet(sCurrCorrType, eOCCorr_ShuChgCD);
        }
    }
    else
    {
    	sShuChHCMapK = OC_CORR_BLOWUP;
    	sShuChHCMapB = 0;
    }

    //读取EEP中分流器放电大电流KB值
    if((TRUE == ParaReadStoreShuDhHCurrCorrKB(kbInfo, 3)) && (0 != kbInfo[1]))
    {
        sShuDhHCMapK = kbInfo[1];
        sShuDhHCMapB = kbInfo[2];

        /*单点校准*/
        if(OC_CORR_BLOWUP == sShuDhHCMapK)
        {
            BitSet(sCurrCorrType, eOCCorr_ShuDhgCS);
        }
        /*双点校准*/
        else
        {
            BitSet(sCurrCorrType, eOCCorr_ShuDhgCD);
        }
    }
    else
    {
    	sShuDhHCMapK = OC_CORR_BLOWUP;
    	sShuDhHCMapB = 0;
    }
}

/*=================================================================================================
** @Name      : void OutCurrSampCorrProcess(void)
** @Input     : void
** @Output    : void
** @Function  : 电流校准处理过程
** @The notes : 1s
**===============================================================================================*/
void OutCurrSampCorrProcess(void)
{
    u8 j = 0;
    u8 step = 0;
    s32 tempC = 0;
    s32 calcK = 0;
    s32 calcB = 0;
    static u8 sCorrSec = 0;
    static u16 sErrTime = 0;
    static u16 sFilTime = 0;
    static s32 sLowStandC = 0;
    static s32 sHighStandC = 0;
    static s32 sFilSumCurr = {0};
    static s32 sLowCurr[OC_CORR_FIL_T] = {0};
    static s32 sHighCurr[OC_CORR_FIL_T] = {0};

    /*手动校零*/
    if(eCORR_HANDZRO == gGCalibrCtrl_89[eCorrCmd89_Type])
    {
    	if((eCORR_HANDZRO != gGCalibrResult_83[eCorrRes83_CorrType])
    		|| (1 != gGCalibrResult_83[eCorrRes83_CorrState]))
    	{
			/*清除校零标志重新校零*/
			ClrCurrZeroFinFlagAPI();
			gGCalibrResult_83[eCorrRes83_CorrType] = eCORR_HANDZRO;
			gGCalibrResult_83[eCorrRes83_CorrState] = 1;
    	}

        /*校零成功*/
    	if(1 == SampGetCurrZeroFinFlagAPI())
    	{
			BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], eCORR_HANDZRO);
			gGCalibrResult_83[eCorrRes83_CorrState] = 100;
			gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
        }
        /*校零失败*/
    	else if(2 == SampGetCurrZeroFinFlagAPI())
		{
			gGCalibrResult_83[eCorrRes83_CorrState] = 101;
		}
    }
    //非调试模式不校准
    if(eMODE_DEBUG != GetGWorkModeAPI())
    {
        //校准数据清零
        gGCalibrResult_83[eCorrRes83_CorrType] = eCORR_INVALID;
        gGCalibrResult_83[eCorrRes83_CorrState] = 0;
    }
//    //有故障则校准失败
//    else if((eErr_B == DiagGetErrLastStateAPI(eERR_ID_CSAMP_ERR))           /*电流采样电路异常*/
//        && ((eCORR_HALOUTS == gGCalibrResult_83[eCorrRes83_CorrType])       /*正在霍尔输出电流单点校准*/
//        || (eCORR_HALOUTD == gGCalibrResult_83[eCorrRes83_CorrType])        /*正在霍尔输出电流双点校准*/
//        || (eCORR_SHUCHGS == gGCalibrResult_83[eCorrRes83_CorrType])        /*正在分流器充电电流单点校准*/
//        || (eCORR_SHUCHGD == gGCalibrResult_83[eCorrRes83_CorrType])        /*正在分流器充电电流双点校准*/
//        || (eCORR_SHUDHGS == gGCalibrResult_83[eCorrRes83_CorrType])        /*正在分流器放电电流单点校准*/
//        || (eCORR_SHUDHGD == gGCalibrResult_83[eCorrRes83_CorrType])))      /*正在分流器放电电流双点校准*/
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
        if((eCORR_HALOUTS == gGCalibrResult_83[eCorrRes83_CorrType])        /*正在霍尔输出电流单点校准*/
            || (eCORR_HALOUTD == gGCalibrResult_83[eCorrRes83_CorrType])    /*正在霍尔输出电流双点校准*/
            || (eCORR_SHUCHGS == gGCalibrResult_83[eCorrRes83_CorrType])    /*正在分流器充电电流单点校准*/
            || (eCORR_SHUCHGD == gGCalibrResult_83[eCorrRes83_CorrType])    /*正在分流器充电电流双点校准*/
            || (eCORR_SHUDHGS == gGCalibrResult_83[eCorrRes83_CorrType])    /*正在分流器放电电流单点校准*/
            || (eCORR_SHUDHGD == gGCalibrResult_83[eCorrRes83_CorrType]))   /*正在分流器放电电流双点校准*/
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
    //正在霍尔输出电流单点校准接收其他校准命令
    else if((eCORR_HALOUTS == gGCalibrResult_83[eCorrRes83_CorrType])       //正在霍尔输出电流单点校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] > 0)                    //已开始校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] < 100)                  //未完成校准
        && (eCORR_HALOUTS != gGCalibrCtrl_89[eCorrCmd89_Type]))             //非霍尔输出电流单点校准命令
    {
        //校准失败
        gGCalibrResult_83[eCorrRes83_CorrState] = 101;
    }
    //正在霍尔放电电流双点校准接收其他校准命令
    else if((eCORR_HALOUTD == gGCalibrResult_83[eCorrRes83_CorrType])       //正在霍尔放电电流双点校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] > 0)                    //已开始校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] < 100)                  //未完成校准
        && (eCORR_HALOUTD != gGCalibrCtrl_89[eCorrCmd89_Type]))             //非霍尔放电电流双点校准命令
    {
        //校准失败
        gGCalibrResult_83[eCorrRes83_CorrState] = 101;
    }
    //正在分流器充电电流双点校准接收其他校准命令
    else if((eCORR_SHUCHGD == gGCalibrResult_83[eCorrRes83_CorrType])       //正在分流器充电电流双点校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] > 0)                    //已开始校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] < 100)                  //未完成校准
        && (eCORR_SHUCHGD != gGCalibrCtrl_89[eCorrCmd89_Type]))             //非分流器充电电流双点校准命令
    {
        //校准失败
        gGCalibrResult_83[eCorrRes83_CorrState] = 101;
    }
    //正在分流器放电电流单点校准接收其他校准命令
    else if((eCORR_SHUDHGS == gGCalibrResult_83[eCorrRes83_CorrType])       //正在分流器放电电流单点校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] > 0)                    //已开始校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] < 100)                  //未完成校准
        && (eCORR_SHUDHGS != gGCalibrCtrl_89[eCorrCmd89_Type]))             //非分流器放电电流单点校准命令
    {
        //校准失败
        gGCalibrResult_83[eCorrRes83_CorrState] = 101;
    }
    //正在分流器放电电流双点校准接收其他校准命令
    else if((eCORR_SHUDHGD == gGCalibrResult_83[eCorrRes83_CorrType])       //正在分流器放电电流双点校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] > 0)                    //已开始校准
        && (gGCalibrResult_83[eCorrRes83_CorrState] < 100)                  //未完成校准
        && (eCORR_SHUDHGD != gGCalibrCtrl_89[eCorrCmd89_Type]))             //非分流器放电电流双点校准命令
    {
        //校准失败
        gGCalibrResult_83[eCorrRes83_CorrState] = 101;
    }
    /*命令正确*/
    else
    {
    	/*正在电流校准*/
		if((gGCalibrCtrl_89[eCorrCmd89_Type] >= eCORR_HALOUTS)
			&& (gGCalibrCtrl_89[eCorrCmd89_Type] <= eCORR_SHUDHGD))
		{
	    	/*当前电流校准状态为步骤*/
			step = gGCalibrResult_83[eCorrRes83_CorrState];

	    	/*之前在电流零点校准*/
			if(eCORR_HANDZRO == gGCalibrResult_83[eCorrRes83_CorrType])
			{
				/*保持在第一步*/
				step = 0;
			}
		}
    	/*非电流校准*/
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
            /*更新电流校准结果*/
        	UpdateOutCurrCorrResult();

            //下发开始电流校准命令
            if((gGCalibrCtrl_89[eCorrCmd89_Type] >= eCORR_HALOUTS)
            	&& (gGCalibrCtrl_89[eCorrCmd89_Type] <= eCORR_SHUDHGD))
            {
                gGCalibrResult_83[eCorrRes83_CorrState] = 1;
                gGCalibrResult_83[eCorrRes83_CorrType] = gGCalibrCtrl_89[eCorrCmd89_Type];
                BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], gGCalibrCtrl_89[eCorrCmd89_Type]);
            }
            break;

        //清除校准参数
        case 1:
            /*清除电流零飘*/
            ClrCurrZeroFinFlagAPI();

            /*霍尔充电电流校准*/
        	if((eCORR_HALOUTS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_HALOUTD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
        		//复位KB参数
                BitClr(sCurrCorrType, eOCCorr_HalOutCS);
                BitClr(sCurrCorrType, eOCCorr_HalOutCD);
                ClrEEPOutCurrCorrKBInfo(0);

                sHalOutCMapK = OC_CORR_BLOWUP;
                sHalOutCMapB = 0;
        	}
            /*分流器充电电流校准*/
        	else if((eCORR_SHUCHGS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_SHUCHGD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
        		//复位KB参数
                BitClr(sCurrCorrType, eOCCorr_ShuChgCS);
                BitClr(sCurrCorrType, eOCCorr_ShuChgCD);

                ClrEEPOutCurrCorrKBInfo(1);
                ClrEEPOutCurrCorrKBInfo(3);

                sShuChgCMapK = OC_CORR_BLOWUP;
                sShuChgCMapB = 0;
                sShuChHCMapK = OC_CORR_BLOWUP;
                sShuChHCMapB = 0;
        	}
            /*分流器放电电流校准*/
        	else
        	{
        		//复位KB参数
                BitClr(sCurrCorrType, eOCCorr_ShuDhgCS);
                BitClr(sCurrCorrType, eOCCorr_ShuDhgCD);

                ClrEEPOutCurrCorrKBInfo(2);
                ClrEEPOutCurrCorrKBInfo(4);

                sShuDhgCMapK = OC_CORR_BLOWUP;
                sShuDhgCMapB = 0;
                sShuDhHCMapK = OC_CORR_BLOWUP;
                sShuDhHCMapB = 0;
        	}

            for(j = 0; j < OC_CORR_FIL_T; j++)
            {
                sLowCurr[j] = 0;
                sHighCurr[j] = 0;
            }

            sErrTime = 0;
            sFilTime = 0;
            gGCalibrCtrl_89[eCorrCmd89_Value] = 0;
            gGCalibrResult_83[eCorrRes83_CorrState] = 2;
            break;

        //等待第一个基准电流
        case 2:
            //下发基准电流
            if(0 != gGCalibrCtrl_89[eCorrCmd89_Value])
            {
                //等待1s再采集
                sErrTime++;
                if(sErrTime >= 1)
                {
                    sErrTime = 0;
                    sFilTime = 0;
                    sLowStandC = 0;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 3;
                }
            }
            break;

        //采集第1个基准电流
        case 3:
            /*霍尔输出电流校准*/
        	if((eCORR_HALOUTS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_HALOUTD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
                /*取霍尔采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCADC_Hall);
                /*取霍尔采样电压值(0.01mV)*/
        		tempC = GetGRealVolt10uVAPI(eCADC_Hall);

//        		//充电负电流
//        		if(tempC < 0)
//        		{
//					//记录基准电流(10mA)
//					sLowStandC = 0 - (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
//        		}
//        		else
//        		{
//					//记录基准电流(10mA)
//					sLowStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
//        		}
				//记录基准电流电压电压(1mV转0.01mV)
				sLowStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value] * 100;
        	}
            /*分流器充电电流校准*/
        	else if((eCORR_SHUCHGS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_SHUCHGD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
                /*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取分流器采样电压值(0.01mV)*/
        		tempC = GetGRealVolt10uVAPI(eCAFE_Shunt);

                //记录基准电流(10mA)
                //sLowStandC = 0 - (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV)
				sLowStandC = 0 - (s32)gGCalibrCtrl_89[eCorrCmd89_Value];

				//小电流≤-110A(1mV对应4A,0.01mV)
				if(sLowStandC >= -2750)
				{
					sCorrSec = 0;
				}
        	}
            /*分流器放电电流校准*/
        	else
        	{
        		/*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取分流器采样电压值(0.01mV)*/
        		tempC = GetGRealVolt10uVAPI(eCAFE_Shunt);

                //记录基准电流(10mA)
                //sLowStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV)
				sLowStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value];

				//小电流≤110A(1mV对应4A,0.01mV)
				if(sLowStandC <= 2750)
				{
					sCorrSec = 0;
				}
        	}

            //采样值与基准值误差50A内
            //if(ABS(tempC, sLowStandC) < 5000)
			//采样值与基准值霍尔误差100mV内/分流器误差10mV内
			if(((ABS(tempC, sLowStandC) < 10000)
				&& ((eCORR_HALOUTS == gGCalibrCtrl_89[eCorrCmd89_Type])
		        || (eCORR_HALOUTD == gGCalibrCtrl_89[eCorrCmd89_Type])))
				|| (ABS(tempC, sLowStandC) < 1000))
            {
               sLowCurr[sFilTime] = tempC;
            }
            //采样电路误差过大或模拟电流输出误差过大
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
            if(sFilTime >= OC_CORR_FIL_T)
            {
                //计算采样通道平均值
                sFilSumCurr = 0;

                for(j = 0; j < OC_CORR_FIL_T; j++)
                {
                    sFilSumCurr += sLowCurr[j];
                }

                sLowCurr[0] =  sFilSumCurr / OC_CORR_FIL_T;

                sFilTime = 0;

                //判断是单点校准还是双点校准
                if((eCORR_HALOUTS == gGCalibrResult_83[eCorrRes83_CorrType])
	                || (eCORR_SHUCHGS == gGCalibrResult_83[eCorrRes83_CorrType])
	                || (eCORR_SHUDHGS == gGCalibrResult_83[eCorrRes83_CorrType]))
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
            if((0 != gGCalibrCtrl_89[eCorrCmd89_Value]) && ((gGCalibrCtrl_89[eCorrCmd89_Value] > sLowStandC)//原值
            	|| ((0 == (sLowStandC % 100)) && (gGCalibrCtrl_89[eCorrCmd89_Value] > (sLowStandC / 100)))))//放大100倍
            {
                //等待1s再采集
                sErrTime++;
                if(sErrTime >= 1)
                {
                    sErrTime = 0;
                    sFilTime = 0;
                    sHighStandC = 0;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 5;
                }
            }
            break;

        //采集第2个基准电压
        case 5:
            /*霍尔输出电流校准*/
        	if((eCORR_HALOUTS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_HALOUTD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
                /*取霍尔采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCADC_Hall);
                /*取霍尔采样电压值(0.01mV)*/
        		tempC = GetGRealVolt10uVAPI(eCADC_Hall);

//        		//充电负电流
//        		/if(tempC < 0)
//        		{
//					//记录基准电流(10mA)
//					sHighStandC = 0 - (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
//        		}
//        		else
//        		{
//					//记录基准电流(10mA)
//					sHighStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
//        		}
        		sHighStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value] * 100;
        	}
            /*分流器充电电流校准*/
        	else if((eCORR_SHUCHGS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_SHUCHGD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
                /*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取霍尔采样电压值(0.01mV)*/
        		tempC = GetGRealVolt10uVAPI(eCAFE_Shunt);

                //记录基准电流(10mA)
        		//sHighStandC = 0 - (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV)
        		sHighStandC = 0 - (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
        	}
            /*分流器放电电流校准*/
        	else
        	{
        		/*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取霍尔采样电压值(0.01mV)*/
        		tempC = GetGRealVolt10uVAPI(eCAFE_Shunt);

                //记录基准电流(10mA)
        		//sHighStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV)
        		sHighStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
        	}

            //采样值与基准值误差50A内
            //if(ABS(tempC, sHighStandC) < 5000)
			//采样值与基准值霍尔误差100mV内/分流器误差10mV内
			if(((ABS(tempC, sHighStandC) < 10000)
				&& ((eCORR_HALOUTS == gGCalibrCtrl_89[eCorrCmd89_Type])
				|| (eCORR_HALOUTD == gGCalibrCtrl_89[eCorrCmd89_Type])))
				|| (ABS(tempC, sHighStandC) < 1000))
            {
                sHighCurr[sFilTime] =tempC;
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
            if(sFilTime >= OC_CORR_FIL_T)
            {
                //计算每个采样通道平均值
                sFilSumCurr = 0;

                for(j = 0; j < OC_CORR_FIL_T; j++)
                {
                    sFilSumCurr += sHighCurr[j];
                }

                sHighCurr[0] =  sFilSumCurr / OC_CORR_FIL_T;

                sFilTime = 0;

                //进入KB值计算
                gGCalibrCtrl_89[eCorrCmd89_Value] = 0;
                gGCalibrResult_83[eCorrRes83_CorrState] = 6;
            }
            break;

        //校准计算处理
        case 6:
            //判断是单点校准还是双点校准
        	if((eCORR_HALOUTS == gGCalibrResult_83[eCorrRes83_CorrType])
                || (eCORR_SHUCHGS == gGCalibrResult_83[eCorrRes83_CorrType])
                || (eCORR_SHUDHGS == gGCalibrResult_83[eCorrRes83_CorrType]))
            {
                sHighStandC = sLowStandC;
                calcK = OC_CORR_BLOWUP;

                calcB = ((s32)sLowStandC - sLowCurr[0]) * OC_CORR_BLOWUP;
            }
            else
            {
                calcK = (((s32)sHighStandC - (s32)sLowStandC) * 1000) / (((s32)sHighCurr[0] - (s32)sLowCurr[0]));
                //calcB = ((((s32)sHighCurr[0] * sLowStandC * 1000) - ((s32)sLowCurr[0] * sHighStandC * 1000))/((s32)sHighCurr[0] - (s32)sLowCurr[0]));
                calcB = (s32)sHighStandC * 1000 - (s32)sHighCurr[0] * calcK;
            }

            /*霍尔输出电流校准*/
        	if((eCORR_HALOUTS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_HALOUTD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
        		//检测霍尔电流采样校准是否正确
        	    //tempC = (GetGRealCurr10mAAPI(eCADC_Hall) * calcK  + calcB) / OC_CORR_BLOWUP;
        		//检测霍尔电压采样校准是否正确
        	    tempC = (GetGRealVolt10uVAPI(eCADC_Hall) * calcK  + calcB) / OC_CORR_BLOWUP;

                //电流误差超过0.8A
                //if(ABS(sHighStandC, tempC) > 80)
                //电压误差超过5mV
                if(ABS(sHighStandC, tempC) > 500)
                {
                    //校准失败
                    gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 101;
                }
                //校准正确
                else
                {
                    //记录霍尔充电电流校准KB值
                    sHalOutCMapK = calcK;
                    sHalOutCMapB = calcB;
                    RcdEEPOutCurrCorrKBInfo(0);

                    /*单点校准*/
                    if(eCORR_HALOUTS == gGCalibrCtrl_89[eCorrCmd89_Type])
                    {
                        BitSet(sCurrCorrType, eOCCorr_HalOutCS);
                    }
                    /*双点校准*/
                    else
                    {
                        BitSet(sCurrCorrType, eOCCorr_HalOutCD);
                    }

                    gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 100;
                }
        	}
            /*分流器充电电流校准*/
        	else if((eCORR_SHUCHGS == gGCalibrCtrl_89[eCorrCmd89_Type])
        		|| (eCORR_SHUCHGD == gGCalibrCtrl_89[eCorrCmd89_Type]))
        	{
        		//检测分流器电流采样校准是否正确
        	    //tempC = (GetGRealCurr10mAAPI(eCAFE_Shunt) * calcK  + calcB) / OC_CORR_BLOWUP;
        		//检测分流器电压采样校准是否正确
        	    tempC = (GetGRealVolt10uVAPI(eCAFE_Shunt) * calcK  + calcB) / OC_CORR_BLOWUP;

                //电流误差超过0.8A
                //if(ABS(sHighStandC, tempC) > 80)
				//电压误差超过0.5mV
				if(ABS(sHighStandC, tempC) > 50)
                {
                    //校准失败
                    gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 101;
                }
                //校准正确
                else
                {
    				//小电流≤110A
    				if(0 == sCorrSec)
    				{
                        //记录分流器充电小电流校准KB值
                        sShuChgCMapK = calcK;
                        sShuChgCMapB = calcB;
                        RcdEEPOutCurrCorrKBInfo(1);

                        //返回重新校准大电流
                        sCorrSec = 1;
	                    gGCalibrResult_83[eCorrRes83_CorrState] = 2;
    				}
    				else
    				{
						//记录分流器充电大电流校准KB值
						sShuChHCMapK = calcK;
						sShuChHCMapB = calcB;
						RcdEEPOutCurrCorrKBInfo(3);
                        sCorrSec = 0;

	                    /*单点校准*/
	                    if(eCORR_SHUCHGS == gGCalibrCtrl_89[eCorrCmd89_Type])
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_ShuChgCS);
	                    }
	                    /*双点校准*/
	                    else
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_ShuChgCD);
	                    }

	                    gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
	                    gGCalibrResult_83[eCorrRes83_CorrState] = 100;
    				}
                }
        	}
            /*分流器放电电流校准*/
        	else
        	{
        		//检测分流器电流采样校准是否正确
        	    //tempC = (GetGRealCurr10mAAPI(eCAFE_Shunt) * calcK  + calcB) / OC_CORR_BLOWUP;
        		//检测分流器电压采样校准是否正确
        	    tempC = (GetGRealVolt10uVAPI(eCAFE_Shunt) * calcK  + calcB) / OC_CORR_BLOWUP;

                //电流误差超过0.8A
                //if(ABS(sHighStandC, tempC) > 80)
				//电压误差超过0.5mV
				if(ABS(sHighStandC, tempC) > 50)
                {
                    //校准失败;
                    gGCalibrCtrl_89[eCorrCmd89_Type] = eCORR_INVALID;
                    gGCalibrResult_83[eCorrRes83_CorrState] = 101;
                }
                //校准正确
                else
                {
    				//小电流≤110A
    				if(0 == sCorrSec)
    				{
						//记录分流器放电小电流校准KB值
						sShuDhgCMapK = calcK;
						sShuDhgCMapB = calcB;
						RcdEEPOutCurrCorrKBInfo(2);

                        //返回重新校准大电流
                        sCorrSec = 1;
	                    gGCalibrResult_83[eCorrRes83_CorrState] = 2;
    				}
    				else
    				{
						//记录分流器放电大电流校准KB值
						sShuDhHCMapK = calcK;
						sShuDhHCMapB = calcB;
						RcdEEPOutCurrCorrKBInfo(4);
                        sCorrSec = 0;

	                    /*单点校准*/
	                    if(eCORR_SHUDHGS == gGCalibrCtrl_89[eCorrCmd89_Type])
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_ShuDhgCS);
	                    }
	                    /*双点校准*/
	                    else
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_ShuDhgCD);
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
            /*更新电流校准结果*/
        	UpdateOutCurrCorrResult();

            //下发开始电流校准命令
            if((eCORR_HALOUTS == gGCalibrCtrl_89[eCorrCmd89_Type])
            	|| (eCORR_HALOUTD == gGCalibrCtrl_89[eCorrCmd89_Type])
            	|| (eCORR_SHUCHGS == gGCalibrCtrl_89[eCorrCmd89_Type])
            	|| (eCORR_SHUCHGD == gGCalibrCtrl_89[eCorrCmd89_Type])
            	|| (eCORR_SHUDHGS == gGCalibrCtrl_89[eCorrCmd89_Type])
            	|| (eCORR_SHUDHGD == gGCalibrCtrl_89[eCorrCmd89_Type]))
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
** @Name      : s32 CorrGetCurrCorrKBValue(e_OCCorrValue index)
** @Input     : index:电流校准KB值索引
** @Output    : 当前电流校准KB值
** @Function  : 获取当前电流校准KB值
** @The notes : 放大1000(OC_CORR_BLOWUP)倍率后的值
**===============================================================================================*/
s32 CorrGetCurrCorrKBValue(e_OCCorrValue index)
{
    s32 value = 0;

    if(eOCCorr_HalOutK == index)
    {
    	value = sHalOutCMapK;
    }
    else if(eOCCorr_HalOutB == index)
    {
    	value = sHalOutCMapB;
    }
    else if(eOCCorr_ShuChgK == index)
    {
    	value = sShuChgCMapK;
    }
    else if(eOCCorr_ShuChgB == index)
    {
    	value = sShuChgCMapB;
    }
    else if(eOCCorr_ShuDhgK == index)
    {
    	value = sShuDhgCMapK;
    }
    else if(eOCCorr_ShuDhgB == index)
    {
    	value = sShuDhgCMapB;
    }
    else
    {
        return(0);
    }

    return(value);
}

/*=================================================================================================
** @Name      : s32 CorrRealHalCurrByKB(s32 curr)
** @Input     : volt:修正前采样电流值 10mA
** @Output    : 修正后实际电流值  0.01A
** @Function  : 通过KB值修正霍尔实际电流采样值
** @The notes :
**===============================================================================================*/
s32 CorrRealHalCurrByKB(s32 curr)
{
    s32 realCurr = 0;

    //霍尔电流通道已校准
    if((1 == BitGet(sCurrCorrType, eOCCorr_HalOutCS))
    	|| (1 == BitGet(sCurrCorrType, eOCCorr_HalOutCD)))
    {
        //按KB值计算
    	realCurr = ((s32)curr * sHalOutCMapK + sHalOutCMapB) / OC_CORR_BLOWUP;
    }
    //霍尔通道未校准
    else
    {
        //返回原始值
    	realCurr = curr;
    }

    return(realCurr);
}

/*=================================================================================================
** @Name      : s32 CorrRealShuCurrByKB(s32 curr)
** @Input     : volt:修正前采样电压值 0.01mV
** @Output    : 修正后实际电流值  0.01A
** @Function  : 通过KB值修正分流器实际电流采样值
** @The notes :
**===============================================================================================*/
s32 CorrRealShuCurrByKB(s32 curr)
{
    s32 realCurr = 0;

    //充电电流且分流器充电通道已校准
    if((curr < 0)
    	&& ((1 == BitGet(sCurrCorrType, eOCCorr_ShuChgCS))
		|| (1 == BitGet(sCurrCorrType, eOCCorr_ShuChgCD))))
    {
		if(curr >= -3250)//130A内考虑小电流KB值
		{
			if(curr >= -2750)//110A内只用小电流KB值
			{
		        //按KB值计算
		    	realCurr = ((s32)curr * sShuChgCMapK + sShuChgCMapB) / OC_CORR_BLOWUP;
			}
			else//110A~130A内用大小电流KB值算平均值
			{
		        //按KB值计算
		    	realCurr = ((s32)curr * sShuChgCMapK + sShuChgCMapB) / OC_CORR_BLOWUP;

		    	//按KB值计算
		    	realCurr = (realCurr + (((s32)curr * sShuChHCMapK + sShuChHCMapB) / OC_CORR_BLOWUP)) / 2;
			}
		}
		else//130A以上考虑大电流KB值
		{
	        //按KB值计算
	    	realCurr = ((s32)curr * sShuChHCMapK + sShuChHCMapB) / OC_CORR_BLOWUP;
		}

        //不能变为放电电流
        if(realCurr > 0)
        {
        	realCurr = 0;
        }
    }
    //放电电流且分流器放电通道已校准
    else if((curr > 0)
		&& ((1 == BitGet(sCurrCorrType, eOCCorr_ShuDhgCS))
		|| (1 == BitGet(sCurrCorrType, eOCCorr_ShuDhgCD))))
    {
		if(curr <= 3250)//130A内考虑小电流KB值
		{
			if(curr <= 2750)//110A内只用小电流KB值
			{
		        //按KB值计算,10mA转0.1A
		    	realCurr = ((s32)curr * sShuDhgCMapK + sShuDhgCMapB) / OC_CORR_BLOWUP;
			}
			else//110A~130A内用大小电流KB值算平均值
			{
		        //按KB值计算,10mA转0.1A
		    	realCurr = ((s32)curr * sShuDhgCMapK + sShuDhgCMapB) / OC_CORR_BLOWUP;

		        //按KB值计算,10mA转0.1A
		    	realCurr = (realCurr + (((s32)curr * sShuDhHCMapK + sShuDhHCMapB) / OC_CORR_BLOWUP)) / 2;
			}
		}
		else//130A以上考虑大电流KB值
		{
	        //按KB值计算,10mA转0.1A
	    	realCurr = ((s32)curr * sShuDhHCMapK + sShuDhHCMapB) / OC_CORR_BLOWUP;
		}

        //不能变为充电电流
        if(realCurr < 0)
        {
        	realCurr = 0;
        }
    }
    //分流器通道未校准
    else
    {
    	//返回原始值
    	realCurr = curr;
    }

    return(realCurr);
}

/*=================================================================================================
** @Name      : void ClrEEPOutCurrCorrKBInfo(u8 chan)
** @Input     : chan:电流通道 0:霍尔电流 1:分流器充电电流 2:分流器放电电流
** @Output    : void
** @Function  : 清除EEP电流采样校准KB值
** @The notes :
**===============================================================================================*/
void ClrEEPOutCurrCorrKBInfo(u8 chan)
{
	u32 flag[3] = {0xffffffff, 0xffffffff, 0xffffffff};

	/*霍尔输出电流(霍尔电流不区分充放电,使用充电记录位置)*/
	if(0 == chan)
	{
        /*清除EEP值*/
		ParaWriteStoreHalChgCurrCorrKB(0, flag, 3);
	}
	/*分流器充电电流*/
	else if(1 == chan)
	{
		/*清除EEP值*/
		ParaWriteStoreShuChgCurrCorrKB(0, flag, 3);
	}
	/*分流器放电电流*/
	else if(2 == chan)
	{
		/*清除EEP值*/
		ParaWriteStoreShuDhgCurrCorrKB(0, flag, 3);
	}
	/*分流器大充电电流*/
	else if(3 == chan)
	{
		/*清除EEP值*/
		ParaWriteStoreShuChHCurrCorrKB(0, flag, 3);
	}
	/*分流器大放电电流*/
	else if(4 == chan)
	{
		/*清除EEP值*/
		ParaWriteStoreShuDhHCurrCorrKB(0, flag, 3);
	}
}

/*=================================================================================================
** @Name      : void RcdEEPOutCurrCorrKBInfo(u8 chan)
** @Input     : chan:电流通道 0:霍尔电流 1:分流器充电电流 2:分流器放电电流
** @Output    : void
** @Function  : 记录电流校准KB值到EEP
** @The notes :
**===============================================================================================*/
void RcdEEPOutCurrCorrKBInfo(u8 chan)
{
	u32 flag = PARA_CFG_FLAG_U32;

	/*霍尔输出电流(霍尔电流不区分充放电,使用充电记录位置)*/
	if(0 == chan)
	{
		/*修改EEP值*/
		ParaWriteStoreHalChgCurrCorrKB(1, (u32*)&sHalOutCMapK, 1);
		ParaWriteStoreHalChgCurrCorrKB(2, (u32*)&sHalOutCMapB, 1);
		ParaWriteStoreHalChgCurrCorrKB(0, &flag, 1);
	}
	/*分流器充电电流*/
	else if(1 == chan)
	{
		/*修改EEP值*/
		ParaWriteStoreShuChgCurrCorrKB(1, (u32*)&sShuChgCMapK, 1);
		ParaWriteStoreShuChgCurrCorrKB(2, (u32*)&sShuChgCMapB, 1);
		ParaWriteStoreShuChgCurrCorrKB(0, &flag, 1);
	}
	/*分流器放电电流*/
	else if(2 == chan)
	{
		/*修改EEP值*/
		ParaWriteStoreShuDhgCurrCorrKB(1, (u32*)&sShuDhgCMapK, 1);
		ParaWriteStoreShuDhgCurrCorrKB(2, (u32*)&sShuDhgCMapB, 1);
		ParaWriteStoreShuDhgCurrCorrKB(0, &flag, 1);
	}
	/*分流器充电电流*/
	else if(3 == chan)
	{
		/*修改EEP值*/
		ParaWriteStoreShuChHCurrCorrKB(1, (u32*)&sShuChHCMapK, 1);
		ParaWriteStoreShuChHCurrCorrKB(2, (u32*)&sShuChHCMapB, 1);
		ParaWriteStoreShuChHCurrCorrKB(0, &flag, 1);
	}
	/*分流器放电电流*/
	else if(4 == chan)
	{
		/*修改EEP值*/
		ParaWriteStoreShuDhHCurrCorrKB(1, (u32*)&sShuDhHCMapK, 1);
		ParaWriteStoreShuDhHCurrCorrKB(2, (u32*)&sShuDhHCMapB, 1);
		ParaWriteStoreShuDhHCurrCorrKB(0, &flag, 1);
	}
}

/*=================================================================================================
** @Name      : void UpdateOutCurrCorrResult(void)
** @Input     : void
** @Output    : void
** @Function  : 更新电流校准结果
** @The notes : 根据校准标志位
**===============================================================================================*/
void UpdateOutCurrCorrResult(void)
{
    /*电流零点已校准*/
	if(1 == SampGetCurrZeroFinFlagAPI())
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_HANDZRO - 1));
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_HANDZRO - 1));
	}

    /*霍尔输出电流单点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_HalOutCS))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_HALOUTS - 1));
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_HALOUTS - 1));
	}

    /*霍尔输出电流双点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_HalOutCD))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_HALOUTD - 1));
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_HALOUTD - 1));
	}

    /*分流器充电电流单点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_ShuChgCS))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_SHUCHGS - 1));
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_SHUCHGS - 1));
	}

    /*分流器充电电流双点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_ShuChgCD))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_SHUCHGD - 1));
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_SHUCHGD - 1));
	}

    /*分流器放电电流单点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_ShuDhgCS))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_SHUDHGS - 1));
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_SHUDHGS - 1));
	}

    /*分流器放电电流双点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_ShuDhgCD))
	{
		BitSet(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_SHUDHGD - 1));
	}
	/*未校准*/
	else
	{
		BitClr(gGCalibrResult_83[eCorrRes83_CorrResult], (eCORR_SHUDHGD - 1));
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

