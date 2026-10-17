/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PublicPara.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 系统公共参数模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "ParaInterface.h"
#include "PublicPara.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
u16 gSysGenParaRO_100[SINDEX100_LEN] =  		/*系统通用参数设置*/
{
	23130,	/*配置标志*/
	0,		/*电池组动态并机使能状态 [0,1] 0:同时接入 1:动态接入*/
	0,		/*电池组故障切除使能 状态[0,1] 0:同时切除 1:动态切除*/
	0,		/*电池组充电切除使能 状态[0,1] 0:同时切除 1:动态切除*/
	0,		/*电池组放电切除使能状态 [0,1] 0:同时切除 1:动态切除*/
	1,		/*电池组最小运行组数值 [1,16](最少正常电池组数)*/
	1,		/*电池组并机总组数值 [1,16](母线上电池组数)*/
	70		/*上层通信设备地址(BSMU或PCS) [70,89]*/
};

u16 gSysHigParaRO_101[SINDEX101_LEN] =  		/*系统硬件参数设置*/
{
	23130,	/*配置标志*/
	1,		/*权限默认开启端口设置 [0,9]	0:均不开启 1:全部开启 [2,5]:CAN口1-4 [6,9]:RS485口1-4*/
	CAN0_BPS,/*CAN#1口波特率设置  1Kbps	125,250,500,1000*/
	CAN1_BPS,/*CAN#2口波特率设置  1Kbps	125,250,500,1000*/
	CAN2_BPS,/*CAN#3口波特率设置  1Kbps	125,250,500,1000*/
	50,		/*远程端口站地址设置  [1,250]*/
	SCI0_BPS,/*远程端口波特率设置  1bps	 9600,19200,38400*/
	70,		/*就地端口站地址设置  [1,250]*/
	SCI1_BPS,/*就地端口波特率设置  1bps	 9600,19200,38400*/
	100,	/*监控端口站地址设置  [1,250]*/
	SCI2_BPS,/*监控端口波特率设置  1bps	 9600,19200,38400*/
	1,		/*自身IP网段号设置  [1,254]*/
	50,		/*自身IP主机号设置 [1,254]*/
	1,		/*服务器IP网段号设置  [1,254]*/
	100		/*服务器IP主机号设置  [1,254]*/
};

u16 gSysGenParaRW_105[SINDEX105_LEN] =  			/*系统通用设置*/
{
	23130,	/*配置标志*/
	0,		/*电池组动态并机使能设置 [0,1] 0:同时接入 1:动态接入*/
	0,		/*电池组故障切除使能设置 [0,1] 0:同时切除 1:动态切除*/
	0,		/*电池组充电切除使能设置 [0,1] 0:同时切除 1:动态切除*/
	0,		/*电池组放电切除使能设置 [0,1] 0:同时切除 1:动态切除*/
	1,		/*电池组最小运行组数设置 [1,16] 最少正常电池组数*/
	1,		/*电池组并机总组数设置 [1,16]	母线上电池组数*/
	70,		/*上层通信设备地址(BSMU或PCS) [70,89]*/
	1		/*电池组激活状态设置 按位表示，每bit表示一个电池组*/
};

