/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : PublicInfo.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     :
** @Instructions :
**===========================================================================================*/
#ifndef FUNTION_SYSTEMINFO_PUBLICINFO_H_
#define FUNTION_SYSTEMINFO_PUBLICINFO_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
/*项目编号(识别电池PACK[电压采样解析/均衡/温感表]与默认参数)[常用配置:0/2/4(标准PACK16S-SPI通信0); 0/1/4(标准PACK16S-CAN通信1); 2/3/6(CATL-PACK14S-SPI通信0)]*/
#define PRJ_PACK_NTC_INFO	(0)				/*项目PACK温度NTC类型信息:0:标准PACK10K(项目:测试工装/标准PACK280AH) 1:亿纬PACK10K(项目:领亚亿纬PACK210AH) 2:CATL-PACK10K(项目:CATL-PACK240AH领亚储能/CATL液冷PACK270AH/CATL-PACK280AH新百城/星云PACK280AH康亿创) 3:CATL-PACK100K(项目:CATL-PACK240AH旧百城改造)*/
#define PRJ_PACK_CELL_INFO	(2)				/*项目PACK单体电压解析与均衡通道选择:0:18S[18+0S](项目:ADI-AFE菊花链测试工装) 1:16S[16+空2+0S](项目:领亚亿纬PACK210AH/BYD-AFE16S/CAN通信BSU) 2:16S[8+空1+8S](项目:ADI-AFE标准PACK280AH) 3:14S[7+空1+7S](项目:CATL-PACK240AH领亚储能/CATL-PACK240AH旧百城改造/CATL-PACK280AH新百城/星云PACK280AH康亿创) 4:52S[16+18+18S](项目:CATL液冷270AH)*/
#define PRJ_PARA_NUM_INFO	(4)				/*项目参数配置信息:0:测试工装-18S*1PACK,1:亿纬PACK210AH-16S*13PACK项目,2:CATL-PACK240AH-14S*15PACK项目 3:标准PACK280AH-14S*15PACK项目 4:标准PACK280AH-16S*15PACK项目 5:CATL液冷270AH-52S*4PACK项目 6:CATL-PACK280AH-14S*15PACK项目*/

#define BMU_DC_PWR_TYPE		(2)				/*BMU直流供电方式(0:自供电按钮 1:自供电手动开关  2:市电)*/

#define BMU_BSU_COM_TYPE	(0)				/*BMU主从通信方式(0:菊花链 1:CAN)*/
#define BMU_BSU_CAN_TYPE	(0)				/*BMU主从CAN通信协议(0:自研从控 1:高特从控)*/

/*设备版本信息*/
#define BMU_DEV_ID			(19012)			/*BMU设备识别码(个位设备类型:0:从控采集板 1:从控通信板 2:主控 3:堆控 4:主控辅助芯片 5:高压模块 6:配电板)*/
#define BMU_HW_VER			(0x00a)			/*BMU硬件版本号[低8位有效]*/

#if(0 == BMU_BSU_COM_TYPE)                  /*使能自研从控BSU菊花链通信*/
#define BMU_SW_VERL			(0x0306)		/*BMU软件版本号低位(归档版本[0~99菊花链].变更号)*/
#elif((1 == BMU_BSU_COM_TYPE) && (0 == BMU_BSU_CAN_TYPE))/*使能自研从控BSU CAN通信*/
#define BMU_SW_VERL			(0x6406)		/*BMU软件版本号低位(归档版本[100~149自研CAN].变更号)*/
#else                                       /*使能高特BMM CAN通信*/
#define BMU_SW_VERL			(0x9606)		/*BMU软件版本号低位(归档版本[150~199高特CAN].变更号)*/
#endif

//#define BMU_SW_VERH		(0x102)			/*BMU软件版本号高位*/
#define BMU_SW_VERH			(((((u16)PILE_MANAG_EN * 100) + (BMU_DC_PWR_TYPE * 10) + 3) << 8) | ((((PRJ_PACK_NTC_INFO % 3) * 100) + ((PRJ_PACK_CELL_INFO % 5) * 10) + PRJ_PARA_NUM_INFO) & 0xff))/*BMU软件版本号高位(区别软件配置[并机/供电/外设.项目信息(NTC类型/PACK类型/项目配置)])*/

