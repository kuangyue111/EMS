/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : DiagUser.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池故障诊断模块用户接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "CurrSample.h"
#include "PortCheck.h"
#include "InsuCheck.h"
#include "CANUser.h"
#include "GroupMsg.h"
#include "StaInterface.h"
#include "EnerInterface.h"
#include "SocInterface.h"
#include "DiagInterface.h"
#include "TacInterface.h"
#include "RecordInterface.h"
#include "TacUser.h"
#include "DiagUser.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
const t_FilPara gErrFilPara[eERR_ID_FINISH] =               /*故障诊断滤波参数(故障滤波时间和自恢复次数)*/
{
	/*故障类型/检测滤波时间(ms)/恢复滤波时间(ms)/允许自恢复次数*/
	/*故障类型:[0:正常(不检测),1:仅提示类，2:充电限制类,3:放电限制类,4:充放电限制类,5:充电保护类,6:放电保护类,7:充放电保护类,8:系统故障类]*/
	/*以下为2级一般告警*/
	{eErr_ChgLim,  3000, 10000, 255},                       /*[0]单体过压2级充电限制*/
	{eErr_DhgLim,  3000, 10000, 255},                       /*[1]单体欠压2级放电限制*/
	{eErr_ChgLim,  3000, 10000, 255},                       /*[2]电池总电压过压2级充电限制*/
	{eErr_DhgLim,  3000, 10000, 255},                       /*[3]电池总电压欠压2级放电限制*/
	{eErr_ChgLim,  3000, 10000, 255},                       /*[4]充电高温2级充电限制*/
	{eErr_ChgLim,  3000, 10000, 255},                       /*[5]充电低温2级充电限制*/
	{eErr_DhgLim,  3000, 10000, 255},                       /*[6]放电高温2级放电限制*/
	{eErr_DhgLim,  3000, 10000, 255},                       /*[7]放电低温2级放电限制*/
	{eErr_ChgLim,  3000, 10000, 255},                       /*[8]充电过流2级限制充电*/
	{eErr_DhgLim,  3000, 10000, 255},                       /*[9]放电过流2级限制放电*/
	{eErr_SysLim,  5000, 10000, 255},                       /*[10]外设高温2级充放电限制*/
	{eErr_SysLim,  1000, 10000, 255},                       /*[11]绝缘2级充放电限制*/
	{eErr_Warm,    5000, 10000, 255},                       /*[12]单体电压压差过大2级预警*/
	{eErr_Warm,    5000, 10000, 255},                       /*[13]单体温度温差过大2级预警*/
	{eErr_Warm,    5000, 10000, 255},                       /*[14]剩余容量SOC高2级预警*/
	{eErr_Warm,    5000, 10000, 255},                       /*[15]剩余容量SOC低2级预警*/

 	/*以下为1级严重告警*/
	{eErr_SysPro,  3000, 10000,   3},                       /*[16]单体过压1级充电保护(需要断接触器)*/
	{eErr_DhgPro,  3000, 30000,   3},                       /*[17]单体欠压1级放电保护(放电断接触器)*/
	{eErr_SysPro,  3000, 10000,   3},                       /*[18]电池总电压过压1级充电保护(需要断接触器)*/
	{eErr_DhgPro,  3000, 10000,   3},                       /*[19]电池总电压欠压1级放电保护(放电断接触器)*/
	{eErr_ChgPro,  3000, 10000, 255},                       /*[20]充电高温1级充电保护(充电断接触器)*/
	{eErr_ChgPro,  3000, 10000, 255},                       /*[21]充电低温1级充电保护(充电断接触器)*/
	{eErr_SysPro,  3000, 30000, 255},                       /*[22]放电高温1级停机保护*(需要断接触器)*/
	{eErr_SysPro,  3000, 30000, 255},                       /*[23]放电低温1级停机保护(需要断接触器)*/
	{eErr_SysPro,  3000, 30000,   1},                       /*[24]充电过流1级充电保护(充电断接触器)*/
	{eErr_SysPro,  3000, 30000,   1},                       /*[25]放电过流1级放电保护(放电断接触器)*/
	{eErr_SysPro,  5000, 10000, 255},                       /*[26]外设高温1级停机保护(需要断接触器)*/
	{eErr_SysErr,  1000, 10000,   5},                       /*[27]绝缘1级故障保护*/
	{eErr_SysLim,  5000, 10000, 255},                       /*[28]单体电压压差过大1级充放电限制*/
	{eErr_SysLim,  5000, 10000, 255},                       /*[29]单体温度温差过大1级充放电限制*/
	{eErr_ChgLim,  5000, 10000, 255},                       /*[30]剩余容量SOC高1级充电限制*/
	{eErr_DhgLim,  5000, 10000, 255},                       /*[31]剩余容量SOC低1级放电限制*/

 	/*以下为提示类告警*/
	{eErr_Warm,    1000,  1000, 255},                       /*[32]SOC显示异常(需要修正)*/
	{eErr_Warm,    1000,  1000, 255},                       /*[33]容量检测异常(需要校准)*/
	{eErr_Warm,   10000,  5000, 255},                       /*[34]充电过流响应超时(电流超限值)*/
	{eErr_Warm,   10000,  5000, 255},                       /*[35]放电过流响应超时(电流超限值)*/
	{eErr_Warm,    3000,  3000, 255},                       /*[36]均衡板温过高(关闭均衡)*/
	{eErr_Warm,    3000,  3000, 255},                       /*[37]低压电源板温异常(提示)*/
	{eErr_Warm,    1000,  1000, 255},                       /*[38]SOS诊断告警*/
	{eErr_Warm,   10000, 10000, 255},                       /*[39]电池箱风扇告警*/
	{eErr_Warm,   20000, 10000, 255},                       /*[40]电池柜百叶风机空调异常告警(提示)*/
	{eErr_Warm,    5000, 10000, 255},                       /*[41]电池柜辅助设备通信中断告警(提示)*/
	{eErr_Norm,       0,     0, 255},                       /*[42]预留提示类告警5*/
	{eErr_Norm,       0,     0, 255},                       /*[43]预留提示类告警6*/
	{eErr_Norm,       0,     0, 255},                       /*[44]预留提示类告警7*/
	{eErr_Norm,       0,     0, 255},                       /*[45]预留提示类告警8*/
	{eErr_Norm,       0,     0, 255},                       /*[46]预留提示类告警9*/
	{eErr_Norm,       0,     0, 255},                       /*[47]预留提示类告警10*/

	/*以下为故障类故障*/
	{eErr_SysErr,  5000, 10000, 255},                       /*[48]均衡温度采样异常*/
	{eErr_SysErr,  5000, 10000, 255},                       /*[49]均衡控制电路异常*/
	{eErr_SysErr,  3000, 10000,   5},                       /*[50]水浸告警*/
	{eErr_SysErr,  3000, 30000,   5},                       /*[51]电池气体检测浓度异常*/
	{eErr_SysErr,  1000, 10000,   5},                       /*[52]从控采样电路异常*/
	{eErr_SysErr,  2000, 10000,   5},                       /*[53]从控电压采样断线*/
	{eErr_SysErr,  2000, 10000,   5},                       /*[54]从控温度采样断线*/
	{eErr_SysErr, 30000,  3000, 255},                       /*[55]从控单体信息缺失*/

	//菊花链通信才有级联线
	#if(0 == BMU_BSU_COM_TYPE)
	{eErr_Warm,    8000,  3000, 255},                       /*[56]从控级联线束异常*/
	#else
	{eErr_Norm,       0,     0, 255},                       /*[56]从控级联线束异常*/
	#endif

    #if(0 == BMU_BSU_COM_TYPE)                              /*菊花链单体采样方式*/
	{eErr_Norm,    8000,  3000, 255},                       /*[57]内部主从通信中断*/
    #else                                                   /*CAN通信单体采样方式*/
	{eErr_SysErr, 10000,  3000, 255},                       /*[57]内部主从通信中断*/
    #endif
	{eErr_Warm,   15000,  3000, 255},                       /*[58]内部外设通信中断*/

	/*主控外部与BSMU通讯*/
	#if(2 == BMU_EXT_COM_DEV)
	{eErr_SysErr, 15000,  2000, 255},                       /*[59]外部BSMU通讯断线*/

	/*主控外部与PCS通讯*/
	#elif(1 == BMU_EXT_COM_DEV)
	{eErr_SysLim, 30000,  1000, 255},                       /*[59]外部PCS通讯断线*/

	/*不监测外部通讯*/
	#else
	{eErr_Norm,       0,     0, 255},                       /*[59]外部通信中断不检测*/
	#endif

	{eErr_SysErr,  5000, 10000, 255},                       /*[60]主控设备地址冲突*/
	{eErr_Norm,       0,     0, 255},                       /*[61]主控主MCU软件异常(辅助MCU检测)*/
	{eErr_SysErr,     0,  5000, 255},                       /*[62]电池组软件急停控制*/
	{eErr_Warm,    3000,  3000, 255},                       /*[63]电池组自行保护*/

	{eErr_Warm,    5000, 10000, 255},                       /*[64]主控内部RTC异常*/
	{eErr_Warm,    5000, 10000, 255},                       /*[65]主控外部EEPROM存储异常*/
	{eErr_Warm,    5000, 10000, 255},                       /*[66]主控外部Flash存储异常*/
	{eErr_SysErr,  3000,  5000,   5},                       /*[67]电池组电流采样电路异常*/
	{eErr_SysErr,  3000, 10000,   5},                       /*[68]输出端高压采样异常*/
	{eErr_Warm,    3000, 10000, 255},                       /*[69]硬件板低压采样异常*/
	{eErr_Warm,    3000, 10000, 255},                       /*[70]外设温度采样异常*/
	{eErr_SysErr,  3000, 10000,   5},                       /*[71]绝缘采样异常*/

	/*板级信号回检使能*/
	#if(1 == BMU_SWH_BACK_TYPE)
	{eErr_SysErr,  3000,  5000, 255},                       /*[72]KM1+回检异常(主正接触器HS回检异常)*/
	{eErr_SysErr,  3000,  5000, 255},                       /*[73]KM2回检异常(预充接触器HS回检异常)*/
	{eErr_SysErr,  3000,  5000, 255},                       /*[74]KM3+回检异常(负极接触器HS回检异常)*/
	{eErr_SysErr,  3000,  5000, 255},                       /*[75]KM4回检异常(电源接触器HS回检异常)*/
	{eErr_SysErr,  3000,  5000, 255},                       /*[76]KM5-8回检异常(5-8附加接触器HS回检异常)*/
	{eErr_SysErr,  3000,  5000, 255},                       /*[77]KM9回检异常(风扇控制HS回检异常)*/
	{eErr_SysErr,  3000,  5000, 255},                       /*[78]KM1-回检异常(主正接触器LS回检异常)*/
	{eErr_SysErr,  3000,  5000, 255},                       /*[79]KM3-回检异常(负极接触器LS回检异常)*/
	/*不检板级信号回检*/
    #else
	{eErr_Norm,    3000,  5000, 255},                       /*[72]KM1+回检异常(主正接触器HS回检异常)*/
	{eErr_Norm,    3000,  5000, 255},                       /*[73]KM2回检异常(预充接触器HS回检异常)*/
	{eErr_Norm,    3000,  5000, 255},                       /*[74]KM3+回检异常(负极接触器HS回检异常)*/
	{eErr_Norm,    3000,  5000, 255},                       /*[75]KM4回检异常(电源接触器HS回检异常)*/
	{eErr_Norm,    3000,  5000, 255},                       /*[76]KM5-8回检异常(5-8附加接触器HS回检异常)*/
	{eErr_Norm,    3000,  5000, 255},                       /*[77]KM9回检异常(风扇控制HS回检异常)*/
	{eErr_Norm,    3000,  5000, 255},                       /*[78]KM1-回检异常(主正接触器LS回检异常)*/
	{eErr_Norm,    3000,  5000, 255},                       /*[79]KM3-回检异常(负极接触器LS回检异常)*/
    #endif

	{eErr_SysErr,  3000, 20000,   5},                       /*[32]主正接触器异常(可能主接触器烧结或不能吸合)*/
	{eErr_SysErr,  3000, 10000,   5},                       /*[33]预充电路异常(可能预充电阻断开或预充接触器不能吸合)*/
	{eErr_SysErr,  3000, 20000,   5},                       /*[34]负极接触器异常(可能负极接触器烧结或不能吸合)*/

	/*气溶胶(化学反应生热产生告警信号仅持续30s)*/
	{eErr_SysErr,  7000, 10000,   1},                       /*[35]消防系统告警*/
	/*七氟丙烷(火探管压力产生告警信号一直持续)*/
	//{eErr_SysErr,  3000, 10000,   1},                     /*[35]消防系统告警*/

	{eErr_SysErr,  2000,  5000,   1},                       /*[36]电池组熔断器熔断*/
	{eErr_SysErr,  1000,  5000,   1},                       /*[37]电池组烟雾报警*/
	{eErr_SysErr,  1000, 30000,   3},                       /*[38]电池组启动预充未满(可能带载启动)*/
	{eErr_SysErr,  1000, 30000,   3},                       /*[39]电池组启动预充过温(预充电阻过温)*/
	{eErr_SysErr,  1000, 20000,   5},                       /*[40]电池组启动失败(系统故障)*/
	{eErr_SysErr,  1000, 20000, 255},                       /*[41]电池组停机带载超时*/
	{eErr_SysErr,  1000, 15000, 255},                       /*[42]电池组停机失败(系统故障)*/
	{eErr_SysErr,     0,  5000,   2},                       /*[43]电池组按钮紧急停机*/
	{eErr_SysPro,  3000,  3000, 255},                       /*[44]断路器未合闸*/
	{eErr_SysPro,  5000,  3000, 255},                       /*[45]电池组内外压差过大*/
	{eErr_Warm,       0,  3000, 255},                       /*[46]电池组带载闭合*/
	{eErr_Warm,       0,  3000, 255}                        /*[47]电池组带载切断*/
};

