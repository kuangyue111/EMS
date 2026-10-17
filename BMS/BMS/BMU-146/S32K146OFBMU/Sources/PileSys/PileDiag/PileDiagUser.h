/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileTactInterface.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控故障诊断用户接口模块
** @Instructions : 
**===============================================================================================*/
#ifndef _PILEDIAGUSER_H
#define _PILEDIAGUSER_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicInfo.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Macro definition
**===============================================================================================*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/
/*告警ID号*/
typedef enum
{
	/*只定义堆控用到的主控告警码*/
    eGWAR_ID_CEL_VL2 = 1,                /*单体电压低2级(放电截止)*/
    eGWAR_ID_LEAK2   = 11,               /*电池组绝缘2级(充放电限制)*/

	eGWAR_ID_CEL_VL1 = 17,               /*单体电压低1级(放电保护)*/
    eGWAR_ID_LEAK1   = 11,               /*电池组绝缘1级(充放电保护)*/

	eGWAR_ID_FIN     = 48                /*最大48个告警诊断*/
}e_RcvGWar_ID;

//主控故障列表故障位
typedef enum
{
    //只定义堆控用到的主控故障码
    eGERR_ID_K0RE_EXP   = 24,           /*KM1+回检异常(主正接触器HS回检异常)*/
    eGERR_ID_K1RE_EXP   = 25,           /*KM2回检异常(预充接触器HS回检异常)*/
    eGERR_ID_K2RE_EXP   = 26,           /*KM3+回检异常(负极接触器HS回检异常)*/

    eGERR_ID_L0RE_EXP   = 30,           /*KM1-回检异常(主正接触器LS回检异常)*/
    eGERR_ID_L2RE_EXP   = 31,           /*KM3-回检异常(负极接触器LS回检异常)*/

    eGERR_ID_K0MAI_EXP  = 32,           /*主正接触器状态异常(可能主接触器烧结或不能吸合)*/
    eGERR_ID_K1PRE_EXP  = 33,           /*预充接触器异常(可能预充电阻断开或预充接触器不能吸合)*/
    eGERR_ID_K2NEG_EXP  = 34,           /*负极接触器异常(可能负极接触器烧结或不能吸合)*/

    eGERR_ID_BATT_SMOG  = 37,           /*电池组烟雾报警*/

    eGERR_ID_IO_EMERG   = 43,           /*电池组按钮紧急停机*/

    eGERR_ID_FIN        = 48,           /*电池组故障码结束*/

}e_RcvGErr_ID;

