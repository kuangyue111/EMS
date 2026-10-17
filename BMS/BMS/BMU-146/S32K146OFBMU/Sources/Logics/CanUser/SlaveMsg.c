/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : SlaveMsg.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 从控CAN报文处理模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "DiagCheck.h"
#include "CellAFESamp.h"
#include "DiagInterface.h"
#include "CANUser.h"
#include "SlaveMsg.h"

//使能自研从控BSU CAN通信信息
#if((1 == BMU_BSU_COM_TYPE) && (0 == BMU_BSU_CAN_TYPE))
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 gSlaveMode[SLAVE_MAX_NUM] = {0};  /*从件工作模式*/

extern u16 gCANDevInfo_1[];       			/*设备信息*/
extern u16 gGBmuGenPara_102[];  			/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[]; 			/*主控高级参数值*/
extern u8 gGHardPara_104[];                 /*主控硬件参数配置信息*/

extern u16 gGBsuGenPara_102[SLAVE_MAX_NUM][11];/*从控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void SendCanSlaveBalanCtrlMsg86(u8 pack, u16 dhgBatt, u16 chgBatt)
** @Input     : pack:从控序号[0,31],0xff:所有电池箱  dhgBatt:放电均衡电池:每bit为1节电池  chgBatt:充电均衡电池:每bit为1节电池
** @Output    : void
** @Function  : 发送请求从控均衡控制命令CAN报文(按单个电池箱[16S]控制)
** @The notes : 50ms周期
**===============================================================================================*/
static void SendCanSlaveBalanCtrlMsg86(u8 pack, u16 dhgBatt, u16 chgBatt);

/*=================================================================================================
** @Name      : static void SendCanSlaveResisTrigMsg86(u8 slave, u8 flag)
** @Input     : slave:从控序号[0,9],0xff:所有从控  flag:触发标志(1触发)
** @Output    : void
** @Function  : 发送请求从控内阻检测命令CAN报文(按单个从控[通信板]控制)
** @The notes : 50ms周期
**===============================================================================================*/
static void SendCanSlaveResisTrigMsg86(u8 slave, u8 flag);

/*=================================================================================================
** @Name      : static void SendCanSlaveModeCtrlMsg85(u8 slave, u8 cmd)
** @Input     : slave:从控序号[0,9],0xff:所有从控  flag:控制命令(1调试模式)
** @Output    : void
** @Function  : 发送请求从控工作模式控制命令CAN报文(按单个从控[通信板]控制)
** @The notes : 50ms周期
**===============================================================================================*/
static void SendCanSlaveModeCtrlMsg85(u8 slave, u8 cmd);

/*=================================================================================================
** @Name      : static void SendCanSlaveParaSetMsg107(u8 slave, u8 index, u16 para)
** @Input     : slave:从控序号[0,9],0xff:所有从控 index:参数子索引号[0,10] para:目标参数值
** @Output    : void
** @Function  : 发送请求从控参数设置命令CAN报文(按单个从控[通信板]控制)
** @The notes : 50ms周期
**===============================================================================================*/
static void SendCanSlaveParaSetMsg107(u8 slave, u8 index, u16 para);

/*=================================================================================================
** @Name      : static void RcvSlaveCellVoltInfo9293(u8 ecu, u8 cell, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] cell:起始电池号[0,399] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机单体电压信息
** @The notes : 一个从机输多个电池箱信息(要求每个从机Pack电压数目必须相等)
**===============================================================================================*/
static void RcvSlaveCellVoltInfo9293(u8 ecu, u8 cell, u8 *pdata, u8 len);

/*=================================================================================================
** @Name      : static void RcvSlaveCellTempInfo9495(u8 ecu, u8 cell, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] cell:起始温度号[0,399] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机单体温度信息(包括均衡板温)
** @The notes : 一个从机传输多个电池箱信息(要求每个从机Pack温度数目必须相等)
**===============================================================================================*/
static void RcvSlaveCellTempInfo9495(u8 ecu, u8 cell, u8 *pdata, u8 len);

/*=================================================================================================
** @Name      : static void RcvSlaveChgBalanStateInfo90(u8 ecu, u8 index, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] index:起始索引号[0,24] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机单体充电均衡状态信息
** @The notes : 一个从机传输多个电池箱信息(要求每个从机Pack电池数目必须相等)
**===============================================================================================*/
static void RcvSlaveChgBalanStateInfo90(u8 ecu, u8 index, u8 *pdata, u8 len);

/*=================================================================================================
** @Name      : static void RcvSlaveDhgBalanStateInfo91(u8 ecu, u8 index, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] index:起始索引号[0,24] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机单体放电均衡状态信息
** @The notes : 一个从机传输多个电池箱信息(要求每个从机Pack电池数目必须相等)
**===============================================================================================*/
static void RcvSlaveDhgBalanStateInfo91(u8 ecu, u8 index, u8 *pdata, u8 len);

