/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileGroupRcv.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控电池组信息接收处理模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicPara.h"
#include "PilePara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "PileInfo.h"
#include "PileDiagUser.h"
#include "PileDiagCheck.h"
#include "PileGroupRcv.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gGSysStateInfo_500[][GINDEX50_LEN];		/*系统状态信息数组*/
extern u16 gGLimCPInfo_510[][GINDEX51_LEN];			/*电流功率限值数组*/
extern u16 gGLimVInfo_520[][GINDEX52_LEN];			/*电压限值信息数组*/
extern u16 gGAlarmInfo_530[][GINDEX53_LEN];			/*告警信息数组*/
extern u16 gGFaultInfo_540[][GINDEX54_LEN]; 		/*故障信息数组*/
extern u16 gGErrDetail_550[][GINDEX55_LEN]; 		/*故障定位信息数组*/
extern u16 gGPortStateInfo_560[][GINDEX56_LEN];		/*输入输出口状态信息数组*/
extern s16 gGSysMainInfo_600[][GINDEX60_LEN];		/*系统整体信息数组*/
extern u16 gGSysCapInfo_610[][GINDEX61_LEN];		/*系统电量信息数组*/
extern s16 gGSysCellInfo_620[][GINDEX62_LEN]; 		/*系统单体信息数组*/
extern u16 gGVoltCharInfo_630[][GINDEX63_LEN]; 		/*电压特征信息数组*/
extern s16 gGTempCharInfo_640[][GINDEX64_LEN]; 		/*温度特征信息数组*/
extern u16 gGStatistTime_700[][GINDEX70_LEN]; 		/*时间统计信息数组*/
extern u16 gGStatistErr_710[][GINDEX71_LEN]; 		/*故障统计信息数组*/
extern u16 gGStatistCapEner_720[][GINDEX72_LEN];	/*当前能量统计信息数组*/
extern u32 gGStatistHisCap_730[][GINDEX73_LEN];		/*累计电量统计信息数组*/
extern u32 gGStatistHisEner_740[][GINDEX74_LEN];	/*累计电能统计信息数组*/
extern u16 gGStatistDhgDeep_750[][GINDEX75_LEN];	/*放电深度统计信息数组*/
extern u16 gGAuthorInfo_800[][GINDEX80_LEN]; 		/*权限级别状态信息数组*/
extern u16 gGCtrlResult_810[][GINDEX81_LEN];		/*控制结果状态信息数组*/
extern u16 gGDebugResult_820[][GINDEX82_LEN];		/*调试结果状态信息数组*/

extern u16 gPGroupState_57[]; 						/*电池组状态信息数组*/

extern u16 gSysGenParaRO_100[];  					/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];  					/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void ClrPileGroupInfoAPI(u8 group)
//输入参数	: group:组号
//输出参数	: void
//函数功能	: 清除电池堆对应组信息
//注意事项	: 只清除接收信息电池组
//=============================================================================================
void ClrPileGroupInfoAPI(u8 group)
{
    u16 i = 0;
    
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return;
    }
    
    //非自身组号(清除接收缓冲区)
    if(group != GetSysSelfGroupIdAPI())
    {
        //接收缓存区不包括自身信息
    	if(group > GetSysSelfGroupIdAPI())
    	{
    		group = group - 1;
    	}

        for(i = 0; i < GINDEX50_LEN; i++)
        {
        	gGSysStateInfo_500[group][i] = 0;
        }
        for(i = 0; i < GINDEX51_LEN; i++)
        {
        	gGLimCPInfo_510[group][i] = 0;
        }
        for(i = 0; i < GINDEX52_LEN; i++)
        {
        	gGLimVInfo_520[group][i] = 0;
        }
        for(i = 0; i < GINDEX53_LEN; i++)
        {
        	gGAlarmInfo_530[group][i] = 0;
        }
        for(i = 0; i < GINDEX54_LEN; i++)
        {
        	gGFaultInfo_540[group][i] = 0;
        }
        for(i = 0; i < GINDEX55_LEN; i++)
        {
        	gGErrDetail_550[group][i] = 0;
        }
        for(i = 0; i < GINDEX56_LEN; i++)
        {
        	gGPortStateInfo_560[group][i] = 0;
        }
        for(i = 0; i < GINDEX60_LEN; i++)
        {
        	gGSysMainInfo_600[group][i] = 0;
        }
        for(i = 0; i < GINDEX61_LEN; i++)
        {
        	gGSysCapInfo_610[group][i] = 0;
        }
        for(i = 0; i < GINDEX62_LEN; i++)
        {
        	gGSysCellInfo_620[group][i] = 0;
        }
        for(i = 0; i < GINDEX63_LEN; i++)
        {
        	gGVoltCharInfo_630[group][i] = 0;
        }
        for(i = 0; i < GINDEX64_LEN; i++)
        {
        	gGTempCharInfo_640[group][i] = 0;
        }
        for(i = 0; i < GINDEX70_LEN; i++)
        {
        	gGStatistTime_700[group][i] = 0;
        }
        for(i = 0; i < GINDEX71_LEN; i++)
        {
        	gGStatistErr_710[group][i] = 0;
        }
        for(i = 0; i < GINDEX72_LEN; i++)
        {
        	gGStatistCapEner_720[group][i] = 0;
        }
        for(i = 0; i < GINDEX73_LEN; i++)
        {
        	gGStatistHisCap_730[group][i] = 0;
        }
        for(i = 0; i < GINDEX74_LEN; i++)
        {
        	gGStatistHisEner_740[group][i] = 0;
        }
        for(i = 0; i < GINDEX75_LEN; i++)
        {
        	gGStatistDhgDeep_750[group][i] = 0;
        }
        for(i = 0; i < GINDEX80_LEN; i++)
        {
        	gGAuthorInfo_800[group][i] = 0;
        }
        for(i = 0; i < GINDEX81_LEN; i++)
        {
        	gGCtrlResult_810[group][i] = 0;
        }
        for(i = 0; i < GINDEX82_LEN; i++)
        {
        	gGDebugResult_820[group][i] = 0;
        }
    }
}

//=============================================================================================
//函数名称	: void ClrPileAllGroupDebugResultAPI(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 清除电池堆所有电池组调试结果信息
//注意事项	: 只清除接收信息电池组
//=============================================================================================
void ClrPileAllGroupDebugResultAPI(void)
{
    u8 i = 0;
    u8 j = 0;

	//清除电池组单组调试信息
    for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
    {
    	if(i != GetSysSelfGroupIdAPI())
    	{
            //接收缓存区不包括自身信息
        	if(i > GetSysSelfGroupIdAPI())
        	{
        		i = i - 1;
        	}

			for(j = 0; j < GINDEX82_LEN; j++)
			{
				gGDebugResult_820[i][i] = 0;
			}
    	}
    }
}

