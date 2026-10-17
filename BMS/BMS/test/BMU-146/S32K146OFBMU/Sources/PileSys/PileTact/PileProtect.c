/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileProtect.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控电池堆故障保护控制任务模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PilePara.h"
#include "PileInfo.h"
#include "PublicInfo.h"
#include "PileGroupRcv.h"
#include "PileDiagUser.h"
#include "PileSysState.h"
#include "PileTactics.h"
#include "PileTactInterface.h"
#include "PileProtect.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u16 sPileAskProtFlag = 0;                //请求保护标志 按位表示[bit0:充电限制保护 bit1:放电限制保护 bit2:充电异常保护 bit3:放电异常保护 bit4:充电瞬时保护 bit5:放电保护故障保护 (0:正常 1:请求保护)
static u16 sPileSysProtFlag = 0;                //异常保护标志 按位表示[bit0:充电限制保护 bit1:放电限制保护 bit2:充电异常保护 bit3:放电异常保护 bit4:充电瞬时保护 bit5:放电保护故障保护 (0:正常 1:执行保护)
static u16 sPileProTimer[eBsuProSort_Num] = {0};//各故障保护计时

static u8 sPileSamProSort = 0;                  //同类故障保护类型(1充电保护 2放电保护)
static u8 sPileSamProTimes = 0;                 //同类故障保护次数(10分钟连续3次则不恢复)
static u32 sPileSamProTimer = 0;                //同类故障保护起始时间