/*=================================================================================================
 ** @Name      : static void RcvSlaveCellResisInfo9697(u8 ecu, u8 cell, u8 *pdata, u8 len)
 ** @Input     : slave:从机节点号[0, 9] cell:起始电池号[0,399] pdata:接收的报文数据域 len:数据长度[0,8]
 ** @Output    : void
 ** @Function  : 接收从机单体电压信息
 ** @The notes : 一个从机输多个电池箱信息(要求每个从机Pack电压数目必须相等)
 **===============================================================================================*/
 static void RcvSlaveCellResisInfo9697(u8 ecu, u8 cell, u8 *pdata, u8 len);

 /*=================================================================================================
 ** @Name      : static void RcvSlaveFaultInfo54(u8 ecu, u8 index, u8 *pdata, u8 len)
 ** @Input     : slave:从机节点号[0, 9] index:起始索引号[0,3] pdata:接收的报文数据域 len:数据长度[0,8]
 ** @Output    : void
 ** @Function  : 接收从机故障码信息
 ** @The notes :
 **===============================================================================================*/
 static void RcvSlaveFaultInfo54(u8 ecu, u8 index, u8 *pdata, u8 len);

 /*=================================================================================================
 ** @Name      : static void RcvSlaveStateInfo50(u8 ecu, u8 index, u8 *pdata, u8 len)
 ** @Input     : slave:从机节点号[0, 9] index:起始索引号[0,3] pdata:接收的报文数据域 len:数据长度[0,8]
 ** @Output    : void
 ** @Function  : 接收从机状态信息
 ** @The notes :
 **===============================================================================================*/
 static void RcvSlaveStateInfo50(u8 ecu, u8 index, u8 *pdata, u8 len);

 /*=================================================================================================
 ** @Name      : static void RcvSlaveParaInfo102(u8 ecu, u8 index, u8 *pdata, u8 len)
 ** @Input     : slave:从机节点号[0, 9] index:起始索引号[0,3] pdata:接收的报文数据域 len:数据长度[0,8]
 ** @Output    : void
 ** @Function  : 接收从机参数信息
 ** @The notes :
 **===============================================================================================*/
 static void RcvSlaveParaInfo102(u8 ecu, u8 index, u8 *pdata, u8 len);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void GroupCheckRcvBsuMsgAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 主控判断从控信息采集完成任务