//堆控告警列表告警位
typedef enum
{
	/*以下为电池2级告警ID*/
    ePWAR_ID_CEL_VH2 = 0,               /*单体电压高2级(充电截止)*/
    ePWAR_ID_CEL_VL2 = 1,               /*单体电压低2级(放电截止)*/
    ePWAR_ID_SUM_VH2 = 2,               /*电池总电压过压2级(充电截止)*/
    ePWAR_ID_SUM_VL2 = 3,               /*电池总电压欠压2级(放电截止)*/
    ePWAR_ID_CHG_TH2 = 4,               /*充电温度高2级(充电截止)*/
    ePWAR_ID_CHG_TL2 = 5,               /*充电温度低2级(充电截止)*/
    ePWAR_ID_DHG_TH2 = 6,               /*放电温度高2级(放电截止)*/
    ePWAR_ID_DHG_TL2 = 7,               /*放电温度低2级(放电截止)*/
    ePWAR_ID_CHG_CO2 = 8,               /*充电过流2级(限制充电)*/
    ePWAR_ID_DHG_CO2 = 9,               /*放电过流2级(限制放电)*/
    ePWAR_ID_PER_TH2 = 10,              /*外设温度高2级(充放电限制)*/
    ePWAR_ID_LEAK2   = 11,              /*电池组绝缘2级(充放电限制)*/
    ePWAR_ID_CV_DIF2 = 12,              /*单体电压一致性下降2级(预警)*/
    ePWAR_ID_CT_DIF2 = 13,              /*单体温度温差过大2级(预警)*/
    ePWAR_ID_CAP_SH2 = 14,              /*充电容量SOC高2级(预警)*/
    ePWAR_ID_CAP_SL2 = 15,              /*剩余容量SOC低2级(预警)*/

    /*以下为电池1级告警ID*/
    ePWAR_ID_CEL_VH1 = 16,              /*单体电压高1级(充电保护)*/
    ePWAR_ID_CEL_VL1 = 17,              /*单体电压低1级(放电保护)*/
    ePWAR_ID_SUM_VH1 = 18,              /*电池总电压过压1级(充电保护)*/
    ePWAR_ID_SUM_VL1 = 19,              /*电池总电压欠压1级(放电保护)*/
    ePWAR_ID_CHG_TH1 = 20,              /*充电温度高1级(充电保护)*/
    ePWAR_ID_CHG_TL1 = 21,              /*充电温度低1级(充电保护)*/
    ePWAR_ID_DHG_TH1 = 22,              /*放电温度高1级(放电保护)*/
    ePWAR_ID_DHG_TL1 = 23,              /*放电温度低1级(放电保护)*/
    ePWAR_ID_CHG_CO1 = 24,              /*充电过流1级(充电延时保护)*/
    ePWAR_ID_DHG_CO1 = 25,              /*放电过流1级(放电延时保护)*/
    ePWAR_ID_PER_TH1 = 26,              /*外设温度过高1级(充放电保护)*/
    ePWAR_ID_LEAK1   = 27,              /*电池组绝缘1级(充放电保护)*/
    ePWAR_ID_CV_DIF1 = 28,              /*单体电压一致性下降1级(充放电限制)*/
    ePWAR_ID_CT_DIF1 = 29,              /*单体温度温差过大1级(充放电限制)*/
    ePWAR_ID_CAP_SH1 = 30,              /*充电容量SOC高1级(充电截止)*/
    ePWAR_ID_CAP_SL1 = 31,              /*剩余容量SOC低1级(放电截止)*/

    //以下堆控自身告警
    ePWAR_ID_SMU_PAR_EXP = 32,          //堆控自身参数异常
    ePWAR_ID_SELF_IC_EXP = 33,          //堆控自身内设异常
    ePWAR_ID_SMU_DEV_REP = 34,          //堆控设备地址冲突
    ePWAR_ID_P_CHG_OP = 35,       		//电池堆充电功率超限(PCS响应超时)
	ePWAR_ID_P_DHG_OP = 36,             //电池堆放电功率超限(PCS响应超时)
	ePWAR_ID_P_NO_NOR_G = 37,           //电池堆无可用电池组
    ePWAR_ID_P_LACK_NOR_G = 38,         //电池堆可用电池组不足
	ePWAR_ID_G_SUMV_DIFF = 39,          //电池组间压压差过大

	ePWAR_ID_PUP_PRE_FAIL = 40,         //系统启动预充失败
    ePWAR_ID_P_START_ERR = 41,          //系统启动故障
    ePWAR_ID_P_STOP_OC = 42,            //系统停机带载超时
    ePWAR_ID_P_STOP_ERR = 43,           //系统停机故障
    ePWAR_ID_P_AUTPAR_ERR = 44,         //系统动态并机失败
    ePWAR_ID_P_AUTDYN_ERR = 45,         //系统动态切除失败
    ePWAR_ID_SW_EM_STOP = 46,           //电池堆软件急停
    ePWAR_ID_P_SELF_PRO = 47,           //电池堆自行保护

    ePWAR_ID_BUT_EM_STOP = 48,          //按钮紧急停机
    ePWAR_ID_P_DCISO_OFF = 49,          //汇流断路器断开
    ePWAR_ID_THUND_EXP = 50,            //防雷器故障
    ePWAR_ID_WATER_WAR = 51,            //浸水报警
    ePWAR_ID_DOOR_OPEN = 52,            //门禁开启
    ePWAR_ID_FIRE_SYS_EXP = 53,         //消防系统异常
    ePWAR_ID_FIRE_SMOG_WAR = 54,        //消防烟雾报警
    ePWAR_ID_FIRE_BEON_ERR = 55,        //消防火警故障
    ePWAR_ID_SYS_LEAK_WAR = 56,         //系统绝缘告警
    ePWAR_ID_SYS_LEAK_ERR = 57,         //系统绝缘故障

    ePWAR_ID_COMM_OFF_PCS = 58,         //外部与PCS通讯中断

    ePWAR_ID_P_KEEP1 = 59,              //电池堆预留告警
    ePWAR_ID_P_KEEP2 = 60,              //电池堆预留告警
    ePWAR_ID_P_KEEP3 = 61,              //电池堆预留告警
    ePWAR_ID_P_KEEP4 = 62,              //电池堆预留告警
    ePWAR_ID_P_KEEP5 = 63,              //电池堆预留告警

    ePWAR_ID_PILE_FIN = 64              //堆告警个数
}e_PILE_WAR_ID;

