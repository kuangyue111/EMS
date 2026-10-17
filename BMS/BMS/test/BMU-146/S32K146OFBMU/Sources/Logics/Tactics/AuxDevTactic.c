/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : AuxDevTactic.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 辅助设备控制策略模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "ParaUser.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "DiagSos.h"
#include "DiagInterface.h"
#include "TacInterface.h"
#include "MbInterface.h"
#include "HeatModbusMap.h"
#include "EventUser.h"
#include "AuxDevMsg.h"
#include "AuxDevTactic.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gGImeuRcvInfo_204[];			/*IMEU辅助板接收信息*/
extern s8 gGImeuTempInfo_205[];			/*IMEU辅助板温湿度信息*/

extern u16 gGBmuGenPara_102[];			/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];		/*主控高级参数值*/
extern u8 gGHardPara_104[]; 			/*主控硬件参数配置信息*/

#if(((PRJ_PARA_NUM_INFO > 0) && (1 == BMU_PWM_FAN_EN)) || (1 == BMU_IMEU_COM_EN))/*使能IMLU1或IMEU2通信交互*/
static s16 sAvgCurr1min = 0;			/*1分钟平均电流 0.1A*/
#endif

#if((PRJ_PARA_NUM_INFO > 0) && (1 == BMU_PWM_FAN_EN))//非测试工装程序是RS485控制IMLU1调速风扇(由IMLU1控制PWM调速风扇时)
static u16 sFanCtrlDuty[BMU_PWM_FAN_NUM] = {0};/*风扇转速控制PWM占空比[0,100] 1%*/

#if(IMLU_FAN_BACK_TYPE > 0)				/*需要检测风扇告警*/
static u16 sFanAlarmState = 0;			/*风扇告警状态位*/
#endif
#endif

#if(1 == BMU_IMEU_COM_EN)				/*使能IMEU2通信交互*/
#if(1 == BMU_EU_SHUTFAN_EN)				/*使能控制百叶风机*/
static u8 gAuxShutUserCmd = 0;			/*辅助设备百叶风机用户控制命令 0:关闭 1:开启*/
#endif
#if(1 == BMU_EU_FIRELIG_EN)				/*使能控制照明声光报警消防等*/
static u8 gAuxRackLightCmd = 0;			/*电池柜照明用户强制控制命令 0:自动控制 1:手动开启  2:手动关闭*/
static u8 gAuxZHSAForceCmd = 0;			/*声光报警器用户强制控制命令 0:自动控制 1:强制开启 2:强制关闭*/
static u8 gAuxFireTrigCmd = 0;			/*气溶胶消防用户触发控制命令 0:自动触发 1:手动触发*/
#endif
static u16 gAuxDevErrFlag = 0;			/*辅助设备控制异常标志 bit0:开启百叶异常 bit1:开启风机异常 bit2:空调告警 bit8:关闭百叶异常 bit9:关闭风机异常 bit10:空调通信中断*/
static u8 gAuxComErrFlag = 0;			/*辅助设备通信异常标志 bit0:空调 bit1:温湿度传感器1 bit2:温湿度传感器2 bit3:温湿度传感器3 bit4:气体传感器1 bit5:气体传感器2 bit6:气体传感器3*/

static u8 sAuxDevAirType = 0;			/*空调类型 0:黑盾   1:同飞*/

static u8 sRcvEu2NewDevAddr = 0;        /*接收IMEU2设备地址[接收到多个时为0xff](IMEU2唯一时用于同步BMU设备地址(IMEU2有拨码方便编址))*/
#endif

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
#if((PRJ_PARA_NUM_INFO > 0) && (1 == BMU_PWM_FAN_EN))//非测试工装程序是RS485控制IMLU1调速风扇(由IMLU1控制PWM调速风扇时)
/*=================================================================================================
** @Name      : static void TacGroupFanCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池箱热管理调速风扇控制策略任务
** @The notes : 1s,通过RS485通信控制IMLU1执行
**===============================================================================================*/
static void TacGroupFanCtrlTask(void);
#endif

#if(1 == BMU_IMEU_COM_EN)/*使能IMEU2通信交互*/
#if(1 == BMU_EU_SHUTFAN_EN)/*使能控制百叶风机*/
/*=============================================================================================
** @Name      : static void TacShutFanCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 百叶排风机通风控制策略任务
** @The notes : 1s任务,通过CAN通信控制IMEU2执行
**===========================================================================================*/
static void TacShutFanCtrlTask(void);
#endif

#if(1 == BMU_EU_FIRELIG_EN)/*使能控制照明/声光报警/消防等*/
/*=============================================================================================
** @Name      : static void TacRackLightCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池柜照明控制策略任务
** @The notes : 1s任务,通过CAN通信控制IMEU2执行
**===========================================================================================*/
static void TacRackLightCtrlTask(void);

/*=============================================================================================
** @Name      : static void TacFireAlarmCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 消防控制器和声光报警控制器控制策略任务
** @The notes : 500ms任务,通过CAN通信控制IMEU2执行
**===========================================================================================*/
static void TacFireAlarmCtrlTask(void);
#endif

/*=============================================================================================
** @Name      : static void TacAirTHumManagTask(void)
** @Input     : void
** @Output    : void
** @Function  : 辅助板空调温湿度管理任务
** @The notes : 1s任务,通过CAN通信控制IMEU2执行
**===========================================================================================*/
static void TacAirTHumManagTask(void);

/*=============================================================================================
** @Name      : static u8 TacCheckAirOnTempNorState(u8 state)
** @Input     : state: 0:制冷状态 1:制热状态
** @Output    : TRUE:正常 FALSE:异常
** @Function  : 判断需要控制空调开启制冷制热时温度是否正常
** @The notes : 检查空调温度和电池温度
**===========================================================================================*/
static u8 TacCheckAirOnTempNorState(u8 state);

/*=============================================================================================
** @Name      : static u8 TacCheckSensorTempNorState(u8 id)
** @Input     : id:温湿度传感器序号[0,1]
** @Output    : TRUE:有效 FALSE:无效
** @Function  : 判断温湿度传感器温度是否有效
** @The notes : 与空调温度对比
**===========================================================================================*/
//static u8 TacCheckSensorTempNorState(u8 id);

/*=============================================================================================
** @Name      : static u8 TacCheckOutETempHighState(void)
** @Input     : void
** @Output    : TRUE:外温较高 FALSE:外温低
** @Function  : 判断柜外/柜壁温度是否较高
** @The notes : 与空调停止制冷温度对比
**===========================================================================================*/
static u8 TacCheckOutETempHighState(void);
#endif

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void TacAuxDevManagTask(void)
** @Input     : void
** @Output    : void
** @Function  : 辅助板管理任务
** @The notes : 500ms驱动1s周期
**===========================================================================================*/
void TacAuxDevManagTask(void)
{
	/*使能IMLU1或IMEU2通信交互*/
	#if(((PRJ_PARA_NUM_INFO > 0) && (1 == BMU_PWM_FAN_EN)) || (1 == BMU_IMEU_COM_EN))
	static u8 sState = 0;
	static u8 sAvgCurrNum = 0;
	static s32 sSumCurr1min = 0;
	#endif

	/*使能IMEU2通信交互*/
	#if(1 == BMU_IMEU_COM_EN)
	u8 i = 0;
	u8 euOffAddr = 0;
	static u8 sTimer1S = 0;
	static u8 sAddrTime = 0;
	#endif

	/*使能IMEU2通信交互*/
	#if(1 == BMU_IMEU_COM_EN)
	//1s间隔判断外设是否通信中断
	if(0 == (sState % 2))
	{
		sTimer1S++;
		/*上电前30S不检测外设通讯状态，IMEU2上电初始化上报所有外设通讯设备通讯中断告警和空调故障位置1*/
		if(sTimer1S >= 30)
		{
			//防止计数越界
			if(sTimer1S > 0xf0)
			{
				sTimer1S = 0xf0;
			}

			//检测空调通信中断
			if(1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_AirCom))
			{
				//刚发生记录事件
				if(0 == BitGet(gAuxComErrFlag, eAuxComErr_Air))
				{
					DiagUserSelfErrRecord(eUDef_ErrRcd_ArComErr, eERR_RCDSORT_WARSET, 0,  0);
				}
				//记录空调通信中断
				BitSet(gAuxComErrFlag, eAuxComErr_Air);
				BitSet(gAuxDevErrFlag, eAuxErr_AirCom);
			}
			else
			{
				//清除空调通信中断
				BitClr(gAuxComErrFlag, eAuxComErr_Air);
				BitClr(gAuxDevErrFlag, eAuxErr_AirCom);
			}

			//检测3个温湿度传感器和3个气体检测传感器
			for(i = 0; i < 3; i++)
			{
				//温湿度传感器通信中断
				if(1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), (eEU204DI_THSen1 + i)))
				{
					//刚发生记录事件
					if(0 == BitGet(gAuxComErrFlag, (eAuxComErr_TH1 + i)))
					{
						DiagUserSelfErrRecord(eUDef_ErrRcd_THComErr, eERR_RCDSORT_WARSET, 0,  0);
					}
					//记录温湿度传感器通信中断
					BitSet(gAuxComErrFlag, (eAuxComErr_TH1 + i));
				}
				else
				{
					//清除温湿度传感器通信中断
					BitClr(gAuxComErrFlag, (eAuxComErr_TH1 + i));
				}

				//气体传感器通信中断
				if(1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenCommu), (eEU204Alarm_Gas1 + i)))
				{
					//刚发生记录事件
					if(0 == BitGet(gAuxComErrFlag, (eAuxComErr_Gas1 + i)))
					{
						DiagUserSelfErrRecord(eUDef_ErrRcd_GsComErr, eERR_RCDSORT_WARSET, 0,  0);
					}
					//记录气体传感器通信中断
					BitSet(gAuxComErrFlag, (eAuxComErr_Gas1 + i));
				}
				else
				{
					//清除气体传感器通信中断
					BitClr(gAuxComErrFlag, (eAuxComErr_Gas1 + i));
				}
			}
		}

		//上电前1min BMU根据IMEU2地址进行同步BMU设备地址
		if(sTimer1S <= 60)
		{
			//等待5s
			if(sTimer1S >= 5)
			{
				//接收IMEU2设备地址有效且唯一
				if((sRcvEu2NewDevAddr >= IMEU_BAS_DEVADDR) && (sRcvEu2NewDevAddr < BMU_BAS_DEVADDR))
				{
					sAddrTime++;
					//等待5s无其它地址
					if(sAddrTime >= 5)
					{
						//地址偏移量
						euOffAddr = sRcvEu2NewDevAddr - IMEU_BAS_DEVADDR;

						//IMEU2地址偏移量与组号不一致
						if(euOffAddr != GetSysSelfGroupIdAPI())
						{
							//设置BMU设备地址
							ParaGroupSelfAddrCfgAPI((BMU_BAS_DEVADDR + euOffAddr), eCMDPORT_INTE);
						}
					}
				}
				else
				{
					sAddrTime = 0;
				}
			}
			else
			{
				sAddrTime = 0;
			}
		}
	}

	/*无IMEU2时*/
	#else
	/*模拟接收到IMEU2通信报文(防止上报IMEU2通信故障)*/
	DiagRcvDevMsgAddAPI(IMEU_BAS_DEVADDR + GetGroupOffsetDevAddrAPI());
	#endif

    //非测试工装程序是RS485控制IMLU1调速风扇(由IMLU1控制PWM调速风扇时)
	#if((PRJ_PARA_NUM_INFO > 0) && (1 == BMU_PWM_FAN_EN))
	//1s周期执行电池箱调速风扇任务
	if(0 == (sState % 2))
	{
		//电池箱调速风扇策略(包括调试模式处理)
		TacGroupFanCtrlTask();
	}
	/*无IMLU1时*/
	#else
	/*模拟接收到IMLU1通信报文(防止上报IMLU1通信故障)*/
	DiagRcvDevMsgAddAPI(IMLU_BAS_DEVADDR);
	#endif

	/*使能IMEU2通信交互*/
	#if(1 == BMU_IMEU_COM_EN)
	/*系统调试模式仅发送百叶手动控制命令*/
    if(eMODE_DEBUG == GetGWorkModeAPI())
    {
    	//间隔5s发送控制命令
    	if(sState >= 20)
    	{
    		/*使能控制百叶风机*/
    		#if(1 == BMU_EU_SHUTFAN_EN)
    		//调试模式发送控制百叶风机(IMEU2根据该帧判断通信状态)
			SendCanImeu2SelfCtrlMsg220(0, gAuxShutUserCmd);
			#endif
    		sState = 11;
    	}
    	//调试模式用特殊值
    	else if(sState <= 10)
    	{
    		sState = 11;
    	}
    	else
    	{
        	/*使能控制照明/声光报警/消防等*/
        	#if(1 == BMU_EU_FIRELIG_EN)
    		//调试模式发送控制IMEU2外设
    		if(13 == sState)
    		{
        		//调试模式发送控制电池柜照明
    			if(1 == gAuxRackLightCmd)
    			{
    				SendCanImeu2SelfCtrlMsg220(1, 1);
    			}
    			else
    			{
    				SendCanImeu2SelfCtrlMsg220(1, 0);
    			}
    		}
    		else if(15 == sState)
    		{
        		//调试模式发送控制电池柜声光报警
    			if(1 == gAuxZHSAForceCmd)
    			{
    				SendCanImeu2SelfCtrlMsg220(2, 1);
    			}
    			else
    			{
    				SendCanImeu2SelfCtrlMsg220(2, 0);
    			}
    		}
    		else if(17 == sState)
    		{
				//必须处于电池柜开门状态才能调试触发消防(防止误触发)
    			if(1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_Door))
    			{
    				//调试模式发送控制电池柜气溶胶消防触发
    				SendCanImeu2SelfCtrlMsg220(3, gAuxFireTrigCmd);
    			}
    			else
    			{
    				gAuxFireTrigCmd = 0;
    			}
    		}
    		#endif

    		sState++;
    	}
        return;
    }

	//刚从调试模式退出
	if(sState >= 10)
	{
		/*使能控制百叶风机*/
		#if(1 == BMU_EU_SHUTFAN_EN)
		//清除调试IMEU2百叶风机状态
		gAuxShutUserCmd = 0;
		#endif
		//清除平均电流
		sAvgCurr1min = GetGSampOutCurrAPI();
		sSumCurr1min = 0;
		sAvgCurrNum = 0;
		sState = 0;
	}
	#endif

	/*使能IMLU1或IMEU2通信交互*/
	#if(((PRJ_PARA_NUM_INFO > 0) && (1 == BMU_PWM_FAN_EN)) || (1 == BMU_IMEU_COM_EN))
	/*使能IMEU2通信交互控制照明/声光报警/消防等*/
	#if(((1 == BMU_IMEU_COM_EN)) && (1 == BMU_EU_FIRELIG_EN))
	/*控制消防控制器和声光报警器*/
	TacFireAlarmCtrlTask();
	#endif

	/*转换成1s任务*/
    if(0 == sState)
    {
    	/*使能IMEU2通信交互*/
    	#if(1 == BMU_IMEU_COM_EN)
    	/*使能控制百叶风机*/
		#if(1 == BMU_EU_SHUTFAN_EN)
    	//百叶排风机通风管理
    	TacShutFanCtrlTask();
		#endif
    	/*使能控制照明/声光报警/消防等*/
    	#if(1 == BMU_EU_FIRELIG_EN)
    	//电池柜照明管理
    	TacRackLightCtrlTask();
    	#endif
		#endif
    	sState = 1;
    }
    else
    {
		//累计1.5分钟电流
		sSumCurr1min += GetGSampOutCurrAPI();
		sAvgCurrNum++;

		//求1.5分钟平均电流
		if(sAvgCurrNum >= 90)
		{
			sAvgCurr1min = (s16)(sSumCurr1min / sAvgCurrNum);
			sSumCurr1min = 0;
			sAvgCurrNum = 0;
		}
		//充放电直接切换
		else if(((sAvgCurr1min >= 200) && (GetGSampOutCurrAPI() <= -200))
			|| ((sAvgCurr1min <= -200) && (GetGSampOutCurrAPI() >= 200)))
		{
			sAvgCurr1min = GetGSampOutCurrAPI();
			sSumCurr1min = 0;
			sAvgCurrNum = 0;
		}

    	/*使能IMEU2通信交互*/
    	#if(1 == BMU_IMEU_COM_EN)
    	//空调温湿度管理
    	TacAirTHumManagTask();
		#endif
    	sState = 0;
    }
    #endif
}

/*使能IMEU2通信交互*/
#if(1 == BMU_IMEU_COM_EN)
/*=============================================================================================
** @Name      : void TacSetAuxDevAirTypeAPI(u8 type)
** @Input     : flag:空调类型 0:黑盾 1:同飞
** @Output    : void
** @Function  : 设置空调类型
** @The notes :
**===========================================================================================*/
void TacSetAuxDevAirTypeAPI(u8 type)
{
	if(type > 1)
	{
		return;
	}

	sAuxDevAirType = type;
}

/*=================================================================================================
** @Name      : void TacRcvImeuMsgAddAPI(u8 addr)
** @Input     : addr:设备地址
** @Output    : void
** @Function  : 接收IMEU2通信信息地址数目计数
** @The notes : 接收到IMEU2报文时调用
**===============================================================================================*/
void TacRcvImeuMsgAddAPI(u8 addr)
{
	/*IMEU2地址范围*/
    if((addr >= IMEU_BAS_DEVADDR) && (addr < BMU_BAS_DEVADDR))
    {
    	/*首次接收同组IMEU2地址*/
    	if(/*(addr == (IMEU_BAS_DEVADDR + GetSysSelfGroupIdAPI())) && */(0 == sRcvEu2NewDevAddr))
    	{
    		/*记录已接收到IMEU2地址*/
    		sRcvEu2NewDevAddr = 1;
    	}
    	/*首次接收到非同组IMEU2新地址*/
    	else if((addr != (IMEU_BAS_DEVADDR + GetSysSelfGroupIdAPI())) && (1 == sRcvEu2NewDevAddr))
    	{
    		/*记录IMEU2地址*/
    		sRcvEu2NewDevAddr = addr;
    	}

    	/*已接收新地址，本次地址不同*/
    	if((sRcvEu2NewDevAddr != addr) && (sRcvEu2NewDevAddr >= IMEU_BAS_DEVADDR) && (sRcvEu2NewDevAddr < BMU_BAS_DEVADDR))
    	{
    		/*记录多个IMEU2*/
    		sRcvEu2NewDevAddr = 0xff;
    	}
    }
}
#endif

