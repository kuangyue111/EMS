/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : ParaDnload.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 参数存储模块参数批量下载
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "PublicPara.h"
#include "ParaInterface.h"
#include "ParaCopy.h"
#include "ParaDnload.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 *sParaDnloadState;						        /*参数批量下载状态 [0]:参数配置状态(0:未操作 1:正在读 2:读成功 3:读失败 4:等待写 5:写成功 6:写失败 7:正在备份 8:备份成功 9:备份失败) [1]:参数配置异常信息(0:无异常 其它:异常码)*/
static u8 *sParaDnloadRdInfo;						        /*参数配置读取信息 [0]:读参数起始EEPROM地址L [1]:读参数起始EEPROM地址H [2]:读参数总字节数 [3]…[n]:读到的参数值 [n+1]:读取参数校验码L [n+2]:读取参数校验码H*/
static u8 *sParaDnloadWtInfo;						        /*参数配置写入信息 [0]:读写命令(0:退出 1:读参数 2:写参数 3:备份参数) [1]:读写参数起始EEPROM地址L [2]:读写参数起始EEPROM地址H [3]:读写参数总字节数 [4]…[n]:写入的参数值 [n+1]:写入参数校验码L [n+2]:写入参数校验码H*/

extern u16 gSysGenParaRO_100[];  							/*系统通用设置*/
extern u16 gSysHigParaRO_101[];  							/*系统硬件设置*/
extern u16 gGBmuGenPara_102[];  							/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];							/*主控高级参数值*/
extern u8 gGHardPara_104[];  								/*主控硬件参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void ParaUpdateReadParaInfo(u16 addr, u8 byteNum)
** @Input     : addr:读起始地址 byteNum:读字节数
** @Output    : TRUE:读成功 FALSE:读失败
** @Function  : 参数批量下载读参数并更新到显示区
** @The notes :
**===============================================================================================*/
static u8 ParaUpdateReadParaInfo(u16 addr, u8 byteNum);

/*=================================================================================================
** @Name      : static void ParaUpdateWriteParaInfo(u16 addr, u8 byteNum)
** @Input     : addr:写起始地址 byteNum:写字节数
** @Output    : TRUE:写成功 FALSE:写失败
** @Function  : 参数批量下载写参数到存储区
** @The notes :
**===============================================================================================*/
static u8 ParaUpdateWriteParaInfo(u16 addr, u8 byteNum);