#define CELL_CHEM_TYPE		(2)				/*电芯类型(0:铅酸 1:三元  2:磷酸铁锂  3:钛酸锂)*/
#define MAX_BSU_NUM			(15)			/*组最大从控数目[1,32]*/
#define MAX_BSU_CELL		(16)			/*从控最大单体数目[8,18]*/
#define MAX_CELL_NUM		(MAX_BSU_NUM * MAX_BSU_CELL)/*组最大单体数目[8,400]*/

#define BMU_BAS_DEVADDR		(50)			/*BMU起始设备地址[50,65]*/
#define SMU_BAS_DEVADDR		(70)			/*BPU起始设备地址[70,79]*/
#define PCS_BAS_DEVADDR		(80)			/*PCS起始设备地址[80,89]*/
#define IMEU_BAS_DEVADDR	(40)			/*主控内部IMEU地址[40,49]*/
#define IMLU_BAS_DEVADDR	(66)			/*主控内部IMLU地址[66,69]*/

/*主控作主从并机功能*/
#define PILE_MANAG_EN		(0)				/*电池堆并机管理功能使能(0:禁用 1:使能)*/
#define PILE_M_MOV_EN		(0)				/*电池堆并机主机变换使能(0:只在1#电池组 1:选择开机最小组号电池组)*/
#define PILE_MOR_F_EN		(1)				/*电池堆最大功能使能(0:仅含并机功能[减小代码量] 1:含其他功能[较大代码量])*/
#define PILE_DIAG_F_EN		(1)				/*电池堆绝缘粘连检测管理功能使能(0:主控自行诊断 1:堆控协调控制)*/
#define PILE_MAX_G_NUM		(4)				/*电池堆并机最大电池组数*/
#define PILE_RCV_G_NUM		(PILE_MAX_G_NUM-1)/*电池堆并机接收信息电池组数*/

/*通信协议选择*/
#define	BMU_SELF_CAN_EN		(1)				/*BMU自身内部CAN协议使能(0:禁止 1:使能)*/
#define	BMU_CATL_CAN_EN		(0)				/*BMU兼容CATL电池CAN协议使能(0:禁止 1:使能)*/
#define	BMU_FROCK_CAN_EN	(1)				/*BMU测试工装CAN协议使能(0:禁止 1:使能)*/

#define	BMU_CATL_CAN_PORT	(0)				/*BMU兼容CATL电池CAN协议通信CAN口*/

/*项目参数定义*/
#define PRJ_PAR_DEV_ID		(0x0103)		/*设备识别码(H:RTU编号[0,255]  L:设备类型[H：[0,255] L：0EMS,1PCS,2BSMU,3:BMU)])*/
#define PRJ_PAR_PRD_DAT		(22010)			/*设备出厂日期(年月-2000+流水号,例如19090,最后一位为批次流水号)*/
#define PRJ_PAR_INS_SIT		(0x0000)		/*项目编码   H:安装地点 L:安装地编号*/
#define PRJ_PAR_SYS_ID		(0x0101)		/*系统识别码 H:机组号(0非机组)  L:电池组号(0非电池组)*/
#define PRJ_PAR_SYS_TYP		(0x0032)		/*系统定位码 H:系统类型(0二级系统 1扩展三级系统 2独立三级系统) L:系统所在设备号*/
#define PRJ_PAR_SYS_NUM		(0x0101)		/*系统信息  H(系统总数) L:电池组数*/

/*电池极限值定义*/
#define BATT_EXEREM_MAXT     80             /*电芯最高极限温度(1℃)*/
#define BATT_EXEREM_MINT     -30            /*电芯最低极限温度(1℃)*/
#define BATT_EXEREM_MAXC     3300           /*电芯最大极限电流(0.1A)*/