/*=============================================================================================
** @Name      : void TacSetPwmFanUserCmdAPI(u8 id, u8 duty)
** @Input     : id:[0,15]:风扇ID(电池箱序号),0xff:所有风扇  duty:控制转速占空比[0,100]1%
** @Output    : void
** @Function  : 设置电池箱调速风扇用户控制命令
** @The notes : 1个电池箱1路风扇(IMLU1控制)
**===========================================================================================*/
void TacSetPwmFanUserCmdAPI(u8 id, u8 duty)
{
	/*非测试工装程序是RS485控制IMLU1调速风扇(由IMLU1控制PWM调速风扇时)*/
	#if((PRJ_PARA_NUM_INFO > 0) && (1 == BMU_PWM_FAN_EN))
	u8 i = 0;

	if(duty > 100)
	{
		duty = 100;
	}

	if((id < BMU_PWM_FAN_NUM) && (id < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))
	{
		//电池箱序号对应指定PWM通道号(电池柜插框位号)
		//sFanCtrlDuty[id] = duty;
		if((gGHardPara_104[eGHardPara104_PackFanPwm0 + id] >= 1)
			&& (gGHardPara_104[eGHardPara104_PackFanPwm0 + id] <= BMU_PWM_FAN_NUM))
		{
			sFanCtrlDuty[gGHardPara_104[eGHardPara104_PackFanPwm0 + id] - 1] = duty;
		}
	}
	else if(0xff == id)
	{
		for(i = 0; ((i < BMU_PWM_FAN_NUM) && (i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])); i++)
		{
			sFanCtrlDuty[i] = duty;
		}
	}
	#endif
}

/*=============================================================================================
** @Name      : void TacSetAuxShutUserCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 百叶风机用户控制命令(0:关闭 1:开启)
** @Function  : 设置百叶风机用户控制命令
** @The notes : IMEU2控制
**===========================================================================================*/
void TacSetAuxShutUserCmdAPI(u8 cmd)
{
	/*使能IMEU2通信交互控制百叶风机*/
	#if((1 == BMU_IMEU_COM_EN) && (1 == BMU_EU_SHUTFAN_EN))
	if(cmd <= 1)
	{
		gAuxShutUserCmd = cmd;
	}
	#endif
}

/*=============================================================================================
** @Name      : void TacSetAuxRackLightCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 电池柜照明用户控制命令( 0:自动控制 1:手动开启   2:手动关闭)
** @Function  : 设置电池柜照明用户强制控制命令
** @The notes : IMEU2控制
**===========================================================================================*/
void TacSetAuxRackLightCmdAPI(u8 cmd)
{
	/*使能IMEU2通信交互控制声光报警*/
	#if((1 == BMU_IMEU_COM_EN) && (1 == BMU_EU_FIRELIG_EN))
	if(cmd <= 2)
	{
		gAuxRackLightCmd = cmd;
	}
	#endif
}

/*=============================================================================================
** @Name      : void TacSetAuxZHSAForceCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 声光报警器用户控制命令( 0:自动控制 1:强制开启 2:强制关闭)
** @Function  : 设置声光报警器用户强制控制命令
** @The notes : IMEU2控制
**===========================================================================================*/
void TacSetAuxZHSAForceCmdAPI(u8 cmd)
{
	/*使能IMEU2通信交互控制声光报警*/
	#if((1 == BMU_IMEU_COM_EN) && (1 == BMU_EU_FIRELIG_EN))
	if(cmd <= 2)
	{
		gAuxZHSAForceCmd = cmd;
	}
	#endif
}

/*=============================================================================================
** @Name      : void TacSetAuxFireTrigCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 气溶胶消防器用户触发命令( 0:自动触发 1:手动触发)
** @Function  : 设置气溶胶消防器用户触发命令
** @The notes : IMEU2控制
**===========================================================================================*/
void TacSetAuxFireTrigCmdAPI(u8 cmd)
{
	/*使能IMEU2通信交互控制百叶风机*/
	#if((1 == BMU_IMEU_COM_EN) && (1 == BMU_EU_FIRELIG_EN))
	if(cmd <= 1)
	{
		gAuxFireTrigCmd = cmd;
	}
	#endif
}

/*=============================================================================================
** @Name      : u8 TacGetAuxDevAirTypeAPI(void)
** @Input     : void
** @Output    : 电池柜空调类型:0黑盾 1同飞
** @Function  : 获取空调类型
** @The notes : IMEU2控制设备
**===========================================================================================*/
u8 TacGetAuxDevAirTypeAPI(void)
{
	/*使能IMEU2控制*/
	#if(1 == BMU_IMEU_COM_EN)
	return(sAuxDevAirType);
	#else
	return(0);
	#endif
}

/*=============================================================================================
** @Name      : u8 TacGetPwmFanCtrlDutyAPI(u8 id)
** @Input     : id:[0,15]:风扇ID(电池箱序号)
** @Output    : 当前控制转速占空比[0,100]1%
** @Function  : 获取电池箱调速风扇当前控制转速占空比
** @The notes : 1个电池箱1路风扇(IMLU1控制)
**===========================================================================================*/
u8 TacGetPwmFanCtrlDutyAPI(u8 id)
{
	/*非测试工装程序是RS485控制IMLU1调速风扇(由IMLU1控制PWM调速风扇时)*/
	#if((PRJ_PARA_NUM_INFO > 0) && (1 == BMU_PWM_FAN_EN))
	if((id < BMU_PWM_FAN_NUM) && (id < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))
	{
		//电池箱序号对应指定PWM通道号(电池柜插框位号)
		//return(sFanCtrlDuty[id];
		if((gGHardPara_104[eGHardPara104_PackFanPwm0 + id] >= 1)
			&& (gGHardPara_104[eGHardPara104_PackFanPwm0 + id] <= BMU_PWM_FAN_NUM))
		{
			return(sFanCtrlDuty[gGHardPara_104[eGHardPara104_PackFanPwm0 + id] - 1]);
		}
	}
	#endif

	return(0);
}

/*=================================================================================================
** @Name      : u16 TacGetPwmFanErrFlagAPI(void)
** @Input     : void
** @Output    : 调速风扇回检异常标志(按bit位表示1路电池箱风扇,非风扇通道号)
** @Function  : 获取电池箱调速风扇回检异常标志
** @The notes : 1个电池箱1路风扇,与电池箱连续序号对应(IMLU1控制)
**===============================================================================================*/
u16 TacGetPwmFanErrFlagAPI(void)
{
	/*需要检测风扇告警*/
	#if((PRJ_PARA_NUM_INFO > 0) && (1 == BMU_PWM_FAN_EN) && (IMLU_FAN_BACK_TYPE > 0))
	return(sFanAlarmState);
	#else
	return(0);
	#endif
}

/*=============================================================================================
** @Name      : u16 TacGetAuxDevErrFlagAPI(void)
** @Input     : void
** @Output    : 辅助设备异常标志(bit0:开启百叶异常 bit1:开启风机异常 bit2:空调告警 bit8:关闭百叶异常 bit9:关闭风机异常 bit10:空调通信中断)
** @Function  : 获取辅助设备异常标志
** @The notes : IMEU2控制设备
**===========================================================================================*/
u16 TacGetAuxDevErrFlagAPI(void)
{
	/*使能IMEU2控制*/
	#if(1 == BMU_IMEU_COM_EN)
	/*使能控制百叶风机*/
	#if(1 == BMU_EU_SHUTFAN_EN)
	return(gAuxDevErrFlag);
	#else
	return(gAuxDevErrFlag & 0x404);
	#endif
	#else
	return(0);
	#endif
}