extern u16 gPSmuGenParaR_102[];  			    /*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
//=============================================================================================
//函数名称	: static void PileSysProCtrlCheck(u8 sort)
//输入参数	: sort:保护类型[0充电限制保护 1放电限制保护 2充电异常保护 3放电异常保护 4充电瞬时过流保护 5放电短路保护 6系统故障保护
//输出参数	: void
//函数功能	: 故障告警保护控制检查
//注意事项	: 
//=============================================================================================
static void PileSysProCtrlCheck(u8 sort);

//=============================================================================================
//函数名称	: static void PileSysProCtrlClr(u8 sort)
//输入参数	: sort:保护类型[0充电限制保护 1放电限制保护 2充电异常保护 3放电异常保护 4充电瞬时过流保护 5放电短路保护 6系统故障保护
//输出参数	: void
//函数功能	: 故障告警保护控制消除
//注意事项	: 
//=============================================================================================
static void PileSysProCtrlClr(u8 sort);

//=============================================================================================
//函数名称	: static void PileErrProCtrlByCmd(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆故障保护下发相应控制命令
//注意事项	: 
//=============================================================================================
static void PileErrProCtrlByCmd(void);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void PileSysProCtrlInit(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆故障告警保护控制初始化
//注意事项	: 
//=============================================================================================
void PileSysProCtrlInit(void)
{
    sPileAskProtFlag = 0;                       //请求保护标志
    sPileSysProtFlag = 0;                       //异常保护标志
    sPileSamProSort = 0;
    sPileSamProTimer = 0;
    sPileSamProTimes = 0;
}

//=============================================================================================
//函数名称	: void PileSysProCtrlTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆故障告警保护控制任务
//注意事项	: 100ms
//=============================================================================================
void PileSysProCtrlTask(void)
{
    u8 errSort = 0;
    u8 groupPro = 0;
    
    //系统调试模式不保护
    if(eMODE_DEBUG == GetPileWorkModeAPI())
    {
    	TacClrPileProFlagAPI();
        return;
    }
    
    //获取当前故障级别
    errSort = GetPileNowErrSortLevelAPI();
    
    //获取当前电池组保护状态
    groupPro = GetPileAllGProStateAPI();
    
    //系统故障保护检测
    if((eWORK_ERR == GetPileWorkStateAPI())             //系统故障
        || (eErr_SysErr == errSort))                    //故障类型故障
    {
        PileSysProCtrlCheck(eBsuProSort_SysErr);        //检测是否保护
    }
    else
    {
        PileSysProCtrlClr(eBsuProSort_SysErr);
    }
    
    //充放电保护检测(产生充放电保护类型告警故障[非故障状态但接触器需断开])
    if((eErr_SysPro == errSort)                         //充放电保护类型故障
        || (ePGPRO_STA_SYSPRO == groupPro))             //电池组充放电保护
    {
        PileSysProCtrlCheck(eBsuProSort_SysPro);        //检测是否保护
    }
    else
    {
        PileSysProCtrlClr(eBsuProSort_SysPro);
    }
    
    //放电保护(仍在放电)保护检测
    if(((eErr_DhgPro == errSort)                        //堆控放电保护类型故障
        || (ePGPRO_STA_DHGPRO == groupPro))             //电池组请求放电保护
        && (eCURR_DHG == GetPileChgStateAPI()))         //仍在放电
    {
        PileSysProCtrlCheck(eBsuProSort_DhgPro);        //检测是否保护
    }
    else if((eErr_DhgPro != errSort)                    //非放电保护类型故障
        && (eErr_SysPro != errSort)                     //非充放电保护类型故障
        && (eErr_SysErr != errSort)                     //非故障保护类型故障
        && (ePGPRO_STA_DHGPRO != groupPro)              //电池组非放电保护
        && (ePGPRO_STA_SYSPRO != groupPro))             //电池组非故障保护
    {
        PileSysProCtrlClr(eBsuProSort_DhgPro);
    }
    
    //放电限制(仍在放电)保护检测
    if(((eErr_SysLim == errSort)                        //充放电限制类型故障
        || (eErr_DhgLim == errSort)                     //放电限制类型故障
        || (ePGPRO_STA_SYSLIM == groupPro)              //电池组非充放电限制
        || (ePGPRO_STA_DHGLIM == groupPro))             //电池组非放电限制
        && (eCURR_DHG == GetPileChgStateAPI()))         //仍在放电
    {
        PileSysProCtrlCheck(eBsuProSort_DhgLim);        //检测是否保护
    }
//  /*有电池放空电压低(仍在放电)保护检测*/
//  else if(((ePGPRO_STA_EMPTY == groupPro)             /*电池组放空*/
//		&& (GetPileGCellMinVoltAPI() <= gPSmuGenParaR_102[eSmuGenPara102_AutUpMinV]))/*电压仍过二级告警(电压回升不保护)*/
//		&& (eCURR_DHG == GetPileChgStateAPI()))         /*仍在放电*/
//	{
//    	PileSysProCtrlCheck(eBsuProSort_DhgLim);        /*检测是否保护*/
//	}
    else
    {
        PileSysProCtrlClr(eBsuProSort_DhgLim);
    }
    
    //充电保护(仍在充电)保护检测
    if(((eErr_ChgPro == errSort)                        //堆控充电保护类型故障
        || (ePGPRO_STA_CHGPRO == groupPro))             //电池组请求充电上保护
        && (eCURR_CHG == GetPileChgStateAPI()))         //仍在充电
    {
        PileSysProCtrlCheck(eBsuProSort_ChgPro);        //检测是否保护
    }
    else if((eErr_ChgPro != errSort)                    //非充电保护类型故障
        && (eErr_SysPro != errSort)                     //非充放电保护类型故障
        && (eErr_SysErr != errSort)                     //非故障保护类型故障
        && (ePGPRO_STA_CHGPRO != groupPro)              //电池组非充电保护
        && (ePGPRO_STA_SYSPRO != groupPro))             //电池组非故障保护
    {
        PileSysProCtrlClr(eBsuProSort_ChgPro);
    }
    
    //充电限制(仍在充电)保护检测
    if(((eErr_SysLim == errSort)                        //充放电限制类型故障
        || (eErr_ChgLim == errSort)                     //充电限制类型故障
        || (ePGPRO_STA_SYSLIM == groupPro)              //电池组充放电限制
        || (ePGPRO_STA_CHGLIM == groupPro))             //电池组充电限制
        && (eCURR_CHG == GetPileChgStateAPI()))         //仍在充电
    {
        PileSysProCtrlCheck(eBsuProSort_ChgLim);        //检测是否保护
    }
//  /*有电池充满电压高(仍在充电)保护检测*/
//  else if(((ePGPRO_STA_FULL == groupPro)              /*电池组充满*/
//		&& (GetPileGCellMaxVoltAPI() >= gPSmuGenParaR_102[eSmuGenPara102_AutUpMaxV]))/*电压仍过二级告警(电压回落不保护)*/
//		&& (eCURR_CHG == GetPileChgStateAPI()))         /*仍在充电*/
//	{
//    	PileSysProCtrlCheck(eBsuProSort_ChgLim);        /*检测是否保护*/
//	}
    else
    {
        PileSysProCtrlClr(eBsuProSort_ChgLim);
    }
}

//=============================================================================================
//函数名称	: void TacClrPileProFlagAPI(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 清除电池堆系统故障告警保护标志
//注意事项	: 
//=============================================================================================
void TacClrPileProFlagAPI(void)
{
    sPileAskProtFlag = 0;
    sPileSysProtFlag = 0;
    sPileSamProSort = 0;
    sPileSamProTimer = 0;
    sPileSamProTimes = 0;
}

