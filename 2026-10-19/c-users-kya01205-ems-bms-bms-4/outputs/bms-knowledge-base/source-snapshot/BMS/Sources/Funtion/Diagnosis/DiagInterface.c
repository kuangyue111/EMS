/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : DiagInterface.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池故障诊断模块用户接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "InsuCheck.h"
#include "DiagInterface.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gGBmuGenPara_102[];                              /*主控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 DiagGetErrProMaskStateHOOK(e_ERR_ID id)
** @Input     : id:故障ID
** @Output    : 故障是否屏蔽故障保护 0不屏蔽 1屏蔽
** @Function  : 获取对应ID故障是否屏蔽故障保护
** @The notes : 用户完善
**===============================================================================================*/
u8 DiagGetErrProMaskStateHOOK(e_ERR_ID id)
{
	/*紧急模式*/
	if(eMODE_EMERG == GetGWorkModeAPI())
	{
		/*指定故障屏蔽*/
		if((eERR_ID_CEL_VL2 == id) || (eERR_ID_CEL_VL1 == id)
			|| (eERR_ID_SUM_VL2 == id) || (eERR_ID_SUM_VL1 == id))
		{
			return(1);
		}
	}
	
	return(0);
}

/*=================================================================================================
** @Name      : u8 DiagCheckErrNeedDispHOOK(e_ERR_ID id)
** @Input     : id:故障ID
** @Output    : 是否需要显示:TRUE:显示 FALSE:不显示
** @Function  : 判断对应故障告警是否向客户显示
** @The notes : 仅检测默认不显示的提示类告警
**===============================================================================================*/
u8 DiagCheckErrNeedDispHOOK(e_ERR_ID id)
{
    //单体过压二级
    if(eERR_ID_CEL_VH2 == id)
    {
        //电压即将到达一级仍在充电则显示告警
        if((GetGCellMaxVoltAPI() >= ((gGBmuGenPara_102[eBmuGenPara102_VH2Lim] + gGBmuGenPara_102[eBmuGenPara102_VH1Lim]) / 2))
        	&& (eCURR_CHG == GetGChgDhgStateAPI()))
    	{
    	    return(TRUE);
    	}
    }
    //单体欠压二级
    else if(eERR_ID_CEL_VL2 == id)
    {
        //电压即将到达一级仍在放电则显示告警
        if((GetGCellMinVoltAPI() <= ((gGBmuGenPara_102[eBmuGenPara102_VL2Lim] + gGBmuGenPara_102[eBmuGenPara102_VL1Lim]) / 2))
        	&& (eCURR_DHG == GetGChgDhgStateAPI()))
    	{
    	    return(TRUE);
    	}
    }
    //充电过温二级
    else if(eERR_ID_CHG_TH2 == id)
    {
        //温度即将到达一级仍在充电则显示告警
        if((GetGCellMaxTempAPI() >= ((s16)((s16)gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim] + (s16)gGBmuGenPara_102[eBmuGenPara102_ChgTH1Lim]) / 2))
        	&& (eCURR_CHG == GetGChgDhgStateAPI()))
    	{
    	    return(TRUE);
    	}
    }
    //充电低温二级
    else if(eERR_ID_CHG_TL2 == id)
    {
        //温度即将到达一级仍在充电则显示告警
        if((GetGCellMinTempAPI() <= ((s16)((s16)gGBmuGenPara_102[eBmuGenPara102_ChgTL2Lim] + (s16)gGBmuGenPara_102[eBmuGenPara102_ChgTL1Lim]) / 2))
        	&& (eCURR_CHG == GetGChgDhgStateAPI()))
    	{
    	    return(TRUE);
    	}
    }
    //放电过温二级
    else if(eERR_ID_DHG_TH2 == id)
    {
        //温度即将到达一级仍在放电则显示告警
        if((GetGCellMaxTempAPI() >= ((s16)((s16)gGBmuGenPara_102[eBmuGenPara102_DhgTH2Lim] + (s16)gGBmuGenPara_102[eBmuGenPara102_DhgTH1Lim]) / 2))
        	&& (eCURR_DHG == GetGChgDhgStateAPI()))
    	{
    	    return(TRUE);
    	}
    }
    //放电过温二级
    else if(eERR_ID_DHG_TL2 == id)
    {
        //温度即将到达一级仍在放电则显示告警
        if((GetGCellMinTempAPI() <= ((s16)((s16)gGBmuGenPara_102[eBmuGenPara102_DhgTL2Lim] + (s16)gGBmuGenPara_102[eBmuGenPara102_DhgTL1Lim]) / 2))
        	&& (eCURR_DHG == GetGChgDhgStateAPI()))
    	{
    	    return(TRUE);
    	}
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 DiagGetCellVTExpStateHOOKk(void)
** @Input     : void
** @Output    : 电池组单体采样异常状态: 0:正常 1:异常
** @Function  : 获取电池组单体采样异常状态
** @The notes : 从控电压采样失效/断线/CAN断线时从控采样异常
**===============================================================================================*/
u8 DiagGetCellVTExpStateHOOKk(void)
{
    /*未接收到全部单体电压或采样断线*/
    if((2 != DiagGetRcvAllSlaveFlagAPI())
        || (2 != DiagGetRcvAllCellFlagAPI())
        || (DiagGetCellSampErrFlagAPI() > 0)
        || (DiagGetCellVOffCellBitAPI() > 0)
        || (DiagGetCellTOffCellBitAPI() > 0)
        || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_SAMP_EXP))
        /*|| (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_SAMP_EXP))*/
        /*|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_INT_LOFF))
        || (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_INT_LOFF))*/)
    {
        return(1);
    }

    return(0);
}

