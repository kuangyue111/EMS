/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : GroupInfo.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     :
** @Instructions :
**===========================================================================================*/
#ifndef FUNTION_SYSTEMINFO_GROUPINFO_H_
#define FUNTION_SYSTEMINFO_GROUPINFO_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "PublicInfo.h"
#include "HigAFESamp.h"
#include "LowADCSamp.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define SLAVE_MAX_NUM			(MAX_BSU_NUM)											/*从控数目*/
#define SLAVE_MAX_CELVNUM		(MAX_BSU_CELL)											/*一个从控单体电压数目*/
#if(0 == BMU_BSU_COM_TYPE)                  /*BMU主从菊花链通信*/
#define SLAVE_MAX_CELTNUM		(4)														/*一个从控单体温度数目*/
#define SLAVE_MAX_BALTNUM		(2)														/*一个从控板温温度数目*/
#elif(0 == BMU_BSU_CAN_TYPE)                /*BMU主从自研CAN通信*/
#define SLAVE_MAX_CELTNUM		(16)													/*一个从控单体温度数目*/
#define SLAVE_MAX_BALTNUM		(2)														/*一个从控板温温度数目(BYD AFE4+2时均衡温度占用单体温度空间)*/
#else                                       /*BMU主从高特CAN通信*/
#define SLAVE_MAX_CELTNUM		(10)													/*一个从控单体温度数目*/
#define SLAVE_MAX_BALTNUM		(0)														/*一个从控板温温度数目*/
#endif
#define GROUP_MAX_CELVNUM		(SLAVE_MAX_NUM*SLAVE_MAX_CELVNUM)						/*组内单体电压总数目*/
#define GROUP_MAX_CELTNUM		(SLAVE_MAX_NUM*SLAVE_MAX_CELTNUM)						/*组内单体温度总数目*/
#define GROUP_MAX_BALTNUM		(SLAVE_MAX_NUM*SLAVE_MAX_BALTNUM)						/*组内均衡板温总数目*/
#if(SLAVE_MAX_CELTNUM >= 16)
#define GROUP_MAX_SUMTNUM		(SLAVE_MAX_NUM*SLAVE_MAX_CELTNUM)						/*组内从控温度总数目(满16节单体温度时没有均衡板温)*/
#else
#define GROUP_MAX_SUMTNUM		(SLAVE_MAX_NUM*(SLAVE_MAX_CELTNUM+SLAVE_MAX_BALTNUM))	/*组内从控温度总数目(包括单体温度和均衡板温)*/
#endif

#define BMU_EXT_COM_DEV			(0)			/*主控外部(上层)通信设备(0:不检测 1:PCS 2BSMU)*/

#define BMU_PWR_SWH_TYPE		(1)			/*BMU电源接触器类型(0:无电源接触器1:独立电源接触器2:共用负极接触器)*/
#define BMU_DC_VH_TYPE			(1)			/*BMU直流端高压采样值获取方式(0:无高压采样 1:自身采样 2:通讯获取)*/
#define	BMU_NEG_SWH_EN			(1)			/*BMU负极接触器控制使能(0:禁止 1:使能)*/
#define	BMU_FAN_SWH_EN			(1)			/*BMU风扇接触器控制使能(0:禁止 1:使能)*/
#define	BMU_HOT_SWH_EN			(1)			/*BMU加热接触器控制使能(0:禁止 1:使能)*/
#define	BMU_SMG_SWH_EN			(1)			/*BMU烟感接触器控制使能(0:禁止 1:使能)*/
#define	BMU_LED_SWH_EN			(1)			/*BMU状态LED显示控制使能(0:禁止 1:使能)*/
#define	BMU_CDHG_DO_EN			(1)			/*BMU充放电干接点同一DO3使能(0:禁止 1:使能)*/
#define BMU_SWH_BACK_TYPE		(0)			/*BMU接触器回检方式(0:不回检(检控制信号) 1:板级信号回检 2:板级采样回检(驱动电流) 3:外部信号回检*/

#define BMU_CELL_R_TRIG_T		(604800000)	/*BMU触发从机电芯内阻检测周期(7天,单位1ms*/

#define BMU_FAN_PWR_EN			(1)			/*BMU控制PWM调速风扇电源使能[复用风扇接触器管脚](0:禁止(主控全速风扇控制) 1:使能)*/
#define BMU_AIR_PWR_EN			(0)			/*BMU控制空调电源使能[复用加热接触器管脚](0:禁止 1:使能)*/

#define BMU_PWM_FAN_EN			(1)			/*BMU控制IMLU散热PWM调速风扇使能[IMLU1控制](0:禁止 1:使能)*/
#define BMU_PWM_FAN_NUM			(16)		/*BMU控制IMLU散热PWM调速风扇最大路数(1个电池箱1路)*/
#define BMU_LU_FLOOD_HZ			(0)			/*BMU检测IMLU水浸告警频率(1Hz,0表示不检测)*/

#define BMU_IMEU_COM_EN			(1)			/*BMU与IMEU2通信控制交互使能(0:禁止 1:使能)*/
#define BMU_EU_SHUTFAN_EN		(1)			/*BMU控制IMEU百叶风机使能*/
#define BMU_EU_FIRELIG_EN		(1)			/*BMU控制IMEU照明/声光报警/消防等使能*/
#define BMU_EU_FLOOD_HZ			(300)		/*BMU检测IMEU水浸告警频率(1Hz,0表示不检测)*/
#define BMU_EU_GAS1_CONC		(2000)		/*BMU检测IMEU可燃气体1(H2)浓度告警值(0.01%,0xffff表示不检测)*/
#define BMU_EU_GAS2_CONC		(2000)		/*BMU检测IMEU可燃气体2(CO)浓度告警值(0.01%,0xffff表示不检测)*/

#define GINDEX45_LEN			(11)		/*历史故障记录信息查询数组长度*/
#define GINDEX46_LEN			(11)		/*历史事件记录信息查询数组长度*/
#define GINDEX47_LEN			(56)		/*历史故障录波信息查询数组长度*/
#define GINDEX48_LEN			(56)		/*历史DOD记录信息查询数组长度*/

#define GINDEX50_LEN			(4)			/*系统状态信息数组长度*/
#define GINDEX51_LEN			(4)			/*电流功率限值信息数组长度*/
#define GINDEX52_LEN			(2)			/*电压限值信息数组长度*/
#define GINDEX53_LEN			(3)			/*告警信息数组长度*/
#define GINDEX54_LEN			(4)			/*故障信息数组长度*/
#define GINDEX55_LEN			(2)			/*故障定位信息数组长度*/
#define GINDEX56_LEN			(4)			/*输入输出口状态信息数组长度*/
#define GINDEX60_LEN			(4)			/*系统整体信息数组长度*/
#define GINDEX61_LEN			(4)			/*系统电量信息数组长度*/
#define GINDEX62_LEN			(4)			/*系统单体信息数组长度*/
#define GINDEX63_LEN			(4)			/*电压特征信息数组长度*/
#define GINDEX64_LEN			(4)			/*温度特征信息数组长度*/
#define GINDEX65_LEN			(4)			/*直流电流温度信息数组长度*/
#define GINDEX66_LEN			(4)			/*低压采样信息数组长度*/
#define GINDEX67_LEN			(4)			/*直流电路信息数组长度*/
#define GINDEX68_LEN			(4)			/*高压驱动电流信息数组长度*/
#define GINDEX69_LEN			(4)			/*高压驱动电流信息数组长度*/
#define GINDEX70_LEN			(4)			/*时间统计信息数组长度*/
#define GINDEX71_LEN			(4)			/*故障统计信息数组长度*/
#define GINDEX72_LEN			(4)			/*当前能量统计信息数组长度*/
#define GINDEX73_LEN			(2)			/*累计电量统计信息数组长度*/
#define GINDEX74_LEN			(2)			/*累计电能统计信息数组长度*/
#define GINDEX75_LEN			(4)			/*放电深度统计信息数组长度*/
#define GINDEX76_LEN			(4)			/*安全等级统计信息数组长度*/
#define GINDEX80_LEN			(2)			/*权限级别状态信息数组长度*/
#define GINDEX81_LEN			(2)			/*控制结果状态信息数组长度*/
#define GINDEX82_LEN			(10)		/*调试结果状态信息数组长度*/
#define GINDEX83_LEN			(3)			/*校准结果状态信息数组长度*/
#define GINDEX85_LEN			(2)			/*权限密码登录信息数组长度*/
#define GINDEX86_LEN			(3)			/*系统控制命令信息数组长度*/
#define GINDEX87_LEN			(12)		/*用户控制命令信息数组长度*/
#define GINDEX88_LEN			(8)			/*系统调试命令信息数组长度*/
#define GINDEX89_LEN			(2)			/*采样校准命令信息数组长度*/
#define GINDEX90_LEN			(25)		/*充电均衡状态信息数组长度*/
#define GINDEX91_LEN			(25)		/*放电均衡状态信息数组长度*/

#if(GROUP_MAX_CELVNUM > 240)
#define GINDEX92_LEN			(240)						/*电池单体电压信息1数组长度*/
#define GINDEX93_LEN			(GROUP_MAX_CELVNUM-240)		/*电池单体电压信息2数组长度*/
#else
#define GINDEX92_LEN			(GROUP_MAX_CELVNUM)			/*电池单体电压信息1数组长度*/
#define GINDEX93_LEN			(0)							/*电池单体电压信息2数组长度*/
#endif