//=============================================================================================
//函数名称	: u16 GetPAllGroupEnStateAPI(void)
//输入参数	: void
//输出参数	: 系统激活的所有电池组(按bit位表示1组):0禁用 1激活
//函数功能	: 获取堆控系统激活的所有电池组
//注意事项	: 
//=============================================================================================
u16 GetPAllGroupEnStateAPI(void)
{
    return(gPGroupState_57[ePGroupState57_EnG]);
}

//=============================================================================================
//函数名称	: u8 GetPGroupEnStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 对应组激活使能状态 0禁用 1激活
//函数功能	: 获取电池组激活使能状态
//注意事项	: 
//=============================================================================================
u8 GetPGroupEnStateAPI(u8 group)
{
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }
    
    return(BitGet(gPGroupState_57[ePGroupState57_EnG], group));
}

//=============================================================================================
//函数名称	: u16 GetPAllGroupNorStateAPI(void)
//输入参数	: void
//输出参数	: 系统内各电池组正常状态(按bit位表示1组):0异常 1正常
//函数功能	: 获取系统内正常状态的电池组
//注意事项	: 激活且无充放电保护
//=============================================================================================
u16 GetPAllGroupNorStateAPI(void)
{
    return(gPGroupState_57[ePGroupState57_NorG]);
}

//=============================================================================================
//函数名称	: u8 GetPGroupNorStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 对应组正常状态 0异常 1正常
//函数功能	: 获取电池组正常状态
//注意事项	: 激活且无充放电保护
//=============================================================================================
u8 GetPGroupNorStateAPI(u8 group)
{
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }
    
    return(BitGet(gPGroupState_57[ePGroupState57_NorG], group));
}

//=============================================================================================
//函数名称	: u16 GetPAllGroupRunStateAPI(void)
//输入参数	: void
//输出参数	: 系统内电池组运行状态(按bit位表示1组):0未运行 1运行
//函数功能	: 获取运行状态的电池组
//注意事项	: 激活且运行接触器闭合
//=============================================================================================
u16 GetPAllGroupRunStateAPI(void)
{
    return(gPGroupState_57[ePGroupState57_RunG]);
}

//=============================================================================================
//函数名称	: u8 GetPGroupRunStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 电池组运行状态 0未运行 1运行
//函数功能	: 获取电池组运行状态
//注意事项	: 激活且运行接触器闭合
//=============================================================================================
u8 GetPGroupRunStateAPI(u8 group)
{
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }
    
    return(BitGet(gPGroupState_57[ePGroupState57_RunG], group));
}

//=============================================================================================
//函数名称	: u8 GetPGroupCanRcvStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 接收电池组CAN信息状态 0未接收 1接收
//函数功能	: 获取接收电池组CAN信息接收状态
//注意事项	: 包括自身与未激活使能组
//=============================================================================================
u8 GetPGroupCanRcvStateAPI(u8 group)
{
    u16 result = 0;

    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //获取对应电池号通讯状态
    result = BitGet(GetPRcvGroupMsgFlagAPI(), group);

    return(result);
}

//=============================================================================================
//函数名称	: u8 GetPAllGroupCanRcvStateAPI(void)
//输入参数	: void
//输出参数	: 所有电池组CAN信息接收状态 FALSE:未接收到所有组 TRUE:接收到所有组
//函数功能	: 获取接收到所有电池组CAN信息状态
//注意事项	: 只包括所有激活使能组(包括自身)
//=============================================================================================
u8 GetPAllGroupCanRcvStateAPI(void)
{
    u16 group = 0;
    
    if(gSysGenParaRO_100[eSGenPara100_AllGNum] < 16)
    {
        //计算所有激活电池组bit位
        group = (0xffff >> (16 - gSysGenParaRO_100[eSGenPara100_AllGNum])) & gPGroupState_57[ePGroupState57_EnG];
    }
    else
    {
        group = 0xffff & gPGroupState_57[ePGroupState57_EnG];
    }
    
    if(group == (GetPRcvGroupMsgFlagAPI() & group))
    {
        return(TRUE);
    }
    
    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 GetPGroupCanOffStateAPI(u8 group)
** @Input     : group:组号[0,15]
** @Output    : 堆控与电池组CAN通讯状态 0正常 1断线
** @Function  : 获取与电池组CAN通讯状态
** @The notes : 包括自身和未激活使能组
**===============================================================================================*/
u8 GetPGroupCanOffStateAPI(u8 group)
{
    u16 result = 0;

    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //获取对应电池号通讯状态
    result = BitGet(GetPileGroupCanOffFlag(), group);

    return(result);
}

//=============================================================================================
//函数名称	: u16 GetPAllGroupCanOffStateAPI(void)
//输入参数	: void
//输出参数	: 各组主控CAN通讯状态(按bit位表示1组) 0正常 1断线
//函数功能	: 获取所有电池组的CAN通讯状态
//注意事项	: 只包括所有激活使能组(包括自身)
//=============================================================================================
u16 GetPAllGroupCanOffStateAPI(void)
{
    u16 group = 0;
    
    if(gSysGenParaRO_100[eSGenPara100_AllGNum] < 16)
    {
        //计算所有激活电池组bit位
        group = (0xffff >> (16 - gSysGenParaRO_100[eSGenPara100_AllGNum])) & gPGroupState_57[ePGroupState57_EnG];
    }
    else
    {
        group = 0xffff & gPGroupState_57[ePGroupState57_EnG];
    }
    
    return(GetPileGroupCanOffFlag() & group);
}

//=============================================================================================
//函数名称	: u16 GetPGroupProStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 电池组主控系统保护状态值[0,5]
//函数功能	: 获取对应电池组主控系统保护状态
//注意事项	: 
//=============================================================================================
u16 GetPGroupProStateAPI(u8 group)
{
    u16 result = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        result = gGSysStateInfo_500[group][eSysStateInfo50_SysPro];
    }
    else
    {
        result = GetGProtectStateAPI();
    }
    
    return(result);
}

//=============================================================================================
//函数名称	: u16 GetPGroupWorkStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 对应组主控工作状态 0:开机 1:待命 2:启动 3:运行 4:停机 5:故障 6:关机
//函数功能	: 获取电池组主控工作状态
//注意事项	: 
//=============================================================================================
u16 GetPGroupWorkStateAPI(u8 group)
{
    u16 result = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        result = gGSysStateInfo_500[group][eSysStateInfo50_SysWor];
    }
    else
    {
        result = GetGWorkStateAPI();
    }
    
    return(result);
}