** @The notes : 用户50ms周期接口调用
**===============================================================================================*/
void GroupCheckRcvBsuMsgAPI(void)
{
	u16 cfgValue = 0;
	static u8 sPack = 0;//Pack序号[0,15]
	static u8 sSlave = 0;//从件序号[0,9]
	static u16 sBalanTime = 0;
	static u32 sHisRTime = 0;
	static u32 sIdleTime = 0xfffffff0;

	//上电前5s等待从控信息
	if(BSPGetOSMillSecTimeAPI() < 5000)
	{
		return;
	}

	//系统空闲
	if(eCURR_IDLE == GetGChgDhgStateAPI())
	{
		if(sIdleTime < 0xfffffff0)
		{
			sIdleTime++;
		}
	}
	else
	{
		sIdleTime = 0;
	}

	/*正常模式下采样失效和内部CAN断线故障*/
	if((eMODE_DEBUG != GetGWorkModeAPI())
		&& ((eErr_B == DiagGetErrFilStateAPI(eERR_ID_SAMP_EXP))
		|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_SAMP_VOFF))
		|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_CELL_MISS))
		/*|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_INT_LOFF))*/
		|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_INT_COFF))))
	{
		//异常计时减慢发送周期
		sBalanTime++;
		//先广播10次,然后1轮单个控制广播1次
		if((sBalanTime <= 10) || (sBalanTime == 120))
		{
			/*广播关闭所有均衡*/
			SendCanSlaveBalanCtrlMsg86(0xff, 0, 0);
			sPack = 0;
		}
		//逐个Pack关闭(防止从控不接收广播命令)
		else if((sBalanTime <= 100) || ((sBalanTime > 120) && (0 == ((sBalanTime - 120) % 20))))
		{
			/*逐个向从控发送均衡命令*/
			SendCanSlaveBalanCtrlMsg86(sPack, GetGroupVoltPasBalanReqState(sPack), 0);

			sPack++;
			if(sPack >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
			{
				sPack = 0;
				if(sBalanTime > 120)
				{
					sBalanTime = 119;
				}
			}
		}
	}
	/*非上电初始化且从控和单体电压正常*/
	else if((eWORK_INIT != GetGWorkStateAPI()) && (eWORK_OFF != GetGWorkStateAPI()))
	{
		/*逐个Pack向从控发送均衡命令*/
		SendCanSlaveBalanCtrlMsg86(sPack, GetGroupVoltPasBalanReqState(sPack), 0);

		sPack++;
		if(sPack >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
		{
			sPack = 0;
		}
		sBalanTime = 0;

		//有电池内阻信息才进行触发,检测SOC85%以上的内阻,计时连续空闲30天或检测间隔7天(充放电截止静置10min,非连续静置超过7*2/3天)
		if((1 == GetCellRSampFinFlagAPI()) && (GetGBattSocAPI() >= 850) && ((sIdleTime >= 51840000)
			|| ((BSPGetOSMillSecTimeAPI() >= (sHisRTime + BMU_CELL_R_TRIG_T)) && (sIdleTime >= 12000) && (sIdleTime <= (BMU_CELL_R_TRIG_T / 75)))))
		{
			/*广播触发所有内阻检测*/
			SendCanSlaveResisTrigMsg86(0xff, 1);

			//发送1s(多次)防止丢帧
			if(((sIdleTime >= 51840020)) || (BSPGetOSMillSecTimeAPI() >= (sHisRTime + BMU_CELL_R_TRIG_T + 1000)))
			{
				sHisRTime = BSPGetOSMillSecTimeAPI();
				sIdleTime = 0;
			}
		}
	}

	/*主从通信正常确保主从调试模式一致*/
	if(eErr_A == DiagGetErrFilStateAPI(eERR_ID_INT_COFF))
	{
		/*主控调试模式而有从控非调试模式*/
		if((eMODE_DEBUG == GetGWorkModeAPI()) && (1 != gSlaveMode[sSlave]))
		{
			/*广播控制从控进入调试模式*/
			SendCanSlaveModeCtrlMsg85(0xff, 1);
		}
		/*主控非调试模式而有从控调试模式*/
		else if((eMODE_DEBUG != GetGWorkModeAPI()) && (1 == gSlaveMode[sSlave]))
		{
			/*广播控制从控退出调试模式*/
			SendCanSlaveModeCtrlMsg85(0xff, 0);
		}

		/*检测从控其它参数与主控参数是否一致(仅从控异常或上电前1分钟检测参数)*/
		if((1 == DiagGetCellVoltExpStateHOOK()) || (BSPGetOSMillSecTimeAPI() <= 60000))
		{
			/*根据主控参数计算有几个从控*/
			cfgValue = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum];

			/*从控菊花链类型参数与主控参数不一致*/
			if(gGHardPara_104[eGHardPara104_BmuSWVerHH] != gGBsuGenPara_102[sSlave][eGBsuPara102_DaisyType])
			{
				/*广播设置所有从控参数[从控类型]*/
				SendCanSlaveParaSetMsg107(0xff, eGBsuPara102_DaisyType, gGHardPara_104[eGHardPara104_BmuSWVerHH]);
			}
			/*从控数目参数与主控参数不一致*/
			else if((cfgValue != gGBsuGenPara_102[sSlave][eGBsuPara102_SlaveNum]))
			{
				/*广播设置所有从控参数[从控数目]*/
				SendCanSlaveParaSetMsg107(0xff, eGBsuPara102_SlaveNum, cfgValue);
			}
			/*从控菊花链个数参数与主控参数不一致*/
			else if(gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] != gGBsuGenPara_102[sSlave][eGBsuPara102_DaisyNum])
			{
				/*广播设置所有从控参数[菊花链数目]*/
				SendCanSlaveParaSetMsg107(0xff, eGBsuPara102_DaisyNum, gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]);
			}
			/*从控单个PACK电压数目参数与主控参数不一致*/
			else if(gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] != gGBsuGenPara_102[sSlave][eGBsuPara102_SampVnum])
			{
				/*广播设置所有从控参数[电压数目]*/
				SendCanSlaveParaSetMsg107(0xff, eGBsuPara102_SampVnum, gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum]);
			}
			/*从控单个PACK温度数目参数与主控参数不一致*/
			else if(gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum] != gGBsuGenPara_102[sSlave][eGBsuPara102_SampTnum])
			{
				/*广播设置所有从控参数[温度数目]*/
				SendCanSlaveParaSetMsg107(0xff, eGBsuPara102_SampTnum, gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum]);
			}
			/*从控其它参数(电压和温度总数)[考虑每个从控数目是否一样]*/
			else
			{
				/*从控电压总数参数与主控参数不一致*/
				if(((gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] / cfgValue) != gGBsuGenPara_102[sSlave][eGBsuPara102_TotalVnum])
					&& (gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] == (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum])))
				{
					/*广播设置所有从控参数[电压数目]*/
					SendCanSlaveParaSetMsg107(0xff, eGBsuPara102_TotalVnum, (gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] / cfgValue));
				}
				/*从控温度总数参数与主控参数不一致*/
				if(((gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] / cfgValue) != gGBsuGenPara_102[sSlave][eGBsuPara102_TotalTnum])
					&& (gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] == (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum] * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum])))
				{
					/*广播设置所有从控参数[温度数目]*/
					SendCanSlaveParaSetMsg107(0xff, eGBsuPara102_TotalTnum, (gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] / cfgValue));
				}
			}
		}

		sSlave++;
		if(sSlave >= (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]))
		{
			sSlave = 0;
		}
	}
	/*主从通信中断*/
	else
	{
		sSlave = 0;
	}

	//上报主从通信中断或上电10s(不检通信断线或从控上传信息不全单体信息缺失)
	if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_INT_COFF)) || ((BSPGetOSMillSecTimeAPI() >= 10000)
		&& (eErr_A == DiagGetErrFilStateAPI(eERR_ID_INT_COFF)) && ((0 == DiagGetFilErrSortAPI(eERR_ID_INT_COFF))
		|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_CELL_MISS)) || (0 == DiagGetFilErrSortAPI(eERR_ID_CELL_MISS)))))
	{
		//记录单体电压和温度采样完成一遍
		WriteCellVSampFinFlagHook(1);
		WriteCellTSampFinFlagHook(1);
	}
}