#if(GROUP_MAX_SUMTNUM > 240)
#define GINDEX94_LEN			(240)						/*电池单体温度信息1数组长度(包括均衡板温个数)*/
#define GINDEX95_LEN			(GROUP_MAX_SUMTNUM-240)		/*电池单体温度信息2数组长度(包括均衡板温个数)*/
#else
#define GINDEX94_LEN			(GROUP_MAX_SUMTNUM)			/*电池单体温度信息1数组长度(包括均衡板温个数)*/
#define GINDEX95_LEN			(0)							/*电池单体温度信息2数组长度(包括均衡板温个数)*/
#endif

#define GINDEX115_LEN       	(15)						/*不同温度充电倍率表长度*/
#define GINDEX116_LEN       	(15)						/*不同温度放电倍率表长度*/
#define GINDEX119_LEN      		(44)						/*电压/SOC平滑电流限值参数表长度*/

#define GINDEX200_LEN			(eReferSampInfo200_Len)		/*主控内部参考采集信息*/
#define GINDEX201_LEN			(eCellBalanT201_Len)		/*单体均衡温度特征信息*/
#define GINDEX202_LEN			(eIMLU1Info202_Len)		    /*IMLU辅助板读取到的信息*/
#define GINDEX203_LEN			(BMU_PWM_FAN_NUM)			/*IMLU辅助板风扇控制信息*/
#define GINDEX204_LEN			(eIMEU2Info204_Len)			/*IMEU辅助板接收信息1*/
#define GINDEX205_LEN			(eIMEU2Info205_Len)			/*IMEU辅助板温湿度信息2*/


/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*协议48子索引列表*/
typedef enum
{
	eHisDodInfo48_Id  = 0,				    /*DOD记录序号*/
	eHisDodInfo48_Num = 1,				    /*DOD记录显示个数*/
	eHisDodInfo48_Y   = 2,				    /*DOD记录时间-年*/
	eHisDodInfo48_MD  = 3,				    /*DOD记录时间-月日*/
	eHisDodInfo48_HM  = 4,				    /*DOD记录时间-时分*/
	eHisDodInfo48_S   = 5,				    /*DOD记录时间-秒*/
	eHisDodInfo48_Dod = 6,				    /*DOD记录值1*/

}e_HisDodInfo48;

/*协议50子索引列表*/
typedef enum
{
	eSysStateInfo50_SysPro = 0,				/*系统保护状态[0,5]		0:未就绪 1:正常 2:充电截止 3:放电截止 4:充放电限制 5:充放电保护*/
	eSysStateInfo50_SysWor = 1,				/*系统工作状态[0,6]		0:开机,1:待命,2:启动,3:运行,4:停机,5:故障 6:关机*/
	eSysStateInfo50_SysChg = 2,				/*电池充放电状态[0,2]	0:空闲 1:充电 2:放电*/
	eSysStateInfo50_SysMod = 3				/*系统工作模式[0,5]		0:自动 1:手动 2:紧急 5:调试*/

}e_SysStateInfo50;

/*协议51子索引列表*/
typedef enum
{
	eLimCPInfo51_ChgC = 0,					/*最大允许充电电流限值(PCS充电输入最大电流)[0,10000]0.1A*/
	eLimCPInfo51_DhgC = 1,					/*最大允许放电电流限值(PCS放电输出最大电流)[0,10000]0.1A*/
	eLimCPInfo51_ChgP = 2,					/*最大允许充电功率限值(PCS充电输入最大功率)[0,10000]0.1kW*/
	eLimCPInfo51_DhgP = 3,					/*最大允许放电功率限值(PCS放电输出最大功率)[0,10000]0.1kW*/

}e_LimCPInfo_51;

/*协议52子索引列表*/
typedef enum
{
	eLimVInfo52_ChgV = 0,					/*最大允许充电电压限值(PCS充电截止电压)[0,10000]0.1V*/
	eLimVInfo52_DhgV = 1					/*最大允许放电电压限值(PCS放电截止电压)[0,10000]0.1V*/

}e_LimVInfo_52;

/*协议53子索引列表*/
typedef enum
{
	eAlarmInfo53_0 = 0,						/*电池告警信息1*/
	eAlarmInfo53_1 = 1,						/*电池告警信息2*/
	eAlarmInfo53_2 = 2						/*电池告警信息3*/

}e_AlarmInfo53;

/*协议54子索引列表*/
typedef enum
{
	eFaultInfo54_0 = 0,						/*电池故障信息1*/
	eFaultInfo54_1 = 1,						/*电池故障信息2*/
	eFaultInfo54_2 = 2						/*电池别故障信息3*/

}e_FaultInfo54;

/*协议55子索引列表*/
typedef enum
{
	eErrDetail55_Code = 0,				    /*电池故障定位码*/
	eErrDetail55_Info = 1,					/*电池故障定位信息*/

}e_ErrDetail55;

/*协议56子索引列表*/
typedef enum
{
	ePortStateInfo56_Input = 0,				/*输入端口回检状态*/
	ePortStateInfo56_Output = 1,			/*输出端口控制状态*/
	ePortStateInfo56_InInside = 2,			/*输入内部回检状态*/
	ePortStateInfo56_OutInside = 3			/*输出内部控制状态*/

}e_PortStateInfo56;

/*协议60子索引列表*/
typedef enum
{
	eSysMainInfo60_SumV = 0,				/*总电压采样值[0,10000]0.1V*/
	eSysMainInfo60_OutC = 1,				/*总电流采样值[-30000,30000]0.1A*/
	eSysMainInfo60_OutP = 2,				/*输出功率值[-30000,30000]0.1kW*/
	eSysMainInfo60_PreV = 3					/*预充电压值[0,10000]0.1V*/

}e_SysMainInfo60;

/*协议61子索引列表*/
typedef enum
{
	eSysCapInfo61_SOC = 0,					/*电池SOC值[0,1000]0.1%*/
	eSysCapInfo61_SOE = 1,					/*电池SOE值[0,1000]0.1%*/
	eSysCapInfo61_SOH = 2,					/*电池SOH值[0,1000]0.1%*/
	eSysCapInfo61_AllCap = 3				/*电池组总容量[0,1000]0.1AH*/

}e_SysCapInfo61;

/*协议62子索引列表*/
typedef enum
{
	eSysCellInfo62_SumV = 0,				/*单体总电压值[0,10000]0.1V*/
	eSysCellInfo62_AvgV = 1,				/*单体平均电压值[0,5000]1mV*/
	eSysCellInfo62_AvgT = 2,				/*单体平均温度值[-40,105]1℃*/
	eSysCellInfo62_EnvT = 3		    		/*系统环境温度值[-40,105]1℃*/

}e_SysCellInfo62;


/*协议63子索引列表*/
typedef enum
{
	eVoltCharInfo63_MaxVNum = 0,			/*单体最大电压序号[1,400]*/
	eVoltCharInfo63_MaxVolt = 1,			/*单体最大电压值[0,5000]1mV*/
	eVoltCharInfo63_MinVNum = 2,			/*单体最小电压序号[1,400]*/
	eVoltCharInfo63_MinVolt = 3				/*单体最小电压值[0,5000]1mV*/

}e_VoltCharInfo63;

/*协议64子索引列表*/
typedef enum
{
	eTempCharInfo64_MaxTNum = 0,			/*单体最高温度序号[1,400]*/
	eTempCharInfo64_MaxTemp = 1,			/*单体最高温度值[-40,105]1℃*/
	eTempCharInfo64_MinTNum = 2,			/*单体最低温度序号[1,400]*/
	eTempCharInfo64_MinTemp = 3				/*单体最低温度值[-40,105]1℃*/

}e_TempCharInfo64;

/*协议65子索引列表*/
typedef enum
{
	eDcCTInfo65_HallSignV = 0,				/*霍尔信号原始电压采样值[0,5000]1mV*/
	eDcCTInfo65_ShutSignV = 1,				/*分流器信号原始电压采样值[-7500,7500]0.01mV*/
	eDcCTInfo65_ShutT = 2,					/*分流器温度采样值[-40,180]1℃*/
	eDcCTInfo65_PowrT = 3					/*电源侧温度采样值[-40,126]1℃*/

}e_DcCTInfo65;

/*协议66子索引列表*/
typedef enum
{
	eLVSamp66_NegV = 0,						/*负极电压采样值[0,10000]0.1V*/
	eLVSamp66_PwrV = 1,						/*低压电源电压采样值[0,3300]0.01V*/
	eLVSamp66_AccV = 2,						/*激活信号ACC电压采样值[0,3300]0.01V*/
	eLVSamp66_Hall5V = 3					/*霍尔电源电压采样值[0,500]0.01V*/

}e_LVSampInfo66;

/*协议67子索引列表*/
typedef enum
{
	eFstLeakInfo67_FcV = 0,					/*充电枪KC供入电压采样值[0,3300]0.01V*/
	eFstLeakInfo67_CC2V = 1,				/*充电枪CC2信号电压采样值[0,500]0.01V*/
	eFstLeakInfo67_PasInsu = 2,				/*正极对地绝缘阻值[0,32767]1kΩ*/
	eFstLeakInfo67_NegInsu = 3				/*负极对地绝缘阻值[0,32767]1kΩ*/

}e_FstLeakInfo67;

/*协议68子索引列表*/
typedef enum
{
	eHsCurrInfo68_MHS0C = 0,				/*HS1通道1驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo68_MHS1C = 1,				/*HS1通道2驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo68_MHS2C = 2,				/*HS2通道1驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo68_MHS3C = 3					/*HS2通道2驱动电流采样值[0,500]0.01A*/

}e_HsCurrInfo68;

