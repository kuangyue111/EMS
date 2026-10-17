/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : EnerUser.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 电能量计算模块用户函数
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "GroupInfo.h"
#include "CapInfoCalc.h"
#include "EnerInfoCalc.h"
#include "CellFadeCalc.h"
#include "SocDisplay.h"
#include "SoeDisplay.h"
#include "SohDisplay.h"
#include "EnerUser.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gGSysCapInfo_61[];				/*系统电量信息数组*/
extern u16 gGStatistCapEner_72[];	        /*当前能量统计信息*/

extern u16 gGBmuGenPara_102[];  			/*主控通用参数值*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void EnerCapCalcAllInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电能量计算模块总初始化
** @The notes :
**===========================================================================================*/
void EnerCapCalcAllInit(void)
{
	/*SOC/SOE/SOH基础计算初始化*/
	GroupCapSocInit();
	GroupEnerSoeInit();
	GroupFadeSohInit();

	/*SOC/SOE/SOH显示初始化*/
	GroupSocDisplayInit();
	GroupSoeDisplayInit();
	GroupSohDisplayInit();
}

/*=============================================================================================
** @Name      : void GroupEnerDisplayTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电能量计算值显示任务
** @The notes : 100ms
**===========================================================================================*/
void GroupEnerDisplayTask(void)
{
	/*SOC/SOE/SOH显示处理*/
	GroupSocDisplayTask();
	GroupSoeDisplayTask();
	GroupSohDisplayTask();

	/*电能电量显示处理*/
	gGSysCapInfo_61[eSysCapInfo61_AllCap] = (u16)(GetGroupTotalCapAPI() / 100);
    gGStatistCapEner_72[eCapEner72_NowEner] = (u16)(GetGroupNowEnerAPI() / 100);
    gGStatistCapEner_72[eCapEner72_AllEner] = (u16)(GetGroupTotalEnerAPI() / 100);

}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/

