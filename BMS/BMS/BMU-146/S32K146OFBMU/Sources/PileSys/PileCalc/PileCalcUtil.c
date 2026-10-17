/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileCalcUtil.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控计算接口工具模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PileCalcUtil.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//================================================================================
//函数名称	: s16 PileUtilPiAdjust(b_PIIO* pPiIO,b_PICfg* pPiCfg)
//输入参数	: b_PIIO* pPiIO,b_PICfg* pPiCfg
//输出参数	: 0:正常 1:异常
//函数功能	: PI调节通用函数
//注意事项	: 
//================================================================================
s16 PileUtilPiAdjust(b_PIIO* pPiIO,const b_PICfg*  pPiCfg)
{
    s16 temp16;         // 16位临时变量
    s32 temp32;         // 32位临时变量
    s16 tempPiOut = 0;  // 积分中间计算变量
    s32 tempPiSum = 0;  // 积分累加中间计算变量
    
    pPiIO->piIn = (s16)PileUtilLimitRange(pPiIO->piIn ,pPiCfg->piInMin,pPiCfg->piInMax);
    
    //参数有效性检测
    if((pPiIO == 0) || (pPiCfg == 0))
    {
        return (1);
    }
    
    //========================P调节=============================================
    tempPiOut = (s16)(((s32)(pPiIO->piIn) * pPiCfg->piKp) >> (u8)(pPiCfg->piKpR));
    
    //========================I 调节===========================================
    //计算当前积分项
    temp32 = (s32)pPiIO->piIn * pPiCfg->piKi;
    temp16 = (s16)(temp32 >> (u8)(pPiCfg->piKiR));
    
     //修改当前积分项
     if(temp16 == 0)
    {
        if(temp32 > pPiCfg->piKiPrecision)
        {
            //当前积分项在(pPiCfg->piKiPrecision) 到2的(pPiCfg->piKiR)次方之间修正为1.
            temp16 = 1;
        }
    }
    else if(temp16 == -1)
    {
        if(temp32 >= -pPiCfg->piKiPrecision)
        {
            //当前积分项在0 到(-pPiCfg->piKiPrecision)之间修改为0
            temp16 = 0;
        }
    }
    else
    {
        //不用修改
    }
    
    //计算积分累加项
    tempPiSum = pPiIO->piSum + temp16;
    tempPiSum = PileUtilLimitRange(tempPiSum,pPiCfg->piSumMin,pPiCfg->piSumMax);
    
    //计算PI输出量
    tempPiOut  += (s16)tempPiSum;
    tempPiOut = (s16)PileUtilLimitRange(tempPiOut,pPiCfg->piOutMin,pPiCfg->piOutMax);
    
    //修改输出参数
    pPiIO->piSum = tempPiSum;
    pPiIO->piOut =  tempPiOut;
    
    return (0);
}

//=============================================================================================
//函数名称	: s32 PileUtilLimitRange(s32 value, s32 min, s32 max)
//输入参数	: value:输入值 min:最小范围值 max:最大范围值
//输出参数	: 输入值限制范围后的值
//函数功能	: 限制输入值范围
//注意事项	: 
//=============================================================================================
s32 PileUtilLimitRange(s32 value, s32 min, s32 max)
{
    if(value < min)
    {
        return(min);
    }
    
    if(value > max)
    {
        return(max);
    }
    
    return(value);
}

//=============================================================================================
//函数名称	: u16 PileUtilLimitRangeU16(u16 value, u16 min, u16 max)
//输入参数	: value:输入值 min:最小范围值 max:最大范围值
//输出参数	: 输入值限制范围后的值
//函数功能	: 限制输入值范围
//注意事项	: 
//=============================================================================================
u16 PileUtilLimitRangeU16(u16 value, u16 min, u16 max)
{
    if(value < min)
    {
        return(min);
    }
    
    if(value > max)
    {
        return(max);
    }
    
    return(value);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