/*协议69子索引列表*/
typedef enum
{
	eHsCurrInfo69_MHS4C = 0,				/*HS3通道1驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo69_MHS5C = 1,				/*HS3通道2驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo69_MHS6C = 2,				/*HS4通道1驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo69_MHS7C = 3					/*HS4通道2驱动电流采样值[0,500]0.01A*/

}e_HsCurrInfo69;

/*协议70子索引列表*/
typedef enum
{
	eTime70_SysRun = 0,						/*系统运行时长[0,65535]1Hour*/
	eTime70_CanChg = 1,						/*可充电时间[0,65535]1Min*/
	eTime70_CanDhg = 2,						/*可放电时间[0,65535]1Min*/
	eTime70_ChgCycle = 3				    /*充放电循环次数[0,65535]1次*/

}e_StatistTime70;

/*协议71子索引列表*/
typedef enum
{
	eErr71_MonWarNum = 0,					/*单月告警数目[0,65535]*/
	eErr71_MonErrNum = 1,					/*单月故障数目[0,65535]*/
	eErr71_HisWarNum = 2,					/*累计告警数目[0,65535]*/
	eErr71_HisErrNum = 3					/*累计故障数目[0,65535]*/

}e_StatistErr71;

/*协议72子索引列表*/
typedef enum
{
	eCapEner72_NowEner = 0,					/*电池组当前电能[0,65535]0.1kWh*/
	eCapEner72_AllEner = 1,					/*电池组总电能[0,65535]0.1kWh*/
	eCapEner72_ChgCap = 2,					/*电池组单次充电电量[0,65535]1AH*/
	eCapEner72_DhgCap = 3,					/*电池组单次放电电量[0,655351]AH*/

}e_StatistCapEner72;

/*协议73子索引列表*/
typedef enum
{
	eHisCap73_HisChgCap = 0,				/*电池组累计充电电量[0,65535]1AH*/
	eHisCap73_HisDhgCap = 1					/*电池组累计放电电量[0,65535]1AH*/

}e_StatistHisCap73;

/*协议74子索引列表*/
typedef enum
{
	eHisEner74_HisChgEner = 0,				/*电池组累计充电电能[0,65535]0.1kWh*/
	eHisEner74_HisDhgEner = 1				/*电池组累计放电电能[0,65535]0.1kWh*/

}e_StatistHisEner74;

/*协议75子索引列表*/
typedef enum
{
	eDodStat75_NowDod = 0,				   /*最新放电深度DOD(单次)[0,1000]0.1%*/
	eDodStat75_FstDod = 1,				   /*最新放电起始DOD(单次)[0,1000]0.1%*/
	eDodStat75_MaxDod = 2,				   /*最大放电深度DOD(单次)[0,1000]0.1%*/
	eDodStat75_AvgDod = 3				   /*平均放电深度DOD(平均)[0,1000]0.1%*/

}e_StatistDodInfo75;

/*协议76子索引列表*/
typedef enum
{
	eDiagSos76_NowSos = 0,				   /*当前安全等级SOS[0,4](等级越低越安全)*/
	eDiagSos76_NowRsn = 1,				   /*当前SOS等级异常原因[按bit位表示]*/
	eDiagSos76_PreSos = 2,				   /*预测安全等级SOS[0,4](等级越低越安全)*/
	eDiagSos76_PreRsn = 3				   /*预测SOS等级异常原因[按bit位表示]*/

}e_StatistSosInfo76;

//协议81子索引列表
typedef enum
{
	eCtrlRes81_AdhCheck = 0,				/*粘连检测控制结果状态 0:自动检测 1:正在检测 2:停止检测 3:检测完成*/
	eCtrlRes81_LeakCheck					/*绝缘检测控制结果状态 0:自动检测 1:正在检测 2:停止检测 3:检测完成*/

}e_CtrlHandleResult81;

//协议83子索引列表
typedef enum
{
	eCorrRes83_CorrType = 0,				/*采样校准类型	0:电流零飘手动校准 1:充电电流单点校准 2:放电电流单点校准 3:充电电流多点校准 4:放电电流多点校准 5:高压单点校准 6:高压多点校准*/
	eCorrRes83_CorrState,					/*采样校准状态	0:未开始 [1,99]:校准过程 100:校准成功 101:校准失败*/
	eCorrRes83_CorrResult					/*采样校准结果	按位表示,每bit为1个校准类型[0:未校准,1:已校准] bit0:电流零飘校准 bit1:充电电流校准 bit2:放电电流校准 bit3:高压单点校准*/

}e_SampCorrResult83;

/*协议87子索引列表*/
typedef enum
{
	eUserCtrl87_CellTrig = 0,				/*单体信息触发命令	"每bit(0-退出,1-触发):bit0:单体电压信息触发;bit1:单体温度信息触发;bit2:单体均衡状态信息触发."*/
	eUserCtrl87_InTran = 1,					/*内部报文转发命令	0-不触发不转发,1-触发内部协议 2-转发内部报文 3-全部触发转发*/
	eUserCtrl87_HisErrSw = 2,				/*历史故障切换命令	0-最新记录,[1,30]-指定日期记录,90-首页记录,91-下一页记录*/
	eUserCtrl87_HisEvnSw = 3,				/*历史事件切换命令	0-最新记录,[1,30]-指定日期记录,90-首页记录,91-下一页记录*/
	eUserCtrl87_HisWavSw = 4,				/*故障录波切换命令	0-最新记录,[1,30]-指定日期记录,90-首条记录,91-下一条记录*/
	eUserCtrl87_HisDodSw = 5,				/*DOD记录切换命令	0-最新记录,90-首页记录,91-下一页记录*/
	eUserCtrl87_AdhCheck = 6,				/*粘连检测控制命令	0:自动检测 1:开始检测(连续检测) 2:停止检测*/
	eUserCtrl87_LeakCheck = 7,				/*绝缘检测控制命令	0:自动检测 1:开始检测(连续检测) 2:停止检测*/
	eUserCtrl87_ShutFan = 8,				/*百叶风机控制命令	0:关闭 1:开启*/
	eUserCtrl87_Lighting = 9,				/*电池柜照明控制命令	0:关闭 1:手动开启   2:手动关闭)*/
	eUserCtrl87_ZHSAForce = 10,				/*声光报警强制控制命令	0:关闭 1:强制开启 2:强制关闭*/
	eUserCtrl87_FireTrig = 11				/*气溶胶消防主动触发命令	0:关闭 1:触发 (预留)*/

}e_UserCtrl87;

/*协议89子索引列表*/
typedef enum
{
    eCorrCmd89_Type = 0,					/*采样校准类型	0:电流零飘手动校准 1:充电电流单点校准 2:放电电流单点校准 3:充电电流多点校准 4:放电电流多点校准 5:高压单点校准 6:高压多点校准 255:终止校准*/
	eCorrCmd89_Value						/*校准点基准值	基准电压 0.1V /基准电流 0.01A*/

}e_SampCorrCmd89;

/*协议200子索引列表*/
typedef enum
{
	eReferSampInfo200_McuPwr5V = 0,			/*MCU供电电源电压采样值[0,500]0.01V*/
	eReferSampInfo200_SbcAux5V,				/*SBC芯片5V输出AUX通道采样值[0,500]0.01V*/
	eReferSampInfo200_SbcMuxV,				/*SBC芯片MUX通道电压采集[0,500]0.01V*/
	eReferSampInfo200_PwmAdcV,				/*PWM电压ADC通道采样值[0,500]0.01V*/
	eReferSampInfo200_PwmCycle,				/*PWM输入捕获通道频率*/
	eReferSampInfo200_Ltc2949Vref,			/*LTC2949芯片基准源,基准回采[0,300]0.01V*/
	eReferSampInfo200_Ltc2949_5V,			/*LTC2949芯片高压侧5V电压采集[0,500]0.01V*/
	eReferSampInfo200_BattHV,				/*电池总电压采样,BATT电压采集[0,10000]0.1V*/
	eReferSampInfo200_PChgHV,				/*预充电压采样,外端电压采集[0,10000]0.1V*/
	eReferSampInfo200_Link1,				/*link+电压采样,预充电压正端电压采集[0,5000]1mV*/
	eReferSampInfo200_Link2,				/*link-电压采样,预充电压负端电压采集[0,5000]1mV*/
	eReferSampInfo200_LoadV7,				/*负载电压原始采样值,V7电压采集[0,10000]1mV*/

	eReferSampInfo200_Len					/*采样中间参考值长度*/
}e_ReferSampInfo200;

/*协议201子索引列表*/
typedef enum
{
	eCellBalanT201_GMaxTemp = 0,			/*单体均衡温度组内最大值[-40,125]1℃*/
	eCellBalanT201_GMaxTSlav = 1,			/*单体均衡温度最大值从控号[0,239]*/
	eCellBalanT201_SMaxTemp0 = 2,			/*单体均衡温度1#从控内最大值[-40,125]1℃*/
	eCellBalanT201_SMaxTemp1 = 3,			/*单体均衡温度2#从控内最大值[-40,125]1℃*/

	eCellBalanT201_Len = MAX_BSU_NUM+2,		/*单体均衡温度特征信息*/
}e_CellBalanTInfo201;

/*协议202子索引列表*/
typedef enum
{
	eIMLU1Info202_FanSign = 0,			    /*风扇回检信号(按Bit位表示)*/
	eIMLU1Info202_DiDOState = 1,		    /*DIDO回检信息*/
	eIMLU1Info202_FloodHz = 2,			    /*水浸回检频率[0,2000]1Hz*/

	eIMLU1Info202_Len                       /*IMLU1接收信息长度*/
}e_IMLURcvInfo202;