/*默认值定义*/
#if(3 == CELL_CHEM_TYPE)                    /*钛酸锂电芯默认值*/
#define DEFAULT_STANDCV     2100            /*电芯标称电压(1mV)*/
#define DEFAULT_LO_MAXV     2500            /*电芯电压断线高端异常电压(1mV)*/
#define DEFAULT_LO_MINV     1800            /*电芯电压断线低端异常电压(1mV)*/
#define DEFAULT_EX_MAXV     2300            /*电芯最高极限电压(1mV)*/
#define DEFAULT_EX_MINV     1900            /*电芯最低极限电压(1mV)*/

#elif(2 == CELL_CHEM_TYPE)                  /*磷酸铁锂电芯默认值*/
#define DEFAULT_STANDCV     3300            /*电芯标称电压(1mV)*/
#define DEFAULT_LO_MAXV     4500            /*电芯电压断线高端异常电压(1mV)*/
#define DEFAULT_LO_MINV     1300            /*电芯电压断线低端异常电压(1mV)*/
#define DEFAULT_EX_MAXV     3850            /*电芯最高极限电压(1mV)*/
#define DEFAULT_EX_MINV     2000            /*电芯最低极限电压(1mV)*/

#elif(1 == CELL_CHEM_TYPE)                  /*三元电芯默认值*/
#define DEFAULT_STANDCV     3700            /*电芯标称电压(1mV)*/
#define DEFAULT_LO_MAXV     4500            /*电芯电压断线高端异常电压(1mV)*/
#define DEFAULT_LO_MINV     2500            /*电芯电压断线低端异常电压(1mV)*/
#define DEFAULT_EX_MAXV     4250            /*电芯最高极限电压(1mV)*/
#define DEFAULT_EX_MINV     2900            /*电芯最低极限电压(1mV)*/

#else                                       /*铅酸电芯*/
#define DEFAULT_STANDCV     3000            /*电芯标称电压(1mV)*/
#define DEFAULT_LO_MAXV     5000            /*电芯电压断线高端异常电压(1mV)*/
#define DEFAULT_LO_MINV     1000            /*电芯电压断线低端异常电压(1mV)*/
#define DEFAULT_EX_MAXV     2900            /*电芯最高极限电压(1mV)*/
#define DEFAULT_EX_MINV     1100            /*电芯最低极限电压(1mV)*/

#endif

/*主索引长度定义*/
#define SINDEX40_LEN		(15)			/*项目信息数组长度*/

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*协议40子索引列表*/
typedef enum
{
	eSysProj40_DevCode = 0,				/*设备识别码		H:RTU编号 L:设备类型（0EMS,1PCS，2BSMU，3:BMU)*/
	eSysProj40_ProjTime,			    /*项目时间		年月-2000（例如18070，最后一位为批次流水号）*/
	eSysProj40_ProjArea,			    /*项目信息		H:安装地（国） L:安装地（省）*/
	eSysProj40_ProjAddr,			    /*项目编码		H:安装地点 L:安装地编号*/
	eSysProj40_SysCode,			        /*系统识别码		H:机组号(0非机组) L:电池组号(0非电池组)*/
	eSysProj40_SysType,			        /*系统定位码		H:系统类型（0二级系统 1扩展三级系统 2独立三级系统）  L:系统所在设备号*/
	eSysProj40_SysInfoH,			    /*系统信息H		H8:系统总数 L8:电池组数*/
	eSysProj40_SysInfoL,			    /*系统信息L		H6:从控数目 L10:电压总数目*/
	eSysProj40_BattInfo,			    /*电池信息		H6:电池类型(0铅酸 1三元 2磷酸铁锂 3钛酸锂 4其它) L10:温度总数目*/
	eSysProj40_StandVolt,			    /*标称电压*/
	eSysProj40_StandCap,			    /*标称容量*/
	eSysProj40_StandEner,			    /*标称能量*/
	eSysProj40_SysVersion,			    /*系统版本号*/
	eSysProj40_MainVersion,			    /*主控版本号*/
	eSysProj40_SlaveVersion			    /*从控版本号*/

}e_SysProjInfo40;

