/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : HigAFESamp.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 高压采样模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPUserAPI.h"
#include "osif.h"
#include "stdbool.h"
#include "DEVGPIO.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "LTC2949.h"
#include "CurrSample.h"
#include "DiagInterface.h"
#include "InsuCheck.h"
#include "TacUser.h"
#include "HigAFESamp.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u8 sHigAFERelyState = 0;						/*高压信息采样外围继电器控制状态(按bit位表示:bit0:总压  bit1:Link-RLY1 bit2:Link+RLY2 bit3:绝缘)[0断开 1闭合]*/
static u8 sHigAFEErrFlag = 0;						/*高压信息采样模拟前端异常标志 按位表示(每bit为1个故障:bit0:芯片初始化异常 bit1:芯片未就绪 bit2:读取总压值异常)[0正常 1故障]*/

static u8 sOutHVSampFinState = 0;					/*外端高压(负载或预充电压)采样完成状态  [0:未采样 1:采样完成]*/
static u8 sDelySampWaitState = 0;					/*需延时等待的采样等待状态  [0:采样完成 1:正在负载电压采样等待 2:正在Link电压采样等待  3:正在绝缘电压采样等待]*/
static u8 sPreVSampChannel = 0;						/*外端预充电压采样通道  [0:负载电压V7通道 1:Link+电压通道]*/
static u8 sLoadVSampState = 0;						/*外端负载电压采样状态  [0:控制继电器未闭合  1:继电器闭合可以读取通道电压值]*/
static u32 sLinkVRlyCtrlT = 0;						/*Link电压采样继电器控制时间  1ms*/
static u32 sIsoVRlyCtrlT = 0;						/*绝缘电压采样继电器控制时间  1ms*/

//static s16 sSampFilterCurr[CURR_FILT_LEN] = {0};	/*采样电流值滤波数组*/
static s16 fastData2949[LTC2949_RDFASTDATA_LENGTH] = {0};/*LTC2949采样值*/

static const u32 sShuntNTCTempTabR[] =              /*分流器NTC温度阻值取值表[SHUNT_T_TAB_LEN],[-41~181]*/
{
	198000,
	1206237, 194750, 183980, 173880, 164402, 155504, 147148, 139297, 131916, 124975,
	118445, 112301, 106515, 101065, 95929, 91087, 86520, 82211, 78144, 74304,
	70676, 67264, 64037, 60985, 58095, 55360, 52770, 50316, 47991, 45786,
	43696, 41706, 39818, 38027, 36327, 34713, 33180, 31723, 30339, 29024,
	27773, 26576, 25437, 24355, 23325, 22345, 21412, 20524, 19678, 18872,
	18103, 17371, 16672, 16006, 15370, 14763, 14184, 13651, 13102, 12597,
	12144, 11653, 11212, 10790, 10386, 10000, 9632, 9280, 8943, 8619,
	8310, 8012, 7727, 7454, 7192, 6940, 6699, 6467, 6244, 6030,
	5825, 5628, 5438, 5256, 5080, 4912, 4750, 4594, 4444, 4299,
	4160, 4026, 3897, 3773, 3654, 3538, 3427, 3320, 3217, 3118,
	3022, 2930, 2840, 2754, 2671, 2591, 2514, 2439, 2367, 2297,
	2230, 2164, 2102, 2041, 1982, 1925, 1870, 1817, 1766, 1716,
	1668, 1622, 1577, 1533, 1491, 1450, 1410, 1371, 1333, 1296,
	1261, 1226, 1193, 1161, 1129, 1099, 1069, 1041, 1013, 986,
	960, 935, 910, 886, 863, 841, 819, 799, 780, 760,
	742, 724, 707, 690, 673, 656, 638, 622, 605, 590,
	574, 559, 545, 531, 517, 504, 491, 480, 468, 457,
	446, 435, 425, 415, 405, 396, 387, 378, 369, 360,
	352, 344, 336, 329, 321, 314, 307, 300, 293, 287,
	280, 274, 268, 262, 257, 251, 246, 241, 235, 230,
	225, 221, 216, 212, 207, 203, 199, 194, 190, 187,
	183, 179, 175, 172, 168, 165, 162, 158, 155, 152,
	149, 146
};

/*分流器温度补偿电压使能*/
#if(1 == SHUNT_V_COMP_EN)
static const s8 sShuntCompTempTab[] =               /*分流器温度补偿温度取值表[SHUNT_V_TAB_LEN],[-30~160]*/
{
	-30, -20, -10, 0, 10, 20, 30, 40, 50, 60,
	70, 80, 90, 100, 110, 120, 130, 140, 150, 160
};

static const s16 sShuntCompVoltTab[] =              /*分流器温度补偿电压取值表[SHUNT_V_TAB_LEN],0.01mV*/
{
	-1, -1, -1, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 2, 2, 2, 2, 3
};
#endif

extern u16 gGBmuHigLevPara_103[];       			/*主控高级参数值*/
extern u8 gGHardPara_104[]; 		                /*主控硬件参数配置信息*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u8 WakeUpReportStatus(void)
** @Input     : void
** @Output    : void
** @Function  : 获取LTC2949是否处于唤醒和正确配置状态
** @The notes :
**===========================================================================================*/
static u8 WakeUpReportStatus(void);

/*=============================================================================================
** @Name      : static float LinkNegVoltFilter(float volt)
** @Input     : volt:当次采样值(1.0V)
** @Output    : 滤波后采样值(1.0V,65535.0表示异常需要重新采样)
** @Function  : Link-电压采样值滤波
** @The notes : 负极接触器闭合或断开3.0V±10mV内不滤波
**===========================================================================================*/
static float LinkNegVoltFilter(float volt);

/*=============================================================================================
** @Name      : static void ClrLtc2949SampInfo(void)
** @Input     : void
** @Output    : void
** @Function  : 清除LTC2949高压采样信息
** @The notes : LTC采样失效时调用
**===========================================================================================*/
static void ClrLtc2949SampInfo(void);

/*=============================================================================================
** @Name      : static u8 SelectHVNextSampStep(u8 nowStep)
** @Input     : nowStep:当前步骤
** @Output    : 下一步执行步骤
** @Function  : 选择高压采样下一步执行步骤
** @The notes : 判断是否返回电流采样
**===========================================================================================*/
static u8 SelectHVNextSampStep(u8 nowStep);

/*=============================================================================================
** @Name      : static u8 SelectHVLoadVSampStep(void)
** @Input     : void
** @Output    : 负载电压采样执行步骤
** @Function  : 选择高压采样负载电压采样执行步骤
** @The notes : 未达到采样间隔时间则下一步
**===========================================================================================*/
static u8 SelectHVLoadVSampStep(void);

/*=============================================================================================
** @Name      : static u8 SelectHVLinkVSampStep(void)
** @Input     : void
** @Output    : 粘连检测Link电压采样执行步骤
** @Function  : 选择高压采样粘连检测Link电压采样执行步骤
** @The notes : 未达到采样间隔时间则下一步
**===========================================================================================*/
static u8 SelectHVLinkVSampStep(void);

/*=============================================================================================
** @Name      : static u8 SelectHVIsoVSampStep(void)
** @Input     : void
** @Output    : 绝缘检测采样执行步骤
** @Function  : 选择高压采样绝缘检测采样执行步骤
** @The notes : 无绝缘检测采样则获取下一步采样
**===========================================================================================*/
static u8 SelectHVIsoVSampStep(void);

/*=============================================================================================
** @Name      : static u8 SelectHVPgndVSampStep(void)
** @Input     : void
** @Output    : 负母线电压采样执行步骤
** @Function  : 选择高压采样负母线电压采样执行步骤
** @The notes : 无负母线电压采样则获取下一步采样
**===========================================================================================*/
static u8 SelectHVPgndVSampStep(void);

/*=============================================================================================
** @Name      : static u8 SelectHVShuTSampStep(void)
** @Input     : void
** @Output    : 分流器温度采样执行步骤
** @Function  : 选择高压采样分流器温度采样执行步骤
** @The notes : 无分流器温度采样则获取下一步采样
**===========================================================================================*/
static u8 SelectHVShuTSampStep(void);

/*=============================================================================================
** @Name      : static void SampCtrlHVLinkVRelyState(u8 state)
** @Input     : 控制状态 0:全断开 1:rly1断开rly2闭合(Link+V) 2:rly1闭合rly2断开(Link-V) 3:全闭合(采负载V)
** @Output    : void
** @Function  : 控制Link电压采样继电器(Rly1/Rly2)状态
** @The notes :
**===========================================================================================*/
static void SampCtrlHVLinkVRelyState(u8 state);

/*=============================================================================================
** @Name      : static void SampCtrlHVIsoVRelyState(u8 state)
** @Input     : 控制状态 0:全断开 1:PE闭合rly9断开(IsoV1) 2:PE闭合rly9闭合(IsoV2)
** @Output    : void
** @Function  : 控制绝缘检测PGND电压采样继电器(PE[RLY6/RLY7]/Rly9)状态
** @The notes :
**===========================================================================================*/
static void SampCtrlHVIsoVRelyState(u8 state);

/*=============================================================================================
** @Name      : static u8 SearchTempTabByResist(u32 data)
** @Input     : data:要查表的数据阻值 1Ω
** @Output    : 对应温度下标值
** @Function  : 通过NTC阻值查询温度表得到对应温度值
** @The notes :
**===========================================================================================*/
static u8 SearchTempTabByResist(u32 data);

#if(1 == SHUNT_V_COMP_EN)/*分流器温度补偿电压使能*/
/*=============================================================================================
** @Name      : static s16 GetShuntTCompVolt(void)
** @Input     : void
** @Output    : 当前温度对应补偿电压值 0.01mV
** @Function  : 通过当前温度计算分流器对应补偿电压值
** @The notes : 查表计算
**===========================================================================================*/
static s16 CalcShuntTCompVolt(void);
#endif

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void HVSampLTC2949Init(void)
** @Input     : void
** @Output    : void
** @Function  : 高压采样芯片2949初始化
** @The notes : 初始化调用(因LTC2949初始化时间有特殊要求，LTC2949不能直接由BSP调用初始化，改由任务调用)
**===========================================================================================*/
void HVSampLTC2949Init(void)
{
	;
}