const u16 cGAlarmDispBit[ALARM_INDEX_LEN] =                 /*需要对外显示的告警位置1*/
{
    0x0ffc, 0xffff, 0x0ff0
};

const u16 cGFaultDispBit[FAULT_INDEX_LEN] =                 /*需要对外显示的故障位置1*/
{
    0xfeff, 0xffff, 0x3fff
};

static u16 gGRealAlarmInfo[ALARM_INDEX_LEN] = {0};          /*实时告警信息数组(包括不对外显示的告警位)*/
static u16 gGRealFaultInfo[FAULT_INDEX_LEN] = {0};          /*实时故障信息数组(包括不对外显示的故障位)*/

extern u16 gCANVerTime_0[];                                 /*校时信息*/
extern u16 gGHisErrInfo_45[];	                            /*历史故障记录信息查询数组*/
extern u16 gGHisWaveInfo_47[];		                        /*历史故障录波信息查询数组*/
extern u16 gGAlarmInfo_53[];			                    /*告警信息数组*/
extern u16 gGFaultInfo_54[];			                    /*故障信息数组*/
extern u16 gGErrDetail_55[];			                    /*故障定位信息数组*/

extern u16 gGUserCtrl_87[];	    	                        /*用户控制命令信息数组*/

extern u16 gGBmuGenPara_102[];                              /*主控通用参数值*/
extern u8 gGHardPara_104[]; 		                        /*主控硬件参数配置信息*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u8 DiagGetExpBefState(e_ERR_ID id)
** @Input     : id:故障码ID
** @Output    : 故障前置状态:0:正常 1:故障/告警
** @Function  : 获取故障前置状态
** @The notes : 当前显示状态
**===============================================================================================*/
static u8 DiagGetExpBefState(e_ERR_ID id);

/*=================================================================================================
** @Name      : static void DiagSetErrDisplayState(e_ERR_ID id, u8 state)
** @Input     : id:故障码ID state:设置状态:0清除状态 1:发生状态
** @Output    : void
** @Function  : 设置显示故障码状态
** @The notes : 清0或置1
**===============================================================================================*/
static void DiagSetErrDisplayState(e_ERR_ID id, u8 state);

/*=================================================================================================
** @Name      : static void DiagRecordHisErrInfo(e_ERR_ID id, eErr_RcdSort sort)
** @Input     : id:故障码ID sort:记录故障类型  1告警产生 2告警消除 3故障产生 4故障消除
** @Output    : void
** @Function  : 记录历史故障信息
** @The notes : 显示历史故障记录并故障事件记录
**===============================================================================================*/
static void DiagRecordHisErrInfo(e_ERR_ID id, eErr_RcdSort sort);

/*=================================================================================================
** @Name      : static u16 DiagGetErrDetailInfo(e_ERR_ID id)
** @Input     : id:故障码ID
** @Output    : 故障定位对应详细信息
** @Function  : 获取故障定位对应详细信息
** @The notes :
**===============================================================================================*/
static u16 DiagGetErrDetailInfo(e_ERR_ID id);

/*=================================================================================================
** @Name      : static u8 DiagBuildErrEventAddInfo(e_ERR_ID id, u16 *addInfo)
** @Input     : id:故障码ID addInfo:构建的附加信息数组(长度必须是5)
** @Output    : 构建结果:TRUE成功 FALSE失败
** @Function  : 构建故障事件记录附加信息
** @The notes :
**===============================================================================================*/
static u8 DiagBuildErrEventAddInfo(e_ERR_ID id, u16 *addInfo);

/*=================================================================================================
** @Name      : static u8 DiagBuildSelfErrEvnAddInfo(e_ERR_ID id, u16 *addInfo)
** @Input     : code:故障码 addInfo:构建的附加信息数组(长度必须是5)
** @Output    : 构建结果:TRUE成功 FALSE失败
** @Function  : 构建用户自定义故障记录附加信息
** @The notes :
**===============================================================================================*/
static u8 DiagBuildSelfErrRcdAddInfo(u8 code, u16 *addInfo);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void DiagErrDisplayInit(void)
** @Input     : void
** @Output    : void
** @Function  : 故障显示信息初始化
** @The notes : 读取初始显示的历史故障,用户任务单次调用(避免占用太长系统初始化时间)
**===============================================================================================*/
void DiagErrDisplayInit(void)
{
    u8 i = 0;
    t_RcdStoreInfo info = {0};

	//读一条历史故障记录到查询显示区
	if(TRUE == RcdDisplayOneErrRcdUserAPI(0, &info))
	{
		//记录主要信息
		gGHisErrInfo_45[i*ERR_RCD_ERRINFO_LEN] = info.codeH & 0x00ff;
		gGHisErrInfo_45[i*ERR_RCD_ERRINFO_LEN+1] = info.codeL;
		gGHisErrInfo_45[i*ERR_RCD_ERRINFO_LEN+2] = info.year;
		gGHisErrInfo_45[i*ERR_RCD_ERRINFO_LEN+3] = info.MD;
		gGHisErrInfo_45[i*ERR_RCD_ERRINFO_LEN+4] = info.HM;
		gGHisErrInfo_45[i*ERR_RCD_ERRINFO_LEN+5] = info.sec;

		//记录附件信息
		gGHisErrInfo_45[i*ERR_RCD_ERRINFO_LEN+6] = info.info1;
		gGHisErrInfo_45[i*ERR_RCD_ERRINFO_LEN+7] = info.info2;
		gGHisErrInfo_45[i*ERR_RCD_ERRINFO_LEN+8] = info.info3;
		gGHisErrInfo_45[i*ERR_RCD_ERRINFO_LEN+9] = info.info4;
		gGHisErrInfo_45[i*ERR_RCD_ERRINFO_LEN+10] = info.info5;
	}

	//读取1条历史故障录波信息(当前使用显示区域数组作录波缓存区不用读取)
	//(void)RcdDisplayOneWaveRcdUserAPI(0, gGHisWaveInfo_47, GINDEX47_LEN);
}

/*=================================================================================================
** @Name      : void DiagErrDisplayTask(void)
** @Input     : void
** @Output    : void
** @Function  : 故障显示任务
** @The notes : 100ms周期,将滤波故障转入故障显示位
**===============================================================================================*/
void DiagErrDisplayTask(void)
{
	u16 flag = 0;
	e_ERR_ID i = eERR_ID_FINISH;
	e_ERR_ID index = eERR_ID_FINISH;
	static u8 sFlag = 0;

	/*记录显示未初始化*/
    if(0 == sFlag)
	{
    	/*记录存储模块未初始化*/
        if(0 == RcdGetErrRcdEnStateAPI())
    	{
        	/*记录存储读写指针初始化*/
        	RcdStoreStateInit();
    	}
        else
        {
        	/*记录显示初始化*/
        	DiagErrDisplayInit();
        	sFlag = 1;
        }
	}

    for(i = (e_ERR_ID)0; i < eERR_ID_FINISH; i++)           /*检测滤波最大个数*/
    {
		if(eErr_B == DiagGetErrLastStateAPI(i))             /*出现告警*/
		{
			if(0 == DiagGetExpBefState(i))                  /*前置状态不是异常*/
			{
				/*产生级别更高的故障*/
				if(DiagGetFilErrSortAPI(i) > DiagGetFilErrSortAPI(index))
				{
					index = i;                              /*记录故障定位码*/
					flag = DiagGetErrDetailInfo(i);         /*记录故障定位详细信息*/
				}

				/*产生告警*/
				if((eErr_ChgLim == DiagGetFilErrSortAPI(i))
					|| (eErr_DhgLim == DiagGetFilErrSortAPI(i))
					|| (eErr_SysLim == DiagGetFilErrSortAPI(i))
					|| (eErr_ChgPro == DiagGetFilErrSortAPI(i))
					|| (eErr_DhgPro == DiagGetFilErrSortAPI(i)))
				{
					/*告警数目计数*/
					GroupHisErrNumAddHandle(0);

					/*记录显示已初始化*/
					if(sFlag > 0)
					{
						/*历史故障记录告警信息产生*/
						DiagRecordHisErrInfo(i, eERR_RCDSORT_WARSET);
					}
				}
				/*产生故障*/
				else if((eErr_SysPro == DiagGetFilErrSortAPI(i))
					|| (eErr_SysErr == DiagGetFilErrSortAPI(i)))
				{
					/*故障数目计数*/
					GroupHisErrNumAddHandle(1);

					/*记录显示已初始化*/
					if(sFlag > 0)
					{
						/*历史故障记录故障信息产生*/
						DiagRecordHisErrInfo(i, eERR_RCDSORT_ERRSET);
					}
				}
				/*产生提醒*/
				else if(eErr_Warm == DiagGetFilErrSortAPI(i))
				{
					/*记录显示已初始化*/
					if(sFlag > 0)
					{
						/*历史故障记录告警信息产生*/
						DiagRecordHisErrInfo(i, eERR_RCDSORT_WARSET);
					}
				}
			}

			DiagSetErrDisplayState(i, 1);                   /*设置产生异常状态*/
		}
		else                                                /*无异常*/
		{
			if(1 == DiagGetExpBefState(i))                  /*前置状态异常*/
			{
				/*记录显示已初始化*/
				if(sFlag > 0)
				{
					/*消除告警*/
					if((eErr_ChgLim == DiagGetFilErrSortAPI(i))
						|| (eErr_DhgLim == DiagGetFilErrSortAPI(i))
						|| (eErr_SysLim == DiagGetFilErrSortAPI(i))
						|| (eErr_ChgPro == DiagGetFilErrSortAPI(i))
						|| (eErr_DhgPro == DiagGetFilErrSortAPI(i)))
					{
						/*历史故障记录告警信息消除*/
						DiagRecordHisErrInfo(i, eERR_RCDSORT_WARCLR);
					}
					/*消除故障*/
					else if((eErr_SysPro == DiagGetFilErrSortAPI(i))
						|| (eErr_SysErr == DiagGetFilErrSortAPI(i)))
					{
						/*历史故障记录故障信息消除*/
						DiagRecordHisErrInfo(i, eERR_RCDSORT_ERRCLR);
					}
					/*消除提示*/
					else if(eErr_Warm == DiagGetFilErrSortAPI(i))
					{
						/*历史故障记录告警信息产生*/
						DiagRecordHisErrInfo(i, eERR_RCDSORT_WARCLR);
					}
				}
			}

			DiagSetErrDisplayState(i, 0);                   /*清除异常状态*/
		}
    }

    /*更新故障定位信息*/
    if(eERR_ID_FINISH != index)                             /*有故障产生*/
    {
    	gGErrDetail_55[eErrDetail55_Code] = (u16)index + 1;
    	gGErrDetail_55[eErrDetail55_Info] = flag;
    }
    else
    {
    	gGErrDetail_55[eErrDetail55_Code] = 0;
    	gGErrDetail_55[eErrDetail55_Info] = 0;
    }
}