//协议82子索引列表
typedef enum
{
    eDbgRes82_DbgType = 0,      //调试类型 0:从控均衡调试 1:从控输出IO调试 2:主控接触器调试 3:主控输出IO调试 4:主控LED调试 5:主控EEP参数读写调试
    eDbgRes82_BMUid = 1,        //调试主控编号 0:所有主控 [1,16]主控编号
    eDbgRes82_BSUid = 2,        //调试从控编号 0:所有从控 [1,16]从控编号

    //从控均衡调试索引序号
    eDbgRes82_BalType = 3,      //调试均衡方式 0:禁止均衡 1:主动均衡 2:被动均衡
    eDbgRes82_BalStat = 4,      //调试均衡状态 0:关闭 1:均充 2:均放
    eDbgRes82_BalCellH = 5,     //调试均衡电池(高16节) 按位表示选择的电池号 0:未选择 1:选择
    eDbgRes82_BalCellL = 6,     //调试均衡电池(低16节) 按位表示选择的电池号 0:未选择 1:选择
    eDbgRes82_BalKeep1 = 7,     //调试均衡保留
    eDbgRes82_BalKeep2 = 8,     //调试均衡保留

    //从控输出IO调试索引序号
    eDbgRes82_OutNum = 3,       //调试输出IO编号 0:所有输出IO [1,2]风扇编号
    eDbgRes82_OutStat = 4,      //调试输出IO状态 0:关闭 1:开启
    eDbgRes82_OutKeep0 = 5,     //调试输出保留
    eDbgRes82_OutKeep1 = 6,     //调试输出保留
    eDbgRes82_OutKeep2 = 7,     //调试输出保留
    eDbgRes82_OutKeep3 = 8,     //调试输出保留

    //从控 EEP参数读写调试索引序号
    eDbgRes82_EepAddr = 3,      //读写参数EEP地址
    eDbgRes82_EepSort = 4,      //读写EEP参数数据类型
    eDbgRes82_WDataHW = 5,      //写EEP当前参数高16位
    eDbgRes82_WDataLW = 6,      //写EEP当前参数低16位
    eDbgRes82_RDataHW = 7,      //读EEP当前参数高16位
    eDbgRes82_RDataLW = 8,      //读EEP当前参数低16位

    //主控接触器调试索引序号
    eDbgRes82_SwhNum = 3,       //调试接触器编号 0:所有接触器 [1,4]接触器编号
    eDbgRes82_SwhStat = 4,      //调试接触器状态 0:断开 1:闭合
    eDbgRes82_SwhKeep0 = 5,     //调试接触器保留
    eDbgRes82_SwhKeep1 = 6,     //调试接触器保留
    eDbgRes82_SwhKeep2 = 7,     //调试接触器保留
    eDbgRes82_SwhKeep3 = 8,     //调试接触器保留

    //主控输出IO调试索引序号(同从控)

    //主控 LED调试索引序号
    eDbgRes82_LedNum = 3,       //调试LED编号 0:所有LED [1,6]LED编号
    eDbgRes82_LedStat = 4,      //调试LED状态 0:灭1:亮
    eDbgRes82_LedKeep0 = 5,     //调试LED保留
    eDbgRes82_LedKeep1 = 6,     //调试LED保留
    eDbgRes82_LedKeep2 = 7,     //调试LED保留
    eDbgRes82_LedKeep3 = 8,     //调试LED保留

    //主控 EEP参数读写调试索引序号(同从控)

    eDbgRes82_DbgResult = 9,    //调试结果 0:未调试 1:调试成功 2:非调试模式 3:调试参数错误 4:通讯失败

}e_DebugResult82;

//协议86子索引列表
typedef enum
{
	eSysCmdCtrl86_State = 0,				/*工作状态控制	0:正常停止 1:正常预充 2:正常启动 3:充电启动 4:放电启动 5:强制停机 6:紧急停机 7:下电关机 8:上电开机*/
	eSysCmdCtrl86_Mode = 1,					/*工作模式控制	0:自动 1:手动 2:紧急 5:调试*/
	eSysCmdCtrl86_ClrCmd = 2				/*系统清除命令	1:清除当前故障 2:清除历史故障 3:清除故障数目 4:清除运行计时 5:清除累计电量 6:清除故障记录 7:清除事件记录  8:清除故障录波 9:清除数据记录 10:恢复出厂设置*/

}e_SysCmdCtrl86;