//=============================================================================================
//函数名称	: u16 GetPGroupChgStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 对应组主控充放电状态 0:空闲 1:充电 2:放电
//函数功能	: 获取电池组主控充放电状态
//注意事项	: 
//=============================================================================================
u16 GetPGroupChgStateAPI(u8 group)
{
    u16 result = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        result = gGSysStateInfo_500[group][eSysStateInfo50_SysChg];
    }
    else
    {
        result = GetGChgDhgStateAPI();
    }
    
    return(result);
}

//=============================================================================================
//函数名称	: u16 GetPGroupWorkModeAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 对应组主控工作模式 0:自动 1:手动 2:紧急  5:调试
//函数功能	: 获取电池组主控工作模式
//注意事项	: 
//=============================================================================================
u16 GetPGroupWorkModeAPI(u8 group)
{
    u16 result = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        result = gGSysStateInfo_500[group][eSysStateInfo50_SysMod];
    }
    else
    {
        result = GetGWorkModeAPI();
    }
    
    return(result);
}

//=============================================================================================
//函数名称	: u8 GetPGroupIsoSwhStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 电池组断路器状态 1:闭合 0:断开
//函数功能	: 获取电池组断路器状态
//注意事项	: 隔离开关
//=============================================================================================
u8 GetPGroupIsoSwhStateAPI(u8 group)
{
    u8 ioState = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        ioState = BitGet(gGPortStateInfo_560[group][ePortStateInfo56_Input], 0);
    }
    else
    {
    	ioState = BitGet(GetGInPortStateAPI(), 0);
    }

    return(ioState);
}

//=============================================================================================
//函数名称	: u8 GetPGroupMainSwhStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 对应组主接触状态 0:断开 1:闭合
//函数功能	: 获得电池组主接触器状态
//注意事项	: 
//=============================================================================================
u8 GetPGroupMainSwhStateAPI(u8 group)
{
    u8 iostate = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }

        /*高边开关和底边开关同时闭合*/
        if((BitGet(gGPortStateInfo_560[group][ePortStateInfo56_Output], 0) > 0)
        	&& (BitGet(gGPortStateInfo_560[group][ePortStateInfo56_Output], 13) > 0))
        {
        	iostate = 1;
        }
        else
        {
        	iostate = 0;
        }
    }
    else
    {
        /*高边开关和底边开关同时闭合*/
        if((BitGet(GetGOutPortStateAPI(), 0) > 0)
        	&& (BitGet(GetGOutPortStateAPI(), 13) > 0))
        {
        	iostate = 1;
        }
        else
        {
        	iostate = 0;
        }
    }
    
    return(iostate);
}

//=============================================================================================
//函数名称	: u8 GetPGroupPreSwhStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 对应组预充接触器状态 0:断开 1:闭合
//函数功能	: 获得电池组预充接触器状态
//注意事项	: 
//=============================================================================================
u8 GetPGroupPreSwhStateAPI(u8 group)
{
    u16 iostate = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        iostate = BitGet(gGPortStateInfo_560[group][ePortStateInfo56_Output], 1);
    }
    else
    {
        /*高边开关闭合*/
        if(BitGet(GetGOutPortStateAPI(), 1) > 0)
        {
        	iostate = 1;
        }
        else
        {
        	iostate = 0;
        }
    }
    
    return(iostate);
}

//=============================================================================================
//函数名称	: u8 GetPGroupNegSwhStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 对应组负接触器状态 0:断开 1:闭合
//函数功能	: 获得电池组负接触器状态
//注意事项	: 
//=============================================================================================
u8 GetPGroupNegSwhStateAPI(u8 group)
{
    u16 iostate = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }

        /*高边开关和底边开关同时闭合*/
        if((BitGet(gGPortStateInfo_560[group][ePortStateInfo56_Output], 2) > 0)
        	&& (BitGet(gGPortStateInfo_560[group][ePortStateInfo56_Output], 14) > 0))
        {
        	iostate = 1;
        }
        else
        {
        	iostate = 0;
        }
    }
    else
    {
        /*高边开关和底边开关同时闭合*/
        if((BitGet(GetGOutPortStateAPI(), 2) > 0)
        	&& (BitGet(GetGOutPortStateAPI(), 14) > 0))
        {
        	iostate = 1;
        }
        else
        {
        	iostate = 0;
        }
    }
    
    return(iostate);
}

//=============================================================================================
//函数名称	: u8 GetPGroupRunSwhAllOffStateAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 电池组运行接触器全断开状态(TRUE:全断开 FALSE:未全断开)
//函数功能	: 获取对应电池组运行接触器全断开状态
//注意事项	: 主接触器/预充接触器/负极接触器
//=============================================================================================
u8 GetPGroupRunSwhAllOffStateAPI(u8 group)
{
    u8 result = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(TRUE);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }

        /*高边开关和底边开关同时断开*/
        if((0 == BitGet(gGPortStateInfo_560[group][ePortStateInfo56_Output], 0))
            && (0 == BitGet(gGPortStateInfo_560[group][ePortStateInfo56_Output], 13))
			&& (0 == BitGet(gGPortStateInfo_560[group][ePortStateInfo56_Output], 2))
        	&& (0 == BitGet(gGPortStateInfo_560[group][ePortStateInfo56_Output], 14))
        	&& (0 == BitGet(gGPortStateInfo_560[group][ePortStateInfo56_Output], 1)))
        {
        	result = TRUE;
        }
        else
        {
        	result = FALSE;
        }
    }
    else
    {

        /*高边开关和底边开关同时断开*/
        if((0 == BitGet(GetGOutPortStateAPI(), 0))
            && (0 == BitGet(GetGOutPortStateAPI(), 13))
			&& (0 == BitGet(GetGOutPortStateAPI(), 2))
        	&& (0 == BitGet(GetGOutPortStateAPI(), 14))
	        && (0 == BitGet(GetGOutPortStateAPI(), 1)))
        {
        	result = TRUE;
        }
        else
        {
        	result = FALSE;
        }
    }
    
    return(result);
}