/*协议204子索引列表*/
typedef enum
{
	eIMEU2Info204_DiSign = 0,				/*DI信号回检状态*/
	eIMEU2Info204_SenCommu,					/*传感器通信告警*/
	eIMEU2Info204_SenState,					/*传感器回检状态*/
	eIMEU2Info204_FloodHz,			    	/*水浸回检频率[0,2000]1Hz*/
	eIMEU2Info204_PressSen,					/*压力变送器检测信号值,1Mpa*/
	eIMEU2Info204_VibraSen,					/*振动传感器检测信号值,1Mpa*/
	eIMEU2Info204_GasConc1,					/*可燃气体传感器1读数,0.01%*/
	eIMEU2Info204_GasConc2,					/*可燃气体传感器2读数,0.01%*/
	eIMEU2Info204_GasConc3,					/*可燃气体传感器3读数,0.01%*/
	eIMEU2Info204_FanSpeed,					/*排风机转速回采,r/min*/

	eIMEU2Info204_Len   					/*IMEU2接收信息值长度*/
}e_IMEU2RcvInfo204;

/*协议205子索引列表*/
typedef enum
{
	eIMEU2Info205_TP10001 = 0,				/*1#PT1000温度值,1℃*/
	eIMEU2Info205_TP10002,					/*2#PT1000温度值,1℃*/
	eIMEU2Info205_TP10003,					/*3#PT1000温度值,1℃*/
	eIMEU2Info205_TP10004,					/*4#PT1000温度值,1℃*/
	eIMEU2Info205_TP10005,					/*5#PT1000温度值,1℃*/
	eIMEU2Info205_TP10006,					/*6#PT1000温度值,1℃*/
	eIMEU2Info205_TP10007,					/*7#PT1000温度值,1℃*/
	eIMEU2Info205_TP10008,					/*8#PT1000温度值,1℃*/
	eIMEU2Info205_TP10009,					/*9#PT1000温度值,1℃*/
	eIMEU2Info205_TP10010,					/*10#PT1000温度值,1℃*/
	eIMEU2Info205_THSenT1,					/*1#温度传感器温度值,1℃*/
	eIMEU2Info205_THSenH1,					/*1#温度传感器湿度值,1%*/
	eIMEU2Info205_THSenT2,					/*2#温度传感器温度值,1℃*/
	eIMEU2Info205_THSenH2,					/*2#温度传感器湿度值,1%*/
	eIMEU2Info205_THSenT3,					/*3#温度传感器温度值,1℃*/
	eIMEU2Info205_THSenH3,					/*3#温度传感器湿度值,1%*/
	eIMEU2Info205_AirInsT,					/*空调当前柜内温度值,1℃*/
	eIMEU2Info205_AirInsH,					/*空调当前柜内湿度值,1%*/
	eIMEU2Info205_AirState,					/*空调状态信息*/
	eIMEU2Info205_AirAlarm,					/*空调告警信息*/
	eIMEU2Info205_AirConT,					/*空调制冷设定温度,1℃*/
	eIMEU2Info205_AirCoffT,					/*空调制冷偏移温度,1℃*/
	eIMEU2Info205_AirHighT,					/*空调高温告警温度,1℃*/
	eIMEU2Info205_AirLowT,					/*空调低温告警温度,1℃*/
	eIMEU2Info205_AirHonT,					/*空调制热设定温度,1℃*/
	eIMEU2Info205_AirHoffT,					/*空调制热偏移温度,1℃*/
	eIMEU2Info205_AirDonT,					/*除湿开启温度,1℃*/
	eIMEU2Info205_AirDoffT,					/*除湿停止温度,1℃*/
	eIMEU2Info205_AirDonH,					/*除湿开启湿度,1%*/
	eIMEU2Info205_AirDoffH,					/*除湿停止湿度,1%*/
	eIMEU2Info205_DehumDiff,				/*TF除湿湿度回差,1%*/
	eIMEU2Info205_HighHumPoint,				/*TF高湿度告警湿度,1%*/
	eIMEU2Info205_AirCoilerT,				/*TF空调盘管温度,1℃*/
	eIMEU2Info205_AirCondenT,				/*TF空调冷凝温度,1℃*/
	eIMEU2Info205_TFAirState,				/*TF空调状态信息*/
	eIMEU2Info205_TFAirAlarm,				/*TF空调告警信息*/

	eIMEU2Info205_Len   			        /*IMEU2温湿度值长度*/
}e_IMEU2TempInfo205;

/*IMEU2DIbit位对应信息(eIMEU2Info204_DiSign)*/
typedef enum
{
    eEU204DI_Fire = 0,                      /*消防告警DI1*/
	eEU204DI_Smog,                    	    /*烟感告警DI2*/
	eEU204DI_Door,                    	    /*门禁告警DI3*/
	eEU204DI_GasH,                    	    /*气体浓度告警DI4*/
	eEU204DI_Tilt,                    	    /*倾倒告警DI5*/
	eEU204DI_ShutOff,                    	/*百叶关闭未到位告警DI6*/
	eEU204DI_AirCom,                    	/*空调通信告警*/
	eEU204DI_Flood,                    		/*水浸告警*/
	eEU204DI_THSen1,                    	/*温湿度传感器1通信告警*/
	eEU204DI_THSen2,                    	/*温湿度传感器2通信告警*/
	eEU204DI_THSen3,                    	/*温湿度传感器3通信告警*/

    eEU204DI_Bit                         	/*IMEU2DIbit位长度*/
}e_EU204DiBit;

/*IMEU2AlarmBit位对应信息(eIMEU2Info204_SenCommu)*/
typedef enum
{
    eEU204Alarm_Gas1 = 0,                   /*气体检测传感器1通信告警*/
    eEU204Alarm_Gas2,                       /*气体检测传感器2通信告警*/
    eEU204Alarm_Gas3,                       /*气体检测传感器3通信告警*/

    eEU204Alarm_Bit                         /*IMEU2AlarmBit位长度*/
}e_EU204AlarmBit;

/*IMEU2StateBit位对应信息(eIMEU2Info204_SenState)*/
typedef enum
{
    eEU204State_ShutOn = 0,                 /*气百叶开启未到位告警*/

    eEU204State_Light = 8,                  /*照明开启状态*/
    eEU204State_ZHS_A = 9,                  /*声光报警开启状态*/

    eEU204State_Bit                         /*MEU2StateBit位长度*/
}e_EU204StateBit;

/*IMEU2AirStateBit位对应信息(eIMEU2Info205_AirState)*/
typedef enum
{
    eEU205AirS_Power = 0,                   /*空调开机状态*/
    eEU205AirS_ColdM,                       /*空调强制制冷模式*/
    eEU205AirS_WindM,                       /*空调强制送风模式*/
    eEU205AirS_WaitM,                       /*空调强制待机模式*/
    eEU205AirS_HeatM,                       /*空调强制制热模式*/
    eEU205AirS_AirErr,                      /*空调传感器故障*/
    eEU205AirS_VHL,                         /*空调高低电压告警*/
    eEU205AirS_THL,                         /*空调高低温度告警*/

    eEU205AirS_Bit                          /*IMEU2StateBit位长度*/
}e_EU204AirStateBit;

/*IMEU2AirAlarmBit位对应信息(eIMEU2Info205_AirAlarm)*/
typedef enum
{
    eEU205AirA_PHL = 0,                     /*空调高低压力告警*/
    eEU205AirA_ComPres,                     /*空调压缩机告警*/

    eEU205AirA_Bit                          /*IMEU2AlarmBit位长度*/
}e_EU204AirAlarmBit;

/*eIMEU2Info205_TFAirState,TF空调状态信息*/
typedef enum
{
	eEU205AirS_WhMachSt_TF = 0,				/*整机状态	0：停止 1：运行*/
	eEU205AirS_IntFanSt_TF,					/*内风机状态	0：停止 1：运行*/
	eEU205AirS_ExtFanSt_TF,					/*外风机状态	0：停止 1：运行*/
	eEU205AirS_CompreSt_TF,					/*压缩机	状态 0：停止 1：运行*/
	eEU205AirS_ElHeatSt_TF,					/*电加热状态 	0：停止 1：运行*/

	eEU205AirS_HTAlarm_TF,					/*高温告警	0：正常 1：告警*/
	eEU205AirS_LTAlarm_TF,					/*低温告警	0：正常 1：告警*/
	eEU205AirS_HHAlarm_TF,					/*高湿告警	0：正常 1：告警*/
	eEU205AirS_Bit_TF                       /*IMEU2TFStateBit位长度*/

}e_EU205AirStateBitTF;