/*协议88子索引列表*/
typedef enum
{
    eDbgCmd88_DbgType = 0,      //调试类型 0:从控均衡调试 1:从控接触器调试 2:从控输出干接点调试 3:从控EEPROM读写调试 4:主控内控输出端口调试 5:主控接触器调试 6:主控输出干接点调试 7:主控EEPROM读写调试
    eDbgCmd88_BMUid = 1,        //调试主控编号 0:所有主控 [1,16]主控编号
    eDbgCmd88_BSUid = 2,        //调试从控编号 0:所有从控 [1,16]从控编号

    //BMU均衡调试索引序号
    eDbgCmd88_BalType = 3,      //调试均衡方式 0:禁止均衡 1:主动均衡 2:被动均衡
    eDbgCmd88_BalStat = 4,      //调试均衡状态 0:关闭 1:均充 2:均放
    eDbgCmd88_BalCellH = 5,     //调试均衡电池(高16节) 按位表示选择的电池号 0:未选择 1:选择
    eDbgCmd88_BalCellL = 6,     //调试均衡电池(低16节) 按位表示选择的电池号 0:未选择 1:选择

    //BMU输出IO调试索引序号
    eDbgCmd88_OutNum = 3,       //调试输出IO编号 0:所有输出IO [1,2]风扇编号
    eDbgCmd88_OutStat = 4,      //调试输出IO状态 0:关闭 1:开启
    eDbgCmd88_OutKeep0 = 5,     //调试输出保留
    eDbgCmd88_OutKeep1 = 6,     //调试输出保留

    //BMU EEP参数读写调试索引序号
    eDbgCmd88_EepAddr = 3,      //读写参数EEP地址
    eDbgCmd88_EepSort = 4,      //读写EEP参数数据类型
    eDbgCmd88_WDataHW = 5,      //写EEP当前参数高16位
    eDbgCmd88_WDataLW = 6,      //写EEP当前参数低16位

    //BMU接触器调试索引序号
    eDbgCmd88_SwhNum = 3,       //调试接触器编号 0:所有接触器 [1,4]接触器编号
    eDbgCmd88_SwhStat = 4,      //调试接触器状态 0:断开 1:闭合
    eDbgCmd88_SwhKeep0 = 5,     //调试接触器保留
    eDbgCmd88_SwhKeep1 = 6,     //调试接触器保留

    //BMU输出IO调试索引序号(同BMU)

    //BMU LED调试索引序号
    eDbgCmd88_LedNum = 3,       //调试LED编号 0:所有LED [1,6]LED编号
    eDbgCmd88_LedStat = 4,      //调试LED状态 0:灭1:亮
    eDbgCmd88_LedKeep0 = 5,     //调试LED保留
    eDbgCmd88_LedKeep1 = 6,     //调试LED保留

    //BMU外设调试索引序号
    eDbgCmd88_DevSort = 3,      //调试外设类型 0:调速风扇 1:百叶风机 2:空调电源 3:空调送风 4:空调待机 5:空调制冷 6:空调制热 7:空调除湿 8:照明 9:声光报警 10:消防触发
    eDbgCmd88_DevState = 4,     //调试外设状态 0:关闭 1:开启 [2,100]:调试参数(风扇调速或空调参数)
    eDbgCmd88_DevFanH = 5,      //调试外设风扇选择(高8通道)
    eDbgCmd88_DevFanL = 6,      //调试外设风扇选择(低8通道)

    //BMU EEP参数读写调试索引序号(同BSU)

    eDbgCmd88_Affirm = 7        //调试确认 1-确认 1-读EEP确认 2-写EEP确认
}e_DebugCmd88;

/*系统保护状态值*/
typedef enum
{
    ePRO_WAIT   = 0,                    /*未就绪状态       [电池正常但直流回路未就绪]*/
    ePRO_READY  = 1,                    /*正常就绪状态    [电池正常且允许充电和放电]*/
    ePRO_FULL   = 2,                    /*充电截止状态    [电池充满仅允许放电]*/
    ePRO_EMPTY  = 3,                    /*放电截止状态    [电池放空仅允许充电]*/
    ePRO_SYSLIM = 4,                    /*充放电限制状态[直流回路正常但不允许充电或放电]*/
    ePRO_SYSPRO = 5,                    /*充放电保护状态[充放电回路断开故障保护]*/

    ePRO_STATE                          /*系统保护状态取值长度*/
}e_ProState;