/*=============================================================================================
** @Name      : u8 TacGetAuxComErrFlagAPI(void)
** @Input     : void
** @Output    : 辅助设备通信异常标志 bit0:空调 bit1:温湿度传感器1 bit2:温湿度传感器1 bit3:温湿度传感器 bit4:气体传感器1 bit5:气体传感器2 bit6:气体传感器3
** @Function  : 获取辅助设备通讯异常标志
** @The notes : IMEU2控制设备
**===========================================================================================*/
u8 TacGetAuxComErrFlagAPI(void)
{
	/*使能IMEU2控制*/
	#if(1 == BMU_IMEU_COM_EN)
	return(gAuxComErrFlag);
	#else
	return(0);
	#endif
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
#if((PRJ_PARA_NUM_INFO > 0) && (1 == BMU_PWM_FAN_EN))//非测试工装程序是RS485控制IMLU1调速风扇(由IMLU1控制PWM调速风扇时)
/*=================================================================================================
** @Name      : static void TacGroupFanCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池箱热管理调速风扇控制策略任务
** @The notes : 1s,通过RS485通信控制IMLU1执行
**===============================================================================================*/
static void TacGroupFanCtrlTask(void)
{
	u8 i = 0;
	u8 k = 0;

	u8 cell = 0;
	u8 close = 0;
	u8 minSpeed = 0;
	u8 maxSpeed = 0;
	s8 boxMaxCellT = 0;           				/*电池箱内最大温度值*/
	s8 boxMinCellT = 0;           				/*电池箱内最大温度值*/
	s32 boxAvgCellT = 0;           				/*电池箱内平均温度值*/

	#if(IMLU_FAN_BACK_TYPE > 0)					/*需要检测风扇告警*/
	u8 j = 0;
	u8 num = 0;
	static u8 sFanToBoxNum[16] = {0xff};		/*风扇通道对应电池箱号*/
	static u8 sFanAlarmTime[4] = {0};			/*风扇告警延时*/
	#endif
	static u32 sChgDhgHigT = 0;					/*系统大功率充放电运行时间计数值 0.8C*/
	static u32 sChgDhgLowT = 0;					/*系统小功率充放电运行时间计数值 10A*/
	static u32 sFanRunTime = 0;				    /*风扇开启时间(确保每次开启至少持续5min)*/
	static u8 sFanCtrlNum = 0;				    /*风扇控制序号*/
	static u8 sFanCtrlTime = 0;				    /*风扇控制周期*/
	static u8 sFanCtrlState = 0;				/*执行步骤*/

	/*电池箱散热风扇调速控制*/
	switch(sFanCtrlState)
	{
		//查询风扇告警状态和水浸传感器采样值
		case 0:
			//查询风扇回检信号和水浸传感器采样值
			(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_IMLU1, 0x03, MB_IMLU_RO_BASE_ADDR, 3, HEAT_MANANG_MODBUS_PORT);

			//上电前5s连续查询确保通信正常
			if(BSPGetOSMillSecTimeAPI() >= 5000)
			{
				//需要检测风扇告警
				#if(IMLU_FAN_BACK_TYPE > 0)
				{
					//进入判断风扇告警状态
					sFanCtrlState = 1;
				}
				//不检测风扇告警
				#else
				{
					/*系统调试模式直接发送控制命令*/
				    if(eMODE_DEBUG == GetGWorkModeAPI())
				    {
				    	//将调试命令更新到显示状态
				    	for(i = 0; ((i < BMU_PWM_FAN_NUM) && (i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])); i++)
				    	{
				    		//将PWM通道转化成实际电池箱序号
							InputGPerImlu1FanAPI(i,  TacGetPwmFanCtrlDutyAPI(i));
				    	}

				    	//直接进入发送命令
				    	sFanCtrlState = 3;
				    }
				    /*IMLU1通讯中断显示无效值*/
				    else if(1 == DiagGetPerImluOffCommuStateAPI())
				    {
				    	/*显示无效值*/
				    	for(i = 0; ((i < BMU_PWM_FAN_NUM) && (i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])); i++)
				    	{
				    		//全部显示无效值
							InputGPerImlu1FanAPI(i,  0xff);
				    	}

				    	sFanCtrlState = 0;
				    }
				    /*非调试模式且通信正常则检测风扇调速需求*/
				    else
				    {
				    	//根据电池温度和温差计算风扇转速
				    	sFanCtrlState = 2;
				    }
				}
				#endif
			}
			break;

		//判断风扇告警状态(将风扇运行状态转为告警状态)
		case 1:
			//需要检测风扇告警
			#if(IMLU_FAN_BACK_TYPE > 0)
			{
				//未初始化
				if(0xff == sFanToBoxNum[0])
				{
					//满15个电池箱则按顺序
					if(gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] >= 15)
					{
						//初始化插框风扇通道号对应的电池箱号
						for(i = 0; i < 16; i++)
						{
							sFanToBoxNum[i] = i + 1;
						}
					}
					//有空缺插框则电池箱不连续(根据配置参数确定位置)
					else
					{
						//将有效电池箱号放到对应插框(无效电池箱号不处理默认值0)
						for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; i++)
						{
							//插框位号配置参数有效
							if((gGHardPara_104[eGHardPara104_PackFanPwm0 + i] >= 1)
								&& (gGHardPara_104[eGHardPara104_PackFanPwm0 + i] <= 16))
							{
								//记录插框位号相应风扇通道对应的电池箱号
								sFanToBoxNum[gGHardPara_104[eGHardPara104_PackFanPwm0 + i] - 1] = i + 1;
							}
							//无效插框位号
							else
							{
								//插框位号对应风扇通道的电池箱无效
								sFanToBoxNum[gGHardPara_104[eGHardPara104_PackFanPwm0 + i] - 1] = 0;
							}
						}
					}
				}

				/*IMLU1通讯正常*/
				if(0 == DiagGetPerImluOffCommuStateAPI())
				{
					//16个风扇只有4路回检信号(闭合(高)为风扇停止状态或告警状态)
					for(i = 0; i < 4; i++)
					{
						/*回检1为停止状态(控制停止或堵转)或告警状态(4个风扇回检信号共用1BIT位,)*/
						if(1 == BitGet(GetGPerImlu1SignInfoAPI(eIMLU1Info202_FanSign), i))
						{
							//回检信号为启停反馈(1为至少1个风扇停止状态)
							#if((1 == IMLU_FAN_BACK_TYPE) || (3 == IMLU_FAN_BACK_TYPE))
							{
								//用于标记风扇正常
								k = 0;
								//1个信号对应4个风扇
								for(j = 0; j < 4; j++)
								{
									//计算实际风扇通道号
									num = i * 4 + j;

									//该风扇通道有电池箱才检测(电池箱对应的风扇通道不连续)
									if((sFanToBoxNum[num] > 0) && (sFanToBoxNum[num] <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))
									{
										//4个风扇均控制开启则异常(有控制停止则正常)
										if(0 == GetGPerImlu1FanInfoAPI(sFanToBoxNum[num] - 1))
										{
											//有控制停止的风扇记录正常
											k = 5;
											BitClr(sFanAlarmState, (sFanToBoxNum[num] - 1));
										}
									}
									//计算空缺通道数
									else
									{
										k++;
									}
								}

								//4个均没有控制停止的风扇(异常)
								if(k < 4)
								{
									//达到延时确认故障
									if(sFanAlarmTime[i] >= IMLU_FAN_CTRL_TIMES)
									{
										//1个信号对应4个风扇
										for(j = 0; j < 4; j++)
										{
											//计算实际风扇通道号
											num = i * 4 + j;

											//该风扇通道有电池箱才上报告警(电池箱对应的风扇通道不连续)
											if((sFanToBoxNum[num] > 0) && (sFanToBoxNum[num] <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))
											{
												BitSet(sFanAlarmState, (sFanToBoxNum[num] - 1));
											}
										}
									}
									//延时
									else
									{
										sFanAlarmTime[i]++;
									}
								}
								//4个中有控制停止的风扇(正常)
								else
								{
									sFanAlarmTime[i] = 0;
								}
							}
							//回检信号为告警反馈(1为告警状态)
							#else
							{
								if(sFanAlarmTime[i] >= IMLU_FAN_CTRL_TIMES)
								{
									//1个故障信号对应4个风扇
									for(j = 0; j < 4; j++)
									{
										//计算实际风扇通道号
										num = i * 4 + j;

										//该风扇通道有电池箱才上报告警(电池箱对应的风扇通道不连续)
										if((sFanToBoxNum[num] > 0) && (sFanToBoxNum[num] <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))
										{
											BitSet(sFanAlarmState, (sFanToBoxNum[num] - 1));
										}
									}
								}
								else
								{
									sFanAlarmTime[i]++;
								}
							}
							#endif
						}
						/*回检0为运行或未连接回检状态或正常状态(4个风扇回检信号共用1BIT位)*/
						else
						{
							//回检信号为启停反馈(0为4个风扇均运行或未连接回检状态)
							#if((1 == IMLU_FAN_BACK_TYPE) || (3 == IMLU_FAN_BACK_TYPE))
							{
								//用于记录风扇异常
								k = 0;
								//1个信号对应4个风扇
								for(j = 0; j < 4; j++)
								{
									//计算实际风扇号
									num = i * 4 + j;

									//该风扇通道有电池箱才检测(电池箱对应的风扇通道不连续)
									if((sFanToBoxNum[num] > 0) && (sFanToBoxNum[num] <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))
									{
										//至少有1个风扇控制停止则异常(无法关停风扇)
										if(0 == GetGPerImlu1FanInfoAPI(sFanToBoxNum[num] - 1))
										{
											k = 1;
											break;
										}
									}
								}

								//有风扇控制停止但无法关停风扇
								if(1 == k)
								{
									if(sFanAlarmTime[i] >= IMLU_FAN_CTRL_TIMES)
									{
										//1个信号对应4个风扇
										for(j = 0; j < 4; j++)
										{
											//计算实际风扇号
											num = i * 4 + j;

											//该风扇通道有电池箱才上报告警(电池箱对应的风扇通道不连续)
											if((sFanToBoxNum[num] > 0) && (sFanToBoxNum[num] <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))
											{
												BitSet(sFanAlarmState, (sFanToBoxNum[num] - 1));
											}
										}
									}
									else
									{
										sFanAlarmTime[i]++;
									}
								}
								//未控制风扇停止
								else
								{
									sFanAlarmTime[i] = 0;
									//1个信号对应4个风扇
									for(j = 0; j < 4; j++)
									{
										//计算实际风扇通道号
										num = i * 4 + j;

										//该风扇通道有电池箱才上报告警(电池箱对应的风扇通道不连续)
										if((sFanToBoxNum[num] > 0) && (sFanToBoxNum[num] <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))
										{
											//无告警
											BitClr(sFanAlarmState, (sFanToBoxNum[num] - 1));
										}
									}
								}
							}
							//回检信号为告警反馈(0为正常状态)
							#else
							{
								sFanAlarmTime[i] = 0;
								//1个信号对应4个风扇
								for(j = 0; j < 4; j++)
								{
									//计算实际风扇通道号
									num = i * 4 + j;

									//该风扇通道有电池箱才上报告警(电池箱对应的风扇通道不连续)
									if((sFanToBoxNum[num] > 0) && (sFanToBoxNum[num] <= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))
									{
										//无告警
										BitClr(sFanAlarmState, (sFanToBoxNum[num] - 1));
									}
								}
							}
							#endif
						}

//						//超出电池箱号(电池箱对应的风扇通道不连续)
//						if(num >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
//						{
//							break;
//						}
					}
				}/*判断IMLU1通讯结束*/
			}
			#endif

			/*系统调试模式直接显示调试命令和发送控制命令*/
		    if(eMODE_DEBUG == GetGWorkModeAPI())
		    {
		    	//将调试命令更新到显示状态
		    	for(i = 0; ((i < BMU_PWM_FAN_NUM) && (i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])); i++)
		    	{
		    		//将PWM通道转化成实际电池箱序号
					InputGPerImlu1FanAPI(i,  TacGetPwmFanCtrlDutyAPI(i));
		    	}

		    	//直接进入发送命令
		    	sFanCtrlState = 3;
		    }
		    /*IMLU1通讯中断显示无效值*/
		    else if(1 == DiagGetPerImluOffCommuStateAPI())
		    {
		    	/*显示无效值*/
		    	for(i = 0; ((i < BMU_PWM_FAN_NUM) && (i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])); i++)
		    	{
		    		//全部显示无效值
					InputGPerImlu1FanAPI(i,  0xff);
		    	}

		    	sFanCtrlState = 0;
		    }
		    /*非调试模式检测风扇调速需求*/
		    else
		    {
				/*需要风扇故障保护时才考虑风扇全部异常时直接全部关闭风扇*/
				#if((3 == IMLU_FAN_BACK_TYPE) || (4 == IMLU_FAN_BACK_TYPE))
				if(((gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] >= 16) && (sFanAlarmState == 0xffff))
					|| ((gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] < 16) && ((sFanAlarmState & (0xffff >> (16 - gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))) == (0xffff >> (16 - gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])))))
				{
					for(i = 0; i < BMU_PWM_FAN_NUM; i++)
					{
						//0%关闭风扇
						sFanCtrlDuty[i] = 0;

						//电池箱风扇显示控制PWM调速显示清零
						InputGPerImlu1FanAPI(i,  0);
					}

					//直接进入发送命令
					sFanCtrlState = 3;
				}
				/*有风扇正常*/
				else
				#endif
				{
					//进入计算各个风扇占空比
					sFanCtrlState = 2;
				}
		    }
			break;

		//根据电池温度和温差计算风扇转速
		case 2:
			//系统开机关机或消防火警或可燃气体告警/打开百叶风机通风时必须关闭风扇
			if((eWORK_INIT == GetGWorkStateAPI()) || (eWORK_OFF == GetGWorkStateAPI())           //开关机
				|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//消防告警
				|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_CGAS_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_CGAS_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//气体浓度告警
				#if((1 == BMU_IMEU_COM_EN) && (1 == BMU_EU_SHUTFAN_EN))
				|| ((0 == DiagGetPerImeuOffCommuStateAPI())                                           //IMEU2通信正常
				&& (0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_ShutOn)) //百叶打开到位
				&& (GetGPerImeu2SignInfoAPI(eIMEU2Info204_FanSpeed) >= 100)                           //风机转
				&& (0 == BitGet(gAuxDevErrFlag, eAuxErr_ShutOn))                                      //百叶正常
				&& (0 == BitGet(gAuxDevErrFlag, eAuxErr_FanOn))                                       //风机正常
				&& (0 == BitGet(gAuxDevErrFlag, eAuxErr_ShutOff))
				&& (0 == BitGet(gAuxDevErrFlag, eAuxErr_FanOff)))
				#endif
				)
			{
				sChgDhgHigT = 0;
				sChgDhgLowT = 0;
				//for(i = 0; i < BMU_PWM_FAN_NUM; i++)
				for(i = 0; ((i < BMU_PWM_FAN_NUM) && (i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])); i++)
				{
					//0%关闭风扇
					//sFanCtrlDuty[i] = 0;
					TacSetPwmFanUserCmdAPI(i, 0);

					//电池箱风扇显示控制PWM调速显示清零
					InputGPerImlu1FanAPI(i,  0);
				}
			}
			//电池温度极高或烟雾告警低速开启风扇
			else if(((TRUE == TacGetGAllSlaveReadyHook()) && (GetGCellMaxTempAPI() >= BATT_EXEREM_MAXT) && (1 == BitGet(DiagGetGroupSosResion(0), eSOS_REASION_TEX)))
				|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE))))//烟雾告警
			{
				sChgDhgHigT = 0;
				sChgDhgLowT = 0;
				//for(i = 0; i < BMU_PWM_FAN_NUM; i++)
				for(i = 0; ((i < BMU_PWM_FAN_NUM) && (i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])); i++)
				{
					//22%开启风扇
					//sFanCtrlDuty[i] = IMLU_FAN_MIN_SPEED;
					TacSetPwmFanUserCmdAPI(i, IMLU_FAN_MIN_SPEED);

					//电池箱风扇显示控制PWM调速显示
					InputGPerImlu1FanAPI(i,  IMLU_FAN_MIN_SPEED);
				}
			}
			//单体温度采样异常必须开启风扇
			else if(FALSE == TacGetGAllSlaveReadyHook())//从控信息异常
			{
				sChgDhgHigT = 0;
				sChgDhgLowT = 0;

				//for(i = 0; i < BMU_PWM_FAN_NUM; i++)
				for(i = 0; ((i < BMU_PWM_FAN_NUM) && (i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])); i++)
				{
					//热失控温度极高风扇30%(风速太多影响热失控判断)
					if(GetGCellMaxTempAPI() >= BATT_EXEREM_MAXT)
					{
						//22%风扇起转排出可燃气体
						//sFanCtrlDuty[i] = IMLU_FAN_MIN_SPEED;
						TacSetPwmFanUserCmdAPI(i, IMLU_FAN_MIN_SPEED);

						//电池箱风扇显示控制PWM调速显示控制值
						InputGPerImlu1FanAPI(i,  IMLU_FAN_MIN_SPEED);
					}
					//温度较高风扇70%
					else if(GetGCellMaxTempAPI() >= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp])
					{
//						//70%开启风扇
//						//sFanCtrlDuty[i] = IMLU_FAN_RUN_SPEED;
//						TacSetPwmFanUserCmdAPI(i, IMLU_FAN_RUN_SPEED);
//
//						//电池箱风扇显示控制PWM调速显示控制值
//						InputGPerImlu1FanAPI(i,  IMLU_FAN_RUN_SPEED);

						//50%开启风扇
						//sFanCtrlDuty[i] = IMLU_FAN_HOL_SPEED;
						TacSetPwmFanUserCmdAPI(i, IMLU_FAN_HOL_SPEED);

						//电池箱风扇显示控制PWM调速显示控制值
						InputGPerImlu1FanAPI(i,  IMLU_FAN_HOL_SPEED);
					}
					//温度低风扇怠速排风即可
					else
					{
//						//50%开启风扇
//						//sFanCtrlDuty[i] = IMLU_FAN_HOL_SPEED;
//						TacSetPwmFanUserCmdAPI(i, IMLU_FAN_HOL_SPEED);
//
//						//电池箱风扇显示控制PWM调速显示控制值
//						InputGPerImlu1FanAPI(i,  IMLU_FAN_HOL_SPEED);

						//22%风扇起转排出可燃气体
						//sFanCtrlDuty[i] = IMLU_FAN_MIN_SPEED;
						TacSetPwmFanUserCmdAPI(i, IMLU_FAN_MIN_SPEED);

						//电池箱风扇显示控制PWM调速显示控制值
						InputGPerImlu1FanAPI(i,  IMLU_FAN_MIN_SPEED);
					}
				}
			}
			//系统正常按策略调速风扇
			else
			{
				/*累计电池组超过0.8C充放电的持续时间*/
				//if((1 == GetGChgDhgStateAPI() && ((0 - GetGSampOutCurrAPI()) > (s16)(FULL_FAN_CHGDHG_CURR * GetGBattAllCapAPI() / 10)))//充电电流大于0.8C
				//	|| ((2 == GetGChgDhgStateAPI()) && (GetGSampOutCurrAPI() > (s16)(FULL_FAN_CHGDHG_CURR * GetGBattAllCapAPI() / 10))))//放电电流大于0.8C
				if(((0 - sAvgCurr1min) > (s16)(FULL_FAN_CHGDHG_CURR * GetGBattAllCapAPI() / 10))//充电电流大于0.8C
					|| (sAvgCurr1min > (s16)(FULL_FAN_CHGDHG_CURR * GetGBattAllCapAPI() / 10)))//放电电流大于0.8C
				{
					//充电或者放电持续时间计数(任务函数1S时间运行计数)
					if(sChgDhgHigT >= 0xffffff00)
					{
						//防止越界
						sChgDhgHigT = 0xffffffff;
					}
					else
					{
						//18s周期执行1次
						sChgDhgHigT += 18;
					}
				}
				/*切换为静置状态(减小功率不清零)*/
				else if(0 == GetGChgDhgStateAPI())
				{
					sChgDhgHigT = 0;
				}

				/*累计电池组超过30A充放电的持续时间*/
				if((sAvgCurr1min < -300) || (sAvgCurr1min > 300))
				{
					//充电或者放电持续时间计数(任务函数1S时间运行计数)
					if(sChgDhgLowT >= 0xffffff00)
					{
						//防止越界
						sChgDhgLowT = 0xffffffff;
					}
					else
					{
						//18s周期执行1次
						sChgDhgLowT += 18;
					}
				}
				/*切换为静置状态(减小功率不清零)*/
				else if(0 == GetGChgDhgStateAPI())
				{
					sChgDhgLowT = 0;
				}

				//按电池箱数目检测
				for(i = 0; ((i < BMU_PWM_FAN_NUM) && (i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])); i++)
				{
					//电池组最高温度值大于26℃,开启风扇调速(温度每增加1℃，风扇转速增加8%；风扇的PWM控制区间为20%~100%)
					//if((GetGCellMaxTempAPI() >= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp])
					if((GetGCellMaxTempAPI() >= 24)
					//需要根据回检状态进行控制保护
					#if((3 == IMLU_FAN_BACK_TYPE) || (4 == IMLU_FAN_BACK_TYPE))
						&& (0 == BitGet(sFanAlarmState, i))//该风扇回检为正常状态
					#endif
						)
					{
						//配置参数异常
						if((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp] >= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp])
						{
							gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp] = (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp] + 10;
						}

						//组最低温度已需要达到全速或0.8C以上充放电持续时间超过15min,所有风扇100%满转速,预防电池温度斜率变大(热失衡)
						if((GetGCellMinTempAPI() >= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp])//到达全速温度
							|| ((sChgDhgHigT >= FULL_FAN_CHGDHG_TIME) && (GetGCellMinTempAPI() >= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp])))
						{
							//从关闭转为开启
							if(0 == TacGetPwmFanCtrlDutyAPI(i))
							{
								//记录开启时间
								sFanRunTime = GetSysSecondTimeAPI();
							}

							//所有电箱100%转速
							//sFanCtrlDuty[i] = 100;
							TacSetPwmFanUserCmdAPI(i, 100);
						}
						//电池组有温度较低且未到达大电流持续时间(逐个电箱判断温度)
						else
						{
							//查找到指定PACK箱内最高温度和最低温度
							boxAvgCellT = 0;
							boxMaxCellT = -41;
							boxMinCellT = 0x7f;
							for(k = 0; k < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum]; k++)
							{
								//电箱序号转换成组内温度序号
								cell = ChangSlaveTidToGroupTid(i, k);
								//转换组内温度序号有效
								if(cell < gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum])
								{
									//求箱内最高温度
									if(GetGCellTempAPI(cell) > boxMaxCellT)
									{
										boxMaxCellT = GetGCellTempAPI(cell);
									}

									//求箱内最低温度
									if(GetGCellTempAPI(cell) < boxMinCellT)
									{
										boxMinCellT = GetGCellTempAPI(cell);
									}

									boxAvgCellT += GetGCellTempAPI(cell);
								}
								else
								{
									break;
								}
							}

							//电箱达到起转温度
							if(boxMaxCellT >= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp])
							{
								//开始100A大功率充放电或已30A小功率充放电超8min,或高于起转温度4℃,或高于起转温度2℃且电池温度比仓温高2℃
								if(((sAvgCurr1min <= -970) && (GetGSampOutCurrAPI() <= -970))
									|| ((sAvgCurr1min >= 970) && (GetGSampOutCurrAPI() >= 970)) || (sChgDhgLowT >= 480)
									|| (boxMaxCellT >= ((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp] + 4))
									|| ((boxMaxCellT >= ((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp] + 2))
								#if(1 == BMU_IMEU_COM_EN)/*使能IMEU2控制(无空调不考虑柜内温度*/
									&& (GetGCellMaxTempAPI() >= ((s16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 2))
								#endif
									))
								{
									//从关闭转为开启
									if(0 == TacGetPwmFanCtrlDutyAPI(i))
									{
										//记录开启时间
										sFanRunTime = GetSysSecondTimeAPI();
									}

									//按温度比例控制风扇转速(风扇在大于22%的PWM起转)
									//sFanCtrlDuty[i] = (boxMaxCellT - (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp]) * (s16)(100 - IMLU_FAN_RUN_SPEED) / (gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp] - (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp]) + IMLU_FAN_RUN_SPEED;
									TacSetPwmFanUserCmdAPI(i, (boxMaxCellT - (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp]) * (s16)(100 - IMLU_FAN_RUN_SPEED) / (gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp] - (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp]) + IMLU_FAN_RUN_SPEED);
								}
								/*使能IMEU2控制(无空调不考虑柜内温度)*/
								#if(1 == BMU_IMEU_COM_EN)
								//电箱温度刚到启动风扇点,且空调开机通风或电箱温度与柜内温差较大,电池箱需要低速排气
								else if((0 == DiagGetPerImeuOffCommuStateAPI())
									&& (((1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_Power))
									&& (boxMaxCellT >= ((s16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 1)))
									|| (boxMaxCellT >= ((s16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 4))))
								{
									//从关闭转为开启
									if(0 == TacGetPwmFanCtrlDutyAPI(i))
									{
										//记录开启时间
										sFanRunTime = GetSysSecondTimeAPI();
									}

									//组内温差过大(5℃)而箱内温度接近组最高温度(2℃)且高于组最低温度(2℃)
									if((GetGCellMaxTempAPI() >= (GetGCellMinTempAPI() + 5))
										&& ((boxMaxCellT + 2) >= GetGCellMaxTempAPI())
										&& (boxMinCellT >= (GetGCellMinTempAPI() + 2)))
									{
										//半程转速
										//sFanCtrlDuty[i] = (IMLU_FAN_RUN_SPEED + 100) / 2;
										TacSetPwmFanUserCmdAPI(i, ((IMLU_FAN_RUN_SPEED + 100) / 2));
									}
									//组内温差小(5℃)或箱内温差大
									else
									{
										//刚好排风起始转速即可
										//sFanCtrlDuty[i] = IMLU_FAN_RUN_SPEED;
										TacSetPwmFanUserCmdAPI(i, IMLU_FAN_RUN_SPEED);
									}
								}
								#endif
								//电箱温度刚到启动点而电池组在静置而与柜内温差小,无需开风扇(已开启的至少间隔5min才能关闭)
								else if(ABS(GetSysSecondTimeAPI() , sFanRunTime) >= 300)
								{
									//关闭风扇
									//sFanCtrlDuty[i] = 0;
									TacSetPwmFanUserCmdAPI(i, 0);
								}
							}
							/*使能IMEU2通信时根据柜内温度调节内循环(无空调无柜内温度)*/
							#if(1 == BMU_IMEU_COM_EN)
							//电箱温度未到启动风扇点,空调开机通风或25℃以上电池与柜内温差较大或箱内温度较高,则电池箱需要低速排气
							else if((0 == DiagGetPerImeuOffCommuStateAPI())
								&& (((1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_Power))
								&& (boxMaxCellT >= ((s16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 3)))
								|| ((boxMaxCellT >= 25) && (boxMaxCellT >= ((s16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 5)))
								|| ((boxMaxCellT >= (GetGCellMinTempAPI() + 5)) && (boxMinCellT >= (GetGCellMinTempAPI() + 2)) && (boxMaxCellT >= ((s16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 4)))))
							{
								//从关闭转为开启
								if(0 == TacGetPwmFanCtrlDutyAPI(i))
								{
									//记录开启时间
									sFanRunTime = GetSysSecondTimeAPI();
								}

								//组内温差过大(5℃)而箱内温度接近组最高温度(3℃)且高于组最低温度(2℃)
								if((GetGCellMaxTempAPI() >= (GetGCellMinTempAPI() + 5))
									&& ((boxMaxCellT + 3) >= GetGCellMaxTempAPI())
									&& (boxMinCellT >= (GetGCellMinTempAPI() + 2)))
								{
									//半程转速
									//sFanCtrlDuty[i] = (IMLU_FAN_HOL_SPEED + 100) / 2;
									TacSetPwmFanUserCmdAPI(i, ((IMLU_FAN_HOL_SPEED + 100) / 2));
								}
								//组内温差小(5℃)或箱内温差大
								else
								{
									//刚好排风起始转速即可
									//sFanCtrlDuty[i] = IMLU_FAN_HOL_SPEED;
									TacSetPwmFanUserCmdAPI(i, IMLU_FAN_HOL_SPEED);
								}
							}
							#endif
							//电箱温度低也未开空调则无需开风扇(已开启的至少间隔5min才能关闭)
							else if(ABS(GetSysSecondTimeAPI() , sFanRunTime) >= 300)
							{
								//关闭风扇
								//sFanCtrlDuty[i] = 0;
								TacSetPwmFanUserCmdAPI(i, 0);
							}

							//以下是按照电箱内平均温度大小提高风扇转速(平均温度越大转速越快)
							//电箱最高温度接近电池组最高温度2℃内才需要提速
							if(((boxMaxCellT + 2) >= GetGCellMaxTempAPI()) && (k > 0))
							{
								//求平均温度
								boxAvgCellT = boxAvgCellT / k;

								//电箱平均温度高压组平均温度才需要提速
								if(boxAvgCellT > GetGCellAvgTempAPI())
								{
									//风扇未开启
									//if(0 == sFanCtrlDuty[i])
									if(0 == TacGetPwmFanCtrlDutyAPI(i))
									{
										//电箱平均温度高于组平均温度2℃才需要提速
										if(boxAvgCellT > (GetGCellAvgTempAPI() + 1))
										{
											//记录开启时间
											sFanRunTime = GetSysSecondTimeAPI();

											//刚好排风起始转速即可
											//sFanCtrlDuty[i] = IMLU_FAN_RUN_SPEED;
											TacSetPwmFanUserCmdAPI(i, IMLU_FAN_RUN_SPEED);
										}
									}
									//风扇已开启
									else
									{
										//原有基础上提速
										//sFanCtrlDuty[i] += (boxAvgCellT - GetGCellAvgTempAPI()) * (s16)(100 - IMLU_FAN_RUN_SPEED) / 10;
										TacSetPwmFanUserCmdAPI(i, TacGetPwmFanCtrlDutyAPI(i) + ((boxAvgCellT - GetGCellAvgTempAPI()) * (s16)(100 - IMLU_FAN_RUN_SPEED) / 10));
									}
								}
							}
							//电箱最高温度低于电池组最高温度3℃以上需要降速
							else if((boxMaxCellT + 3) < GetGCellMaxTempAPI())
							{
								//电箱内温度接近电池组最低温3℃内需要降速10%(目前高于60%转速才降)
								if(((GetGCellMinTempAPI() + 3) > boxMinCellT)
									&& (TacGetPwmFanCtrlDutyAPI(i) >= 60))
								{
									//原有基础上降速
									//sFanCtrlDuty[i] -= (s16)(100 - IMLU_FAN_RUN_SPEED) / (gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp] - (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp]);
									TacSetPwmFanUserCmdAPI(i, TacGetPwmFanCtrlDutyAPI(i) - ((100 - IMLU_FAN_RUN_SPEED) / (gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp] - (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp])));
								}
							}

//							//以下是按照电箱内温差大小提高风扇转速(温差越大转速越快)
//							//电箱最高温度接近电池组最高温度2℃内才需要提速
//							if((boxMaxCellT + 2) >= GetGCellMaxTempAPI())
//							{
//								//电箱内温差超过15℃直接转速100%
//								if(boxMaxCellT >= (boxMinCellT + 15))
//								{
//									//从关闭转为开启
//									if(0 == TacGetPwmFanCtrlDutyAPI(i))
//									{
//										//记录开启时间
//										sFanRunTime = GetSysSecondTimeAPI();
//									}
//
//									//100%开启风扇
//									//sFanCtrlDuty[i] = 100;
//									TacSetPwmFanUserCmdAPI(i, 100);
//								}
//								//电箱内温差超过3℃需要按照温差大小提高转速
//								else if(boxMaxCellT > (boxMinCellT + 3))
//								{
//									//风扇未开启
//									//if(0 == sFanCtrlDuty[i])
//									if(0 == TacGetPwmFanCtrlDutyAPI(i))
//									{
//										//记录开启时间
//										sFanRunTime = GetSysSecondTimeAPI();
//
//										//刚好排风起始转速即可
//										//sFanCtrlDuty[i] = IMLU_FAN_RUN_SPEED;
//										TacSetPwmFanUserCmdAPI(i, IMLU_FAN_RUN_SPEED);
//									}
//									//风扇已开启
//									else
//									{
//										//原有基础上提速
//										//sFanCtrlDuty[i] += (boxMaxCellT - boxMinCellT - 3) * (s16)(100 - IMLU_FAN_RUN_SPEED) / 12;
//										TacSetPwmFanUserCmdAPI(i, TacGetPwmFanCtrlDutyAPI(i) + ((boxMaxCellT - boxMinCellT - 3) * (s16)(100 - IMLU_FAN_RUN_SPEED) / 12));
//									}
//								}
//							}
//							//电箱最高温度低于电池组最高温度3℃以上需要降速
//							else if((boxMaxCellT + 3) < GetGCellMaxTempAPI())
//							{
//								//电箱内温度接近电池组最低温3℃内需要降速10%(目前高于60%转速才降)
//								if(((GetGCellMinTempAPI() + 3) > boxMinCellT)
//									&& (TacGetPwmFanCtrlDutyAPI(i) >= 60))
//								{
//									//原有基础上降速
//									//sFanCtrlDuty[i] -= (s16)(100 - IMLU_FAN_RUN_SPEED) / (gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp] - (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp]);
//									TacSetPwmFanUserCmdAPI(i, TacGetPwmFanCtrlDutyAPI(i) - ((100 - IMLU_FAN_RUN_SPEED) / (gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp] - (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp])));
//								}
//							}

							//记录是否有关闭的风扇
							if(0 == TacGetPwmFanCtrlDutyAPI(i))
							{
								close = 1;
							}

							//记录最低转速(非0)
							if((0 == minSpeed) || ((TacGetPwmFanCtrlDutyAPI(i) > 0) && (TacGetPwmFanCtrlDutyAPI(i) < minSpeed)))
							{
								minSpeed = TacGetPwmFanCtrlDutyAPI(i);
							}

							//记录最高转速
							if((0 == i) || (TacGetPwmFanCtrlDutyAPI(i) > maxSpeed))
							{
								maxSpeed = TacGetPwmFanCtrlDutyAPI(i);
							}
						}
					}
					/*使能IMEU2通信时根据柜内温度调节内循环(无空调无柜内温度也无制热,不需低温开风扇)*/
					#if(1 == BMU_IMEU_COM_EN)
					//电池组温度值低于15℃或可能空调除湿加热(空调加热出现温差才开风扇)
					else if(((GetGCellMinTempAPI() <= (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_HotCloseTemp])
						|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) >= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDonH))
						&& (((0 == sAuxDevAirType) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDoffT)))
						|| ((1 == sAuxDevAirType) && (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirState), eEU205AirS_ElHeatSt_TF))))))
						&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (GetGCellMinTempAPI() + 5))
						&& (0 == DiagGetPerImeuOffCommuStateAPI())
					//需要根据回检状态进行控制保护
					#if((3 == IMLU_FAN_BACK_TYPE) || (4 == IMLU_FAN_BACK_TYPE))
						&& (0 == BitGet(sFanAlarmState, num))//该风扇回检为正常状态
					#endif
						)
					{
						//电池组中温差过大(3℃),需要开启风扇
						if(GetGCellMaxTempAPI() >= (GetGCellMinTempAPI() + 3))
						{
							//查找到指定PACK箱内最高温度和最低温度
							boxMaxCellT = -41;
							boxMinCellT = 0x7f;
							for(k = 0; k < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum]; k++)
							{
								//电箱序号转换成组内温度序号
								cell = ChangSlaveTidToGroupTid(i, k);
								//转换组内温度序号有效
								if(cell < gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum])
								{
									//求箱内最高温度
									if(GetGCellTempAPI(cell) > boxMaxCellT)
									{
										boxMaxCellT = GetGCellTempAPI(cell);
									}

									//求箱内最低温度
									if(GetGCellTempAPI(cell) < boxMinCellT)
									{
										boxMinCellT = GetGCellTempAPI(cell);
									}
								}
								else
								{
									break;
								}
							}

							//从关闭转为开启
							if(0 == TacGetPwmFanCtrlDutyAPI(i))
							{
								//记录开启时间
								sFanRunTime = GetSysSecondTimeAPI();
							}

							//箱内温差过大(6℃)或组内温差大(5℃)时最低温度电池箱
							if((boxMaxCellT >= (boxMinCellT + 6)) || ((GetGCellMaxTempAPI() >= (GetGCellMinTempAPI() + 5))
								&& ((boxMinCellT == GetGCellMinTempAPI()) || (i = ChangGroupTidToSlaveNum(GetGCellMinTNumAPI() - 1)))))
							{
								//90%转速
								//sFanCtrlDuty[i] = 90;
								TacSetPwmFanUserCmdAPI(i, 90);
							}
							//组内温差过大(5℃)
							else if(GetGCellMaxTempAPI() >= (GetGCellMinTempAPI() + 5))
							{
								//按温度比例控制风扇转速(风扇在大于50%~90%的PWM内)
								//sFanCtrlDuty[i] = (GetGCellMaxTempAPI() - boxMinCellT) * (s16)(90 - IMLU_FAN_HOL_SPEED) / (GetGCellMaxTempAPI() - GetGCellMinTempAPI()) + IMLU_FAN_HOL_SPEED;
								TacSetPwmFanUserCmdAPI(i, (GetGCellMaxTempAPI() - boxMinCellT) * (s16)(90 - IMLU_FAN_HOL_SPEED) / (GetGCellMaxTempAPI() - GetGCellMinTempAPI()) + IMLU_FAN_HOL_SPEED);

								//箱内温差过大(4℃)
								if(boxMaxCellT >= (boxMinCellT + 4))
								{
									//转速提高10%
									//if(sFanCtrlDuty[i] <= 60)
									if(TacGetPwmFanCtrlDutyAPI(i) <= 60)
									{
										//sFanCtrlDuty[i] += 10;
										TacSetPwmFanUserCmdAPI(i, TacGetPwmFanCtrlDutyAPI(i) + 10);
									}
									//else if(sFanCtrlDuty[i] <= 65)
									else if(TacGetPwmFanCtrlDutyAPI(i) <= 65)
									{
										//sFanCtrlDuty[i] += 7;
										TacSetPwmFanUserCmdAPI(i, TacGetPwmFanCtrlDutyAPI(i) + 7);
									}
									//else if(sFanCtrlDuty[i] <= 70)
									else if(TacGetPwmFanCtrlDutyAPI(i) <= 70)
									{
										//sFanCtrlDuty[i] += 4;
										TacSetPwmFanUserCmdAPI(i, TacGetPwmFanCtrlDutyAPI(i) + 4);
									}
									//else if(sFanCtrlDuty[i] <= 75)
									else if(TacGetPwmFanCtrlDutyAPI(i) <= 75)
									{
										//sFanCtrlDuty[i] += 2;
										TacSetPwmFanUserCmdAPI(i, TacGetPwmFanCtrlDutyAPI(i) + 2);
									}
								}
							}
							//组内温差较小(5℃内)较低温电池箱
							else if(boxMinCellT <= (GetGCellMinTempAPI() + 1))
							{
								//60%转速
								//sFanCtrlDuty[i] = IMLU_FAN_HOL_SPEED + 10;
								TacSetPwmFanUserCmdAPI(i, IMLU_FAN_HOL_SPEED + 10);
							}
							//电池组温差较小(5℃内)处于高温电池箱
							else
							{
								//55%开启风扇
								//sFanCtrlDuty[i] = IMLU_FAN_HOL_SPEED + 5;
								TacSetPwmFanUserCmdAPI(i, IMLU_FAN_HOL_SPEED + 5);
							}
						}
						//电池组中温差很小(3℃内)(已开启的至少间隔2min才能关闭)
						else if(ABS(GetSysSecondTimeAPI() , sFanRunTime) >= 120)
						{
							//关闭风扇
							//sFanCtrlDuty[i] = 0;
							TacSetPwmFanUserCmdAPI(i, 0);
						}
					}
					#endif
					//电池组温度处于中间温度或该风扇回检故障(已开启的至少间隔5min才能关闭)
					else if(ABS(GetSysSecondTimeAPI() , sFanRunTime) >= 300)
					{
						/*PWM占空比为0*/
						//sFanCtrlDuty[i] = 0;
						TacSetPwmFanUserCmdAPI(i, 0);
					}

					//电池箱风扇显示控制PWM调速显示控制值
					//InputGPerImlu1FanAPI(i,  sFanCtrlDuty[i]);
					InputGPerImlu1FanAPI(i,  TacGetPwmFanCtrlDutyAPI(i));
				}

				//需要散热开启时:风扇最低转速不能低于排风起始转速且不能有停止的风扇(按照最低转速),温度到达区间中点且有速度差时要有100%风扇
				if((minSpeed > 0) && ((minSpeed < IMLU_FAN_RUN_SPEED) || (close > 0)
					|| ((GetGCellMaxTempAPI() >= ((gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp] + (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp]) / 2))
					&& (maxSpeed < 100) && ((maxSpeed - minSpeed) >= 5))))
				{
					//最低速低于起始排风转速
					if(minSpeed < IMLU_FAN_RUN_SPEED)
					{
						//计算最低速偏差(全部加上速度偏差即可)
						minSpeed = IMLU_FAN_RUN_SPEED - minSpeed;
					}
					//最低速高于起始排风转速
					else
					{
						//最低速偏差为0
						minSpeed = 0;
					}

					//将最高速提高至偏差
					maxSpeed += minSpeed;

					//最高速必须高于排风起始转速
					if(maxSpeed < IMLU_FAN_RUN_SPEED)
					{
						maxSpeed = IMLU_FAN_RUN_SPEED;
					}

					//将每个风扇逐个等速提升
					for(i = 0; ((i < BMU_PWM_FAN_NUM) && (i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])); i++)
					{
						//该风扇无需开启
						if(0 == TacGetPwmFanCtrlDutyAPI(i))
						{
							//记录开启时间
							sFanRunTime = GetSysSecondTimeAPI();

							//取最低转速
							TacSetPwmFanUserCmdAPI(i, IMLU_FAN_HOL_SPEED);
						}
						//该风扇需要开启
						else
						{
							//该风扇转速度按等差提速到起始排风转速以上
							TacSetPwmFanUserCmdAPI(i, TacGetPwmFanCtrlDutyAPI(i) + minSpeed);

							//温差过大时电池箱非最高速(确保最高温处于最高速)
							if((GetGCellMaxTempAPI() >= (GetGCellMinTempAPI() + 4)) && (TacGetPwmFanCtrlDutyAPI(i) < maxSpeed))
							{
								//查找该电池包温度是否最高温度(需最高速)
								for(k = 0; k < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum]; k++)
								{
									//电箱序号转换成组内温度序号
									cell = ChangSlaveTidToGroupTid(i, k);
									//转换组内温度序号有效
									if(cell < gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum])
									{
										//箱内温度与组最高温一致
										if(GetGCellTempAPI(cell) >= GetGCellMaxTempAPI())
										{
											//该电池箱风扇转速度提高到最高速
											TacSetPwmFanUserCmdAPI(i, maxSpeed);
											break;
										}
										//箱内温度接近组最高温
										else if(((GetGCellTempAPI(cell) + 1) >= GetGCellMaxTempAPI())
											&& ((TacGetPwmFanCtrlDutyAPI(i) + 5) < maxSpeed))
										{
											//该电池箱风扇转速度提高到最高速
											TacSetPwmFanUserCmdAPI(i, (maxSpeed - 5));
										}
									}
								}
							}

							//温度较高无100%转速(最高速需要提到100%其它等比例提速)
							if((GetGCellMaxTempAPI() >= ((gGBmuHigLevPara_103[eBmuHigLevPara103_FanOpenTemp] + (s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp]) / 2))
								&& (maxSpeed < 100) && ((maxSpeed - minSpeed) >= 5) && (TacGetPwmFanCtrlDutyAPI(i) >= IMLU_FAN_RUN_SPEED))
							{
								//以最低速为起转速度等差比例提速(放大100倍确保精度)
								TacSetPwmFanUserCmdAPI(i, IMLU_FAN_RUN_SPEED + (((u16)(100 - IMLU_FAN_RUN_SPEED) * 100 + 50) / (maxSpeed - IMLU_FAN_RUN_SPEED) * (TacGetPwmFanCtrlDutyAPI(i) - IMLU_FAN_RUN_SPEED) / 100));
							}
						}

						//电池箱风扇显示控制PWM调速显示控制值
						//InputGPerImlu1FanAPI(i,  sFanCtrlDuty[i]);
						InputGPerImlu1FanAPI(i,  TacGetPwmFanCtrlDutyAPI(i));
					}
				}
			}

			//下发控制所有风扇命令
			//MbClientSendWriteMultReqAPI(MODBUS_SALVE_ADDR_IMLU1, IMLU_ADDR_FANCTRL_SPEED_NUM, sFanCtrlDuty, BMU_PWM_FAN_NUM, HEAT_MANANG_MODBUS_PORT);
			//sFanCtrlState = 0;
			//分多次控制风扇
			sFanCtrlNum = 0;
			sFanCtrlTime = 0;
			sFanCtrlState = 3;
			break;

		//分多次风扇控制(防止RS485连续发送占用时间)
		case 3:
			//下发控制所有风扇命令
			//MbClientSendWriteMultReqAPI(MODBUS_SALVE_ADDR_IMLU1, IMLU_ADDR_FANCTRL_SPEED_NUM, sFanCtrlDuty, BMU_PWM_FAN_NUM, HEAT_MANANG_MODBUS_PORT);
			//不少于4个要控制
			if((sFanCtrlNum + 4) <= BMU_PWM_FAN_NUM)
			{
				//下发风扇控制命令(一次控制4个风扇)
				MbClientSendWriteMultReqAPI(MODBUS_SALVE_ADDR_IMLU1, IMLU_ADDR_FANCTRL_SPEED_NUM+sFanCtrlNum, &sFanCtrlDuty[sFanCtrlNum], 4, HEAT_MANANG_MODBUS_PORT);
			}
			//最后一帧少于4个要控制
			else if(sFanCtrlNum < BMU_PWM_FAN_NUM)
			{
				//下发风扇控制命令(控制剩余风扇)
				MbClientSendWriteMultReqAPI(MODBUS_SALVE_ADDR_IMLU1, IMLU_ADDR_FANCTRL_SPEED_NUM+sFanCtrlNum, &sFanCtrlDuty[sFanCtrlNum], BMU_PWM_FAN_NUM - sFanCtrlNum, HEAT_MANANG_MODBUS_PORT);
			}

			sFanCtrlNum += 4;
			//发送完成
			if(sFanCtrlNum >= BMU_PWM_FAN_NUM)
			{
				sFanCtrlNum = 0;
				//返回下一周期检测状态
				//sFanCtrlState = 0;
				/*系统调试模式*/
				if(eMODE_DEBUG == GetGWorkModeAPI())
				{
					//直接返回更新调试命令
					sFanCtrlState = 0;
				}
				/*非调试模式*/
				else
				{
					//增加等待时间(延长调速控制周期)
					sFanCtrlState = 4;
				}
			}
			break;

		//等待并确保风扇控制到位(防止RS485丢帧未控制成功)
		case 4:
			//等待7s(中途返回控制1次4s,实际全程周期7s+7s+4s)
			if(sFanCtrlTime < 7)
			{
				sFanCtrlTime++;

				//间隔4s返回控制一次
				if(0 == (sFanCtrlTime % 4))
				{
					//返回发送控制命令
					sFanCtrlNum = 0;
					sFanCtrlState = 3;
				}
				//间隔4s查询一次(防止误报通信中断)
				else if(1 == (sFanCtrlTime % 4))
				{
					//查询风扇回检信号和水浸传感器采样值
					(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_IMLU1, 0x03, MB_IMLU_RO_BASE_ADDR, 3, HEAT_MANANG_MODBUS_PORT);
				}
			}
			//等待7+4s后返回重新检测
			else
			{
				sFanCtrlNum = 0;
				sFanCtrlTime = 0;
				sFanCtrlState = 0;
			}
			break;

		default:
			sChgDhgHigT = 0;
			sChgDhgLowT = 0;
			sFanCtrlState = 0;
			break;
	}
}
#endif