/*=================================================================================================
** @Name      : void DiagUserSelfErrRecord(u8 code, eErr_RcdSort sort, u16 rcdInfo0,  u16 rcdInfo1)
** @Input     : id:用户故障码  sort:记录故障类型  1告警产生 3故障产生 rcdInfo0:需记录的信息0 rcdInfo1:需记录的信息1
** @Output    : void
** @Function  : 用户增加自定义故障记录(用户自定义仅记录故障告警产生)
** @The notes : 用户需要产生自定义故障时调用
**===============================================================================================*/
void DiagUserSelfErrRecord(u8 code, eErr_RcdSort sort, u16 rcdInfo0,  u16 rcdInfo1)
{
	t_SysClock sysClock = {0};
    t_RcdCreateInfo rcdInfo = {0};

    //参数错误
	if((code >= 0x0100) || ((sort != eERR_RCDSORT_WARSET) && (sort != eERR_RCDSORT_ERRSET)))
	{
		return;
	}

    /*取到相应故障记录附加信息*/
    if(TRUE == DiagBuildSelfErrRcdAddInfo(code, &rcdInfo.info1))
    {
		//故障类型(告警发生)
		rcdInfo.type = sort;
        //故障记录码(自定义告警无前缀)
		rcdInfo.code = code;

		//需要特殊处理增加附加信息的记录
		//单体电压温度采样值异常(记录异常值)
		//压降或温升异常(记录变化值)
		//电流采样异常(记录AD值)
		if((eUDef_ErrRcd_CellVErr == code) || (eUDef_ErrRcd_CellTErr == code)
			|| (eUDef_ErrRcd_CellVDn == code) || (eUDef_ErrRcd_CellTUp == code)
			|| (eUDef_ErrRcd_HallCErr == code) || (eUDef_ErrRcd_ShunCErr == code)
			|| (eUDef_ErrRcd_SmcuErr == code))
		{
			rcdInfo.info5 = rcdInfo0;
		}
		//充放电效率异常(记录充放电量)
		//SOS模型推断异常(记录异常前后值)
		//实时SOS等级异常(记录异常前后值)
		//预测SOS等级异常(记录异常前后值)
		else if((eUDef_ErrRcd_ChgEffe == code) || (eUDef_ErrRcd_SosMode == code)
			|| (eUDef_ErrRcd_ReSosErr == code) || (eUDef_ErrRcd_PrSosErr == code))
		{
			rcdInfo.info3 = rcdInfo0;
			rcdInfo.info4 = rcdInfo1;
		}

		//产生故障事件记录
		RcdCreateErrRcdInfoAPI(&rcdInfo);

		//正在查询显示当天故障信息才更新到显示最新内容
		if(0 == gGUserCtrl_87[eUserCtrl87_HisErrSw])
		{
			//显示最新新历史故障记录
			//记录类型
			gGHisErrInfo_45[0] = sort;
			//记录告警码(自定义告警无前缀)
			gGHisErrInfo_45[1] = code;

			/*记录时间信息*/
			sysClock = GetSysDateClockAPI();

			gGHisErrInfo_45[2] = sysClock.year;                                 //记录时间-年
			gGHisErrInfo_45[3] = ((u16)sysClock.month << 8) | sysClock.day;     //记录时间-月日
			gGHisErrInfo_45[4] = ((u16)sysClock.hour << 8) | sysClock.minute;   //记录时间-时分
			gGHisErrInfo_45[5] = sysClock.second;                               //记录时间-秒

			//显示附加信息
			gGHisErrInfo_45[6] = rcdInfo.info1;
			gGHisErrInfo_45[7] = rcdInfo.info2;
			gGHisErrInfo_45[8] = rcdInfo.info3;
			gGHisErrInfo_45[9] = rcdInfo.info4;
			gGHisErrInfo_45[10] = rcdInfo.info5;

			/*触发发送历史故障信息显示*/
			SendCanGroupHisErrInfoMsg();
		}
    }
}

/*=================================================================================================
** @Name      : void DiagChangHisErrDisplayAPI(u8 date, u8 home)
** @Input     : date:历史故障日期[0,30]:0-30天前记录 home:读首页标志 0:读当前记录 1:读当日首页记录
** @Output    : void
** @Function  : 切换历史故障显示信息
** @The notes : 按照日期读取指定时间的历史故障显示(读30条)
**===============================================================================================*/
void DiagChangHisErrDisplayAPI(u8 date, u8 home)
{
    static u8 sHisDate = 0;
    t_RcdStoreInfo info = {0};

    /*不切换日期且读首页记录则跳转切换首页*/
    if((sHisDate == date) && (1 == home))
    {
    	/*切换首页失败*/
    	if(FALSE == RcdJumpDispHomeErrRcdUserAPI())
    	{
    		/*不更新显示*/
    		return;
    	}
    }

	/*读取一条记录成功*/
	if(TRUE == RcdDisplayOneErrRcdUserAPI(date, &info))
	{
		//记录主要信息
		gGHisErrInfo_45[0] = info.codeH & 0x00ff;
		gGHisErrInfo_45[1] = info.codeL;
		gGHisErrInfo_45[2] = info.year;
		gGHisErrInfo_45[3] = info.MD;
		gGHisErrInfo_45[4] = info.HM;
		gGHisErrInfo_45[5] = info.sec;

		//记录附件信息
		gGHisErrInfo_45[6] = info.info1;
		gGHisErrInfo_45[7] = info.info2;
		gGHisErrInfo_45[8] = info.info3;
		gGHisErrInfo_45[9] = info.info4;
		gGHisErrInfo_45[10] = info.info5;
	}
	/*读取已失败*/
	else
	{
		/*切换日期未读取到记录*/
		if(sHisDate != date)
		{
			/*剩余显示清零*/
			gGHisErrInfo_45[0] = 0;
			gGHisErrInfo_45[1] = 0;
			gGHisErrInfo_45[2] = 0;
			gGHisErrInfo_45[3] = 0;
			gGHisErrInfo_45[4] = 0;
			gGHisErrInfo_45[5] = 0;

			gGHisErrInfo_45[6] = 0;
			gGHisErrInfo_45[7] = 0;
			gGHisErrInfo_45[8] = 0;
			gGHisErrInfo_45[9] = 0;
			gGHisErrInfo_45[10] = 0;
		}
	}
    sHisDate = date;

	/*触发发送历史故障信息*/
	SendCanGroupHisErrInfoMsg();
}

/*=================================================================================================
** @Name      : void DiagClrHisErrDisplayAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除历史故障显示信息
** @The notes : 显示区域清零
**===============================================================================================*/
void DiagClrHisErrDisplayAPI(void)
{
	/*历史故障记录查询显示区清零*/
	gGHisErrInfo_45[0] = 0;
	gGHisErrInfo_45[1] = 0;
	gGHisErrInfo_45[2] = 0;
	gGHisErrInfo_45[3] = 0;
	gGHisErrInfo_45[4] = 0;
	gGHisErrInfo_45[5] = 0;

	gGHisErrInfo_45[6] = 0;
	gGHisErrInfo_45[7] = 0;
	gGHisErrInfo_45[8] = 0;
	gGHisErrInfo_45[9] = 0;
	gGHisErrInfo_45[10] = 0;

	/*触发发送历史故障查询信息*/
	SendCanGroupHisErrInfoMsg();
}

/*=================================================================================================
** @Name      : void DiagChangHisWaveDisplayAPI(u8 date, u8 home)
** @Input     : date:历史故障录波日期[0,30]:0-30天前记录 home:读首页标志 0:读当前记录 1:读当日首页记录
** @Output    : void
** @Function  : 切换历史故障录波显示信息
** @The notes : 按照日期读取指定时间的历史故障录波显示(读1条)
**===============================================================================================*/
void DiagChangHisWaveDisplayAPI(u8 date, u8 home)
{
    u8 i = 0;
    static u8 sHisDate = 0;

    /*不切换日期且读首页记录则跳转切换首页*/
    if((sHisDate == date) && (1 == home))
    {
    	/*切换首页失败*/
    	if(FALSE == RcdJumpDispHomeWaveRcdUserAPI())
    	{
    		/*显示区已有有效记录*/
    		if((gGHisWaveInfo_47[0] > 0) && (gGHisWaveInfo_47[1] > 0))
    		{
    			/*不更新显示*/
    			return;
    		}
    	}
    }

	/*读取一条记录失败*/
	if(FALSE == RcdDisplayOneWaveRcdUserAPI(date, gGHisWaveInfo_47, GINDEX47_LEN))
	{
		/*切换日期未读取到记录*/
		if(sHisDate != date)
		{
			for(i = 0; i < GINDEX47_LEN; i++)
			{
				/*显示清零*/
				gGHisWaveInfo_47[i] = 0;
			}
		}
	}
    sHisDate = date;

	/*触发发送历史故障录波信息*/
    SendCanGroupHisWaveInfoMsg();
}