/*系统工作状态值*/
typedef enum
{
    eWORK_INIT  = 0,                    /*开机初始化状态*/
    eWORK_IDLE  = 1,                    /*待命空闲状态*/
    eWORK_START = 2,                    /*正在启动状态*/
    eWORK_RUN   = 3,                    /*运行状态*/
    eWORK_STOP  = 4,                    /*正在停机状态*/
    eWORK_ERR   = 5,                    /*故障状态*/
    eWORK_OFF   = 6,                    /*关机下电状态*/

    eWORK_STATE                         /*系统工作状态*/
}e_WorkState;

/*系统充放电状态值*/
typedef enum
{
    eCURR_IDLE = 0,                     /*空闲状态*/
    eCURR_CHG  = 1,                     /*充电状态*/
    eCURR_DHG  = 2,                     /*放电状态*/

    eCURR_STATE                         /*系统充放电状态*/
}e_CurrState;

/*系统工作模式值*/
typedef enum
{
    eMODE_AUTO  = 0,                    /*自动模式*/
    eMODE_HAND  = 1,                    /*手动模式*/
    eMODE_EMERG = 2,                    /*紧急模式*/
    eMODE_DEBUG = 5,                    /*调试模式*/

    eMODE_STATE                         /*系统工作模式*/
}e_WorkMode;

/*系统控制命令值*/
typedef enum
{
    eCTRL_NORSTOP  = 0,                 /*正常停机命令*/
    eCTRL_PRECHG   = 1,                 /*启动预充命令*/
    eCTRL_NORSTART = 2,                 /*正常启动命令*/
    eCTRL_CHGSTART = 3,                 /*充电启动命令*/
    eCTRL_DHGSTART = 4,                 /*放电启动命令*/
    eCTRL_FORCSTOP = 5,                 /*强制停机命令*/
    eCTRL_EMERSTOP = 6,                 /*紧急停机命令*/
    eCTRL_POWEROFF = 7,                 /*下电关机命令*/
    eCTRL_POWERON  = 8,                 /*上电开机命令*/

    eCTRL_SYSCMD                        /*系统控制命令*/
}e_SysCtrlCmd;

/*系统清除命令值*/
typedef enum
{
    eCLR_NONE    = 0,                   /*无效命令*/
    eCLR_NOWERR  = 1,                   /*清除当前故障*/
    eCLR_ERRRCD  = 2,                   /*清除故障记录*/
    eCLR_ERRNUM  = 3,                   /*清除故障数目*/
    eCLR_RUNTIME = 4,                   /*清除运行计时*/
    eCLR_HISCAP  = 5,                   /*清除累计电量*/
    eCLR_DODRCD  = 6,                   /*清除DOD记录*/
    eCLR_EVENRCD = 7,                   /*清除事件记录*/
    eCLR_WAVERCD = 8,                   /*清除故障录波*/
    eCLR_HISDATA = 9,                   /*清除数据记录*/
    eCLR_FACTORY = 10,                  /*恢复出厂设置*/
    eCLR_FALCLIM = 11,                  /*复位充放电降功率限制*/
    eCLR_CDHGLIM = 12,                  /*复位充放电截止限制*/
    eCLR_SOSLOCK = 13,                  /*清除SOS锁定*/

    eCLR_SYSCLR                         /*系统清除命令*/
}e_SysClrCmd;

//系统调试类型命令值
typedef enum
{
    eDBGCMD_BSUBAL = 0,                 //从控均衡调试
    eDBGCMD_BSUSWH = 1,                 //从控接触器调试
    eDBGCMD_BSUDRY = 2,                 //从控干接点调试
    eDBGCMD_BSUEEP = 3,                 //从控EEP读写调试
    eDBGCMD_BMUOUT = 4,                 //主控内控输出端口调试
    eDBGCMD_BMUSWH = 5,                 //主控接触器调试
    eDBGCMD_BMUDRY = 6,                 //主控干接点调试
    eDBGCMD_BMUEEP = 7,                 //主控EEP读写调试
    eDBGCMD_BMUAUX = 8,                 //主控外设调试

    eDBGCMD_SMUOUT = 10,                //堆控内控输出端口调试
    eDBGCMD_SMUSWH = 11,                //堆控接触器调试
    eDBGCMD_SMUDRY = 12,                //堆控干接点调试
    eDBGCMD_SMUEEP = 13,                //堆控EEP读写调试

    eDBGCMD_FIN                         //命令结束
}e_DebugTypeCmd;