#if(1 == BMU_IMEU_COM_EN)/*使能IMEU2通信交互*/
#if(1 == BMU_EU_SHUTFAN_EN)/*使能控制百叶风机*/
/*=============================================================================================
** @Name      : static void TacShutFanCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 百叶排风机通风控制策略任务
** @The notes : 1s任务,通过CAN通信控制IMEU2执行
**===========================================================================================*/
static void TacShutFanCtrlTask(void)
{
    u8 fanSate = 0;
    static u8 sFanSate = 0;
    static u8 sCtrlStep = 0;
    static u8 sCtrlTimes = 0;
    static u8 sVentTime = 0;
    static u8 sHisDate = 0;
    static u8 sSubDate = 0;

    switch(sCtrlStep)
    {
    	//发送控制百叶风机报文
		case 0:
			sVentTime++;
			SendCanImeu2SelfCtrlMsg220(0, sFanSate);
			//上电前3s连续发送
			if(BSPGetOSMillSecTimeAPI() >= 3100)
			{
				sCtrlStep = 1;
			}
			break;

		//判断控制是否完成
		case 1:
			sVentTime++;
			//IMEU2通信正常时判断控制结果
			if(0 == DiagGetPerImeuOffCommuStateAPI())
			{
				//百叶打开未到位或关闭未到位或排气风机转速异常
				if(((1 == sFanSate) && ((1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_ShutOn)) || (GetGPerImeu2SignInfoAPI(eIMEU2Info204_FanSpeed) <= 10)))
					|| ((0 == sFanSate) && ((1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_ShutOff)) || (GetGPerImeu2SignInfoAPI(eIMEU2Info204_FanSpeed) > 10))))
				{
					//控制15次(30s)仍异常
					if(sCtrlTimes >= 15)
					{
						//分别记录异常原因标志
						//开启百叶异常
						if((1 == sFanSate) && (1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_ShutOn)))
						{
							BitSet(gAuxDevErrFlag, eAuxErr_ShutOn);
						}
						//开启风机异常
						if((1 == sFanSate) && (GetGPerImeu2SignInfoAPI(eIMEU2Info204_FanSpeed) <= 10))
						{
							BitSet(gAuxDevErrFlag, eAuxErr_FanOn);
						}
						//关闭百叶异常
						if((0 == sFanSate) && (1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_ShutOff)))
						{
							BitSet(gAuxDevErrFlag, eAuxErr_ShutOff);
						}
						//关闭风机异常
						if((0 == sFanSate) && (GetGPerImeu2SignInfoAPI(eIMEU2Info204_FanSpeed) > 10))
						{
							BitSet(gAuxDevErrFlag, eAuxErr_FanOff);
						}

						//下一步
						sCtrlStep = 2;
						sCtrlTimes = 0;
					}
					//需要等待
					else
					{
						sCtrlTimes++;
						//返回再次控制
						sCtrlStep = 0;
					}
				}
				//百叶风机均正常
				else
				{
					//消除百叶风机异常标志
					BitClr(gAuxDevErrFlag, eAuxErr_ShutOn);
					BitClr(gAuxDevErrFlag, eAuxErr_FanOn);
					BitClr(gAuxDevErrFlag, eAuxErr_ShutOff);
					BitClr(gAuxDevErrFlag, eAuxErr_FanOff);
					sCtrlTimes = 0;
					//下一步
					sCtrlStep = 2;
				}
			}
			//IMEU2通信中断则直接控制
			else
			{
				//下一步
				sCtrlTimes = 0;
				sCtrlStep = 2;
			}
			break;

		//百叶控制状态变化必须的时间间隔
		case 2:
			sVentTime++;

			//间隔10s或手动控制百叶开启或关闭
			if((sCtrlTimes >= IMEU2_SHUTFAN_CTRL_T) || ((1 == gAuxShutUserCmd)
				&& (0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_ShutOff))))	//需要开启但仍处于关闭到位
			{
				//下一步
				sCtrlTimes = 0;
				sCtrlStep = 3;
			}
			else
			{
				sCtrlTimes++;
				//至少10s需要1帧报文
				if(sCtrlTimes == (IMEU2_SHUTFAN_CTRL_T % 10))
				{
					//发送命令(不能发送过快影响IMEU2其它任务)
					SendCanImeu2SelfCtrlMsg220(0, sFanSate);
				}
			}
			break;

		//判断百叶风机需求状态
		case 3:
			/*优先执行手动控制开启*/
			if(1 == gAuxShutUserCmd)
			{
				//开启百叶风机
				fanSate = 1;
				sSubDate = 0;
				sVentTime = IMEU2_SHUTFAN_VENT_T;//用于允许手动及时关闭(无需连续通风1分钟才允许关闭)

				//日期变换消除控制命令(防止忘记关闭百叶)
				if(sHisDate != GetSysDateClockAPI().day)
				{
					gAuxShutUserCmd = 0;
					sHisDate = GetSysDateClockAPI().day;
				}
			}
			/*消防已触发必须关闭风机停止气体*/
			else if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//消防告警
			{
				//关闭百叶风机
				fanSate = 0;
				sVentTime = 0;
			}
			/*可燃气体浓度告警或烟感告警开风机排出气体*/
			else if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_CGAS_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_CGAS_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE))//气体告警
				|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))))//烟雾告警
			{
				//开启百叶风机
				fanSate = 1;
				sVentTime = 0;
				sSubDate = 0;
				sHisDate = GetSysDateClockAPI().day;
			}
			else
			{
				//间隔30天通风1分钟(中午通风)
				/*if((sSubDate >= IMEU2_SHUTFAN_WAIT_T) && (GetSysDateClockAPI().hour >= 12))
				{
					//开启百叶风机
					fanSate = 1;
					sVentTime = 0;
					sSubDate = 0;
					sHisDate = GetSysDateClockAPI().day;
				}
				//正在开启需要达到通风时长1分钟
				else */if(1 == sFanSate)
				{
					//达到通风时长1分钟
					if(sVentTime >= IMEU2_SHUTFAN_VENT_T)
					{
						//关闭百叶风机
						fanSate = 0;
						sVentTime = 0;
					}
					//未达到通风时长1分钟
					else
					{
						//保持开启百叶风机
						fanSate = 1;
						sVentTime++;
					}
					sSubDate = 0;
					sHisDate = GetSysDateClockAPI().day;
				}
				//未达到通风间隔时间
				else
				{
					//保持关闭百叶风机
					fanSate = 0;
					sVentTime = 0;

					//日期变换计时1天
					if(sHisDate != GetSysDateClockAPI().day)
					{
						sSubDate++;
						sHisDate = GetSysDateClockAPI().day;
					}
				}
			}

			//控制状态变化
			if(fanSate != sFanSate)
			{
				/*产生事件*/
				EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_ShutPwr, GetGSampOutCurrAPI(), fanSate, (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT)), (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH)));
				sFanSate = fanSate;
			}
			sCtrlStep = 0;
			break;

		default:
			sCtrlStep = 0;
			break;
    }
}
#endif