//堆控故障列表故障位
typedef enum
{
	/*以下为各电池组故障ID*/
    ePERR_ID_COMM_OFF_G1 = 0,           //电池组1CAN通讯线断线
	ePERR_ID_COMM_OFF_G2 = 1,           //电池组2CAN通讯线断线
	ePERR_ID_COMM_OFF_G16 = 15,         //电池组16CAN通讯线断线

    ePERR_ID_PWRL_OFF_G1 = 16,          //电池组1动力线断线
	ePERR_ID_PWRL_OFF_G2 = 17,          //电池组2动力线断线
	ePERR_ID_PWRL_OFF_G16 = 31,         //电池组16动力线断线

    ePERR_ID_CTRLSWH_ERR_G1 = 32,       //电池堆控制组1接触器失败
	ePERR_ID_CTRLSWH_ERR_G2 = 33,       //电池堆控制组2接触器失败
	ePERR_ID_CTRLSWH_ERR_G16 = 47,      //电池堆控制组16接触器失败

    ePERR_ID_SWHSTATE_EXP_G1 = 48, 	    //电池堆检测组1接触器跳变
	ePERR_ID_SWHSTATE_EXP_G2 = 49,      //电池堆检测组2接触器跳变
	ePERR_ID_SWHSTATE_EXP_G16 = 63,     //电池堆检测组16接触器跳变

	ePERR_ID_PILE_FIN = 64              //堆故障个数
}e_PILE_ERR_ID;


//堆控提醒列表告警位
typedef enum
{
	/*以下为各电池组提醒ID*/
    ePNOT_ID_EN_NORUN_G1 = 0,     		//组1使能未运行提醒
	ePNOT_ID_EN_NORUN_G2 = 1,     		//组2使能未运行提醒
	ePNOT_ID_EN_NORUN_G16 = 15,     	//组16使能未运行提醒

    ePNOT_ID_CLIM_DYN_G1 = 16,    		//组1充电限制切除提醒
	ePNOT_ID_CLIM_DYN_G2 = 17,    		//组2充电限制切除提醒
	ePNOT_ID_CLIM_DYN_G16 = 31,    		//组16充电限制切除提醒

	ePNOT_ID_DLIM_DYN_G1 = 32,    		//组1放电限制切除提醒
	ePNOT_ID_DLIM_DYN_G2 = 33,    		//组2放电限制切除提醒
	ePNOT_ID_DLIM_DYN_G16 = 47,    		//组16放电限制切除提醒

    ePNOT_ID_PERR_DYN_G1 = 48,    		//组1故障保护切除提醒
    ePNOT_ID_PERR_DYN_G2 = 49,    		//组2故障保护切除提醒
    ePNOT_ID_PERR_DYN_G16 = 63,    		//组16故障保护切除提醒

    ePNOT_ID_PILE_FIN = 64              //堆提醒个数
}e_PILE_NOT_ID;

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
//=============================================================================================
//函数名称	: void PileErrStatistTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆故障告警统计任务
//注意事项	: 100ms
//=============================================================================================
void PileErrStatistTask(void);
#endif

//=============================================================================================
//函数名称	: u8 GetPileNowErrSortLevelAPI(void)
//输入参数	: void
//输出参数	: 电池堆当前故障告警级别[0:无故障告警,1:提示类告警，2:充电限制类告警,3:放电限制类告警,4:充放电限制类告警,5:充电保护类故障,6:放电保护类故障,7:充放电保护类故障,8:系统故障类故障]
//函数功能	: 获取电池堆当前故障告警级别
//注意事项	: 
//=============================================================================================
u8 GetPileNowErrSortLevelAPI(void);

//=============================================================================================
//函数名称	: u8 GetPileGErrSysProStateAPI(void)
//输入参数	: void
//输出参数	: 组故障进行系统故障保护状态: 0无故障保护 1发生故障保护
//函数功能	: 获取电池堆内组故障进行系统故障保护状态
//注意事项	:
//=============================================================================================
u8 GetPileGErrSysProStateAPI(void);

#endif
#endif
