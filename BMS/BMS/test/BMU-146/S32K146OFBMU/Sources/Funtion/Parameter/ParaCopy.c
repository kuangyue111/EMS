/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ParaCopy.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 参数存储模块备份参数
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "GroupPara.h"
#include "PublicPara.h"
#include "ParaInterface.h"
#include "ParaCopy.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
/*CRC 循环冗余表*/
const u8 cEepCRCHiExp[] = {
    0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
    0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
    0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01,
    0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41,
    0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81,
    0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0,
    0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01,
    0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40,
    0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
    0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
    0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01,
    0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,
    0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
    0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
    0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01,
    0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,
    0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
    0x40
};

const u8 cEepCRCLoExp[] = {
    0x00, 0xc0, 0xc1, 0x01, 0xc3, 0x03, 0x02, 0xc2, 0xc6, 0x06, 0x07, 0xc7, 0x05, 0xc5, 0xc4,
    0x04, 0xcc, 0x0c, 0x0d, 0xcd, 0x0f, 0xcf, 0xce, 0x0e, 0x0a, 0xca, 0xcb, 0x0b, 0xc9, 0x09,
    0x08, 0xc8, 0xd8, 0x18, 0x19, 0xd9, 0x1b, 0xdb, 0xda, 0x1a, 0x1e, 0xde, 0xdf, 0x1f, 0xdd,
    0x1d, 0x1c, 0xdc, 0x14, 0xd4, 0xd5, 0x15, 0xd7, 0x17, 0x16, 0xd6, 0xd2, 0x12, 0x13, 0xd3,
    0x11, 0xd1, 0xd0, 0x10, 0xf0, 0x30, 0x31, 0xf1, 0x33, 0xf3, 0xf2, 0x32, 0x36, 0xf6, 0xf7,
    0x37, 0xf5, 0x35, 0x34, 0xf4, 0x3c, 0xfc, 0xfd, 0x3d, 0xff, 0x3f, 0x3e, 0xfe, 0xfa, 0x3a,
    0x3b, 0xfb, 0x39, 0xf9, 0xf8, 0x38, 0x28, 0xe8, 0xe9, 0x29, 0xeb, 0x2b, 0x2a, 0xea, 0xee,
    0x2e, 0x2f, 0xef, 0x2d, 0xed, 0xec, 0x2c, 0xe4, 0x24, 0x25, 0xe5, 0x27, 0xe7, 0xe6, 0x26,
    0x22, 0xe2, 0xe3, 0x23, 0xe1, 0x21, 0x20, 0xe0, 0xa0, 0x60, 0x61, 0xa1, 0x63, 0xa3, 0xa2,
    0x62, 0x66, 0xa6, 0xa7, 0x67, 0xa5, 0x65, 0x64, 0xa4, 0x6c, 0xac, 0xad, 0x6d, 0xaf, 0x6f,
    0x6e, 0xae, 0xaa, 0x6a, 0x6b, 0xab, 0x69, 0xa9, 0xa8, 0x68, 0x78, 0xb8, 0xb9, 0x79, 0xbb,
    0x7b, 0x7a, 0xba, 0xbe, 0x7e, 0x7f, 0xbf, 0x7d, 0xbd, 0xbc, 0x7c, 0xb4, 0x74, 0x75, 0xb5,
    0x77, 0xb7, 0xb6, 0x76, 0x72, 0xb2, 0xb3, 0x73, 0xb1, 0x71, 0x70, 0xb0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9c, 0x5c,
    0x5d, 0x9d, 0x5f, 0x9f, 0x9e, 0x5e, 0x5a, 0x9a, 0x9b, 0x5b, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4b, 0x8b, 0x8a, 0x4a, 0x4e, 0x8e, 0x8f, 0x4f, 0x8d, 0x4d, 0x4c, 0x8c,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
};

static u8 sParaEepCopyReqFlag = 0;                          /*EEP备份请求标志(按位表示需要备份的扇区)*/
static u32 sParaEepCopyTime = 0;                            /*EEP备份请求时间(记录最近一次)*/
static u8 sParaFlashCopyReqFlag = 0;                        /*FLASH备份请求标志(按位表示需要备份的扇区)*/
static u32 sParaFlashCopyTime = 0;                          /*FLASH备份请求时间(记录最近一次)*/
static u16 sParaEepCopyReqInfo[3][2] = {0};                 /*EEP备份请求的各扇区请求信息(备份起始地址和结束地址)*/

static u8 sParaEepCopySec = 0;                              /*正执行EEP备份的扇区(按位表示扇区)*/
static u8 sParaFlashCopySec = 0;                            /*正执行Flash备份的扇区(按位表示扇区)*/
static u16 sParaEepCopySecInfo[3][2] = {0};                 /*需要执行EEP备份的各扇区信息(备份起始地址和结束地址)*/

static u8 sParaCopyEepStep = 0xff;                          /*EEPROM备份参数执行步骤 0:开始备份 1-20:备份过程 0xff:备份完成*/
static u8 sParaCopyFlashStep = 0xff;                        /*FLASH备份参数执行步骤 0:开始备份 1-20:备份过程 0xff:备份完成*/