/*=============================================================================================
** @Name      : void HigInfoSampleTask(void *P)
** @Input     : void
** @Output    : void
** @Function  : 高压采/分流器电流样任务
** @The notes : 周期任务
**===========================================================================================*/
void HigInfoSampleTask(void *P)
{
	static u8 sHVSampStep = 0xaa;
	static u8 sCfgErrTime = 0;		//初始化配置异常次数
	static u8 sComErrTime = 0;		//回检通信就绪异常次数
	static u8 sRcvErrTime = 0;		//接收总压采样值异常次数
	static u8 sLoaErrTimes = 0;		//负载电压跳变异常次数
	static u8 sIsoSamTimes = 0;		//绝缘连续采样次数
	u8 i = 0;
	u16 error = 0;
	u8 isReady = 0;
	float calcTemp = 0.0;
	float floatTemp = 0.0;
	float SumVolt = 0.0;
	float nowVolt = 0.0;
	float maxVolt = 0.0;
	float minVolt = 65535.0;
	float InsoVolt[INSO_VFIT_TIMES] = {0.0};

	for(;;)
	{
		switch(sHVSampStep)
		{
			/*等待LTC2949上电就绪*/
			case 0xaa:
				sHVSampStep = 0xbb;
				OSIF_TimeDelay(60);/*至少等待55ms,不要轻易修改*/
				break;

			/*LTC2949初始化第一阶段*/
			case 0xbb:
				LTC2949_init_lib(0, false, false);/* Initialize LTC2949 library*/
				LTC2949_init_device_state();	  /* Initialize LTC2949's device state*/
				error = LTC2949_WakeupAndAck();   /* wakeup LTC2949*/
				LTC2949_WriteTbctl(LTC2949_PRE2_MAX_EXTCLK, false);/*外置晶振使能*/
				LTC2949_OpctlIdle(); 			  /* clear OPCTRL, go to STANDBY state*/
				sHVSampStep = 0xcc;
				//OSIF_TimeDelay(200);/*必要延时,不要轻易修改*/
				OSIF_TimeDelay(50);/*必要延时,不要轻易修改*/
				break;

			/*LTC2949初始化第二阶段*/
			case 0xcc:
				error |= LTC2949_GoCont(LTC2949_BM_FACTRL_FACH1 | LTC2949_BM_FACTRL_FACH2 | LTC2949_BM_FACTRL_FACHA,
										LTC2949_BM_ADCCONF_P2ASV, NULL, false);
				//初始化出错
				if(error)
				{
					//初始化配置异常
					if(sCfgErrTime >= 3)
					{
						/*记录高压采样芯片初始化异常*/
						BitSet(sHigAFEErrFlag, 0);
						/*使能分流器采集电流*/
						if(1 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))
						{
							/*记录电流采样AFE异常*/
							SetCurrSampExpFlagAPI(eCExp_AfeErr, 1, 0);
						}
					}
					else
					{
						sCfgErrTime++;
					}

					/*返回重新初始化*/
					sHVSampStep = 0xbb;
					OSIF_TimeDelay(10);
				}
				else
				{
					sHVSampStep = 0xdd;
					//OSIF_TimeDelay(150);/*必要延时,不要轻易修改*/
					OSIF_TimeDelay(50);/*必要延时,不要轻易修改*/
				}
				break;

			/*LTC2949初始化结果检测,正常则闭合常闭继电器*/
			case 0xdd:
				error |= LTC2949_ContResult(LTC2949_BM_FACTRL_FACH1 | LTC2949_BM_FACTRL_FACH2 | LTC2949_BM_FACTRL_FACHA,
										LTC2949_BM_ADCCONF_P2ASV, NULL, false);
				//初始化出错
				if(error)
				{
					//初始化配置异常
					if(sCfgErrTime >= 3)
					{
						/*记录高压采样芯片初始化异常*/
						BitSet(sHigAFEErrFlag, 0);
						/*使能分流器采集电流*/
						if(1 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))
						{
							SetCurrSampExpFlagAPI(eCExp_AfeErr, 1, 0);/*记录电流采样AFE异常*/
						}
					}
					else
					{
						sCfgErrTime++;
					}

					/*返回重新初始化*/
					sHVSampStep = 0xbb;
					OSIF_TimeDelay(10);
				}
				else
				{
					/*清除高压采样芯片初始化异常*/
					BitClr(sHigAFEErrFlag, 0);

					/*接触器粘连检测步骤复位*/
					//SetSwhAdhesDiagFlagAPI(eAdhVlink_Init);
					/*绝缘检测步骤复位*/
					//SetInsuVpgndSampFlagAPI(eIsoVpgnd_Init);

					/*作总压采样准备*/
					Ltc2949GpioSetOutPutState(AFE_IOCTRL_BAT, 1);	/*闭合电池总电压继电器，RLY8*/

					/*控制Link继电器闭合作负载电压采样准备*/
					SampCtrlHVLinkVRelyState(3);

					/*变量清零*/
					sOutHVSampFinState = 0;
					sPreVSampChannel = 0;

					sLoaErrTimes = 0;
					sIsoSamTimes = 0;

					sCfgErrTime = 0;
					sHVSampStep = 0;
					OSIF_TimeDelay(5);
				}
				break;

			/*触发分流器电流采样通道:采样前检测LTC2949是否处于唤醒和正确配置状态(待完善),芯片正常则分流器电流采样*/
			case LTC2949_STEP_SHUV:
				// make sure HS byte is cleared at the beginning
				error |= LTC2949_RdFastData(NULL, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
				//isReady = CheckDeviceIsReady();
				if(1 == isReady)
				{
					/*LTC2949复位*/
					LTC2949_reset();
					/*记录高压采样芯片异常*/
					BitSet(sHigAFEErrFlag, 1);
					/*使能分流器采集电流*/
					if(1 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))
					{
						/*记录电流采样AFE异常*/
						SetCurrSampExpFlagAPI(eCExp_AfeErr, 1, 0);
					}
					sHVSampStep = 0xbb;	/*LTC2949重新初始化*/

					/*确认高压模拟前端异常*/
					if(sComErrTime >= 5)
					{
						ClrLtc2949SampInfo();
						sComErrTime = 0;
					}
					else
					{
						sComErrTime++;
					}
				}
				else
				{
					/*通信正常*/
					sComErrTime = 0;
					/*清除高压采样芯片异常*/
					BitClr(sHigAFEErrFlag, 1);

					/*使能分流器采样*/
					if(1 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))
					{
						/*清除电流采样AFE异常*/
						SetCurrSampExpFlagAPI(eCExp_AfeErr, 0, 0);

						/* 触发分流器AD转换*/
						LTC2949_SlotFastCfg(LTC2949_SLOTMUX_CFI1P, LTC2949_SLOTMUX_CFI1M);
						error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);
						/*进入分流器电流采集*/
						sHVSampStep = LTC2949_STEP_SHUV + 1;
					}
					/*无分流器采样直接进行总压*/
					else
					{
						/*获取下一采样通道*/
						sHVSampStep = SelectHVNextSampStep(sHVSampStep);
						if(sHVSampStep == LTC2949_STEP_SUMV)
						{
							/* 触发电池总电压AD转换*/
							LTC2949_SlotFastCfg(LTC2949_SLOTMUX_VBATP, LTC2949_SLOTMUX_GND);
							error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);
							/*进入总压采集*/
							sHVSampStep = LTC2949_STEP_SUMV + 1;
						}
					}
				}

				OSIF_TimeDelay(4);
				break;

			/*读取分流器电流采样值*/
			case LTC2949_STEP_SHUV + 1:
				LTC2949_RdFastData(fastData2949, NULL, LTC2949_68XX_CMD_RDCVA, NULL);	/*分流器采样值读取*/
				//floatTemp = fastData2949[LTC2949_RDFASTDATA_I1] * LTC2949_LSB_FIFOI1;
				floatTemp = fastData2949[LTC2949_RDFASTDATA_I1] * 0.00000760371;
				calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_Shunt, floatTemp);

				#if(1 == SHUNT_V_COMP_EN)/*分流器温度补偿电压使能*/
				calcTemp = calcTemp + CalcShuntTCompVolt();
				#endif

				InputGroupHVSideData(HIG_AFE_Shunt, (s16)calcTemp);

				/*把±7500*0.01mV当做AD值处理*/
				CalcSampleADToCurr(eCAFE_Shunt, ((s32)calcTemp));

				sHVSampStep = SelectHVNextSampStep(sHVSampStep);
				OSIF_TimeDelay(2);
				break;

			/*触发电池总压采样通道*/
			case LTC2949_STEP_SUMV:
				error |= LTC2949_RdFastData(NULL, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
				LTC2949_SlotFastCfg(LTC2949_SLOTMUX_VBATP, LTC2949_SLOTMUX_GND);
				error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);/* 触发电池总电压AD转换*/
				sHVSampStep = LTC2949_STEP_SUMV + 1;

				OSIF_TimeDelay(4);/*转换至少等待0.8ms*/
				break;

			/*读取电池总压采样值*/
			case LTC2949_STEP_SUMV + 1:
				LTC2949_RdFastData(fastData2949, NULL, LTC2949_68XX_CMD_RDCVA, NULL);	/*电池总电压采样值读取*/
				
				/*电池总电压采样正常*/
				if(fastData2949[LTC2949_RDFASTDATA_BAT] > 0)
				{
					/*接收正常*/
					sRcvErrTime = 0;
					/*清除高压采样总压值异常*/
					BitClr(sHigAFEErrFlag, 2);

					//floatTemp = fastData2949[LTC2949_RDFASTDATA_BAT] * LTC2949_LSB_FIFOBAT;
					floatTemp = fastData2949[LTC2949_RDFASTDATA_BAT] * 0.000375183;

					calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_BatV, floatTemp);	/*计算总压 0.01V*/
					InputGroupHVSideData(HIG_AFE_BatV, calcTemp);				/*显示总压 0.01V转0.1V*/
					InputInsuSampDataAPI(eInsu_Bat, (calcTemp / 100));			/*绝缘检测需要总压值 1V*/

		        	/*片选下一步骤*/
					sHVSampStep = SelectHVNextSampStep(sHVSampStep);
					OSIF_TimeDelay(2);
				}
				/*电池总电压采样异常(300ms无通信导致休眠)*/
				else
				{
					/*确认总压采样异常*/
					sRcvErrTime++;
					if(sRcvErrTime >= 2)
					{
						/*记录高压采样总压值异常*/
						BitSet(sHigAFEErrFlag, 2);

						/*清除总压和绝缘PGND电压*/
						InputGroupHVSideData(HIG_AFE_BatV, 0);
						InputGroupHVSideData(HIG_AFE_PgndV, 0);
						sRcvErrTime = 0;
					}

					/*返回重新初始化*/
					sHVSampStep = 0xbb;
					OSIF_TimeDelay(500);//不能连续初始化,否则占用太长时间影响SBC喂狗造成SBC复位
				}
				break;

			/*负载电压采样,继电器控制*/
			case LTC2949_STEP_LODV:
				/*控制Link继电器闭合进行负载电压采样*/
				SampCtrlHVLinkVRelyState(3);

				/*记录正在负载电压采样等待*/
				sDelySampWaitState = 1;

				/*等待电压稳定时去采集下一通道(下一轮回头到达延时再读取)*/
				sHVSampStep = SelectHVNextSampStep(sHVSampStep);
				OSIF_TimeDelay(5);
				break;

			/*负载电压采样,触发负载电压采样通道*/
			case LTC2949_STEP_LODV + 1:
				/*记录正在负载电压采样等待*/
				sDelySampWaitState = 1;

				/*使能用Link+电压通道采集预充电压*/
				#if(1 == RUN_PREV_SAMP_CHAN)
				/*负极接触器闭合状态*/
				if(ePSTATE_ON == TacGetGroupSwhCtrlStateAPI(eGroupSwh_Neg))
				{
					/*启动Link通道*/
					error |= LTC2949_RdFastData(NULL, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
					LTC2949_SlotFastCfg(LTC2949_SLOTMUX_V2, 0);
					error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);/* 触发link+电压采样*/
					sPreVSampChannel = 1;
				}
				/*负极接触器断开状态*/
				else
				{
					/*启动V7通道*/
					error |= LTC2949_RdFastData(NULL, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
					LTC2949_SlotFastCfg(LTC2949_SLOTMUX_V7, 0);
					error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);/* 触发芯片负载电压采样*/
					sPreVSampChannel = 0;
				}
				/*用负载电压通道V7采集预充电压*/
				#else
				/*启动V7通道*/
				error |= LTC2949_RdFastData(NULL, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
				LTC2949_SlotFastCfg(LTC2949_SLOTMUX_V7, 0);
				error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);/* 触发芯片负载电压采样*/
				sPreVSampChannel = 0;
				#endif
				sHVSampStep = LTC2949_STEP_LODV + 2;
				OSIF_TimeDelay(5);/*转换至少等待0.8ms*/
				break;

			/*负载电压采样,读取负载电压值*/
			case LTC2949_STEP_LODV + 2:
				LTC2949_RdFastData(fastData2949, NULL, LTC2949_68XX_CMD_RDCVA, NULL);	/* poll LTC2949 for conversion done*/
				//floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * LTC2949_LSB_FIFOAUX;
				floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * 0.000375183;

				/*跳变3mv(高压3V)内*/
				if((ABS(GetGDcSideSampRealInfoAPI(eReferSampInfo200_LoadV7), (floatTemp * 1000)) <= 3)
					|| (sLoaErrTimes >= 3))
				{
					InputSwhAdhesSampData(eAdhSamp_LoadV, floatTemp);				/*记录端口原始电压,直接取端口电压 1V*/
					InputGroupHVSideData(HIG_AFE_LoadV7, (s32)(floatTemp * 1000));  /*显示端口原始电压,直接取端口电压 1mV*/

					/*负载电压采样等待完成*/
					sDelySampWaitState = 0;

					/*有接触器闭合负载电压按照粘连(预充)公式计算*/
					if(FALSE == TacGetGroupRunSwhAllOffStateAPI())
					{
						calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_PreV, floatTemp);	/*转换成高压 0.01V*/

						/*输入预充电压*/
						InputGroupHVSideData(HIG_AFE_PreV, calcTemp);

						/*设置外端预充采样完成标志*/
						SampSetOutHVSampFinStateAPI(1);

						sLoaErrTimes = 0;
						sHVSampStep = SelectHVNextSampStep(sHVSampStep);
					}
					/*接触器全断开无粘连检测负载电压按照粘连(负载)公式计算*/
					else
					{
						sLoaErrTimes = 0;
						sHVSampStep = SelectHVNextSampStep(sHVSampStep);

						/*使能接触器粘连检测*/
						if(1 == BitGet(gGBmuHigLevPara_103[eBmuHigLevPara103_AdhMonitType], 0))
						{
							/*下一步不是检测粘连则此处计算预充(负载)电压*/
							if((sHVSampStep >= LTC2949_STEP_ISOV) || (sHVSampStep < LTC2949_STEP_LNKV))
							{
								/*接触器已粘连按照未粘连(预充)公式计算*/
								if(GetSwhAdhesErrFlagAPI() > 0)
								{
									/*计算预充电压*/
									calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_PreV, floatTemp);	/*转换成高压 0.01V*/

									/*输入预充电压*/
									InputGroupHVSideData(HIG_AFE_PreV, calcTemp);
								}
								/*接触器未粘连按照粘连(负载)公式计算*/
								else
								{
									/*计算负载电压*/
									calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_LoadV, floatTemp);	/*转换成高压 0.01V*/

									/*输入负载电压*/
									InputGroupHVSideData(HIG_AFE_LoadV, calcTemp);
								}

								/*设置外侧(预充/负载)电压采样完成标志*/
								SampSetOutHVSampFinStateAPI(1);
							}
						}
						/*未使能接触器粘连检测*/
						else
						{
							calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_LoadV, floatTemp);	/*转换成高压 0.01V*/

							/*输入负载电压*/
							InputGroupHVSideData(HIG_AFE_LoadV, calcTemp);

							/*设置预充采样完成标志*/
							SampSetOutHVSampFinStateAPI(1);
						}
					}

					/*需要断开Link继电器*/
					if(0 == gGBmuHigLevPara_103[eBmuHigLevPara103_AdhRlyState])
					{
						/*下一步不是检测粘连则此处计算预充(负载)电压*/
						if((sHVSampStep >= LTC2949_STEP_ISOV) || (sHVSampStep < LTC2949_STEP_LNKV))
						{
							/*断开Link继电器*/
							SampCtrlHVLinkVRelyState(0);
						}
					}
				}
				/*采集负载电压异常*/
				else
				{
					/*返回重新采一次*/
					sLoaErrTimes++;
					sHVSampStep = LTC2949_STEP_LODV + 1;
				}
				OSIF_TimeDelay(2);
				break;

			/*开始接触器粘连检测电压(Link+/Link-)采样:继电器控制(先片选Link+)*/
			case LTC2949_STEP_LNKV:
				/*可能粘连时断开绝缘检测继电器(RLY6/RLY7),避免外部干扰Link电压
				if((GetSwhAdhesErrFlagAPI() > 0)
					&& (eErr_A == DiagGetErrLastStateAPI(eERR_ID_K0MAI_EXP))
					//&& (eErr_A == DiagGetErrLastStateAPI(eERR_ID_K1PRE_EXP))
					&& (eErr_A == DiagGetErrLastStateAPI(eERR_ID_K2NEG_EXP)))
				{
					//断开绝缘检测继电器(RLY6/RLY7)采集的负母线电压总是总压一半,不受绝缘情况影响,所以要保持闭合采集真实负母线电压
					DEVGPIOSetPortState(ePOut_ISO_PGND, ePSTATE_OFF);
				}*/

				/*断开RLY1，闭合RLY2*/
				SampCtrlHVLinkVRelyState(1);
				SetSwhAdhesDiagFlagAPI(eAdhVlink_SampV1);		/*粘连检测开始Link+采样*/

				/*记录正在Link电压采样等待*/
				sDelySampWaitState = 2;

				/*等待电压稳定时去采集下一通道(下一轮回头到达延时再读取)*/
				sHVSampStep = SelectHVNextSampStep(sHVSampStep);
				OSIF_TimeDelay(5);
				break;

			/*link+电压转换,外端正极Link+电压采集通道使能*/
			case LTC2949_STEP_LNKV + 1:
				/*记录正在Link电压采样等待*/
				sDelySampWaitState = 2;

				error |= LTC2949_RdFastData(NULL, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
				LTC2949_SlotFastCfg(LTC2949_SLOTMUX_V2, 0);
				error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);/* 触发link+电压采样*/
				sHVSampStep = LTC2949_STEP_LNKV + 2;
				OSIF_TimeDelay(4);/*转换至少等待0.8ms*/
				break;

			/*link+电压采样,读取外端正极Link+电压采集通道*/
			case LTC2949_STEP_LNKV + 2:
				LTC2949_RdFastData(fastData2949, NULL, LTC2949_68XX_CMD_RDCVA, NULL);	/* poll LTC2949 for conversion done*/
				//floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * LTC2949_LSB_FIFOAUX;
				floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * 0.000375183;

				calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_Link1V, floatTemp);	/*转换成1mV*/
				InputGroupHVSideData(HIG_AFE_Link1V, calcTemp);					/*显示低压1mV*/
				InputSwhAdhesSampData(eAdhSamp_Link1, floatTemp);				/*接触器粘连检测,直接取端口电压 1V*/
				SetSwhAdhesDiagFlagAPI(eAdhVlink_SeleV2);		                /*粘连检测开始Link-片选*/
				sHVSampStep = LTC2949_STEP_LNKV + 3;
				OSIF_TimeDelay(2);
				break;

			/*继续接触器粘连检测电压(Link+/Link-)采样:继电器控制(再片选Link-)*/
			case LTC2949_STEP_LNKV + 3:
				/*闭合RLY1，断开RLY2*/
				SampCtrlHVLinkVRelyState(2);
				SetSwhAdhesDiagFlagAPI(eAdhVlink_SampV2);		/*粘连检测开始Link-采样*/

				/*记录正在Link电压采样等待*/
				sDelySampWaitState = 2;

				/*等待电压稳定时去采集下一通道(下一轮回头到达延时再读取)*/
				sHVSampStep = SelectHVNextSampStep(sHVSampStep);
				OSIF_TimeDelay(5);
				break;

			/*link-电压转换,负极Link-电压采集通道使能*/
			case LTC2949_STEP_LNKV + 4:
				/*记录正在Link电压采样等待*/
				sDelySampWaitState = 2;

				error |= LTC2949_RdFastData(NULL, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
				LTC2949_SlotFastCfg(LTC2949_SLOTMUX_V3, 0);
				error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);/* 触发link-电压采样*/
				sHVSampStep = LTC2949_STEP_LNKV + 5;
				OSIF_TimeDelay(4);/*转换至少等待0.8ms*/
				break;

			/*link-电压采样,读取外端负极Link-电压采集通道*/
			case LTC2949_STEP_LNKV + 5:
				LTC2949_RdFastData(fastData2949, NULL, LTC2949_68XX_CMD_RDCVA, NULL);	/* poll LTC2949 for conversion done*/
				//floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * LTC2949_LSB_FIFOAUX;
				floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * 0.000375183;

				floatTemp = LinkNegVoltFilter(floatTemp);                           /*采样值滤波处理*/
				if(65535.0 == floatTemp)                                            /*采样值异常*/
				{
					sHVSampStep = LTC2949_STEP_LNKV + 4;                            /*返回重新读取采样值*/
				}
				else                                                                /*采样值正常*/
				{
					calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_Link2V, floatTemp);	/*转换成高压 0.01V*/
					InputGroupHVSideData(HIG_AFE_Link2V, calcTemp);					/*显示高压 0.01V转0.1V*/

					InputSwhAdhesSampData(eAdhSamp_Link2, floatTemp);				/*接触器粘连检测,直接取端口电压 1V*/
					SetSwhAdhesDiagFlagAPI(eAdhVlink_SampOK);						/*粘连检测电压采样结束*/

					/*需要断开Link继电器*/
					if(0 == gGBmuHigLevPara_103[eBmuHigLevPara103_AdhRlyState])
					{
						/*断开Link继电器*/
						SampCtrlHVLinkVRelyState(0);
					}
					/*不需要断开Link继电器*/
					else
					{
						/*闭合Link继电器*/
						SampCtrlHVLinkVRelyState(3);
					}

					/*已确认粘连时闭合绝缘检测继电器(RLY6/RLY7),确保负母线电压为真实值
					if((GetSwhAdhesErrFlagAPI() > 0)
						&& (gGBmuHigLevPara_103[eBmuHigLevPara103_LeakMonitType] > 0)
						&& ((eErr_B == DiagGetErrLastStateAPI(eERR_ID_K0MAI_EXP))
						//|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_K1PRE_EXP))
						|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_K2NEG_EXP))))
					{
						//断开绝缘检测继电器(RLY6/RLY7)采集的负母线电压总是总压一半,不受绝缘情况影响,所以要保持闭合采集真实负母线电压
						DEVGPIOSetPortState(ePOut_ISO_PGND, ePSTATE_ON);
					}*/

					/*Link电压采样等待完成*/
					sDelySampWaitState = 0;

					sHVSampStep = SelectHVNextSampStep(sHVSampStep);
				}
				OSIF_TimeDelay(2);
				break;

			/*开始绝缘PGND电压采样:继电器控制*/
			case LTC2949_STEP_ISOV:
				/*需要片选V1*/
				if(eIsoVpgnd_SeleV1 == GetInsuVpgndSampFlagAPI())
				{
					/*闭合PE(RLY6/RLY7)/断开RLY9*/
					SampCtrlHVIsoVRelyState(1);

					SetInsuVpgndSampFlagAPI(eIsoVpgnd_SampV1);	       /*进入V1采样*/
				}
				/*需要片选V2*/
				else
				{
					/*闭合PE(RLY6/RLY7)/闭合RLY9*/
					SampCtrlHVIsoVRelyState(2);

					SetInsuVpgndSampFlagAPI(eIsoVpgnd_SampV2);	       /*进入V2采样*/
				}
				sIsoSamTimes = 0;

				/*记录正在绝缘电压采样等待*/
				sDelySampWaitState = 3;

				/*等待电压稳定时去采集下一通道(下一轮回头到达延时再读取)*/
				sHVSampStep = SelectHVNextSampStep(sHVSampStep);
				OSIF_TimeDelay(5);
				break;

			/*PGND电压采样,机壳相对对负母线电压采样通道使能*/
			case LTC2949_STEP_ISOV + 1:
			case LTC2949_STEP_PGND:
				if(sHVSampStep == (LTC2949_STEP_ISOV + 1))
				{
					/*记录正在绝缘电压采样等待*/
					sDelySampWaitState = 3;
				}

				//DEVGPIOSetPortState(ePOut_ISO_PGND, ePSTATE_ON);   /*闭合绝缘检测PE继电器*(RLY6/RLY7)*/
				/* 触发PGND电压采样*/
				error |= LTC2949_RdFastData(NULL, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
				LTC2949_SlotFastCfg(LTC2949_SLOTMUX_V1, 0);
				error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);

				/* 进入下一步读取采样值*/
				sHVSampStep = sHVSampStep + 1;
				OSIF_TimeDelay(4);/*转换至少等待0.8ms*/
				break;

			/*读取PGND电压采样,机壳相对对负母线电压值*/
			case LTC2949_STEP_ISOV + 2:
			case LTC2949_STEP_PGND + 1:
				LTC2949_RdFastData(fastData2949, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
				//floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * LTC2949_LSB_FIFOAUX;
				floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * 0.000375183;

				/*绝缘检测采样滤波使能*/
				#if(1 == INSO_VFIT_ENABLE)
				InsoVolt[sIsoSamTimes] = Ltc2949ChangeADtoVolt(HIG_AFE_PgndV, floatTemp);	/*转换成高压 0.1V*/

				/*计算采样次数*/
				sIsoSamTimes++;
				/*达到采样次数*/
				if(sIsoSamTimes >= INSO_VFIT_TIMES)
				{
					for(i = 0; i < INSO_VFIT_TIMES; i++)
					{
						nowVolt = InsoVolt[i];
						SumVolt += nowVolt;

						if(nowVolt > maxVolt)
						{
							maxVolt = nowVolt;
						}

						if(nowVolt < minVolt)
						{
							minVolt = nowVolt;
						}
					}

					calcTemp = (SumVolt - maxVolt - minVolt) / ((float)INSO_VFIT_TIMES - 2.0);/*取平均值*/

					/*绝缘检测*/
					if((LTC2949_STEP_ISOV + 2) == sHVSampStep)
					{
						/*已经采完V1*/
						if(eIsoVpgnd_SampV1 == GetInsuVpgndSampFlagAPI())
						{
							InputGroupHVSideData(HIG_AFE_PgndV, calcTemp);		/*输出PGND 0.1V*/

							/*输入到绝缘计算模块 1V*/
							InputInsuSampDataAPI(eInsu_V1, calcTemp / 10);

							/*进入片选V2*/
							//SetInsuVpgndSampFlagAPI(eIsoVpgnd_SeleV2);

							/*直接闭合PE(RLY6/RLY7)/闭合RLY9,片选好V2*/
							SampCtrlHVIsoVRelyState(2);

							SetInsuVpgndSampFlagAPI(eIsoVpgnd_SampV2);	        /*进入V2采样*/

							/*记录正在绝缘电压采样等待*/
							sDelySampWaitState = 3;
						}
						/*已经采完V2*/
						else
						{
							/*输入到绝缘计算模块 1V*/
							InputInsuSampDataAPI(eInsu_V2, calcTemp / 10);

							/*需要闭合PE则切换片选好V1*/
							if(1 == gGBmuHigLevPara_103[eBmuHigLevPara103_LedkRlyState])
							{
								/*闭合PE(RLY6/RLY7)/断开RLY9*/
								SampCtrlHVIsoVRelyState(1);
							}
							/*需要断开PGND继电器*/
							else
							{
								/*断开PE(RLY6/RLY7)/断开RLY9*/
								SampCtrlHVIsoVRelyState(0);
							}

							SetInsuVpgndSampFlagAPI(eIsoVpgnd_SampOK);	        /*标志采样完成*/

							/*绝缘电压采样等待完成*/
							sDelySampWaitState = 0;
						}
					}
					/*仅采集母线电压*/
					else
					{
						InputGroupHVSideData(HIG_AFE_PgndV, calcTemp);			/*输出PGND 0.1V*/
					}

					/*计算中间数据必须清零*/
					SumVolt = 0;
					nowVolt = 0;
					maxVolt = 0;
					minVolt = 65535.0;
					for(i = 0; i < INSO_VFIT_TIMES; i++)
					{
						InsoVolt[i] = 0;
					}
					sIsoSamTimes = 0;

					/*进入下一步采样*/
					sHVSampStep = SelectHVNextSampStep(sHVSampStep);
				}
				/*未达到滤波采集次数*/
				else
				{
					/*返回重新采样*/
					sHVSampStep = sHVSampStep - 1;
				}

				#else
				calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_PgndV, floatTemp);
				InputGroupHVSideData(HIG_AFE_PgndV, calcTemp);
				/*已经采完V1*/
				if(eIsoVpgnd_SampV1 == GetInsuVpgndSampFlagAPI())
				{
					/*输入到绝缘计算模块*/
					InputInsuSampDataAPI(eInsu_V1, calcTemp / 10);

					/*切换片选好V2*/
					/*闭合PE(RLY6/RLY7)/闭合RLY9*/
					SampCtrlHVIsoVRelyState(2);

					/*进入V2采样*/
					SetInsuVpgndSampFlagAPI(eIsoVpgnd_SampV2);
				}
				/*已经采完V2*/
				else
				{
					/*输入到绝缘计算模块*/
					InputInsuSampDataAPI(eInsu_V2, calcTemp / 10);

					/*需要闭合PE则切换片选好V1*/
					if(1 == gGBmuHigLevPara_103[eBmuHigLevPara103_LedkRlyState])
					{
						/*闭合PE(RLY6/RLY7)/断开RLY9*/
						SampCtrlHVIsoVRelyState(1);
					}
					/*需要断开PGND继电器*/
					else
					{
						/*断开PE(RLY6/RLY7)/断开RLY9*/
						SampCtrlHVIsoVRelyState(0);
					}

					SetInsuVpgndSampFlagAPI(eIsoVpgnd_SampOK);	       /*标志采样完成*/

					/*绝缘电压采样等待完成*/
					sDelySampWaitState = 0;
				}

				/*进入下一步采样*/
				sHVSampStep = SelectHVNextSampStep(sHVSampStep);
				#endif
				OSIF_TimeDelay(2);
				break;

			/*分流器温度采样使能*/
			case LTC2949_STEP_SHUT:
				error |= LTC2949_RdFastData(NULL, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
				LTC2949_SlotFastCfg(LTC2949_SLOTMUX_V4, 0);
				error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);/* 触发分流器环境温度采样*/
				sHVSampStep = LTC2949_STEP_SHUT + 1;
				OSIF_TimeDelay(4);/*转换至少等待0.8ms*/
				break;

			/*分流器温度采样读取*/
			case LTC2949_STEP_SHUT + 1:
				LTC2949_RdFastData(fastData2949, NULL, LTC2949_68XX_CMD_RDCVA, NULL);	/* poll LTC2949 for conversion done*/
				//floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * LTC2949_LSB_FIFOAUX;
				floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * 0.000375183;
				calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_Temp, floatTemp);
				InputGroupHVSideData(HIG_AFE_Temp, calcTemp);
				sHVSampStep = SelectHVNextSampStep(sHVSampStep);
				OSIF_TimeDelay(2);
				break;

			/*芯片基准源采样,LTC2949基准回采使能*/
			case LTC2949_STEP_REFV:
				error |= LTC2949_RdFastData(NULL, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
				LTC2949_SlotFastCfg(LTC2949_SLOTMUX_V5, 0);
				error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);/* 触发芯片基准源采样*/
				sHVSampStep = LTC2949_STEP_REFV + 1;
				OSIF_TimeDelay(4);/*转换至少等待0.8ms*/
				break;

			/*芯片基准源采样,LTC2949基准回采*/
			case LTC2949_STEP_REFV + 1:
				LTC2949_RdFastData(fastData2949, NULL, LTC2949_68XX_CMD_RDCVA, NULL);	/* poll LTC2949 for conversion done*/
				//floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * LTC2949_LSB_FIFOAUX;
				floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * 0.000375183;
				calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_Vref, floatTemp);
				InputGroupHVSideData(HIG_AFE_Vref, calcTemp);
				sHVSampStep = SelectHVNextSampStep(sHVSampStep);
				OSIF_TimeDelay(2);
				break;

			/*芯片5V输出电压采样,高压侧5V电压采集使能*/
			case LTC2949_STEP_IC5V:
				error |= LTC2949_RdFastData(NULL, NULL, LTC2949_68XX_CMD_RDCVA, NULL);
				LTC2949_SlotFastCfg(LTC2949_SLOTMUX_V6, 0);
				error |= LTC2949_ADxxAddressed(MD_NORMAL, CELL_CH_ALL, DCP_DISABLED, 0);/* 触发芯片5V输出电压采样*/
				sHVSampStep = LTC2949_STEP_IC5V + 1;
				OSIF_TimeDelay(4);/*转换至少等待0.8ms*/
				break;

			/*芯片5V输出电压采样,高压侧5V电压采集*/
			case LTC2949_STEP_IC5V + 1:
				LTC2949_RdFastData(fastData2949, NULL, LTC2949_68XX_CMD_RDCVA, NULL);	/* poll LTC2949 for conversion done*/
				//floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * LTC2949_LSB_FIFOAUX;
				floatTemp = fastData2949[LTC2949_RDFASTDATA_AUX] * 0.000375183;
				calcTemp = Ltc2949ChangeADtoVolt(HIG_AFE_Out5V, floatTemp);
				InputGroupHVSideData(HIG_AFE_Out5V, calcTemp);
				sHVSampStep = SelectHVNextSampStep(sHVSampStep);
				OSIF_TimeDelay(2);
				break;

			default:
				sHVSampStep = 0;
				OSIF_TimeDelay(1);
				break;
		}
	}
}