/*=================================================================================================
** @Name      : void CanRcvSlaveMsgHandleAPI(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 自研从控协议CAN报文接收处理
** @The notes : CAN接收回调函数调用
**===============================================================================================*/
void CanRcvSlaveMsgHandleAPI(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
	u8 rcvDev = 0;
	u8 aimDev = 0;
	u8 rcvFun = 0;
	u8 rcvIndex = 0;
	u16 subIndex = 0;

	rcvDev = ID & 0xff;
	aimDev = (ID >> 8) & 0xff;
	rcvIndex = (ID >> 16) & 0xff;
	rcvFun = (ID >> 25) & 0xffff;

	/*只接收从控报文*/
	if((eCAN1 != canNum) || (0 == DLC) || (rcvDev >= IMEU_BAS_DEVADDR)
		|| ((0xff != aimDev) && (gCANDevInfo_1[eCANDev1_Addr] != aimDev)) || ((rcvFun != 3)
		&& (rcvFun != 6) && (rcvFun != 8) && (rcvFun != 9) && (rcvFun != 10) && (rcvFun != 11) && (rcvFun != 12)))
	{
		return;
	}

	//从控号按主控序号偏移(同一堆内从控不同方便统一升级)
	if(rcvDev >= GetGroupOffsetDevAddrAPI() * (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]))
	{
		//从控序号去掉偏移量
		rcvDev -= GetGroupOffsetDevAddrAPI() * (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]);
	}
	//从控编址不在本主控偏移量内
	else
	{
		return;
	}

	/*记录能接收到报文通信正常*/
	DiagRcvDevMsgAddAPI(rcvDev);

	/*BSU单体信息帧报文*/
	if(11 == rcvFun)
	{
		/*BSU单体电压信息报文*/
		if((rcvIndex >= 50) && (rcvIndex < 150))
		{
			subIndex = (rcvIndex - 50) * 4;

			RcvSlaveCellVoltInfo9293(rcvDev, subIndex, pdata, DLC);
		}
		/*BSU单体温度信息报文*/
		//else if((rcvIndex >= 150) && (rcvIndex < 200))
		else if((rcvIndex >= 150) && (rcvIndex < 250))
		{
			//subIndex = (rcvIndex - 150) * 8;
			subIndex = (rcvIndex - 150) * 4;

			RcvSlaveCellTempInfo9495(rcvDev, subIndex, pdata, DLC);
		}
		/*BSU充电均衡状态信息报文*/
		else if(rcvIndex < 10)
		{
			subIndex = rcvIndex * 4;

			RcvSlaveChgBalanStateInfo90(rcvDev, subIndex, pdata, DLC);
		}
		/*BSU放电均衡状态信息报文*/
		else if(rcvIndex < 20)
		{
			subIndex = (rcvIndex - 10) * 4;

			RcvSlaveDhgBalanStateInfo91(rcvDev, subIndex, pdata, DLC);
		}
	}
	/*BSU主动更新或请求查询(长帧)信息帧报文*/
	else if(((6 == rcvFun) || (8 == rcvFun)) && (DLC > 2))
	{
		/*240节后*/
		if((93 == rcvIndex) || (95 == rcvIndex) || (97 == rcvIndex))
		{
			subIndex = 240 + pdata[1];
		}
		/*前240节*/
		else
		{
			subIndex = pdata[1];
		}

		/*BSU单体电压信息报文*/
		if((92 == rcvIndex) || (93 == rcvIndex))
		{
			RcvSlaveCellVoltInfo9293(rcvDev, subIndex, &pdata[2], ((pdata[0] & 0x0f) * 2));
		}
		/*BSU单体温度信息报文*/
		else if((94 == rcvIndex) || (95 == rcvIndex))
		{
			RcvSlaveCellTempInfo9495(rcvDev, subIndex, &pdata[2], ((pdata[0] & 0x0f) * 2));
		}
		/*BSU充电均衡状态信息报文*/
		else if(90 == rcvIndex)
		{
			RcvSlaveChgBalanStateInfo90(rcvDev, subIndex, &pdata[2], ((pdata[0] & 0x0f) * 2));
		}
		/*BSU放电均衡状态信息报文*/
		else if(91 == rcvIndex)
		{
			RcvSlaveDhgBalanStateInfo91(rcvDev, subIndex, &pdata[2], ((pdata[0] & 0x0f) * 2));
		}
		/*BSU单体内阻信息报文*/
		else if((96 == rcvIndex) || (97 == rcvIndex))
		{
			RcvSlaveCellResisInfo9697(rcvDev, subIndex, &pdata[2], ((pdata[0] & 0x0f) * 2));
		}
		/*BSU单故障信息报文*/
		else if(54 == rcvIndex)
		{
			RcvSlaveFaultInfo54(rcvDev, subIndex, &pdata[2], ((pdata[0] & 0x0f) * 2));
		}
		/*BSU状态信息报文*/
		else if(50 == rcvIndex)
		{
			RcvSlaveStateInfo50(rcvDev, subIndex, &pdata[2], ((pdata[0] & 0x0f) * 2));
		}
		/*BSU常规参数信息报文*/
		else if(102 == rcvIndex)
		{
			RcvSlaveParaInfo102(rcvDev, subIndex, &pdata[2], ((pdata[0] & 0x0f) * 2));
		}
	}
	//单帧信息
	else if(10 == rcvFun)
	{
		/*BSU单故障信息报文*/
		if(54 == rcvIndex)
		{
			RcvSlaveFaultInfo54(rcvDev, 0, pdata, DLC);
		}
		/*BSU状态信息报文*/
		else if(50 == rcvIndex)
		{
			RcvSlaveStateInfo50(rcvDev, 0, pdata, DLC);
		}
	}
	//参数信息
	else if(12 == rcvFun)
	{
		/*BSU常规参数信息报文*/
		if((rcvIndex >= 50) && (rcvIndex < 75))
		{
			subIndex = (rcvIndex - 50) * 4;

			RcvSlaveParaInfo102(rcvDev, subIndex, pdata, DLC);
		}
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void SendCanSlaveBalanCtrlMsg86(u8 pack, u16 dhgBatt, u16 chgBatt)
** @Input     : pack:电池箱序号[0,31],0xff:所有电池箱  dhgBatt:放电均衡电池:每bit为1节电池  chgBatt:充电均衡电池:每bit为1节电池
** @Output    : void
** @Function  : 发送请求从控均衡控制命令CAN报文(按单个电池箱[16S]控制)
** @The notes : 50ms周期
**===============================================================================================*/
static void SendCanSlaveBalanCtrlMsg86(u8 pack, u16 dhgBatt, u16 chgBatt)
{
    t_CANMSG msg;
    u8 slave = 0;

    //电池箱序号无效
    if((pack >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) && (0xff != pack))
    {
    	return;
    }

    if(0xff == pack)
    {
    	slave = 0xff;
    	pack = 0;
    }
    else
    {
    	//从控号按主控序号偏移(同一堆内从控不同方便统一升级)
    	slave = GetGroupOffsetDevAddrAPI() * (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]);
    	slave += pack / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum];
    	//每组从控无偏移量(都从0开始)
    	//slave = pack / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum];

    	pack = pack % gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + 1;
    }

    /*请求帧ID*/
    msg.ID = 0x05560000 | (((u16)slave) << 8) | (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev];

    /*报文长度*/
    msg.Len = 8;

    /*请求帧数据域(低位在前)*/
    msg.Data[0] = 0xc3;
    msg.Data[1] = 0x00;
    msg.Data[2] = pack;
	msg.Data[3] = 0;
	msg.Data[4] = dhgBatt & 0xff;
	msg.Data[5] = (dhgBatt >> 8) & 0xff;
	msg.Data[6] = chgBatt & 0xff;
	msg.Data[7] = (chgBatt >> 8) & 0xff;

    /*向CAN1通道发送*/
    (void)BSPCANSendQin(eCAN1, &msg);
}