u16 gSysHigParaRW_106[SINDEX106_LEN] =  		/*系统高级设置*/
{
	0,		/*配置标志*/
	0,		/*权限默认开启端口设置 [0,9]	0:均不开启 1:全部开启 [2,5]:CAN口1-4 [6,9]:RS485口1-4*/
	0,		/*CAN#1口波特率设置  1Kbps	125,250,500,1000*/
	0,		/*CAN#2口波特率设置  1Kbps	125,250,500,1000*/
	0,		/*CAN#3口波特率设置  1Kbps	125,250,500,1000*/
	0,		/*远程端口站地址设置  [1,250]*/
	0,		/*远程端口波特率设置  1bps	 9600,19200,38400*/
	0,		/*就地端口站地址设置  [1,250]*/
	0,		/*就地端口波特率设置  1bps	 9600,19200,38400*/
	0,		/*监控端口站地址设置  [1,250]*/
	0,		/*监控端口波特率设置  1bps	 9600,19200,38400*/
	0,		/*自身IP网段号设置  [1,254]*/
	0,		/*自身IP主机号设置 [1,254]*/
	0,		/*服务器IP网段号设置  [1,254]*/
	0		/*服务器IP主机号设置  [1,254]*/
};

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 ParaCheckSGenParaEffect(u32 addr, u32 para)
** @Input     : addr:参数地址 para:参数值
** @Output    : TRUE:有效合法 FALSE:非法无效
** @Function  : 检测系统通用参数值是否有效
** @The notes : U16
**===============================================================================================*/
u8 ParaCheckSGenParaEffect(u32 addr, u32 para)
{
    /*根据地址判断有效性*/
	switch(addr)
	{
		/*系统通用参数配置标志*/
		case PARA_SYSGEN_PARA_ADDR + (2 * eSGenPara100_CfgFlag):
			/*不需检有效性*/
			return(TRUE);

		/*电池组动态并机使能状态 [0,1] 0:同时接入 1:动态接入*/
		case PARA_SYSGEN_PARA_ADDR + (2 * eSGenPara100_AutConn):
		/*电池组故障切除使能 状态[0,1] 0:同时切除 1:动态切除*/
		case PARA_SYSGEN_PARA_ADDR + (2 * eSGenPara100_ErrSepa):
		/*电池组充电切除使能 状态[0,1] 0:同时切除 1:动态切除*/
		case PARA_SYSGEN_PARA_ADDR + (2 * eSGenPara100_ChgSepa):
		/*电池组放电切除使能状态 [0,1] 0:同时切除 1:动态切除*/
		case PARA_SYSGEN_PARA_ADDR + (2 * eSGenPara100_DhgSepa):
			if(para <= 1)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

		/*电池组最小运行组数值 [1,16](最少正常电池组数)*/
		case PARA_SYSGEN_PARA_ADDR + (2 * eSGenPara100_MinGNum):
		/*电池组并机总组数值 [1,16](母线上电池组数)*/
		case PARA_SYSGEN_PARA_ADDR + (2 * eSGenPara100_AllGNum):
			if((para >= MIN_BMU_NUM_CFG) && (para <= MAX_BMU_NUM_CFG))
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

		/*上层通信设备地址(BSMU或PCS) [70,89]*/
		case PARA_SYSGEN_PARA_ADDR + (2 * eSGenPara100_ExtAddr):
			if((para >= MIN_EXT_ADDR_CFG) && (para <= MAX_EXT_ADDR_CFG))
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

		/*电池组激活使能地址[0,65535]*/
		case PARA_SYSGEN_PARA_ADDR + (2 * eSGenPara105_GroupEn):
			//if((para & (0xffff << MAX_BMU_NUM_CFG)) > 0)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

		default:
			break;
	}

    return(FALSE);
}

/*=================================================================================================
 ** @Name      : u8 ParaCheckSHWParaEffect(u32 addr, u32 para)
 ** @Input     : addr:参数地址 para:参数值
 ** @Output    : TRUE:有效合法 FALSE:非法无效
 ** @Function  : 检测系统硬件参数值是否有效
 ** @The notes :
 **===============================================================================================*/
 u8 ParaCheckSHWParaEffect(u32 addr, u32 para)
 {
     /*根据地址判断有效性*/
 	switch(addr)
 	{
 		/*系统硬件参数配置标志*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_CfgFlag):
 			/*不需检有效性*/
 			return(TRUE);

 		/*权限默认开启端口 [0,9]	0:均不开启 1:全部开启 [2,5]:CAN口1-4 [6,9]:RS485口1-4*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_PermPort):
			if(para <= 9)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

	 	/*CAN#1口波特率  1Kbps	125,250,500,1000*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_CAN0Rate):
 		/*CAN#2口波特率  1Kbps	125,250,500,1000*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_CAN1Rate):
 		/*CAN#3口波特率  1Kbps	125,250,500,1000*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_CAN2Rate):
			if((125 == para) || (250 == para) || (500 == para) || (1000 == para))
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

 		/*远程端口站地址  [1,250]*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_RemoteAddr):
		/*就地端口站地址  [1,250]*/
		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_LocalAddr):
 		/*监控端口站地址  [1,250]*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_MonitAddr):
			if((para >= 1) && (para <= 250))
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

 		/*远程端口波特率  1bps	 9600,19200,38400*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_RemoteRate):
 		/*就地端口波特率  1bps	 9600,19200,38400*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_LocalRate):
 		/*监控端口波特率  1bps	 9600,19200,38400*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_MonitRate):
			if((9600 == para) || (19200 == para) || (38400 == para))
			{
				/*参数合法*/
				return(TRUE);
			}
			break;


 		/*自身IP网段号  [1,254]*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_SelfIPSeg):
 		/*自身IP主机号 [1,254]*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_SelfIPHID):
 		/*服务器IP网段号  [1,254]*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_ServIPSeg):
 		/*服务器IP主机号  [1,254]*/
 		case PARA_SYSHW_PARA_ADDR + (2 * eSHWPara101_ServIPHID):
			if((para >= 1) && (para <= 254))
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

		default:
			break;
	}

	return(FALSE);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