//=============================================================================================
//函数名称	: u8 GetPileAskChgProState(void)
//输入参数	: void 
//输出参数	: 电池堆充电请求保护状态:0:不请求保护 1:请求保护
//函数功能	: 获得电池堆充电请求保护状态
//注意事项	: 
//=============================================================================================
u8 GetPileAskChgProState(void)
{
    u16 state = 0;
    
    if((1 == BitGet(sPileAskProtFlag, eBsuProSort_SysErr))
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_SysPro))
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_ChgPro))
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_ChgFst))
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_ChgLim)))
    {
        state = 1;
    }
    
    return(state);
}

//=============================================================================================
//函数名称	: u8 GetPileAskDhgProState(void)
//输入参数	: void 
//输出参数	: 电池堆放电请求保护状态:0:不请求保护 1:请求保护
//函数功能	: 获得电池堆放电请求保护状态
//注意事项	: 
//=============================================================================================
u8 GetPileAskDhgProState(void)
{
    u16 state = 0;
    
    if((1 == BitGet(sPileAskProtFlag, eBsuProSort_SysErr))
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_SysPro))
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_DhgPro))
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_DhgFst))
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_DhgLim)))
    {
        state = 1;
    }
    
    return(state);
}

//=============================================================================================
//函数名称	: u8 GetPileAskSysProState(void)
//输入参数	: void 
//输出参数	: 电池堆充放电请求保护状态:0:不请求保护 1:请求保护
//函数功能	: 获得电池堆请求充放电保护状态
//注意事项	: 同时有充电保护和放电保护
//=============================================================================================
u8 GetPileAskSysProState(void)
{
    //return(sPileAskProtFlag);
    
    //只考虑充放电均保护的故障保护
    u16 state = 0;
    
    if((1 == BitGet(sPileAskProtFlag, eBsuProSort_SysErr))          //请求故障保护
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_SysPro))      //请求充放电保护
        || (((1 == BitGet(sPileAskProtFlag, eBsuProSort_ChgFst))    //同时请求充电保护和放电保护
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_ChgPro))
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_ChgLim)))
        && ((1 == BitGet(sPileAskProtFlag, eBsuProSort_DhgFst))
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_DhgPro))
        || (1 == BitGet(sPileAskProtFlag, eBsuProSort_DhgLim)))))
    {
        state = 1;
    }
    
    return(state);
}