/*=================================================================================================
** @Name      : static void SendCanSlaveResisTrigMsg86(u8 slave, u8 flag)
** @Input     : slave:从控序号[0,9],0xff:所有从控  flag:触发标志(1触发)
** @Output    : void
** @Function  : 发送请求从控内阻检测命令CAN报文(按单个从控[通信板]控制)
** @The notes : 50ms周期
**===============================================================================================*/
static void SendCanSlaveResisTrigMsg86(u8 slave, u8 flag)
{
    t_CANMSG msg;

    //从控序号无效
    if((slave >= (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum])) && (0xff != slave))
    {
        return;
    }

    //非广播报文(单个控制)
    if(0xff != slave)
    {
        //从控号按主控序号偏移(同一堆内从控不同方便统一升级)
        slave += GetGroupOffsetDevAddrAPI() * (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]);
    }

    /*请求帧ID*/
    msg.ID = 0x05560000 | (((u16)slave) << 8) | (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev];

    /*报文长度*/
    msg.Len = 8;

    /*请求帧数据域(低位在前)*/
    msg.Data[0] = 0xc1;
    msg.Data[1] = 0x03;
    msg.Data[2] = flag;
	msg.Data[3] = 0;
	msg.Data[4] = 0;
	msg.Data[5] = 0;
	msg.Data[6] = 0;
	msg.Data[7] = 0;

    /*向CAN1通道发送*/
    (void)BSPCANSendQin(eCAN1, &msg);
}

/*=================================================================================================
** @Name      : static void SendCanSlaveModeCtrlMsg85(u8 slave, u8 cmd)
** @Input     : slave:从控序号[0,9],0xff:所有从控  flag:控制命令(1调试模式)
** @Output    : void
** @Function  : 发送请求从控工作模式控制命令CAN报文(按单个从控[通信板]控制)
** @The notes : 50ms周期
**===============================================================================================*/
static void SendCanSlaveModeCtrlMsg85(u8 slave, u8 cmd)
{
    t_CANMSG msg;

    //从控序号无效
    if((slave >= (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum])) && (0xff != slave))
    {
    	return;
    }

    //非广播报文(单个控制)
    if(0xff != slave)
    {
        //从控号按主控序号偏移(同一堆内从控不同方便统一升级)
        slave += GetGroupOffsetDevAddrAPI() * (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]);
    }

    /*请求帧ID*/
    msg.ID = 0x05550000 | (((u16)slave) << 8) | (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev];

    /*报文长度*/
    msg.Len = 8;

    /*请求帧数据域(低位在前)*/
    msg.Data[0] = 0xc1;
    msg.Data[1] = 0x00;
    msg.Data[2] = cmd;
	msg.Data[3] = 0;
	msg.Data[4] = 0;
	msg.Data[5] = 0;
	msg.Data[6] = 0;
	msg.Data[7] = 0;

    /*向CAN1通道发送*/
    (void)BSPCANSendQin(eCAN1, &msg);
}

/*=================================================================================================
** @Name      : static void SendCanSlaveParaSetMsg107(u8 slave, u8 index, u16 para)
** @Input     : slave:从控序号[0,9],0xff:所有从控 index:参数子索引号[0,10] para:目标参数值
** @Output    : void
** @Function  : 发送请求从控参数设置命令CAN报文(按单个从控[通信板]控制)
** @The notes : 50ms周期
**===============================================================================================*/
static void SendCanSlaveParaSetMsg107(u8 slave, u8 index, u16 para)
{
    t_CANMSG msg;

    //从控序号无效
    if((slave >= (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum])) && (0xff != slave))
    {
    	return;
    }

    //非广播报文(单个控制)
    if(0xff != slave)
    {
        //从控号按主控序号偏移(同一堆内从控不同方便统一升级)
        slave += GetGroupOffsetDevAddrAPI() * (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]);
    }

    /*请求帧ID*/
    msg.ID = 0x056B0000 | (((u16)slave) << 8) | (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev];

    /*报文长度*/
    msg.Len = 8;

    /*请求帧数据域(低位在前)*/
    msg.Data[0] = 0xc1;
    msg.Data[1] = index;
    msg.Data[2] = para & 0xff;
	msg.Data[3] = (para >> 8) & 0xff;
	msg.Data[4] = 0;
	msg.Data[5] = 0;
	msg.Data[6] = 0;
	msg.Data[7] = 0;

    /*向CAN1通道发送*/
    (void)BSPCANSendQin(eCAN1, &msg);
}

