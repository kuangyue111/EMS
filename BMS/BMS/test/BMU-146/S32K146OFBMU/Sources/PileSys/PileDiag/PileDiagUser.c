/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileDiagUser.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控故障诊断用户接口模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicPara.h"
#include "PublicInfo.h"
#include "PileInfo.h"
#include "PileGroupRcv.h"
#include "PileTactics.h"
#include "PileAutoCtrl.h"
#include "PileDiagCheck.h"
#include "PileDiagMaker.h"
#include "PileDiagUser.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gPAlarmInfo_53[];			/*告警信息数组*/
extern u16 gPFaultInfo_54[]; 			/*故障信息数组*/
extern u16 gPNotesInfo_55[];			/*提示信息数组*/
extern u16 gPStatistErr_71[]; 			/*故障统计信息数组*/

extern u16 gSysGenParaRO_100[];  		/*系统通用参数设置*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
//=============================================================================================
//函数名称	: void PileErrStatistTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆故障告警统计任务
//注意事项	: 100ms
//=============================================================================================
void PileErrStatistTask(void)
{
    u8 i = 0;
    u8 num = 0;
    u32 monAlarm = 0;
    u32 monFault = 0;
    u32 hisAlarm = 0;
    u32 hisFault = 0;
    
    //初始化
    if(eWORK_INIT == GetPileWorkStateAPI())
    {
        //上传0
    	gPStatistErr_71[ePErr71_MonWarNum] = 0;
    	gPStatistErr_71[ePErr71_MonErrNum] = 0;
    	gPStatistErr_71[ePErr71_HisWarNum] = 0;
    	gPStatistErr_71[ePErr71_HisErrNum] = 0;
    }
    //非初始化
    else
    {
        //检测所有电池组
        for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
        {
            //计算使能电池组
            if((1 == GetPGroupEnStateAPI(i))
                && (0 == GetPGroupCanOffStateAPI(i)))
            {
                num++;
                monAlarm += GetPGroupMonWarNumAPI(i);
                monFault += GetPGroupMonErrNumAPI(i);
                hisAlarm += GetPGroupHisWarNumAPI(i);
                hisFault += GetPGroupHisErrNumAPI(i);
            }
        }
        
        if(num > 0)
        {
        	gPStatistErr_71[ePErr71_MonWarNum] = monAlarm;
        	gPStatistErr_71[ePErr71_MonErrNum] = monFault;
        	gPStatistErr_71[ePErr71_HisWarNum] = hisAlarm;
        	gPStatistErr_71[ePErr71_HisErrNum] = hisFault;
        }
    }
}
#endif

//=============================================================================================
//函数名称	: u8 GetPileNowErrSortLevelAPI(void)
//输入参数	: void
//输出参数	: 电池堆当前故障告警级别[0:无故障告警,1:提示类告警，2:充电限制类告警,3:放电限制类告警,4:充放电限制类告警,5:充电保护类故障,6:放电保护类故障,7:充放电保护类故障,8:系统故障类故障]
//函数功能	: 获取电池堆当前故障告警级别
//注意事项	: 
//=============================================================================================
u8 GetPileNowErrSortLevelAPI(void)
{
    //故障状态保护故障
    /*if((1 == BitGet(gPAlarmInfo_53[2], (ePWAR_ID_P_NO_NOR_G - 32)))
        || (1 == BitGet(gPAlarmInfo_53[2], (ePWAR_ID_PUP_PRE_FAIL - 32)))
        || (1 == BitGet(gPAlarmInfo_53[2], (ePWAR_ID_P_START_ERR - 32)))
        || (1 == BitGet(gPAlarmInfo_53[2], (ePWAR_ID_P_STOP_OC - 32)))
        || (1 == BitGet(gPAlarmInfo_53[2], (ePWAR_ID_P_STOP_ERR - 32)))
        || (1 == BitGet(gPAlarmInfo_53[2], (ePWAR_ID_P_AUTDYN_ERR - 32)))
        || (1 == BitGet(gPAlarmInfo_53[2], (ePWAR_ID_SW_EM_STOP - 32)))
        || (1 == BitGet(gPAlarmInfo_53[3], (ePWAR_ID_BUT_EM_STOP - 48)))
        || (1 == BitGet(gPAlarmInfo_53[3], (ePWAR_ID_THUND_EXP - 48)))
        || (1 == BitGet(gPAlarmInfo_53[3], (ePWAR_ID_WATER_WAR - 48)))
        || (1 == BitGet(gPAlarmInfo_53[3], (ePWAR_ID_FIRE_SMOG_WAR - 48)))
        || (1 == BitGet(gPAlarmInfo_53[3], (ePWAR_ID_FIRE_BEON_ERR - 48)))
        || (1 == BitGet(gPAlarmInfo_53[3], (ePWAR_ID_SYS_LEAK_ERR - 48)))
        || (1 == BitGet(gPAlarmInfo_53[3], (ePWAR_ID_COMM_OFF_PCS - 48))))*/
    if(((gPAlarmInfo_53[2] & 0x6f20) > 0)
        || ((gPAlarmInfo_53[3] & 0x06cd) > 0))
    {
        return(eErr_SysErr);
    }

    //电池组故障保护故障(一组有问题整堆故障保护)
    if((gPFaultInfo_54[1] > 0)		//动力线断线
        || (gPFaultInfo_54[2] > 0)	//组接触器控制失败
        || (gPFaultInfo_54[3] > 0)) //组接触器状态异常
    {
        return(eErr_SysErr);
    }

    //电池组通信中断故障(无故障切除)
    if((gPFaultInfo_54[0] > 0)
    	&& (0 == gSysGenParaRO_100[eSGenPara100_ErrSepa]))
    {
        return(eErr_SysErr);
    }
    
    //充放电保护故障
    /*if((1 == BitGet(gPAlarmInfo_53[2], (ePWAR_ID_P_LACK_NOR_G - 32)))
        || (1 == BitGet(gPAlarmInfo_53[2], (ePWAR_ID_G_SUMV_DIFF - 32)))
	    || (1 == BitGet(gPAlarmInfo_53[3], (ePWAR_ID_P_DCISO_OFF - 48))))*/
    if(/*((gPAlarmInfo_53[2] & 0x0040) > 0)   	//可用电池组不足堆保护(无故障组上电时可用)
        || ((gPAlarmInfo_53[2] & 0x0080) > 0)   //组间总压压差过大堆保护(动态并机使能时可用)
        || */((gPAlarmInfo_53[3] & 0x0002) > 0))//电池堆断路器断开堆保护
    {
        return(eErr_SysPro);
    }

    //充放电限制故障
    if(1 == BitGet(gPAlarmInfo_53[2], (ePWAR_ID_SYS_LEAK_WAR - 32)))
    {
        return(eErr_SysLim);
    }

    //提示类故障
    if((gPAlarmInfo_53[2] > 0)
    	|| (gPAlarmInfo_53[3] > 0)
		|| (gPFaultInfo_54[0] > 0))
    {
        return(eErr_Warm);
    }

    return(eErr_Norm);
}