//=============================================================================================
//函数名称	: u16 GetPGroupChgLimCurrAPI(u8 group)
//输入参数	: group:电池组号
//输出参数	: 对应组充电电流限值(0.1A)
//函数功能	: 获得电池组充电电流限值
//注意事项	: 
//=============================================================================================
u16 GetPGroupChgLimCurrAPI(u8 group)
{
    u16 limCurr = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        limCurr = gGLimCPInfo_510[group][eLimCPInfo51_ChgC];
    }
    else
    {
    	limCurr = GetGChgLimCurrAPI();
    }
    
    return(limCurr);
}

//=============================================================================================
//函数名称	: u16 GetPGroupDhgLimCurrAPI(u8 group)
//输入参数	: group:电池组号
//输出参数	: 对应组放电电流限值(0.1A)
//函数功能	: 获得电池组放电电流限值
//注意事项	: 
//=============================================================================================
u16 GetPGroupDhgLimCurrAPI(u8 group)
{
    u16 iostate = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }

        iostate = gGLimCPInfo_510[group][eLimCPInfo51_DhgC];
    }
    else
    {
        iostate = GetGDhgLimCurrAPI();
    }
    
    return(iostate);
}

//=============================================================================================
//函数名称	: u16 GetPGroupChgLimVoltAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 对应组充电电压(总压)限值(0.1V)
//函数功能	: 获取电池组充电电压(总压)限值
//注意事项	: 
//=============================================================================================
u16 GetPGroupChgLimVoltAPI(u8 group)
{
    u16 sumV = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        sumV = gGLimVInfo_520[group][eLimVInfo52_ChgV];
    }
    else
    {
        sumV = GetGChgLimSumVAPI();
    }
    
    return(sumV);
}

//=============================================================================================
//函数名称	: u16 GetPGroupDhgLimVoltAPI(u8 group)
//输入参数	: group:组号[0,15]
//输出参数	: 对应组放电电压(总压)限值(0.1V)
//函数功能	: 获取电池组放电电压(总压)限值
//注意事项	: 
//=============================================================================================
u16 GetPGroupDhgLimVoltAPI(u8 group)
{
    u16 sumV = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        sumV = gGLimVInfo_520[group][eLimVInfo52_DhgV];
    }
    else
    {
        sumV = GetGDhgLimSumVAPI();
    }
    
    return(sumV);
}

//=============================================================================================
//函数名称	: u16 GetPGroupSSumVoltAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组采样总电压(0.1V)
//函数功能	: 获得电池组采样总电压
//注意事项	: 采样值
//=============================================================================================
u16 GetPGroupSSumVoltAPI(u8 group)
{
    u16 sumv = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        sumv = (u16)gGSysMainInfo_600[group][eSysMainInfo60_SumV];
    }
    else
    {
        sumv = GetGSampSumVoltAPI();
    }
    
    return(sumv);
}

//=============================================================================================
//函数名称	: s16 GetPGroupOutCurrAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组输出电流(0.1A)
//函数功能	: 获得电池组输出电流
//注意事项	: 
//=============================================================================================
s16 GetPGroupOutCurrAPI(u8 group)
{
    s16 sumc = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        sumc = (s16)gGSysMainInfo_600[group][eSysMainInfo60_OutC];
    }
    else
    {
        sumc = GetGSampOutCurrAPI();
    }
    
    return(sumc);
}

//=============================================================================================
//函数名称	: s16 GetPGroupOutPowerAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组输出功率(0.1kW)
//函数功能	: 获得电池组输出功率
//注意事项	:
//=============================================================================================
s16 GetPGroupOutPowerAPI(u8 group)
{
    s16 sumc = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        sumc = (s16)gGSysMainInfo_600[group][eSysMainInfo60_OutP];
    }
    else
    {
        sumc = GetGSampOutPowerAPI();
    }

    return(sumc);
}

//=============================================================================================
//函数名称	: u16 GetPGroupPreVoltAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组采样预充电压(0.1V)
//函数功能	: 获得电池组采样预充电压
//注意事项	: 采样值
//=============================================================================================
u16 GetPGroupPreVoltAPI(u8 group)
{
    u16 prev = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        prev = (u16)gGSysMainInfo_600[group][eSysMainInfo60_PreV];
    }
    else
    {
    	prev = GetGSampPreVoltAPI();
    }

    return(prev);
}

//=============================================================================================
//函数名称	: u16 GetPGroupBattSocAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组电池SOC 0.1%
//函数功能	: 获得电池组电池SOC
//注意事项	: 
//=============================================================================================
u16 GetPGroupBattSocAPI(u8 group)
{
    u16 soc = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        soc = (u16)gGSysCapInfo_610[group][eSysCapInfo61_SOC];
    }
    else
    {
        soc = GetGBattSocAPI();
    }
    
    return(soc);
}

//=============================================================================================
//函数名称	: u16 GetPGroupBattSoeAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组电池SOE 0.1%
//函数功能	: 获得电池组电池SOE
//注意事项	: 
//=============================================================================================
u16 GetPGroupBattSoeAPI(u8 group)
{
    u16 soc = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        soc = (u16)gGSysCapInfo_610[group][eSysCapInfo61_SOE];
    }
    else
    {
        soc =  GetGBattSoeAPI();
    }
    
    return(soc);
}

//=============================================================================================
//函数名称	: u16 GetPGroupBattSohAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组电池SOH 0.1%
//函数功能	: 获得电池组电池SOH
//注意事项	: 
//=============================================================================================
u16 GetPGroupBattSohAPI(u8 group)
{
    u16 soh = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        soh = (u16)gGSysCapInfo_610[group][eSysCapInfo61_SOH];
    }
    else
    {
        soh = GetGBattSohAPI();
    }
    
    return(soh);
}

//=============================================================================================
//函数名称	: u16 GetPGroupAllCapAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应电池组总容量 1AH
//函数功能	: 获得电池组总容量
//注意事项	:
//=============================================================================================
u16 GetPGroupAllCapAPI(u8 group)
{
    u16 hischgc = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        hischgc = gGSysCapInfo_610[group][eSysCapInfo61_AllCap];
    }
    else
    {
        hischgc = GetGBattAllCapAPI();
    }

    return(hischgc);
}

//=============================================================================================
//函数名称	: u16 GetPGroupCSumVoltAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单体总电压(0.1V)
//函数功能	: 获得电池组单体总电压
//注意事项	: 累加值
//=============================================================================================
u16 GetPGroupCSumVoltAPI(u8 group)
{
    u16 sumVolt = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        sumVolt = (u16)gGSysCellInfo_620[group][eSysCellInfo62_SumV];
    }
    else
    {
    	sumVolt = GetGCellSumVoltAPI();
    }

    return(sumVolt);
}