/*=================================================================================================
** @Name      : static void RcvSlaveCellVoltInfo9293(u8 ecu, u8 cell, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] cell:起始电池号[0,399] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机单体电压信息
** @The notes : 一个从机输多个电池箱信息(要求每个从机Pack电压数目必须相等)
**===============================================================================================*/
static void RcvSlaveCellVoltInfo9293(u8 ecu, u8 cell, u8 *pdata, u8 len)
{
	u8 i = 0;
	u8 packId = 0;
	u8 cellId = 0;
	u16 cellVolt = 0;
	static u8 sRcvCellNum = 0;
	static u32 sRcvCellBit = 0;

    /*参数错误*/
	if((ecu >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) || (ecu > (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]))
		|| (len < 2) || (cell >= gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
		|| (cell >= (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum])))
	{
		return;
	}

	//u16每帧1~4节(根据帧长度判断)
	for(i = 0; i < (len / 2); i++)
	{
		if((cell + i) < gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
		{
			/*转化为电池箱号和箱内电池号(这里要求每个从机和Pack温度数目必须一样,否则计算电压序号会出错)*/
			packId = ecu * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + ((cell + i) / gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum]);
			cellId = (cell + i) % gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum];

			/*Pack号和Pack内电池号正确正确*/
			if((packId < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
				&& (cellId < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum]))
			{
				//获取电压
				cellVolt = ((u16)pdata[1 + (i * 2)] << 8) | pdata[i * 2];

				/*采样电压值判断断线*/
				DiagCheckCellVOffLFlagAPI(packId, cellId, cellVolt);

				/*输出采样电压值*/
				WriteSampCellVoltHook(packId, cellId, cellVolt);

				//首次接收到该从控信息
				if(0 == BitGet(sRcvCellBit, packId))
				{
					//计数接收到从控数目
					sRcvCellNum++;
				}
				//记录该从控已接收
				BitSet(sRcvCellBit, packId);
				//判断接收完成
				if((sRcvCellNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
					&& (2 == DiagGetRcvAllCellFlagAPI()))
				{
					//记录单体电压采样完成一遍
					WriteCellVSampFinFlagHook(1);
				}
			}
			/*Pack号或Pack内电池号越界*/
			else
			{
				break;
			}
		}
		/*组内电池号越界*/
		else
		{
			break;
		}
	}
}

/*=================================================================================================
** @Name      : static void RcvSlaveCellTempInfo9495(u8 ecu, u8 cell, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] cell:起始温度号[0,399] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机单体温度信息(包括均衡板温)
** @The notes : 一个从机传输多个电池箱信息(要求每个从机Pack温度数目必须相等)
**===============================================================================================*/
static void RcvSlaveCellTempInfo9495(u8 ecu, u8 cell, u8 *pdata, u8 len)
{
	u8 i = 0;
	u8 packId = 0;
	u8 cellId = 0;
	s16 cellTemp = 0;
	u8 slaveCelTNum = 0;//一个从机电池温度个数
	u8 slaveAllTNum = 0;//一个从机所有温度个数
	static u8 sRcvCellNum = 0;
	static u32 sRcvCellBit = 0;

    /*计算每个从机的温度数目(这里要求每个从机和Pack温度数目必须一样,否则计算温度序号会出错)*/
	slaveCelTNum = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum] * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum];
	slaveAllTNum = (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum]) * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum];

    /*参数错误*/
	if((ecu >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) || (ecu > (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]))
		|| (0 == len) || (cell >= (gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_BTSumNum]))
		|| (cell >= slaveAllTNum))
	{
		return;
	}