/*=================================================================================================
** @Name      : void DiagClrHisWaveDisplayAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除历史故障录波显示信息
** @The notes : 显示区域清零
**===============================================================================================*/
void DiagClrHisWaveDisplayAPI(void)
{
    u8 i = 0;

	/*显示清零*/
	for(i = 0; i < GINDEX47_LEN; i++)
	{
		gGHisWaveInfo_47[i] = 0;
	}

	/*触发发送历史故障信息*/
	SendCanGroupHisWaveInfoMsg();
}
/*=================================================================================================
** @Name      : void DiagRcdHisErrInfoRecordAPI(u8 id, u8 sort)
** @Input     : id:故障码ID sort:记录故障类型  1告警产生 2告警消除 3故障产生 4故障消除
** @Output    : void
** @Function  : 用户记录历史故障事件信息
** @The notes : 需要故障产生前记录事件时调用
**===============================================================================================*/
void DiagRcdHisErrInfoRecordAPI(u8 id, u8 sort)
{
	DiagRecordHisErrInfo(id, sort);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u8 DiagGetExpBefState(e_ERR_ID id)
** @Input     : id:故障码ID
** @Output    : 故障前置状态:0:正常 1:故障/告警
** @Function  : 获取故障前置状态
** @The notes : 当前显示状态
**===============================================================================================*/
static u8 DiagGetExpBefState(e_ERR_ID id)
{
    u8 state = 0;
    u8 index = 0;
    u8 expbit = 0;

    if(id < eERR_ID_WARFIN)                                 //告警ID(按位显示)
    {
        //计算故障位
        index = (u32)id / ALARM_DATATYPE;                   //获得告警id的数组子索引
        if(index >= ALARM_INDEX_LEN)                        //告警数组子索引越界
        {
            return(0xff);                                   //返回无效
        }

        expbit = id % ALARM_DATATYPE;                       //获得告警相应位

        state = (u8)BitGet(gGRealAlarmInfo[index], expbit); //获得告警位状态
    }
    else if(id < eERR_ID_FINISH)                            //故障ID(按位显示)
    {
        //计算故障位
        index = (u32)(id - eERR_ID_ERRSTART) / FAULT_DATATYPE;//获得故障id的数组子索引
        if(index >= FAULT_INDEX_LEN)                        //故障数组子索引越界
        {
            return(0xff);                                   //返回无效
        }

        expbit = id % FAULT_DATATYPE;                       //获得故障相应位

        state = (u8)BitGet(gGRealFaultInfo[index], expbit); //获得故障位状态
    }
    else                                                    //ID无效
    {
    	return(0xff);                                       //返回无效
    }

    return(state);
}

/*=================================================================================================
** @Name      : static void DiagSetErrDisplayState(e_ERR_ID id, u8 state)
** @Input     : id:故障码ID state:设置状态:0清除状态 1:发生状态
** @Output    : void
** @Function  : 设置显示故障码状态
** @The notes : 清0或置1
**===============================================================================================*/
static void DiagSetErrDisplayState(e_ERR_ID id, u8 state)
{
    u8 index = 0;
    u8 expbit = 0;

    if(id < eERR_ID_WARFIN)                                 /*告警ID(按位显示)*/
    {
        /*计算故障位*/
        index = (u32)id / ALARM_DATATYPE;                   /*获得告警id的数组子索引*/
        if(index >= ALARM_INDEX_LEN)                        /*告警数组子索引越界*/
        {
            return;                                         /*无效*/
        }

        expbit = id % ALARM_DATATYPE;                       /*获得告警相应位*/

        if(1 == state)                                      /*告警发生*/
        {
            BitSet(gGRealAlarmInfo[index], expbit);         /*设置告警位状态置1*/

            if((1 == BitGet(cGAlarmDispBit[index], expbit)) /*策略需要显示该告警位状态*/
            	|| (TRUE == DiagCheckErrNeedDispHOOK(id)))  /*用户需要显示该告警位状态*/
            {
                BitSet(gGAlarmInfo_53[index], expbit);      /*设置告警显示位状态置1*/
            }
        }
        else
        {
        	BitClr(gGRealAlarmInfo[index], expbit);         /*消除告警位状态清0*/
        	BitClr(gGAlarmInfo_53[index], expbit);          /*消除告警显示位状态清0*/
        }
    }
    else if(id < eERR_ID_FINISH)                            /*故障ID(按位显示)*/
    {
        /*计算故障位*/
        index = (u32)(id - eERR_ID_ERRSTART) / FAULT_DATATYPE;/*获得故障id的数组子索引*/
        if(index >= FAULT_INDEX_LEN)                        /*故障数组子索引越界*/
        {
            return;                                         /*无效*/
        }

        expbit = id % FAULT_DATATYPE;                       /*获得故障相应位*/

        if(1 == state)                                      /*故障发生*/
        {
            BitSet(gGRealFaultInfo[index], expbit);         /*设置故障位状态置1*/

            if(1 == BitGet(cGFaultDispBit[index], expbit))  /*显示该故障位状态*/
            {
                BitSet(gGFaultInfo_54[index], expbit);      /*设置故障显示位状态置1*/
            }
        }
        else
        {
        	BitClr(gGRealFaultInfo[index], expbit);         /*消除故障位状态清0*/
        	BitClr(gGFaultInfo_54[index], expbit);          /*消除故障显示位状态清0*/
        }
    }
}

/*=================================================================================================
** @Name      : static void DiagRecordHisErrInfo(e_ERR_ID id, eErr_RcdSort sort)
** @Input     : id:故障码ID sort:记录故障类型  1告警产生 2告警消除 3故障产生 4故障消除
** @Output    : void
** @Function  : 记录历史故障信息
** @The notes : 显示历史故障记录并故障事件记录
**===============================================================================================*/
static void DiagRecordHisErrInfo(e_ERR_ID id, eErr_RcdSort sort)
{
	t_SysClock sysClock = {0};
    t_RcdCreateInfo rcdInfo = {0};

    //参数错误
    if((id >= eERR_ID_FINISH) || (sort >= eERR_RCDSORT_LEN))
    {
        return;
    }

    /*取到相应故障记录附加信息*/
    if(TRUE == DiagBuildErrEventAddInfo(id, &rcdInfo.info1))
    {
		rcdInfo.type = sort;

		//告警类型
		if((eERR_RCDSORT_WARSET == sort) || (eERR_RCDSORT_WARCLR == sort))
		{
			rcdInfo.code = ERR_RCD_WARCODE_FST + id;        //告警记录码
		}
		//故障类型
		else
		{
			rcdInfo.code = ERR_RCD_ERRCODE_FST + id;        //故障记录码
		}

		//产生故障事件记录
		RcdCreateErrRcdInfoAPI(&rcdInfo);

		//正在查询显示当天故障信息才更新到显示最新内容
		if(0 == gGUserCtrl_87[eUserCtrl87_HisErrSw])
		{
			//显示最新新历史故障记录
			//记录类型
			gGHisErrInfo_45[0] = sort;

			//告警类型
			if((eERR_RCDSORT_WARSET == sort) || (eERR_RCDSORT_WARCLR == sort))
			{
				gGHisErrInfo_45[1] = ERR_RCD_WARCODE_FST + id;      //告警记录码
			}
			//故障类型
			else
			{
				gGHisErrInfo_45[1] = ERR_RCD_ERRCODE_FST + id;      //故障记录码
			}

			/*记录时间信息*/
			sysClock = GetSysDateClockAPI();

			gGHisErrInfo_45[2] = sysClock.year;                                 //记录时间-年
			gGHisErrInfo_45[3] = ((u16)sysClock.month << 8) | sysClock.day;     //记录时间-月日
			gGHisErrInfo_45[4] = ((u16)sysClock.hour << 8) | sysClock.minute;   //记录时间-时分
			gGHisErrInfo_45[5] = sysClock.second;                               //记录时间-秒

			//显示附加信息
			gGHisErrInfo_45[6] = rcdInfo.info1;
			gGHisErrInfo_45[7] = rcdInfo.info2;
			gGHisErrInfo_45[8] = rcdInfo.info3;
			gGHisErrInfo_45[9] = rcdInfo.info4;
			gGHisErrInfo_45[10] = rcdInfo.info5;

			/*触发发送历史故障信息显示*/
			SendCanGroupHisErrInfoMsg();
		}
    }

    /*故障告警发生时进行故障录波处理*/
    if((eERR_RCDSORT_WARSET == sort)
    	|| (eERR_RCDSORT_ERRSET == sort))
    {
    	/*单体过压二级告警*/
    	if(eERR_ID_CEL_VH2 == id)
    	{
        	/*单体过压二级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_CVH2);
    	}
    	/*单体过压一级告警*/
    	else if(eERR_ID_CEL_VH1 == id)
    	{
        	/*单体过压一级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_CVH1);
    	}
    	/*单体欠压二级告警*/
    	else if(eERR_ID_CEL_VL2 == id)
    	{
        	/*单体欠压二级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_CVL2);
    	}
    	/*单体欠压一级告警*/
    	else if(eERR_ID_CEL_VL1 == id)
    	{
        	/*单体欠压一级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_CVL1);
    	}
    	/*总压过压二级告警*/
    	else if(eERR_ID_SUM_VH2 == id)
    	{
        	/*总压过压二级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_HVH2);
    	}
    	/*总压过压一级告警*/
    	else if(eERR_ID_SUM_VH1 == id)
    	{
        	/*总压过压一级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_HVH1);
    	}
    	/*总压欠压二级告警*/
    	else if(eERR_ID_SUM_VL2 == id)
    	{
        	/*总压欠压二级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_HVL2);
    	}
    	/*总压欠压一级告警*/
    	else if(eERR_ID_SUM_VL1 == id)
    	{
        	/*总压欠压一级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_HVL1);
    	}
    	/*充电过温二级告警*/
    	else if(eERR_ID_CHG_TH2 == id)
    	{
        	/*充电过温二级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_CTH2);
    	}
    	/*充电过温一级告警*/
    	else if(eERR_ID_CHG_TH1 == id)
    	{
        	/*充电过温一级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_CTH1);
    	}
    	/*充电低温二级告警*/
    	else if(eERR_ID_CHG_TL2 == id)
    	{
        	/*充电低温二级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_CTL2);
    	}
    	/*充电低温一级告警*/
    	else if(eERR_ID_CHG_TL1 == id)
    	{
        	/*充电低温一级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_CTL1);
    	}
    	/*放电过温二级告警*/
    	else if(eERR_ID_DHG_TH2 == id)
    	{
        	/*放电过温二级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_DTH2);
    	}
    	/*放电过温一级告警*/
    	else if(eERR_ID_DHG_TH1 == id)
    	{
        	/*放电过温一级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_DTH1);
    	}
    	/*放电低温二级告警*/
    	else if(eERR_ID_DHG_TL2 == id)
    	{
        	/*放电低温二级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_DTL2);
    	}
    	/*放电低温一级告警*/
    	else if(eERR_ID_DHG_TL1 == id)
    	{
        	/*放电低温一级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_DTL1);
    	}
    	/*充电过流二级告警*/
    	else if(eERR_ID_CHG_CO2 == id)
    	{
        	/*充电过流二级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_CCO2);
    	}
    	/*充电过流一级告警*/
    	else if(eERR_ID_CHG_CO1 == id)
    	{
        	/*充电过流一级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_CCO1);
    	}
    	/*放电过流二级告警*/
    	else if(eERR_ID_DHG_CO2 == id)
    	{
        	/*放电过流二级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_DCO2);
    	}
    	/*放电过流一级告警*/
    	else if(eERR_ID_DHG_CO1 == id)
    	{
        	/*放电过流一级告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_DCO1);
    	}
    	/*充电超限值告警*/
    	else if(eERR_ID_CHG_COT == id)
    	{
        	/*充电超限值告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_CCOL);
    	}
    	/*放电超限值告警*/
    	else if(eERR_ID_DHG_COT == id)
    	{
        	/*放电超限值告警故障录波*/
    		RcdSetFaultWaveTrigAPI(eFatWavSort_DCOL);
    	}
    }

	//更新显示故障记录数目
	//ErrEvntRcdUpdateStateAPI();
}

/*=================================================================================================
** @Name      : static u16 DiagGetErrDetailInfo(e_ERR_ID id)
** @Input     : id:故障码ID
** @Output    : 故障定位对应详细信息
** @Function  : 获取故障定位对应详细信息
** @The notes :
**===============================================================================================*/
static u16 DiagGetErrDetailInfo(e_ERR_ID id)
{
    u16 flag = 0;

    /*根据故障码选择相应定位信息*/
    switch(id)
    {
    	/*单体电压高2级(充电截止)*/
    	case eERR_ID_CEL_VH2:
    		flag = GetGCellMaxVoltAPI();
    		break;

    	/*单体电压低2级(放电截止)*/
    	case eERR_ID_CEL_VL2:
    		flag = GetGCellMinVoltAPI();
    		break;

    	/*电池总电压过压2级(充电截止)*/
    	case eERR_ID_SUM_VH2:
    		flag = GetGSampSumVoltAPI();
    		break;

    	/*电池总电压欠压2级(放电截止)*/
    	case eERR_ID_SUM_VL2:
    		flag = GetGSampSumVoltAPI();
    		break;

    	/*充电温度高2级(充电截止)*/
    	case eERR_ID_CHG_TH2:
    		flag = GetGCellMaxTempAPI();
    		break;

        /*充电温度低2级(充电截止)*/
    	case eERR_ID_CHG_TL2:
        	flag = GetGCellMinTempAPI();
        	break;

		/*放电温度高2级(放电截止)*/
		case eERR_ID_DHG_TH2:
			flag = GetGCellMaxTempAPI();
			break;

        /*放电温度低2级(放电截止)*/
    	case eERR_ID_DHG_TL2:
        	flag = GetGCellMinTempAPI();
        	break;

        /*充电过流2级(限制充电)*/
    	case eERR_ID_CHG_CO2:
        	flag = (u16)GetGSampOutCurrAPI();
        	break;

        /*放电过流2级(限制放电)*/
    	case eERR_ID_DHG_CO2:
        	flag = (u16)GetGSampOutCurrAPI();
        	break;

        /*外设温度高2级(充放电限制)*/
    	case eERR_ID_PER_TH2:
    		if(GetGEnvirTempAPI() > GetGShuntSampTempAPI())
    		{
        		flag = GetGEnvirTempAPI();
    		}
    		else
    		{
    			flag = GetGShuntSampTempAPI();
    		}
        	break;

        /*电池组绝缘2级(充放电限制)*/
    	case eERR_ID_LEAK2:
    		if(GetGPasInsuResistAPI() < GetGNegInsuResistAPI())
			{
				flag = GetGPasInsuResistAPI();
			}
			else
			{
				flag = GetGNegInsuResistAPI();
			}
        	break;

        /*单体电压一致性下降2级(预警)*/
    	case eERR_ID_CV_DIF2:
        	flag = GetGCellMaxVoltAPI() - GetGCellMinVoltAPI();
        	break;

        /*单体温度温差过大2级(预警)*/
    	case eERR_ID_CT_DIF2:
        	flag = GetGCellMaxTempAPI() - GetGCellMinTempAPI();
        	break;

        /*充电容量SOC高2级(预警)*/
    	case eERR_ID_CAP_SH2:
        	flag = GetGBattSocAPI();
        	break;

        /*剩余容量SOC低2级(预警)*/
    	case eERR_ID_CAP_SL2:
        	flag = GetGBattSocAPI();
        	break;

    	/*以下为电池1级告警ID*/
        /*单体电压高1级(充电保护)*/
    	case eERR_ID_CEL_VH1:
			flag = GetGCellMaxVoltAPI();
			break;

		/*单体电压低1级(放电保护)*/
    	case eERR_ID_CEL_VL1:
        	flag = GetGCellMinVoltAPI();
        	break;

        /*电池总电压过压1级(充电保护)*/
    	case eERR_ID_SUM_VH1:
        	flag = GetGSampSumVoltAPI();
        	break;

        /*电池总电压欠压1级(放电保护)*/
    	case eERR_ID_SUM_VL1:
        	flag = GetGSampSumVoltAPI();
        	break;

        /*充电温度高1级(充电保护)*/
    	case eERR_ID_CHG_TH1:
        	flag = GetGCellMaxTempAPI();
        	break;

		/*充电温度低1级(充电保护)*/
		case eERR_ID_CHG_TL1:
			flag = GetGCellMinTempAPI();
			break;

        /*放电温度高1级(放电保护)*/
    	case eERR_ID_DHG_TH1:
        	flag = GetGCellMaxTempAPI();
        	break;

        /*放电温度低1级(放电保护)*/
    	case eERR_ID_DHG_TL1:
        	flag = GetGCellMinTempAPI();
        	break;

        /*充电过流1级(充电延时保护)*/
    	case eERR_ID_CHG_CO1:
        	flag = GetGSampOutCurrAPI();
        	break;

        /*放电过流1级(放电延时保护)*/
    	case eERR_ID_DHG_CO1:
        	flag = GetGSampOutCurrAPI();
        	break;

        /*外设温度过高1级(充放电保护)*/
    	case eERR_ID_PER_TH1:
    		if(GetGEnvirTempAPI() > GetGShuntSampTempAPI())
			{
				flag = GetGEnvirTempAPI();
			}
			else
			{
				flag = GetGShuntSampTempAPI();
			}
        	break;

        /*电池组绝缘1级(充放电保护)*/
    	case eERR_ID_LEAK1:
    		if(GetGPasInsuResistAPI() < GetGNegInsuResistAPI())
			{
				flag = GetGPasInsuResistAPI();
			}
			else
			{
				flag = GetGNegInsuResistAPI();
			}
        	break;

        /*单体电压一致性下降1级(充放电限制)*/
    	case eERR_ID_CV_DIF1:
        	flag = GetGCellMaxVoltAPI() - GetGCellMinVoltAPI();
        	break;

        /*单体温度温差过大1级(充放电限制)*/
    	case eERR_ID_CT_DIF1:
        	flag = GetGCellMaxTempAPI() - GetGCellMinTempAPI();
        	break;

        /*充电容量SOC高1级(充电截止)*/
    	case eERR_ID_CAP_SH1:
        	flag = GetGBattSocAPI();
        	break;

        /*剩余容量SOC低1级(放电截止)*/
    	case eERR_ID_CAP_SL1:
    		flag = GetGBattSocAPI();
    		break;

		/*以下为提示类告警ID*/
    	/*SOC显示异常(需要修正)*/
    	case eERR_ID_SOC_EXP:
    		flag = GetGRealSocMilliAPI();
    		break;

    	/*容量检测异常(需要校准)*/
    	case eERR_ID_CAP_EXP:
     		flag = GetCapEnerDropExpAPI();
        	break;

        /*充电过流响应超时(电流超限值)*/
    	case eERR_ID_CHG_COT:
     		flag = GetGSampOutCurrAPI();
        	break;

        /*放电过流响应超时(电流超限值)*/
    	case eERR_ID_DHG_COT:
     		flag = GetGSampOutCurrAPI();
        	break;

        /*均衡板温异常(关闭均衡)*/
    	case eERR_ID_BAL_TH:
     		flag = (u16)GetGBalanMaxTempAPI();
        	break;

        /*低压电源板温异常(提示)*/
    	case eERR_ID_DCV_TH:
     		flag = (u16)GetGDcPwrSampTempAPI();
        	break;

		/*SOS诊断异常(提示)*/
		case eERR_ID_SOS_EXP:
			flag = DiagGetGroupSosResion(0);
			break;

		/*电池箱风扇异常(提示)*/
		case eERR_ID_FAN_EXP:
			flag = TacGetPwmFanErrFlagAPI();
			break;

		/*电池柜配电板外设(百叶风机空调)状态异常告警(提示)*/
		case eERR_ID_SHU_EXP:
			flag = TacGetAuxDevErrFlagAPI();
			break;

		/*电池柜配电板外设(温湿度采集器/气体检测/空调)通信中断告警(提示)*/
		case eERR_ID_THC_OFF:
			flag = TacGetAuxComErrFlagAPI();
			break;

    	/*故障类型开始*/
		/*以下为从控告警ID(48-51对应[0-3])*/
        /*均衡温度采样异常*/
    	case eERR_ID_BAL_TEXP:
     		flag = DiagGetBalTOffSlaveIdAPI();
        	break;

        /*均衡控制异常*/
    	case eERR_ID_BAL_CEXP:
     		flag = DiagGetBalErrSlaveIdAPI();
        	break;

		/*水浸告警异常*/
		case eERR_ID_FLOOD_ERR:
			flag = GetGPerImeu2SignInfoAPI(eIMEU2Info204_FloodHz);
			break;

        /*电池气体检测浓度异常*/
    	case eERR_ID_CGAS_EXP:
    		if(1 == BitGet(DiagGetPerGasErrFlagAPI(), 2))//气体2告警
    		{
         		flag = GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2);
    		}
    		else
    		{
    			flag = GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1);
    		}
        	break;

    	/*以下为从控内部故障ID(52-55对应[4-7])*/
        /*单体采样电路异常*/
    	case eERR_ID_SAMP_EXP:
     		flag = DiagGetCellVOffSlaveIdAPI();
        	break;

        /*单体电压采样断线*/
    	case eERR_ID_SAMP_VOFF:
     		flag = DiagGetCellVOffSlaveIdAPI();
        	break;

        	/*单体温度采样断线*/
    	case eERR_ID_SAMP_TOFF:
     		flag = DiagGetCellTOffSlaveIdAPI();
        	break;

        /*单体信息缺失*/
    	case eERR_ID_CELL_MISS:
     		flag = DiagGetRcvCellInfoSlaveIdAPI();
        	break;

		/*以下为主从通信故障ID(56-59对应[8-11])*/
       	/*内部主从级联异常(与下级从控连接)*/
        case eERR_ID_INT_LOFF:
     		flag = DiagGetRcvSlaveMsgSlaveIdAPI();
        	break;

        /*内部主从CAN断线故障(与下级从控通讯)*/
    	case eERR_ID_INT_COFF:
     		flag = DiagGetRcvSlaveMsgSlaveIdAPI();
        	break;

        /*外设主从CAN断线故障(与内部绝缘检测仪通讯)*/
    	case eERR_ID_PER_COFF:
     		flag = 0;
        	break;

       	/*外部通讯断线(与上级或充放电设备CAN/RS485通讯)*/
    	case eERR_ID_EXT_COFF:
     		flag = 0;
        	break;

		/*以下为主控软件报警ID(60-63对应[12-15])*/
       	/*主控设备地址冲突*/
    	case eERR_ID_MECU_REP:
     		flag = gGBmuGenPara_102[eBmuGenPara102_BmuDev];
        	break;

        /*故障位17预留*/
    	case eERR_ID_ERR17_KEPP:
     		flag = 0;
        	break;

        /*电池组软件急停*/
    	case eERR_ID_SW_EMERG:
     		flag = TacGetGEmergStopFlagAPI();
        	break;

        /*电池组自行保护*/
    	case eERR_ID_SELF_PRO:
     		flag = TacGetGHappenProFlagAPI();
        	break;

		/*以下为主控内部故障ID(64-79对应[16-31])*/
        /*主控内部RTC异常*/
        case eERR_ID_MRTC_EXP:
     		flag = 0;
        	break;

        /*主控EEPROM存储异常*/
    	case eERR_ID_MEEP_EXP:
     		flag = 0;
        	break;

       	/*主控外部Flash异常*/
    	case eERR_ID_MFLA_EXP:
     		flag = 0;
        	break;

        /*电池组电流采样电路异常*/
    	case eERR_ID_CSAMP_ERR:
     		flag = SampGetCurrSampExpFlagAPI(0xff);
        	break;

       	/*输出端高压采样异常*/
    	case eERR_ID_HSAMP_EXP:
     		flag = ((u16)DiagGetHigVoltExpFlagAPI() << 8) | SampGetHigVSampExpFlagAPI();
        	break;

        /*硬件板低压侧采样异常*/
    	case eERR_ID_LSAMP_EXP:
     		flag = SampGetLowVSampExpFlagAPI();
        	break;

        /*外围温度采样异常*/
    	case eERR_ID_TSAMP_EXP:
     		flag = SampGetLowVSampExpFlagAPI();
        	break;

        /*绝缘采样异常*/
    	case eERR_ID_LEAK_EXP:
    		flag = GetInsuErrStateFlagAPI();
    		break;

    	/*KM1+回检异常(主正接触器HS回检异常)*/
    	case eERR_ID_K0RE_EXP:
     		flag = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Mai);
        	break;

        /*KM2回检异常(预充接触器HS回检异常)*/
    	case eERR_ID_K1RE_EXP:
     		flag = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Pre);
        	break;

        /*KM3+回检异常(负极接触器HS回检异常)*/
    	case eERR_ID_K2RE_EXP:
     		flag = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Neg);
        	break;

        /*KM4回检异常(电源接触器HS回检异常)*/
    	case eERR_ID_K3RE_EXP:
     		flag = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Pwr);
        	break;

        /*KM5-8回检异常(4个附加接触器HS回检异常)*/
    	case eERR_ID_K7RE_EXP:
     		flag = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Hot);
        	break;

        /*KM9回检异常(风扇接触器HS回检异常)*/
    	case eERR_ID_K8RE_EXP:
     		flag = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Fan);
        	break;

        /*KM1-回检异常(主正接触器LS回检异常)*/
    	case eERR_ID_L0RE_EXP:
     		flag = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Mai);
        	break;

        /*KM3-回检异常(负极接触器LS回检异常)*/
    	case eERR_ID_L2RE_EXP:
     		flag = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Neg);
        	break;

		//以下为主控外围故障ID(80-95对应[32-47])
    	/*主正接触器状态异常(可能主接触器烧结或不能吸合)*/
    	case eERR_ID_K0MAI_EXP:
     		flag = ((u16)TacGetGSwhCtrlFailFlagAPI() << 8) | DiagGetSwhStateVExpFlagAPI();
        	break;

        /*预充接触器异常(可能预充电阻断开或预充接触器不能吸合)*/
    	case eERR_ID_K1PRE_EXP:
     		flag = ((u16)TacGetGSwhCtrlFailFlagAPI() << 8) | DiagGetSwhStateVExpFlagAPI();
        	break;

        /*负极接触器异常(可能负极接触器烧结或不能吸合)*/
    	case eERR_ID_K2NEG_EXP:
     		flag = ((u16)TacGetGSwhCtrlFailFlagAPI() << 8) | DiagGetSwhStateVExpFlagAPI();
        	break;

		/*消防系统告警*/
		case eERR_ID_FIRE_EXP:
			flag = GetGEnvirTempAPI();
			break;

        /*电池组熔断器熔断*/
    	case eERR_ID_FUSE_OFF:
     		flag = DEVGPIOGetInputDryState();
        	break;

        /*电池组烟雾报警*/
    	case eERR_ID_BATT_SMOG:
     		flag = DEVGPIOGetInputDryState();
        	break;

        /*电池组启动预充未满(可能带载启动)*/
    	case eERR_ID_STARTUP_VL:
     		flag = GetGSampPreVoltAPI();
        	break;

       	/*电池组启动预充过温(预充接触器过热)*/
    	case eERR_ID_STARTUP_TH:
     		flag = GetGShuntSampTempAPI();
        	break;

        /*电池组启动失败(系统故障)*/
    	case eERR_ID_STARTUP_ERR:
     		flag = TacGetGStartFailFlagAPI();
        	break;

        /*电池组停机带载超时*/
    	case eERR_ID_STOP_CH:
     		flag = GetGSampOutCurrAPI();
        	break;

        /*电池组停机失败(系统故障)*/
    	case eERR_ID_STOP_ERR:
     		flag = TacGetGStopFailFlagAPI();
        	break;

        /*电池组按钮紧急停机*/
    	case eERR_ID_IO_EMERG:
     		flag = GetGSampOutCurrAPI();
        	break;

        /*断路器未合闸*/
    	case eERR_ID_ISO_OFF:
     		flag = GetGWorkStateAPI();
        	break;

        /*电池组内外压差过大*/
    	case eERR_ID_DC_HV_DIFF:
     		flag = ABS(GetGCellSumVoltAPI(), GetGSampPreVoltAPI());
        	break;

        /*电池组带载闭合*/
    	case eERR_ID_SWHON_CH:
     		flag = GetGSampOutCurrAPI();
        	break;

        /*电池组带载断开*/
    	case eERR_ID_SWHOFF_CH:
     		flag = GetGSampOutCurrAPI();
        	break;

    	default:
    		flag = 0;
    		break;
    }

    return(flag);
}

/*=================================================================================================
** @Name      : static u8 DiagBuildErrEventAddInfo(e_ERR_ID id, u16 *addInfo)
** @Input     : id:故障码ID addInfo:构建的附加信息数组(长度必须是5)
** @Output    : 构建结果:TRUE成功 FALSE失败
** @Function  : 构建故障事件记录附加信息
** @The notes :
**===============================================================================================*/
static u8 DiagBuildErrEventAddInfo(e_ERR_ID id, u16 *addInfo)
{
    //当前保护状态和工作状态
    addInfo[0] = ((u16)GetGProtectStateAPI() << 8) | (GetGWorkStateAPI() & 0x00ff);

    //当前输出电流
    addInfo[1] = (u16)GetGSampOutCurrAPI();

    /*根据故障码选择相应自定义附加信息*/
    switch(id)
    {
    	/*单体电压高2级(充电截止)*/
    	case eERR_ID_CEL_VH2:
    		addInfo[2] = GetGCellMaxVoltAPI();
    		addInfo[3] = GetGCellMaxVNumAPI();
    		addInfo[4] = GetGCellAvgVoltAPI();
    		break;

    	/*单体电压低2级(放电截止)*/
    	case eERR_ID_CEL_VL2:
    		addInfo[2] = GetGCellMinVoltAPI();
    		addInfo[3] = GetGCellMinVNumAPI();
    		addInfo[4] = GetGCellAvgVoltAPI();
    		break;

    	/*电池总电压过压2级(充电截止)*/
    	case eERR_ID_SUM_VH2:
    		addInfo[2] = GetGCellMaxVoltAPI();
    		addInfo[3] = GetGCellMaxVNumAPI();
    		addInfo[4] = GetGSampSumVoltAPI();
    		break;

    	/*电池总电压欠压2级(放电截止)*/
    	case eERR_ID_SUM_VL2:
    		addInfo[2] = GetGCellMinVoltAPI();
    		addInfo[3] = GetGCellMinVNumAPI();
    		addInfo[4] = GetGSampSumVoltAPI();
    		break;

    	/*充电温度高2级(充电截止)*/
    	case eERR_ID_CHG_TH2:
    		addInfo[2] = GetGCellMaxTempAPI();
    		addInfo[3] = GetGCellMaxTNumAPI();
    		addInfo[4] = GetGCellAvgTempAPI();
    		break;

		/*充电温度低2级(充电截止)*/
		case eERR_ID_CHG_TL2:
    		addInfo[2] = GetGCellMinTempAPI();
    		addInfo[3] = GetGCellMinTNumAPI();
    		addInfo[4] = GetGCellAvgTempAPI();
			break;

    	/*放电温度高2级(放电截止)*/
    	case eERR_ID_DHG_TH2:
    		addInfo[2] = GetGCellMaxTempAPI();
    		addInfo[3] = GetGCellMaxTNumAPI();
    		addInfo[4] = GetGCellAvgTempAPI();
        	break;

        /*放电温度低2级(放电截止)*/
    	case eERR_ID_DHG_TL2:
    		addInfo[2] = GetGCellMinTempAPI();
    		addInfo[3] = GetGCellMinTNumAPI();
    		addInfo[4] = GetGCellAvgTempAPI();
        	break;

        /*充电过流2级(限制充电)*/
    	case eERR_ID_CHG_CO2:
    		addInfo[2] = GetGSampSumVoltAPI();
    		addInfo[3] = GetGChgLimCurrAPI();
    		addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_ChgCH2Lim];
        	break;

        /*放电过流2级(限制放电)*/
    	case eERR_ID_DHG_CO2:
    		addInfo[2] = GetGSampSumVoltAPI();
    		addInfo[3] = GetGDhgLimCurrAPI();
    		addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_DhgCH2Lim];
        	break;

        /*外设温度高2级(充放电限制)*/
    	case eERR_ID_PER_TH2:
    		addInfo[2] = (u16)GetGEnvirTempAPI();
    		addInfo[3] = (u16)GetGShuntSampTempAPI();
    		if(GetGEnvirTempAPI() > GetGShuntSampTempAPI())
    		{
        		addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_Envir2Lim];
    		}
    		else
    		{
        		addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_Perip2Lim];
    		}
        	break;

        /*电池组绝缘2级(充放电限制)*/
    	case eERR_ID_LEAK2:
    		addInfo[2] = GetGPasInsuResistAPI();
    		addInfo[3] = GetGNegInsuResistAPI();
    		//addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_Leak2Lim];
    		addInfo[4] = GetGPgndNegVoltAPI();
        	break;

        /*单体电压一致性下降2级(预警)*/
    	case eERR_ID_CV_DIF2:
    		addInfo[2] = GetGCellMaxVoltAPI();
    		addInfo[3] = GetGCellMinVoltAPI();
    		//addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_VDiff2SubV];
    		addInfo[4] = GetGCellAvgVoltAPI();
        	break;

        /*单体温度温差过大2级(预警)*/
    	case eERR_ID_CT_DIF2:
    		addInfo[2] = GetGCellMaxTempAPI();
    		addInfo[3] = GetGCellMinTempAPI();
    		//addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_TDiff2SubT];
    		addInfo[4] = GetGCellAvgTempAPI();
        	break;

        /*充电容量SOC高2级(预警)*/
    	case eERR_ID_CAP_SH2:
    		addInfo[2] = GetGBattSocAPI();
    		addInfo[3] = GetGBattSoeAPI();
    		//addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_CapHig2Soc];
    		addInfo[4] = GetGCellMaxVoltAPI();
        	break;

        /*剩余容量SOC低2级(预警)*/
    	case eERR_ID_CAP_SL2:
    		addInfo[2] = GetGBattSocAPI();
    		addInfo[3] = GetGBattSoeAPI();
    		//addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_CapLow2Soc];
    		addInfo[4] = GetGCellMinVoltAPI();
        	break;

    	/*以下为电池1级告警ID*/
        /*单体电压高1级(充电保护)*/
    	case eERR_ID_CEL_VH1:
    		addInfo[2] = GetGCellMaxVoltAPI();
    		addInfo[3] = GetGCellMaxVNumAPI();
    		addInfo[4] = GetGCellAvgVoltAPI();
			break;

		/*单体电压低1级(放电保护)*/
    	case eERR_ID_CEL_VL1:
    		addInfo[2] = GetGCellMinVoltAPI();
    		addInfo[3] = GetGCellMinVNumAPI();
    		addInfo[4] = GetGCellAvgVoltAPI();
        	break;

        /*电池总电压过压1级(充电保护)*/
    	case eERR_ID_SUM_VH1:
    		addInfo[2] = GetGCellMaxVoltAPI();
    		addInfo[3] = GetGCellMaxVNumAPI();
    		addInfo[4] = GetGSampSumVoltAPI();
        	break;

        /*电池总电压欠压1级(放电保护)*/
    	case eERR_ID_SUM_VL1:
    		addInfo[2] = GetGCellMinVoltAPI();
    		addInfo[3] = GetGCellMinVNumAPI();
    		addInfo[4] = GetGSampSumVoltAPI();
        	break;

        /*充电温度高1级(充电保护)*/
    	case eERR_ID_CHG_TH1:
    		addInfo[2] = GetGCellMaxTempAPI();
    		addInfo[3] = GetGCellMaxTNumAPI();
    		addInfo[4] = GetGCellAvgTempAPI();
        	break;

        /*充电温度低1级(充电保护)*/
    	case eERR_ID_CHG_TL1:
    		addInfo[2] = GetGCellMinTempAPI();
    		addInfo[3] = GetGCellMinTNumAPI();
    		addInfo[4] = GetGCellAvgTempAPI();
        	break;

		/*放电温度高1级(放电保护)*/
		case eERR_ID_DHG_TH1:
    		addInfo[2] = GetGCellMaxTempAPI();
    		addInfo[3] = GetGCellMaxTNumAPI();
    		addInfo[4] = GetGCellAvgTempAPI();
			break;

        /*放电温度低1级(放电保护)*/
    	case eERR_ID_DHG_TL1:
    		addInfo[2] = GetGCellMinTempAPI();
    		addInfo[3] = GetGCellMinTNumAPI();
    		addInfo[4] = GetGCellAvgTempAPI();
        	break;

        /*充电过流1级(充电延时保护)*/
    	case eERR_ID_CHG_CO1:
    		addInfo[2] = (u16)GetGSampOutCurrAPI();
    		addInfo[3] = GetGChgLimCurrAPI();
    		addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_ChgCH1Lim];
        	break;

        /*放电过流1级(放电延时保护)*/
    	case eERR_ID_DHG_CO1:
    		addInfo[2] = (u16)GetGSampOutCurrAPI();
    		addInfo[3] = GetGDhgLimCurrAPI();
    		addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_DhgCH1Lim];
        	break;

        /*外设温度过高1级(充放电保护)*/
    	case eERR_ID_PER_TH1:
    		addInfo[2] = (u16)GetGEnvirTempAPI();
    		addInfo[3] = (u16)GetGShuntSampTempAPI();
    		if(GetGEnvirTempAPI() > GetGShuntSampTempAPI())
    		{
        		addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_Envir1Lim];
    		}
    		else
    		{
        		addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_Perip1Lim];
    		}
        	break;

        /*电池组绝缘1级(充放电保护)*/
    	case eERR_ID_LEAK1:
    		addInfo[2] = GetGPasInsuResistAPI();
    		addInfo[3] = GetGNegInsuResistAPI();
    		//addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_Leak1Lim];
    		addInfo[4] = GetGPgndNegVoltAPI();
        	break;

        /*单体电压一致性下降1级(充放电限制)*/
    	case eERR_ID_CV_DIF1:
    		addInfo[2] = GetGCellMaxVoltAPI();
    		addInfo[3] = GetGCellMinVoltAPI();
    		//addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_VDiff1SubV];
    		addInfo[4] = GetGCellAvgVoltAPI();
        	break;

        /*单体温度温差过大1级(充放电限制)*/
    	case eERR_ID_CT_DIF1:
    		addInfo[2] = GetGCellMaxTempAPI();
    		addInfo[3] = GetGCellMinTempAPI();
    		//addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_TDiff1SubT];
    		addInfo[4] = GetGCellAvgTempAPI();
        	break;

        /*充电容量SOC高1级(充电截止)*/
    	case eERR_ID_CAP_SH1:
    		addInfo[2] = GetGBattSocAPI();
    		addInfo[3] = GetGBattSoeAPI();
    		//addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_CapHig1Soc];
    		addInfo[4] = GetGCellMaxVoltAPI();
        	break;

        /*剩余容量SOC低1级(放电截止)*/
    	case eERR_ID_CAP_SL1:
    		addInfo[2] = GetGBattSocAPI();
    		addInfo[3] = GetGBattSoeAPI();
    		//addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_CapLow1Soc];
    		addInfo[4] = GetGCellMinVoltAPI();
    		break;

		/*以下为提示类告警ID*/
    	/*SOC显示异常(需要修正)*/
    	case eERR_ID_SOC_EXP:
    		addInfo[2] = GetGBattSocAPI();
    		addInfo[3] = GetGBattSoeAPI();
    		addInfo[4] = GetGRealSocMilliAPI();
    		break;

    	/*容量检测异常(需要校准)*/
    	case eERR_ID_CAP_EXP:
    		addInfo[2] = GetCapEnerDropExpAPI();
    		addInfo[3] = GetGroupTotalCapAPI() / 1000;
    		addInfo[4] = GetGroupTotalEnerAPI() / 100;
        	break;

        /*充电过流响应超时(电流超限值)*/
    	case eERR_ID_CHG_COT:
    		addInfo[2] = GetGSampSumVoltAPI();
    		addInfo[3] = GetGChgLimCurrAPI();
    		addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_ChgCH2Lim];
        	break;

        /*放电过流响应超时(电流超限值)*/
    	case eERR_ID_DHG_COT:
    		addInfo[2] = GetGSampSumVoltAPI();
    		addInfo[3] = GetGDhgLimCurrAPI();
    		addInfo[4] = gGBmuGenPara_102[eBmuGenPara102_DhgCH2Lim];
        	break;

        /*均衡板温过高(关闭均衡)*/
    	case eERR_ID_BAL_TH:
    		addInfo[2] = GetGBalanMaxTSlaveAPI();
    		addInfo[3] = (u16)GetGBalanMaxTempAPI();
    		//菊花链通信从控或自研CAN通信从控
    		#if((0 == BMU_BSU_COM_TYPE) || (0 == BMU_BSU_CAN_TYPE))
    		addInfo[4] = GetGroupVoltPasBalanReqState(addInfo[2] - 1);
    		if((GetGroupVoltPasBalanReqState(addInfo[2] - 1) & 0x0f0000) > 0)
    		{
    			//超过16串显示在第16串
    			BitSet(addInfo[4], 15);
    		}
            #endif
        	break;

        /*低压电源板温异常(提示)*/
    	case eERR_ID_DCV_TH:
    		addInfo[2] = (u16)GetGDcPwrSampTempAPI();
    		addInfo[3] = (u16)GetGEnvirTempAPI();
    		addInfo[4] = (u16)GetGShuntSampTempAPI();
        	break;

		/*SOS诊断异常(提示)*/
		case eERR_ID_SOS_EXP:
			addInfo[2] = DiagGetGroupSosState(0);
			addInfo[3] = DiagGetGroupSosResion(0);
			addInfo[4] = 0;
			break;

		/*电池箱风扇异常(提示)*/
		case eERR_ID_FAN_EXP:
			addInfo[2] = TacGetPwmFanErrFlagAPI();
			addInfo[3] = GetGPerImlu1SignInfoAPI(eIMLU1Info202_FanSign);
			addInfo[4] = DiagGetRcvPerOffCommuBitAPI();
			break;

		/*电池柜配电板外设(百叶风机空调)状态异常告警(提示)*/
		case eERR_ID_SHU_EXP:
			addInfo[2] = TacGetAuxComErrFlagAPI();
			if(1 == TacGetAuxDevAirTypeAPI())//同飞空调
			{
				addInfo[3] = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirAlarm) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirState);
			}
			else
			{
				addInfo[3] = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirAlarm) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState);
			}
			addInfo[4] = GetGPerImeu2SignInfoAPI(eIMEU2Info204_FanSpeed);
			break;

			/*电池柜配电板外设(温湿度采集器/气体检测/空调)通信中断告警(提示)*/
		case eERR_ID_THC_OFF:
			addInfo[2] = TacGetAuxDevErrFlagAPI();
			addInfo[3] = GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign);
			addInfo[4] = GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenCommu);
			break;

    	/*故障类型开始*/
		/*以下为从控告警ID(48-51对应[0-3])*/
        /*均衡温度采样异常*/
    	case eERR_ID_BAL_TEXP:
    		addInfo[2] = DiagGetBalTOffSlaveIdAPI();
    		addInfo[3] = DiagGetBalTOffCellBitAPI();
    		addInfo[4] = GetGBalanMaxTempAPI();
        	break;

        /*均衡控制异常*/
    	case eERR_ID_BAL_CEXP:
    		addInfo[2] = DiagGetBalErrSlaveIdAPI();
    		addInfo[3] = DiagGetBalErrCellBitAPI();
    		if((DiagGetBalErrCellBitAPI() & 0x0f0000) > 0)
    		{
    			//超过16串显示在第16串
    			BitSet(addInfo[3], 15);
    		}
    		addInfo[4] = GetGBalanMaxTempAPI();
        	break;

        /*水浸告警*/
    	case eERR_ID_FLOOD_ERR:
    		addInfo[2] = GetGPerImeu2SignInfoAPI(eIMEU2Info204_FloodHz);
    		addInfo[3] = DiagGetRcvPerOffCommuBitAPI();
    		addInfo[4] = 0;
        	break;

        /*电池气体检测浓度异常*/
    	case eERR_ID_CGAS_EXP:
    		addInfo[2] = GetGCellMinVoltAPI();
    		addInfo[3] = GetGCellMaxTempAPI();
    		if(1 == BitGet(DiagGetPerGasErrFlagAPI(), 2))//气体2告警
    		{
    			addInfo[4] = GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2);
    		}
    		else
    		{
    			addInfo[4] = GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1);
    		}
        	break;

    	/*以下为从控内部故障ID(52-55对应[4-7])*/
        /*单体采样电路异常*/
    	case eERR_ID_SAMP_EXP:
    		addInfo[2] = DiagGetCellSErrSlaveIdAPI();
    		addInfo[3] = DiagGetCellSampErrFlagAPI();
    		addInfo[4] = GetGCellSumVoltAPI();
        	break;

        /*单体电压采样断线*/
    	case eERR_ID_SAMP_VOFF:
    		addInfo[2] = DiagGetCellVOffSlaveIdAPI();
    		addInfo[3] = DiagGetCellVOffCellBitAPI();
    		if((DiagGetCellVOffCellBitAPI() & 0x0f0000) > 0)
    		{
    			//超过16串显示在第16串
    			BitSet(addInfo[3], 15);
    		}
    		addInfo[4] = GetGCellSumVoltAPI();
        	break;

        /*单体温度采样断线*/
    	case eERR_ID_SAMP_TOFF:
    		addInfo[2] = DiagGetCellTOffSlaveIdAPI();
    		addInfo[3] = DiagGetCellTOffCellBitAPI();
    		addInfo[4] = GetGCellAvgTempAPI();
        	break;

        /*单体信息缺失*/
    	case eERR_ID_CELL_MISS:
    		addInfo[2] = DiagGetRcvCellInfoSlaveIdAPI();
    		addInfo[3] = DiagGetRcvSlaveMsgSlaveIdAPI();
    		addInfo[4] = GetGCellSumVoltAPI();
        	break;

		/*以下为主从通信故障ID(56-59对应[8-11])*/
       	/*内部主从级联异常(与下级从控连接)*/
        case eERR_ID_INT_LOFF:
    		addInfo[2] = GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmAdcV);
    		addInfo[3] = GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmCycle);
    		addInfo[4] = GetGCellSumVoltAPI();
        	break;

        /*内部主从CAN断线故障(与下级从控通讯)*/
    	case eERR_ID_INT_COFF:
    		addInfo[2] = DiagGetRcvCellInfoSlaveIdAPI();
    		addInfo[3] = DiagGetRcvSlaveMsgSlaveIdAPI();
    		addInfo[4] = GetGCellSumVoltAPI();
        	break;

        /*外设主从CAN断线故障(与内部子系统通讯)*/
    	case eERR_ID_PER_COFF:
    		addInfo[2] = DiagGetRcvPerOffCommuBitAPI();
    		addInfo[3] = GetGChgLimCurrAPI();
    		addInfo[4] = GetGDhgLimCurrAPI();
        	break;

       	/*外部通讯断线(与上级或充放电设备CAN/RS485通讯)*/
    	case eERR_ID_EXT_COFF:
    		addInfo[2] = GetGSampSumVoltAPI();
    		addInfo[3] = GetGChgLimCurrAPI();
    		addInfo[4] = GetGDhgLimCurrAPI();
        	break;

		/*以下为主控软件报警ID(60-63对应[12-15])*/
       	/*主控设备地址冲突*/
    	case eERR_ID_MECU_REP:
    		addInfo[2] = gGBmuGenPara_102[eBmuGenPara102_BmuDev];
    		addInfo[3] = GetGChgLimCurrAPI();
    		addInfo[4] = GetGDhgLimCurrAPI();
        	break;

        /*故障位17预留*/
    	case eERR_ID_ERR17_KEPP:
    		addInfo[2] = 0;
    		addInfo[3] = 0;
    		addInfo[4] = 0;
        	break;

        /*电池组软件急停*/
    	case eERR_ID_SW_EMERG:
    		addInfo[2] = TacGetGEmergStopFlagAPI();
    		addInfo[3] = GetGSampSumVoltAPI();
    		addInfo[4] = GetGSampPreVoltAPI();
        	break;

        /*电池组自行保护*/
    	case eERR_ID_SELF_PRO:
    		addInfo[2] = TacGetGHappenProFlagAPI();
    		addInfo[3] = GetGSampSumVoltAPI();
    		addInfo[4] = GetGSampPreVoltAPI();
        	break;

		/*以下为主控内部故障ID(64-79对应[16-31])*/
        /*主控内部RTC异常*/
        case eERR_ID_MRTC_EXP:
    		addInfo[2] = (u16)(BSPGetOSMillSecTimeAPI() / 1000);
    		addInfo[3] = GetGSampSumVoltAPI();
    		addInfo[4] = GetGSampPreVoltAPI();
        	break;

        /*主控EEPROM存储异常*/
    	case eERR_ID_MEEP_EXP:
    		addInfo[2] = (u16)(BSPGetOSMillSecTimeAPI() / 1000);
    		addInfo[3] = GetGSampSumVoltAPI();
    		addInfo[4] = GetGSampPreVoltAPI();
        	break;

       	/*主控外部Flash异常*/
    	case eERR_ID_MFLA_EXP:
    		addInfo[2] = (u16)(BSPGetOSMillSecTimeAPI() / 1000);
    		addInfo[3] = GetGSampSumVoltAPI();
    		addInfo[4] = GetGSampPreVoltAPI();
        	break;

        /*电池组电流采样电路异常*/
    	case eERR_ID_CSAMP_ERR:
    		addInfo[2] = SampGetCurrSampExpFlagAPI(0xff);
    		addInfo[3] = GetGSampSumVoltAPI();
    		addInfo[4] = GetGSampPreVoltAPI();
        	break;

       	/*输出端高压采样异常*/
    	case eERR_ID_HSAMP_EXP:
    		addInfo[2] = ((u16)DiagGetHigVoltExpFlagAPI() << 8) | SampGetHigVSampExpFlagAPI();
    		addInfo[3] = GetGSampSumVoltAPI();

    		//采样通道异常或预充电压值异常
    		if((SampGetHigVSampExpFlagAPI() > 0) || (1 == BitGet(DiagGetHigVoltExpFlagAPI(), 1)))
    		{
    			//显示预充电压
    			addInfo[4] = GetGSampPreVoltAPI();
    		}
    		//总压采样值异常
    		else
    		{
    			//显示累计总压
    			addInfo[4] = GetGCellSumVoltAPI();
    		}
        	break;

        /*硬件板低压侧采样异常*/
    	case eERR_ID_LSAMP_EXP:
    		addInfo[2] = SampGetLowVSampExpFlagAPI();
    		addInfo[3] = GetGHallSignVoltAPI();
    		addInfo[4] = GetGEnvirTempAPI();
        	break;

        /*外围温度采样异常*/
    	case eERR_ID_TSAMP_EXP:
    		addInfo[2] = SampGetLowVSampExpFlagAPI();
    		addInfo[3] = GetGShuntSampTempAPI();
    		addInfo[4] = GetGEnvirTempAPI();
        	break;

        /*绝缘采样异常*/
    	case eERR_ID_LEAK_EXP:
    		addInfo[2] = GetInsuErrStateFlagAPI();
    		addInfo[3] = GetGPasInsuResistAPI();
    		addInfo[4] = GetGNegInsuResistAPI();
    		break;

    	/*KM1+回检异常(主正接触器HS回检异常)*/
    	case eERR_ID_K0RE_EXP:
    		addInfo[2] = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Mai);
    		addInfo[3] = GetGInPortStateAPI();
    		addInfo[4] = GetGOutPortStateAPI();
        	break;

        /*KM2回检异常(预充接触器HS回检异常)*/
    	case eERR_ID_K1RE_EXP:
    		addInfo[2] = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Pre);
    		addInfo[3] = GetGInPortStateAPI();
    		addInfo[4] = GetGOutPortStateAPI();
        	break;

        /*KM3+回检异常(负极接触器HS回检异常)*/
    	case eERR_ID_K2RE_EXP:
    		addInfo[2] = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Neg);
    		addInfo[3] = GetGInPortStateAPI();
    		addInfo[4] = GetGOutPortStateAPI();
        	break;

        /*KM4回检异常(电源接触器HS回检异常)*/
    	case eERR_ID_K3RE_EXP:
    		addInfo[2] = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Pwr);
    		addInfo[3] = GetGInPortStateAPI();
    		addInfo[4] = GetGOutPortStateAPI();
        	break;

        /*KM5-8回检异常(4个附加接触器HS回检异常)*/
    	case eERR_ID_K7RE_EXP:
    		addInfo[2] = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Hot);
    		addInfo[3] = GetGInPortStateAPI();
    		addInfo[4] = GetGOutPortStateAPI();
        	break;

        /*KM9回检异常(风扇接触器HS回检异常)*/
    	case eERR_ID_K8RE_EXP:
    		addInfo[2] = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Fan);
    		addInfo[3] = GetGInPortStateAPI();
    		addInfo[4] = GetGOutPortStateAPI();
        	break;

        /*KM1-回检异常(主正接触器LS回检异常)*/
    	case eERR_ID_L0RE_EXP:
    		addInfo[2] = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Mai);
    		addInfo[3] = GetGInPortStateAPI();
    		addInfo[4] = GetGOutPortStateAPI();
        	break;

        /*KM3-回检异常(负极接触器LS回检异常)*/
    	case eERR_ID_L2RE_EXP:
    		addInfo[2] = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Neg);
    		addInfo[3] = GetGInPortStateAPI();
    		addInfo[4] = GetGOutPortStateAPI();
        	break;

		//以下为主控外围故障ID(80-95对应[32-47])
    	/*主正接触器状态异常(可能主接触器烧结或不能吸合)*/
    	case eERR_ID_K0MAI_EXP:
    		addInfo[2] = ((u16)TacGetGSwhCtrlFailFlagAPI() << 8) | TacGetGroupRunSwhStateAPI();
    		addInfo[3] = ((u16)DiagGetSwhStateVExpFlagAPI() << 8) | GetSwhAdhesErrFlagAPI();

    		//诊断粘连故障
    		if(GetSwhAdhesErrFlagAPI() > 0)
    		{
    			addInfo[4] = (u16)GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link1);//记录主正诊断粘连值
    		}
    		//检测状态异常
    		else
    		{
    			addInfo[4] = GetGSampPreVoltAPI();//记录外端电压采样值
    		}
        	break;

        /*预充接触器异常(可能预充电阻断开或预充接触器不能吸合)*/
    	case eERR_ID_K1PRE_EXP:
    		addInfo[2] = ((u16)TacGetGSwhCtrlFailFlagAPI() << 8) | TacGetGroupRunSwhStateAPI();
    		addInfo[3] = ((u16)DiagGetSwhStateVExpFlagAPI() << 8) | GetSwhAdhesErrFlagAPI();

    		//诊断粘连故障
    		if(GetSwhAdhesErrFlagAPI() > 0)
    		{
    			addInfo[4] = (u16)GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link1);//记录主正诊断粘连值
    		}
    		//检测状态异常
    		else
    		{
    			addInfo[4] = GetGSampPreVoltAPI();//记录外端电压采样值
    		}
        	break;

        /*负极接触器异常(可能负极接触器烧结或不能吸合)*/
    	case eERR_ID_K2NEG_EXP:
    		addInfo[2] = ((u16)TacGetGSwhCtrlFailFlagAPI() << 8) | TacGetGroupRunSwhStateAPI();
    		addInfo[3] = ((u16)DiagGetSwhStateVExpFlagAPI() << 8) | GetSwhAdhesErrFlagAPI();

    		//诊断粘连故障
    		if(GetSwhAdhesErrFlagAPI() > 0)
    		{
    			addInfo[4] = (u16)GetGDcSideSampRealInfoAPI(eReferSampInfo200_Link2);//记录主负诊断粘连值
    		}
    		//检测状态异常
    		else
    		{
    			addInfo[4] = GetGSampPreVoltAPI();//记录外端电压采样值
    		}
        	break;

        /*消防告警异常*/
    	case eERR_ID_FIRE_EXP:
    		addInfo[2] = GetGEnvirTempAPI();
    		addInfo[3] = GetGPerInsidMaxTempAPI();
    		addInfo[4] = GetGPerEnvirMaxTempAPI();
        	break;

        /*电池组熔断器熔断*/
    	case eERR_ID_FUSE_OFF:
    		addInfo[2] = GetGSampSumVoltAPI();
    		addInfo[3] = GetGInPortStateAPI();
    		addInfo[4] = GetGOutPortStateAPI();
        	break;

        /*电池组烟雾报警*/
    	case eERR_ID_BATT_SMOG:
    		addInfo[2] = GetGSampPreVoltAPI();
    		addInfo[3] = GetGInPortStateAPI();
    		addInfo[4] = GetGOutPortStateAPI();
        	break;

        /*电池组启动预充未满(可能带载启动)*/
    	case eERR_ID_STARTUP_VL:
    		addInfo[2] = ((u16)TacGetGroupRunSwhStateAPI() << 8) | TacGetGStartFailFlagAPI();
    		addInfo[3] = GetGSampSumVoltAPI();
    		addInfo[4] = GetGSampPreVoltAPI();
        	break;

       	/*电池组启动预充过温(预充接触器过热)*/
    	case eERR_ID_STARTUP_TH:
    		addInfo[2] = ((u16)TacGetGroupRunSwhStateAPI() << 8) | TacGetGStartFailFlagAPI();
    		addInfo[3] = (u16)GetGEnvirTempAPI();
    		addInfo[4] = GetGSampPreVoltAPI();
        	break;

        /*电池组启动失败(系统故障)*/
    	case eERR_ID_STARTUP_ERR:
    		addInfo[2] = ((u16)TacGetGroupRunSwhStateAPI() << 8) | TacGetGStartFailFlagAPI();
    		addInfo[3] = GetGSampSumVoltAPI();
    		addInfo[4] = GetGSampPreVoltAPI();
        	break;

        /*电池组停机带载超时*/
    	case eERR_ID_STOP_CH:
    		addInfo[2] = ((u16)TacGetGroupRunSwhStateAPI() << 8) | TacGetGStopFailFlagAPI();
    		addInfo[3] = GetGChgLimCurrAPI();
    		addInfo[4] = GetGDhgLimCurrAPI();
        	break;

        /*电池组停机失败(系统故障)*/
    	case eERR_ID_STOP_ERR:
    		addInfo[2] = ((u16)TacGetGroupRunSwhStateAPI() << 8) | TacGetGStopFailFlagAPI();
    		addInfo[3] = GetGSampSumVoltAPI();
    		addInfo[4] = GetGSampPreVoltAPI();
        	break;

        /*电池组按钮紧急停机*/
    	case eERR_ID_IO_EMERG:
    		addInfo[2] = TacGetGEmergStopFlagAPI();
    		addInfo[3] = GetGSampSumVoltAPI();
    		addInfo[4] = GetGSampPreVoltAPI();
        	break;

        /*断路器未合闸*/
    	case eERR_ID_ISO_OFF:
    		addInfo[2] = GetGSampPreVoltAPI();
    		addInfo[3] = GetGInPortStateAPI();
    		addInfo[4] = GetGOutPortStateAPI();
        	break;

        /*电池组内外压差过大*/
    	case eERR_ID_DC_HV_DIFF:
    		addInfo[2] = GetGSampSumVoltAPI();
    		addInfo[3] = GetGSampPreVoltAPI();
    		addInfo[4] = GetGPgndNegVoltAPI();
        	break;

        /*电池组带载闭合*/
    	case eERR_ID_SWHON_CH:
    		addInfo[2] = GetGSampPreVoltAPI();
    		addInfo[3] = GetGChgLimCurrAPI();
    		addInfo[4] = GetGDhgLimCurrAPI();
        	break;

        /*电池组带载断开*/
    	case eERR_ID_SWHOFF_CH:
    		addInfo[2] = GetGSampPreVoltAPI();
    		addInfo[3] = GetGChgLimCurrAPI();
    		addInfo[4] = GetGDhgLimCurrAPI();
        	break;

    	default:
    		return(FALSE);
    }

    return(TRUE);
}

