/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPMCG.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : CRG驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "clockMan1.h"
#include "clock_manager.h"
#include "BSPMCG.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern uint32_t SystemCoreClock;

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 BSPCRGInit(u8 oscClock, u8 busClock)
** @Input     : oscClock:晶振时钟(M) busClock:总线时钟(M)
** @Output    : void
** @Function  : 初始化结果 TRUE:成功 FALSE:失败
** @The notes : 初始化MCG始终频率
**===========================================================================================*/
u8 BSPCRGInit(u8 oscClock, u8 busClock)
{
	(void)oscClock;
	(void)busClock;

	CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
					g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);

	CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);
	SystemCoreClockUpdate();      /*时钟更新*/

	return(TRUE);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