#if(1 == BMU_EU_FIRELIG_EN)
/*=============================================================================================
** @Name      : static void TacRackLightCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池柜照明控制策略任务
** @The notes : 1s任务,通过CAN通信控制IMEU2执行
**===========================================================================================*/
static void TacRackLightCtrlTask(void)
{
	static u8 sTime = 0;

	sTime++;
	//电池柜门禁打开则开启照明
	if(1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_Door))
	{
		//强制开
		if(1 == gAuxRackLightCmd)
		{
			//取消强制命令
			gAuxRackLightCmd = 0;
		}
		//强制关
		else if(2 == gAuxRackLightCmd)
		{
			//照明打开或30s定时放送(防止照明无反馈信号)
			if((sTime >= 30) || (1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_Light)))
			{
				//发送关闭照明命令
				SendCanImeu2SelfCtrlMsg220(1, 0);
				sTime = 0;
			}
		}
		//照明未打开或30s定时放送(防止照明无反馈信号)
		else if((sTime >= 30) || (0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_Light)))
		{
			//发送开启照明命令
			SendCanImeu2SelfCtrlMsg220(1, 1);
			sTime = 0;
		}
	}
	//电池柜门禁关闭则关闭照明
	else
	{
		//强制关
		if(2 == gAuxRackLightCmd)
		{
			//取消强制命令
			gAuxRackLightCmd = 0;
		}
		//强制开
		else if(1 == gAuxRackLightCmd)
		{
			//照明未打开或30s定时放送(防止照明无反馈信号)
			if((sTime >= 30) || (0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_Light)))
			{
				//发送关闭照明命令
				SendCanImeu2SelfCtrlMsg220(1, 1);
				sTime = 0;
			}
		}
		//照明已打开或30s定时放送(防止照明无反馈信号)
		else if((sTime >= 30) || (1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_Light)))
		{
			//发送关闭照明命令
			SendCanImeu2SelfCtrlMsg220(1, 0);
			sTime = 0;
		}
	}
}