/*eIMEU2Info205_TFAirAlarm,	TF空调告警信息*/
typedef enum
{
	eEU205AirA_LHAlarm_TF = 0,				/*低湿告警	0：正常 1：告警*/
	eEU205AirA_CoilTErr_TF,			    	/*盘管温感失效（融霜探头断路/短路）	0：正常 1：告警*/
	eEU205AirA_CondTErr_TF,					/*冷凝温感失效（冷凝探头断路/短路）	0：正常 1：告警*/
	eEU205AirA_IntTErr_TF,			    	/*内温感失效（柜内探头断路/短路）	0：正常 1：告警*/
	eEU205AirA_HPAlarm_TF,					/*高压告警	0：正常 1：告警*/
	eEU205AirA_LPAlarm_TF,					/*低压告警	0：正常 1：告警*/
	eEU205AirA_PhSeqErr_TF,					/*逆相（相序报警）	0：正常 1：告警*/
	eEU205AirA_AutoMode_TF,					/*空调强制自动模式--同飞	0：关闭 1:开启*/

	eEU205AirA_Bit_TF                       /*IMEU2TFAlarmBit位长度*/

}e_EU205AirAlarmBitTF;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
#if(1 == BMU_BSU_COM_TYPE)//使能CAN通信
#if(0 == BMU_BSU_CAN_TYPE)//使能自研CAN通信
/*=============================================================================================
** @Name      : void InputBsuBalanceState(u8 cell, u8 state)
** @Input     : cell:组内电池号[0,399] state:连续16节电池充电均衡开启状态值[0关闭 1:开启(均充)]
** @Output    : void
** @Function  : 输入接收到从机充电均衡状态值
** @The notes : 按连续16Bit位表示连续16节电池顺序(非一个数组索引表示1个从控)
**===========================================================================================*/
void InputSlaveChgBalanState(u16 cell, u16 state);

/*=============================================================================================
** @Name      : void InputSlaveDhgBalanState(u8 cell, u8 state)
** @Input     : cell:组内电池号[0,399] state:连续16节电池放电均衡开启状态值[0关闭 1:开启(均放)]
** @Output    : void
** @Function  : 输入接收到从机放电均衡状态值
** @The notes : 按连续16Bit位表示连续16节电池顺序(非一个数组索引表示1个从控)
**===========================================================================================*/
void InputSlaveDhgBalanState(u16 cell, u16 state);

/*=============================================================================================
** @Name      : void InputBsuCellVolt(u8 ecu, u8 cell, u16 volt)
** @Input     : ecu:从控节点号[0,31] cell:从控内电池号[0,15] resist:内阻值 0.1μΩ
** @Output    : void
** @Function  : 输入接收到从控单体内阻值
** @The notes :
**===========================================================================================*/
void InputBsuCellResis(u8 ecu, u8 cell, u16 resist);

#if(0)
/*=============================================================================================
** @Name      : void InputBsuBalanceState(u8 ecu, u8 cell, u8 state)
** @Input     : ecu:从控节点号[0,31] cell:从控电池号[0,23] state:均衡状态值[0关闭 1:均充 2均放]
** @Output    : void
** @Function  : 输入接收到从控均衡状态值
** @The notes : 按连续Bit位表示电池顺序(非一个数组索引表示1个从控),每个Pack从控电压数目必须一样
**===========================================================================================*/
void InputBsuBalanceState(u8 ecu, u8 cell, u8 state);
#endif

#else//使能高特CAN通信
/*=============================================================================================
** @Name      : void InputBsuChgBalanState(u8 ecu, u32 cellState)
** @Input     : ecu:从控节点号[0,31] cellState:从控各电池均衡状态值(按bit位表示电池[0关闭 1:均充]
** @Output    : void
** @Function  : 输入接收到从控充电均衡的电池
** @The notes : 按连续Bit位表示电池顺序(非一个数组索引表示1个从控),每个Pack从控电压数目必须一样
**===========================================================================================*/
void InputBsuChgBalanState(u8 ecu, u32 cellState);

/*=============================================================================================
** @Name      : void InputBsuDhgBalanState(u8 ecu, u32 cellState)
** @Input     : ecu:从控节点号[0,31] cellState:从控各电池放电均衡状态值(按bit位表示电池[0关闭 1:均放]
** @Output    : void
** @Function  : 输入接收到从控放电均衡的电池
** @The notes : 按连续Bit位表示电池顺序(非一个数组索引表示1个从控),每个Pack从控电压数目必须一样
**===========================================================================================*/
void InputBsuDhgBalanState(u8 ecu, u32 cellState);
#endif
#endif

/*=============================================================================================
** @Name      : void InputBsuCellVolt(u8 ecu, u8 cell, u16 volt)
** @Input     : ecu:从控节点号[0,31] cell:从控内电池号[0,15] volt:电压值 1mv
** @Output    : void
** @Function  : 输入接收到从控单体电压值
** @The notes :
**===========================================================================================*/
void InputBsuCellVolt(u8 ecu, u8 cell, u16 volt);

/*=============================================================================================
** @Name      : void InputBsuCellTemp(u8 ecu, u8 num, s8 temp)
** @Input     : ecu:从控节点号[0,31] num:从控内单体温度序号[0,3] temp:温度值(-40~126℃)
** @Output    : void
** @Function  : 输入接收到BSU单体温度值
** @The notes : 不包括均衡板温
**===========================================================================================*/
void InputBsuCellTemp(u8 ecu, u8 num, s8 temp);

/*=============================================================================================
** @Name      : void InputBsuBalanTemp(u8 ecu, u8 num, s8 temp)
** @Input     : ecu:BSU节点号[0,31] num:从控内均衡板温序号[0,1] temp:温度值(-40~126℃)
** @Output    : void
** @Function  : 输入接收到BSU均衡板温温度值
** @The notes : 放在全部单体温度数组之后
**===========================================================================================*/
void InputBsuBalanTemp(u8 ecu, u8 num, s8 temp);

/*=============================================================================================
** @Name      : void InputGroupHVSideData(e_HAfeSampChan index, s32 data)
** @Input     : index:高压端信息索引 data:高压端信息值
** @Output    : void
** @Function  : 输入高压端采样信息值
** @The notes :
**===========================================================================================*/
void InputGroupHVSideData(e_HAfeSampChan index, s32 data);

/*=============================================================================================
** @Name      : void InputGroupDaisyPwmData(u32 freq)
** @Input     : freq:PWM频率
** @Output    : void
** @Function  : 输入PWM频率采样值
** @The notes :
**===========================================================================================*/
void InputGroupDaisyPwmData(u32 freq);

/*=============================================================================================
** @Name      : void InputGroupLVSideData(e_BMULVSampADC index, s32 data)
** @Input     : index:低压端信息索引 data:低压端信息值
** @Output    : void
** @Function  : 输入低压端AD采样信息值
** @The notes :
**===========================================================================================*/
void InputGroupLVSideData(e_BMULVSampADC index, s32 data);

/*=============================================================================================
** @Name      : void InputGPerImlu1SignAPI(e_IMLURcvInfo202 index, u16 info)
** @Input     : index:信号信息索引 data:信号信息值
** @Output    : void
** @Function  : 输入IMLU1信号信息值
** @The notes :
**===========================================================================================*/
void InputGPerImlu1SignAPI(e_IMLURcvInfo202 index, u16 info);

/*=============================================================================================
** @Name      : void InputGPerImlu1FanAPI(u8 index, u8 info)
** @Input     : index:风扇序号  data:风扇信息
** @Output    : void
** @Function  : 输入IMLU1风扇信息值
** @The notes :
**===========================================================================================*/
void InputGPerImlu1FanAPI(u8 index, u8 info);

/*=============================================================================================
** @Name      : void ClrGPerImlu1AllInfoAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除所有IMLU1接收信息
** @The notes : IMLU1通讯中断时使用
**===========================================================================================*/
void ClrGPerImlu1AllInfoAPI(void);

/*=============================================================================================
** @Name      : void ClrGPerImeu2AllInfoAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除所有IMEU2接收信息
** @The notes : IMEU2通讯中断时使用
**===========================================================================================*/
void ClrGPerImeu2AllInfoAPI(void);

/*=============================================================================================
** @Name      : u16 GetBsuCellVolt(u8 ecu, u8 cell)
** @Input     : ecu:从控节点号[0,31] cell:从控内电池号[0,15]
** @Output    : 从控单体当前电压值:1mV
** @Function  : 获取指定从控内部单体当前电压值
** @The notes :
**===========================================================================================*/
u16 GetBsuCellVolt(u8 ecu, u8 cell);

/*=============================================================================================
** @Name      : s8 GetBsuCellTemp(u8 ecu, u8 num)
** @Input     : ecu:从控节点号[0,31] num:从控内单体温度序号[0,3]
** @Output    : 从控单体当前温度值:1℃
** @Function  : 获取指定从控内部单体当前温度值
** @The notes :
**===========================================================================================*/
s8 GetBsuCellTemp(u8 ecu, u8 num);

/*=============================================================================================
** @Name      : s8 GetBsuBalanTemp(u8 ecu, u8 num)
** @Input     : ecu:从控节点号[0,31] num:从控内均衡板温序号[0,1]
** @Output    : 从控均衡板温当前温度值:1℃
** @Function  : 获取指定从控内部均衡板温当前温度值
** @The notes :
**===========================================================================================*/
s8 GetBsuBalanTemp(u8 ecu, u8 num);

/*=================================================================================================
** @Name      : s16 GetGDcSideSampRealInfoAPI(e_ReferSampInfo200 index)
** @Input     : index:直流侧采样信息序号
** @Output    : 直流侧实际采样信息值
** @Function  : 获取电池组直流侧实际采样信息值
** @The notes : 采样中间参考值
**===============================================================================================*/
s16 GetGDcSideSampRealInfoAPI(e_ReferSampInfo200 index);

/*=================================================================================================
** @Name      : u16 GetGPerImlu1SignInfoAPI(e_IMLURcvInfo202 index)
** @Input     : index:IMLU1信号信息序号
** @Output    : IMLU1信号信息值
** @Function  : 获取电池组IMLU1信号信息值
** @The notes : IMLU1反馈值
**===============================================================================================*/
u16 GetGPerImlu1SignInfoAPI(e_IMLURcvInfo202 index);