/*=============================================================================================
** @Name      : float Ltc2949ChangeADtoVolt(e_HAfeSampChan index, float data)
** @Input     : index:采样通道 data:采样值
** @Output    : void
** @Function  : 将LTC2949采样值转化为实际值
** @The notes : 15bits
**===========================================================================================*/
float Ltc2949ChangeADtoVolt(e_HAfeSampChan index, float data)
{
	float tempData = 0.0;

	switch(index)
	{
		/*分流器电流采样 0.01mV*/
		case HIG_AFE_Shunt:
			tempData = data * 1000;				/*转化为1mv值*/
			//tempData = tempData * 4 / 3 * 100;/*需根据实际分流器比例进行修改,0.01A*/
			tempData = tempData * 100;  		/*转化为0.01mV*/
			break;

		/*电池总电压采样, 0.01V*/
		case HIG_AFE_BatV:
			tempData = data * 221 * 100;		/*Y=221*X,1V*/
			break;

		/*PGND电压采样,机壳相对对负母线电压 0.1V*/
		case HIG_AFE_PgndV:
			tempData = data * 221 * 10;			/*Y=221*X,1V*/
			break;

		/*link+电压采样,外端电压(预充电压)正极电压采集 1V转1mV*/
		case HIG_AFE_Link1V:
			//tempData = (667.666667 * (data - 3.0) + 3.0) * 100;/*0.01V*/
			tempData = data * 1000;/*1mV*/
			break;

		/*link-电压采样,外端电压(预充电压)负极电压采集1V转1mV*/
		case HIG_AFE_Link2V:
			//tempData = (667.666667 * (data - 3.0) + 3.0) * 100;/*0.01V*/
			//tempData = (303 - 101 * data) * 100;/*加大负极粘连电压使用 0.01V*/
			tempData = data * 1000;/*1mV*/
			break;

		/*分流器温度采样 1℃*/
		case HIG_AFE_Temp:
			/*电阻上的电压值,1mv*/
			tempData = data * 1000;
			if(tempData > 2999.0)
			{
				tempData = 2999.0;
			}
			/*电压值转换为电阻值,10K电阻分压X=5*Y/(10+Y),Y=10*X/(5-X),1kΩ*/
			tempData = (tempData * 10000) / (3000 - tempData);
			/*查表转换为温度值,1℃*/
			tempData = SearchTempTabByResist(tempData) - 41;
			break;

		/*采样芯片基准源,LTC2949基准回采 0.01V*/
		case HIG_AFE_Vref:
			tempData = data * 100;			/*0.01V*/
			break;

		/*采样芯片5V输出电压采样,高压侧5V电压采集 0.01V*/
		case HIG_AFE_Out5V:
			tempData = data * 2 * 100;		/*Y=2*X,1V*/
			break;

		/*负载电压采样,无粘连时电压采集 0.01V*/
		case HIG_AFE_LoadV:
			//tempData = data * 1894.9 + 0.2694;	/*Y=(4421*X/2.35),1V*/
			//data += 0.0115;
			//data -= 0.002;
			tempData = data * 442100 / 2.35;		/*Y=(4421*X/2.35),1V*/
			break;

		/*预充电压采样,有粘连时电压采集 0.01V*/
		case HIG_AFE_PreV:
			/*Link+电压采样通道*/
			if(1 == sPreVSampChannel)
			{
				tempData = (data * 2201 - 6628) * 100;		/*Y=(2201*X-6628),1V*/
			}
			/*负载电压V7采样通道*/
			else
			{
				//data += 0.0135;
				tempData = (data * 2201 / 2.35) * 100;		/*Y=(2201*X/2.35),1V*/
			}
			break;

		default:
			tempData = 0;
			break;
	}

	return(tempData);
}