/*=============================================================================================
** @Name      : static void TacFireAlarmCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 消防控制器和声光报警控制器控制策略任务
** @The notes : 500ms任务,通过CAN通信控制IMEU2执行
**===========================================================================================*/
static void TacFireAlarmCtrlTask(void)
{
    static u8 sTime = 0;
    static u8 sOutFlag = 0;
    static u8 sCtrlStep = 0;

    //上电前30s不检测
    if(BSPGetOSMillSecTimeAPI() <= 30000)
    {
    	return;
    }

    sTime++;
    switch(sCtrlStep)
    {
    	//发送控制声光报警器报文
		case 0:
			/*手动控制强制开启声光报警*/
			if(1 == gAuxZHSAForceCmd)
			{
				//发送控制开启声光报警器命令
				SendCanImeu2SelfCtrlMsg220(2, 1);
			}
			/*手动控制强制关闭声光报警*/
			else if(2 == gAuxZHSAForceCmd)
			{
				//已经开启声光报警或30s定时放送(防止声光报警无反馈信号)
				if((sTime >= 60) || (1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_ZHS_A)))
				{
					//发送控制关闭声光报警器命令
					SendCanImeu2SelfCtrlMsg220(2, 0);
					sTime = 0;
				}

				//无需声光报警
				if((0 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//烟雾告警
					&& (0 == BitGet(GetGFaultInfoAPI((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE))))//消防告警
				{
					//转化为自动控制
					gAuxZHSAForceCmd = 0;
				}
			}
			/*自动控制声光报警:消防或烟感告警必须开启*/
			else if((1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//烟雾告警
				|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE))))//消防告警
			{
				//发送控制开启声光报警器命令
				SendCanImeu2SelfCtrlMsg220(2, 1);
			}
			//已经开启声光报警或30s定时放送(防止声光报警无反馈信号)
			else if((sTime >= 60) || (1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_ZHS_A)))
			{
				//发送控制关闭声光报警器命令
				SendCanImeu2SelfCtrlMsg220(2, 0);
				sTime = 0;
			}
			sCtrlStep = 1;
			break;

		//发送触发气溶胶消防控制器报文
		case 1:
			/*温度非直接极高(采样失效导致)*/
			if(GetGCellMaxTempAPI() < BATT_EXEREM_MAXT)
			{
				/*电池过温经历逐渐温升过程即使异常也视为热失控*/
				if(1 == BitGet(GetGAlarmInfoAPI(eERR_ID_DHG_TH1 / ALARM_DATATYPE), (eERR_ID_DHG_TH1 % ALARM_DATATYPE)))
				{
					BitSet(sOutFlag, 0);
				}
				else
				{
					BitClr(sOutFlag, 0);
				}
			}

			/*可燃气体告警*/
			if(1 == BitGet(GetGFaultInfoAPI((eERR_ID_CGAS_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_CGAS_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
			{
				BitSet(sOutFlag, 1);
			}
			/*系统恢复完全正常*/
			else if((eWORK_ERR != GetGWorkStateAPI()) && (GetGCellMaxTempAPI() < BATT_EXEREM_MAXT)
				&& (0 == BitGet(GetGAlarmInfoAPI(eERR_ID_DHG_TH1 / ALARM_DATATYPE), (eERR_ID_DHG_TH1 % ALARM_DATATYPE))))
			{
				BitClr(sOutFlag, 1);
			}

			/*消防烟感告警且可燃气体浓度告警且电池温度高极值必须触发消防*/
			if(((1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//烟雾告警
				|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE))))//消防告警
				&& (1 == BitGet(sOutFlag, 1)) && ((1 == BitGet(sOutFlag, 0)) || (TRUE == TacGetGAllSlaveReadyHook()))
				&& (GetGCellMaxTempAPI() >= BATT_EXEREM_MAXT) && (1 == BitGet(DiagGetGroupSosResion(0), eSOS_REASION_TEX)))
			{
				SendCanImeu2SelfCtrlMsg220(3, 1);
			}
			/*消防烟感告警且可燃气体浓度告警且电池柜壁温度高极值必须触发消防*/
			else if(((1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//烟雾告警
				|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE))))//消防告警
				&& (1 == BitGet(sOutFlag, 1)) && (FALSE == TacGetGAllSlaveReadyHook()) && ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (BATT_EXEREM_MAXT + 10))
				|| (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) >= (BATT_EXEREM_MAXT + 10)) || (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) >= (BATT_EXEREM_MAXT + 10))
				|| (GetGPerImeu2TempInfoAPI(eIMEU2Info205_TP10001) >= (BATT_EXEREM_MAXT + 10)) || (GetGPerImeu2TempInfoAPI(eIMEU2Info205_TP10002) >= (BATT_EXEREM_MAXT + 10))
				|| (GetGPerImeu2TempInfoAPI(eIMEU2Info205_TP10003) >= (BATT_EXEREM_MAXT + 10)) || (GetGPerImeu2TempInfoAPI(eIMEU2Info205_TP10004) >= (BATT_EXEREM_MAXT + 10))))//柜壁温度高
			{
				SendCanImeu2SelfCtrlMsg220(3, 1);
			}
			/*消防烟感告警和电池温度高极值全部消除则清除*/
			else if((0 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//烟雾告警
				&& (0 == BitGet(GetGFaultInfoAPI((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//消防告警
				&& (0 == BitGet(sOutFlag, 1)) && (0 == BitGet(sOutFlag, 0)) && (GetGCellMaxTempAPI() < BATT_EXEREM_MAXT)
				&& (0 == BitGet(DiagGetGroupSosResion(0), eSOS_REASION_TEX)))
			{
				//间隔30s发送1帧
				if((sTime % 30) < 2)
				{
					//强制触发
					if(1 == gAuxFireTrigCmd)
					{
						SendCanImeu2SelfCtrlMsg220(3, 1);
					}
					else
					{
						SendCanImeu2SelfCtrlMsg220(3, 0);
					}
					//间隔30s消除强制触发命令
					gAuxFireTrigCmd = 0;
				}
			}
			sCtrlStep = 0;
			break;

		default:
			sCtrlStep = 0;
			break;
    }
}
#endif

/*=============================================================================================
** @Name      : static void TacAirTHumManagTask(void)
** @Input     : void
** @Output    : voidde
** @Function  : 辅助板空调温湿度管理任务
** @The notes : 1s任务,通过CAN通信控制IMEU2执行
**===========================================================================================*/
static void TacAirTHumManagTask(void)
{
    static u8 sCtrlStep = 0;
    static u8 sCfgIndex = 0;
	static u8 sAirState = 0;	//空调开关机状态(0:关机 1:开机)
	static u8 sAirColdState = 0;//空调制冷状态(0:内循环 1:制冷)
	static u8 sAirHeatState = 0;//空调制热状态(0:内循环 1:制热)
    static s8 sAirColdOnTemp = 0;
    static s8 sAirHeatOnTemp = 0;
    static s8 sAirDehumOnHum = 0;
    static s8 sAirDehumOffHum = 0;
    static u16 sAirKeepTime = 0;	//空调控制状态保持时间(计算制冷状态间隔时间)
    static u32 sAirIdleColdT = 0;	//空调待机时制冷起始时间(判断空闲制冷时长)
    static u32 sAirColdCtrlT = 0;	//空调制冷切换控制起始时间(判断制冷持续时长,确保至少持续15min)
    static u32 sAirPwrCtrlT = 0;	//空调开关机切换控制起始时间(判断开关机持续时长,确保至少持续15min)

    //BMS初始化时等待空调信息不做控制
    if((eWORK_INIT == GetGWorkStateAPI())          //系统开机
    	|| (eWORK_OFF == GetGWorkStateAPI()))      //系统关机
	{
    	//空调通信正常且开机状态
		if((0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_AirCom))
			&& (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_Power)))
		{
//			//开关机控制状态变化(刚上电不必开机15min)
//			if(0 == sAirState)
//			{
//				//记录开机关机切换时间
//				sAirPwrCtrlT = GetSysSecondTimeAPI();
//			}
	    	//记录空调初始开机状态
			sAirState = 1;
		}
    	sCtrlStep = 0;
		return;
	}

    switch(sCtrlStep)
    {
    	/*配置空调高低温阈值(暂无需配置告警值)
		case 0:
			//过温非目标值
			if((IMEU2_AIR_HT_WARN_T != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHighT)) && (sCfgIndex <= 0))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_HTWarnT, IMEU2_AIR_HT_WARN_T);
				//配置下一个
				sCfgIndex = 1;
			}
			//黑盾空调低温非目标值
			else if((0 == sAuxDevAirType) && (IMEU2_AIR_LT_WARN_T != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirLowT)) && (sCfgIndex <= 1))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_LTWarnT, IMEU2_AIR_LT_WARN_T);
				//配置下一个
				sCfgIndex = 0;
			}
			//同飞空调高湿非目标值
			else if((1 == sAuxDevAirType) && (IMEU2_AIR_HH_WARN_H != GetGPerImeu2TempInfoAPI(eIMEU2Info205_HighHumPoint)) && (sCfgIndex <= 1))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_HHWarnH, IMEU2_AIR_HH_WARN_H);
				sAirKeepTime = 0;
				sCfgIndex = 0;
				sCtrlStep = 1;
			}
			else
			{
				sAirKeepTime = 0;
				sCfgIndex = 0;
				sCtrlStep = 1;
			}
			break;*/

		//空调故障则强制关机
		case 0:
			//空调报故障
			if((1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_AirErr))/*黑盾空调*/
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_VHL))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_THL))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirAlarm), eEU205AirA_PHL))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirAlarm), eEU205AirA_ComPres))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirState), eEU205AirS_HTAlarm_TF))/*同飞空调*/
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirState), eEU205AirS_LTAlarm_TF))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirState), eEU205AirS_HHAlarm_TF))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirAlarm), eEU205AirA_LHAlarm_TF))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirAlarm), eEU205AirA_CoilTErr_TF))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirAlarm), eEU205AirA_CondTErr_TF))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirAlarm), eEU205AirA_IntTErr_TF))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirAlarm), eEU205AirA_HPAlarm_TF))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirAlarm), eEU205AirA_LPAlarm_TF))
				|| (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TFAirAlarm), eEU205AirA_PhSeqErr_TF)))
			{
				//记录空调故障标志
				BitSet(gAuxDevErrFlag, eAuxErr_AirWarn);

				//使能空调故障保护
				#if(1 == IMEU2_AIR_ERRPRO_EN)
				//空调通讯正常且未关机
				if(/*(0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_AirCom))
					&& */(0 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_Power)))
				{
//					/*同飞空调(温度设定点为中点)*/
//					if(1 == sAuxDevAirType)
//					{
//						//空调制冷设定温度取配置中间值
//						sAirColdOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2;
//						//空调制热设定温度配置中间值
//						sAirHeatOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2;
//					}
//					/*黑盾空调(温度设定点为停止制冷温度和开启制热温度)*/
//					else
//					{
//						//空调制冷设定温度为度配置停止制冷温度值
//						sAirColdOnTemp = (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET];
//						//空调制热设定温度为度配置开启制热温度值
//						sAirHeatOnTemp = (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET];
//					}

					//清除空调制冷制热控制状态
					sAirColdState = 0;
					sAirHeatState = 0;

					//发送强制关机命令
					SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForPwr, 0);

					//控制状态变化
					if(1 == sAirState)
					{
						/*产生事件*/
						EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirPwr, GetGSampOutCurrAPI(), 0, (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT)), (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH)));
						sAirState = 0;
					}
				}
				//返回不再控制
				sAirKeepTime = 0;
				sCtrlStep = 0;

				//不考虑空调故障保护
				#else
				//进行策略检测
				sAirKeepTime = 0;
				sCtrlStep = 2;
				#endif
			}
			//空调正常(未上报空调告警)
			else
			{
				//清除空调故障标志
				BitClr(gAuxDevErrFlag, eAuxErr_AirWarn);

				//进行策略检测
				sAirKeepTime = 0;
				sCtrlStep = 2;
			}
			break;

		//检测制冷还是制热
		case 2:
			//温度正常,柜内温度或电池温度特别高,需要制冷(电池柜前仓温度偏高2℃[夏天会高很多不可信])
			if((TRUE == TacCheckAirOnTempNorState(0))
				&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + 7))
				/*|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + 9)) && (TRUE == TacCheckSensorTempNorState(0)))
				|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + 9)) && (TRUE == TacCheckSensorTempNorState(1)))*/
				|| (GetGCellAvgTempAPI() >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 4))
				|| (GetGCellMaxTempAPI() >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 6))))
			{
//				/*同飞空调(温度设定点为中点)*/
//				if(1 == sAuxDevAirType)
//				{
//					//空调制冷启动温度低于当前空调温度-1/2回差
//					sAirColdOnTemp = (s8)(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) - (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2) - 1);
//
//					//空调制热启动温度配置值中点
//					sAirHeatOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2;
//				}
//				/*黑盾空调(温度设定点为停止制冷温度)*/
//				else
//				{
//					//空调制冷启动温度低于当前空调温度-回差
//					sAirColdOnTemp = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - 1;
//
//					//空调制热启动温度配置值(开启制热温度)
//					sAirHeatOnTemp = (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET];
//				}

				//制冷状态变化
				if(0 == sAirColdState)
				{
					//记录制冷状态变化时间
					sAirColdCtrlT = GetSysSecondTimeAPI();
				}

				//记录需要制冷
				sAirColdState = 1;
				//记录不需要制热
				sAirHeatState = 0;
			}
			//温度正常,有电流(≥60A)或柜外温度低(冬季),且环境温度和电池温度都很高,需要制冷(电池柜前仓温度偏高2℃)
			else if((TRUE == TacCheckAirOnTempNorState(0))
				&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + 5))
				/*|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + 7)) && (TRUE == TacCheckSensorTempNorState(0)))
				|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + 7)) && (TRUE == TacCheckSensorTempNorState(1)))*/
				|| (GetGCellAvgTempAPI() >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2))
				|| (GetGCellMaxTempAPI() >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 4)))
				&& ((sAvgCurr1min <= -550) || (sAvgCurr1min >= 550) || (FALSE == TacCheckOutETempHighState())))
			{
//				/*同飞空调(温度设定点为中点)*/
//				if(1 == sAuxDevAirType)
//				{
//					//空调制冷启动温度低于当前空调温度-1/2回差
//					sAirColdOnTemp = (s8)(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) - (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2) - 1);
//
//					//空调制热启动温度配置值中点
//					sAirHeatOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2;
//				}
//				/*黑盾空调(温度设定点为停止制冷温度)*/
//				else
//				{
//					//空调制冷启动温度低于当前空调温度-回差
//					sAirColdOnTemp = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - 1;
//
//					//空调制热启动温度配置值(开启制热温度)
//					sAirHeatOnTemp = (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET];
//				}

				//制冷状态变化
				if(0 == sAirColdState)
				{
					//记录制冷状态变化时间
					sAirColdCtrlT = GetSysSecondTimeAPI();
				}

				//记录需要制冷
				sAirColdState = 1;
				//记录不需要制热
				sAirHeatState = 0;
			}
			//温度正常,电流较大(≥100A或柜外温度低(冬天)≥60A),且环境温度稍高或电池温度稍高,需要制冷(电池柜前仓温度偏高2℃[夏天会高很多不可信])
			else if((TRUE == TacCheckAirOnTempNorState(0))
				&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + 2))
				/*|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + 4)) && (TRUE == TacCheckSensorTempNorState(0)))
				|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + 4)) && (TRUE == TacCheckSensorTempNorState(1)))*/
				|| (GetGCellAvgTempAPI() >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1))
				|| (GetGCellMaxTempAPI() >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2)))
				&& ((sAvgCurr1min <= -950) || (sAvgCurr1min >= 950)
				|| (((sAvgCurr1min <= -550) || (sAvgCurr1min >= 550))
				&& (FALSE == TacCheckOutETempHighState()))))
			{
//				/*同飞空调(温度设定点为中点)*/
//				if(1 == sAuxDevAirType)
//				{
//					//空调制冷启动温度低于当前空调温度-1/2回差
//					sAirColdOnTemp = (s8)(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) - (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2) - 1);
//
//					//空调制热启动温度配置值中点
//					sAirHeatOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2;
//				}
//				/*黑盾空调(温度设定点为停止制冷温度)*/
//				else
//				{
//					//空调制冷启动温度低于当前空调温度-回差
//					sAirColdOnTemp = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - 1;
//
//					//空调制热启动温度配置值(开启制热温度)
//					sAirHeatOnTemp = (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET];
//				}

				//制冷状态变化
				if(0 == sAirColdState)
				{
					//记录制冷状态变化时间
					sAirColdCtrlT = GetSysSecondTimeAPI();
				}

				//记录需要制冷
				sAirColdState = 1;
				//记录不需要制热
				sAirHeatState = 0;
			}
			//温度正常,电流很大(≥120A或柜外温度低(冬天)≥100A),且环境温度或电池温度达到制冷点,需要制冷(电池柜前仓温度偏高2℃)
			else if((TRUE == TacCheckAirOnTempNorState(0))
				&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (s8)gGHardPara_104[eGHardPara104_AirColdOn_ET])
				/*|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + 2)) && (TRUE == TacCheckSensorTempNorState(0)))
				|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + 2)) && (TRUE == TacCheckSensorTempNorState(1)))*/
				|| (GetGCellAvgTempAPI() >= (s8)gGHardPara_104[eGHardPara104_AirColdOn_CT])
				|| (GetGCellMaxTempAPI() >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1)))
				&& ((sAvgCurr1min <= -1150) || (sAvgCurr1min >= 1150)
				|| (((sAvgCurr1min <= -950) || (sAvgCurr1min >= 950))
				&& (FALSE == TacCheckOutETempHighState()))))
			{
//				/*同飞空调(温度设定点为中点)*/
//				if(1 == sAuxDevAirType)
//				{
//					//空调制冷启动温度低于当前空调温度-1/2回差
//					sAirColdOnTemp = (s8)(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) - (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2) - 1);
//
//					//空调制热启动温度配置值中点
//					sAirHeatOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2;
//				}
//				/*黑盾空调(温度设定点为停止制冷温度)*/
//				else
//				{
//					//空调制冷启动温度低于当前空调温度-回差
//					sAirColdOnTemp = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - 1;
//
//					//空调制热启动温度配置值(开启制热温度)
//					sAirHeatOnTemp = (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET];
//				}

				//制冷状态变化
				if(0 == sAirColdState)
				{
					//记录制冷状态变化时间
					sAirColdCtrlT = GetSysSecondTimeAPI();
				}

				//记录需要制冷
				sAirColdState = 1;
				//记录不需要制热
				sAirHeatState = 0;
			}
			//温度正常,且环境温度很低或电池温度很低,需要制热(电池柜前仓温度偏高2℃)
			else if((TRUE == TacCheckAirOnTempNorState(1))
				&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET])
				/*|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) <= (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]) && (TRUE == TacCheckSensorTempNorState(0)))
				|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) <= (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]) && (TRUE == TacCheckSensorTempNorState(1)))*/
				|| (GetGCellMinTempAPI() <= (s8)gGHardPara_104[eGHardPara104_AirHeatOn_CT])))
			{
//				/*同飞空调(温度设定点为中点)*/
//				if(1 == sAuxDevAirType)
//				{
//					//空调制热启动温度高于当前空调温度+1/2回差
//					sAirHeatOnTemp = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + ((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2 + 1;
//
//					//空调制冷启动温度默认值(取中间值)
//					sAirColdOnTemp = ((s16)gGHardPara_104[eGHardPara104_AirColdOn_ET] + (s16)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2;
//				}
//				/*黑盾空调(温度设定点为开启制热温度)*/
//				else
//				{
//					//空调制热启动温度高于当前空调温度
//					sAirHeatOnTemp = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 1;
//
//					//空调制冷设定温度配置值(停止制冷温度)
//					sAirColdOnTemp = (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET];
//				}

				//记录需要制热
				sAirHeatState = 1;
				//记录不需要制冷
				sAirColdState = 0;
			}
			//环境温度和电池温度均正常达到关闭制冷制热温度
			else
			{
				//温度异常,或达到关闭制冷温度
				if((FALSE == TacCheckAirOnTempNorState(0))
					|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET])
					/*&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] + 1)) || (FALSE == TacCheckSensorTempNorState(0)))
					&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] + 1)) || (FALSE == TacCheckSensorTempNorState(1)))*/
					&& (GetGCellAvgTempAPI() <= (s8)gGHardPara_104[eGHardPara104_AirColdOff_CT])
					&& (GetGCellMaxTempAPI() <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_CT] + 2)))
					|| ((GetSysSecondTimeAPI() >= (sAirIdleColdT + 700))//非充放电状态(≤60A)已制冷12min
					&& (GetGSampOutCurrAPI() >= -550) && (GetGSampOutCurrAPI() <= 550)//无瞬时电流即未重新开始充放电(≤60A)
					&& (((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] + 3))
					/*&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] + 4)) || (FALSE == TacCheckSensorTempNorState(0)))
					&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] + 4)) || (FALSE == TacCheckSensorTempNorState(1)))*/
					&& (GetGCellAvgTempAPI() <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_CT] + 1))
					&& (GetGCellMaxTempAPI() <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_CT] + 3)))
					|| ((TRUE == TacCheckOutETempHighState())//柜外温度高时无需降温太低
					&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] + 5))
					/*&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] + 6)) || (FALSE == TacCheckSensorTempNorState(0)))
					&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] + 6)) || (FALSE == TacCheckSensorTempNorState(1)))*/
					&& (GetGCellAvgTempAPI() <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_CT] + 2))
					&& (GetGCellMaxTempAPI() <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_CT] + 4))))))
				{
//					/*同飞空调(温度设定点为中点)*/
//					if(1 == sAuxDevAirType)
//					{
//						//空调制冷设定温度取配置中间值
//						sAirColdOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2;
//					}
//					/*黑盾空调(温度设定点为停止制冷温度)*/
//					else
//					{
//						//空调制冷设定温度为度配置停止制冷温度值
//						sAirColdOnTemp = (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET];
//					}

					//制冷状态变化
					if(1 == sAirColdState)
					{
						//记录制冷状态变化时间
						sAirColdCtrlT = GetSysSecondTimeAPI();
					}

					//记录关闭空调制冷
					sAirColdState = 0;
				}

				//温度异常,达到关闭制热温度
				if((FALSE == TacCheckAirOnTempNorState(1))
					|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET])
					/*&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) >= ((s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] + 2)) || (FALSE == TacCheckSensorTempNorState(0)))
					&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) >= ((s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] + 2)) || (FALSE == TacCheckSensorTempNorState(1)))*/
					&& ((GetGCellMinTempAPI() >= (s8)gGHardPara_104[eGHardPara104_AirHeatOff_CT])
					|| (GetGCellAvgTempAPI() >= ((s8)gGHardPara_104[eGHardPara104_AirHeatOff_CT] + 3)))))
				{
//					/*同飞空调(温度设定点为中点)*/
//					if(1 == sAuxDevAirType)
//					{
//						//空调制热设定温度配置中间值
//						sAirHeatOnTemp =  ((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2;
//					}
//					/*黑盾空调(温度设定点为开启制热温度)*/
//					else
//					{
//						//空调制热设定温度为度配置开启制热温度值
//						sAirHeatOnTemp = (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET];
//					}

					//记录关闭空调制热
					sAirHeatState = 0;
				}
			}

			/*检测同飞空调参数有效性(温度设定点为中点)*/
			if(1 == sAuxDevAirType)
			{
				//根据制冷需求状态转化为控制温度(非强制控制)
				if(1 == sAirColdState)
				{
					//空调制冷启动温度低于当前空调温度-1/2回差
					//sAirColdOnTemp = (s8)(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) - (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2) - 1);
					//温度下降导致设定值变化会导致空调停机,直接设置固定值
					sAirColdOnTemp = 20;
				}
				else
				{
					//当前温度高于配置温度(关停不了制冷)
					if(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET])
					{
						//空调制冷设定温度比当前温度高1/回差
						//sAirColdOnTemp = (s8)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2) + 1;
						//防止温度上升自动开启需多次修改值,直接设置固定值
						sAirColdOnTemp = 32;
					}
					else
					{
						//空调制冷设定温度取配置中间值
						//sAirColdOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2;
						//防止温度上升自动开启需多次修改值,直接设置固定值
						sAirColdOnTemp = 32;
					}
				}

				//根据制热需求状态转化为控制温度(非强制控制)
				if(1 == sAirHeatState)
				{
					//空调制热启动温度高于当前空调温度+1/2回差
					sAirHeatOnTemp = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + ((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2 + 1;
				}
				else
				{
					//当前温度低于配置温度(关停不了制热)
					if(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET])
					{
						//空调制热设定温度比当前温度低1/2回差
						sAirHeatOnTemp = (s8)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) - (((s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]) / 2) - 1;
					}
					else
					{
						//空调制热设定温度配置中间值
						sAirHeatOnTemp =  ((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2;
					}
				}

				//制冷温度不超过设定值
				if((1 == sAirColdState)//开启空调要比设定温度底2℃
					&& (sAirColdOnTemp > (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2 - 2)))
				{
					sAirColdOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2 - 2;
				}
				else if((0 == sAirColdState)//关闭空调要确保当前温度低于开启温度1℃
					&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (sAirColdOnTemp + (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2) - 1)))
				{
					//设定值提高2℃
					if(sAirColdOnTemp <= 30)
					{
						sAirColdOnTemp += 2;
					}
					else
					{
						sAirColdOnTemp = 32;
					}

					//不能低于设定值
					if(sAirColdOnTemp < (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2))
					{
						sAirColdOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2;
					}
				}
				//制冷温度最低20℃
				if(sAirColdOnTemp < 20)
				{
					sAirColdOnTemp = 20;
				}

				//制热温度不低于设定值
				if(sAirHeatOnTemp < (((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2))
				{
					sAirHeatOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2;
				}
				//制热温度最高25℃(规格书限制35℃)
				if(sAirHeatOnTemp > 25)
				{
					sAirHeatOnTemp = 25;
				}
			}
			/*检测黑盾空调参数有效性(温度设定点为停止制冷温度和开启制热温度)*/
			else
			{
				//根据制冷需求状态转化为控制温度(非强制控制)
				if(1 == sAirColdState)
				{
					//空调制冷启动温度低于当前空调温度-回差
					//sAirColdOnTemp = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - 1;
					//温度下降导致设定值变化会导致空调停机,直接设置固定值
					sAirColdOnTemp = 18;
				}
				else
				{
					//当前温度高于配置温度(关停不了制冷)
					if(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET])
					{
						//空调制冷设定温度比当前温度高
						//sAirColdOnTemp = (s8)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 1;
						//防止温度上升自动开启需多次修改值,直接设置固定值
						sAirColdOnTemp = 30;
					}
					//当前温度低于配置停止温度
					else
					{
						//空调制冷设定温度为配置停止制冷温度值
						//sAirColdOnTemp = (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET];
						//防止温度上升自动开启需多次修改值,直接设置固定值
						sAirColdOnTemp = 30;
					}
				}
				//根据制热需求状态转化为控制温度(非强制控制)
				if(1 == sAirHeatState)
				{
					//空调制热启动温度高于当前空调温度
					sAirHeatOnTemp = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 1;
				}
				else
				{
					//当前温度低于配置温度(关停不了制热)
					if(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET])
					{
						//空调制热设定温度比当前温度低回差
						sAirHeatOnTemp = (s8)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] - 1;
					}
					else
					{
						//空调制热设定温度为配置开启制热温度值
						sAirHeatOnTemp = (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET];
					}
				}

				//制冷温度不超过设定值
				if((1 == sAirColdState)//开启空调要比设定温度底3℃
					&& (sAirColdOnTemp > ((s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] - 3)))
				{
					//确保停止温度比配置值低3℃(防止温度下降过快自动停止)
					sAirColdOnTemp = (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] - 3;
				}
				else if((0 == sAirColdState)//关闭空调要确保当前温度低于开启温度1℃
					&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (sAirColdOnTemp + ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) - 1)))
				{
					//设定值提高2℃
					if(sAirColdOnTemp <= 28)
					{
						sAirColdOnTemp += 2;
					}
					else
					{
						sAirColdOnTemp = 30;
					}

					//不能低于设定值
					if(sAirColdOnTemp < (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET])
					{
						sAirColdOnTemp = (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET];
					}
				}
				//制冷温度最低16℃
				if(sAirColdOnTemp < 16)
				{
					sAirColdOnTemp = 16;
				}

				//制热温度不低于设定值
				if(sAirHeatOnTemp < (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET])
				{
					sAirHeatOnTemp = (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET];
				}
				//制热温度最高26℃
				if(sAirHeatOnTemp > 26)
				{
					sAirHeatOnTemp = 26;
				}
			}

			sCfgIndex = 0;
			sCtrlStep = 3;
			break;

		//配置空调制冷制热温度
		case 3:
			//制冷设定温度非目标值
			if((sAirColdOnTemp != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirConT)) && (sCfgIndex <= 0))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ColdOnT, sAirColdOnTemp);
				//配置下一个
				sCfgIndex = 1;
			}
			//黑盾空调制冷偏移温度非目标值(配置回差)
			else if((0 == sAuxDevAirType) && (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirCoffT)) && (sCfgIndex <= 1))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ColdOffT, (s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]);
				//配置下一个
				sCfgIndex = 2;
			}
			//同飞空调制冷偏移温度非目标值(1/2配置回差,低于20℃至少偏移3℃)
			else if((1 == sAuxDevAirType) && (sAirColdOnTemp <= 20) && ((s8)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= 20) && ((((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2) < 3) && (3 != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirCoffT)) && (sCfgIndex <= 1))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ColdOffT, 3);
				//配置下一个
				sCfgIndex = 2;
			}
			//同飞空调制冷偏移温度非目标值(1/2配置回差)
			else if((1 == sAuxDevAirType) && ((((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2) != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirCoffT)) && (sCfgIndex <= 1))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ColdOffT, ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2);
				//配置下一个
				sCfgIndex = 2;
			}
			//制热设定温度非目标值
			else if((sAirHeatOnTemp != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHonT)) && (sCfgIndex <= 2))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_HeatOnT, sAirHeatOnTemp);
				//配置下一个
				sCfgIndex = 3;
			}
			//黑盾空调制热偏移温度非目标值(配置回差)
			else if((0 == sAuxDevAirType) && (((s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]) != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHoffT)) && (sCfgIndex <= 3))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_HeatOffT, (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]);
				//配置完成
				sCfgIndex = 0;
				sCtrlStep = 4;
			}
			//同飞空调制热偏移温度非目标值(1/2配置回差)
			else if((1 == sAuxDevAirType) && ((((s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]) / 2) != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHoffT)) && (sCfgIndex <= 3))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_HeatOffT, ((s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]) / 2);
				//配置完成
				sCfgIndex = 0;
				sCtrlStep = 4;
			}
			else
			{
				sCfgIndex = 0;
				sCtrlStep = 4;
			}
			break;

		//检测是否需要除湿(制冷优先于除湿加热,即同时满足制冷和除湿加热条件时优先制冷,制冷停止后转为除湿加热,加热到制冷启动温度又转为开启制冷)
		case 4:
			//非制冷制热状态不除湿
			if((0 == sAirColdState) && (0 == sAirHeatState))
			{
				/*同飞空调(湿度设定点为中间点)*/
				if(1 == sAuxDevAirType)
				{
					//空调除湿启动湿度低于当前空调湿度
					sAirDehumOnHum = 80;
					//空调除湿湿度回差为1/2配置值
					sAirDehumOffHum = 20;
				}
				/*黑盾空调(湿度设定点除湿启动点)*/
				else
				{
					//空调除湿启动湿度低于当前空调湿度
					sAirDehumOnHum = 99;
					//空调除湿停止湿度回差为配置回差
					sAirDehumOffHum = 60;
				}
			}
			//环境湿度高需要除湿
			else if((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) >= gGHardPara_104[eGHardPara104_AirDeHumOn_H])
				|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) >= (gGHardPara_104[eGHardPara104_AirDeHumOff_H] - 5)) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) >= (s8)gGHardPara_104[eGHardPara104_AirDeHumOn_H]) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) <= 100) && (0 == BitGet(gAuxComErrFlag, eAuxComErr_TH1)))
				|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) >= (gGHardPara_104[eGHardPara104_AirDeHumOff_H] - 5)) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH2) >= gGHardPara_104[eGHardPara104_AirDeHumOn_H]) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH2) <= 100) && (0 == BitGet(gAuxComErrFlag, eAuxComErr_TH2))))
			{
				/*同飞空调(湿度设定点为中间点)*/
				if(1 == sAuxDevAirType)
				{
					//空调除湿湿度回差为1/2配置值
					sAirDehumOffHum = (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2;

					//空调除湿启动湿度低于当前空调湿度
					//sAirDehumOnHum = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) - ((gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2) - 1;
					sAirDehumOnHum = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) - sAirDehumOffHum - 1;

					//除湿开启湿度不超过设定值中点
					if(sAirDehumOnHum > (((u16)gGHardPara_104[eGHardPara104_AirDeHumOn_H] + gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2))
					{
						sAirDehumOnHum = ((u16)gGHardPara_104[eGHardPara104_AirDeHumOn_H] + gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2;
					}
					//除湿开启湿度最低50%
					else if(sAirDehumOnHum < 50)
					{
						sAirDehumOnHum = 50;
					}
				}
				/*黑盾空调(湿度设定点除湿启动点)*/
				else
				{
					//空调除湿启动湿度低于当前空调湿度
					sAirDehumOnHum = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) - 1;
					//空调除湿停止湿度回差为配置回差
					sAirDehumOffHum = sAirDehumOnHum - (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]);

					//除湿开启湿度不超过设定值
					if(sAirDehumOnHum > (s8)gGHardPara_104[eGHardPara104_AirDeHumOn_H])
					{
						sAirDehumOnHum = (s8)gGHardPara_104[eGHardPara104_AirDeHumOn_H];
					}
					//除湿开启湿度最低50%
					else if(sAirDehumOnHum < 50)
					{
						sAirDehumOnHum = 50;
					}

					//除湿关闭湿度不超过设定值
					if(sAirDehumOffHum > (s8)gGHardPara_104[eGHardPara104_AirDeHumOff_H])
					{
						sAirDehumOffHum = (s8)gGHardPara_104[eGHardPara104_AirDeHumOff_H];
					}
					//除湿关闭湿度最低45%
					else if(sAirDehumOffHum < 45)
					{
						sAirDehumOffHum = 45;
					}
				}
			}
			//湿度低时要根据电池柜内外温差进一步降低湿度防止凝露
			else
			{
//				//柜外环境温度高于柜内温度10℃需要将湿度控制到60%以下(防止凝露)
//				if(((GetGPerEnvirAvgTempAPI() >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 10))
//					|| ((GetGPerEnvirAvgTempAPI() >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) + 10)) && (0 == BitGet(gAuxComErrFlag, eAuxComErr_TH1)))
//					|| ((GetGPerEnvirAvgTempAPI() >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) + 10)) && (0 == BitGet(gAuxComErrFlag, eAuxComErr_TH2))))
//					&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) >= 52)
//					|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) >= 52) && (0 == BitGet(gAuxComErrFlag, eAuxComErr_TH1)))
//					|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH2) >= 52) && (0 == BitGet(gAuxComErrFlag, eAuxComErr_TH2)))))
//				{
//					/*同飞空调(湿度设定点为中间点)*/
//					if(1 == sAuxDevAirType)
//					{
//						//空调除湿湿度回差为1/2配置值
//						sAirDehumOffHum = (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2;
//
//						//空调除湿启动湿度60%-1/2回差
//						//sAirDehumOnHum = 60 - ((gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2);
//						sAirDehumOnHum = 60 - sAirDehumOffHum;
//
//						//最小50%
//						if(sAirDehumOnHum < 50)
//						{
//							sAirDehumOnHum = 50;
//						}
//					}
//					/*黑盾空调(湿度设定点除湿启动点)*/
//					else
//					{
//						//空调除湿启动湿度50%
//						sAirDehumOnHum = 60;
//						sAirDehumOffHum = 55;
//					}
//				}
//				//柜外环境温度高于柜内温度5℃需要将湿度控制到60%以下(防止凝露)
//				else if(((GetGPerEnvirAvgTempAPI() >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 5))
//					|| ((GetGPerEnvirAvgTempAPI() >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) + 5)) && (0 == BitGet(gAuxComErrFlag, eAuxComErr_TH1)))
//					|| ((GetGPerEnvirAvgTempAPI() >= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) + 5)) && (0 == BitGet(gAuxComErrFlag, eAuxComErr_TH2))))
//					&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) >= 67)
//					|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) >= 67) && (0 == BitGet(gAuxComErrFlag, eAuxComErr_TH1)))
//					|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH2) >= 67) && (0 == BitGet(gAuxComErrFlag, eAuxComErr_TH2)))))
//				{
//					/*同飞空调(湿度设定点为中间点)*/
//					if(1 == sAuxDevAirType)
//					{
//						//空调除湿湿度回差为1/2配置值
//						sAirDehumOffHum = (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2;
//
//						//空调除湿启动湿度70%-1/2回差
//						//sAirDehumOnHum = 70 - ((gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2);
//						sAirDehumOnHum = 60 - sAirDehumOffHum;
//
//						//最小50%
//						if(sAirDehumOnHum < 50)
//						{
//							sAirDehumOnHum = 50;
//						}
//					}
//					/*黑盾空调(湿度设定点除湿启动点)*/
//					else
//					{
//						//空调除湿启动湿度70%
//						sAirDehumOnHum = 70;
//						sAirDehumOffHum = 65;
//					}
//				}
				//柜内外温差小或湿度低需要关闭除湿
				/*else */if((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) <= (gGHardPara_104[eGHardPara104_AirDeHumOff_H] - 20))
					|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) <= gGHardPara_104[eGHardPara104_AirDeHumOff_H])
					&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) <= gGHardPara_104[eGHardPara104_AirDeHumOff_H]) || (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) > 100) || (1 == BitGet(gAuxComErrFlag, eAuxComErr_TH1)))
					&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH2) <= gGHardPara_104[eGHardPara104_AirDeHumOff_H]) || (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH2) > 100) || (1 == BitGet(gAuxComErrFlag, eAuxComErr_TH2))))
					|| ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) <= (gGHardPara_104[eGHardPara104_AirDeHumOff_H] - 10))
					&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) <= (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - 5)) || (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) > 100) || (1 == BitGet(gAuxComErrFlag, eAuxComErr_TH1)))
					&& ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH2) <= (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - 5)) || (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH2) > 100) || (1 == BitGet(gAuxComErrFlag, eAuxComErr_TH2)))))
				{
					/*同飞空调(湿度设定点为中间点)*/
					if(1 == sAuxDevAirType)
					{
						//空调除湿启动湿度配置值中点
						sAirDehumOnHum = ((u16)gGHardPara_104[eGHardPara104_AirDeHumOn_H] + (u16)gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2;

						//空调除湿湿度回差为1/2配置值
						sAirDehumOffHum = (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2;
					}
					/*黑盾空调(湿度设定点除湿启动点)*/
					else
					{
						//空调除湿启动湿度为配置值
						sAirDehumOnHum = gGHardPara_104[eGHardPara104_AirDeHumOn_H];
						//空调除湿停止湿度为配置值
						sAirDehumOffHum = gGHardPara_104[eGHardPara104_AirDeHumOff_H];
					}
				}
				/*无需除湿启停切换,未计算处于无效值需要赋初值*/
				else
				{
					/*同飞空调(湿度设定点为中间点)*/
					if(1 == sAuxDevAirType)
					{
						//除湿开启湿度最低50%
						if((sAirDehumOnHum < 50) || (sAirDehumOnHum > (((u16)gGHardPara_104[eGHardPara104_AirDeHumOn_H] + gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2)))
						{
							//空调除湿启动湿度配置值中点
							sAirDehumOnHum = ((u16)gGHardPara_104[eGHardPara104_AirDeHumOn_H] + (u16)gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2;

							//空调除湿湿度回差为1/2配置值
							sAirDehumOffHum = (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2;
						}
					}
					/*黑盾空调(湿度设定点为除湿启动点)*/
					else
					{
						//除湿开启/关闭湿度无效
						if((sAirDehumOnHum < 50) || (sAirDehumOnHum > (s8)gGHardPara_104[eGHardPara104_AirDeHumOn_H])
							|| (sAirDehumOffHum < 45) || (sAirDehumOffHum > (s8)gGHardPara_104[eGHardPara104_AirDeHumOff_H]))
						{
							//取配置值
							sAirDehumOnHum = (s8)gGHardPara_104[eGHardPara104_AirDeHumOn_H];
							sAirDehumOffHum = (s8)gGHardPara_104[eGHardPara104_AirDeHumOff_H];
						}
					}
				}
			}
			sAirKeepTime = 0;
			sCfgIndex = 0;
			sCtrlStep = 5;
			break;

		//配置空调除湿温度湿度(包括重新检测其他参数)
		case 5:
			//计数空调保持当前状态时间(确保空调压缩机启停均间隔5分钟)
			sAirKeepTime++;

			//间隔5s执行一次(IMEU2不支持通信过快)
			if(1 != (sAirKeepTime % 5))
			{
				return;
			}

			//黑盾空调除湿开启温度非目标值
			if((0 == sAuxDevAirType) && ((s8)gGHardPara_104[eGHardPara104_AirDeHumOn_T] != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDonT)) && (sCfgIndex <= 0))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_DehOnT, (s8)gGHardPara_104[eGHardPara104_AirDeHumOn_T]);
				//配置下一个
				sCfgIndex = 1;
			}
			//黑盾空调除湿停止温度非目标值
			else if((0 == sAuxDevAirType) && ((s8)gGHardPara_104[eGHardPara104_AirDeHumOff_T] != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDoffT)) && (sCfgIndex <= 1))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_DehOffT, (s8)gGHardPara_104[eGHardPara104_AirDeHumOff_T]);
				//配置下一个
				sCfgIndex = 2;
			}
			//除湿开启湿度非目标值
			else if((sAirDehumOnHum != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDonH)) && (sCfgIndex <= 2))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_DehOnH, sAirDehumOnHum);
				//配置下一个
				sCfgIndex = 3;
			}
			//黑盾空调除湿停止湿度非目标值
			else if((0 == sAuxDevAirType) && (sAirDehumOffHum != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirDoffH)) && (sCfgIndex <= 3))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_DehOffH, sAirDehumOffHum);
				//配置下一个
				sCfgIndex = 4;
			}
			//同飞空调除湿回差值
			else if((1 == sAuxDevAirType) && (sAirDehumOffHum != GetGPerImeu2TempInfoAPI(eIMEU2Info205_DehumDiff)) && (sCfgIndex <= 3))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_DehumDiff, sAirDehumOffHum);
				//配置下一个
				sCfgIndex = 4;
			}
			//强制模式使能
			#if(1 == IMEU2_AIR_FORCMD_EN)
			//需要开启空调制冷未处于前置制冷模式
			else if((1 == sAirColdState) && (0 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_ColdM)) && (sCfgIndex <= 4))
			{
				//发送强制制冷模式命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForCold, 1);
				//配置下一个
				sCfgIndex = 5;
			}
			//需要开启空调制热未处于前置制热模式
			else if((1 == sAirHeatState) && (0 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_HeatM)) && (sCfgIndex <= 4))
			{
				//发送强制制热模式命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForHeat, 1);
				//配置下一个
				sCfgIndex = 5;
			}
			//不需要空调制冷和制热,环境温度高则强制送风模式
			else if((0 == sAirColdState) && (0 == sAirHeatState) && ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (s8)IMEU2_AIR_COLD_MIN_T) || (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_ColdM))) && (0 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_WindM)) && (sCfgIndex <= 4))
			{
				//发送强制送风模式命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForBlow, 1);
				//配置下一个
				sCfgIndex = 5;
			}
			//不需要空调制冷和制热,环境温度低则强制待机风模式
			else if((0 == sAirColdState) && (0 == sAirHeatState) && ((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) < ((s8)IMEU2_AIR_COLD_MIN_T - 5)) || (1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_HeatM))) && (0 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_WaitM)) && (sCfgIndex <= 4))
			{
				//发送强制待机模式命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForIdle, 1);
				//配置下一个
				sCfgIndex = 5;
			}
			#else
			//制冷设定温度非目标值
			else if((sAirColdOnTemp != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirConT)) && (sCfgIndex <= 4))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ColdOnT, sAirColdOnTemp);
				//配置下一个
				sCfgIndex = 5;
			}
			//黑盾空调制冷偏移温度非目标值(配置回差)
			else if((0 == sAuxDevAirType) && (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirCoffT)) && (sCfgIndex <= 5))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ColdOffT, (s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]);
				//配置下一个
				sCfgIndex = 6;
			}
			//同飞空调制冷偏移温度非目标值(1/2配置回差,低于20℃至少偏移3℃)
			else if((1 == sAuxDevAirType) && (sAirColdOnTemp <= 20) && ((s8)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= 20) && ((((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2) < 3) && (3 != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirCoffT)) && (sCfgIndex <= 5))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ColdOffT, 3);
				//配置下一个
				sCfgIndex = 6;
			}
			//同飞空调制冷偏移温度非目标值(1/2配置回差)
			else if((1 == sAuxDevAirType) && ((((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2) != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirCoffT)) && (sCfgIndex <= 5))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_ColdOffT, ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2);
				//配置下一个
				sCfgIndex = 6;
			}
			#endif
			/*过温非目标值(使用默认值暂不配置)
			else if((IMEU2_AIR_HT_WARN_T != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHighT)) && (sCfgIndex <= 6))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_HTWarnT, IMEU2_AIR_HT_WARN_T);
				//配置下一个
				sCfgIndex = 7;
			}
			//黑盾空调低温非目标值(使用默认值暂不配置)
			else if((0 == sAuxDevAirType) && (IMEU2_AIR_LT_WARN_T != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirLowT)) && (sCfgIndex <= 7))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_LTWarnT, IMEU2_AIR_LT_WARN_T);
				//配置下一个
				sCfgIndex = 8;
			}
			//同飞空调高湿非目标值
			else if((1 == sAuxDevAirType) && (IMEU2_AIR_HH_WARN_H != GetGPerImeu2TempInfoAPI(eIMEU2Info205_HighHumPoint)) && (sCfgIndex <= 8))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_HHWarnH, IMEU2_AIR_HH_WARN_H);
				//配置下一个
				sCtrlStep = 9;
			}*/
			//强制模式未使能(自动模式)
			#if(0 == IMEU2_AIR_FORCMD_EN)
			//制热设定温度非目标值
			else if((sAirHeatOnTemp != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHonT)) && (sCfgIndex <= 9))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_HeatOnT, sAirHeatOnTemp);
				//配置下一个
				sCfgIndex = 10;
			}
			//黑盾空调制热偏移温度非目标值(配置回差)
			else if((0 == sAuxDevAirType) && (((s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]) != GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHoffT)) && (sCfgIndex <= 10))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_HeatOffT, (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]);
				//配置完成
				sCfgIndex = 11;
			}
			//同飞空调制热偏移温度非目标值(1/2配置回差)
			else if((1 == sAuxDevAirType) && ((((s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]) / 2)!= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirHoffT)) && (sCfgIndex <= 10))
			{
				//发送配置命令
				SendCanImeu2AirCtrlMsg221(eEuAirCmd_HeatOffT, ((s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET] - (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET]) / 2);
				//配置完成
				sCfgIndex = 11;
			}
			#endif
			//全部参数设置一遍
			else
			{
				//强制模式未使能(自动模式)(需要调节制冷制热点防止自动停机)
				#if(0 == IMEU2_AIR_FORCMD_EN)
				//需要制冷
				if(1 == sAirColdState)
				{
//					//开启空调改变设定值会导致压缩机停止
//					//当前空调温度即将下降到停止温度
//					if(((1 == sAuxDevAirType) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= ((sAirColdOnTemp - (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2)) + 1)))/*同飞空调(温度设定点为中点)*/
//						|| ((0 == sAuxDevAirType) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= (sAirColdOnTemp + 1))))/*黑盾空调(温度设定点为停止制冷温度)*/
//					{
//						/*同飞空调温度设定点为中点,最低20℃*/
//						if(1 == sAuxDevAirType)
//						{
//							//设定值下降2℃
//							if(sAirColdOnTemp >= 22)
//							{
//								sAirColdOnTemp -= 2;
//							}
//							else
//							{
//								sAirColdOnTemp = 20;
//							}
//						}
//						/*黑盾空调温度设定点为制冷停止,最低16℃*/
//						else
//						{
//							//设定值下降2℃
//							if(sAirColdOnTemp >= 18)
//							{
//								sAirColdOnTemp -= 2;
//							}
//							else
//							{
//								sAirColdOnTemp = 16;
//							}
//						}
//					}
				}
				//停止制冷
				else
				{
					//当前空调温度即将上升到开启温度
					if(((1 == sAuxDevAirType) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= ((sAirColdOnTemp + (((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2)) - 1)))/*同飞空调(温度设定点为中点)*/
						|| ((0 == sAuxDevAirType) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (sAirColdOnTemp + ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] - (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) - 1))))/*黑盾空调(温度设定点为停止制冷温度)*/
					{
						//设定值提高2℃
						if(sAirColdOnTemp <= 30)
						{
							sAirColdOnTemp += 2;
						}
						else
						{
							sAirColdOnTemp = 32;
						}
					}
				}
				#endif