//=============================================================================================
//函数名称	: u16 TacGetPileHappenProFlagAPI(void)
//输入参数	: void 
//输出参数	: 按位表示电池堆已产生系统保护的故障告警: bit0:充电限制保护 bit1:放电限制保护 bit2:充电异常保护 bit3:放电异常保护 bit4:充电瞬时保护 bit5:放电保护故障保护 (0:正常 1:产生保护)
//函数功能	: 获得电池堆系统产生的故障保护标志 
//注意事项	: 已执行的保护
//=============================================================================================
u16 TacGetPileHappenProFlagAPI(void)
{
    return(sPileSysProtFlag);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: static void PileSysProCtrlCheck(u8 sort)
//输入参数	: sort:保护类型[0充电限制保护 1放电限制保护 2充电异常保护 3放电异常保护 4充电瞬时过流保护 5放电短路保护 6系统故障保护
//输出参数	: void
//函数功能	: 故障告警保护控制检查
//注意事项	: 
//=============================================================================================
static void PileSysProCtrlCheck(u8 sort)
{
    static u32 sHisCtrlTime = 0;
    
    switch(sort)
    {
        //系统故障保护
        case eBsuProSort_SysErr:
            BitSet(sPileAskProtFlag, eBsuProSort_SysErr);                            //设置请求保护
            if(sPileProTimer[eBsuProSort_SysErr] >= (gPSmuGenParaR_102[eSmuGenPara102_ErrProT] * 10))//达到保护时间
            {
                PileErrProCtrlByCmd();                                               //断开相应运行接触器保护
                BitSet(sPileSysProtFlag, eBsuProSort_SysErr);                        //设置执行保护
            }
            else
            {
                sPileProTimer[eBsuProSort_SysErr]++;
            }
            break;
            
        //充放电保护
        case eBsuProSort_SysPro:
            BitSet(sPileAskProtFlag, eBsuProSort_SysPro);                            //设置请求保护
            if(sPileProTimer[eBsuProSort_SysPro] >= (gPSmuGenParaR_102[eSmuGenPara102_ErrProT] * 10))//达到保护时间
            {
                if(sPileSamProSort > 0)                                             //已发生过充放电保护
                {
                    if((ABS(sHisCtrlTime, BSPGetOSMillSecTimeAPI()) >= 5000)        //间隔5s有电池组接触器未断开时的控制有效计算保护次数
                        && (GetPAllGroupRunStateAPI() > 0))
                    {
                        sPileSamProTimes++;
                    }
                }
                else if((0 == sPileSamProSort) || (sPileSamProTimes < BSU_EXP_PRO_MAX_TIMES))//首次产生放电保护(其他保护未达到最大保护次数)
                {
                    sPileSamProSort = 3;
                    sPileSamProTimes = 1;
                    sPileSamProTimer = BSPGetOSMillSecTimeAPI();
                }
                sHisCtrlTime = BSPGetOSMillSecTimeAPI();                            //记录保护控制时间

                PileErrProCtrlByCmd();                                               //断开相应运行接触器保护
                BitSet(sPileSysProtFlag, eBsuProSort_SysPro);                        //设置执行保护
            }
            else
            {
                sPileProTimer[eBsuProSort_SysPro]++;
            }
            break;
            
        //放电短路保护
        case eBsuProSort_DhgFst:
            BitSet(sPileAskProtFlag, eBsuProSort_DhgFst);                           //设置请求保护
            BitSet(sPileSysProtFlag, eBsuProSort_DhgFst);                           //设置执行保护
            PileErrProCtrlByCmd();                                                  //断开相应运行接触器保护
            break;
            
        //充电瞬时保护
        case eBsuProSort_ChgFst:
            BitSet(sPileAskProtFlag, eBsuProSort_ChgFst);                           //设置请求保护
            BitSet(sPileSysProtFlag, eBsuProSort_ChgFst);                           //设置执行保护
            PileErrProCtrlByCmd();                                                  //断开相应运行接触器保护
            break;
            
        //放电保护
        case eBsuProSort_DhgPro:
            BitSet(sPileAskProtFlag, eBsuProSort_DhgPro);                           //设置请求保护
            if(sPileProTimer[eBsuProSort_DhgPro] >= (gPSmuGenParaR_102[eSmuGenPara102_DhgProT] * 10))//达到保护时间
            {
                if(2 == sPileSamProSort)                                            //已发生过放电保护
                {
                    if((ABS(sHisCtrlTime, BSPGetOSMillSecTimeAPI()) >= 5000)        //间隔5s有电池组接触器未断开时的控制有效计算保护次数
                        && (GetPAllGroupRunStateAPI() > 0))
                    {
                        sPileSamProTimes++;
                    }
                }
                else if((0 == sPileSamProSort) || (sPileSamProTimes < BSU_EXP_PRO_MAX_TIMES))//首次产生放电保护(其他保护未达到最大保护次数)
                {
                    sPileSamProSort = 2; 
                    sPileSamProTimes = 1;
                    sPileSamProTimer = BSPGetOSMillSecTimeAPI();
                }
                sHisCtrlTime = BSPGetOSMillSecTimeAPI();                            //记录保护控制时间
                
                PileErrProCtrlByCmd();                                              //断开相应运行接触器保护
                BitSet(sPileSysProtFlag, eBsuProSort_DhgPro);                       //设置执行保护
            }
            else
            {
                sPileProTimer[eBsuProSort_DhgPro]++;
            }
            break;
            
        //充电保护
        case eBsuProSort_ChgPro:
            BitSet(sPileAskProtFlag, eBsuProSort_ChgPro);                           //设置请求保护
            if(sPileProTimer[eBsuProSort_ChgPro] >= (gPSmuGenParaR_102[eSmuGenPara102_ChgProT] * 10))//达到保护时间
            {
                if(1 == sPileSamProSort)                                            //已发生过充电保护
                {
                    if((ABS(sHisCtrlTime, BSPGetOSMillSecTimeAPI()) >= 5000)        //间隔5s有电池组接触器未断开时的控制有效计算保护次数
                        && (GetPAllGroupRunStateAPI() > 0))
                    {
                        sPileSamProTimes++;
                    }
                }
                else if((0 == sPileSamProSort) || (sPileSamProTimes < BSU_EXP_PRO_MAX_TIMES))//首次产生充电保护(其他保护未达到最大保护次数)
                {
                    sPileSamProSort = 1;
                    sPileSamProTimes = 1;
                    sPileSamProTimer = BSPGetOSMillSecTimeAPI();
                }
                sHisCtrlTime = BSPGetOSMillSecTimeAPI();                            //记录保护控制时间
                
                PileErrProCtrlByCmd();                                              //断开相应运行接触器保护
                BitSet(sPileSysProtFlag, eBsuProSort_ChgPro);                       //设置执行保护
            }
            else
            {
                sPileProTimer[eBsuProSort_ChgPro]++;
            }
            break;
            
        //放电限制保护(只请求保护不断接触器)
        case eBsuProSort_DhgLim:
            if(sPileProTimer[eBsuProSort_DhgLim] >= (gPSmuGenParaR_102[eSmuGenPara102_DhgLimT] * 10))//达到保护时间
            {
                //PileErrProCtrlByCmd();                                            //断开相应运行接触器保护
                BitSet(sPileAskProtFlag, eBsuProSort_DhgLim);                       //设置请求保护
            }
            else
            {
                sPileProTimer[eBsuProSort_DhgLim]++;
            }
            break;
            
        //充电限制保护(只请求保护不断接触器)
        case eBsuProSort_ChgLim:
            if(sPileProTimer[eBsuProSort_ChgLim] >= (gPSmuGenParaR_102[eSmuGenPara102_ChgLimT] * 10))//达到保护时间
            {
                //PileErrProCtrlByCmd();                                            //断开相应运行接触器保护
                BitSet(sPileAskProtFlag, eBsuProSort_ChgLim);                       //设置请求保护
            }
            else
            {
                sPileProTimer[eBsuProSort_ChgLim]++;
            }
            break;
            
        default:
            break;
    }
}

//=============================================================================================
//函数名称	: static void PileSysProCtrlClr(u8 sort)
//输入参数	: sort:保护类型[0充电限制保护 1放电限制保护 2充电异常保护 3放电异常保护 4充电瞬时过流保护 5放电短路保护 6系统故障保护
//输出参数	: void
//函数功能	: 故障告警保护控制消除
//注意事项	: 
//=============================================================================================
static void PileSysProCtrlClr(u8 sort)
{
    //类型有效
    if(sort < eBsuProSort_Num)
    {
        //已发生过保护
        if(sPileSamProSort > 0)
        {
            //距离上次保护时间超过10min未达到3次保护
            if(((sPileSamProTimer + 600000) < BSPGetOSMillSecTimeAPI())
                && (sPileSamProTimes < BSU_EXP_PRO_MAX_TIMES))
            {
                //参数复位
                sPileSamProSort = 0;
                sPileSamProTimes = 0;
                sPileSamProTimer = 0;
            }
            //达到3次保护
            else if(sPileSamProTimes >= BSU_EXP_PRO_MAX_TIMES)
            {
                //故障保护间隔1小时
                if((sPileSamProTimer + 3600000) < BSPGetOSMillSecTimeAPI())
                {
                    //允许再尝试一次
                    //sSamProSort = 0;
                    sPileSamProTimes = 2;
                    sPileSamProTimer =  BSPGetOSMillSecTimeAPI();
                }
                //不清除本类故障保护
                else if(((2 == sPileSamProSort) && ((eBsuProSort_DhgPro == sort) || (eBsuProSort_SysPro == sort)))
                    || ((1 == sPileSamProSort) && ((eBsuProSort_ChgPro == sort) || (eBsuProSort_SysPro == sort)))
                    || ((3 == sPileSamProSort) && (eBsuProSort_SysPro == sort)))
                {
                    //充电保护或放电保护均上升为充放电保护(防止再启动)
                    BitSet(sPileAskProtFlag, eBsuProSort_SysPro);
                    BitSet(sPileSysProtFlag, eBsuProSort_SysPro);
                    return;
                }
            }
        }
        
        sPileProTimer[sort] = 0;
        BitClr(sPileAskProtFlag, sort);
        BitClr(sPileSysProtFlag, sort);
    }
}

//=============================================================================================
//函数名称	: static void PileErrProCtrlByCmd(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆故障保护下发相应控制命令
//注意事项	: 
//=============================================================================================
static void PileErrProCtrlByCmd(void)
{
	//所有电池组的接触器全部断开
	if(0 == GetPAllGroupRunStateAPI())
	{
		return;
	}
	
	//广播所有电池组强制停机
	PileBroadAllGroupForcibStopCmdHook();
	//广播所有电池组紧急停机
	//PileBroadAllGroupEmergStopCmdHook();
}

#endif