//=============================================================================================
//函数名称	: u16 GetPGroupAvgVoltAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单体平均电压(1mV)
//函数功能	: 获得电池组单体平均电压
//注意事项	: 
//=============================================================================================
u16 GetPGroupAvgVoltAPI(u8 group)
{
    u16 avgVolt = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        avgVolt = (u16)gGSysCellInfo_620[group][eSysCellInfo62_AvgV];
    }
    else
    {
    	avgVolt = GetGCellAvgVoltAPI();
    }
    
    return(avgVolt);
}

//=============================================================================================
//函数名称	: s16 GetPGroupAvgTempAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单体平均温度(1℃)
//函数功能	: 获得电池组单体平均温度
//注意事项	:
//=============================================================================================
s16 GetPGroupAvgTempAPI(u8 group)
{
    s16 avgTemp = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        avgTemp = gGSysCellInfo_620[group][eSysCellInfo62_AvgT];
    }
    else
    {
    	avgTemp = GetGCellAvgTempAPI();
    }

    return(avgTemp);
}

//=============================================================================================
//函数名称	: s16 GetPGroupEnvirTempAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组环境温度(1℃)
//函数功能	: 获得电池组环境温度值
//注意事项	: 采样值
//=============================================================================================
s16 GetPGroupEnvirTempAPI(u8 group)
{
    s16 envTemp = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        envTemp = gGSysCellInfo_620[group][eSysCellInfo62_EnvT];
    }
    else
    {
    	envTemp = GetGEnvirTempAPI();
    }

    return(envTemp);
}

//=============================================================================================
//函数名称	: u16 GetPGroupCellMaxVNumAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单体最大电压电池号[1,400]
//函数功能	: 获取电池组单体最大电压电池号
//注意事项	:
//=============================================================================================
u16 GetPGroupCellMaxVNumAPI(u8 group)
{
    u16 num = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        num = gGVoltCharInfo_630[group][eVoltCharInfo63_MaxVNum];
    }
    else
    {
        num = GetGCellMaxVNumAPI();
    }
    
    return(num);
}

//=============================================================================================
//函数名称	: u16 GetPGroupCellMaxVoltAPI(u8 group)
//输入参数	: group:电池组号
//输出参数	: 对应组单体最大电压值(1mv)
//函数功能	: 获取电池组单体最大电压值
//注意事项	: 
//=============================================================================================
u16 GetPGroupCellMaxVoltAPI(u8 group)
{
    u16 volt = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        volt = gGVoltCharInfo_630[group][eVoltCharInfo63_MaxVolt];
    }
    else
    {
    	volt = GetGCellMaxVoltAPI();
    }
    
    return(volt);
}

//=============================================================================================
//函数名称	: u16 GetPGroupCellMinVNumAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单体最小电压电池号[1,400]
//函数功能	: 获取电池组单体最小电压电池号
//注意事项	:
//=============================================================================================
u16 GetPGroupCellMinVNumAPI(u8 group)
{
    u16 num = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        num = gGVoltCharInfo_630[group][eVoltCharInfo63_MinVNum];
    }
    else
    {
        num = GetGCellMinVNumAPI();
    }
    
    return(num);
}

//=============================================================================================
//函数名称	: u16 GetPGroupCellMinVoltAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单体最小电压值(1mv)
//函数功能	: 获取电池组单体最小电压值
//注意事项	: 
//=============================================================================================
u16 GetPGroupCellMinVoltAPI(u8 group)
{
    s16 temp = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        temp = gGVoltCharInfo_630[group][eVoltCharInfo63_MinVolt];
    }
    else
    {
        temp = GetGCellMinVoltAPI();
    }
    
    return(temp);
}

//=============================================================================================
//函数名称	: u16 GetPGroupCellMaxTNumAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单体最大温度序号[1,400]
//函数功能	: 获取电池组单体最大温度序号
//注意事项	:
//=============================================================================================
u16 GetPGroupCellMaxTNumAPI(u8 group)
{
    u16 num = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        num = gGTempCharInfo_640[group][eTempCharInfo64_MaxTNum];
    }
    else
    {
        num = GetGCellMaxTNumAPI();
    }
    
    return(num);
}

//=============================================================================================
//函数名称	: s16 GetPGroupCellMaxTempAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单体最大温度值(1℃)
//函数功能	: 获取电池组单体最大温度值
//注意事项	: 
//=============================================================================================
s16 GetPGroupCellMaxTempAPI(u8 group)
{
    s16 temp = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        temp = gGTempCharInfo_640[group][eTempCharInfo64_MaxTemp];
    }
    else
    {
        temp = GetGCellMaxTempAPI();
    }
    
    return(temp);
}

//=============================================================================================
//函数名称	: u16 GetPGroupCellMinTNumAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单体最小温度序号[1,400]
//函数功能	: 获取电池组单体最小温度序号
//注意事项	:
//=============================================================================================
u16 GetPGroupCellMinTNumAPI(u8 group)
{
    u16 num = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        num = gGTempCharInfo_640[group][eTempCharInfo64_MinTNum];
    }
    else
    {
        num = GetGCellMinTNumAPI();
    }
    
    return(num);
}

//=============================================================================================
//函数名称	: s16 GetPGroupCellMinTempAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单体最低温度值(1℃)
//函数功能	: 获取电池组单体最低温度值
//注意事项	: 
//=============================================================================================
s16 GetPGroupCellMinTempAPI(u8 group)
{
    s16 temp = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        temp = gGTempCharInfo_640[group][eTempCharInfo64_MinTemp];
    }
    else
    {
        temp = GetGCellMinTempAPI();
    }
    
    return(temp);
}

//=============================================================================================
//函数名称	: u16 GetPGroupCanChgTimeAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组可充电时间(1min)
//函数功能	: 获得电池组可充电时间
//注意事项	: 
//=============================================================================================
u16 GetPGroupCanChgTimeAPI(u8 group)
{
    u16 time = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        time = gGStatistTime_700[group][eTime70_CanChg];
    }
    else
    {
        time = GetGCanChgTimeAPI();
    }
    
    return(time);
}

//=============================================================================================
//函数名称	: u16 GetPGroupCanDhgTimeAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组可放电时间(1min)
//函数功能	: 获得电池组可放电时间
//注意事项	: 
//=============================================================================================
u16 GetPGroupCanDhgTimeAPI(u8 group)
{
    u16 time = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        time = gGStatistTime_700[group][eTime70_CanDhg];
    }
    else
    {
        time = GetGCanDhgTimeAPI();
    }
    
    return(time);
}