extern u16 gSysGenParaRO_100[];  							/*系统通用设置*/
extern u16 gSysHigParaRO_101[];  							/*系统硬件设置*/
extern u16 gGBmuGenPara_102[];  							/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];							/*主控高级参数值*/
extern u8 gGHardPara_104[];  								/*主控硬件参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void ParaCheckSecCrcAndCopyReq(u8 secNum, u8 flag)
** @Input     : secNum:扇区号[0, 2] flag:立即备份标志(0延时备份 1立即备份)
** @Output    : 16位CRC校验码
** @Function  : 检测外部EEPROM各扇区检验码和设置扇区备份请求
** @The notes :
**===============================================================================================*/
static void ParaCheckSecCrcAndCopyReq(u8 secNum, u8 flag);

/*=================================================================================================
** @Name      : static void ParaCopyIntEepDataToExtEepDeal(void)
** @Input     : void
** @Output    : void
** @Function  : 将内部EEP参数备份到外部处理
** @The notes : 分次备份扇区数据,每次104byte,遇到跨页分两次拷贝
**===============================================================================================*/
static void ParaCopyIntEepDataToExtEepDeal(void);

/*=================================================================================================
** @Name      : static void ParaCopyEepDataToIntFlashDeal(void)
** @Input     : void
** @Output    : void
** @Function  : 将EEP参数备份到Flash处理
** @The notes : 分次备份扇区数据,每次104byte,FLASH要求写入长度必须是8的倍数
**===============================================================================================*/
static void ParaCopyEepDataToIntFlashDeal(void);

/*=================================================================================================
** @Name      : static u16 CalcExtEepSecDataCrc(u16 secAddr)
** @Input     : secAddr:指定参数扇区EEP起始地址
** @Output    : 扇区CRC校验码
** @Function  : 计算外部EEP指定地址块参数CRC校验码
** @The notes : 一个扇区数据(必须是指定起始地址才有效)
**===============================================================================================*/
static u16 CalcExtEepSecDataCrc(u16 secAddr);

/*=================================================================================================
** @Name      : static u16 CalcIntEepSecDataCrc(u16 secAddr)
** @Input     : secAddr:指定参数扇区EEP起始地址
** @Output    : 扇区CRC校验码
** @Function  : 计算内部EEP指定地址块参数CRC校验码
** @The notes : 一个扇区数据(必须是指定起始地址才有效)
**===============================================================================================*/
static u16 CalcIntEepSecDataCrc(u16 secAddr);

/*=================================================================================================
** @Name      : static u16 CalcIntFlashSecDataCrc(u32 secAddr)
** @Input     : secAddr:指定参数扇区Flash起始地址
** @Output    : 扇区CRC校验码
** @Function  : 计算内部Flash指定地址扇区参数CRC校验码
** @The notes : 一个扇区数据(必须是指定起始地址才有效)
**===============================================================================================*/
static u16 CalcIntFlashSecDataCrc(u32 secAddr);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void ParaStoreCopyTask(void)
** @Input     : void
** @Output    : void
** @Function  : 参数存储备份任务
** @The notes : 100ms周期调用
**===============================================================================================*/
void ParaStoreCopyTask(void)
{
    static u8 sFlag = 0;
    static u8 sState = 0;
    static u32 sHisTime = 0;

    switch(sState)
    {
    	//备份任务
    	case 0:
    	    /*需要执行EEPROM备份*/
    	    if(sParaEepCopySec > 0)
    	    {
    	        /*执行EEPROM备份处理任务*/
    	    	ParaCopyIntEepDataToExtEepDeal();
    	    }
    	    /*需要执行FLASH备份*/
    	    else if(sParaFlashCopySec > 0)
    	    {
    	        /*执行FLASH备份处理任务*/
    	        ParaCopyEepDataToIntFlashDeal();
    	    }
    	    /*EEPROM请求备份,上电延迟2s后立刻或延迟60s后备份到外部EEPROM*/
    	    else if((sParaEepCopyReqFlag > 0) && (BSPGetOSMillSecTimeAPI() > 2000)
    	        && ((0 == sParaEepCopyTime) || (ABS(GetSysSecondTimeAPI(), sParaEepCopyTime) >= 60)))
    	    {
    	    	/*外部EEPROM读写异常*/
    	   	    if(FALSE == ParaCheckExtEepReadyHook())
    	    	{
    	   	    	sParaEepCopyReqFlag = 0;
    	    	}
    	        /*外部EEPROM读写正常*/
    	   	    else
    	        {
    	   	    	/*记录请求备份的起始地址和结束地址*/
    	   	    	if(1 == BitGet(sParaEepCopyReqFlag, 0))
    	   	    	{
    	   	    		sParaEepCopySecInfo[0][0] = sParaEepCopyReqInfo[0][0];
    	   	    		sParaEepCopySecInfo[0][1] = sParaEepCopyReqInfo[0][1];
    	   	    	}
    	   	    	if(1 == BitGet(sParaEepCopyReqFlag, 1))
    	   	    	{
    	   	    		sParaEepCopySecInfo[1][0] = sParaEepCopyReqInfo[1][0];
    	   	    		sParaEepCopySecInfo[1][1] = sParaEepCopyReqInfo[1][1];
    	   	    	}
    	   	    	if(1 == BitGet(sParaEepCopyReqFlag, 2))
    	   	    	{
    	   	    		sParaEepCopySecInfo[2][0] = sParaEepCopyReqInfo[2][0];
    	   	    		sParaEepCopySecInfo[2][1] = sParaEepCopyReqInfo[2][1];
    	   	    	}

    	            /*记录需要备份的EEPROM扇区*/
    	        	sParaEepCopySec |= sParaEepCopyReqFlag;

    	            sParaEepCopyReqFlag = 0;

    	    		/*未执行EEPROM备份*/
    	        	if(0xff == sParaCopyEepStep)
    	        	{
    	        		/*开始EEPROm备份*/
    	        		sParaCopyEepStep = 0;
    	        	}
    	        }
    	    }
    	    /*FLASH请求备份,上电延迟5s后立刻或间隔1小时备份到内部FLASH*/
    	    else if((sParaFlashCopyReqFlag > 0) && (BSPGetOSMillSecTimeAPI() > 5000)
    	        && ((0 == sParaFlashCopyTime) || (ABS(GetSysSecondTimeAPI(), sParaFlashCopyTime) >= 3600)))
    	    {
    	   	    /*记录需要备份的FLASH扇区*/
    	        sParaFlashCopySec |= sParaFlashCopyReqFlag;

    	        sParaFlashCopyReqFlag = 0;

    			/*未执行FLASH备份*/
    	    	if(0xff == sParaCopyFlashStep)
    	    	{
    	    		/*开始FLASH备份*/
    	        	sParaCopyFlashStep = 0;
    	    	}
    	    }
    	    //参数未修改变化(开机3s后)
    	    else if(BSPGetOSMillSecTimeAPI() > 3000)
    	    {
    	        //系统状态或充放电状态变换更新一次EEPROM各扇区校验码和立即备份
    	        if(TRUE == ParaGetStateChangReqCopyHook())
    	        {
    	            //计算个扇区参数校验码是否变化并请求备份
    	        	sHisTime = GetSysSecondTimeAPI();
    	        	sState = 1;
    	        	//sFlag = 1;
    	        	sFlag = 0;
    	        }
    	        //刚上电或间隔10min更新一次EEPROM各扇区校验码和延时备份
    	        else if((0 == sHisTime) || (ABS(GetSysSecondTimeAPI(), sHisTime) >= 600))
    	        {
    	            //计算个扇区参数校验码是否变化并请求备份
    	        	sHisTime = GetSysSecondTimeAPI();
    	        	sState = 1;
    	        	sFlag = 0;
    	        }
    	    }
    	    break;

		//判断扇区1是否需要备份
		case 1:
            //计算个扇区参数校验码是否变化并请求备份
        	ParaCheckSecCrcAndCopyReq(0, sFlag);
        	sState = 2;
    		break;

		//判断扇区2是否需要备份
		case 2:
			//计算个扇区参数校验码是否变化并请求备份
			ParaCheckSecCrcAndCopyReq(1, sFlag);
			sState = 3;
			break;

		//判断扇区3是否需要备份
		case 3:
			//计算个扇区参数校验码是否变化并请求备份
			ParaCheckSecCrcAndCopyReq(2, sFlag);
			sState = 0;
			break;

		//异常
		default:
			sState = 0;
			break;
    }
}

