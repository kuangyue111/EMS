/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPDOG.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 软件看门狗驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPConfig.h"
#include "BSPTypeDef.h"
#include "wdg_pal1.h"
#include "BSPWDOG.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;

static u8 sWatchDogCycle = 0;    /*喂狗周期(0.1s)*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPWatchDogInit(u8 cycle)
** @Input     : cycle:喂狗周期[0.1s]
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : 看门狗模块初始化
** @The notes :
**===========================================================================================*/
u8 BSPWatchDogInit(u8 cycle)
{
    //u32 winTime = 0;
    
    /*模块未使能不进行初始化*/
    if(1 != gBSPFunCfg.Bits.WDOGEn)
    {
        return(FALSE);
    }

    /*BSP定时喂狗周期(100ms)*/
    sWatchDogCycle = cycle * 100;

    /*计算喂狗窗口时间*/
    //wdg_pal1_Config0.percentWindow = 100;
    WDG_SetWindow(&wdg_pal1_Instance, true, 100);

    /* Initialize WDOG module */
    WDG_Init(&wdg_pal1_Instance, &wdg_pal1_Config0);/* Use the LPO clock as source */
											  /* WDOG not functional in Wait/Debug/Stop mode */
    										  /* Enable further updates of the WDOG configuration */
											  /* Timeout interrupt disabled */
											  /* Window mode disabled */
											  /* Window value */
											  /* Timeout value */
											  /* Prescaler disabled */

    /* Refreshes the WDOG counter */
    WDG_Refresh(&wdg_pal1_Instance);

    return(TRUE);
}

/*=============================================================================================
** @Name      : void BSPWatchDogTask(void)
** @Input     : void
** @Output    : void
** @Function  : 100ms定时,BSP调用
** @The notes :
**===========================================================================================*/
void BSPWatchDogTask(void)
{
    static u8 sTime = 0;
    
    /*内部看门狗未初始化*/
    if((0 == sWatchDogCycle) || (1 != gBSPFunCfg.Bits.WDOGEn))
    {
        return;
    }

    sTime++;
    if(sTime >= sWatchDogCycle)       	  /*达到周期*/
    {
    	WDG_Refresh(&wdg_pal1_Instance);  /*硬件喂狗一次*/
        sTime = 0;
    }
}

/*=============================================================================================
** @Name      : void BSPWatchDogOutput(void)
** @Input     : void
** @Output    : void
** @Function  : 直接触发喂狗
** @The notes : 上层用户需要时调用,用户执行较长时间处理的函数时必须调用
**===========================================================================================*/
void BSPWatchDogOutput(void)
{
    /*内部看门狗未初始化*/
    if((0 == sWatchDogCycle) || (1 != gBSPFunCfg.Bits.WDOGEn))
    {
        return;
    }
    /*软件喂狗*/
    WDG_Refresh(&wdg_pal1_Instance);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