/*=============================================================================================
** @Name      : void Ltc2949GpioSetOutPutState(e_AfeIoCtrlChan chan, u8 state)
** @Input     : index:gpio端口     state: 0 或 1
** @Output    : void
** @Function  : 设置gpio端口号输出状态
** @The notes :
**===========================================================================================*/
void Ltc2949GpioSetOutPutState(e_AfeIoCtrlChan chan, u8 state)
{
	if((chan > 5) || (chan < 1) || (state > 1))
	{
		return;
	}

	if(state == 1)
	{
		BitSet(sHigAFERelyState, (u8)(chan - 1));
		LTC2949_GpioConfig(chan, LTC2949_BM_GPIOCFG_OUPUT_HIGH);
		//LTC2949_Gpo1to4ConfigSet(LTC2949_BM_GPIOCFG_OUPUT_HIGH << chan);
	}
	else
	{
		BitClr(sHigAFERelyState, (u8)(chan - 1));
		LTC2949_GpioConfig(chan, LTC2949_BM_GPIOCFG_OUPUT_LOW);
		//LTC2949_Gpo1to4ConfigSet(LTC2949_BM_GPIOCFG_OUPUT_LOW << chan);
	}

	LTC2949_GpioCurrConfigWrite();
}

/*=============================================================================================
** @Name      : u8 CheckDeviceIsReady(void)
** @Input     : void
** @Output    : void
** @Function  : 检测LTC2949是否处于唤醒和正确配置状态
** @The notes :
**===========================================================================================*/
u8 CheckDeviceIsReady(void)
{
	u8 data[10] = {0};
	u8 error = 0;
	bool expChkFailed = false;

	if (((error = LTC2949_READ(LTC2949_REG_OPCTRL, 1, data)) != 0) 		// read OPCTRL and check for PEC error
		|| (data[0] != LTC2949_BM_OPCTRL_CONT) 							// check if continuous mode is enabled
		|| ((error = LTC2949_READ(LTC2949_REG_FACTRL, 1, data)) != 0) 	// read FACTRL and check for PEC error
		|| ((data[0] & ~LTC2949_BM_FACTRL_FACONV) != (LTC2949_BM_FACTRL_FACH2 | LTC2949_BM_FACTRL_FACHA)) // check if correct fast mode is enabled
		|| ((error = LTC2949_ADCConfigRead(data)) != 0) 				// read ADCCONF and check for PEC error
		|| (data[0] != LTC2949_BM_ADCCONF_P2ASV) 						// check if ADCCONF is set correctly
		|| ((error = LTC2949_ReadChkStatusFaults(						// check status and faults registers for any failure
			/*bool lockMemAndClr:     */ false,
			/*bool printResult:       */ false,
			/*u8 len:                 */ 10,
			/*u8 * statFaultsExpAndRd:*/ data,
			/*bool * expChkFailed:    */ &expChkFailed,
			/*u8 expDefaultSet):      */ LTC2949_STATFAULTSCHK_IGNORE_STATUPD | LTC2949_STATFAULTSCHK_DFLT_AFTER_CLR)) != 0) || expChkFailed
		)
	{
		//Serial.print('R');
		//PrintComma();
		error |= WakeUpReportStatus();
		//error |= Cont(false); // go IDLE
		//Delay(LTC2949_TIMING_CONT_CYCLE);
		//error |= Cont(true);
		//timeSlotFast = millis() + LTCDEF_CYCLE_TIME_SLOT_FAST;
	}
	return error;
}