/*=================================================================================================
** @Name      : static u16 ParaCreateCRC(u8 *data, u8 len)
** @Input     : data:需计算CRC的数据 len:数据长度
** @Output    : 16位CRC校验码
** @Function  : 计算16位CRC校验码
** @The notes :
**===============================================================================================*/
static u16 ParaCreateCRC(u8 *data, u8 len);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void ParaDownloadInit(void
** @Input     : void
** @Output    : void
** @Function  : 参数批量下载功能初始化
** @The notes :
**===============================================================================================*/
void ParaDownloadInit(void)
{
	sParaDnloadState = ParaGetDnloadStateInfoPtrHook();
	sParaDnloadRdInfo = ParaGetDnloadReadInfoPtrHook();
	sParaDnloadWtInfo = ParaGetDnloadWriteInfoPtrHook();
}

/*=================================================================================================
** @Name      : void ParaChangDownloadState(void)
** @Input     : void
** @Output    : void
** @Function  : 切换参数批量下载状态
** @The notes : 接收到配置状态切换命令后调用
**===============================================================================================*/
void ParaChangDownloadState(void)
{
	/*模块未初始化*/
	if((NULL == sParaDnloadState) || (NULL == sParaDnloadWtInfo))
	{
		return;
	}

	/*设置状态有效*/
	if(sParaDnloadWtInfo[eParaDnWt_RdOrWt] < eParaDnCmd_Fin)
	{
		/*退出*/
		if(eParaDnCmd_Out == sParaDnloadWtInfo[eParaDnWt_RdOrWt])
		{
			/*无操作*/
			sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_None;
		}
//		/*读参数*/
//		else if(eParaDnCmd_Rd == sParaDnloadWtInfo[eParaDnWt_RdOrWt])
//		{
//			/*正在读*/
//			sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_Rding;
//		}
//		/*写参数*/
//		else if(eParaDnCmd_Wr == sParaDnloadWtInfo[eParaDnWt_RdOrWt])
//		{
//			/*等待写*/
//			sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_Wring;
//		}
//		/*备份参数*/
//		else if(eParaDnCmd_Wr == sParaDnloadWtInfo[eParaDnWt_RdOrWt])
//		{
//			/*正在备份*/
//			sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_Cping;
//		}
		/*读写或备份命令*/
		else
		{
			/*等待读写或备份(等待下发地址或参数)*/
			sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_Wait;
		}

		sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_Normal;
	}
	else
	{
		sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_None;
		sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_CmdExp;
	}
}

/*=================================================================================================
** @Name      : void ParaDownloadWriteParaHandle(void)
** @Input     : void
** @Output    : void
** @Function  : 参数批量下载读写参数处理
** @The notes : 接收完读写命令和写入参数信息后调用
**===============================================================================================*/
void ParaDownloadWriteParaHandle(void)
{
	u8 len = 0;
	u16 addr = 0;
	u8 result = 0;

	/*模块未初始化*/
	if((NULL == sParaDnloadState) || (NULL == sParaDnloadRdInfo) || (NULL == sParaDnloadWtInfo))
	{
		return;
	}

	/*读写控制命令处理*/
	ParaChangDownloadState();

	/*未处于等待读写或备份参数状态*/
//	if((eParaDnState_Rding != sParaDnloadState[eParaDnSta_CfgState])
//		&& (eParaDnState_Wring != sParaDnloadState[eParaDnSta_CfgState])
//		&& (eParaDnState_Rding != sParaDnloadState[eParaDnSta_CfgState]))
	if(eParaDnState_Wait != sParaDnloadState[eParaDnSta_CfgState])
	{
		sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_NotWWait;
		return;
	}

	/*读参数*/
	if(eParaDnCmd_Rd == sParaDnloadWtInfo[eParaDnWt_RdOrWt])
	{
		/*正在读*/
		sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_Rding;
	}
	/*写参数*/
	else if(eParaDnCmd_Wr == sParaDnloadWtInfo[eParaDnWt_RdOrWt])
	{
		/*等待写*/
		sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_Wring;
	}
	/*备份参数*/
	else if(eParaDnCmd_Wr == sParaDnloadWtInfo[eParaDnWt_RdOrWt])
	{
		/*正在备份*/
		sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_Cping;
	}

	/*获取写参数地址和长度*/
	addr = ((u16)sParaDnloadWtInfo[eParaDnWt_AddrH] << 8) | sParaDnloadWtInfo[eParaDnWt_AddrL];
	len = sParaDnloadWtInfo[eParaDnWt_ByteNum];

	/*输入参数无效*/
	if(((addr + len)  > PARA_DNLOAD_MAX_ADDR)
		|| (len > PARA_DNLOAD_MAX_BYTE)
		|| (0 == len))
	{
		/*读写或备份失败*/
		sParaDnloadState[eParaDnSta_CfgState] += 2;
		sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_AddrLenErr;
		return;
	}

	/*读参数*/
	if(eParaDnState_Rding == sParaDnloadState[eParaDnSta_CfgState])
	{
		/*更新读参数信息*/
		result = ParaUpdateReadParaInfo(addr, len);

		if(TRUE == result)
		{
			/*读成功*/
			sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_RdOk;
			sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_Normal;
		}
		else
		{
			/*读失败*/
			sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_RdErr;
			sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_RWEepErr;
		}
	}
	/*写参数*/
	else if(eParaDnState_Wring == sParaDnloadState[eParaDnSta_CfgState])
	{
		/*更新写参数信息*/
		result = ParaUpdateWriteParaInfo(addr, len);

		if(TRUE == result)
		{
			/*写成功*/
			sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_WrOk;
			sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_Normal;
		}
		else
		{
			/*写失败*/
			sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_WrErr;
			if(0 == sParaDnloadState[eParaDnSta_ErrInfo])
			{
				sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_RWEepErr;
			}
		}
	}
	/*备份参数*/
	else if(eParaDnState_Cping == sParaDnloadState[eParaDnSta_CfgState])
	{
		/*设置EEPROM参数备份请求*/
		ParaSetEepCopyReqFlag(addr, PARA_SEC_PARA_LEN, 1);

		/*设置FLASH参数备份请求*/
		ParaSetFlashCopyReqFlag(addr, 1);

		/*备份成功*/
		sParaDnloadState[eParaDnSta_CfgState] = eParaDnState_CpOk;
		sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_Normal;
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void ParaUpdateReadParaInfo(u16 addr, u8 byteNum)
** @Input     : addr:读起始地址 byteNum:读字节数
** @Output    : TRUE:读成功 FALSE:读失败
** @Function  : 参数批量下载读参数并更新到显示区
** @The notes :
**===============================================================================================*/
static u8 ParaUpdateReadParaInfo(u16 addr, u8 byteNum)
{
	u16 crc = 0;
	u8 result = 0;

	/*模块未初始化*/
	if(NULL == sParaDnloadRdInfo)
	{
		return(FALSE);
	}

	/*更新读地址和长度*/
	sParaDnloadRdInfo[eParaDnRd_ByteNum] = byteNum;
	sParaDnloadRdInfo[eParaDnRd_AddrL] = addr & 0xff;
	sParaDnloadRdInfo[eParaDnRd_AddrH] = (addr >> 8) & 0xff;

	/*读内部EEPROM*/
	result = BSPINTEEPReadByte(addr, &sParaDnloadRdInfo[eParaDnRd_Data], byteNum);

	/*读内部EEPROM失败*/
	if(FALSE == result)
	{
		/*外部EEPROM读写异常*/
		if(FALSE == ParaCheckExtEepReadyHook())
		{
			result = FALSE;
		}
		else
		{
			/*读外部EEPROM*/
			result = DEVEXTEEPReadByte(addr, &sParaDnloadRdInfo[eParaDnRd_Data], byteNum);
		}
	}

	/*读内外EEPROM失败*/
	if(FALSE == result)
	{
		/*读内部FLASH*/
		result = BSPINTFLASHReadByte(addr, &sParaDnloadRdInfo[eParaDnRd_Data], byteNum);
	}

	/*读出参数成功*/
	if(TRUE == result)
	{
		/*计算校验码*/
		crc = ParaCreateCRC(&sParaDnloadRdInfo[eParaDnRd_Data], byteNum);
		sParaDnloadRdInfo[eParaDnRd_Data + byteNum] = crc & 0xff;
		sParaDnloadRdInfo[eParaDnRd_Data + byteNum + 1] = (crc >> 8) & 0xff;
	}

	return(result);
}

/*=================================================================================================
** @Name      : static void ParaUpdateWriteParaInfo(u16 addr, u8 byteNum)
** @Input     : addr:写起始地址 byteNum:写字节数
** @Output    : TRUE:写成功 FALSE:写失败
** @Function  : 参数批量下载写参数到存储区
** @The notes :
**===============================================================================================*/
static u8 ParaUpdateWriteParaInfo(u16 addr, u8 byteNum)
{
	u16 crc = 0;
	u16 result = 0;

	/*模块未初始化*/
	if(NULL == sParaDnloadRdInfo)
	{
		return(FALSE);
	}

	/*计算校验码*/
	crc = ParaCreateCRC(&sParaDnloadWtInfo[eParaDnWt_Data], byteNum);

	/*计算校验码与下发校验码匹配(参数传输正确)*/
	if(crc == (((u16)sParaDnloadWtInfo[eParaDnWt_Data + byteNum + 1] << 8) | sParaDnloadWtInfo[eParaDnWt_Data + byteNum]))
	{
		/*参数写入存储区用户处理*/
		result = ParaWriteParaUserHandleHook(addr, &sParaDnloadWtInfo[eParaDnWt_Data], byteNum);

		/*写入成功*/
		if(TRUE == result)
		{
			sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_Normal;
		}
		else
		{
			sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_RWEepErr;
		}

		return(result);
	}
	/*计算校验码与下发校验码不匹配(参数传输错误)*/
	else
	{
		sParaDnloadState[eParaDnSta_ErrInfo] = eParaDnErr_CommuErr;
	}

	return(FALSE);
}

/*=================================================================================================
** @Name      : static u16 ParaCreateCRC(u8 *data, u8 len)
** @Input     : data:需计算CRC的数据 len:数据长度
** @Output    : 16位CRC校验码
** @Function  : 计算16位CRC校验码
** @The notes :
**===============================================================================================*/
static u16 ParaCreateCRC(u8 *data, u8 len)
{
    u8 i = 0;
    u8 j = 0;
    u16 CRC16 = 0xFFFF;

    for(i = 0; i < len; i++)
    {
        CRC16 = CRC16 ^ data[i];

        for(j = 0; j < 8; j++)
        {
            if(CRC16 & 0x01)
            {
                CRC16 = CRC16 ^ 0xA001;
            }
            CRC16 = CRC16 >> 1;
        }
    }

    return(CRC16);
}