/*系统调试结果值*/
typedef enum
{
    eDBGRES_UNEXE = 0,                  //未执行调试
	eDBGRES_SUCCE,                      //调试成功
	eDBGRES_NOMOD,                      //非调试模式
	eDBGRES_ERRPA,                      //错误参数
	eDBGRES_CANOF,                      //通信中断
	eDBGRES_FIN                         //调试结果结束
}e_DebugResult;

/*系统采样校准命令值*/
typedef enum
{
	eCORR_INVALID = 0,                  /*0:无校准*/
	eCORR_HANDZRO = 1,                  /*1:电流零飘手动校准*/
	eCORR_HALOUTS = 2,                  /*2:霍尔电流单点校准*/
	eCORR_HALOUTD = 3,                  /*3:霍尔电流双点校准*/
	eCORR_SHUCHGS = 4,                  /*4:分流器充电电流单点校准*/
	eCORR_SHUCHGD = 5,                  /*5:分流器充电电流双点校准*/
	eCORR_SHUDHGS = 6,                  /*6:分流器放电电流单点校准*/
	eCORR_SHUDHGD = 7,                  /*7:分流器放电电流双点校准*/
	eCORR_SUMVOLS = 10,                 /*10:电池总压单点校准*/
	eCORR_SUMVOLD = 11,                 /*11:电池总压双点校准*/
	eCORR_PREVOLS = 12,                 /*12:预充电压单点校准*/
	eCORR_PREVOLD = 13,                 /*13:预充电压双点校准*/
	eCORR_LINVOLS = 14,                 /*14:Link电压单点校准*/
	eCORR_LINVOLD = 15,                 /*15:Link电压双点校准*/
}e_SysSampCorrCmd;

/*系统控制命令端口值*/
typedef enum
{
    eCMDPORT_CAN0 = 0,                  /*CAN0口*/
	eCMDPORT_CAN1 = 1,                  /*CAN1口*/
	eCMDPORT_CAN2 = 2,                  /*CAN2口*/
	eCMDPORT_SCI0 = 5,                  /*RS485-0口*/
	eCMDPORT_SCI1 = 6,                  /*RS485-1口*/
	eCMDPORT_SCI2 = 7,                  /*RS485-2口*/
	eCMDPORT_BUTT = 10,                 /*按钮口*/
	eCMDPORT_FSTC = 11,                 /*快充口*/
	eCMDPORT_INTE = 12,                 /*内部*/
}e_SysCmdPortt;

/*系统状态检测结果值*/
typedef enum
{
    eSYSRESULT_KEEP = 0,                /*系统检测等待*/
    eSYSRESULT_FIN  = 1,                /*系统检测正常*/
    eSYSRESULT_ERR  = 2,                /*系统检测故障*/
}e_SysStateResult;

/*故障诊断类型/级别*/
typedef enum
{
    eErr_Norm   = 0,                    /*正常(不检测)*/
    eErr_Warm   = 1,                    /*仅提示类*/
    eErr_ChgLim = 2,                    /*充电限制类(发生时限制充电,继续充电则请求停机[不断接触器])*/
    eErr_DhgLim = 3,                    /*放电限制类(发生时限制放电,继续放电则请求停机[不断接触器])*/
    eErr_SysLim = 4,                    /*充放电限制类(发生时限制充放电,继续充/放电则请求停机[不断接触器])*/
    eErr_ChgPro = 5,                    /*充电保护类(发生时限制充电,继续充电则请求停机和断接触器)*/
    eErr_DhgPro = 6,                    /*放电保护类(发生时限制放电,继续放电则请求停机和断接触器)*/
    eErr_SysPro = 7,                    /*充放电保护类(发生时限制充放电,请求停机和断接触器[需要断接触器的充电保护或放电保护需要设置为充放电保护故障类型]) */
    eErr_SysErr = 8,                    /*系统故障类(发生时限制充放电,系统状态故障,请求停机和断接触器)*/
}e_ErrSort;