/*=================================================================================================
** @Name      : static u8 DiagBuildSelfErrEvnAddInfo(e_ERR_ID id, u16 *addInfo)
** @Input     : code:故障码 addInfo:构建的附加信息数组(长度必须是5)
** @Output    : 构建结果:TRUE成功 FALSE失败
** @Function  : 构建用户自定义故障记录附加信息
** @The notes :
**===============================================================================================*/
static u8 DiagBuildSelfErrRcdAddInfo(u8 code, u16 *addInfo)
{
    //当前保护状态和工作状态
    addInfo[0] = ((u16)GetGProtectStateAPI() << 8) | (GetGWorkStateAPI() & 0x00ff);

    //当前输出电流
    addInfo[1] = (u16)GetGSampOutCurrAPI();

    /*根据故障码选择相应自定义附加信息*/
    switch(code)
    {
    	/*RTOS异常软件复位*/
    	case eUDef_ErrRcd_OSReset:
    		addInfo[2] = GetSysSecondTimeAPI() / 3600;
    		addInfo[3] = 0;
    		addInfo[4] = 0;
    		break;

    	/*单体电压采样值无效*/
    	case eUDef_ErrRcd_VSampErr:
		/*单体温度采样值无效*/
		case eUDef_ErrRcd_TSampErr:
    		addInfo[2] = DiagGetCellSErrSlaveIdAPI();
    		addInfo[3] = DiagGetCellSampErrFlagAPI();
    		if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmAdcV) < 100)
    		{
    			BitSet(addInfo[4], 0);
    		}
    		if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmCycle) < 1000)
    		{
    			BitSet(addInfo[4], 1);
    		}
    		break;

		/*单体电压采样值异常*/
		case eUDef_ErrRcd_CellVErr:
			addInfo[2] = DiagGetCellVOffSlaveIdAPI();
			addInfo[3] = DiagGetCellVOffCellBitAPI();
    		/*if((DiagGetCellVOffCellBitAPI() & 0x0f0000) > 0)
    		{
    			//超过16串显示在第16串
    			BitSet(addInfo[3], 15);
    		}
			if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmAdcV) < 100)
			{
				BitSet(addInfo[4], 0);
			}
			if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmCycle) < 1000)
			{
				BitSet(addInfo[4], 1);
			}*/
			break;

		/*单体温度采样值异常*/
		case eUDef_ErrRcd_CellTErr:
			addInfo[2] = DiagGetCellTOffSlaveIdAPI();
			addInfo[3] = DiagGetCellTOffCellBitAPI();
			/*if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmAdcV) < 100)
			{
				BitSet(addInfo[4], 0);
			}
			if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmCycle) < 1000)
			{
				BitSet(addInfo[4], 1);
			}*/
			break;

		/*未接收全部从控信息*/
		case eUDef_ErrRcd_SlaveOff:
    		addInfo[2] = DiagGetRcvCellInfoSlaveIdAPI();
    		addInfo[3] = DiagGetRcvSlaveMsgSlaveIdAPI();
			if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmAdcV) < 100)
			{
				BitSet(addInfo[4], 0);
			}
			if(GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmCycle) < 1000)
			{
				BitSet(addInfo[4], 1);
			}
			break;

		/*霍尔电流采样异常*/
		case eUDef_ErrRcd_HallCErr:
			addInfo[2] = SampGetCurrSampExpFlagAPI(0xff);
			addInfo[3] = gGHardPara_104[eGHardPara104_CSampChan];
			//addInfo[4] = GetGRealCurr10mAAPI(eCADC_Hall) / 10;
			break;

		/*分流器电流采样异常*/
		case eUDef_ErrRcd_ShunCErr:
			addInfo[2] = SampGetCurrSampExpFlagAPI(0xff);
			addInfo[3] = gGHardPara_104[eGHardPara104_CSampChan];
			//addInfo[4] = GetGRealCurr10mAAPI(eCAFE_Shunt) / 10;
			break;

		/*俩通道电流采样差异过大*/
		case eUDef_ErrRcd_TWOCDiff:
			addInfo[2] = SampGetCurrSampExpFlagAPI(0xff);
			addInfo[3] = GetGRealCurr10mAAPI(eCADC_Hall) / 10;
			addInfo[4] = GetGRealCurr10mAAPI(eCAFE_Shunt) / 10;
			break;

		/*辅助MCU异常*/
		case eUDef_ErrRcd_SmcuErr:
			addInfo[2] = GetGOutPortStateAPI();
			addInfo[3] = GetGInPortStateAPI();
			//addInfo[4] = GetGSMcuErrFlagAPI();
			break;

		/*烟感信号异常*/
		case eUDef_ErrRcd_SmgWarn:
			addInfo[2] = GetGOutPortStateAPI();
			addInfo[3] = GetGInPortStateAPI();
			addInfo[4] = TacGetGSmogAlarmFlagAPI();
			break;

		/*IMLU1通信异常*/
		case eUDef_ErrRcd_LuComErr:
			addInfo[2] = DiagGetRcvPerOffCommuBitAPI();
			addInfo[3] = ((u16)GetGPerInsidMaxTempAPI() << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT);
			addInfo[4] = (GetGCellMaxTempAPI() << 8) | GetGCellMinTempAPI();
			break;

		/*IMEU2通信异常*/
		case eUDef_ErrRcd_EuComErr:
			addInfo[2] = DiagGetRcvPerOffCommuBitAPI();
			addInfo[3] = ((u16)GetGPerInsidMaxTempAPI() << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT);
			addInfo[4] = ((u16)GetGPerInsidMaxHumAPI() << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH);
			break;

		/*空调通信异常*/
		case eUDef_ErrRcd_ArComErr:
			if(1 == TacGetAuxDevAirTypeAPI())//同飞空调
			{
				addInfo[2] = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirAlarm) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirState);
			}
			else
			{
				addInfo[2] = ((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirAlarm) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState);
			}
			addInfo[3] = ((u16)GetGPerInsidMaxTempAPI() << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT);
			addInfo[4] = ((u16)GetGPerInsidMaxHumAPI() << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH);
			break;

		/*温湿度传感器通信异常*/
		case eUDef_ErrRcd_THComErr:
			addInfo[2] = (GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign) >> eEU204DI_THSen1);
			addInfo[3] = ((u16)GetGPerInsidMaxTempAPI() << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT);
			addInfo[4] = ((u16)GetGPerInsidMaxHumAPI() << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH);
			break;

		/*气体传感器通信异常*/
		case eUDef_ErrRcd_GsComErr:
			addInfo[2] = GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenCommu);
			addInfo[3] = GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1);
			addInfo[4] = GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2);
			break;

		/*单体电压超过极限值*/
		case eUDef_ErrRcd_CellVExt:
			addInfo[2] = GetGCellMaxVNumAPI();
			addInfo[3] = GetGCellMaxVoltAPI();
			addInfo[4] = GetGCellMinVoltAPI();
			break;

		/*单体温度超过极限值*/
		case eUDef_ErrRcd_CellTExt:
			addInfo[2] = GetGCellMaxTNumAPI();
			addInfo[3] = GetGCellMaxTempAPI();
			addInfo[4] = GetGCellMinTempAPI();
			break;

		/*均衡板温超过极限值*/
		case eUDef_ErrRcd_BalaTExt:
    		addInfo[2] = GetGBalanMaxTSlaveAPI();
    		addInfo[3] = (u16)GetGBalanMaxTempAPI();
    		//菊花链通信从控或自研CAN通信从控
    		#if((0 == BMU_BSU_COM_TYPE) || (0 == BMU_BSU_CAN_TYPE))
    		addInfo[4] = GetGroupVoltPasBalanReqState(addInfo[2] - 1);
    		if((GetGroupVoltPasBalanReqState(addInfo[2] - 1) & 0x0f0000) > 0)
    		{
    			//超过16串显示在第16串
    			BitSet(addInfo[4], 15);
    		}
            #endif
			break;

		/*电源板温超过极限值*/
		case eUDef_ErrRcd_DCBTExt:
    		addInfo[2] = (u16)GetGDcPwrSampTempAPI();
    		addInfo[3] = (u16)GetGEnvirTempAPI();
    		addInfo[4] = GetGSbcAux5VVoltAPI();
			break;

		/*输出电流超过极限值*/
		case eUDef_ErrRcd_CurrExt:
			addInfo[2] = GetGSampSumVoltAPI();
			addInfo[3] = GetGSampPreVoltAPI();
			//充电记录最大电压值
			if(GetGSampOutCurrAPI() < 0)
			{
				addInfo[4] = GetGCellMaxVoltAPI();
			}
			else
			{
				addInfo[4] = GetGCellMinVoltAPI();
			}
			break;

		/*单体电压压降过快*/
		case eUDef_ErrRcd_CellVDn:
			addInfo[2] = GetGCellMinVNumAPI();
			addInfo[3] = GetGCellMinVoltAPI();
			//addInfo[4] = subValue;//下降值
			break;

		/*单体温度温升过快*/
		case eUDef_ErrRcd_CellTUp:
			addInfo[2] = GetGCellMaxTNumAPI();
			addInfo[3] = GetGCellMaxTempAPI();
			//addInfo[4] = subValue;//上升值
			break;

		/*电池组待机电流异常*/
		case eUDef_ErrRcd_CurrIdle:
			addInfo[2] = GetGChgLimCurrAPI();
			addInfo[3] = GetGDhgLimCurrAPI();
			addInfo[4] = GetGCellMaxVoltAPI();
			break;

		/*同一单体充放电保护*/
		case eUDef_ErrRcd_CellVPro:
			//充电截止
			if(ePRO_FULL == GetGProtectStateAPI())
			{
				addInfo[2] = GetGCellMaxVNumAPI();
				addInfo[3] = GetGCellMaxVoltAPI();
			}
			else
			{
				addInfo[2] = GetGCellMinVNumAPI();
				addInfo[3] = GetGCellMinVoltAPI();
			}
			addInfo[4] = GetGCellAvgVoltAPI();
			break;

		/*同一单体多次温度保护*/
		case eUDef_ErrRcd_CellTPro:
			addInfo[2] = GetGCellMaxTNumAPI();
			addInfo[3] = GetGCellMaxTempAPI();
			addInfo[4] = GetGCellAvgTempAPI();
			break;

		/*总容量衰减严重异常*/
		case eUDef_ErrRcd_CapDrop:
			addInfo[2] = GetGroupTotalCapAPI() / 1000;
			addInfo[3] = GetGroupStandCapAPI() / 1000;
			addInfo[4] = GetGChgCycleTimesAPI();
			break;

		/*充放深度长期不足*/
		case eUDef_ErrRcd_DodLack:
			addInfo[2] = GetGStatistNowDodAPI();
			addInfo[3] = GetGStatistMaxDodAPI();
			addInfo[4] = GetGChgCycleTimesAPI();
			break;

		/*充放电效率异常*/
		case eUDef_ErrRcd_ChgEffe:
			//addInfo[2] = GetGHisChgEnerAPI();
			//addInfo[3] = GetGHisDhgEnerAPI();
			addInfo[4] = GetGStatistNowDodAPI();
			break;

		/*SOS模型推断异常*/
		case eUDef_ErrRcd_SosMode:
			//addInfo[2] = DiagGetGroupSosState(0);
			//addInfo[3] = DiagGetGroupSosState(1);
			addInfo[4] = DiagGetGroupSosResion(1);
			break;

		/*当前SOS异常*/
		case eUDef_ErrRcd_ReSosErr:
			//addInfo[2] = DiagGetGroupSosState(1);
			//addInfo[3] = DiagGetGroupSosState(0);
			addInfo[4] = DiagGetGroupSosResion(0);
			break;

		/*预测SOS异常*/
		case eUDef_ErrRcd_PrSosErr:
			//addInfo[2] = DiagGetGroupSosState(0);
			//addInfo[3] = DiagGetGroupSosState(1);
			addInfo[4] = DiagGetGroupSosResion(1);
			break;

    	default:
    		return(FALSE);
    }

    return(TRUE);
}