//				//开启百叶排风机或打开柜门需要除湿(更新除湿参数)
//				if((GetGPerImeu2SignInfoAPI(eIMEU2Info204_FanSpeed) >= 10)
//					|| (1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_ShutOff))
//					|| (1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_Door)))
//				{
//					/*同飞空调(湿度设定点为中间点)*/
//					if(1 == sAuxDevAirType)
//					{
//						//空调除湿湿度回差为1/2配置值
//						sAirDehumOffHum = (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2;
//
//						//空调除湿启动湿度低于当前空调湿度
//						//sAirDehumOnHum = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) - ((gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2) - 1;
//						sAirDehumOnHum = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) - sAirDehumOffHum - 1;
//
//						//除湿开启湿度不超过设定值中点
//						if(sAirDehumOnHum > (((u16)gGHardPara_104[eGHardPara104_AirDeHumOn_H] + gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2))
//						{
//							sAirDehumOnHum = ((u16)gGHardPara_104[eGHardPara104_AirDeHumOn_H] + gGHardPara_104[eGHardPara104_AirDeHumOff_H]) / 2;
//						}
//						//除湿开启湿度最低50%
//						else if(sAirDehumOnHum < 50)
//						{
//							sAirDehumOnHum = 50;
//						}
//					}
//					/*黑盾空调(湿度设定点除湿启动点)*/
//					else
//					{
//						//空调除湿启动湿度低于当前空调湿度
//						sAirDehumOnHum = GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) - 1;
//						//空调除湿停止湿度回差为配置回差
//						sAirDehumOffHum = sAirDehumOnHum - (gGHardPara_104[eGHardPara104_AirDeHumOn_H] - gGHardPara_104[eGHardPara104_AirDeHumOff_H]);
//
//						//除湿开启湿度不超过设定值
//						if(sAirDehumOnHum > (s8)gGHardPara_104[eGHardPara104_AirDeHumOn_H])
//						{
//							sAirDehumOnHum = (s8)gGHardPara_104[eGHardPara104_AirDeHumOn_H];
//						}
//						//除湿开启湿度最低50%
//						else if(sAirDehumOnHum < 50)
//						{
//							sAirDehumOnHum = 50;
//						}
//
//						//除湿关闭湿度不超过设定值
//						if(sAirDehumOffHum > (s8)gGHardPara_104[eGHardPara104_AirDeHumOff_H])
//						{
//							sAirDehumOffHum = (s8)gGHardPara_104[eGHardPara104_AirDeHumOff_H];
//						}
//						//除湿关闭湿度最低45%
//						else if(sAirDehumOffHum < 45)
//						{
//							sAirDehumOffHum = 45;
//						}
//					}
//				}

				//记录非充放电状态开启空调时间
				if((0 == sAirColdState) || (0 == sAirIdleColdT) || (sAirIdleColdT > GetSysSecondTimeAPI())
					|| (sAvgCurr1min <= -550) || (sAvgCurr1min >= 550))
				{
					//充放电状态或关闭空调时更新时间
					sAirIdleColdT = GetSysSecondTimeAPI();
				}

				//重新检测参数是否配置成功
				sCfgIndex = 0;
				//进入开关机逻辑
				sCtrlStep = 6;
			}
			break;

		//判断是否需要开关机(无制冷/制热/除湿且无需通风即关机)
		case 6:
			//计数空调保持当前状态时间(确保空调开关机间隔5分钟)
			sAirKeepTime++;

			//间隔5s执行一次(IMEU2不支持通信过快)
			if(2 != (sAirKeepTime % 5))
			{
				return;
			}

			//电池热失控温度异常或极高或有气体告警或消防烟雾告警则强制关闭空调
			if((FALSE == TacGetGAllSlaveReadyHook()) || ((GetGCellMaxTempAPI() >= BATT_EXEREM_MAXT) && (1 == BitGet(DiagGetGroupSosResion(0), eSOS_REASION_TEX)))//电池温度异常
				|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_CGAS_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_CGAS_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//气体告警
				|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))//烟雾告警
				|| (1 == BitGet(GetGFaultInfoAPI((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_FIRE_EXP - eERR_ID_ERRSTART) % FAULT_DATATYPE))))//消防告警
			{
				//空调通信正常且开机状态
				if(/*(0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_AirCom))
					&& */(1 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_Power)))
				{
//					/*同飞空调(温度设定点为中点)*/
//					if(1 == sAuxDevAirType)
//					{
//						//空调制冷设定温度取配置中间值
//						sAirColdOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirColdOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET]) / 2;
//						//空调制热设定温度配置中间值
//						sAirHeatOnTemp = ((s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET] + (s8)gGHardPara_104[eGHardPara104_AirHeatOff_ET]) / 2;
//					}
//					/*黑盾空调(温度设定点为停止制冷温度和开启制热温度)*/
//					else
//					{
//						//空调制冷设定温度为度配置停止制冷温度值
//						sAirColdOnTemp = (s8)gGHardPara_104[eGHardPara104_AirColdOff_ET];
//						//空调制热设定温度为度配置开启制热温度值
//						sAirHeatOnTemp = (s8)gGHardPara_104[eGHardPara104_AirHeatOn_ET];
//					}

					//清除空调制冷制热除湿控制状态
					sAirColdState = 0;
					sAirHeatState = 0;

					/*同飞空调(湿度设定点为中间点)*/
					if(1 == sAuxDevAirType)
					{
						//空调除湿启动湿度低于当前空调湿度
						sAirDehumOnHum = 80;
						//空调除湿湿度回差为1/2配置值
						sAirDehumOffHum = 20;
					}
					/*黑盾空调(湿度设定点除湿启动点)*/
					else
					{
						//空调除湿启动湿度低于当前空调湿度
						sAirDehumOnHum = 99;
						//空调除湿停止湿度回差为配置回差
						sAirDehumOffHum = 60;
					}

					//发送强制关机命令
					SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForPwr, 0);

					//控制状态变化
					if(1 == sAirState)
					{
						//记录开机关机切换时间
						sAirPwrCtrlT = GetSysSecondTimeAPI();

						/*产生事件*/
						EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirPwr, GetGSampOutCurrAPI(), 0, (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT)), (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH)));
						sAirKeepTime = 2;
						sAirState = 0;
					}
				}
			}
			//系统正常不需关空调保护
			else
			{
				//空调通信正常
				//if(0 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_DiSign), eEU204DI_AirCom))
				{
					//无制冷/制热/除湿(已关机时不考虑开机除湿),确认是否需要关机
					if((0 == sAirColdState) && (0 == sAirHeatState)//无需制冷制热
						/*&& ((0 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_Power))//已关机
						|| ((0 == sAuxDevAirType) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) < sAirDehumOnHum))//无需除湿
						|| ((1 == sAuxDevAirType) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH) < (sAirDehumOnHum + sAirDehumOffHum))))*/)
					{
						//空调已关机状态
						if(0 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_Power))
						{
							//至少等待控制3分钟(开关机间隔5min)后温度高充放电需要开机通风
							if((sAirKeepTime >= IMEU2_AIR_CTRL_T) && (GetSysSecondTimeAPI() >= (sAirPwrCtrlT + IMEU2_AIR_CYCLE_T))
								//在充放电且温度较高或电池与柜内温差大
								&& ((((GetGCellMaxTempAPI() >= ((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp] + 4))
								/*|| ((s16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) >= ((s16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 5))*/
								|| ((GetGCellMaxTempAPI() >= ((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp] + 2))
								&& (GetGCellMaxTempAPI() >= ((s16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 3))))
								&& ((sAvgCurr1min <= -250) || (sAvgCurr1min >= 250)))
								//间隔3小时电池与柜内温差大
								|| ((GetSysSecondTimeAPI() >= (sAirPwrCtrlT + 10800))
								&& (GetGCellMaxTempAPI() >= 26) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= 30)
								&& (GetGCellMaxTempAPI() >= ((s16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 5)))))
							{
								//发送强制开机命令
								SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForPwr, 1);

								//控制状态变化
								if(0 == sAirState)
								{
									//记录开机关机切换时间
									sAirPwrCtrlT = GetSysSecondTimeAPI();

									/*产生事件*/
									EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirPwr, GetGSampOutCurrAPI(), 1, (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT)), (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH)));
									sAirState = 1;

									//重启空调直接回头检测下一轮周期
									//sAirKeepTime = IMEU2_AIR_CYCLE_T;
									//重启空调重新计时
									sAirKeepTime = 0;
								}
							}

							//开关机切换超过15min(或已重启空调),到达检测周期5分钟(仍无需开机)
							if(((GetSysSecondTimeAPI() >= (sAirPwrCtrlT + IMEU2_AIR_STATE_T)) || (1 == sAirState)) && (sAirKeepTime >= IMEU2_AIR_CYCLE_T))
							{
								//直接回头检测下一轮
								sAirKeepTime = 0;
								sCtrlStep = 0;
							}
							//需保持关机状态
							else
							{
								//返回配置参数
								sCtrlStep = 5;
							}
						}
						//空调未关机
						else
						{
							//非充放电且无需通风即可以关机(开关机间隔15min)
							if((sAirKeepTime >= IMEU2_AIR_CTRL_T) && (GetSysSecondTimeAPI() >= (sAirPwrCtrlT + IMEU2_AIR_STATE_T))//达到开关机间隔时间
								&& (sAvgCurr1min >= -250) && (sAvgCurr1min <= 250)//非充放电
								&& (((GetGCellMaxTempAPI() <= ((s16)gGBmuHigLevPara_103[eBmuHigLevPara103_FanCloseTemp] + 2))//电池温度低
								&& (GetGCellMaxTempAPI() <= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 3)))//电池温度接近柜内温度
								|| ((GetGCellMaxTempAPI() <= (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) + 1))//电池温度比柜内温度低
								/*&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) <= GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT))*/)))//前仓温度与柜内温度一致
							{
								//发送强制关机命令
								SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForPwr, 0);

								//控制状态变化
								if(1 == sAirState)
								{
									//记录开机关机切换时间
									sAirPwrCtrlT = GetSysSecondTimeAPI();

									/*产生事件*/
									EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirPwr, GetGSampOutCurrAPI(), 0, (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT)), (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH)));
									sAirState = 0;

									//关闭空调重新计时
									sAirKeepTime = 0;
								}
							}

							//开关机切换超过15min,到达检测周期5分钟(仍无需关机)
							if((GetSysSecondTimeAPI() >= (sAirPwrCtrlT + IMEU2_AIR_STATE_T)) && (sAirKeepTime >= IMEU2_AIR_CYCLE_T))
							{
								//直接回头检测下一轮
								sAirKeepTime = 0;
								sCtrlStep = 0;
							}
							//需保持开机状态
							else
							{
								//返回配置参数
								sCtrlStep = 5;
							}
						}
					}
					//空调需要制冷/制热或除湿无需关机
					else
					{
						//空调已关机状态
						if(0 == BitGet(GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirState), eEU205AirS_Power))
						{
							//发送强制开机命令
							SendCanImeu2AirCtrlMsg221(eEuAirCmd_ForPwr, 1);

							//控制状态变化
							if(0 == sAirState)
							{
								//记录开机关机切换时间
								sAirPwrCtrlT = GetSysSecondTimeAPI();

								/*产生事件*/
								EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_AirPwr, GetGSampOutCurrAPI(), 1, (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT)), (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH)));
								sAirState = 1;
							}
						}

						//已开关机或制冷持续15min,且已达到检测周期5分钟(非制冷保持3分钟)
						if((GetSysSecondTimeAPI() >= (sAirPwrCtrlT + IMEU2_AIR_STATE_T)) && (GetSysSecondTimeAPI() >= (sAirColdCtrlT + IMEU2_AIR_STATE_T))
							&& ((sAirKeepTime >= IMEU2_AIR_CYCLE_T) || ((sAirKeepTime >= IMEU2_AIR_CTRL_T) && (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) < IMEU2_AIR_COLD_MIN_T))))
						{
							//回头检测下一轮
							sAirKeepTime = 0;
							sCtrlStep = 0;
						}
						//5min内检测和配置设置值
						else
						{
							//返回配置参数
							sCtrlStep = 5;
						}
					}
				}
			}
			break;

		default:
			sCtrlStep = 0;
			break;
    }
}

