/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileWorkState.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控工作状态任务模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicInfo.h"
#include "PileInfo.h"
#include "PileTactics.h"
#include "PileStartUp.h"
#include "PileTactInterface.h"
#include "PileWorkState.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
//static u32 sChgOrDhgCap = 0;          	//高低压时的充放电电电量
//static u8 sBattVoltState = 0;         	//电池组电压状态:0:正常 1:充电高压 2:放电低压

extern u16 gPSysStateInfo_50[];				/*系统状态信息数组*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
//=============================================================================================
//函数名称	: static void PileInitStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆初始化状态处理任务
//注意事项	: 开机自检,100ms
//=============================================================================================
static void PileInitStateHandle(void);

//=============================================================================================
//函数名称	: static void PileIdleStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆空闲状态处理任务
//注意事项	: 100ms
//=============================================================================================
static void PileIdleStateHandle(void);

//=============================================================================================
//函数名称	: static void PileStartStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆启动状态处理任务
//注意事项	: 10ms
//=============================================================================================
static void PileStartStateHandle(void);

//=============================================================================================
//函数名称	: static void PileRunStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆运行状态处理任务
//注意事项	: 100ms
//=============================================================================================
static void PileRunStateHandle(void);

//=============================================================================================
//函数名称	: static void PileStopStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆停机状态处理任务
//注意事项	: 100ms
//=============================================================================================
static void PileStopStateHandle(void);

//=============================================================================================
//函数名称	: static void PileErrStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆故障状态恢复处理任务
//注意事项	: 100ms
//=============================================================================================
static void PileErrStateHandle(void);

//=============================================================================================
//函数名称	: static void PileOffStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 系统关机状态处理任务
//注意事项	: 100ms
//=============================================================================================
static void PileOffStateHandle(void);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void void PileWorkStateCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 堆系统状态检测更新任务
** @The notes : 100ms
**===============================================================================================*/
void PileWorkStateCheckTask(void)
{
    u16 state = eWORK_INIT;
    static u8 sHisState = 0;
    
    state = gPSysStateInfo_50[ePStateInfo50_SysWor];

    /*外部控制电池组工作状态变换处理*/
    if(sHisState != state)
    {
    	//事件记录
    	//PileSysStateChangeDeal(sHisState, state);
    	sHisState = state;
    }

    /*根据不同工作状态执行相应处理任务*/
    switch(state)
    {
        case eWORK_INIT:
        	PileInitStateHandle();  //初始化状态处理任务
            break;
            
        case eWORK_IDLE:
        	PileIdleStateHandle();  //空闲状态处理任务
            break;
            
        case eWORK_START:
            //PileStartStateHandle();//启动状态处理任务(在10ms任务单独启动,此处不处理)
            break;
            
        case eWORK_RUN:
        	PileRunStateHandle();   //运行状态处理任务
            break;
            
        case eWORK_STOP:
        	PileStopStateHandle();  //停止状态处理任务
            break;
            
        case eWORK_ERR:
        	PileErrStateHandle();   //故障状态处理任务
            break;
            
        case eWORK_OFF:
        	PileOffStateHandle();   //关机状态处理任务
            break;
            
        default:
            break;
    }
    
    /*工作状态改变处理*/
    if(sHisState != gPSysStateInfo_50[ePStateInfo50_SysWor])
    {
    	//事件记录
        //PileSysStateChangeDeal(sHisState, gPSysStateInfo_50[ePStateInfo50_SysWor]);
    }
}

//=============================================================================================
//函数名称	: void PileWorkStartCheckTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 检测电池堆启动检测任务
//注意事项	: 10ms
//=============================================================================================
void PileWorkStartCheckTask(void)
{
    /*启动状态才执行*/
    if(gPSysStateInfo_50[ePStateInfo50_SysWor] == eWORK_START)
    {
    	//执行启动状态处理任务
    	PileStartStateHandle();
    }
}