/*=============================================================================================
** @Name      : void SampSetOutHVSampFinStateAPI(u8 state)
** @Input     : state:[0未采样 1采样完成]
** @Output    : void
** @Function  : 设置外端高压采样完成状态
** @The notes : 负载电压或预充电压
**===========================================================================================*/
void SampSetOutHVSampFinStateAPI(u8 state)
{
	/*状态正确*/
	if(state <= 1)
	{
		sOutHVSampFinState = state;
	}
}

/*=============================================================================================
** @Name      : u8 SampGetOutHVSampFinStateAPI(void)
** @Input     : void
** @Output    : 外端高压采样完成标志 [0未采样 1采样完成]
** @Function  : 获取外端高压采样完成状态
** @The notes : 负载电压或预充电压
**===========================================================================================*/
u8 SampGetOutHVSampFinStateAPI(void)
{
	return(sOutHVSampFinState);
}

/*=============================================================================================
** @Name      : u8 SampGetHigVSampExpFlagAPI(void)
** @Input     : void
** @Output    : 高压采样失效标志[按Bit位表示:Bit0:初始化配置异常 Bit1:通信异常](0:正常 1:异常)
** @Function  : 获取高压采样失效标志
** @The notes :
**===========================================================================================*/
u8 SampGetHigVSampExpFlagAPI(void)
{
    return(sHigAFEErrFlag);
}

/*=============================================================================================
** @Name      : u8 SampGetHigAFERelyStateAPI(void)
** @Input     : void
** @Output    : 高压采样外围继电器控制状态(按位表示1个继电器) 0:断开 1:闭合
** @Function  : 获取高压高压采样各个外围继电器控制状态
** @The notes : bit0:总压  bit1:Link-RLY1 bit2:Link+RLY2 bit3:绝缘
**===========================================================================================*/
u8 SampGetHigAFERelyStateAPI(void)
{
    return(sHigAFERelyState);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u8 WakeUpReportStatus(void)
** @Input     : void
** @Output    : void
** @Function  : 获取LTC2949是否处于唤醒和正确配置状态
** @The notes :
**===========================================================================================*/
static u8 WakeUpReportStatus(void)
{
	u8 error = 0;

	error = LTC2949_WakeupAndAck();
	error |= LTC2949_ReadChkStatusFaults(true, true, 10, NULL, NULL, LTC2949_STATFAULTSCHK_DEFAULT_SETTING);

	return(error);
}

/*=============================================================================================
** @Name      : static float LinkNegVoltFilter(float volt)
** @Input     : volt:当次采样值(1.0V)
** @Output    : 滤波后采样值(1.0V,65535.0表示异常需要重新采样)
** @Function  : Link-电压采样值滤波
** @The notes : 负极接触器闭合或断开3.0V±10mV内不滤波
**===========================================================================================*/
static float LinkNegVoltFilter(float volt)
{
	u8 i = 0;
	float aftVolt = 0;
	float maxVolt = 0;
	float minVolt = 65535.0;
	static u8 sSampTime = 0;
	static u8 sSampIndex = 0;
	static float sHisVolt[10] = {65535.0, 65535.0, 65535.0, 65535.0, 65535.0, 65535.0, 65535.0, 65535.0, 65535.0, 65535.0};

	/*首次采样*/
	if(65535.0 == sHisVolt[0])
	{
		/*当次在粘连范围*/
		if((volt >= (Adhesion_Link2_Vref - Adhesion_Link2_Voff))
			&& (volt <= (Adhesion_Link2_Vref + Adhesion_Link2_Voff)))
		{
			/*重新开始记录滤波值*/
			aftVolt = 65535.0;
		}
		else
		{
			aftVolt = volt;
		}

		maxVolt = 3.0;
		minVolt = 3.0;

		sHisVolt[0] = volt;
		sSampIndex = 1;
		sSampTime = 1;
	}
	/*负极接触器闭合则不滤波*/
	else if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))
	{
		aftVolt = (sHisVolt[0] + volt) / 2;

		maxVolt = aftVolt;
		minVolt = aftVolt;

		sHisVolt[sSampIndex] = aftVolt;
		sSampIndex = 0;
		sSampTime = 0;
	}
	/*需要滤波*/
	else
	{
		/*当次在粘连范围*/
		if((volt >= (Adhesion_Link2_Vref - Adhesion_Link2_Voff))
			&& (volt <= (Adhesion_Link2_Vref + Adhesion_Link2_Voff)))
		{
			/*刚采样1次*/
			if(0 == sSampIndex)
			{
				/*重新开始记录滤波值*/
				aftVolt = 65535.0;

				/*重新开始记录滤波值*/
				sHisVolt[sSampIndex] = volt;
				sSampIndex++;
				sSampTime++;
			}
			/*采样多次*/
			else
			{
				/*上次未粘连*/
				if((sHisVolt[sSampIndex - 1] < (Adhesion_Link2_Vref - Adhesion_Link2_Voff))
					|| (sHisVolt[sSampIndex - 1] > (Adhesion_Link2_Vref + Adhesion_Link2_Voff)))
				{
					/*重新开始记录滤波值*/
					sSampIndex = 0;
					sSampTime = 0;
				}

				/*记录滤波值*/
				sHisVolt[sSampIndex] = volt;
				sSampIndex++;
				sSampTime++;

				/*采样满滤波次数*/
				if(sSampTime >= 10)
				{
					/*计算平均值*/
					for(i = 0; i < sSampTime; i++)
					{
						if(sHisVolt[i] > maxVolt)
						{
							maxVolt = sHisVolt[i];
						}
						if(sHisVolt[i] < minVolt)
						{
							minVolt = sHisVolt[i];
						}

						aftVolt += sHisVolt[i];
					}

					aftVolt = (aftVolt - maxVolt - minVolt) / (sSampTime - 2);
					sSampIndex = 0;
					sSampTime = 0;
				}
				/*采样未达到滤波次数*/
				else
				{
					aftVolt = 65535.0;
				}
			}
		}
		/*非粘连故障范围*/
		else
		{
			/*记录滤波值*/
			sHisVolt[sSampIndex] = volt;
			sSampIndex++;
			sSampTime++;

			/*采样最大序号*/
			if(sSampIndex >= 10)
			{
				sSampIndex = 0;
			}

			/*采样满最大次数*/
			if(sSampTime > 10)
			{
				sSampTime = 10;
			}

			/*计算全部平均值*/
			for(i = 0; i < sSampTime; i++)
			{
				if(sHisVolt[i] > maxVolt)
				{
					maxVolt = sHisVolt[i];
				}
				if(sHisVolt[i] < minVolt)
				{
					minVolt = sHisVolt[i];
				}

				aftVolt += sHisVolt[i];
			}

			/*采样满4次去最大最小值*/
			if(sSampTime >= 4)
			{
				aftVolt = (aftVolt - maxVolt - minVolt) / (sSampTime - 2);
			}
			/*采样不满4次全部平均*/
			else
			{
				aftVolt = aftVolt / sSampTime;
			}
		}
	}

	/*计算结果在粘连范围*/
	if((aftVolt >= (Adhesion_Link2_Vref - Adhesion_Link2_Voff))
		&& (aftVolt <= (Adhesion_Link2_Vref + Adhesion_Link2_Voff)))
	{
		/*最大最小值不在粘连范围*/
		if((minVolt < (Adhesion_Link2_Vref - Adhesion_Link2_Voff - 0.01))
			|| (maxVolt >= (Adhesion_Link2_Vref + Adhesion_Link2_Voff + 0.01)))
		{
			/*认为是数据跳动造成,不报异常*/
			aftVolt = 3.0;
		}
	}

	return(aftVolt);
}