//=============================================================================================
//函数名称	: u16 GetPGroupChgTimesAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组充放电次数
//函数功能	: 获得电池组充放电次数
//注意事项	: 
//=============================================================================================
u16 GetPGroupChgTimesAPI(u8 group)
{
    u16 times = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        times = gGStatistTime_700[group][eTime70_ChgCycle];
    }
    else
    {
        times = GetGChgCycleTimesAPI();
    }
    
    return(times);
}

//=============================================================================================
//函数名称	: u16 GetPGroupMonWarNumAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组当月告警数目
//函数功能	: 获得电池组当月告警数目
//注意事项	:
//=============================================================================================
u16 GetPGroupMonWarNumAPI(u8 group)
{
    u16 hiswarmNum = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        hiswarmNum = (u32)gGStatistErr_710[group][eErr71_MonWarNum];
    }
    else
    {
        hiswarmNum = GetGMonWarNumAPI();
    }

    return(hiswarmNum);
}

//=============================================================================================
//函数名称	: u16 GetPGroupMonErrNumAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应电池组当月故障数目
//函数功能	: 获得电池组当月故障数目
//注意事项	:
//=============================================================================================
u16 GetPGroupMonErrNumAPI(u8 group)
{
    u16 hiswarmNum = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        hiswarmNum = (u32)gGStatistErr_710[group][eErr71_MonErrNum];
    }
    else
    {
        hiswarmNum = GetGMonErrNumAPI();
    }

    return(hiswarmNum);
}

//=============================================================================================
//函数名称	: u16 GetPGroupHisWarNumAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组历史告警数目
//函数功能	: 获得电池组历史告警数目
//注意事项	: 
//=============================================================================================
u16 GetPGroupHisWarNumAPI(u8 group)
{
    u16 hiswarmNum = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        hiswarmNum = (u32)gGStatistErr_710[group][eErr71_HisWarNum];
    }
    else
    {
        hiswarmNum = GetGHisWarNumAPI();
    }
    
    return(hiswarmNum);
}

//=============================================================================================
//函数名称	: u16 GetPGroupHisErrNumAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组历史故障数目
//函数功能	: 获得电池组历史故障数目
//注意事项	: 
//=============================================================================================
u16 GetPGroupHisErrNumAPI(u8 group)
{
    u16 hiswErrNum = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        hiswErrNum = (u32)gGStatistErr_710[group][eErr71_HisErrNum];
    }
    else
    {
        hiswErrNum = GetGHisErrNumAPI();
    }
    
    return(hiswErrNum);
}

//=============================================================================================
//函数名称	: u16 GetPGroupRemainEnerAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组剩余电能 0.1kWh
//函数功能	: 获得电池组剩余电能
//注意事项	: 
//=============================================================================================
u16 GetPGroupRemainEnerAPI(u8 group)
{
    u16 remainEner = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        remainEner = gGStatistCapEner_720[group][eCapEner72_NowEner];
    }
    else
    {
        remainEner = GetGRemainEnerAPI();
    }
    
    return(remainEner);
}

//=============================================================================================
//函数名称	: u16 GetPGroupAllEnerAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组总电能 0.1kWh
//函数功能	: 获得电池组总电能
//注意事项	: 
//=============================================================================================
u16 GetPGroupAllEnerAPI(u8 group)
{
    u16 allener = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        allener = (u32)gGStatistCapEner_720[group][eCapEner72_AllEner];
    }
    else
    {
        allener = GetGBattAllEnerAPI();
    }
    
    return(allener);
}

//=============================================================================================
//函数名称	: u16 GetPGroupNowChgCapAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单次充电电量 1AH
//函数功能	: 获得电池组单次充电电量
//注意事项	: 
//=============================================================================================
u16 GetPGroupNowChgCapAPI(u8 group)
{
    u16 nowChgC = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        nowChgC = gGStatistCapEner_720[group][eCapEner72_ChgCap];
    }
    else
    {
        nowChgC = GetGNowChgCapAPI();
    }
    
    return(nowChgC);
}

//=============================================================================================
//函数名称	: u16 GetPGroupNowDhgCapAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组单次放电电量 1AH
//函数功能	: 获得电池组单次放电电量
//注意事项	: 
//=============================================================================================
u16 GetPGroupNowDhgCapAPI(u8 group)
{
    u32 nowDhgC = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        nowDhgC = (u32)gGStatistCapEner_720[group][eCapEner72_DhgCap];
    }
    else
    {
        nowDhgC = GetGNowDhgCapAPI();
    }
    
    return(nowDhgC);
}

//=============================================================================================
//函数名称	: u32 GetPGroupHisChgCapAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组历史充电电量 1AH
//函数功能	: 获得电池组历史充电电量
//注意事项	: 
//=============================================================================================
u32 GetPGroupHisChgCapAPI(u8 group)
{
    u32 hischgc = 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        hischgc = gGStatistHisCap_730[group][eHisCap73_HisChgCap];
    }
    else
    {
        hischgc = GetGHisChgCapAPI();
    }
    
    return(hischgc);
}

//=============================================================================================
//函数名称	: u32 GetPGroupHisDhgCapAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组历史放电电量 1AH
//函数功能	: 获得电池组历史放电电量
//注意事项	: 
//=============================================================================================
u32 GetPGroupHisDhgCapAPI(u8 group)
{
    u16 hisdhgc= 0;

    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        hisdhgc = gGStatistHisCap_730[group][eHisCap73_HisDhgCap];
    }
    else
    {
        hisdhgc = GetGHisDhgCapAPI();
    }
    
    return(hisdhgc);
}

//=============================================================================================
//函数名称	: u32 GetPGroupHisChgEnerAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组历史充电电能 0.1kWh
//函数功能	: 获得电池组历史充电电能
//注意事项	: 
//=============================================================================================
u32 GetPGroupHisChgEnerAPI(u8 group)
{
    u32 hische = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        hische = gGStatistHisEner_740[group][eHisEner74_HisChgEner];
    }
    else
    {
        hische = GetGHisChgEnerAPI();
    }
    
    return(hische);
}

//=============================================================================================
//函数名称	: u32 GetPGroupHisDhgEnerAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 对应组历史放电电能 0.1kWh
//函数功能	: 获得电池组历史放电电能
//注意事项	: 
//=============================================================================================
u32 GetPGroupHisDhgEnerAPI(u8 group)
{
    u32 hisdchge = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        hisdchge = gGStatistHisEner_740[group][eHisEner74_HisDhgEner];
    }
    else
    {
        hisdchge = GetGHisDhgEnerAPI();
    }
    
    return(hisdchge);
}

