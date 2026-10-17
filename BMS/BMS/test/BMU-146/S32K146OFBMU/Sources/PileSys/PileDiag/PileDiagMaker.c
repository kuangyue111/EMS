/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileDiagMaker.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控故障诊断故障码设置接口模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicPara.h"
#include "PileSysState.h"
#include "PileInfo.h"
#include "PileGroupRcv.h"
#include "PileTactics.h"
#include "PileAutoCtrl.h"
#include "PileProtect.h"
#include "PileDiagUser.h"
#include "PileDiagCheck.h"
#include "PileDiagMaker.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gPAlarmInfo_53[];			/*告警信息数组*/
extern u16 gPFaultInfo_54[]; 			/*故障信息数组*/
extern u16 gPNotesInfo_55[];			/*提示信息数组*/

extern u16 gSysGenParaRO_100[];  		/*系统通用参数设置*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//============================================================================================= 
//函数名称	: void PileWarCodeMakerTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆告警码检测任务
//注意事项	: 100ms
//=============================================================================================
void PileWarCodeMakerTask(void)
{
	u8 i = 0;
	u8 group = 0;
	u8 runNum = 0;
	u16 warInfo[2] = {0};

	/*将运行电池组告警整合成电池堆告警*/
    runNum = GetPileRunGroupNumAPI();                   //正在运行的电池组
    if(runNum > 0)                                      //有正在运行的组
    {
        //整合运行组告警
        for(i = 0; i < runNum; i++)
        {
            group = GetPileRunGroupIdAPI(i);            //获取正在运行的电池组号
            warInfo[0] |= GetPGroupRcvWarInfoAPI(group, 0);
            warInfo[1] |= GetPGroupRcvWarInfoAPI(group, 1);
        }
    }
    else if(((ePRO_WAIT != GetPileProStateAPI())        //非未就绪状态(有异常)
		&& (ePRO_READY != GetPileProStateAPI()))        //非就绪状态(有异常)
		|| ((0 == gSysGenParaRO_100[eSGenPara100_ErrSepa])  //禁止故障切除
		&& (0 == gSysGenParaRO_100[eSGenPara100_ChgSepa])   //禁止充电切除
		&& (0 == gSysGenParaRO_100[eSGenPara100_DhgSepa]))) //禁止放电切除
    {
        //整合激活使能组告警
        for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
        {
        	if(1 == GetPGroupEnStateAPI(i))
        	{
        		warInfo[0] |= GetPGroupRcvWarInfoAPI(i, 0);
        		warInfo[1] |= GetPGroupRcvWarInfoAPI(i, 1);
        	}
        }
    }
    else                                                //无正在运行的组
    {
        warInfo[0] = 0;
        warInfo[1] = 0;
    }

    //电池组运行组告警
    gPAlarmInfo_53[0] = warInfo[0];
    gPAlarmInfo_53[1] = warInfo[1];

	//激活未运行的电池组
	if(eWORK_RUN == GetPileWorkStateAPI())
	{
		gPNotesInfo_55[0] = GetPAllGroupEnStateAPI() & (~GetPAllGroupRunStateAPI());
	}
	else
	{
		gPNotesInfo_55[0] = 0;
	}

	//充电切除的电池组
	gPNotesInfo_55[1] = GetPileChgSepGroup();

	//放电切除的电池组
	gPNotesInfo_55[2] = GetPileDhgSepGroup();

	//故障保护切除的电池组
	gPNotesInfo_55[2] = GetPileErrSepGroup();
}