//=============================================================================================
//函数名称	: u8 GetPileGErrSysProStateAPI(void)
//输入参数	: void
//输出参数	: 组故障进行系统故障保护状态: 0无故障保护 1发生故障保护
//函数功能	: 获取电池堆内组故障进行系统故障保护状态
//注意事项	:
//=============================================================================================
u8 GetPileGErrSysProStateAPI(void)
{
    u8 i = 0;

    //判定电池组是否产生一组故障整体停机
    for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
    {
        //已激活且通讯正常及断路器闭合电池组
        if((1 == GetPGroupEnStateAPI(i))                    //该组激活使能
             && (0 == GetPGroupCanOffStateAPI(i))           //该组通讯正常
             && (1 == GetPGroupIsoSwhStateAPI(i)))          //隔离开关吸合
        {
            //组烟雾报警
            if(1 == GetPGroupRcvErrStateAPI(i, eGERR_ID_BATT_SMOG))
            {
                return(1);
            }

            //组按钮急停
            if(1 == GetPGroupRcvErrStateAPI(i, eGERR_ID_IO_EMERG))
            {
                return(1);
            }

            //组运行接触器回检异常
            if((1 == GetPGroupRcvErrStateAPI(i, eGERR_ID_K0RE_EXP))       //主正HS板级回检异常
                || (1 == GetPGroupRcvErrStateAPI(i, eGERR_ID_K1RE_EXP))   //预充HS板级回检异常
                || (1 == GetPGroupRcvErrStateAPI(i, eGERR_ID_K2RE_EXP))   //主负HS板级回检异常
                || (1 == GetPGroupRcvErrStateAPI(i, eGERR_ID_L0RE_EXP))   //主正LS板级回检异常
                || (1 == GetPGroupRcvErrStateAPI(i, eGERR_ID_L2RE_EXP))   //主负LS板级回检异常
                || (1 == GetPGroupRcvErrStateAPI(i, eGERR_ID_K0MAI_EXP))  //主正外部状态异常
                || (1 == GetPGroupRcvErrStateAPI(i, eGERR_ID_K1PRE_EXP))  //预充外部状态异常
                || (1 == GetPGroupRcvErrStateAPI(i, eGERR_ID_K2NEG_EXP))) //主负外部状态异常
            {
                return(1);
            }
        }
    }

    return(0);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
