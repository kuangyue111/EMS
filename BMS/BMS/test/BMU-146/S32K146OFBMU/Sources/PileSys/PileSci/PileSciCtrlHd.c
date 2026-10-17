/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileSciCtrlHd.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 串口控制堆控处理模块接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "ParaUser.h"
#include "ParaInterface.h"
#include "PublicInfo.h"
#include "PublicPara.h"
#include "PileInfo.h"
#include "PileUserAPI.h"
#include "PileTactUser.h"
#include "SciCtrlHandle.h"
#include "PileSciCtrlHd.h"

#include "GroupInfo.h"
#include "TacUser.h"
#include "Authority.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gPAuthorInfo_80[]; 								/*权限级别状态信息数组*/
extern u16 gPDebugResult_82[];								/*调试结果状态信息数组*/

extern u16 gPAuthorCtrl_85[];								/*权限密码登录信息数组*/
extern u16 gPSysCmdCtrl_86[];								/*系统控制命令信息数组*/
extern u16 gPDebugCtrl_88[];								/*系统调试命令信息数组*/

extern u16 gGAuthorInfo_80[]; 	                            /*权限级别状态信息数组*/
extern u16 gGDebugResult_82[]; 	                            /*调试结果状态信息数组*/

extern u16 gGAuthorCtrl_85[];			                    /*权限密码登录信息数组*/
extern u16 gGSysCmdCtrl_86[];			                    /*系统控制命令信息数组*/
extern u16 gGDebugCtrl_88[];			                    /*系统调试命令信息数组*/

