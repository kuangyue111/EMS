/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : VoltLimCalc.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 总压限值计算模块
** @Instructions : 对可调压充放电设备有效
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "CalcUtil.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "CalcInterface.h"
#include "TacInterface.h"
#include "CurrLimCalc.h"
#include "VoltLimCalc.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gGLimVInfo_52[];				/*电压限值信息数组*/
extern u16 gGBmuHigLevPara_103[];  		/*主控高级参数值*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void VoltLimCalcTask(void)
** @Input     : void
** @Output    : void
** @Function  : 充放电电压限值计算任务
** @The notes : 1S
**===========================================================================================*/
void VoltLimCalcTask(void)
{
    /*总电压限值调节使能*/
    #if(1 == VOLT_LIM_AJUST_EN)
    static u8 sChgTime = 0;
    static u8 sDhgTime = 0;
    
    if((GetGWorkStateAPI() == eWORK_INIT)           /*系统初始化*/
        || (GetGWorkStateAPI() == eWORK_OFF)        /*系统关机*/
        || (1 == CalcGetGroupCellVExpStateHook())   /*单体异常*/
        || (GetGCellSumVoltAPI() < gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV]))/*总电压异常*/
    {
        /*恒压0V代表需PCS停机*/
    	gGLimVInfo_52[eLimVInfo52_ChgV] = 0;
    	gGLimVInfo_52[eLimVInfo52_DhgV] = 0;        /*电压限值为0*/
    }
    else if(eWORK_RUN != GetGWorkStateAPI())        /*系统未运行*/
    {
        /*按总电压±1V恒压*/
    	gGLimVInfo_52[eLimVInfo52_ChgV] = GetGCellSumVoltAPI() + 10;
    	gGLimVInfo_52[eLimVInfo52_DhgV] = GetGCellSumVoltAPI() - 10;
    }
    else                                           /*系统运行则正常计算调节*/
    {
        /*允许充电*/
        if(GetGChgLimCurrAPI() > 0)
        {
            /*未开始降电流*/
            if((0xff == GetFallChgCIndexStepAPI()) || (GetGChgLimCurrAPI() >= 100))
            {
                /*执行均充最大电压*/
            	gGLimVInfo_52[eLimVInfo52_ChgV] = CalcGetChgLimMaxSumVHook();
            }
            /*到达高压开始降电流*/
            else
            {
                /*总压比均充电压大*/
                if((GetGCellSumVoltAPI() + 10) > CalcGetChgLimMaxSumVHook())
                {
                    /*按均充电压恒压*/
                	gGLimVInfo_52[eLimVInfo52_ChgV] = CalcGetChgLimMaxSumVHook();
                }
                /*总电压未到达均衡最大电压*/
                else
                {
                    /*间隔60s检测一次是否上提恒压目标值*/
                    if(sChgTime >= 60)
                    {
                        /*充电电流降到2A则上提1V恒压目标值*/
                        if(GetGSampOutCurrAPI() >= -20)
                        {
                        	gGLimVInfo_52[eLimVInfo52_ChgV] = GetGCellSumVoltAPI() + 10;
                        }
                        sChgTime = 0;
                    }
                    else
                    {
                        sChgTime++;
                    }
                }
            }
        }
        /*已不允许充电*/
        else
        {
            /*电池未充满*/
            if(1 != TacGetGroupBattStateAPI())
            {
                /*按照当前总电压恒压*/
            	gGLimVInfo_52[eLimVInfo52_ChgV] = GetGCellSumVoltAPI();
            }
            /*电池已经充满*/
            else
            {
                /*电池电压过高或还在充电*/
                if((GetGCellMaxVoltAPI() >= CalcGetChgLimCellVoltHook())
                    || (GetGSampOutCurrAPI() <= -5))
                {
                    /*间隔60s下降0.1V恒压目标值*/
                    if(sChgTime >= 60)
                    {
                        /*充电电流大于5A则下降0.5V*/
                        if(GetGSampOutCurrAPI() <= -50)
                        {
                            if(gGLimVInfo_52[eLimVInfo52_ChgV] > 5)
                            {
                            	gGLimVInfo_52[eLimVInfo52_ChgV] = gGLimVInfo_52[eLimVInfo52_ChgV] - 5;
                            }
                        }
                        /*充电电流较小则下降0.1V*/
                        else
                        {
                            if(gGLimVInfo_52[eLimVInfo52_ChgV] > 1)
                            {
                            	gGLimVInfo_52[eLimVInfo52_ChgV] = gGLimVInfo_52[eLimVInfo52_ChgV] - 1;
						    }
                        }
                        sChgTime = 0;
                    }
                    else
                    {
                        sChgTime++;
                    }
                }
            }
        }
            
        /*允许放电*/
        if(GetGDhgLimCurrAPI() > 0)
        {
        	/*未开始降电流*/
        	if((0xff == GetFallDhgCIndexStepAPI()) || (GetGDhgLimCurrAPI() >= 100))
            {
                /*执行放电最小电压限值*/
            	gGLimVInfo_52[eLimVInfo52_DhgV] = CalcGetDhgLimMinSumVHook();
            }
            /*已低压开始降电流*/
            else
            {
                /*总压比放电目标电压小*/
                if((GetGCellSumVoltAPI() - 10) < CalcGetDhgLimMinSumVHook())
                {
                    /*按放电目标电压恒压*/
                	gGLimVInfo_52[eLimVInfo52_DhgV] = CalcGetDhgLimMinSumVHook();
                }
                /*总电压未到达放电最低电压*/
                else
                {
                    /*间隔60s检测一次是否下降恒压目标值*/
                    if(sDhgTime >= 60)
                    {
                        /*放电电流降到2A则下降1V恒压目标值*/
                        if(GetGSampOutCurrAPI() <= 20)
                        {
                        	gGLimVInfo_52[eLimVInfo52_DhgV] = GetGCellSumVoltAPI() - 10;
                        }
                        sDhgTime = 0;
                    }
                    else
                    {
                        sDhgTime++;
                    }
                }
            }
        }
        /*已不允许放电*/
        else
        {
            /*电池未放空*/
            if(2 != TacGetGroupBattStateAPI())
            {
                /*按照当前总电压恒压*/
            	gGLimVInfo_52[eLimVInfo52_DhgV] = GetGCellSumVoltAPI();
            }
            /*电池已经放空*/
            else
            {
                /*电池电压低高或还在放电*/
                if((GetGCellMinVoltAPI() <= CalcGetDhgLimCellVoltHook())
                    || (GetGSampOutCurrAPI() >= 5))
                {
                    /*间隔60上升0.1V恒压目标值*/
                    if(sDhgTime >= 60)
                    {
                        /*放电电流大于5A则下降0.5V*/
                        if(GetGSampOutCurrAPI() >= 50)
                        {
                            if(gGLimVInfo_52[eLimVInfo52_DhgV] > 5)
                            {
                            	gGLimVInfo_52[eLimVInfo52_DhgV] = gGLimVInfo_52[eLimVInfo52_DhgV] - 5;
                            }
                        }
                        /*放电电流较小则下降0.1V*/
                        else
                        {
                            if(gGLimVInfo_52[eLimVInfo52_DhgV] > 1)
                            {
                            	gGLimVInfo_52[eLimVInfo52_DhgV] = gGLimVInfo_52[eLimVInfo52_DhgV] - 1;
						    }
                        }
                        sDhgTime = 0;
                    }
                    else
                    {
                        sDhgTime++;
                    }
                }
            }
        }
    }
    /*总电压限值调节禁止*/
    #else
    /*计算电压限值*/
    if((GetSysWorkStateAPI() == eWORK_INIT)         /*系统初始化*/
        || (GetSysWorkStateAPI() == eWORK_ERR))     /*系统故障*/
    {
    	gGLimVInfo_52[eLimVInfo52_ChgV] = 0;
        gGLimVInfo_52[eLimVInfo52_DhgV] = 0;        /*电压限值为0*/
    }
    else                                            /*系统正常则正常计算*/
    {
        /*取电池组总压高低压限值*/
    	gGLimVInfo_52[eLimVInfo52_ChgV] = CalcGetChgLimVH2SumVolt();
        gGLimVInfo_52[eLimVInfo52_DhgV] = CalcGetDhgLimVL2SumVolt();
    }
    #endif
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/