/*=============================================================================================
** @Name      : static void ClrLtc2949SampInfo(void)
** @Input     : void
** @Output    : void
** @Function  : 清除LTC2949高压采样信息
** @The notes : LTC采样失效时调用
**===========================================================================================*/
static void ClrLtc2949SampInfo(void)
{
	/*高压清零*/
	InputGroupHVSideData(HIG_AFE_BatV, 0);
	InputGroupHVSideData(HIG_AFE_PreV, 0);
	//InputGroupHVSideData(HIG_AFE_LoaV, 0);
	InputGroupHVSideData(HIG_AFE_PgndV, 0);
	InputGroupHVSideData(HIG_AFE_Link1V, 0);
	InputGroupHVSideData(HIG_AFE_Link2V, 0);
	InputGroupHVSideData(HIG_AFE_Temp, 0);
	InputGroupHVSideData(HIG_AFE_Vref, 0);
	InputGroupHVSideData(HIG_AFE_Out5V, 0);

	/*分流器电流清零*/
	InputGroupHVSideData(HIG_AFE_Shunt, 0);
	CalcSampleADToCurr(eCAFE_Shunt, CURR_SHUNT_MIN_V);

	/*绝缘检测数据清零*/
	InputInsuSampDataAPI(eInsu_Bat, 0);
	InputInsuSampDataAPI(eInsu_V1, 0);
	InputInsuSampDataAPI(eInsu_V2, 0);

	/*接触器粘连检测数据清零*/
	InputSwhAdhesSampData(eAdhSamp_Link1, 0);
	InputSwhAdhesSampData(eAdhSamp_Link2, 0);

	/*接触器粘连检测步骤复位*/
	SetSwhAdhesDiagFlagAPI(eAdhVlink_Init);

	/*绝缘检测步骤复位*/
	SetInsuVpgndSampFlagAPI(eIsoVpgnd_Init);
}

/*=============================================================================================
** @Name      : static u8 SelectHVNextSampStep(u8 nowStep)
** @Input     : nowStep:当前步骤
** @Output    : 下一步执行步骤
** @Function  : 选择高压采样下一步执行步骤
** @The notes : 判断是否返回电流采样
**===========================================================================================*/
static u8 SelectHVNextSampStep(u8 nowStep)
{
	u8 step = 0;
	static u8 sHisStep = 0;
	static u8 sHvSapTimes = HVLV_SAMP_TIMES;

	/*分流器电流采样使能则交叉采样*/
	if(1 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCAFE_Shunt))
	{
		/*处于电流采样*/
		if(nowStep < LTC2949_STEP_SUMV)
		{
			/*初次采样或其它通道采完*/
			if((0 == sHisStep) || (sHisStep >= LTC2949_STEP_IC5V) || (sHisStep < LTC2949_STEP_SUMV))
			{
				/*进入总压采样*/
				step = LTC2949_STEP_SUMV;
			}
			/*当前电流超过过流极限值则只采集电流和总压*/
			else if((GetGSampOutCurrAPI() >= BATT_EXEREM_MAXC) || (GetGSampOutCurrAPI() <= (0 - (s16)BATT_EXEREM_MAXC)))
			{
				/*返回总压采样*/
				step = LTC2949_STEP_SUMV;
			}
			/*正处于总电压采样(直接进行负载电压采样)*/
			else if(sHisStep < LTC2949_STEP_LODV)
			{
				/*刚上电前5s*/
				if(BSPGetOSMillSecTimeAPI() < 5000)
				{
					/*需要全部采样*/
					sHvSapTimes = HVLV_SAMP_TIMES;
				}
				/*启停或运行时高压内外压差过大*/
				else if((ABS(GetGSampSumVoltAPI(), GetGSampPreVoltAPI()) >= gGBmuHigLevPara_103[eBmuHigLevPara103_SafeSubV])
					&& ((eWORK_RUN == GetGWorkStateAPI())
					|| (eWORK_START == GetGWorkStateAPI())
					|| (eWORK_STOP == GetGWorkStateAPI())))
				{
					/*需要全部采样*/
					sHvSapTimes = HVLV_SAMP_TIMES;
				}
				/*计算高压采样次数*/
				else
				{
					//防止越界
					if(sHvSapTimes < 255)
					{
						sHvSapTimes++;
					}
				}

				/*进入负载电压采样*/
				step = SelectHVLoadVSampStep();
			}
			/*已进行负载采样(判断是否进行接触器粘连电压采样)*/
			else if(sHisStep < LTC2949_STEP_LNKV)
			{
				/*进入接触器粘连检测Link±电压采样*/
				step = SelectHVLinkVSampStep();
			}
			/*已进行Link电压采样(判断是否进行绝缘电压采样)*/
			else if(sHisStep < LTC2949_STEP_ISOV)
			{
				/*进入绝缘检测PGND电压采样*/
				step = SelectHVIsoVSampStep();
			}
			/*已进行绝缘电压采样(不需要进行负母线电压采样)
			else if(sHisStep < LTC2949_STEP_PGND)
			{
				//进入负母线电压PGND电压采样
				step = SelectHVPgndVSampStep();
			}*/
			/*已进行负母线电压(或绝缘电压)采样(判断是否进行分流器温度采样)*/
			else if(sHisStep < LTC2949_STEP_SHUT)
			{
				/*进入分流器温度采样*/
				step = SelectHVShuTSampStep();
			}
			/*已进行分流器温度采样(直接进行基准电压采样)*/
			else if(sHisStep < LTC2949_STEP_REFV)
			{
				/*进入基准电压采样*/
				step = LTC2949_STEP_REFV;
			}
			/*已进行基准电压采样(直接进行5V输出电压采样)*/
			else if(sHisStep < LTC2949_STEP_IC5V)
			{
				/*进入5V输出电压采样*/
				step = LTC2949_STEP_IC5V;
			}
			/*已进行5V输出电压采样(直接返回总压采样)*/
			else
			{
				/*总压采样次数清零*/
				sHvSapTimes = 0;

				/*返回总压采样*/
				step = LTC2949_STEP_SUMV;
			}

			/*采集绝缘负母线电压之后的通道(绝缘之后的通道需要降低频率采集)*/
			if(step >= LTC2949_STEP_SHUT)
			{
				/*正在启动/停机或未达到高压采样次数则返回检测总电压采样*/
				if((eWORK_START == GetGWorkStateAPI())
					|| (eWORK_STOP == GetGWorkStateAPI())
					|| (sHvSapTimes < HVLV_SAMP_TIMES))
				{
					/*采电池总压*/
					step = LTC2949_STEP_SUMV;
				}
			}

			/*记录电流采样之前采样步骤*/
			sHisStep = step;
		}
		/*处于非分流器电流采样*/
		else
		{
			/*返回电流采样*/
			step = LTC2949_STEP_SHUV;
			/*记录电流采样之前采样步骤*/
			sHisStep = nowStep;
		}
	}
	/*没有分流器电流采样则按顺序下一步采样*/
	else
	{
		/*正处于复位初始化完成(直接进行总压采样)*/
		if(nowStep < LTC2949_STEP_SUMV)
		{
			step = LTC2949_STEP_SUMV;
		}
		/*正处于总电压采样(直接进行负载电压采样)*/
		else if(nowStep < LTC2949_STEP_LODV)
		{
			/*刚上电前5s*/
			if(BSPGetOSMillSecTimeAPI() < 5000)
			{
				/*需要全部采样*/
				sHvSapTimes = HVLV_SAMP_TIMES;
			}
			/*启停或运行时高压内外压差过大*/
			else if((ABS(GetGSampSumVoltAPI(), GetGSampPreVoltAPI()) >= gGBmuHigLevPara_103[eBmuHigLevPara103_SafeSubV])
				&& ((eWORK_RUN == GetGWorkStateAPI())
				|| (eWORK_START == GetGWorkStateAPI())
				|| (eWORK_STOP == GetGWorkStateAPI())))
			{
				/*需要全部采样*/
				sHvSapTimes = HVLV_SAMP_TIMES;
			}
			/*计算高压采样次数*/
			else
			{
				//防止越界
				if(sHvSapTimes < 255)
				{
					sHvSapTimes++;
				}
			}

			/*进入负载电压采样*/
			step = SelectHVLoadVSampStep();
		}
		/*正处于负载压采样(判断是否进行接触器粘连电压采样)*/
		else if(nowStep < LTC2949_STEP_LNKV)
		{
			/*进入接触器粘连检测Link±电压采样*/
			step = SelectHVLinkVSampStep();
		}
		/*正处于接触器粘连电压采样(判断是否进行绝缘电压采样)*/
		else if(nowStep < LTC2949_STEP_ISOV)
		{
			/*进入绝缘检测PGND电压采样*/
			step = SelectHVIsoVSampStep();
		}
		/*正处于绝缘电压采样(不需要进行负母线电压采样)
		else if(nowStep < LTC2949_STEP_PGND)
		{
			//进入负母线电压PGND电压采样
			step = SelectHVPgndVSampStep();
		}*/
		/*正处于负母线电压(或绝缘电压)采样(判断是否进行分流器温度采样)*/
		else if(nowStep < LTC2949_STEP_SHUT)
		{
			/*选择分流器温度采样步骤*/
			step = SelectHVShuTSampStep();
		}
		/*正处于分流器温度采样(直接进行基准电压采样)*/
		else if(nowStep < LTC2949_STEP_REFV)
		{
			/*直接进入LTC基准电压采样*/
			step = LTC2949_STEP_REFV;
		}
		/*正处于基准电压采样(直接进行5V输出电压采样)*/
		else if(nowStep < LTC2949_STEP_IC5V)
		{
			/*进入5V输出电压采样*/
			step = LTC2949_STEP_IC5V;
		}
		/*正处于5V输出电压采样(直接返回)*/
		else
		{
			/*总压采样次数清零*/
			sHvSapTimes = 0;

			/*返回电流采样(无电流采样会直接进入总压采样)*/
			step = LTC2949_STEP_SHUV;
		}

		/*采集绝缘负母线电压之后的通道(绝缘之后的通道需要降低频率采集)*/
		if(step >= LTC2949_STEP_SHUT)
		{
			/*正在启动/停机或未达到高压采样次数则返回检测总电压采样*/
			if((eWORK_START == GetGWorkStateAPI())
				|| (eWORK_STOP == GetGWorkStateAPI())
				|| (sHvSapTimes < HVLV_SAMP_TIMES))
			{
				/*采电池总压*/
				step = LTC2949_STEP_SUMV;
			}
		}

		sHisStep = step;
	}

	return(step);
}