//	//u8每帧1~8节(根据帧长度判断)
//	for(i = 0; i < len; i++)
//	{
//		/*单体电池温度*/
//		if((cell + i) < slaveCelTNum)
//		{
//			/*转化为电池箱号和箱内电池号*/
//			packId = ecu * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + ((cell + i) / gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum]);
//			cellId = (cell + i) % gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum];
//
//			/*单体温度序号正确*/
//			if((packId < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
//				&& (cellId < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum]))
//			{
//				//获取温度
//				cellTemp = (s8)pdata[i];
//
//				/*采样温度值判断断线*/
//				DiagCheckCellTOffLFlagAPI(packId, cellId, cellTemp);
//
//				/*输出采样温度值*/
//				WriteSampCellTempHook(packId, cellId, cellTemp);
//
//				//首次接收到该从控信息
//				if(0 == BitGet(sRcvCellBit, packId))
//				{
//					//计数接收到从控数目
//					sRcvCellNum++;
//				}
//				//记录该从控已接收
//				BitSet(sRcvCellBit, packId);
//				//判断接收完成
//				if((sRcvCellNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
//					&& (2 == DiagGetRcvAllCellFlagAPI()))
//				{
//					//记录单体温度采样完成一遍
//					WriteCellTSampFinFlagHook(1);
//				}
//			}
//		}
//		/*均衡板温*/
//		else
//		{
//			/*转化为电池箱号和箱内电池号*/
//			packId = ecu * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + ((cell + i - slaveCelTNum) / gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum]);
//			cellId = (cell + i - slaveCelTNum) % gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum];
//
//			/*均衡温度序号正确*/
//			if((packId < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
//				&& (cellId < gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum]))
//			{
//				//获取温度
//				cellTemp = (s8)pdata[i];
//
//				/*均衡温度采样线断线检测*/
//				DiagCheckBalanTOffLFlagAPI(packId, cellId, cellTemp);
//
//				/*输出均衡电阻温度,按温度顺序排列[0,1]*/
//				WriteSampBalanTempHook(packId, cellId, cellTemp);
//			}
//		}
//	}

	//u16每帧1~4节(根据帧长度判断)
	for(i = 0; i < (len / 2); i++)
	{
		/*单体电池温度*/
		if((cell + i) < slaveCelTNum)
		{
			/*转化为电池箱号和箱内电池号*/
			packId = ecu * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + ((cell + i) / gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum]);
			cellId = (cell + i) % gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum];

			/*单体温度序号正确*/
			if((packId < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
				&& (cellId < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum]))
			{
				//获取温度(1位小数0.1℃)
				cellTemp = ((s16)pdata[1 + (i * 2)] << 8) | pdata[i * 2];

				//四舍五入成整数1℃
				if(cellTemp < 0)
				{
					cellTemp = (cellTemp - 5) / 10;
				}
				else
				{
					cellTemp = (cellTemp + 5) / 10;
				}

				/*采样温度值判断断线*/
				DiagCheckCellTOffLFlagAPI(packId, cellId, cellTemp);

				/*输出采样温度值*/
				WriteSampCellTempHook(packId, cellId, cellTemp);

				//首次接收到该从控信息
				if(0 == BitGet(sRcvCellBit, packId))
				{
					//计数接收到从控数目
					sRcvCellNum++;
				}
				//记录该从控已接收
				BitSet(sRcvCellBit, packId);
				//判断接收完成
				if((sRcvCellNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
					&& (2 == DiagGetRcvAllCellFlagAPI()))
				{
					//记录单体温度采样完成一遍
					WriteCellTSampFinFlagHook(1);
				}
			}
		}
		/*均衡板温*/
		else
		{
			/*转化为电池箱号和箱内电池号*/
			packId = ecu * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + ((cell + i - slaveCelTNum) / gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum]);
			cellId = (cell + i - slaveCelTNum) % gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum];

			/*均衡温度序号正确*/
			if((packId < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
				&& (cellId < gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum]))
			{
				//获取温度(1位小数0.1℃)
				cellTemp = ((s16)pdata[1 + (i * 2)] << 8) | pdata[i * 2];

				//四舍五入成整数1℃
				if(cellTemp < 0)
				{
					cellTemp = (cellTemp - 5) / 10;
				}
				else
				{
					cellTemp = (cellTemp + 5) / 10;
				}

				/*均衡温度采样线断线检测*/
				DiagCheckBalanTOffLFlagAPI(packId, cellId, cellTemp);

				/*输出均衡电阻温度,按温度顺序排列[0,1]*/
				WriteSampBalanTempHook(packId, cellId, cellTemp);
			}
		}
	}
}

/*=================================================================================================
** @Name      : static void RcvSlaveChgBalanStateInfo90(u8 ecu, u8 index, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] index:起始索引号[0,24] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机单体充电均衡状态信息
** @The notes : 一个从机传输多个电池箱信息(要求每个从机Pack电池数目必须相等)
**===============================================================================================*/
static void RcvSlaveChgBalanStateInfo90(u8 ecu, u8 index, u8 *pdata, u8 len)
{
	u8 i = 0;
	u8 packId = 0;
	u16 cellId = 0;
	u16 state = 0;

    /*参数错误*/
	if((ecu >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) || (ecu > (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]))
		|| (len < 2) || (index >= GINDEX90_LEN))
	{
		return;
	}

    /*转化为组内实际起始电池号*/
	cellId = (u16)ecu * gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + index * 16;

	//u16每帧1~4节(根据帧长度判断)
	for(i = 0; i < (len / 2); i++)
	{
		/*组内电池号正确*/
		if(((cellId + (i * 16)) < gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
			&& (((index + i) * 16) < (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum])))
		{
			//获取均衡状态
			state = ((u16)pdata[1 + (i * 2)] << 8) | pdata[i * 2];

			//输入放电均衡状态
			InputSlaveChgBalanState((packId + (i * 16)), state);
		}
		/*电池号越界*/
		else
		{
			break;
		}
	}
}

/*=================================================================================================
** @Name      : static void RcvSlaveDhgBalanStateInfo91(u8 ecu, u8 index, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] index:起始索引号[0,24] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机单体放电均衡状态信息
** @The notes : 一个从机传输多个电池箱信息(要求每个从机Pack电池数目必须相等)
**===============================================================================================*/
static void RcvSlaveDhgBalanStateInfo91(u8 ecu, u8 index, u8 *pdata, u8 len)
{
	u8 i = 0;
	u8 packId = 0;
	u16 cellId = 0;
	u16 state = 0;

    /*参数错误*/
	if((ecu >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) || (ecu > (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]))
		|| (len < 2) || (index >= GINDEX91_LEN))
	{
		return;
	}

    /*转化为组内实际起始电池号*/
	cellId = (u16)ecu * gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + index * 16;

	//u16每帧1~4节(根据帧长度判断)
	for(i = 0; i < (len / 2); i++)
	{
		/*组内电池号正确*/
		if(((cellId + i) < gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
			&& (((index + i) * 16) < (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum])))
		{
			//获取均衡状态
			state = ((u16)pdata[1 + (i * 2)] << 8) | pdata[i * 2];

			//输入放电均衡状态
			InputSlaveDhgBalanState((packId + (i * 16)), state);
		}
		/*电池号越界*/
		else
		{
			break;
		}
	}
}