/*=================================================================================================
** @Name      : u8 GetGPerImlu1FanInfoAPI(u8 index)
** @Input     : index:IMLU1风扇信息序号
** @Output    : IMLU1风扇信息值
** @Function  : 获取电池组IMLU1风扇信息值
** @The notes : IMLU1控制调速值
**===============================================================================================*/
u8 GetGPerImlu1FanInfoAPI(u8 index);

/*=================================================================================================
** @Name      : u16 GetGPerImeu2SignInfoAPI(e_IMEU2RcvInfo204 index)
** @Input     : index:IMEU2信号信息序号
** @Output    : IMLU2信号信息值
** @Function  : 获取电池组IMEU2信号信息值
** @The notes :
**===============================================================================================*/
u16 GetGPerImeu2SignInfoAPI(e_IMEU2RcvInfo204 index);

/*=================================================================================================
** @Name      : s8 GetGPerImeu2TempInfoAPI(e_IMEU2TempInfo205 index)
** @Input     : index:IMEU2信号信息序号
** @Output    : IMLU2温湿度信息值
** @Function  : 获取电池组IMEU2温湿度信息值
** @The notes :
**===============================================================================================*/
s8 GetGPerImeu2TempInfoAPI(e_IMEU2TempInfo205 index);

/*=================================================================================================
** @Name      : u8 GetGPerInsidMaxHumAPI(void)
** @Input     : void
** @Output    : 电池柜内部最高环境湿度值 1%
** @Function  : 获取电池柜内部最高环境湿度(IMEU2采集)
** @The notes : 多个温湿度传感器取最大值,for循环
**===============================================================================================*/
u8 GetGPerInsidMaxHumAPI(void);

/*=================================================================================================
** @Name      : s8 GetGPerInsidMaxTempAPI(void)
** @Input     : void
** @Output    : 电池柜内部最高环境温度值 1℃
** @Function  : 获取电池柜内部最高环境温度(IMEU2采集)
** @The notes : 多个温湿度传感器取最大值,for循环
**===============================================================================================*/
s8 GetGPerInsidMaxTempAPI(void);

/*=================================================================================================
** @Name      : s8 GetGPerInsidMinTempAPI(void)
** @Input     : void
** @Output    : 电池柜内部最低环境温度值 1℃
** @Function  : 获取电池柜内部最低高环境温度(IMEU2采集)
** @The notes : 多个温湿度传感器取最小值(排除断线值),for循环
**===============================================================================================*/
s8 GetGPerInsidMinTempAPI(void);

/*=================================================================================================
** @Name      : s8 GetGPerEnvirAvgTempAPI(void)
** @Input     : void
** @Output    : 电池柜外部平均环境温度值 1℃
** @Function  : 获取电池柜外部平均环境温度(IMEU2采集)
** @The notes : 多个采样点取平均值,for循环
**===============================================================================================*/
s8 GetGPerEnvirAvgTempAPI(void);

/*=================================================================================================
** @Name      : s8 GetGPerEnvirMaxTempAPI(void)
** @Input     : void
** @Output    : 电池柜外部最高环境温度值 1℃
** @Function  : 获取电池柜外部最高环境温度(IMEU2采集)
** @The notes : 多个采样点取最大值,for循环
**===============================================================================================*/
s8 GetGPerEnvirMaxTempAPI(void);

/*=================================================================================================
** @Name      : u16 GetGProtectStateAPI(void)
** @Input     : void
** @Output    : 电池组保护状态[0,5]
** @Function  : 获取电池组当前保护状态
** @The notes : e_ProState
**===============================================================================================*/
u16 GetGProtectStateAPI(void);

/*=================================================================================================
** @Name      : u16 GetGWorkStateAPI(void)
** @Input     : void
** @Output    : 电池组工作状态[0,6]
** @Function  : 获取电池组当前工作状态
** @The notes : e_WorkState
**===============================================================================================*/
u16 GetGWorkStateAPI(void);

/*=================================================================================================
** @Name      : u16 GetGChgDhgStateAPI(void)
** @Input     : void
** @Output    : 电池组充放电状态[0,2]
** @Function  : 获取电池组当前充放电状态
** @The notes : e_CurrState
**===============================================================================================*/
u16 GetGChgDhgStateAPI(void);

/*=================================================================================================
** @Name      : u16 GetGWorkModeAPI(void)
** @Input     : void
** @Output    : 电池组工作模式[0,5]
** @Function  : 获取电池组当前工作模式
** @The notes : e_WorkMode
**===============================================================================================*/
u16 GetGWorkModeAPI(void);

/*=================================================================================================
** @Name      : u16 GetGChgLimCurrAPI(void)
** @Input     : void
** @Output    : 实时充电电流限值 0.1A
** @Function  : 获取电池组当前充电电流限值
** @The notes : 最大充电电流请求值
**===============================================================================================*/
u16 GetGChgLimCurrAPI(void);

/*=================================================================================================
** @Name      : u16 GetGDhgLimCurrAPI(void)
** @Input     : void
** @Output    : 实时放电电流限值 0.1A
** @Function  : 获取电池组当前放电电流限值
** @The notes : 最大放电电流请求值
**===============================================================================================*/
u16 GetGDhgLimCurrAPI(void);

/*=============================================================================================
** @Name      : u16 GetGChgLimPowerAPI(void)
** @Input     : void
** @Output    : 充电功率限值 0.1kW
** @Function  : 返回充电功率限值
** @The notes : 最大充电功率请求值
**===========================================================================================*/
u16 GetGChgLimPowerAPI(void);

/*=============================================================================================
** @Name      : u16 GetGDhgLimPowerAPI(void)
** @Input     : void
** @Output    : 放电功率限值 0.1kW
** @Function  : 返回放电功率限值
** @The notes : 最大放电功率请求值
**===========================================================================================*/
u16 GetGDhgLimPowerAPI(void);

/*=============================================================================================
** @Name      : u16 GetGChgLimSumVAPI(void)
** @Input     : void
** @Output    : 充电总压限值 0.1V
** @Function  : 返回充电总压限值
** @The notes :
**===========================================================================================*/
u16 GetGChgLimSumVAPI(void);

/*=============================================================================================
** @Name      : u16 GetGDhgLimSumVAPI(void)
** @Input     : void
** @Output    : 放电总压限值 0.1V
** @Function  : 返回放电总压限值
** @The notes :
**===========================================================================================*/
u16 GetGDhgLimSumVAPI(void);

/*=============================================================================================
** @Name      : u16 GetGDhgLimSumVAPI(void)
** @Input     : index:告警信息索引号[0,2]
** @Output    : 电池组告警信息
** @Function  : 返回电池组告警信息
** @The notes : 16位告警码
**===========================================================================================*/
u16 GetGAlarmInfoAPI(u8 index);

/*=============================================================================================
** @Name      : u16 GetGFaultInfoAPI(void)
** @Input     : index:故障信息索引号[0,2]
** @Output    : 电池组故障信息
** @Function  : 返回电池组故障信息
** @The notes : 16位故障码
**===========================================================================================*/
u16 GetGFaultInfoAPI(u8 index);

/*=============================================================================================
** @Name      : u16 GetGInPortStateAPI(void)
** @Input     : void
** @Output    : 各输出IO端口当前状态值(按bit位表示 0分 1合)
** @Function  : 返回输出IO端口当前状态值
** @The notes :
**===========================================================================================*/
u16 GetGInPortStateAPI(void);

/*=============================================================================================
** @Name      : u16 GetGOutPortStateAPI(void)
** @Input     : void
** @Output    : 各输出IO端口当前状态值(按bit位表示 0分 1合)
** @Function  : 返回各输出IO端口当前状态值
** @The notes :
**===========================================================================================*/
u16 GetGOutPortStateAPI(void);

/*=================================================================================================
** @Name      : u16 GetGSampSumVoltAPI(void)
** @Input     : void
** @Output    : 电池组采样总电压 0.1V
** @Function  : 获取电池组采样总电压
** @The notes : 单体累加值
**===============================================================================================*/
u16 GetGSampSumVoltAPI(void);

/*=================================================================================================
** @Name      : s16 GetGSampOutCurrAPI(void)
** @Input     : void
** @Output    : 电池组采样输出电流 0.1A
** @Function  : 获取电池组采样输出电流
** @The notes : 负号为充电
**===============================================================================================*/
s16 GetGSampOutCurrAPI(void);

/*=================================================================================================
** @Name      : s16 GetGSampOutPowerAPI(void)
** @Input     : void
** @Output    : 电池组输出功率 0.1kW
** @Function  : 获取电池组输出功率值
** @The notes : 负号为充电
**===============================================================================================*/
s16 GetGSampOutPowerAPI(void);

/*=================================================================================================
** @Name      : u16 GetGSampPreVoltAPI(void)
** @Input     : void
** @Output    : 电池组外端预充电压 0.1V
** @Function  : 获取电池组外端预充电压值
** @The notes : 采样值
**===============================================================================================*/
u16 GetGSampPreVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGBattSocAPI(void)
** @Input     : void
** @Output    : 电池组SOC 0.1%
** @Function  : 获取电池组SOC值
** @The notes :
**===============================================================================================*/
u16 GetGBattSocAPI(void);

/*=================================================================================================
** @Name      : u16 GetGBattSoeAPI(void)
** @Input     : void
** @Output    : 电池组SOE 0.1%
** @Function  : 获取电池组SOE值
** @The notes :
**===============================================================================================*/
u16 GetGBattSoeAPI(void);