/*=================================================================================================
** @Name      : void ParaSetEepCopyReqFlag(u16 addr, u16 len, u8 flag)
** @Input     : addr:需备份的EEPROM起始地址 len:备份地址长度 flag:立即备份标志(0延时备份 1立即备份)
** @Output    : void
** @Function  : 设置EEPROM参数备份请求标志
** @The notes : 根据地址设置需要备份的扇区和记录备份时间
**===============================================================================================*/
void ParaSetEepCopyReqFlag(u16 addr, u16 len, u8 flag)
{
	/*立即备份*/
	if((1 == flag) || (BSPGetOSMillSecTimeAPI() < 5000))
	{
		sParaEepCopyTime = 0;
	}
	/*延时备份*/
	else
	{
		/*没有备份请求才更新请求时间*/
		if((0 == sParaEepCopyTime)
			|| (0 == sParaEepCopyReqFlag))
		{
			sParaEepCopyTime = GetSysSecondTimeAPI();
		}
	}

    /*EEPROM扇区0(0~497)*/
	if(addr < EEP_SEC0_CRC_ADDR)
	{
		/*备份区域未请求备份*/
		if(0 == BitGet(sParaEepCopyReqFlag, 0))
		{
			/*首次记录需要备份的起始地址和结束地址*/
			sParaEepCopyReqInfo[0][0] = addr;
			sParaEepCopyReqInfo[0][1] = addr + len;
		}
		/*备份区域已请求备份*/
		else
		{
			/*在已请求地址上增加需要备份的地址*/
			if(sParaEepCopyReqInfo[0][0] > addr)
			{
				/*地址往前取小地址*/
				sParaEepCopyReqInfo[0][0] = addr;
			}

			/*在已请求备份结束地址上延长新的备份结束地址*/
			if(sParaEepCopyReqInfo[0][1] < (addr + len))
			{
				/*结束地址往后取大地址*/
				sParaEepCopyReqInfo[0][1] = addr + len;
			}
		}
		BitSet(sParaEepCopyReqFlag, 0);
	}
    /*EEPROM扇区1(500~997)*/
	else if(addr < EEP_SEC1_CRC_ADDR)
	{
		/*备份区域未请求备份*/
		if(0 == BitGet(sParaEepCopyReqFlag, 1))
		{
			/*首次记录需要备份的起始地址和长度*/
			sParaEepCopyReqInfo[1][0] = addr;
			sParaEepCopyReqInfo[1][1] = addr + len;
		}
		/*备份区域已请求备份*/
		else
		{
			/*在已请求地址上增加需要备份的地址*/
			if(sParaEepCopyReqInfo[1][0] > addr)
			{
				/*地址往前取小地址*/
				sParaEepCopyReqInfo[1][0] = addr;
			}

			/*在已请求备份结束地址上延长新的备份结束地址*/
			if(sParaEepCopyReqInfo[1][1] < (addr + len))
			{
				/*结束地址往后取大地址*/
				sParaEepCopyReqInfo[1][1] = addr + len;
			}
		}
		BitSet(sParaEepCopyReqFlag, 1);
	}
    /*EEPROM扇区2(1000~1497)*/
	else if(addr < EEP_SEC2_CRC_ADDR)
	{
		/*备份区域未请求备份*/
		if(0 == BitGet(sParaEepCopyReqFlag, 2))
		{
			/*首次记录需要备份的起始地址和长度*/
			sParaEepCopyReqInfo[2][0] = addr;
			sParaEepCopyReqInfo[2][1] = addr + len;
		}
		/*备份区域已请求备份*/
		else
		{
			/*在已请求地址上增加需要备份的地址*/
			if(sParaEepCopyReqInfo[2][0] > addr)
			{
				/*地址往前取小地址*/
				sParaEepCopyReqInfo[2][0] = addr;
			}

			/*在已请求备份结束地址上延长新的备份结束地址*/
			if(sParaEepCopyReqInfo[2][1] < (addr + len))
			{
				/*结束地址往后取大地址*/
				sParaEepCopyReqInfo[2][1] = addr + len;
			}
		}
		BitSet(sParaEepCopyReqFlag, 2);
	}
    /*EEPROM其它地址*/
	else
	{
		return;
	}
}