/*=============================================================================================
** @Name      : static u8 SelectHVLoadVSampStep(void)
** @Input     : void
** @Output    : 负载电压采样执行步骤
** @Function  : 选择高压采样负载电压采样执行步骤
** @The notes : 未达到采样间隔时间则下一步
**===========================================================================================*/
static u8 SelectHVLoadVSampStep(void)
{
	u8 step = 0;

	/*正在粘连电压采样等待*/
	if(2 == sDelySampWaitState)
	{
		/*进入粘连检测步骤*/
		step = SelectHVLinkVSampStep();
	}
	/*正在绝缘电压采样等待*/
	else if(3 == sDelySampWaitState)
	{
		/*进入绝缘检测步骤*/
		step = SelectHVIsoVSampStep();
	}
	/*已经控制负载电压继电器(Rly1&Rly2闭合)*/
	else if(1 == sLoadVSampState)
	{
		/*到达负载电压采集延时(无外电压间隔时间短[动力线较长时形成电容影响采样,无电压也需要较长时间])*/
		if((BSPGetOSMillSecTimeAPI() >= (sLinkVRlyCtrlT + gGBmuHigLevPara_103[eBmuHigLevPara103_Lk0SampDelyT]))
			/*|| ((GetGDcSideSampRealInfoAPI(eReferSampInfo200_PChgHV) <= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV])
			&& (BSPGetOSMillSecTimeAPI() >= (sLinkVRlyCtrlT + (gGBmuHigLevPara_103[eBmuHigLevPara103_Lk0SampDelyT] / 2))))*/)
		{
			/*进入负载电压读取*/
			step = LTC2949_STEP_LODV + 1;
		}
		/*延时未到不能读取负载电压值*/
		else
		{
			/*进入绝缘检测步骤(不能进入粘连检测[会断开Rly1&Rly2])*/
			//step = SelectHVIsoVSampStep();
			/*进入负母线电压采样步骤(不能进入绝缘检测[会投切继电器干扰负载电压采样值])*/
			step = SelectHVPgndVSampStep();
		}
	}
	/*未控制继电器(Rly1&Rly2未闭合)*/
	else
	{
		/*正等待粘连检测采样(已控制继电器等待延时)*/
		if((eAdhVlink_SampV1 == GetSwhAdhesDiagFlagAPI())
			|| (eAdhVlink_SampV2 == GetSwhAdhesDiagFlagAPI()))
		{
			/*进入粘连检测检测步骤*/
			step = SelectHVLinkVSampStep();
		}
		/*到达继电器控制间隔时间*/
		else if(BSPGetOSMillSecTimeAPI() >= (sLinkVRlyCtrlT + gGBmuHigLevPara_103[eBmuHigLevPara103_RlyCtrlGapT]))
		{
			/*进入负载电压继电器控制:Rly1&Rly2闭合*/
			step = LTC2949_STEP_LODV;
			/*记录link电压继电器控制时间*/
			//sLinkVRlyCtrlT = BSPGetOSMillSecTimeAPI();
		}
		/*未到达继电器控制间隔时间*/
		else
		{
			/*进入绝缘检测步骤*/
			step = SelectHVIsoVSampStep();
		}
	}

	return(step);
}

/*=============================================================================================
** @Name      : static u8 SelectHVLinkVSampStep(void)
** @Input     : void
** @Output    : 粘连检测Link电压采样执行步骤
** @Function  : 选择高压采样粘连检测Link电压采样执行步骤
** @The notes : 未达到采样间隔时间则下一步
**===========================================================================================*/
static u8 SelectHVLinkVSampStep(void)
{
	u8 step = 0;

	/*正在负载电压采样等待(跳过需要等待的检测)*/
	if(1 == sDelySampWaitState)
	{
		/*进入负母线电压采样步骤*/
		step = SelectHVPgndVSampStep();
	}
	/*正在绝缘电压采样等待*/
	else if(3 == sDelySampWaitState)
	{
		/*进入绝缘检测步骤*/
		step = SelectHVIsoVSampStep();
	}
	/*进入Link电压采样需要判断是否请求粘连检测*/
	/*下一步1:需要控制接触器粘连检测Link+通道继电器*/
	else if(eAdhVlink_SeleV1 == GetSwhAdhesDiagFlagAPI())
	{
		/*正在启动或停止时已检测完一遍无粘连(无需再检粘连)*/
		if(((eWORK_START == GetGWorkStateAPI()) || (eWORK_STOP == GetGWorkStateAPI()))
			&& (1 == GetInsuAdhesFinStateAPI()) && (0 == GetSwhAdhesErrFlagAPI()))
		{
			/*进入负母线电压采样步骤*/
			//step = SelectHVPgndVSampStep();
			/*直接返回电池总压流采样(加快总压和预充电压采样)*/
			step = LTC2949_STEP_SUMV;
		}
		/*达到继电器控制间隔时间*/
		else if(BSPGetOSMillSecTimeAPI() >= (sLinkVRlyCtrlT + gGBmuHigLevPara_103[eBmuHigLevPara103_RlyCtrlGapT]))
		{
			/*控制Link+继电器:断开RLY1,闭合RLY2*/
			step = LTC2949_STEP_LNKV;
			/*记录link电压继电器控制时间*/
			//sLinkVRlyCtrlT = BSPGetOSMillSecTimeAPI();
		}
		/*未到达继电器控制间隔时间*/
		else
		{
			/*进入绝缘检测步骤*/
			//step = SelectHVIsoVSampStep();
			/*进入负母线电压采样步骤(不能进入绝缘检测[会投切继电器干扰负载电压采样值])*/
			step = SelectHVPgndVSampStep();
		}
	}
	/*下一步3:需要控制接触器粘连检测Link-通道继电器*/
	else if(eAdhVlink_SeleV2 == GetSwhAdhesDiagFlagAPI())
	{
		/*达到继电器控制间隔时间*/
		if(BSPGetOSMillSecTimeAPI() >= (sLinkVRlyCtrlT + gGBmuHigLevPara_103[eBmuHigLevPara103_RlyCtrlGapT]))
		{
			/*控制Link-继电器:闭合RLY1,断开RLY2*/
			step = LTC2949_STEP_LNKV + 3;
			/*记录link电压继电器控制时间*/
			//sLinkVRlyCtrlT = BSPGetOSMillSecTimeAPI();
		}
		/*未到达继电器控制间隔时间*/
		else
		{
			/*进入绝缘检测步骤*/
			//step = SelectHVIsoVSampStep();
			/*进入负母线电压采样步骤(不能进入绝缘检测[会投切继电器干扰负载电压采样值])*/
			step = SelectHVPgndVSampStep();
		}
	}
	/*下一步2:需要读取接触器粘连检测Link+通道电压*/
	else if(eAdhVlink_SampV1 == GetSwhAdhesDiagFlagAPI())
	{
		/*达到采样间隔延时(无外电压间隔时间短[动力线较长时形成电容影响采样,无电压也需要较长时间])*/
		if((BSPGetOSMillSecTimeAPI() >= (sLinkVRlyCtrlT + gGBmuHigLevPara_103[eBmuHigLevPara103_Lk1SampDelyT]))
			/*|| ((GetGDcSideSampRealInfoAPI(eReferSampInfo200_PChgHV) <= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV])
			&& (BSPGetOSMillSecTimeAPI() >= (sLinkVRlyCtrlT + (gGBmuHigLevPara_103[eBmuHigLevPara103_Lk1SampDelyT] / 2))))*/)
		{
			/*读取Link+电压*/
			step = LTC2949_STEP_LNKV + 1;
		}
		/*未到达读取采样间隔延时*/
		else
		{
			/*进入绝缘检测步骤*/
			//step = SelectHVIsoVSampStep();
			/*进入负母线电压采样步骤(不能进入绝缘检测[会投切继电器干扰负载电压采样值])*/
			step = SelectHVPgndVSampStep();
		}
	}
	/*下一步4:需要读取接触器粘连检测Link-通道电压*/
	else if(eAdhVlink_SampV2 == GetSwhAdhesDiagFlagAPI())
	{
		/*达到采样间隔延时(无外电压间隔时间短[动力线较长时形成电容影响采样,无电压也需要较长时间])*/
		if((BSPGetOSMillSecTimeAPI() >= (sLinkVRlyCtrlT + gGBmuHigLevPara_103[eBmuHigLevPara103_Lk2SampDelyT]))
			/*|| ((GetGDcSideSampRealInfoAPI(eReferSampInfo200_PChgHV) <= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV])
			&& (BSPGetOSMillSecTimeAPI() >= (sLinkVRlyCtrlT + (gGBmuHigLevPara_103[eBmuHigLevPara103_Lk2SampDelyT] / 2))))*/)
		{
			/*读取Link-电压*/
			step = LTC2949_STEP_LNKV + 4;
		}
		/*未到达读取采样间隔延时*/
		else
		{
			/*进入绝缘检测步骤*/
			//step = SelectHVIsoVSampStep();
			/*进入负母线电压采样步骤(不能进入绝缘检测[会投切继电器干扰负载电压采样值])*/
			step = SelectHVPgndVSampStep();
		}
	}
	/*下一步:无需接触器粘连检测Link压采样则判断是否进行绝缘电压采样*/
	else
	{
		/*进入绝缘检测步骤*/
		step = SelectHVIsoVSampStep();
	}

	return(step);
}

/*=============================================================================================
** @Name      : static u8 SelectHVIsoVSampStep(void)
** @Input     : void
** @Output    : 绝缘检测采样执行步骤
** @Function  : 选择高压采样绝缘检测采样执行步骤
** @The notes : 无绝缘检测采样则获取下一步采样
**===========================================================================================*/
static u8 SelectHVIsoVSampStep(void)
{
	u8 step = 0;

	/*正在负载电压采样或粘连检测电压采样等待(跳过需要等待的检测)*/
	if((1 == sDelySampWaitState) || (2 == sDelySampWaitState))
	{
		/*进入负母线电压采样步骤*/
		step = SelectHVPgndVSampStep();
	}
	/*下一步:需要设置绝缘检测继电器*/
	else if((eIsoVpgnd_SeleV1 == GetInsuVpgndSampFlagAPI())
		|| (eIsoVpgnd_SeleV2 == GetInsuVpgndSampFlagAPI()))
	{
		/*正在启动或停止时已检测完一遍无绝缘故障(无需再检绝缘)*/
		if(((eWORK_START == GetGWorkStateAPI()) || (eWORK_STOP == GetGWorkStateAPI()))
			&& (1 == GetInsuAdhesFinStateAPI()) && (0 == GetInsuErrStateFlagAPI()) && (eIsoVpgnd_SeleV1 == GetInsuVpgndSampFlagAPI()))
		{
			/*进入负母线电压采样步骤*/
			//step = SelectHVPgndVSampStep();
			/*直接返回电池总压流采样(加快总压和预充电压采样)*/
			step = LTC2949_STEP_SUMV;
		}
		/*达到继电器控制间隔时间*/
		else if(BSPGetOSMillSecTimeAPI() >= (sIsoVRlyCtrlT + gGBmuHigLevPara_103[eBmuHigLevPara103_RlyCtrlGapT]))
		{
			/*片选V1/V2继电器*/
			step = LTC2949_STEP_ISOV;
			/*记录绝缘电压继电器控制时间*/
			//sIsoVRlyCtrlT = BSPGetOSMillSecTimeAPI();
		}
		/*未到达继电器控制间隔时间*/
		else
		{
			/*进入分流器温度采样*/
			step = SelectHVShuTSampStep();
		}
	}
	/*下一步:直接绝缘检测电压采样 V1或V2*/
	else if((eIsoVpgnd_SampV1 == GetInsuVpgndSampFlagAPI())
		|| (eIsoVpgnd_SampV2 == GetInsuVpgndSampFlagAPI()))
	{
		/*达到采样延时:间隔至少500ms*/
		if(BSPGetOSMillSecTimeAPI() >= (sIsoVRlyCtrlT + gGBmuHigLevPara_103[eBmuHigLevPara103_LedkSampDelyT]))
		{
			/*采PGND电压 V1或V2*/
			step = LTC2949_STEP_ISOV + 1;
		}
		/*未达到采样延时*/
		else
		{
			/*进入分流器温度采样*/
			step = SelectHVShuTSampStep();
		}
	}
	/*下一步:无需绝缘检测则判断是否进行负母线电压采集*/
	else
	{
		/*进入负母线电压采样*/
		step = SelectHVPgndVSampStep();
	}

	return(step);
}