/*系统时间定义*/
typedef struct
{
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u16 msecond;
}t_SysClock;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=================================================================================================
** @Name      : void SysPublicInfoInit(void)
** @Input     : void
** @Output    : void
** @Function  : 系统系统通用信息初始化
** @The notes :
**===============================================================================================*/
void SysPublicInfoInit(void);

/*=================================================================================================
** @Name      : void SysClockCountTask(void)
** @Input     : void
** @Output    : void
** @Function  : 系统时钟(北京时间)计数任务,ms计时
** @The notes : 1ms定时中断调用
**===============================================================================================*/
void SysClockCountTask(void);

/*=================================================================================================
** @Name      : void SysClockRecordTask(void)
** @Input     : void
** @Output    : void
** @Function  : 系统时钟时间记录任务(包括记录运行时间)
** @The notes : 2s周期调用
**===============================================================================================*/
void SysClockRecordTask(void);

/*=================================================================================================
** @Name      : void SysDispProjInfoTask(void)
** @Input     : void
** @Output    : void
** @Function  : 系统项目参数更新显示任务
** @The notes : 2s周期调用
**===============================================================================================*/
void SysDispProjInfoTask(void);

/*=================================================================================================
** @Name      : void SysClockExtVerifyAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 系统时钟外部校时处理
** @The notes : 接收校时报文时调用
**===============================================================================================*/
void SysClockExtVerifyAPI(void);

/*=================================================================================================
** @Name      : void SysClockRtcVerifyAPI(t_SysClock *rtcClock)
** @Input     : rtcClock:RTC当前时间
** @Output    : void
** @Function  : 系统时钟RTC校时处理
** @The notes : 发现系统时钟与RTC不一致时调用
**===============================================================================================*/
void SysClockRtcVerifyAPI(t_SysClock *rtcClock);

/*=================================================================================================
** @Name      : void SysClockUpdateAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 将显示区校时信息更新到系统时钟信息
** @The notes : 校时信息变化时调用
**===============================================================================================*/
void SysClockUpdateAPI(void);

/*=================================================================================================
** @Name      : void SysClockDisplayAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 更新系统时钟信息到显示区
** @The notes : 查询系统时间时调用
**===============================================================================================*/
void SysClockDisplayAPI(void);

/*=================================================================================================
** @Name      : void SysDevSerialDispayAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 读取系统设备条码序列号进行显示
** @The notes : 从存储区读取
**===============================================================================================*/
void SysDevSerialDispayAPI(void);

/*=================================================================================================
** @Name      : void ClrSysSecondTimeAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除电池组系统运行计时
** @The notes :
**===============================================================================================*/
void ClrSysSecondTimeAPI(void);

/*=================================================================================================
** @Name      : u32 GetSysSecondTimeAPI(void)
** @Input     : void
** @Output    : 系统计时 1s
** @Function  : 获取电池组系统运行计时
** @The notes : 上电开始计时,考虑可能会外部清零复位
**===============================================================================================*/
u32 GetSysSecondTimeAPI(void);

 /*=================================================================================================
 ** @Name      : t_SysClock GetSysDateClockAPI(void)
 ** @Input     : void
 ** @Output    : 系统时钟 年/月/日/时/分/秒/毫秒
 ** @Function  : 获取电池组系统时钟
 ** @The notes : 北京时间
 **===============================================================================================*/
 t_SysClock GetSysDateClockAPI(void);

 //=============================================================================================
 //函数名称	: u8 GetSysSelfGroupIdAPI(void)
 //输入参数	: void
 //输出参数	: 系统自身所在电池组组号
 //函数功能	: 获取系统自身所在电池组组号
 //注意事项	: 自身主控序号
 //=============================================================================================
 u8 GetSysSelfGroupIdAPI(void);

#endif