//=============================================================================================
//函数名称	: void PileErrCodeMakerTask(void)
//输入参数	: void 
//输出参数	: void
//函数功能	: 电池堆故障告警码检测任务
//注意事项	: 100ms 
//============================================================================================= 
void PileErrCodeMakerTask(void)
{
	u16 warInfo[2] = {0};
    static u8 sChgLimTime = 0;
    static u8 sDhgLimTime = 0;
    static u16 sNoCanUseTime = 0;
    static u16 sLaCanUseTime = 0;
    static u16 sGSwhErrTime = 0;
    static u16 sGSwhClrTime = 0;

    //Bit34:堆控设备地址冲突
    if(GetPileAddrRepErrFlag() > 0)
    {
        BitSet(warInfo[0], (ePWAR_ID_SMU_DEV_REP - 32));
    }
    else
    {
        //BitClr(warInfo[0], (ePWAR_ID_SMU_DEV_REP - 32));
    }

    //电池堆有充电功率
    if(GetPileOutSumPowerAPI() < 0)
    {
        //充电限值很小而功率超过5kW或限值大而功率超过限值(误差2.0kW)
        if(((GetPileChgLimPowerAPI() <= 30) && (GetPileOutSumPowerAPI() <= (-50)))
            || ((GetPileChgLimPowerAPI() > 30) && (GetPileOutSumPowerAPI() < (-20 - (s16)GetPileChgLimPowerAPI()))))
        {
            //Bit35:充电过功率超时
            if(sChgLimTime > PERR_SYS_CHG_OP_T)
            {
                BitSet(warInfo[0], (ePWAR_ID_P_CHG_OP - 32));
            }
            else
            {
                sChgLimTime++;
            }
        }
        //充电功率正常
        else
        {
            sChgLimTime = 0;
            //BitClr(warInfo[0], (ePWAR_ID_P_CHG_OP - 32));
        }

        //充电状态清除放电超限告警
        sDhgLimTime = 0;
        //BitClr(warInfo[0], (ePWAR_ID_P_DHG_OP - 32));
    }
    //电池堆有放电功率
    else if(GetPileOutSumPowerAPI() > 0)
    {
        //放电限值很小而功率超过5kW或限值大而功率超过限值(误差1.5kW)
        if(((GetPileDhgLimPowerAPI() <= 30) && (GetPileOutSumPowerAPI() >= 50))
            || ((GetPileDhgLimPowerAPI() > 30) && (GetPileOutSumPowerAPI() > (20 + (s16)GetPileDhgLimPowerAPI()))))
        {
            //Bit36:放电过功率超时
            if(sDhgLimTime > PERR_SYS_DHG_OP_T)
            {
                BitSet(warInfo[0], (ePWAR_ID_P_DHG_OP - 32));
            }
            else
            {
                sDhgLimTime++;
            }
        }
        //放电功率正常
        else
        {
            sDhgLimTime = 0;
            //BitClr(warInfo[0], (ePWAR_ID_P_DHG_OP - 32));
        }

        //放电状态清除充电超限告警
        sDhgLimTime = 0;
        //BitClr(warInfo[0], (ePWAR_ID_P_CHG_OP - 32));
    }
    //非充放电状态
    else
    {
        sChgLimTime = 0;
        sDhgLimTime = 0;
        //BitClr(warInfo[0], (ePWAR_ID_P_CHG_OP - 32));
        //BitClr(warInfo[0], (ePWAR_ID_P_DHG_OP - 32));
    }

    //系统正常可用电池组组数为0
    if(0 == GetPileNorGroupNumAPI())
    {
        //检测超时
        if(sNoCanUseTime > PERR_SYS_NO_NOR_G_T)
        {
            //Bit37:系统无可用电池组
            BitSet(warInfo[0], (ePWAR_ID_P_NO_NOR_G - 32));
        }
        else
        {
            sNoCanUseTime++;
        }
    }
    //有可用电池组
    else
    {
        sNoCanUseTime = 0;
        //BitClr(warInfo[0], (ePWAR_ID_P_NO_NOR_G - 32));

        //启动判断可启动电池组不足(正常电池组不足或正常电池组压差过大)
        if(1 == BitGet(GetPileSysStartErrFlag(), e_CtrlExp_LaOKGroup)
            || (1 == BitGet(GetPileSysStartErrFlag(), e_CtrlExp_SumVDiff)))
        {
            //Bit38:系统可用电池组不足
            BitSet(warInfo[0], (ePWAR_ID_P_LACK_NOR_G - 32));
        }
        //有电池组故障导致不满足最小运行组数
        else if((2 == GetPileAllGSysStateAPI())
        	|| (GetPileNorGroupNumAPI() < gSysGenParaRO_100[eSGenPara100_MinGNum]))
        {
            //检测超时
            if(sLaCanUseTime > PERR_SYS_LACK_NOR_G_T)
            {
                //系统可用电池组不足
                BitSet(warInfo[0], (ePWAR_ID_P_LACK_NOR_G - 32));
            }
            else
            {
                sLaCanUseTime++;
            }
        }
        else
        {
            //BitClr(warInfo[0], (ePWAR_ID_P_LACK_NOR_G - 32));
        }
    }

    //Bit39:电池组间压压差过大
    if(GetPileGSumVDiffFlag() > 0)
    {
        BitSet(warInfo[0], (ePWAR_ID_G_SUMV_DIFF - 32));
    }
    else
    {
        //BitClr(warInfo[0], (ePWAR_ID_G_SUMV_DIFF - 32));
    }
    
    //Bit40:电池堆启动预充失败(环流过大)
    if(1 == BitGet(GetPileSysStartErrFlag(), e_CtrlExp_UpCOver))
    {
        BitSet(warInfo[0], (ePWAR_ID_PUP_PRE_FAIL - 32));
    }
    else
    {
        //BitClr(warInfo[0], (ePWAR_ID_PUP_PRE_FAIL - 32));
    }
    
    //Bit41:电池堆启动控制失败(控制接触器失败或电池组故障)
    /*if((1 == BitGet(GetPileSysStartErrFlag(), e_CtrlExp_UpPChgCmd))
        || (1 == BitGet(GetPileSysStartErrFlag(), e_CtrlExp_UpMainCmd))
        || (1 == BitGet(GetPileSysStartErrFlag(), e_CtrlExp_DnPChgCmd))
        || (1 == BitGet(GetPileSysStartErrFlag(), e_CtrlExp_UpGroupErr)))*/
    if(GetPileSysStartErrFlag() > 0)
    {
        BitSet(warInfo[0], (ePWAR_ID_P_START_ERR - 32));
    }
    else
    {
        //BitClr(warInfo[0], (ePWAR_ID_P_START_ERR - 32));
    }
    
    //Bit42:电池堆带载停机失败
    if(1 == BitGet(GetPileSysStopErrFlag(), e_CtrlExp_DnCOver))
    {
        BitSet(warInfo[0], (ePWAR_ID_P_STOP_OC - 32));
    }
    else
    {
        //BitClr(warInfo[0], (ePWAR_ID_P_STOP_OC - 32));
    }
    
    //Bit43:电池堆停机失败故障
    if((1 == BitGet(GetPileSysStopErrFlag(), e_CtrlExp_DnMainCmd))
        || (1 == BitGet(GetPileSysStopErrFlag(), e_CtrlExp_DnGroupErr)))
    {
        BitSet(warInfo[0], (ePWAR_ID_P_STOP_ERR - 32));
    }
    else
    {
        //BitClr(warInfo[0], (ePWAR_ID_P_STOP_ERR - 32));
    }
    
    //Bit44:电池堆动态并机失败
    if(GetPileAutoParErrFlag() > 0)
    {
        BitSet(warInfo[0], (ePWAR_ID_P_AUTPAR_ERR - 32));
    }
    else
    {
        //BitClr(warInfo[0], (ePWAR_ID_P_AUTPAR_ERR - 32));
    }
    
    //Bit45:电池堆动态切除失败
    if(GetPileAutoSepErrFlag() > 0)
    {
        BitSet(warInfo[0], (ePWAR_ID_P_AUTDYN_ERR - 32));
    }
    else
    {
        //BitClr(warInfo[0], (ePWAR_ID_P_AUTDYN_ERR - 32));
    }

    //Bit46:堆控软件急停
    if(GetPileSysEmergStopFlag() > 0)
    {
        BitSet(warInfo[0], (ePWAR_ID_SW_EM_STOP - 32));
    }
    else
    {
        //BitClr(warInfo[0], (ePWAR_ID_SW_EM_STOP - 32));
    }

    //Bit47:电池堆自行保护
    if(TacGetPileHappenProFlagAPI() > 0)
    {
        BitSet(warInfo[0], (ePWAR_ID_P_SELF_PRO - 32));
    }
    else
    {
        //BitClr(warInfo[0], (ePWAR_ID_P_SELF_PRO - 32));
    }

    //Bit58:外部与PCS通讯中断
    if(GetPilePcsCanOffFlag() > 0)
    {
        BitSet(warInfo[1], (ePWAR_ID_COMM_OFF_PCS - 48));
    }
    else
    {
        //BitClr(warInfo[1], (ePWAR_ID_COMM_OFF_PCS - 48));
    }

	//堆控自身告警
    gPAlarmInfo_53[2] = warInfo[0];
    gPAlarmInfo_53[3] = warInfo[1];

	//堆控电池组CAN通讯断线状态
	gPFaultInfo_54[0] = GetPAllGroupCanOffStateAPI();

	//堆控电池组动力线断线状态
	gPFaultInfo_54[1] = GetPileGroupPwrLOffFlag();

	//堆控电池组接触器控制失败状态
	gPFaultInfo_54[2] = GetPileSysCtrlFailGroup();

	//堆控电池组接触器状态跳变状态
	if(GetPileGSwhStateErrFlag() > 0)
	{
		sGSwhClrTime = 0;

		if(sGSwhErrTime >= 30)
		{
			gPFaultInfo_54[3] = GetPileGSwhStateErrFlag();
		}
		else
		{
			sGSwhErrTime++;
		}
	}
	else
	{
		sGSwhErrTime = 0;

		if(sGSwhClrTime >= 80)
		{
			gPFaultInfo_54[3] = 0;
		}
		else
		{
			sGSwhClrTime++;
		}
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