/*=================================================================================================
** @Name      : u16 GetGBattSoeAPI(void)
** @Input     : void
** @Output    : 电池组SOH 0.1%
** @Function  : 获取电池组SOH值
** @The notes :
**===============================================================================================*/
u16 GetGBattSohAPI(void);

/*=================================================================================================
** @Name      : u16 GetGBattAllCapAPI(void)
** @Input     : void
** @Output    : 电池组总容量 0.1AH
** @Function  : 获取电池组总容量值
** @The notes : 实测值
**===============================================================================================*/
u16 GetGBattAllCapAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellSumVoltAPI(void)
** @Input     : void
** @Output    : 电池组求和总电压 0.1V
** @Function  : 获取电池组求和总电压
** @The notes : 单体累加值
**===============================================================================================*/
u16 GetGCellSumVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellAvgVoltAPI(void)
** @Input     : void
** @Output    : 电池组单体平均电压 1mV
** @Function  : 获取电池组单体平均电压
** @The notes : 单体累加平均值
**===============================================================================================*/
u16 GetGCellAvgVoltAPI(void);

/*=================================================================================================
** @Name      : s16 GetGCellAvgTempAPI(void)
** @Input     : void
** @Output    : 电池组单体平均温度 1℃
** @Function  : 获取电池组单体平均温度
** @The notes : 单体累加平均值
**===============================================================================================*/
s16 GetGCellAvgTempAPI(void);

/*=================================================================================================
** @Name      : s16 GetGEnvirTempAPI(void)
** @Input     : void
** @Output    : 电池组环境温度 1℃
** @Function  : 获取电池组环境温度
** @The notes : 第1路外围温度
**===============================================================================================*/
s16 GetGEnvirTempAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMaxVNumAPI(void)
** @Input     : void
** @Output    : 电池组单体最大电压电池号[1,400]
** @Function  : 获取电池组单体最大电压电池号
** @The notes :
**===============================================================================================*/
u16 GetGCellMaxVNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMaxVoltAPI(void)
** @Input     : void
** @Output    : 电池组单体最大电压值 1mV
** @Function  : 获取电池组单体最大电压值
** @The notes :
**===============================================================================================*/
u16 GetGCellMaxVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMaxVNumAPI(void)
** @Input     : void
** @Output    : 电池组单体最小电压电池号[1,400]
** @Function  : 获取电池组单体最小电压电池号
** @The notes :
**===============================================================================================*/
u16 GetGCellMinVNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMinVoltAPI(void)
** @Input     : void
** @Output    : 电池组单体最小电压值 1mV
** @Function  : 获取电池组单体最小电压值
** @The notes :
**===============================================================================================*/
u16 GetGCellMinVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMaxTNumAPI(void)
** @Input     : void
** @Output    : 电池组单体最高温度序号[1,400]
** @Function  : 获取电池组单体最高温度序号
** @The notes :
**===============================================================================================*/
u16 GetGCellMaxTNumAPI(void);

/*=================================================================================================
** @Name      : s16 GetGCellMaxTempAPI(void)
** @Input     : void
** @Output    : 电池组单体最高温度值 1℃
** @Function  : 获取电池组单体最高温度值
** @The notes :
**===============================================================================================*/
s16 GetGCellMaxTempAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMinTNumAPI(void)
** @Input     : void
** @Output    : 电池组单体最低温度序号[1,400]
** @Function  : 获取电池组单体最低温度序号
** @The notes :
**===============================================================================================*/
u16 GetGCellMinTNumAPI(void);

/*=================================================================================================
** @Name      : s16 GetGCellMinTempAPI(void)
** @Input     : void
** @Output    : 电池组单体最低温度值 1℃
** @Function  : 获取电池组单体最低温度值
** @The notes :
**===============================================================================================*/
s16 GetGCellMinTempAPI(void);

/*=================================================================================================
** @Name      : u16 GetGHallSignVoltAPI(void)
** @Input     : void
** @Output    : 电池组霍尔电流源信号电压采样值 1mV
** @Function  : 获取电池组霍尔电流源信号电压采样值
** @The notes :
**===============================================================================================*/
u16 GetGHallSignVoltAPI(void);

/*=================================================================================================
** @Name      : s16 GetGShuntSampCurrAPI(void)
** @Input     : void
** @Output    : 电池组分流器电流源采样值 0.01A
** @Function  : 获取电池组分流器电流源采样值
** @The notes :
**===============================================================================================*/
s16 GetGShuntSampCurrAPI(void);

/*=================================================================================================
** @Name      : s16 GetGShuntSampTempAPI(void)
** @Input     : void
** @Output    : 电池组分流器温度采样值 1℃
** @Function  : 获取电池组分流器温度采样值
** @The notes :
**===============================================================================================*/
s16 GetGShuntSampTempAPI(void);

/*=================================================================================================
** @Name      : s16 GetGDcPwrSampTempAPI(void)
** @Input     : void
** @Output    : 电池组主控低压电源温度采样值 1℃
** @Function  : 获取电池组主控低压电源温度采样值
** @The notes :
**===============================================================================================*/
s16 GetGDcPwrSampTempAPI(void);

/*=================================================================================================
** @Name      : u16 GetGPgndNegVoltAPI(void)
** @Input     : void
** @Output    : 电池组负母线电压值[0,5000]0.1V
** @Function  : 获取电池组负母线电压值
** @The notes :
**===============================================================================================*/
u16 GetGPgndNegVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGLowAccVoltAPI(void)
** @Input     : void
** @Output    : 电池组单激活信号电压值[0,3600]0.01V
** @Function  : 获取电池组激活信号电压值
** @The notes :
**===============================================================================================*/
u16 GetGLowAccVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGLowHall5VVoltAPI(void)
** @Input     : void
** @Output    : 电池组霍尔5V基准电压值[0,500]0.01V
** @Function  : 获取电池组霍尔5V基准电压采样值
** @The notes :
**===============================================================================================*/
u16 GetGLowHall5VVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGFstChgFCVoltAPI(void)
** @Input     : void
** @Output    : 电池组快充枪接入电压值[0,3600]0.01V
** @Function  : 获取电池组快充枪接入电压值
** @The notes :
**===============================================================================================*/
u16 GetGFstChgFCVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGFstChgCC2VoltAPI(void)
** @Input     : void
** @Output    : 电池组快充枪CC2信号电压值[0,3600]0.01V
** @Function  : 获取电池组快充枪CC2信号电压值
** @The notes :
**===============================================================================================*/
u16 GetGFstChgCC2VoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGPasInsuResistAPI(void)
** @Input     : void
** @Output    : 电池组正极绝缘值[0,30000]1kΩ
** @Function  : 获取电池组电池正极绝缘值
** @The notes :
**===============================================================================================*/
u16 GetGPasInsuResistAPI(void);

/*=================================================================================================
** @Name      : u16 GetGNegInsuResistAPI(void)
** @Input     : void
** @Output    : 电池组负极绝缘值[0,30000]1kΩ
** @Function  : 获取电池组电池负极绝缘值
** @The notes :
**===============================================================================================*/
u16 GetGNegInsuResistAPI(void);

/*=================================================================================================
** @Name      : u16 GetGNegInsulationAPI(void)
** @Input     : channel:高边开关通道号[0,7]
** @Output    : 电池组高边开关驱动电流值[-1000,1000] 1mA
** @Function  : 获取电池组高边开关驱动电流值
** @The notes :
**===============================================================================================*/
s16 GetGHSDriverCurrAPI(u8 channel);

/*=================================================================================================
** @Name      : u16 GetGCanChgTimeAPI(void)
** @Input     : void
** @Output    : 电池组可充电时间 1min
** @Function  : 获取电池组可充电时间
** @The notes :
**===============================================================================================*/
u16 GetGCanChgTimeAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCanDhgTimeAPI(void)
** @Input     : void
** @Output    : 电池组可放电时间 1min
** @Function  : 获取电池组可放电时间
** @The notes :
**===============================================================================================*/
u16 GetGCanDhgTimeAPI(void);

/*=================================================================================================
** @Name      : u16 GetGChgCycleTimesAPI(void)
** @Input     : void
** @Output    : 电池组充放电循环次数 1次
** @Function  : 获取电池组充放电循环次数
** @The notes :
**===============================================================================================*/
u16 GetGChgCycleTimesAPI(void);

/*=================================================================================================
** @Name      : u16 GetGMonWarNumAPI(void)
** @Input     : void
** @Output    : 电池组当月告警数目 1个
** @Function  : 获取电池组当月告警数目
** @The notes :
**===============================================================================================*/
u16 GetGMonWarNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGMonErrNumAPI(void)
** @Input     : void
** @Output    : 电池组当月故障数目 1个
** @Function  : 获取电池组当月故障数目
** @The notes :
**===============================================================================================*/
u16 GetGMonErrNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGHisWarNumAPI(void)
** @Input     : void
** @Output    : 电池组累计告警数目 1个
** @Function  : 获取电池组累计告警数目
** @The notes :
**===============================================================================================*/
u16 GetGHisWarNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGHisErrNumAPI(void)
** @Input     : void
** @Output    : 电池组累计故障数目 1个
** @Function  : 获取电池组累计故障数目
** @The notes :
**===============================================================================================*/
u16 GetGHisErrNumAPI(void);

/*=================================================================================================
** @Name      : u32 GetGHisErrSumNumAPI(void)
** @Input     : void
** @Output    : 电池组累计故障总数目 1个
** @Function  : 获取电池组累计故障总数目
** @The notes : 包括告警数目和故障数目
**===============================================================================================*/
u32 GetGHisErrSumNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGRemainEnerAPI(void)
** @Input     : void
** @Output    : 电池组当前剩余电能 0.1kWh
** @Function  : 获取电池组当前剩余电能
** @The notes :
**===============================================================================================*/
u16 GetGRemainEnerAPI(void);

