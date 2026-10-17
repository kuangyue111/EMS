/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileAutoCtrl.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控动态投切控制任务模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicPara.h"
#include "PilePara.h"
#include "PileInfo.h"
#include "PublicInfo.h"
#include "PileGroupRcv.h"
#include "PileSysState.h"
#include "PileLimCalc.h"
#include "CalcUtil.h"
#include "PileTactics.h"
#include "PileTactInterface.h"
#include "PileAutoCtrl.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
u16 gBsuAutoVoltPara[AUTO_RUN_VOLT_TAB_LEN] =   //动态接入检测电压参数信息
{
    2800,                           //运行组所在电压点1[2000,5000]mV
    2900,                           //运行组所在电压点2[2000,5000]mV
    3000,                           //运行组所在电压点3[2000,5000]mV
    3100,                           //运行组所在电压点4[2000,5000]mV
    3200,                           //运行组所在电压点5[2000,5000]mV
    3300,                           //运行组所在电压点6[2000,5000]mV
    3400,                           //运行组所在电压点7[2000,5000]mV
    3500,                           //运行组所在电压点8[2000,5000]mV
    3600                            //运行组所在电压点9[2000,5000]mV
};

u16 gBsuAutoCurrPara[AUTO_RUN_CURR_TAB_LEN] =   //动态接入检测电流参数信息
{
    50,                             //运行组所在电流点1[10,5000]A
    100,                            //运行组所在电流点2[10,5000]A
    200,                            //运行组所在电流点3[10,5000]A
    300,                            //运行组所在电流点4[10,5000]A
    400,                            //运行组所在电流点5[10,5000]A
    550,                            //运行组所在电流点6[10,5000]A
    700,                            //运行组所在电流点7[10,5000]A
    850,                            //运行组所在电流点8[10,5000]A
    1000                            //运行组所在电流点9[10,5000]A
};

u16 gBsuAutoChgSubVolt[AUTO_RUN_VOLT_TAB_LEN][AUTO_RUN_CURR_TAB_LEN] =  //动态接入充电电压偏移参数配置信息
{
{  30, 35, 45, 55, 60, 65, 70, 75, 80},    //2800V以下不同充电电流产生的浮充电压[mV]
{   25, 30, 40, 50, 55, 60, 65, 70, 75},    //2900V以下不同充电电流产生的浮充电压[mV]
{   20, 25, 35, 45, 50, 55, 60, 65, 70},    //3000V以下不同充电电流产生的浮充电压[mV]
{   16, 21, 31, 41, 46, 51, 56, 61, 66},    //3100V以下不同充电电流产生的浮充电压[mV]
{   13, 18, 28, 38, 43, 48, 53, 58, 63},    //3200V以下不同充电电流产生的浮充电压[mV]
{   11, 16, 26, 36, 41, 46, 51, 56, 61},    //3300V以下不同充电电流产生的浮充电压[mV]
{   14, 19, 29, 39, 44, 49, 54, 59, 64},    //3400V以下不同充电电流产生的浮充电压[mV]
{   18, 23, 33, 43, 47, 53, 58, 63, 68},    //3500V以下不同充电电流产生的浮充电压[mV]
{   23, 28, 38, 48, 52, 58, 63, 68, 73}     //3600V以下不同充电电流产生的浮充电压[mV]
};

u16 gBsuAutoDhgSubVolt[AUTO_RUN_VOLT_TAB_LEN][AUTO_RUN_CURR_TAB_LEN] =  //动态接入放电电压偏移参数配置信息
{
{   25, 30, 40, 50, 55, 60, 65, 70, 75},    //2800V以下不同放电电流产生的压降电压[mV]
{   20, 25, 35, 45, 50, 55, 60, 65, 70},    //2900V以下不同放电电流产生的压降电压[mV]
{   16, 21, 31, 41, 46, 51, 56, 61, 66},    //3000V以下不同放电电流产生的压降电压[mV]
{   13, 18, 28, 38, 43, 48, 53, 58, 63},    //3000V以下不同放电电流产生的压降电压[mV]
{   11, 16, 26, 36, 41, 46, 51, 56, 61},    //3200V以下不同放电电流产生的压降电压[mV]
{   14, 19, 29, 39, 44, 49, 54, 59, 64},    //3300V以下不同放电电流产生的压降电压[mV]
{   18, 23, 33, 43, 47, 53, 58, 63, 68},    //3400V以下不同放电电流产生的压降电压[mV]
{   23, 28, 38, 48, 52, 58, 63, 68, 73},    //3500V以下不同放电电流产生的压降电压[mV]
{   28, 33, 43, 53, 57, 63, 68, 73, 78}     //3600V以下不同放电电流产生的压降电压[mV]
};

static u8 gPileParErrFlag = 0;              //系统动态接入异常标志
static u8 gPileSepErrFlag = 0;              //系统动态切除异常标志
static u16 gPileChgSepGroup = 0;            //系统充电限制切除的电池组
static u16 gPileDhgSepGroup = 0;            //系统放电限制切除的电池组
static u16 gPileErrSepGroup = 0;            //系统放电限制切除的电池组

