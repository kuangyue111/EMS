/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : SciCtrlHandle.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 串口控制处理模块接口
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
#include "GroupInfo.h"
#include "Authority.h"
#include "CANUser.h"
#include "TacUser.h"
#include "SciCtrlHandle.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u16 sSciDefaultValueWR = 0;                          /*串口写信息默认值(可读写)*/

extern u16 gCANVerTime_0[];			                        /*校时信息*/
extern u8 gCANSerialNum_2[];			                    /*序列号信息*/

extern u16 gGAuthorInfo_80[]; 	                            /*权限级别状态信息数组*/
extern u16 gGDebugResult_82[]; 	                            /*调试结果状态信息数组*/

extern u16 gGAuthorCtrl_85[];			                    /*权限密码登录信息数组*/
extern u16 gGSysCmdCtrl_86[];			                    /*系统控制命令信息数组*/
extern u16 gGDebugCtrl_88[];			                    /*系统调试命令信息数组*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u16 *SciGetPtrDefaultValueRO(void)
** @Input     : void
** @Output    : 读写串口默认值寄存器地址指针
** @Function  : 获取读写串口默认值寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16 *SciGetPtrDefaultValueWR(void)
{
	/*写入*/
	return(&sSciDefaultValueWR);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrVerTimeInfoWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写校时信息寄存器地址指针
** @Function  : 获取读写校时信息寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrVerTimeInfoWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
{
	if(index < CANINDEX0_LEN)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*写入时间值*/
			gCANVerTime_0[index] = data;

			/*写入最后一个时间-秒*/
			if((CANINDEX0_LEN - 1) == index)
			{
				/*执行外部校时处理*/
				SysClockExtVerifyAPI();
			}
		}

		return(&gCANVerTime_0[index]);
	}

	return(&sSciDefaultValueWR);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrSerialNumInfoWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写设备序列号信息寄存器地址指针
** @Function  : 获取读写设备序列号信息寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrSerialNumInfoWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
{
	u8 sindex = 0;

	sindex = index * 2;

	if(sindex < CANINDEX2_LEN)
	{
		/*写功能码*/
		if((6 == fncCode) || (16 == fncCode))
		{
			/*已获得串口控制权限且记录存储成功*/
			if(eAuthL_Admin == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Sci0 + sciNum)))
			{
				/*显示和写入配置值(低8位)*/
				gCANSerialNum_2[sindex] = data & 0xff;
				(void)RcvDeviceSerialParaCfgAPI(sindex, eAuthP_Sci0 + sciNum);

				/*高8位有效*/
				if((sindex + 1) < CANINDEX2_LEN)
				{
					/*显示和写入配置值(高8位)*/
					gCANSerialNum_2[sindex + 1] = (data >> 8) & 0xff;
					(void)RcvDeviceSerialParaCfgAPI(sindex + 1, eAuthP_Sci0 + sciNum);
				}
			}

			return(&sSciDefaultValueWR);
		}
		/*读功能码*/
		else
		{
			sSciDefaultValueWR = ((u16)gCANSerialNum_2[index * 2 + 1] << 8) | gCANSerialNum_2[index * 2];
			return(&sSciDefaultValueWR);
		}
	}

	return(&sSciDefaultValueWR);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrAuthPasswordWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写权限密码信息寄存器地址指针
** @Function  : 获取读写权限密码信息寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrAuthPasswordWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
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

	return(&sSciDefaultValueWR);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrWorkStateCmdWR(u8 fncCode, u8 sciNum, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 data:写入数据(请求修改有效)
** @Output    : 读写工作状态切换命令寄存器地址指针
** @Function  : 获取读写工作状态切换命令寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrWorkStateCmdWR(u8 fncCode, u8 sciNum, u16 data)
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

/*=================================================================================================
** @Name      : u16* SciGetPtrWorkModeCmdWR(u8 fncCode, u8 sciNum, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 data:写入数据(请求修改有效)
** @Output    : 读写工作模式切换命令寄存器地址指针
** @Function  : 获取读写工作模式切换命令寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrWorkModeCmdWR(u8 fncCode, u8 sciNum, u16 data)
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

/*=================================================================================================
** @Name      : u16* SciGetPtrSysClrCmdWR(u8 fncCode, u8 sciNum, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 data:写入数据(请求修改有效)
** @Output    : 读写系统清除命令寄存器地址指针
** @Function  : 获取读写系统清除命令寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrSysClrCmdWR(u8 fncCode, u8 sciNum, u16 data)
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

/*=================================================================================================
** @Name      : u16* SciGetPtrSysDebugCmdWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
** @Input     : fncCode:功能码 sciNum:串口号 index:索引号 data:写入数据(请求修改有效)
** @Output    : 读写系统调试命令寄存器地址指针
** @Function  : 获取读写系统调试命令寄存器地址指针
** @The notes : 可读写,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrSysDebugCmdWR(u8 fncCode, u8 sciNum, u8 index, u16 data)
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
	else
	{
		sSciDefaultValueWR = 0;
	}

	return(&sSciDefaultValueWR);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrVerTimeInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读项目参数信息寄存器地址指针
** @Function  : 获取读项目参数信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrVerTimeInfoRO(u8 index)
{
	if(index < CANINDEX0_LEN)
	{
		/*更新当前时间到通信显示字段*/
		SysClockDisplayAPI();
		return(&gCANVerTime_0[index]);
	}
	else
	{
		sSciDefaultValueWR = 0;
	}

	return(&sSciDefaultValueWR);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrSerialNumInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读项设备序列号信息寄存器地址指针
** @Function  : 获取读设备序列号信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrSerialNumInfoRO(u8 index)
{
	u8 sindex = 0;

	sindex = index * 2;

	if(sindex < CANINDEX2_LEN)
	{
		/*高8位有效*/
		if((sindex + 1) < CANINDEX2_LEN)
		{
			sSciDefaultValueWR = ((u16)gCANSerialNum_2[sindex + 1] << 8) | gCANSerialNum_2[sindex];
		}
		/*只有低8位*/
		else
		{
			sSciDefaultValueWR = gCANSerialNum_2[sindex];
		}

		/*读取设备序列号*/
		return(&sSciDefaultValueWR);
	}
	else
	{
		sSciDefaultValueWR = 0;
	}

	return(&sSciDefaultValueWR);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrAuthPasswordRO(u8 index)
** @Input     : index:索引号
** @Output    : 读权限信息寄存器地址指针
** @Function  : 获取读项目参数信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrAuthPasswordRO(u8 index)
{
	if(index < GINDEX80_LEN)
	{
		/*读取当前权限*/
		return(&gGAuthorInfo_80[index]);
	}
	else
	{
		sSciDefaultValueWR = 0;
	}

	return(&sSciDefaultValueWR);
}

/*=================================================================================================
** @Name      : u16* SciGetPtrSysDebugResultRO(u8 index)
** @Input     : index:索引号
** @Output    : 读系统调试结果信息寄存器地址指针
** @Function  : 获取读系统调试结果信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetPtrSysDebugResultRO(u8 index)
{
	if(index < GINDEX82_LEN)
	{
		/*读取当前权限*/
		return(&gGDebugResult_82[index]);
	}
	else
	{
		sSciDefaultValueWR = 0;
	}

	return(&sSciDefaultValueWR);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