/*=================================================================================================
** @Name      : void ParaSetFlashCopyReqFlag(u16 addr, u8 flag)
** @Input     : addr:需备份的EEPROM地址  flag:立即备份标志(0延时备份 1立即备份)
** @Output    : void
** @Function  : 设置FLASH参数备份请求标志
** @The notes : 根据地址设置需要备份的扇区和记录备份时间
**===============================================================================================*/
void ParaSetFlashCopyReqFlag(u16 addr, u8 flag)
{
	/*立即备份*/
	if((1 == flag) || (BSPGetOSMillSecTimeAPI() < 5000))
	{
		sParaFlashCopyTime = 0;
	}
	/*延时备份*/
	else
	{
		/*没有备份请求才更新请求时间*/
		if((0 == sParaFlashCopyTime)
			|| (0 == sParaFlashCopyReqFlag))
		{
			sParaFlashCopyTime = GetSysSecondTimeAPI();
		}
	}

    /*EEPROM扇区0(0~497)*/
	if(addr < EEP_SEC0_CRC_ADDR)
	{
		BitSet(sParaFlashCopyReqFlag, 0);
	}
    /*EEPROM扇区1(500~997)*/
	else if(addr < EEP_SEC1_CRC_ADDR)
	{
		BitSet(sParaFlashCopyReqFlag, 1);
	}
    /*EEPROM扇区2(1000~1497)*/
	else if(addr < EEP_SEC2_CRC_ADDR)
	{
		BitSet(sParaFlashCopyReqFlag, 2);
	}
    /*EEPROM其它地址*/
	else
	{
		return;
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void ParaCheckSecCrcAndCopyReq(u8 secNum, u8 flag)
** @Input     : secNum:扇区号[0, 2] flag:立即备份标志(0延时备份 1立即备份)
** @Output    : 16位CRC校验码
** @Function  : 检测外部EEPROM各扇区检验码和设置扇区备份请求
** @The notes :
**===============================================================================================*/
static void ParaCheckSecCrcAndCopyReq(u8 secNum, u8 flag)
{
    u16 calcCrc = 0;
    u16 readCrc = 0;
    u16 eepAddr = 0;
    u16 copAddr = 0;
    u32 flaAddr = 0;

    /*扇区0*/
    if(0 == secNum)
    {
    	eepAddr = EEP_SEC0_CRC_ADDR;
    	flaAddr = FLA_SEC0_CRC_ADDR;
        copAddr = EEP_SEC0_START_ADDR;
    }
    /*扇区1*/
    else if(1 == secNum)
    {
    	eepAddr = EEP_SEC1_CRC_ADDR;
    	flaAddr = FLA_SEC1_CRC_ADDR;
        copAddr = EEP_SEC1_START_ADDR;
    }
    /*扇区2*/
    else if(2 == secNum)
    {
    	eepAddr = EEP_SEC2_CRC_ADDR;
    	flaAddr = FLA_SEC2_CRC_ADDR;
        copAddr = EEP_SEC2_START_ADDR;
    }
    /*无效扇区*/
    else
    {
    	return;
    }

	//计算内部EEP扇区参数校验码
	calcCrc = CalcIntEepSecDataCrc(copAddr);

	//读取内部EEPROM当前校验码
	(void)BSPINTEEPReadWord(eepAddr, &readCrc, 1);

	//内部EEPRON扇区0校验码改变
	if(readCrc != calcCrc)
	{
		//重新写内部EEPROM校验码
		(void)BSPINTEEPWriteWord(eepAddr, &calcCrc, 1);
	}

	/*外部EEPROM读写正常,才将参数备份到外部EEPROM*/
	if(TRUE == ParaCheckExtEepReadyHook())
	{
		//读取外部EEPROM扇区当前校验码
		(void)DEVEXTEEPReadWord(eepAddr, &readCrc, 1);

		//外部EEPROM记录校验码与内部EEPRON扇区校验码不一致
		if(readCrc != calcCrc)
		{
			//重新计算外部EEP扇区参数校验码
			readCrc = CalcExtEepSecDataCrc(copAddr);

			/*计算外部EEPROM校验码与内部不一致*/
			if(readCrc != calcCrc)
			{
				//内外EEPROM参数未同步则请求外部EEPROM扇区参数备份
				ParaSetEepCopyReqFlag(copAddr, PARA_SEC_PARA_LEN, flag);
			}
			/*计算内部EEPROM校验码与外部一致(写参数时已同时写内外EEPROM)*/
			else
			{
				//重写外部EEPROM校验码即可
				(void)DEVEXTEEPWriteWord(eepAddr, &calcCrc, 1);
			}
		}
	}

	//读取内部FLASH扇区校验码当前校验码
	(void)BSPINTFLASHReadWord(flaAddr, &readCrc, 1);

	//内部FLASH扇区校验码与内部EEPRON扇区校验码不一致
	if(readCrc != calcCrc)
	{
		//请求内部FLASH扇区参数备份
		ParaSetFlashCopyReqFlag(copAddr, flag);
	}
}

/*=================================================================================================
** @Name      : static void ParaCopyIntEepDataToExtEepDeal(void)
** @Input     : void
** @Output    : void
** @Function  : 将内部EEP参数备份到外部处理
** @The notes : 分次备份扇区数据,每次104byte,遇到跨页分两次拷贝
**===============================================================================================*/
static void ParaCopyIntEepDataToExtEepDeal(void)
{
    u16 len = 0;
    u16 eepCrc = 0;
    u16 extCrc = 0;
    u16 calcCrc = 0;
    u16 eepAddr = 0;   //备份扇区起始地址
    u16 finAddr = 0;   //备份扇区结束地址
    u16 fstAddr = 0;   //需要备份起始地址
    u16 endAddr = 0;   //需要备份结束地址
    u8 data[PARA_COPY_PARA_LEN] = {0};
    static u16 sFinLen = 0;

    /*扇区0(地址0-497)*/
    if(1 == BitGet(sParaEepCopySec, 0))
    {
    	eepAddr = EEP_SEC0_START_ADDR;
        finAddr = EEP_SEC0_START_ADDR + PARA_SEC_PARA_LEN;
        fstAddr = sParaEepCopySecInfo[0][0];
        endAddr = sParaEepCopySecInfo[0][1];
    }
    /*扇区1(地址500-997)*/
    else if(1 == BitGet(sParaEepCopySec, 1))
    {
    	eepAddr = EEP_SEC1_START_ADDR;
        finAddr = EEP_SEC1_START_ADDR + PARA_SEC_PARA_LEN;
        fstAddr = sParaEepCopySecInfo[1][0];
        endAddr = sParaEepCopySecInfo[1][1];
    }
    /*扇区2(地址1000-1497)*/
    else if(1 == BitGet(sParaEepCopySec, 2))
    {
    	eepAddr = EEP_SEC2_START_ADDR;
        finAddr = EEP_SEC2_START_ADDR + PARA_SEC_PARA_LEN;
        fstAddr = sParaEepCopySecInfo[2][0];
        endAddr = sParaEepCopySecInfo[2][1];
    }
    /*其他地址非扇区起始地址无效*/
    else
    {
    	sParaEepCopySec = 0;
        sParaCopyEepStep = 0xff;
    }

    switch(sParaCopyEepStep)
    {
        /*检测是否需要备份*/
        case 0:
        	/*计算内部EEPROM校验码和已写入校验码*/
            calcCrc = CalcIntEepSecDataCrc(eepAddr);
    	    (void)BSPINTEEPReadWord(finAddr, &eepCrc, 1);

        	/*外部EEPROM读写正常,从内部EEPROM备份到外部EEPROM*/
        	if(TRUE == ParaCheckExtEepReadyHook())
            {
                /*读取外部EEPROM中校验码*/
        	    (void)DEVEXTEEPReadWord(finAddr, &extCrc, 1);
            }
        	/*外部EEPROM读写异常,只更新内部EEP校验码不进行备份*/
        	else
            {
                /*判断内部EEP参数校验码无效*/
                if(calcCrc != eepCrc)
                {
                	/*重新写内部EEPROM校验码*/
            	    (void)BSPINTEEPWriteWord(finAddr, &calcCrc, 1);
                }
                eepCrc = calcCrc;
                extCrc = calcCrc;
            }

            /*判断内部EEP参数校验码无效或外部EEP校验码与内部EEPROM校验码不一致*/
            if((calcCrc != eepCrc) || (extCrc != eepCrc))
            {
                /*外部EEPROM开始备份*/
                sParaCopyEepStep = 1;
            }
            else
            {
                //扇区0备份
                if(eepAddr < EEP_SEC0_CRC_ADDR)
                {
                    //清除扇区备份标志
                    BitClr(sParaEepCopySec, 0);
                }
                //扇区1备份
                else if(eepAddr < EEP_SEC1_CRC_ADDR)
                {
                    //清除扇区备份标志
                    BitClr(sParaEepCopySec, 1);
                }
                //扇区2备份
                else
                {
                    //清除扇区备份标志
                    BitClr(sParaEepCopySec, 2);
                }

                //无需要备份区域
                if(0 == sParaEepCopySec)
                {
                    //备份步骤清零
                	sParaCopyEepStep = 0xff;
                }
            }

            sFinLen = 0;
            break;

        //读取内部EEPROM中104byte写到外部EEPROM
        case 1:
            //计算本次EEP起始地址
            //eepAddr = eepAddr + sFinLen;					//整个扇区备份
            eepAddr = fstAddr + sFinLen;					//只备份参数变化区域

            //最后一次写
            //if((eepAddr + PARA_COPY_PARA_LEN) >= finAddr)	//整个扇区备份
            if((eepAddr + PARA_COPY_PARA_LEN) >= endAddr)	//只备份参数变化区域
            {
                //不能写校验码地址
                //len = finAddr - eepAddr;					//整个扇区备份
                len = endAddr - eepAddr;					//只备份参数变化区域
            }
            //非最后次写
            else
            {
                //写104个地址
                len = PARA_COPY_PARA_LEN;
            }

            //整页跨页写(外部EEPROM不支持跨页写)
            if(len > PARA_COPY_EXTEEP_PAG_SIZE)
            {
            	//只能写1页
            	len = PARA_COPY_EXTEEP_PAG_SIZE;
            }

			//页内跨页写(外部EEPROM不支持跨页写)
			if((eepAddr % PARA_COPY_EXTEEP_PAG_SIZE) >= ((eepAddr + len) % PARA_COPY_EXTEEP_PAG_SIZE))
			{
            	//只能写本页剩余地址
            	len = PARA_COPY_EXTEEP_PAG_SIZE - (eepAddr % PARA_COPY_EXTEEP_PAG_SIZE);
			}

            /*外部EEPROM读写正常,从外部EEPROM备份到内部EEPROM*/
            if(TRUE == ParaCheckExtEepReadyHook())
            {
                /*读内部EEP中104byte参数值*/
                (void)BSPINTEEPReadByte(eepAddr, data, len);

                /*重写外部EEPROM参数*/
                (void)DEVEXTEEPWriteByte(eepAddr, data, len);
            }
        	/*外部EEPROM读写异常,认为拷贝完成*/
            else
            {
                /*直接设置拷贝完*/
            	eepAddr = finAddr;
            }

            /*写最后一次*/
            //if((eepAddr + len) >= finAddr)				//整个扇区备份
            if((eepAddr + len) >= endAddr)					//只备份参数变化区域
            {
                //备份完成*/
            	sFinLen = 0;
                sParaCopyEepStep = 2;
            }
            /*非最后一次写*/
            else
            {
                //记录已备份长度*/
            	sFinLen += len;
            }
            break;

        //计算和记录EEP参数校验码
        case 2:
        	/*计算内部EEPROM校验码和重写校验码*/
            calcCrc = CalcIntEepSecDataCrc(eepAddr);
    	    (void)BSPINTEEPWriteWord(finAddr, &calcCrc, 1);

        	/*外部EEPROM读写正常*/
        	if(TRUE == ParaCheckExtEepReadyHook())
            {
            	/*计算外部EEPROM校验码和重写校验码*/
                calcCrc = CalcExtEepSecDataCrc(eepAddr);
        	    (void)DEVEXTEEPWriteWord(finAddr, &calcCrc, 1);
            }

            //扇区0备份
            if(eepAddr < EEP_SEC0_CRC_ADDR)
            {
                //清除扇区备份标志
                BitClr(sParaEepCopySec, 0);
            }
            //扇区1备份
            else if(eepAddr < EEP_SEC1_CRC_ADDR)
            {
                //清除扇区备份标志
                BitClr(sParaEepCopySec, 1);
            }
            //扇区2备份
            else
            {
                //清除扇区备份标志
                BitClr(sParaEepCopySec, 2);
            }

            //仍有扇区需要备份
            if(sParaEepCopySec > 0)
            {
                //返回继续备份
            	sParaCopyEepStep = 0;
            }
            //无扇区需要备份
            else
            {
                //清除备份步骤
            	sParaCopyEepStep = 0xff;
            }
            break;

        default:
            break;
    }
}

/*=================================================================================================
** @Name      : static void ParaCopyEepDataToIntFlashDeal(void)
** @Input     : void
** @Output    : void
** @Function  : 将EEP参数备份到Flash处理
** @The notes : 分次备份扇区数据,每次104byte,FLASH要求写入长度必须是8的倍数
**===============================================================================================*/
static void ParaCopyEepDataToIntFlashDeal(void)
{
    u16 len = 0;
    u16 eepCrc = 0;
    u16 flashCrc = 0;
    u16 calcCrc = 0;
    u16 finAddr = 0;
    u16 eepAddr = 0;
    u32 flashAddr = 0;
    u32 fCrcAddr = 0;
    u8 data[100] = {0};
    static u8 sTime = 0;

    /*扇区0(地址0-497)*/
    if(1 == BitGet(sParaFlashCopySec, 0))
    {
        eepAddr = EEP_SEC0_START_ADDR;
        finAddr = EEP_SEC0_START_ADDR + PARA_SEC_PARA_LEN;
        flashAddr = FLA_SEC0_START_ADDR;
        fCrcAddr = FLA_SEC0_CRC_ADDR;
    }
    /*扇区1(地址500-997)*/
    else if(1 == BitGet(sParaFlashCopySec, 1))
    {
        eepAddr = EEP_SEC1_START_ADDR;
        finAddr = EEP_SEC1_START_ADDR + PARA_SEC_PARA_LEN;
        flashAddr = FLA_SEC1_START_ADDR;
        fCrcAddr = FLA_SEC1_CRC_ADDR;
    }
    /*扇区2(地址1000-1497)*/
    else if(1 == BitGet(sParaFlashCopySec, 2))
    {
        eepAddr = EEP_SEC2_START_ADDR;
        finAddr = EEP_SEC2_START_ADDR + PARA_SEC_PARA_LEN;
        flashAddr = FLA_SEC2_START_ADDR;
        fCrcAddr = FLA_SEC2_CRC_ADDR;
    }
    /*其他地址非扇区起始地址无效*/
    else
    {
    	sParaFlashCopySec = 0;
        sParaCopyFlashStep = 0xff;
    }

    switch(sParaCopyFlashStep)
    {
        /*检测是否需要备份*/
        case 0:
			/*计算内部EEPROM校验码和已写入校验码*/
			calcCrc = CalcIntEepSecDataCrc(eepAddr);
			(void)BSPINTEEPReadWord(finAddr, &eepCrc, 1);

            /*读取Flash中校验码*/
            (void)BSPINTFLASHReadWord(fCrcAddr, &flashCrc, 1);

            /*判断EEP参数校验码无效或EEP校验码与Flash校验码不一致*/
            if((calcCrc != eepCrc) || (flashCrc != eepCrc))
            {
                /*擦除对应扇区开始备份*/
                (void)BSPINTFLASHEraseSector(flashAddr);
                sParaCopyFlashStep = 1;
            }
            else
            {
                //扇区0备份
                if(flashAddr < FLA_SEC0_CRC_ADDR)
                {
                    //清除扇区备份标志
                    BitClr(sParaFlashCopySec, 0);
                }
                //扇区1备份
                else if(flashAddr < FLA_SEC1_CRC_ADDR)
                {
                    //清除扇区备份标志
                    BitClr(sParaFlashCopySec, 1);
                }
                //扇区2备份
                else
                {
                    //清除扇区备份标志
                    BitClr(sParaFlashCopySec, 2);
                }

                //无需要备份区域
                if(0 == sParaFlashCopySec)
                {
                    //备份步骤清零
                	sParaCopyFlashStep = 0xff;
                }
            }

            sTime = 0;
            break;

        //读取EEPROM中104byte写到Flash
        case 1:
            //计算本次EEP起始地址
            eepAddr = eepAddr + sTime * PARA_COPY_PARA_LEN;

            //计算本次Flash起始地址
            flashAddr = flashAddr + sTime * PARA_COPY_PARA_LEN;

            //最后一次写
            if((eepAddr + PARA_COPY_PARA_LEN) >= finAddr)
            {
                //不能写校验码地址
                len = finAddr - eepAddr;
            }
            //非最后次写
            else
            {
                //写104个地址
                len = PARA_COPY_PARA_LEN;
            }

			/*读内部EEP中104byte参数值*/
			(void)BSPINTEEPReadByte(eepAddr, data, len);

            /*参数重新写入Flash*/
            (void)BSPINTFLASHWriteByte(flashAddr, data, len);

            /*写最后一次*/
            if((eepAddr + len) >= finAddr)
            {
                //备份完成*/
                sTime = 0;
                sParaCopyFlashStep = 2;
            }
            /*非最后一次写*/
            else
            {
                //记录备份次数*/
                sTime++;
            }
            break;

        //计算和记录EEP参数校验码
        case 2:
			/*计算内部EEPROM校验码和写入校验码*/
			calcCrc = CalcIntEepSecDataCrc(eepAddr);
			(void)BSPINTEEPWriteWord(finAddr, &calcCrc, 1);
            sParaCopyFlashStep = 3;
            break;

        //计算和记录Flash参数校验码
        case 3:
            calcCrc = CalcIntFlashSecDataCrc(flashAddr);
            data[0] = calcCrc & 0xff;
            data[1] = (calcCrc >> 8) & 0xff;

            //将CRC写Flash(FLASH要求最少写8Byte)
            (void)BSPINTFLASHWriteByte(fCrcAddr, data, 8);

            //扇区0备份
            if(flashAddr < FLA_SEC0_CRC_ADDR)
            {
                //清除扇区备份标志
                BitClr(sParaFlashCopySec, 0);
            }
            //扇区1备份
            else if(flashAddr < FLA_SEC1_CRC_ADDR)
            {
                //清除扇区备份标志
                BitClr(sParaFlashCopySec, 1);
            }
            //扇区2备份
            else
            {
                //清除扇区备份标志
                BitClr(sParaFlashCopySec, 2);
            }

            //仍有扇区需要备份
            if(sParaFlashCopySec > 0)
            {
                //返回继续备份
            	sParaCopyFlashStep = 0;
            }
            //无扇区需要备份
            else
            {
                //清除备份步骤
            	sParaCopyFlashStep = 0xff;
            }
            break;

        default:
            break;
    }
}

/*=================================================================================================
** @Name      : static u16 CalcExtEepSecDataCrc(u16 secAddr)
** @Input     : secAddr:指定参数扇区EEP起始地址
** @Output    : 扇区CRC校验码
** @Function  : 计算外部EEP指定地址块参数CRC校验码
** @The notes : 一个扇区数据(必须是指定起始地址才有效)
**===============================================================================================*/
static u16 CalcExtEepSecDataCrc(u16 secAddr)
{
    u16 num = 0;
    u16 len = 0;
    u8 uchCRCHi = 0xff;     //CRC高字节初始化
    u8 uchCRCLo = 0xff;     //CRC低字节初始化
    u16 uIndex = 0;         //查询表索引
    u16 crcTemp = 0;
    u8 data[PARA_SEC_PARA_LEN] = {0};

    //EEPROM扇区0或1或2起始地址(498个参数)
    if((EEP_SEC0_START_ADDR == secAddr)
      || (EEP_SEC1_START_ADDR == secAddr)
      || (EEP_SEC2_START_ADDR == secAddr))
    {
        len = PARA_SEC_PARA_LEN;
    }
    //非扇区0/1/2起始地址无效
    else
    {
        return(0);
    }

    //读取参数
    (void)DEVEXTEEPReadByte(secAddr, data, len);

    //计算校验码
    while(len)
    {
        uIndex = (u16)(uchCRCHi ^ data[num]);
        uchCRCHi = (u8)(uchCRCLo ^ cEepCRCHiExp[uIndex]);
        uchCRCLo = cEepCRCLoExp[uIndex];
        len--;
        num++;
    }

    crcTemp = ((u16)uchCRCHi << 8) | uchCRCLo;

    return(crcTemp);
}

/*=================================================================================================
** @Name      : static u16 CalcIntEepSecDataCrc(u16 secAddr)
** @Input     : secAddr:指定参数扇区EEP起始地址
** @Output    : 扇区CRC校验码
** @Function  : 计算内部EEP指定地址块参数CRC校验码
** @The notes : 一个扇区数据(必须是指定起始地址才有效)
**===============================================================================================*/
static u16 CalcIntEepSecDataCrc(u16 secAddr)
{
    u16 num = 0;
    u16 len = 0;
    u8 uchCRCHi = 0xff;     //CRC高字节初始化
    u8 uchCRCLo = 0xff;     //CRC低字节初始化
    u16 uIndex = 0;         //查询表索引
    u16 crcTemp = 0;
    u8 data[PARA_SEC_PARA_LEN] = {0};

    //EEPROM扇区0或1或2起始地址(498个参数)
    if((EEP_SEC0_START_ADDR == secAddr)
      || (EEP_SEC1_START_ADDR == secAddr)
      || (EEP_SEC2_START_ADDR == secAddr))
    {
        len = PARA_SEC_PARA_LEN;
    }
    //非扇区0/1/2起始地址无效
    else
    {
        return(0);
    }

    //读取参数
    (void)BSPINTEEPReadByte(secAddr, data, len);

    //计算校验码
    while(len)
    {
        uIndex = (u16)(uchCRCHi ^ data[num]);
        uchCRCHi = (u8)(uchCRCLo ^ cEepCRCHiExp[uIndex]);
        uchCRCLo = cEepCRCLoExp[uIndex];
        len--;
        num++;
    }

    crcTemp = ((u16)uchCRCHi << 8) | uchCRCLo;

    return(crcTemp);
}

/*=================================================================================================
** @Name      : static u16 CalcIntFlashSecDataCrc(u32 secAddr)
** @Input     : secAddr:指定参数扇区Flash起始地址
** @Output    : 扇区CRC校验码
** @Function  : 计算内部Flash指定地址扇区参数CRC校验码
** @The notes : 一个扇区数据(必须是指定起始地址才有效)
**===============================================================================================*/
static u16 CalcIntFlashSecDataCrc(u32 secAddr)
{
    u16 num = 0;
    u16 len = 0;
    u8 uchCRCHi = 0xff;     //CRC高字节初始化
    u8 uchCRCLo = 0xff;     //CRC低字节初始化
    u16 uIndex = 0;         //查询表索引
    u16 crcTemp = 0;
    u8 data[PARA_SEC_PARA_LEN] = {0};

    //内部FLASH扇区0或1或2(498个参数)
    if((FLA_SEC0_START_ADDR == secAddr)
        || (FLA_SEC1_START_ADDR == secAddr)
        || (FLA_SEC2_START_ADDR == secAddr))
    {
        len = PARA_SEC_PARA_LEN;
    }
    //非扇区0/1/2起始地址无效
    else
    {
        return(0);
    }

    //读取参数
    (void)BSPINTFLASHReadByte(secAddr, data, len);

    //计算校验码
    while(len)
    {
        uIndex = (u16)(uchCRCHi ^ data[num]);
        uchCRCHi = (u8)(uchCRCLo ^ cEepCRCHiExp[uIndex]);
        uchCRCLo = cEepCRCLoExp[uIndex];
        len--;
        num++;
    }

    crcTemp = ((u16)uchCRCHi << 8) | uchCRCLo;

    return(crcTemp);
}