extern t_SYSCTRL_INFO gPileCtrlInfo;        //系统启动停机控制信息

extern u16 gSysGenParaRO_100[];  			/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];  			/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
//=============================================================================================
//函数名称	: static u16 SelectCanAutoRunGroup(void)
//输入参数	: void  
//输出参数	: 按位表示可以自动接入的电池组[0:没有自动并网组 1:可自动并网的组号]
//函数功能	: 获取可自动接入的电池组 
//注意事项	: 
//=============================================================================================
static u16 SelectCanAutoRunGroup(void);

//=============================================================================================
//函数名称	: static u16 CheckIdleCanAutoRunGroup(void) 
//输入参数	: void 
//输出参数	: 0:无可接入组 其他:按位表示可动态接入电池组
//函数功能	: 检测系统空闲状态下计算有无可动态接入电池组 
//注意事项	: 
//=============================================================================================
static u16 CheckIdleCanAutoRunGroup(void);

//=============================================================================================
//函数名称	: static u8 CheckSysIdleGroupFitAutoRunState(u8 group)
//输入参数	: group:需接入运行组
//输出参数	: 1:适合运行 0:不适合运行
//函数功能	: 系统空闲时判断目标电池组是否适合自动投入运行
//注意事项	: 判断该组与运行组的压差
//=============================================================================================
static u8 CheckSysIdleGroupFitAutoRunState(u8 group);

//=============================================================================================
//函数名称	: static u16 CheckChgCanAutoRunGroup(void) 
//输入参数	: void 
//输出参数	: 0:无可接入组 其他:按位表示可动态接入电池组
//函数功能	: 检测系统充电状态下计算有无可动态接入电池组 
//注意事项	: 
//=============================================================================================
static u16 CheckChgCanAutoRunGroup(void);

//=============================================================================================
//函数名称	: static u8 CheckSysChgGroupFitAutoRunState(u8 group)
//输入参数	: 需接入运行组
//输出参数	: 1:适合运行 0:不适合运行
//函数功能	: 系统充电时判断目标电池组是否适合自动投入运行
//注意事项	: 判断该组与运行组的压差
//=============================================================================================
static u8 CheckSysChgGroupFitAutoRunState(u8 group);

//=============================================================================================
//函数名称	: static u16 CheckDhgCanAutoRunGroup(void) 
//输入参数	: void 
//输出参数	: 0:无可接入组 其他:按位表示可动态接入电池组
//函数功能	: 检测系统放电状态下计算有无可动态接入电池组 
//注意事项	: 
//=============================================================================================
static u16 CheckDhgCanAutoRunGroup(void);