//=============================================================================================
//函数名称	: u16 GetPGroupCtrlResultAPI(u8 group, u8 index)
//输入参数	: group:电池组号[0,15] index:控制结果信息子索引号[0,7]
//输出参数	: 对应组控制结果指定子索引信息
//函数功能	: 获得电池组控制结果信息
//注意事项	:
//=============================================================================================
u16 GetPGroupCtrlResultAPI(u8 group, u8 index)
{
    u16 result = 0;

    //判断输入的电池组号是否超过最大组数
    if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    	|| (index >= GINDEX81_LEN))
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        result = gGCtrlResult_810[group][index];
    }
    else
    {
    	result = GetGCtrlResultAPI(index);
    }

    return(result);
}

//=============================================================================================
//函数名称	: u16 GetPGroupDebugResultAPI(u8 group, u8 index)
//输入参数	: group:电池组号[0,15] index:调试信息子索引号[0,7]
//输出参数	: 对应组调试结果指定子索引信息
//函数功能	: 获得电池组调试结果信息
//注意事项	:
//=============================================================================================
u16 GetPGroupDebugResultAPI(u8 group, u8 index)
{
    u16 result = 0;

    //判断输入的电池组号是否超过最大组数
    if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    	|| (index >= GINDEX82_LEN))
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        result = gGDebugResult_820[group][index];
    }
    else
    {
    	result = GetGDebugResultAPI(index);
    }

    return(result);
}

//=============================================================================================
//函数名称	: u8 GetPGroupAllPreSafeCStateAPI(void)
//输入参数	: void
//输出参数	: 所有电池组预充电流安全状态 0:不安全 1:安全
//函数功能	: 获取所有电池组预充电流安全状态
//注意事项	: 使能且预充接触器闭合电池组
//=============================================================================================
u8 GetPGroupAllPreSafeCStateAPI(void)
{
    u8 temp = 0;
    s16 currMin = 0;
    s16 currMax = 0;
    
    currMin = 0 - (s16)gPSmuGenParaR_102[eSmuGenPara102_PreSafeC];
    currMax = (s16)gPSmuGenParaR_102[eSmuGenPara102_PreSafeC];
    
    for(temp = 0; temp < gSysGenParaRO_100[eSGenPara100_AllGNum]; temp++)
    {
        //未使能或接触器未闭合
        if((0 == GetPGroupEnStateAPI(temp))
            || (0 == GetPGroupPreSwhStateAPI(temp)))
        {
            continue;
        }
        
        //判断电流是否是安全电流
        if(0 == IsInside(currMin, GetPGroupOutCurrAPI(temp), currMax))
        {
            return(0);
        }
    }
    
    return(1);
}

//=============================================================================================
//函数名称	: u8 GetPGroupAllMainSafeCStateAPI(void)
//输入参数	: void
//输出参数	: 所有电池组主回路电流安全状态 0:不安全 1:安全
//函数功能	: 获取所有电池组主回路电流安全状态
//注意事项	: 使能且主接触器闭合电池组
//=============================================================================================
u8 GetPGroupAllMainSafeCStateAPI(void)
{
    u8 temp = 0;
    s16 currMin = 0;
    s16 currMax = 0;
    
    currMin = 0 - (s16)gPSmuGenParaR_102[eSmuGenPara102_MaiSafeC];
    currMax = (s16)gPSmuGenParaR_102[eSmuGenPara102_MaiSafeC];
    
    for(temp = 0; temp < gSysGenParaRO_100[eSGenPara100_AllGNum]; temp++)
    {
        //未使能或接触器未闭合
        if((0 == GetPGroupEnStateAPI(temp))
            || (0 == GetPGroupMainSwhStateAPI(temp)))
        {
            continue;
        }
        
        //判断电流是否是安全电流
        if(0 == IsInside(currMin, GetPGroupOutCurrAPI(temp), currMax))
        {
            return(0);
        }
    }
    
    return(1);
}

//=============================================================================================
//函数名称	: u8 GetPGroupMainSafeCStateAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 相应组主回路电流安全状态 0:不安全 1:安全
//函数功能	: 获取电池组主回路电流安全状态
//注意事项	: 
//=============================================================================================
u8 GetPGroupMainSafeCStateAPI(u8 group)
{
    s16 currMin = 0;
    s16 currMax = 0;
    
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(1);
    }
    
    currMin = 0 - (s16)gPSmuGenParaR_102[eSmuGenPara102_MaiSafeC];
    currMax = (s16)gPSmuGenParaR_102[eSmuGenPara102_MaiSafeC];

    //判断电流是否是安全电流
    if(0 == IsInside(currMin, GetPGroupOutCurrAPI(group), currMax))
    {
        return(0);
    }
    
    return(1);
}

//=============================================================================================
//函数名称	: u8 GetPGroupEmStopStateAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: 相应组紧急停机状态 0:非急停 1:急停
//函数功能	: 获取电池组紧急停机状态
//注意事项	: 手动按钮急停
//=============================================================================================
u8 GetPGroupEmStopStateAPI(u8 group)
{
    //判断输入的电池组号是否超过最大组数
    if(group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
    {
        return(1);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }

        //判断是否按钮急停
        if(1 == BitGet(gGFaultInfo_540[group][2], G_ERR_FST_BIT_BUTEMSTOP))
        {
            return(1);
        }
    }
    //自身组号
    else
    {
        //判断是否按钮急停
        if(1 == BitGet(GetGFaultInfoAPI(2), G_ERR_FST_BIT_BUTEMSTOP))
        {
            return(1);
        }
    }
    
    return(0);
}

//=============================================================================================
//函数名称	: u8 GetPAllGroupCellVLStateAPI(void)
//输入参数	: group:电池组号[0,15]
//输出参数	: 相应组电池组低压告警状态  0:正常 1:一级故障 2:二级告警
//函数功能	: 获取电池组低压告警状态
//注意事项	: 上电的电池组
//=============================================================================================
u8 GetPAllGroupCellVLStateAPI(void)
{
	u8 temp = 0;
	u8 flag = 0;

    for(temp = 0; temp < gSysGenParaRO_100[eSGenPara100_AllGNum]; temp++)
    {
        //使能或断路器闭合,有CAN信息
        if(((1 == GetPGroupEnStateAPI(temp))
            || (1 == GetPGroupIsoSwhStateAPI(temp)))
        	&& (1 == GetPGroupCanRcvStateAPI(temp)))
        {
        	//获取低压一级告警码状态
            if(1 == GetPGroupRcvWarStateAPI(temp, eGWAR_ID_CEL_VL1))
			{
            	flag = 1;
			}
        	//获取低压二级告警码状态
            else if(1 == GetPGroupRcvWarStateAPI(temp, eGWAR_ID_CEL_VL2))
			{
            	if(0 == flag)
            	{
            		flag = 2;
            	}
			}
        }
    }

    return(flag);
}

