/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : Authority.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 权限管理模块接口函数
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicPara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "EventUser.h"
#include "Authority.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
/*当前权限开发的级别(用户级别:0:普通用户,1:运维用户, 2:高级用户 3:超级用户)*/
/*当前权限开发的端口([0,4]CAN口 [5,9]串口 其它:网口 0xff所有, 无端口开放权限则为0)*/
extern u16 gGAuthorInfo_80[]; 	                            /*权限级别状态信息数组*/

extern u16 gGAuthorCtrl_85[];			                    /*权限密码登录信息数组*/
extern u16 gSysHigParaRO_101[];  		                    /*系统硬件参数设置*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void AuthorityInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组管理权限初始化
** @The notes : 默认权限处理
**===============================================================================================*/
void AuthorityInit(void)
{
	/*不开放端口权限*/
	if(0 == gSysHigParaRO_101[eSHWPara101_PermPort])
	{
		/*普通用户权限*/
		gGAuthorInfo_80[0] = eAuthL_User;
		gGAuthorInfo_80[1] = eAuthP_Can0;
	}
	/*开放所有端口权限*/
	else if(1 == gSysHigParaRO_101[eSHWPara101_PermPort])
	{
		/*默认开放一级运维权限*/
		gGAuthorInfo_80[0] = eAuthL_Operater;
		gGAuthorInfo_80[1] = eAuthP_All;
	}
	/*开放CAN端口权限*/
	else if(gSysHigParaRO_101[eSHWPara101_PermPort] < 6)
	{
		/*默认开放一级运维权限*/
		gGAuthorInfo_80[0] = eAuthL_Operater;
		gGAuthorInfo_80[1] = eAuthP_Can0 + (gSysHigParaRO_101[eSHWPara101_PermPort] - 2);
	}
	/*开放串口端口权限*/
	else if(gSysHigParaRO_101[eSHWPara101_PermPort] < 10)
	{
		/*默认开放一级运维权限*/
		gGAuthorInfo_80[0] = eAuthL_Operater;
		gGAuthorInfo_80[1] = eAuthP_Sci0 + (gSysHigParaRO_101[eSHWPara101_PermPort] - 6);
	}
}

/*=================================================================================================
** @Name      : void RcvSysAuthorityCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 系统接收切换管理权限命令处理函数
** @The notes : 通信口命令,按钮和内部口默认最高权限不需切换
**===============================================================================================*/
void RcvSysAuthorityCmdAPI(u8 port)
{
	/*清除权限密码正确*/
	if((AUTHORITY_NAMECLR == gGAuthorCtrl_85[0]) && (AUTHORITY_PWORDCLR == gGAuthorCtrl_85[1]))
	{
	    /*RS485#2口可清除任意端口权限到默认权限,通信所在口可清除任意端口权限到默认权限*/
		if((eAuthP_Sci1 == port) || (gGAuthorInfo_80[1] == port))
		{
		    /*复位默认权限*/
		    AuthorityInit();
		}

		/*通信所在口为默认权限口则切换为最低权限*/
		if(gGAuthorInfo_80[1] == port)
		{
			/*退回0级普通权限*/
			gGAuthorInfo_80[0] = eAuthL_User;
		}
	}
	/*一级运维权限密码正确*/
	else if((AUTHORITY_NAME1 == gGAuthorCtrl_85[0]) && (AUTHORITY_PWORD1 == gGAuthorCtrl_85[1]))
	{
		/*开放一级运维权限*/
		gGAuthorInfo_80[0] = eAuthL_Operater;
		gGAuthorInfo_80[1] = port;
	}
	/*二级高级用户权限密码正确*/
	else if((AUTHORITY_NAME2 == gGAuthorCtrl_85[0]) && (AUTHORITY_PWORD2 == gGAuthorCtrl_85[1]))
	{
		/*开放二级高级用户权限*/
		gGAuthorInfo_80[0] = eAuthL_Service;
		gGAuthorInfo_80[1] = port;
	}
	/*三级超级用户权限密码正确*/
	else if(((AUTHORITY_NAME3 == gGAuthorCtrl_85[0]) && (AUTHORITY_PWORD3 == gGAuthorCtrl_85[1]))
		|| ((AUTHORITY_NAME4 == gGAuthorCtrl_85[0]) && (AUTHORITY_PWORD4 == gGAuthorCtrl_85[1])))
	{
		/*开放三级超级用户权限*/
		gGAuthorInfo_80[0] = eAuthL_Admin;
		gGAuthorInfo_80[1] = port;
	}

	/*产生事件*/
	EventRcdDisplayUserAPI(eEVN_RCDSORT_CMD, 0, GetGSampOutCurrAPI(), gGAuthorInfo_80[0], gGAuthorInfo_80[1], port);
}

/*=================================================================================================
** @Name      : u8 GetSysNowAuthorityAPI(e_AuthOpenPort port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口 [10,12]按钮或内部口 0xff:所有口
** @Output    : 系统当前权限级别[0,3]
** @Function  : 获取系统相应端口当前权限级别
** @The notes :
**===============================================================================================*/
u8 GetSysNowAuthorityAPI(e_AuthOpenPort port)
{
	/*为权限端口*/
	if((port == gGAuthorInfo_80[1])
		|| (eAuthP_All == gGAuthorInfo_80[1]))
	{
		/*当前权限*/
		return(gGAuthorInfo_80[0]);
	}
	/*按钮或内部*/
	else if((eAuthP_BUTT == port)
		|| (eAuthP_FSTC == port)
		|| (eAuthP_INTE == port))
	{
		/*最高权限*/
		return(eAuthL_Admin);
	}
	/*未获得权限端口*/
	else
	{
		/*普通权限*/
		return(eAuthL_User);
	}
}

/*=================================================================================================
** @Name      : u8 GetSysAuthorityInfoAPI(u8 index)
** @Input     : index:信息索引
** @Output    : 系统相权限级别信息
** @Function  : 获取系统相权限级别信息
** @The notes :
**===============================================================================================*/
u8 GetSysAuthorityInfoAPI(u8 index)
{
	/*信息索引正确*/
	if(index  < GINDEX80_LEN)
	{
		return(gGAuthorInfo_80[index]);
	}

	return(0);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