/*=============================================================================================
** @Name      : static u8 SelectHVPgndVSampStep(void)
** @Input     : void
** @Output    : 负母线电压采样执行步骤
** @Function  : 选择高压采样负母线电压采样执行步骤
** @The notes : 无负母线电压采样则获取下一步采样
**===========================================================================================*/
static u8 SelectHVPgndVSampStep(void)
{
	u8 step = 0;
	static u32 sNegVRlySampT = 0;

	/*已绝缘检测投臂不能采负母线电压*/
	if(eIsoVpgnd_SampV2 == GetInsuVpgndSampFlagAPI())
	{
		/*进入分流器温度采样*/
		step = SelectHVShuTSampStep();
	}
	/*达到负母线电压采样间隔周期(超过继电器控制等到时间)*/
	else if((BSPGetOSMillSecTimeAPI() >= (sIsoVRlyCtrlT + gGBmuHigLevPara_103[eBmuHigLevPara103_LedkSampDelyT]))
		&& (BSPGetOSMillSecTimeAPI() >= (sNegVRlySampT + gGBmuHigLevPara_103[eBmuHigLevPara103_PgndVSampGapT])))
	{
		/*采PGND负母线电压(对应绝缘PGND电压V1)*/
		step = LTC2949_STEP_PGND;
		/*记录负母线电压采样时间*/
		sNegVRlySampT = BSPGetOSMillSecTimeAPI();
	}
	/*无需负母线电压采样*/
	else
	{
		/*进入分流器温度采样*/
		step = SelectHVShuTSampStep();
	}

	return(step);
}

/*=============================================================================================
** @Name      : static u8 SelectHVShuTSampStep(void)
** @Input     : void
** @Output    : 分流器温度采样执行步骤
** @Function  : 选择高压采样分流器温度采样执行步骤
** @The notes : 无分流器温度采样则获取下一步采样
**===========================================================================================*/
static u8 SelectHVShuTSampStep(void)
{
	u8 step = 0;
	static u32 sAuxChanSampT = 0;

	/*辅助通道采样周期间隔2s*/
	if((0 == sAuxChanSampT) || (BSPGetOSMillSecTimeAPI() >= (sAuxChanSampT + 2000)))
	{
		/*使能分流器温度采样*/
		#if(1 == SHUNT_T_SAMP_EN)
		/*直接进入分流器温度采样*/
		step = LTC2949_STEP_SHUT;
		#else
		/*直接进入LTC基准电压采样*/
		step = LTC2949_STEP_REFV;
		#endif

		sAuxChanSampT = BSPGetOSMillSecTimeAPI();
	}
	else
	{
		/*直接返回分流器电流采样(增加电流采样周期[约是总压2倍])*/
		step = LTC2949_STEP_SHUV;
		/*直接返回电池总压流采样(增加总压采样周期[约与总压相同])*/
		//step = LTC2949_STEP_SUMV;
	}

	return(step);
}

/*=============================================================================================
** @Name      : static void SampCtrlHVLinkVRelyState(u8 state)
** @Input     : 控制状态 0:全断开 1:rly1断开rly2闭合(Link+V) 2:rly1闭合rly2断开(Link-V) 3:全闭合(采负载V)
** @Output    : void
** @Function  : 控制Link电压采样继电器(Rly1/Rly2)状态
** @The notes :
**===========================================================================================*/
static void SampCtrlHVLinkVRelyState(u8 state)
{
	/*全断开*/
	if(0 == state)
	{
		/*断开RLY1/RLY2*/
		Ltc2949GpioSetOutPutState(AFE_IOCTRL_Link1, 0); /*断开LINK+电压继电器，RLY2*/
		Ltc2949GpioSetOutPutState(AFE_IOCTRL_Link2, 0);	/*断开LINK-电压继电器，RLY1*/
		sLoadVSampState = 0;                            /*记录未控制负载电压继电器*/
	}
	else if(1 == state)
	{
		/*断开RLY1/闭合RLY2采集Link+电压*/
		Ltc2949GpioSetOutPutState(AFE_IOCTRL_Link1, 1); /*闭合LINK+电压继电器，RLY2*/
		Ltc2949GpioSetOutPutState(AFE_IOCTRL_Link2, 0); /*断开LINK-电压继电器，RLY1*/
		sLoadVSampState = 0;                            /*记录未控制负载电压继电器*/
	}
	else if(2 == state)
	{
		/*闭合RLY1/断开RLY2采集Link-电压*/
		Ltc2949GpioSetOutPutState(AFE_IOCTRL_Link1, 0); /*断开LINK+电压继电器，RLY2*/
		Ltc2949GpioSetOutPutState(AFE_IOCTRL_Link2, 1); /*闭合LINK-电压继电器，RLY1*/
		sLoadVSampState = 0;                            /*记录未控制负载电压继电器*/
	}
	else if(3 == state)
	{
		/*闭合RLY1/RLY2采集负载电压*/
		Ltc2949GpioSetOutPutState(AFE_IOCTRL_Link1, 1); /*闭合LINK+电压继电器，RLY2*/
		Ltc2949GpioSetOutPutState(AFE_IOCTRL_Link2, 1); /*闭合LINK-电压继电器，RLY1*/
		sLoadVSampState = 1;                            /*记录已控制负载电压继电器*/
	}

	/*记录link电压继电器控制时间*/
	sLinkVRlyCtrlT = BSPGetOSMillSecTimeAPI();
}

/*=============================================================================================
** @Name      : static void SampCtrlHVIsoVRelyState(u8 state)
** @Input     : 控制状态 0:全断开 1:PE闭合rly9断开(IsoV1) 2:PE闭合rly9闭合(IsoV2)
** @Output    : void
** @Function  : 控制绝缘检测PGND电压采样继电器(PE[RLY6/RLY7]/Rly9)状态
** @The notes :
**===========================================================================================*/
static void SampCtrlHVIsoVRelyState(u8 state)
{
	/*全断开*/
	if(0 == state)
	{
		/*断开绝缘PE*/
		DEVGPIOSetPortState(ePOut_ISO_PGND, ePSTATE_OFF);  /*断开绝缘检测继电器PE(RLY6/RLY7)*/
		Ltc2949GpioSetOutPutState(AFE_IOCTRL_ISO, 0);	   /*控制RLY9断开*/
	}
	/*无桥臂*/
	else if(1 == state)
	{
		/*采集绝缘V1*/
		DEVGPIOSetPortState(ePOut_ISO_PGND, ePSTATE_ON);   /*闭合绝缘检测继电器PE(RLY6/RLY7)*/
		Ltc2949GpioSetOutPutState(AFE_IOCTRL_ISO, 0);	   /*控制RLY9断开*/
	}
	/*投入桥臂*/
	else if(2 == state)
	{
		/*采集绝缘V2*/
		DEVGPIOSetPortState(ePOut_ISO_PGND, ePSTATE_ON);   /*闭合绝缘检测继电器PE(RLY6/RLY7)*/
		Ltc2949GpioSetOutPutState(AFE_IOCTRL_ISO, 1);	   /*控制RLY9闭合*/
	}

	/*记录绝缘电压继电器控制时间*/
	sIsoVRlyCtrlT = BSPGetOSMillSecTimeAPI();
}

/*=============================================================================================
** @Name      : static u8 SearchTempTabByResist(u32 data)
** @Input     : data:要查表的数据阻值 1Ω
** @Output    : 对应温度下标值
** @Function  : 通过NTC阻值查询温度表得到对应温度值
** @The notes :
**===========================================================================================*/
static u8 SearchTempTabByResist(u32 data)
{
    u8 i = 0;

	/*断线阻值很大(-41℃)*/
    if(data >= sShuntNTCTempTabR[0])
    {
    	return(0);
    }
	/*非断线(-40℃以上)*/
    else
    {
    	/*从-40℃开始检测*/
		for(i = 1; i < SHUNT_T_TAB_LEN; i++)
		{
			/*达到当前阻值(阻值表降序)*/
			if(data >= sShuntNTCTempTabR[i])
			{
				/*判断与前一个值的中间值*/
				if(data <= ((sShuntNTCTempTabR[i] + sShuntNTCTempTabR[i - 1]) / 2))
				{
					/*没有超过中间值取当前值*/
					return(i);
				}
				/*超过中间值说明偏大取前一个更大阻值*/
				else
				{
					return(i - 1);
				}
			}
		}
    }

	/*阻值很小低于表中阻值*/
    return(SHUNT_T_TAB_LEN);
}


#if(1 == SHUNT_V_COMP_EN)/*分流器温度补偿电压使能*/
/*=============================================================================================
** @Name      : static s16 GetShuntTCompVolt(void)
** @Input     : void
** @Output    : 当前温度对应补偿电压值 0.01mV
** @Function  : 通过当前温度计算分流器对应补偿电压值
** @The notes : 查表计算
**===========================================================================================*/
static s16 CalcShuntTCompVolt(void)
{
    u8 i = 0;
    s16 temp = 0;
    s16 volt = 0;

    /*温度采样值正常*/
    if(GetGShuntSampTempAPI() > -41)
    {
    	temp = GetGShuntSampTempAPI();
    }
    /*温度采样值异常*/
    else
    {
    	return(0);
    }

    /*高于表中温度*/
    if(temp >= sShuntCompTempTab[SHUNT_V_TAB_LEN])
    {
    	volt = sShuntCompVoltTab[SHUNT_V_TAB_LEN];
    }
    else
	{
		for(i = 0; i < SHUNT_V_TAB_LEN; i++)
		{
			/*与表中温度一致*/
			if(temp == sShuntCompTempTab[i])
			{
				/*取表中补偿值*/
				volt = sShuntCompVoltTab[i];
			}
			else if(temp < sShuntCompTempTab[i])
			{
				/*低于表中温度*/
				if(0 == i)
				{
					/*取最低温补偿值*/
					volt = sShuntCompVoltTab[0];
				}
				/*处于表中温度中间*/
				else
				{
					/*按比率计算*/
					volt = (s32)(sShuntCompVoltTab[i] - sShuntCompVoltTab[i - 1]) * (temp - sShuntCompTempTab[i - 1]) / (sShuntCompTempTab[i] - sShuntCompTempTab[i - 1]);
				}
			}
		}
	}

    return(volt);
}
#endif