//=============================================================================================
//函数名称	: void SetPileSysWorkStateAPI(e_WorkState state)
//输入参数	: state:工作状态[0开机初始化 1待机空闲 2启动 3运行 4停机 5故障 6关机]
//输出参数	: void
//函数功能	: 设置电池堆系统工作状态
//注意事项	: 
//=============================================================================================
void SetPileSysWorkStateAPI(e_WorkState state)
{
    if(state >= eWORK_STATE)
    {
        return;
    }
    
    gPSysStateInfo_50[ePStateInfo50_SysWor] = state;
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: static void PileInitStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆初始化状态处理任务
//注意事项	: 开机自检,100ms
//=============================================================================================
static void PileInitStateHandle(void)
{
    u8 state = 0;
    
    //清除启停控制故障
    TacClrPileCtrlErrFlagAPI();

    state = PileCheckInitStateResultHook();
    
    if(state == FunR_Fault)             //自检失败
    {
    	gPSysStateInfo_50[ePStateInfo50_SysWor] = eWORK_ERR;
    }
    else if(state == FunR_Ok)           //自检成功
    {
        //进入待机空闲状态
    	gPSysStateInfo_50[ePStateInfo50_SysWor] = eWORK_IDLE;
    }
    
    //开机自检状态处理
    //PileInitStateDealHook((u8)gPSysStateInfo_50[ePStateInfo50_SysWor]);
}

//=============================================================================================
//函数名称	: static void PileIdleStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆空闲状态处理任务
//注意事项	: 100ms
//=============================================================================================
static void PileIdleStateHandle(void)
{
    //待机自动切入故障状态
    if(PileCheckIdleStateResultHook() == FunR_Fault)
    {
    	gPSysStateInfo_50[ePStateInfo50_SysWor] = eWORK_ERR;
    }
    
    //待机空闲状态处理
    //PileIdleStateDealHook((u8)gPSysStateInfo_50[ePStateInfo50_SysWor]);
}

//=============================================================================================
//函数名称	: static void PileStartStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆启动状态处理任务
//注意事项	: 10ms
//=============================================================================================
static void PileStartStateHandle(void)
{
    if(PileCheckStartStateResultHook() == FunR_Fault)
    {
        gPSysStateInfo_50[ePStateInfo50_SysWor] = eWORK_ERR;
    }
    else
    {
        //堆启动电池组控制任务 
    	PileSysStartCtrlTask();
    }
    
    //启动状态处理
    //PileStartStateDealHook((u8)gPSysStateInfo_50[ePStateInfo50_SysWor]);
}

//=============================================================================================
//函数名称	: static void PileRunStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆运行状态处理任务
//注意事项	: 100ms
//=============================================================================================
static void PileRunStateHandle(void)
{
    u8 result = 0;

    //电池外部故障检测
    result = PileCheckRunStateResultHook();

    if(result == FunR_Fault)
    {
        gPSysStateInfo_50[ePStateInfo50_SysWor] = eWORK_ERR;
    }
    else if(result == FunR_Ok)
    {
        gPSysStateInfo_50[ePStateInfo50_SysWor] = eWORK_IDLE;
    }
    
    //运行状态处理
    //PileRunStateDealHook((u8)gPSysStateInfo_50[ePStateInfo50_SysWor]);
}

//=============================================================================================
//函数名称	: static void PileStopStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆停机状态处理任务
//注意事项	: 100ms
//=============================================================================================
static void PileStopStateHandle(void)
{
    if(PileCheckStopStateResultHook() == FunR_Fault)
    {
        gPSysStateInfo_50[ePStateInfo50_SysWor] = eWORK_ERR;
    }
    else
    {
        //堆停机电池组控制任务 
    	PileSysNorStopCtrlTask();
    }
    
    //停机状态处理
    //PileStopStateDealHook((u8)gPSysStateInfo_50[ePStateInfo50_SysWor]);
}

//=============================================================================================
//函数名称	: static void PileErrStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆故障状态恢复处理任务
//注意事项	: 100ms
//=============================================================================================
static void PileErrStateHandle(void)
{
    //故障状态下停止电池堆
	PileSysErrStopCtrlTask();
    
    if(PileCheckErrStateResultHook() == FunR_Ok)
    {
        gPSysStateInfo_50[ePStateInfo50_SysWor] = eWORK_IDLE;
    }
    
    //故障状态处理
    //PileErrStateDealHook((u8)gPSysStateInfo_50[ePStateInfo50_SysWor]);
}

//=============================================================================================
//函数名称	: static void PileOffStateHandle(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 系统关机状态处理任务
//注意事项	: 100ms
//=============================================================================================
static void PileOffStateHandle(void)
{
    u8 result = 0;
    
    //电池外部故障检测
    result = PileCheckOffStateResultHook();
    
    if(FunR_Ok == result)                           //检测完成(需要开机)
    {
        gPSysStateInfo_50[ePStateInfo50_SysWor] = eWORK_INIT;//初始化状态
    }
    else                                            //关机保持
    {
    	PileSysPwrOffCtrlTask();                    //系统关机控制
    }

    //关机状态处理
    //PileOffStateDealHook((u8)gPSysStateInfo_50[ePStateInfo50_SysWor]);
}

#endif