extern u16 gSysGenParaRO_100[];								/*系统通用参数设置*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u16* SciGetPtrPileAuthPwdWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写堆控权限密码信息寄存器地址指针
** @Function  : 获取堆控读写权限密码信息寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileAuthPwdWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX85_LEN)
		{
			/*写功能码*/
			if((6 == fncCode) || (16 == fncCode))
			{
				/*写入*/
				gPAuthorCtrl_85[index] = data;

				/*执行密码处理*/
				PileSetSelfGroupOperAuthCmdAPI(eAuthP_Sci0 + sciNum, index, gPAuthorCtrl_85[index]);
			}

			return(&gPAuthorCtrl_85[index]);
		}
	}
	/*单组无堆控*/
	else
	{
		if(index < GINDEX85_LEN)
		{
			/*写功能码*/
			if((6 == fncCode) || (16 == fncCode))
			{
				/*写入*/
				gGAuthorCtrl_85[index] = data;

				/*写入最后一个密码*/
				if((GINDEX85_LEN - 1) == index)
				{
					/*执行密码检测*/
					RcvSysAuthorityCmdAPI(eAuthP_Sci0 + sciNum);
				}
			}

			return(&gGAuthorCtrl_85[index]);
		}
	}

	return(SciGetPtrDefaultValueWR());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileWStateCmdWR(u8 fncCode, u8 sciNum, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 data:写入数据(请求修改有效)
** @Output    : 读写工作状态切换命令寄存器地址指针
** @Function  : 获取读写工作状态切换命令寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileWStateCmdWR(u8 fncCode, u8 sciNum, u16 data)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*已获得串口控制权限*/
			if(eAuthL_User != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
			{
				gPSysCmdCtrl_86[eSysCmdCtrl86_State] = data;

				/*执行命令*/
				RcvPileWorkStateCmdAPI(eAuthP_Sci0 + sciNum);
			}
		}

		return(&gPSysCmdCtrl_86[eSysCmdCtrl86_State]);
	}
	/*单组无堆控*/
	else
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*已获得串口控制权限*/
			if(eAuthL_User != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
			{
				gGSysCmdCtrl_86[eSysCmdCtrl86_State] = data;

				/*执行命令*/
				RcvGroupWorkStateCmdAPI(eAuthP_Sci0 + sciNum);
			}
		}

		return(&gGSysCmdCtrl_86[eSysCmdCtrl86_State]);
	}
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileWModeCmdWR(u8 fncCode, u8 sciNum, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 data:写入数据(请求修改有效)
** @Output    : 读写工作模式切换命令寄存器地址指针
** @Function  : 获取读写工作模式切换命令寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileWModeCmdWR(u8 fncCode, u8 sciNum, u16 data)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*已获得串口控制权限*/
			if(eAuthL_User != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
			{
				gPSysCmdCtrl_86[eSysCmdCtrl86_Mode] = data;

				/*执行命令*/
				RcvPileWorkModeCmdAPI(eAuthP_Sci0 + sciNum);
			}
		}

		return(&gPSysCmdCtrl_86[eSysCmdCtrl86_Mode]);
	}
	/*单组无堆控*/
	else
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*已获得串口控制权限*/
			if(eAuthL_User != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
			{
				gGSysCmdCtrl_86[eSysCmdCtrl86_Mode] = data;

				/*执行命令*/
				RcvGroupWorkModeCmdAPI(eAuthP_Sci0 + sciNum);
			}
		}

		return(&gGSysCmdCtrl_86[eSysCmdCtrl86_Mode]);
	}
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileSysClrCmdWR(u8 fncCode, u8 sciNum, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 data:写入数据(请求修改有效)
** @Output    : 读写系统清除命令寄存器地址指针
** @Function  : 获取读写系统清除命令寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileSysClrCmdWR(u8 fncCode, u8 sciNum, u16 data)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*已获得串口控制权限*/
			if(eAuthL_User != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
			{
				gPSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd] = data;

				/*执行命令*/
				RcvPileSysClrCmdAPI(eAuthP_Sci0 + sciNum);
			}
		}

		return(&gPSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd]);
	}
	/*单组无堆控*/
	else
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*已获得串口控制权限*/
			if(eAuthL_User != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
			{
				gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd] = data;

				/*执行命令*/
				RcvGroupSysClrCmdAPI(eAuthP_Sci0 + sciNum);
			}
		}

		return(&gGSysCmdCtrl_86[eSysCmdCtrl86_ClrCmd]);
	}
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileDebugCmdWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写系统调试命令寄存器地址指针
** @Function  : 获取读写系统调试命令寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileDebugCmdWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX88_LEN)
		{
			/*写功能码*/
			if((6 == fncCode) || (16 == fncCode))
			{
				/*已获得串口超级权限*/
				if(eAuthL_Admin == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
				{
					/*写入*/
					gPDebugCtrl_88[index] = data;

					//调试命令设置
					if(index < eDbgCmd88_Affirm)
					{
						RcvPileDebugOperCmdAPI(index);
					}
					/*写入最后调试确认*/
					else if(eDbgCmd88_Affirm == index)
					{
						/*执行调试*/
						RcvPileDebugAffirmCmdAPI(eAuthP_Sci0 + sciNum);
					}
				}
			}

			return(&gPDebugCtrl_88[index]);
		}
	}
	/*单组无堆控*/
	else
	{
		if(index < GINDEX88_LEN)
		{
			/*写功能码*/
			if((6 == fncCode) || (16 == fncCode))
			{
				/*已获得串口超级权限*/
				if(eAuthL_Admin == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
				{
					/*写入*/
					gGDebugCtrl_88[index] = data;

					//调试命令设置
					if(index < eDbgCmd88_Affirm)
					{
						RcvGroupDebugOperCmdAPI(index);
					}
					/*写入最后调试确认*/
					else if(eDbgCmd88_Affirm == index)
					{
						/*执行调试*/
						RcvGroupDebugAffirmCmdAPI(eAuthP_Sci0 + sciNum);
					}
				}
			}

			return(&gGDebugCtrl_88[index]);
		}
	}

	return(SciGetPtrDefaultValueWR());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileAuthPwdRO(u8 index)
** @Input     : index:索引号
** @Output    : 读权限信息寄存器地址指针
** @Function  : 获取读项目参数信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileAuthPwdRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX80_LEN)
		{
			/*更新当前权限*/
			gPAuthorInfo_80[index] = GetSysAuthorityInfoAPI(index);

			/*读取当前权限*/
			return(&gPAuthorInfo_80[index]);
		}
	}
	/*单组无堆控*/
	else
	{
		if(index < GINDEX80_LEN)
		{
			/*读取当前权限*/
			return(&gGAuthorInfo_80[index]);
		}
	}

	return(SciGetPtrDefaultValueWR());
}

/*=================================================================================================
** @Name      : u16* SciGetPtrPileDebugResultRO(u8 index)
** @Input     : index:索引号
** @Output    : 读系统调试结果信息寄存器地址指针
** @Function  : 获取读系统调试结果信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrPileDebugResultRO(u8 index)
{
	/*多组并机堆控有效*/
	if(gSysGenParaRO_100[eSGenPara100_AllGNum] > 1)
	{
		if(index < PINDEX82_LEN)
		{
			/*读取当前权限*/
			return(&gPDebugResult_82[index]);
		}
	}
	/*单组无堆控*/
	else
	{
		if(index < GINDEX82_LEN)
		{
			/*读取当前权限*/
			return(&gGDebugResult_82[index]);
		}
	}

	return(SciGetPtrDefaultValueWR());
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