/*=================================================================================================
** @Name      : static void RcvSlaveCellResisInfo9697(u8 ecu, u8 cell, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] cell:起始电池号[0,399] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机单体电压信息
** @The notes : 一个从机输多个电池箱信息(要求每个从机Pack电压数目必须相等)
**===============================================================================================*/
static void RcvSlaveCellResisInfo9697(u8 ecu, u8 cell, u8 *pdata, u8 len)
{
	u8 i = 0;
	u8 packId = 0;
	u8 cellId = 0;
	u16 cellResis = 0;
	static u8 sRcvCellNum = 0;
	static u32 sRcvCellBit = 0;

    /*参数错误*/
	if((ecu >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) || (ecu > (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]))
		|| (len < 2) || (cell >= gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
		|| (cell >= (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum])))
	{
		return;
	}

	//u16每帧1~4节(根据帧长度判断)
	for(i = 0; i < (len / 2); i++)
	{
		if((cell + i) < gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
		{
			/*转化为电池箱号和箱内电池号(这里要求每个从机和Pack电池数目必须一样,否则计算电池序号会出错)*/
			packId = ecu * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + ((cell + i) / gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum]);
			cellId = (cell + i) % gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum];

			/*Pack号和Pack内电池号正确*/
			if((packId < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
				&& (cellId < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum]))
			{
				//获取阻值
				cellResis = ((u16)pdata[1 + (i * 2)] << 8) | pdata[i * 2];

				/*输出采样内阻值*/
				InputBsuCellResis(packId, cellId, cellResis);

				//首次接收到该从控信息
				if(0 == BitGet(sRcvCellBit, packId))
				{
					//计数接收到从控数目
					sRcvCellNum++;
				}
				//记录该从控已接收
				BitSet(sRcvCellBit, packId);
				//判断接收完成
				if((sRcvCellNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
					&& (2 == DiagGetRcvAllCellFlagAPI()))
				{
					//记录单体内阻采样完成一遍
					WriteCellRSampFinFlagHook(1);
				}
			}
			/*Pack号或Pack内电池号越界*/
			else
			{
				break;
			}
		}
		/*组内电池号越界*/
		else
		{
			break;
		}
	}
}

/*=================================================================================================
** @Name      : static void RcvSlaveFaultInfo54(u8 ecu, u8 index, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] index:起始索引号[0,3] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机故障码信息
** @The notes :
**===============================================================================================*/
static void RcvSlaveFaultInfo54(u8 ecu, u8 index, u8 *pdata, u8 len)
{
	u8 i = 0;
	u8 packId = 0;
	u16 fault = 0;

    /*参数错误*/
	if((ecu >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) || (ecu > (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]))
		|| (len < 2) || ((index + (len / 2)) > 4))
	{
		return;
	}

	//计算对应电池箱序号
	packId = ecu * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum];

	//u16每帧1~4个索引(根据帧长度判断)
	for(i = 0; i < (len / 2); i++)
	{
		//获取故障码
		fault = ((u16)pdata[1 + (i * 2)] << 8) | pdata[i * 2];

		//索引1含采样失效
		if(1 == (index + i))
		{

			//从控上报采样失效
			if((fault & 0x008) > 0)
			{
				/*记录该从控采样异常*/
				DiagRecordAFESampErrFlagAPI(packId, 1);
				DiagRecordAFESampErrFlagAPI(packId, 3);
			}
			else
			{
				/*清除该从控采样异常*/
				DiagRecordAFESampErrFlagAPI(packId, 0);
				DiagRecordAFESampErrFlagAPI(packId, 2);
			}

			//从控上报电压采样断线
			if((fault & 0x010) > 0)
			{
				/*记录该从控电压采样断线*/
				DiagRecordAFEVOffLFlagAPI(packId, 1);
			}
			else
			{
				/*清除该从控电压采样断线*/
				DiagRecordAFEVOffLFlagAPI(packId, 0);
			}
		}
	}
}

/*=================================================================================================
** @Name      : static void RcvSlaveStateInfo50(u8 ecu, u8 index, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] index:起始索引号[0,3] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机状态信息
** @The notes :
**===============================================================================================*/
static void RcvSlaveStateInfo50(u8 ecu, u8 index, u8 *pdata, u8 len)
{
	u8 i = 0;
	u16 info = 0;

    /*参数错误*/
	if((ecu >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) || (ecu > (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]))
		|| (len < 2) || ((index + (len / 2)) > 1))
	{
		return;
	}

	//u16每帧1个索引(根据帧长度判断)
	//for(i = 0; i < (len / 2); i++)
	for(i = 0; i < 1; i++)
	{
		//获取状态信息
		info = ((u16)pdata[1 + (i * 2)] << 8) | pdata[i * 2];

		//索引0为模式
		if(0 == (index + i))
		{
			//记录从控上报工作模式
			gSlaveMode[ecu] = info;
		}
	}
}

/*=================================================================================================
** @Name      : static void RcvSlaveParaInfo102(u8 ecu, u8 index, u8 *pdata, u8 len)
** @Input     : slave:从机节点号[0, 9] index:起始索引号[0,3] pdata:接收的报文数据域 len:数据长度[0,8]
** @Output    : void
** @Function  : 接收从机参数信息
** @The notes :
**===============================================================================================*/
static void RcvSlaveParaInfo102(u8 ecu, u8 index, u8 *pdata, u8 len)
{
	u8 i = 0;
	u16 para = 0;

    /*参数错误*/
	if((ecu >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) || (ecu > (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]))
		|| (len < 2) || ((index + (len / 2)) > (eGBsuPara102_Fin + 3)))
	{
		return;
	}

	//u16每帧1~4个索引(根据帧长度判断)
	for(i = 0; i < (len / 2); i++)
	{
		//获取参数值
		para = ((u16)pdata[1 + (i * 2)] << 8) | pdata[i * 2];

//		//索引3为从控类型
//		if(3 == (index + i))
//		{
//			//记录从控上报参数值
//			gSlaveType[ecu] = para;
//		}

		//索引正确
		if((index + i) < eGBsuPara102_Fin)
		{
			//记录从控上报参数值
			gGBsuGenPara_102[ecu][index + i] = para;
		}
	}
}
#endif