/*=================================================================================================
** @Name      : u16 GetGBattAllEnerAPI(void)
** @Input     : void
** @Output    : 电池组电池总电能 0.1kWh
** @Function  : 获取电池组电池总电能
** @The notes :
**===============================================================================================*/
u16 GetGBattAllEnerAPI(void);

/*=================================================================================================
** @Name      : u16 GetGNowChgCapAPI(void)
** @Input     : void
** @Output    : 电池组当前单次充电量 1Ah
** @Function  : 获取电池组当前单次充电量
** @The notes :
**===============================================================================================*/
u16 GetGNowChgCapAPI(void);

/*=================================================================================================
** @Name      : u16 GetGNowDhgCapAPI(void)
** @Input     : void
** @Output    : 电池组当前单次放电量 1Ah
** @Function  : 获取电池组当前单次放电量
** @The notes :
**===============================================================================================*/
u16 GetGNowDhgCapAPI(void);

/*=================================================================================================
** @Name      : u32 GetGHisChgCapAPI(void)
** @Input     : void
** @Output    : 电池组累计充电电量 1Ah
** @Function  : 获取电池组累计充电电量
** @The notes :
**===============================================================================================*/
u32 GetGHisChgCapAPI(void);

/*=================================================================================================
** @Name      : u32 GetGHisDhgCapAPI(void)
** @Input     : void
** @Output    : 电池组累计放电电量 1Ah
** @Function  : 获取电池组累计放电电量
** @The notes :
**===============================================================================================*/
u32 GetGHisDhgCapAPI(void);

/*=================================================================================================
** @Name      : u32 GetGHisChgEnerAPI(void)
** @Input     : void
** @Output    : 电池组累计充电电能 0.1kWh
** @Function  : 获取电池组累计充电电能
** @The notes :
**===============================================================================================*/
u32 GetGHisChgEnerAPI(void);

/*=================================================================================================
** @Name      : u32 GetGHisDhgEnerAPI(void)
** @Input     : void
** @Output    : 电池组累计放电电能 0.1kWh
** @Function  : 获取电池组累计放电电能
** @The notes :
**===============================================================================================*/
u32 GetGHisDhgEnerAPI(void);

/*=================================================================================================
** @Name      : u16 GetGStatistNowDodAPI(void)
** @Input     : void
** @Output    : 电池组当前放电深度DOD 0.1%
** @Function  : 获取电池组当前放电深度DOD
** @The notes :
**===============================================================================================*/
u16 GetGStatistNowDodAPI(void);

/*=================================================================================================
** @Name      : u16 GetGStatistNowDodAPI(void)
** @Input     : void
** @Output    : 电池组最大放电深度DOD 0.1%
** @Function  : 获取电池组最大放电深度DOD
** @The notes :
**===============================================================================================*/
u16 GetGStatistMaxDodAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCtrlResultAPI(u8 index)
** @Input     : index:子索引号[0,1]
** @Output    : 电池组控制结果信息
** @Function  : 获取电池组控制结果信息
** @The notes :
**===============================================================================================*/
u16 GetGCtrlResultAPI(u8 index);

/*=================================================================================================
** @Name      : u16 GetGDebugResultAPI(u8 index)
** @Input     : index:子索引号[0,8]
** @Output    : 电池组调试结果信息
** @Function  : 获取电池组调试结果信息
** @The notes :
**===============================================================================================*/
u16 GetGDebugResultAPI(u8 index);

/*=================================================================================================
** @Name      : u16 GetGSbcAux5VVoltAPI(void)
** @Input     : void
** @Output    : 电池组SBC5V基准电压采样值 0.01V
** @Function  : 获取电池组SBC5V基准电压采样值
** @The notes :
**===============================================================================================*/
u16 GetGSbcAux5VVoltAPI(void);

/*=================================================================================================
** @Name      : s8 GetGBalanMaxTempAPI(void)
** @Input     : void
** @Output    : 电池组均衡板温最大值 1℃
** @Function  : 获取电池组均衡板温最大值
** @The notes :
**===============================================================================================*/
s8 GetGBalanMaxTempAPI(void);

/*=================================================================================================
** @Name      : u8 GetGBalanMaxTSlaveAPI(void)
** @Input     : void
** @Output    : 电池组最大均衡板温从控号[1,400]
** @Function  : 获取电池组最大均衡板温从控
** @The notes :
**===============================================================================================*/
u8 GetGBalanMaxTSlaveAPI(void);

/*=================================================================================================
** @Name      : u8 GetGHigPGNDSwhStateAPI(void)
** @Input     : void
** @Output    : 电池组负极PGND继电器状态 0:断开 1:闭合
** @Function  : 获取电池组负极PGND继电器状态
** @The notes :
**===============================================================================================*/
u8 GetGHigPGNDSwhStateAPI(void);

/*=================================================================================================
** @Name      : u16 GetGChgBalanStateAPI(u16 index)
** @Input     : index:索引号[0,24]
** @Output    : 电池组单体充电均衡状态(按bit位表示 0关闭 1均充)
** @Function  : 获取电池组单体充电均衡状态
** @The notes : 指定索引非从控序号
**===============================================================================================*/
u16 GetGChgBalanStateAPI(u16 index);

/*=================================================================================================
** @Name      : u16 GetGDhgBalanStateAPI(u16 index)
** @Input     : index:索引号[0,24]
** @Output    : 电池组单体放电均衡状态(按bit位表示 0关闭 1均放)
** @Function  : 获取电池组单体放电均衡状态
** @The notes : 指定索引非从控序号
**===============================================================================================*/
u16 GetGDhgBalanStateAPI(u16 index);

/*=================================================================================================
** @Name      : u16 GetGCellVoltAPI(void)
** @Input     : cell:电池号[0,399]
** @Output    : 电池组单体电压值 1mV
** @Function  : 获取电池组单体电压值
** @The notes :
**===============================================================================================*/
u16 GetGCellVoltAPI(u16 cell);

/*=================================================================================================
** @Name      : s8 GetGCellTempAPI(u16 cell)
** @Input     : cell:电池号[0,399]
** @Output    : 电池组单体温度值 1℃
** @Function  : 获取电池组单体温度值
** @The notes :
**===============================================================================================*/
s8 GetGCellTempAPI(u16 cell);

/*=================================================================================================
** @Name      : s8 GetGBalanTempAPI(u16 cell)
** @Input     : cell:电池号[0,63]
** @Output    : 电池组单体均衡温度值 1℃
** @Function  : 获取电池组单体均衡温度值
** @The notes :
**===============================================================================================*/
s8 GetGBalanTempAPI(u16 cell);

/*=================================================================================================
** @Name      : s8 GetGBalanSlavMaxTAPI(u8 slave)
** @Input     : slave:从控号[0,31]
** @Output    : 电池组单体均衡温度从控内部最大温度值 1℃
** @Function  : 获取电池组单体均衡温度从控内部最大温度值
** @The notes :
**===============================================================================================*/
s8 GetGBalanSlavMaxTAPI(u8 slave);

/*=================================================================================================
** @Name      : u8 GetGroupOffsetDevAddrAPI(void)
** @Input     : void
** @Output    : 电池组自身设备地址偏移量
** @Function  : 获取电池组自身偏移设备地址
** @The notes : 主控设备地址偏移量
**===============================================================================================*/
u8 GetGroupOffsetDevAddrAPI(void);

/*=================================================================================================
** @Name      : u16 ChangSlaveVidToGroupVid(u8 slave, u8 cell)
** @Input     : slave:从控号[0,31] cell:从控内电池号[0,17]
** @Output    : 从控内电压序号对应组内电压序号[0,399],0xffff:错误序号
** @Function  : 将从控内电压序号转换成电池组内电压序号
** @The notes : 每个从控电压数目不一样的需要特殊处理
**===============================================================================================*/
u16 ChangSlaveVidToGroupVid(u8 slave, u8 cell);

/*=================================================================================================
** @Name      : u16 ChangSlaveTidToGroupVid(u8 slave, u8 cell)
** @Input     : slave:从控号[0,31] cell:从控内温度号[0,8]
** @Output    : 从控内温度序号对应组内温度序号[0,199],0xffff:错误序号
** @Function  : 将从控内温度序号转换成电池组内温度序号
** @The notes : 每个从控温度数目不一样的需要特殊处理
**===============================================================================================*/
u16 ChangSlaveTidToGroupTid(u8 slave, u8 cell);

/*=================================================================================================
** @Name      : u8 ChangGroupTidToSlaveNum(u16 id)
** @Input     : id:组内温度序号[0,399]
** @Output    : 组内温度序号所在从控序号[0,31],0xffff:错误序号
** @Function  : 将电池组内温度序号转换成所在从控序号
** @The notes : 每个从控温度数目不一样的需要特殊处理
**===============================================================================================*/
u8 ChangGroupTidToSlaveNum(u16 id);

/*=================================================================================================
** @Name      : u8 ChangGroupTidToSlaveTid(u16 id)
** @Input     : id:组内温度序号[0,399]
** @Output    : 组内温度序号所在从控内温度序号[0,4],0xffff:错误序号
** @Function  : 将电池组内温度序号转换成所在从控内温度序号
** @The notes : 每个从控温度数目不一样的需要特殊处理
**===============================================================================================*/
u8 ChangGroupTidToSlaveTid(u16 id);

#endif