//=============================================================================================
//函数名称	: static u8 CheckSysDhgGroupFitAutoRunState(u8 group)
//输入参数	: 需接入运行组
//输出参数	: 1:适合运行 0:不适合运行
//函数功能	: 系统放电时判断目标电池组是否适合自动投入运行
//注意事项	: 判断该组与运行组的压差
//=============================================================================================
static u8 CheckSysDhgGroupFitAutoRunState(u8 group);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void PileAutoCtrlGroupTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆控制电池组动态投切任务
//注意事项	: 100ms
//=============================================================================================
void PileAutoCtrlGroupTask(void)
{
    u8 i = 0;
    u8 group = 0;
    u8 sepGroup = 0xff;
    u8 sepReason = 0xff;
    static u8 sState = 0;
    static u8 sCtrlGroup = 0xff;
    static u8 sErrTime = 0;
    static u16 sFitGroup = 0;
    
    switch(sState)
    {
        //判断有无电池组需要切除
        case 0:
            if((eWORK_RUN == GetPileWorkStateAPI())         //电池堆运行
                && (GetPileRunGroupNumAPI() > 0))           //有正常运行电池组
            {
                sErrTime = 0;
                sCtrlGroup = 0xff;
                
                //清除正在运行的电池组切除记录
                gPileChgSepGroup = gPileChgSepGroup & (~GetPAllGroupRunStateAPI());
                gPileDhgSepGroup = gPileDhgSepGroup & (~GetPAllGroupRunStateAPI());
                gPileErrSepGroup = gPileErrSepGroup & (~GetPAllGroupRunStateAPI());
                
                if(GetPileSepGroupNumAPI() > 0)             //有电池组需要切除
                {
                    sState = 1;                             //进入检测是否需要执行切除
                }
                else                                        //无电池组需要切除
                {
                    sState = 10;                            //进入动态接入检测
                }
            }
            else
            {
                gPileChgSepGroup = 0;
                gPileDhgSepGroup = 0;
            }
            break;
            
        //根据当前电池保护状态判断是否执行切除
        case 1:
            for(i = 0; i < GetPileSepGroupNumAPI(); i++)    //检测需要切除的电池组
            {
                group = GetPileSepGroupIdAPI(i);
                
                //电池组未激活-必须切除
                if(eSEP_REASON_DIS == GetPileSepGroupReasonAPI(i)) //电池组未激活使能
                {
                    sepGroup = group;
                    sepReason = GetPileSepGroupReasonAPI(i);
                    break;
                }
                //电池组故障-故障切除激活且可以切除才切除
                else if((eSEP_REASON_OFF == GetPileSepGroupReasonAPI(i))//电池组通讯断线
                    || (eSEP_REASON_ERR == GetPileSepGroupReasonAPI(i)) //电池组故障
                    || (eSEP_REASON_ISO == GetPileSepGroupReasonAPI(i)))//电池组断路器断开
                {
                    //故障切除使能且当前剩余正常组数满足最小组数
                    if((1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
                        && (GetPileRunGroupNumAPI() >= gSysGenParaRO_100[eSGenPara100_MinGNum]))
                    {
                        sepGroup = group;
                        sepReason = GetPileSepGroupReasonAPI(i);
                        break;
                    }
                }
                //电池组充放电限制-故障切除激活且无其他故障电池组切除才切除
                else if(eSEP_REASON_SYSLIM == GetPileSepGroupReasonAPI(i))//电池组充放电限制
                {
                    //系统非充放电限制且至少1组可以切除
                    if((ePGPRO_STA_SYSPRO != GetPileAllGProStateAPI())
                        && (ePGPRO_STA_SYSLIM != GetPileAllGProStateAPI())
                        && (1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
                        && ((gSysGenParaRO_100[eSGenPara100_MinGNum] + 1) <= GetPileRunGroupNumAPI()))
                    {
                        sepGroup = group;
                        sepReason = GetPileSepGroupReasonAPI(i);
                    }
                }
                //电池组充电限制-故障切除激活且系统充电才切除
                else if(eSEP_REASON_CHGLIM == GetPileSepGroupReasonAPI(i)) //电池组充电限制
                {
                    //系统充电且至少1组可以切除
                    if((eCURR_CHG == GetPileChgStateAPI())
                        && (ePGPRO_STA_SYSPRO != GetPileAllGProStateAPI())
                        && (ePGPRO_STA_SYSLIM != GetPileAllGProStateAPI())
                        && (ePGPRO_STA_CHGLIM != GetPileAllGProStateAPI())
                        && (1 == gSysGenParaRO_100[eSGenPara100_ChgSepa])
                        && ((gSysGenParaRO_100[eSGenPara100_MinGNum] + 1) <= GetPileRunGroupNumAPI()))
                    {
                        //无其他更严重电池组需要切除
                        if((0xff == sepGroup)
                            || (sepReason > eSEP_REASON_SYSLIM))
                        {
                            sepGroup = group;
                            sepReason = GetPileSepGroupReasonAPI(i);
                        }
                    }
                }
                //电池组充电限制-故障切除激活且系统充电才切除
                else if(eSEP_REASON_DHGLIM == GetPileSepGroupReasonAPI(i)) //电池组放电限制
                {
                    //系统放电且至少1组可以切除
                    if((eCURR_DHG == GetPileChgStateAPI())
                        && (ePGPRO_STA_SYSPRO != GetPileAllGProStateAPI())
                        && (ePGPRO_STA_SYSLIM != GetPileAllGProStateAPI())
                        && (ePGPRO_STA_CHGLIM != GetPileAllGProStateAPI())
                        && (1 == gSysGenParaRO_100[eSGenPara100_DhgSepa])
                        && ((gSysGenParaRO_100[eSGenPara100_MinGNum] + 1) <= GetPileRunGroupNumAPI()))
                    {
                        //无其他更严重电池组需要切除
                        if((0xff == sepGroup)
                            || (sepReason > eSEP_REASON_SYSLIM))
                        {
                            sepGroup = group;
                            sepReason = GetPileSepGroupReasonAPI(i);
                        }
                    }
                }
            }
            
            //选取到需切除的电池组
            if(0xff != sepGroup)
            {
                //记录需要切除的电池组号
                sCtrlGroup = sepGroup;
                
                //电池组电流不安全且激活使能切除需要降功率
                if((GetPGroupOutCurrAPI(sCtrlGroup) > gPSmuGenParaR_102[eSmuGenPara102_AutSafeC])
                    || (GetPGroupOutCurrAPI(sCtrlGroup) < (0 - (s16)gPSmuGenParaR_102[eSmuGenPara102_AutSafeC])))
                {
                    //设置请求降到目标安全电流
                    SetExtReqPileSafeLimCAPI(gPSmuGenParaR_102[eSmuGenPara102_AutSafeC]);
                    sState = 2;
                }
                //不需要降电流
                else
                {
                    sState = 3;
                }
            }
            //没找到需切除的电池组
            else
            {
                sState = 10;
            }
            break;
            
        //检测电流是否安全
        case 2:
            //电池组电流安全
            if((GetPGroupOutCurrAPI(sCtrlGroup) <= gPSmuGenParaR_102[eSmuGenPara102_AutSafeC])
                && (GetPGroupOutCurrAPI(sCtrlGroup) >= (0 - (s16)gPSmuGenParaR_102[eSmuGenPara102_AutSafeC])))
            {
                sErrTime = 0;
                sState = 3;
            }
            //电池组电流不安全
            else
            {
                //设置请求降到目标安全电流
                SetExtReqPileSafeLimCAPI(gPSmuGenParaR_102[eSmuGenPara102_AutSafeC]);
                
                sErrTime++;
                //PCS响应超时
                if(sErrTime >= 50)
                {
                    //设置切除失败标志
                    gPileSepErrFlag = 1;
                    sState = 0;
                }
            }
            break;
            
        //发送命令将电池组切除
        case 3:
            //电池组未切除
            if((eWORK_RUN == GetPGroupWorkStateAPI(sCtrlGroup))
                || (FALSE == GetPGroupRunSwhAllOffStateAPI(sCtrlGroup)))
            {
                //未超时
                if(sErrTime < gPSmuGenParaR_102[eSmuGenPara102_DnSwhExpT])
                {
                    //电池组充电
                    if((GetPGroupOutCurrAPI(sCtrlGroup) < -10)
                        || (1 == GetPGroupChgStateAPI(sCtrlGroup)))
                    {
                        //发送正常停机控制命令
                    	PileSetGroupWorkStateCtrlCmdHook(sCtrlGroup, eCTRL_NORSTOP);
                    }
                    //电池组电流不安全
                    else
                    {
                        //发送强制停机控制命令
                    	PileSetGroupWorkStateCtrlCmdHook(sCtrlGroup, eCTRL_FORCSTOP);
                    }
                    sErrTime++;
                }
                //控制超时
                else
                {
                    //设置切除失败标志
                    BitSet(gPileCtrlInfo.CtrlFailGroup, sCtrlGroup);
                    gPileSepErrFlag = 1;
                    sErrTime = 0;
                    sState = 0;
                }
            }
            //电池组成功切除
            else
            {
                //记录电池组切除类别
                if((eSEP_REASON_OFF == sepReason)
                    || (eSEP_REASON_ERR == sepReason))
                {
                    BitSet(gPileErrSepGroup, sCtrlGroup);
                }
                else if((eSEP_REASON_ISO == sepReason)
                    || (eSEP_REASON_SYSLIM == sepReason))
                {
                    BitSet(gPileChgSepGroup, sCtrlGroup);
                    BitSet(gPileDhgSepGroup, sCtrlGroup);
                }
                else if(eSEP_REASON_CHGLIM == sepReason)
                {
                    BitSet(gPileChgSepGroup, sCtrlGroup);
                }
                else if(eSEP_REASON_DHGLIM == sepReason)
                {
                    BitSet(gPileDhgSepGroup, sCtrlGroup);
                }
                
                //更新电池组状态信息
                PileAllGStateCheckTask();
                
                //清除动态投切目标安全电流
                SetExtReqPileSafeLimCAPI(0xffff);
                
                //进入动态接入检测
                sErrTime = 0;
                sState = 10;
            }
            break;
        
        //判断是否需要动态接入
        case 10:
            //有电池组未运行且激活动态接入
            if((GetPileRunGroupNumAPI() > 0)
                && (GetPileNorGroupNumAPI() > GetPileRunGroupNumAPI())
                && (1 == gSysGenParaRO_100[eSGenPara100_AutConn]))
            {
                sCtrlGroup = 0xff;
                sErrTime = 0;
                sState = 11;
            }
            //不需动态接入
            else
            {
                sErrTime = 0;
                sState = 0;
            }
            break;
            
        //检测有无电池组需要接入
        case 11:
            //查找适合接入的电池组
            sFitGroup |= SelectCanAutoRunGroup();
            
            //有电池组适合动态接入
            if(sFitGroup > 0)
            {
                //找出需要接入的电池组
                for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
                {
                    //该电池组适合接入
                    if(1 ==  BitGet(sFitGroup, i))
                    {
                        sCtrlGroup = i;
                        break;
                    }
                }
                
                //没找到电池组
                if(i >= gSysGenParaRO_100[eSGenPara100_AllGNum])
                {
                    sState = 0;
                }
                //找到电池需要接入
                else
                {
                    //电池堆电流不安全且激活使能切除需要降功率
                    if((GetPileOutSumCurrAPI() > (gPSmuGenParaR_102[eSmuGenPara102_AutSafeC] * GetPileRunGroupNumAPI()))
                        || (GetPileOutSumCurrAPI() < ((0 - (s16)gPSmuGenParaR_102[eSmuGenPara102_AutSafeC] * GetPileRunGroupNumAPI()))))
                    {
                        //设置请求降到目标安全电流
                        SetExtReqPileSafeLimCAPI(gPSmuGenParaR_102[eSmuGenPara102_AutSafeC]);
                        sState = 12;
                    }
                    //不需要降电流
                    else
                    {
                        sState = 13;
                    }
                }
            }
            //没有电池组适合动态接入
            else
            {
                sState = 0;
            }
            sErrTime = 0;
            break;
            
        //检测电流是否安全
        case 12:
            //电池堆动态接入电流安全
            if((GetPileOutSumCurrAPI() <= (gPSmuGenParaR_102[eSmuGenPara102_AutSafeC] * GetPileRunGroupNumAPI()))
                && (GetPileOutSumCurrAPI() >= ((0 - (s16)gPSmuGenParaR_102[eSmuGenPara102_AutSafeC] * GetPileRunGroupNumAPI()))))
            {
                sErrTime = 0;
                sState = 13;
            }
            //电池组电流不安全
            else
            {
                //设置请求降到目标安全电流
                SetExtReqPileSafeLimCAPI(gPSmuGenParaR_102[eSmuGenPara102_AutSafeC]);
                
                sErrTime++;
                //PCS响应超时
                if(sErrTime >= 50)
                {
                    //返回故障切除检测
                    gPileParErrFlag = 1;
                    sErrTime = 0;
                    sState = 0;
                }
            }
            break;
            
        //控制接入和判断接入结果
        case 13:
            //动态接入成功
            if((eWORK_RUN == GetPGroupWorkStateAPI(sCtrlGroup))
                && (FALSE == GetPGroupRunSwhAllOffStateAPI(sCtrlGroup)))
            {
                //更新电池组状态信息
            	PileAllGStateCheckTask();
                
                //清除目标安全电流
                SetExtReqPileSafeLimCAPI(0xffff);
                
                BitClr(sFitGroup, sCtrlGroup);
                sErrTime = 0;
                sState = 0;
            }
            //未动态接入
            else
            {
                //控制超时
                if(sErrTime >= gPSmuGenParaR_102[eSmuGenPara102_UpSwhExpT])
                {
                    //设置切除失败标志,不再控制这组电池组
                    BitSet(gPileCtrlInfo.CtrlFailGroup, sCtrlGroup);
                    BitClr(sFitGroup, sCtrlGroup);
                    gPileParErrFlag = 1;
                    sErrTime = 0;
                    sState = 0;
                }
                //控制未超时
                else
                {
                    //系统充电
                    if((GetPileOutSumCurrAPI() < -10)
                        || (1 == GetPileChgStateAPI()))
                    {
                        //发送充电启动控制命令
                    	PileSetGroupWorkStateCtrlCmdHook(sCtrlGroup, eCTRL_CHGSTART);
                    }
                    //系统非充电
                    else
                    {
                        //发送放电启动控制命令
                    	PileSetGroupWorkStateCtrlCmdHook(sCtrlGroup, eCTRL_DHGSTART);
                    }
                    
                    //控制计时
                    sErrTime++;
                }
            }
            break;
            
        default:
            break;
    }
}

//=============================================================================================
//函数名称	: void ClrPileAutoRunErrFlag(void)
//输入参数	: void  
//输出参数	: void
//函数功能	: 清除电池堆动态投切异常标志
//注意事项	: 
//=============================================================================================
void ClrPileAutoRunErrFlag(void)
{
    gPileParErrFlag = 0;
    gPileSepErrFlag = 0;
    gPileChgSepGroup = 0;
    gPileDhgSepGroup = 0;
    gPileErrSepGroup = 0;
}

//=============================================================================================
//函数名称	: u16 GetPileChgSepGroup(void)
//输入参数	: void  
//输出参数	: 电池堆充电限制切除的电池组(按位表示)
//函数功能	: 获取电池堆充电限制切除的电池组
//注意事项	: 包括充放电限制切除电池组
//=============================================================================================
u16 GetPileChgSepGroup(void)
{
    return(gPileChgSepGroup);
}

//=============================================================================================
//函数名称	: u16 GetPileDhgSepGroup(void)
//输入参数	: void  
//输出参数	: 电池堆放电限制切除的电池组(按位表示)
//函数功能	: 获取电池堆放电限制切除的电池组
//注意事项	: 包括充放电限制切除电池组
//=============================================================================================
u16 GetPileDhgSepGroup(void)
{
    return(gPileDhgSepGroup);
}

//=============================================================================================
//函数名称	: u16 GetPileErrSepGroup(void)
//输入参数	: void  
//输出参数	: 电池堆故障保护切除的电池组(按位表示)
//函数功能	: 获取电池堆故障保护切除的电池组
//注意事项	: 
//=============================================================================================
u16 GetPileErrSepGroup(void)
{
    return(gPileErrSepGroup);
}

//=============================================================================================
//函数名称	: u8 GetPileAutoParErrFlag(void)
//输入参数	: void  
//输出参数	: 电池堆动态接入异常标志 0:正常 1:失败
//函数功能	: 获取电池堆动态接入异常标志
//注意事项	: 
//=============================================================================================
u8 GetPileAutoParErrFlag(void)
{
    return(gPileParErrFlag);
}

//=============================================================================================
//函数名称	: u8 GetPileAutoSepErrFlag(void)
//输入参数	: void  
//输出参数	: 电池堆动态切除异常标志 0:正常 1:失败
//函数功能	: 获取电池堆动态切除异常标志
//注意事项	: 
//=============================================================================================
u8 GetPileAutoSepErrFlag(void)
{
    return(gPileSepErrFlag);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: static u16 SelectCanAutoRunGroup(void)
//输入参数	: void  
//输出参数	: 按位表示可以自动接入的电池组[0:没有自动并网组 1:可自动并网的组号]
//函数功能	: 获取可自动接入的电池组 
//注意事项	: 
//=============================================================================================
static u16 SelectCanAutoRunGroup(void)
{
    u16 canGroup = 0;
    u16 chgDhgState = 0;
    
    chgDhgState = GetPileChgStateAPI();
    
    //根据不同的充放电状态执行并网动作
    switch(chgDhgState)
    {
        //待机处理
        case eCURR_IDLE:
            canGroup = CheckIdleCanAutoRunGroup();
            break;
            
       //充电处理
        case eCURR_CHG:
            canGroup = CheckChgCanAutoRunGroup();
            break;
            
        //放电处理
        case eCURR_DHG:
            canGroup = CheckDhgCanAutoRunGroup();
            break;
            
        default:
            canGroup = 0;
            break;
    }
    
    return(canGroup);
}

//=============================================================================================
//函数名称	: static u16 CheckIdleCanAutoRunGroup(void) 
//输入参数	: void 
//输出参数	: 0:无可接入组 其他:按位表示可动态接入电池组
//函数功能	: 检测系统空闲状态下计算有无可动态接入电池组 
//注意事项	: 
//=============================================================================================
static u16 CheckIdleCanAutoRunGroup(void)
{
    u8 i = 0;
    u16 result = 0;
    
    //逐组判断是否适合接入运行
    for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
    {
        //检测系统状态适合接入运行(电池组接入后不改变电池堆状态)
        if((1 == GetPGroupEnStateAPI(i))                    //组使能
            && (0 == GetPGroupCanOffStateAPI(i))            //组通讯正常
            && (1 == GetPGroupIsoSwhStateAPI(i))            //组隔离开关闭合
            && (eWORK_IDLE == GetPGroupWorkStateAPI(i))     //组空闲
            && ((ePRO_SYSPRO != GetPGroupProStateAPI(i))    //组非充放电保护
            || (GetPGroupChgLimCurrAPI(i) > 0)              //组非充电保护(充电允许)
		    || (GetPGroupDhgLimCurrAPI(i) > 0)))            //组非放电保护(放电允许)
        {
            if(((0 == GetPileChgLimCurrAPI())           	//系统充放电均限制(非故障电池组均可并机)
                && (0 == GetPileDhgLimCurrAPI()))
				|| ((0 == GetPileChgLimCurrAPI())           //系统充电禁止(可放电)
				&& (GetPGroupDhgLimCurrAPI(i) > 0))         //电池组可放电(允许并机)
				|| ((0 == GetPileDhgLimCurrAPI())           //系统放电禁止(可充电)
				&& (GetPGroupChgLimCurrAPI(i) > 0))         //电池组可充电(允许并机)
				|| ((GetPGroupChgLimCurrAPI(i) > 0)         //电池组可充放电(允许并机)
				&& (GetPGroupDhgLimCurrAPI(i) > 0)))
            {
                //检测电压状态适合接入运行
                if(1 == CheckSysIdleGroupFitAutoRunState(i))
                {
                    BitSet(result, i);
                }
            }
        }
    }
    
    return(result);
}

//=============================================================================================
//函数名称	: static u8 CheckSysIdleGroupFitAutoRunState(u8 group)
//输入参数	: group:需接入运行组
//输出参数	: 1:适合运行 0:不适合运行
//函数功能	: 系统空闲时判断目标电池组是否适合自动投入运行
//注意事项	: 判断该组与运行组的压差
//=============================================================================================
static u8 CheckSysIdleGroupFitAutoRunState(u8 group)
{
    u8 i = 0;
    u8 num = 0;
    s16 volSub = 0;
    
    //与正在运行的电池组逐组比较
    for(i = 0; i < GetPileRunGroupNumAPI(); i++)
    {
        num = GetPileRunGroupIdAPI(i);
    
        //计算总电压差
        volSub = (s16)GetPGroupCSumVoltAPI(group) - (s16)GetPGroupCSumVoltAPI(num);
    
        //总电压差满足条件(6V内)
        if(1 == IsInside((0 - (s16)gPSmuGenParaR_102[eSmuGenPara102_UpSafeV]), volSub, gPSmuGenParaR_102[eSmuGenPara102_UpSafeV]))
        {
            //需要接入的电池组电压比运行的电池组电压高(环流放电)
            if(volSub > 0)
            {
                //防止接入组低压放电和运行组高压充电
                if((0 == GetPGroupDhgLimCurrAPI(group))
                    || (0 == GetPGroupChgLimCurrAPI(num))
                    || (GetPGroupCellMinVoltAPI(group) < gPSmuGenParaR_102[eSmuGenPara102_AutUpMinV])
                    || (GetPGroupCellMaxVoltAPI(num) > gPSmuGenParaR_102[eSmuGenPara102_AutUpMaxV]))
                {
                    return(0);
                }
            }
            //需要接入的电池组电压比运行的电池组电压低(环流充电)
            else if(volSub < 0) 
            {
                //防止接入组高压充电和运行组低压放电
                if((0 == GetPGroupChgLimCurrAPI(group))
                    || (0 == GetPGroupDhgLimCurrAPI(num))
                    || (GetPGroupCellMaxVoltAPI(group) > gPSmuGenParaR_102[eSmuGenPara102_AutUpMaxV])
                    || (GetPGroupCellMinVoltAPI(num) < gPSmuGenParaR_102[eSmuGenPara102_AutUpMinV]))
                {
                    return(0);
                }
            }
        }
        //总压差超过安全范围
        else
        {
            //不适合接入
            return(0);
        }
    }
    
    return(1);
}

//=============================================================================================
//函数名称	: static u16 CheckChgCanAutoRunGroup(void) 
//输入参数	: void 
//输出参数	: 0:无可接入组 其他:按位表示可动态接入电池组
//函数功能	: 检测系统充电状态下计算有无可动态接入电池组 
//注意事项	: 
//=============================================================================================
static u16 CheckChgCanAutoRunGroup(void)
{
    u8 i = 0;
    u16 result = 0;

    for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
    {
        //检测系统状态适合接入运行
        if((1 == GetPGroupEnStateAPI(i))                    //组使能
            && (0 == GetPGroupCanOffStateAPI(i))            //组通讯正常
            && (1 == GetPGroupIsoSwhStateAPI(i))            //组隔离开关闭合
            && (eWORK_IDLE == GetPGroupWorkStateAPI(i))     //组空闲
            && ((ePRO_SYSPRO != GetPGroupProStateAPI(i))    //组非充放电保护
            || (GetPGroupChgLimCurrAPI(i) > 0)              //组非充电保护(充电允许)
		    || (GetPGroupDhgLimCurrAPI(i) > 0)))            //组非放电保护(放电允许)
        {
            //检测电池状态适合接入运行(电池组接入后允许继续充电)
            if(GetPGroupChgLimCurrAPI(i) > 0)               //电池组充电允许
            {
                //检测电压状态适合接入运行
                if(1 == CheckSysChgGroupFitAutoRunState(i))
                {
                    BitSet(result, i);
                }
            }
        }
    }
    
    return(result);
}

//=============================================================================================
//函数名称	: static u8 CheckSysChgGroupFitAutoRunState(u8 group)
//输入参数	: 需接入运行组
//输出参数	: 1:适合运行 0:不适合运行
//函数功能	: 系统充电时判断目标电池组是否适合自动投入运行
//注意事项	: 判断该组与运行组的压差
//=============================================================================================
static u8 CheckSysChgGroupFitAutoRunState(u8 group)
{
    u8 num = 0;
    u8 cRow = 0;
    u8 vRow = 0;
    u16 volt = 0;
    s16 curr = 0;
    u16 voltUp = 0;
    u16 voltDn = 0;
    u16 voltSub = 0;
    
    curr = GetPileOutSumCurrAPI();
    
    if(curr > 0)
    {
        return(0);
    }
    else
    {
        num = GetPileRunGroupNumAPI();
        if(num > 0)
        {
            curr = (0 - curr) / num;
        }
        else
        {
            curr = 0;
        }
    }
    
    //查找当前电流是否在充电电流表中
    cRow = SearchHigFromUpTabs16(curr, (s16*)gBsuAutoCurrPara, AUTO_RUN_CURR_TAB_LEN);
    if(cRow >= AUTO_RUN_CURR_TAB_LEN)
    {
        //电流过大不进行动态接入
        return(0);
    }
    
    //计算当前单体所处电压平台(平均值 1mV)
    volt = GetPileSampSumVoltAPI() * 100 / gPSmuGenParaR_102[eSmuGenPara102_VSumNum];
    
    //查找当前电压所处电压平台
    vRow = SearchHigFromUpTabs16(volt, (s16*)gBsuAutoVoltPara, AUTO_RUN_VOLT_TAB_LEN);
    if(vRow > AUTO_RUN_VOLT_TAB_LEN)
    {
        //电流过大不进行动态接入
        vRow = AUTO_RUN_VOLT_TAB_LEN;
    }
    
    //计算当前浮充电压 0.1V
    voltSub = (u16)((u32)gBsuAutoChgSubVolt[vRow][cRow] * (u32)gPSmuGenParaR_102[eSmuGenPara102_VSumNum] / 100);
    
    //计算电池堆相应静态电压 0.1V
    volt = GetPileSampSumVoltAPI() - voltSub;
    
    //计算允许接入的电压上限和下限(安全电压一半)
    voltUp = volt + (gPSmuGenParaR_102[eSmuGenPara102_UpSafeV] / 2);
    voltDn = volt - (gPSmuGenParaR_102[eSmuGenPara102_UpSafeV] / 2);
    
    //电压差满足条件
    if(1 == IsInside(voltDn, GetPGroupCSumVoltAPI(group), voltUp))
    {
        return(1);
    }
    
    return(0);
}

//=============================================================================================
//函数名称	: static u16 CheckDhgCanAutoRunGroup(void) 
//输入参数	: void 
//输出参数	: 0:无可接入组 其他:按位表示可动态接入电池组
//函数功能	: 检测系统放电状态下计算有无可动态接入电池组 
//注意事项	: 
//=============================================================================================
static u16 CheckDhgCanAutoRunGroup(void)
{
    u8 i = 0;
    u16 result = 0;

    for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
    {
        //检测系统状态适合接入运行
        if((1 == GetPGroupEnStateAPI(i))                    //组使能
            && (0 == GetPGroupCanOffStateAPI(i))            //组通讯正常
            && (1 == GetPGroupIsoSwhStateAPI(i))            //组隔离开关闭合
            && (eWORK_IDLE == GetPGroupWorkStateAPI(i))     //组空闲
            && ((ePRO_SYSPRO != GetPGroupProStateAPI(i))    //组非充放电保护
            || (GetPGroupChgLimCurrAPI(i) > 0)              //组非充电保护(充电允许)
		    || (GetPGroupDhgLimCurrAPI(i) > 0)))            //组非放电保护(放电允许)
        {
            //检测电池状态适合接入运行(电池组接入后允许继续充电)
        	if(GetPGroupDhgLimCurrAPI(i) > 0)               //电池组放电允许
            {
                //检测电压状态适合接入运行s
                if(1 == CheckSysDhgGroupFitAutoRunState(i))
                {
                    BitSet(result, i);
                }
            }
        }
    }
    
    return(result);
}

//=============================================================================================
//函数名称	: static u8 CheckSysDhgGroupFitAutoRunState(u8 group)
//输入参数	: 需接入运行组
//输出参数	: 1:适合运行 0:不适合运行
//函数功能	: 系统放电时判断目标电池组是否适合自动投入运行
//注意事项	: 判断该组与运行组的压差
//=============================================================================================
static u8 CheckSysDhgGroupFitAutoRunState(u8 group)
{
    u8 num = 0;
    u8 cRow = 0;
    u8 vRow = 0;
    u16 volt = 0;
    s16 curr = 0;
    u16 voltUp = 0;
    u16 voltDn = 0;
    u16 voltSub = 0;
    
    curr = GetPileOutSumCurrAPI();
    
    if(curr < 0)
    {
        return(0);
    }
    else
    {
        num = GetPileRunGroupNumAPI();
        if(num > 0)
        {
            curr = curr / num;
        }
        else
        {
            curr = 0;
        }
    }
    
    //查找当前电流是否在充电电流表中
    cRow = SearchHigFromUpTabs16(curr, (s16*)gBsuAutoCurrPara, AUTO_RUN_CURR_TAB_LEN);
    if(cRow >= AUTO_RUN_CURR_TAB_LEN)
    {
        //电流过大不进行动态接入
        return(0);
    }
    
    //计算当前单体所处电压平台(平均值 1mV)
    volt = GetPileSampSumVoltAPI() * 100 / gPSmuGenParaR_102[eSmuGenPara102_VSumNum];
    
    //查找当前电压所处电压平台
    vRow = SearchHigFromUpTabs16(volt, (s16*)gBsuAutoVoltPara, AUTO_RUN_VOLT_TAB_LEN);
    if(vRow > AUTO_RUN_VOLT_TAB_LEN)
    {
        //电流过大不进行动态接入
        vRow = AUTO_RUN_VOLT_TAB_LEN;
    }
    
    //计算当前压降电压 0.1V
    voltSub = (u16)((u32)gBsuAutoDhgSubVolt[vRow][cRow] * (u32)gPSmuGenParaR_102[eSmuGenPara102_VSumNum] / 100);
    
    //计算电池堆相应静态电压 0.1V
    volt = GetPileSampSumVoltAPI() + voltSub;
    
    //计算允许接入的电压上限和下限(安全电压一半)
    voltUp = volt + (gPSmuGenParaR_102[eSmuGenPara102_UpSafeV] / 2);
    voltDn = volt - (gPSmuGenParaR_102[eSmuGenPara102_UpSafeV] / 2);
    
    //电压差满足条件
    if(1 == IsInside(voltDn, GetPGroupCSumVoltAPI(group), voltUp))
    {
        return(1);
    }
    
    return(0);
}

#endif