//=============================================================================================
//函数名称	: u8 GetPAllGroupLeakStateAPI(void)
//输入参数	: group:电池组号[0,15]
//输出参数	: 相应组电池组绝缘告警状态  0:正常 1:一级故障 2:二级告警
//函数功能	: 获取电池组绝缘告警状态
//注意事项	: 上电的电池组
//=============================================================================================
u8 GetPAllGroupLeakStateAPI(void)
{
	u8 temp = 0;
	u8 flag = 0;

    for(temp = 0; temp < gSysGenParaRO_100[eSGenPara100_AllGNum]; temp++)
    {
        //使能或断路器闭合,有CAN信息
        if(((1 == GetPGroupEnStateAPI(temp))
            || (1 == GetPGroupIsoSwhStateAPI(temp)))
        	&& (1 == GetPGroupCanRcvStateAPI(temp)))
        {
        	//获取绝缘一级告警码状态
            if(1 == GetPGroupRcvWarStateAPI(temp, eGWAR_ID_LEAK1))
			{
            	flag = 1;
			}
        	//获取绝缘二级告警码状态
            else if(1 == GetPGroupRcvWarStateAPI(temp, eGWAR_ID_LEAK2))
			{
            	if(0 == flag)
            	{
            		flag = 2;
            	}
			}
        }
    }

    return(flag);
}

//=============================================================================================
//函数名称	: u8 GetPGroupRcvWarStateAPI(u8 group, u8 id)
//输入参数	: group:电池组号[0,15] id:告警码[0,47]
//输出参数	: 接收到对应组告警码状态 0:正常 1:告警
//函数功能	: 获取接收到主控的告警码状态
//注意事项	:
//=============================================================================================
u8 GetPGroupRcvWarStateAPI(u8 group, u8 id)
{
    u8 state = 0;

    //判断输入的电池组号是否超过最大组数
    if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
        || (id >= eGERR_ID_FIN))
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }

        //前16个故障
        if(id < 16)
        {
            state = BitGet(gGAlarmInfo_530[group][0], id);
        }
        //中16个故障
        else if(id < 32)
        {
            state = BitGet(gGAlarmInfo_530[group][1], (id - 16));
        }
        //后16个故障
        else if(id < 48)
        {
            state = BitGet(gGAlarmInfo_530[group][2], (id - 32));
        }
        //其他无效
        else
        {
            state = 0;
        }
    }
    //自身组号
    else
    {
        //前16个故障
        if(id < 16)
        {
            state = BitGet(GetGAlarmInfoAPI(0), id);
        }
        //中16个故障
        else if(id < 32)
        {
            state = BitGet(GetGAlarmInfoAPI(1), (id - 16));
        }
        //后16个故障
        else if(id < 48)
        {
            state = BitGet(GetGAlarmInfoAPI(2), (id - 32));
        }
        //其他无效
        else
        {
            state = 0;
        }
    }

    return(state);
}

//=============================================================================================
//函数名称	: u8 GetPGroupRcvErrStateAPI(u8 group, u8 id)
//输入参数	: group:电池组号[0,15] id:故障码
//输出参数	: 接收到对应组故障码状态 0:正常 1:故障
//函数功能	: 获取接收到主控的故障码状态
//注意事项	: 
//=============================================================================================
u8 GetPGroupRcvErrStateAPI(u8 group, u8 id)
{
    u8 state = 0;
    
    //判断输入的电池组号是否超过最大组数
    if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
        || (id >= eGERR_ID_FIN))
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }
        
        //前16个故障
        if(id < 16)
        {
            state = BitGet(gGFaultInfo_540[group][0], id);
        }
        //中16个故障
        else if(id < 32)
        {
            state = BitGet(gGFaultInfo_540[group][1], (id - 16));
        }
        //后16个故障
        else if(id < 48)
        {
            state = BitGet(gGFaultInfo_540[group][2], (id - 32));
        }
        //其他无效
        else
        {
            state = 0;
        }
    }
    //自身组号
    else
    {
        //前16个故障
        if(id < 16)
        {
            state = BitGet(GetGFaultInfoAPI(0), id);
        }
        //中16个故障
        else if(id < 32)
        {
            state = BitGet(GetGFaultInfoAPI(1), (id - 16));
        }
        //后16个故障
        else if(id < 48)
        {
            state = BitGet(GetGFaultInfoAPI(2), (id - 32));
        }
        //其他无效
        else
        {
            state = 0;
        }
    }
    
    return(state);
}

//=============================================================================================
//函数名称	: u16 GetPGroupRcvWarInfoAPI(u8 group, u8 index)
//输入参数	: group:电池组号[0,15] index:告警信息索引[0,2]
//输出参数	: 接收到对应组告警信息(每bit为1个告警)
//函数功能	: 获取接收到主控的告警信息
//注意事项	:
//=============================================================================================
u16 GetPGroupRcvWarInfoAPI(u8 group, u8 index)
{
    u16 state = 0;

    //判断输入的电池组号是否超过最大组数
    if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
        || (index >= GINDEX53_LEN))
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }

        state = gGAlarmInfo_530[group][index];
    }
    //自身组号
    else
    {
        state = GetGAlarmInfoAPI(index);
    }

    return(state);
}

//=============================================================================================
//函数名称	: u16 GetPGroupRcvErrInfoAPI(u8 group, u8 index)
//输入参数	: group:电池组号[0,15] index:故障信息索引[0,2]
//输出参数	: 接收到对应组故障信息(每bit为1个故障)
//函数功能	: 获取接收到主控的故障信息
//注意事项	:
//=============================================================================================
u16 GetPGroupRcvErrInfoAPI(u8 group, u8 index)
{
    u16 state = 0;

    //判断输入的电池组号是否超过最大组数
    if((group >= gSysGenParaRO_100[eSGenPara100_AllGNum])
        || (index >= GINDEX54_LEN))
    {
        return(0);
    }

    //非堆控自身所在主控
    if(group != GetSysSelfGroupIdAPI())
    {
        //组号在主系统之后
        if(group > GetSysSelfGroupIdAPI())
        {
            group = group - 1;
        }

        state = gGFaultInfo_540[group][index];
    }
    //自身组号
    else
    {
        state = GetGFaultInfoAPI(index);
    }

    return(state);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/


#endif
