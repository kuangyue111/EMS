/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : DEVCAN.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : CAN通讯管理函数模块
** @Instructions :
**===========================================================================================*/
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "PublicPara.h"
#include "BSPCAN.h"
#include "DEVCAN.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gSysHigParaRO_101[];         /*系统硬件参数设置*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 DEVCANResetInit(e_CANNUM canNum)
** @Input     : canNum:CAN通道号
** @Output    : TRUE:成功 FALSE:失败
** @Function  : CAN口复位重新初始化(设置通信波特率)
** @The notes : 用户调用
**===========================================================================================*/
u8 DEVCANResetInit(e_CANNUM canNum)
{
    u16 bps = 0;
    u8 result = FALSE;

    switch(canNum)
    {
        case eCAN0:
            //CAN0口波特率
            bps = gSysHigParaRO_101[eSHWPara101_CAN0Rate];
            //result = BSPCANSetRate(eCAN0, CAN0_BPS);
            if(CAN0_BPS != bps)
            {
                result = BSPCANSetRate(eCAN0, bps);
            }
            break;
            
        case eCAN1:
            //CAN1口波特率
            bps = gSysHigParaRO_101[eSHWPara101_CAN1Rate];
            //result = BSPCANSetRate(eCAN1, CAN1_BPS);
            if(CAN1_BPS != bps)
            {
                result = BSPCANSetRate(eCAN1, bps);
            }
            break;
        
        case eCAN2:
            //CAN2口波特率
            bps = gSysHigParaRO_101[eSHWPara101_CAN2Rate];
            //result = BSPCANSetRate(eCAN2, CAN2_BPS);
            if(CAN2_BPS != bps)
            {
                result = BSPCANSetRate(eCAN2, bps);
            }
            break;
            
        default:
            result = FALSE;
            break;
    }
    
    return(result);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/