/*=================================================================================================
** @Name      : u8 DiagGetCellVoltExpStateHOOK(void)
** @Input     : void
** @Output    : 单体电压异常状态 0正常 1异常
** @Function  : 获取单体电压异常状态
** @The notes : 用户完善
**===============================================================================================*/
u8 DiagGetCellVoltExpStateHOOK(void)
{
	/*未接收到全部单体电压或采样断线*/
	if((2 != DiagGetRcvAllSlaveFlagAPI())
		|| (2 != DiagGetRcvAllCellFlagAPI())
		|| (DiagGetCellSampErrFlagAPI() > 0)
		|| (DiagGetCellVOffCellBitAPI() > 0)
		|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_SAMP_EXP))
		|| (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_SAMP_EXP))
		/*|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_INT_LOFF))
		|| (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_INT_LOFF))*/)
	{
		return(1);
	}

	return(0);
}

/*=================================================================================================
** @Name      : u8 DiagGetCellTempExpStateHOOK(void)
** @Input     : void
** @Output    : 单体温度异常状态 0正常 1异常
** @Function  : 获取单体温度异常状态
** @The notes : 用户完善
**===============================================================================================*/
u8 DiagGetCellTempExpStateHOOK(void)
{
	/*未接收到全部单体温度或采样断线*/
	if((2 != DiagGetRcvAllSlaveFlagAPI())
		|| (2 != DiagGetRcvAllCellFlagAPI())
		|| (DiagGetCellSampErrFlagAPI() > 0)
		|| (DiagGetCellTOffCellBitAPI() > 0)
		|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_SAMP_EXP))
		|| (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_SAMP_EXP))
		/*|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_INT_LOFF))
		|| (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_INT_LOFF))*/)
	{
		return(1);
	}

	return(0);
}

/*=================================================================================================
** @Name      : u16 DiagGetCellVDiffSubVHOOK(u16 value)
** @Input     : 转换前的单体电压压差过大压差
** @Output    : 转换后的单体电压压差过大压差 1mV
** @Function  : 获取单体电压压差过大压差值
** @The notes : 根据电压区间转换
**===============================================================================================*/
u16 DiagGetCellVDiffSubVHOOK(u16 value)
{
    u16 maxV = 0;
    u16 minV = 0;
    u16 avgV = 0;
    u16 temp = 0;

    maxV = GetGCellMaxVoltAPI();
    minV = GetGCellMinVoltAPI();
    avgV = GetGCellAvgVoltAPI();

    //所有电压完全在检测区间
    if((minV >= gGBmuGenPara_102[eBmuGenPara102_CellPlaDnV])
        && (maxV <= gGBmuGenPara_102[eBmuGenPara102_CellPlaUpV]))
    {
        //取配置告警差值
        temp = value;
    }
    //平均电压在检测区间而最大最小值不在检测区间
    else if(((avgV >= gGBmuGenPara_102[eBmuGenPara102_CellPlaDnV])
        && (maxV >= gGBmuGenPara_102[eBmuGenPara102_CellPlaUpV]))
        || ((avgV <= gGBmuGenPara_102[eBmuGenPara102_CellPlaUpV])
        && (minV <= gGBmuGenPara_102[eBmuGenPara102_CellPlaDnV])))
    {
        //取配置告警差值的1.5倍
        temp = value * 15 / 10;
    }
    //平均电压临近检测区间上边界
    else if((avgV >= gGBmuGenPara_102[eBmuGenPara102_CellPlaUpV])
        && (minV <= gGBmuGenPara_102[eBmuGenPara102_CellPlaUpV]))
    {
        //取配置告警差值的1.5倍
        temp = value * 15 / 10;
    }
    //平均电压临近检测区间下边界
    else if((avgV <= gGBmuGenPara_102[eBmuGenPara102_CellPlaDnV])
        && (maxV >= gGBmuGenPara_102[eBmuGenPara102_CellPlaDnV]))
    {
        //取配置告警差值的1.5倍
        temp = value * 15 / 10;
    }
    //平均电压远远不在检测区间
    else
    {
        //取配置告警差值的2倍
        temp = value * 2;
    }

    return(temp);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