/*=============================================================================================
** @Name      : static u8 TacCheckAirOnTempNorState(u8 state)
** @Input     : state: 0:制冷状态 1:制热状态
** @Output    : TRUE:正常 FALSE:异常
** @Function  : 判断需要控制空调开启制冷制热时温度是否正常
** @The notes : 检查空调温度和电池温度
**===========================================================================================*/
static u8 TacCheckAirOnTempNorState(u8 state)
{
	//制冷
	if(0 == state)
	{
		//空调温度和电池温度正常且在可制冷范围内
		if((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (s8)IMEU2_AIR_COLD_MIN_T)
			&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= 100)
			&& (TRUE == TacGetGAllSlaveReadyHook())
			&& (GetGCellMinTempAPI() >= ((s8)gGHardPara_104[eGHardPara104_AirHeatOff_CT] + 5))
			&& (GetGCellMaxTempAPI() >= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_CT] - 5))
			&& (GetGCellMaxTempAPI() <= 80))
		{
			return(TRUE);
		}
	}
	//制热
	else if(1 == state)
	{
		//空调温度和电池温度正常且在可制热范围内
		if((GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) >= (s8)IMEU2_AIR_LT_WARN_T)
			&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT) <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_ET] - 5))
			&& (TRUE == TacGetGAllSlaveReadyHook())
			&& (GetGCellMaxTempAPI() <= ((s8)gGHardPara_104[eGHardPara104_AirColdOff_CT] - 5))
			&& (GetGCellMinTempAPI() <= ((s8)gGHardPara_104[eGHardPara104_AirHeatOff_CT] + 5)))
		{
			return(TRUE);
		}
	}

	return(FALSE);
}

/*=============================================================================================
** @Name      : static u8 TacCheckSensorTempNorState(u8 id)
** @Input     : id:温湿度传感器序号[0,1]
** @Output    : TRUE:有效 FALSE:无效
** @Function  : 判断温湿度传感器温度是否有效
** @The notes : 与空调温度对比
**===========================================================================================*/
/*static u8 TacCheckSensorTempNorState(u8 id)
{
	//温湿度传感器1
	if(0 == id)
	{
		//温度1在合理范围且与空调温度一致
		if((0 == BitGet(gAuxComErrFlag, eAuxComErr_TH1))
			&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) >= -40)
			&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) <= 100)
			&& (ABS(GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1), GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT)) <= 10))
		{
			return(TRUE);
		}
	}
	//温湿度传感器2
	else if(1 == id)
	{
		//温度2在合理范围且与空调温度一致
		if((0 == BitGet(gAuxComErrFlag, eAuxComErr_TH2))
			&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) >= -40)
			&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2) <= 100)
			&& (ABS(GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT2), GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT)) <= 10))
		{
			return(TRUE);
		}
	}

	return(FALSE);
}*/

/*=============================================================================================
** @Name      : static u8 TacCheckOutETempHighState(void)
** @Input     : void
** @Output    : TRUE:外温较高 FALSE:外温低
** @Function  : 判断柜外/柜壁温度是否较高
** @The notes : 与空调停止制冷温度对比
**===========================================================================================*/
static u8 TacCheckOutETempHighState(void)
{
	u8 i = 0;
	u8 errNum = 0;

	//检测1路柜外温度和4路柜壁温度(找到任一路温度高则外温高)
	for(i = 0; i < 5; i++)
	{
		//温度正常
		if((GetGPerImeu2TempInfoAPI(eIMEU2Info205_TP10001 + i) >= -40)
			&& (GetGPerImeu2TempInfoAPI(eIMEU2Info205_TP10001 + i) <= 126))
		{
			//任一路高于空调停止制冷环境温度则认为外温较高
			if(GetGPerImeu2TempInfoAPI(eIMEU2Info205_TP10001 + i) >= gGHardPara_104[eGHardPara104_AirColdOff_ET])
			{
				return(TRUE);
			}
		}
		//温度异常
		else
		{
			//计算异常温度个数
			errNum++;
		}
	}

	//所有外温都异常默认为外温较高
	if(errNum >= 5)
	{
		return(TRUE);
	}

	return(FALSE);
}
#endif
