/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileGroupState.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控检测各电池组状态任务模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicPara.h"
#include "PilePara.h"
#include "PublicInfo.h"
#include "PileInfo.h"
#include "PileGroupRcv.h"
#include "PileDiagUser.h"
#include "PileProtect.h"
#include "PileTactInterface.h"
#include "PileSysState.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static t_GROUPSTATE_INFO sGroupStateInfo;   //电池组总状态信息

extern u16 gPSysStateInfo_50[];				/*系统状态信息数组*/
extern u16 gPGroupState_57[]; 				/*电池组状态信息数组*/

extern u16 gSysGenParaRO_100[];  			/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];  			/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void PileSysStateInit(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 系统状态检测模块初始化
//注意事项	:
//=============================================================================================
void PileSysStateInit(void)
{
    //初始化系统工作状态(初始化)/电池保护状态(未就绪)/充放电状态(空闲)
	gPSysStateInfo_50[ePStateInfo50_SysPro] = ePRO_WAIT;
	gPSysStateInfo_50[ePStateInfo50_SysChg] = eCURR_IDLE;
	gPSysStateInfo_50[ePStateInfo50_SysWor] = eWORK_INIT;
}

//=============================================================================================
//函数名称	: void PileAllGStateCheckTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆检测电池组总状态任务
//注意事项	: 100ms,分析电池组总状态
//=============================================================================================
void PileAllGStateCheckTask(void)
{
    u8 i = 0;
    u8 allNum = 0;      //激活组数
    u8 norNum = 0;      //无故障可用组数(包括充放电限制的)
    u8 fitNum = 0;      //适合运行组数(包括运行的)
    u8 runNum = 0;      //正常运行状态组数(运行状态)
    u8 sepNum = 0;      //需要隔离的组数(接触器闭合组)
    
    u8 fullNum = 0;     //充满电池组数
    u8 emptyNum = 0;    //放空电池组数
    u8 cLimNum = 0;     //充电限制电池组数
    u8 dLimNum = 0;     //充电限制电池组数
    u8 sLimNum = 0;     //充放电限制电池组数
    u8 cProNum = 0;     //充电保护电池组数
    u8 dProNum = 0;     //放电保护电池组数
    u8 sProNum = 0;     //充放电保护电池组数
    
    u8 intFlag = 0;
    u8 batFlag = 0;
    u16 norGroup = 0;
    u16 useGroup = 0;
    u16 errGroup = 0;
    
    /*上电组数大于最小运行组数时可用组数不能小于最小组数;上电组数小于最小组数时必须全部上电组均可用*/
    for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
    {
        //该组激活使能
        if(1 == GetPGroupEnStateAPI(i))
        {
            //通讯正常
            if(0 == GetPGroupCanOffStateAPI(i))
            {
                //已使能上电电池组
                allNum++;
                
                //电池组运行接触器闭合
                if(FALSE == GetPGroupRunSwhAllOffStateAPI(i))
                {
                    //计算使用的电池组数
                    BitSet(useGroup, i);
                }
                
                //电池组非初始化
                if((eWORK_INIT != GetPGroupWorkStateAPI(i))
                    && (eWORK_OFF != GetPGroupWorkStateAPI(i)))
                {
                    //电池组正常未保护
                    if((ePRO_SYSPRO != GetPGroupProStateAPI(i))
                        || (GetPGroupChgLimCurrAPI(i) > 0)
                        || (GetPGroupDhgLimCurrAPI(i) > 0))
                    {
                        //记录正常的电池组
                        BitSet(norGroup, i);
                        
                        //记录正常电池组序列
                        sGroupStateInfo.norGroup[norNum] = i;
                        norNum++;
                        
                        //电池堆运行状态
                        if(eWORK_RUN == GetPileWorkStateAPI())
                        {
                            //计算运行的电池组数目及充放电限制状态
                            if((eWORK_RUN == GetPGroupWorkStateAPI(i))
                                || (FALSE == GetPGroupRunSwhAllOffStateAPI(i)))
                            {
                                //正在运行电池组
                                sGroupStateInfo.runGroup[runNum] = i;
                                runNum++;

                                //电池组充满
                                if(ePRO_FULL == GetPGroupProStateAPI(i))
                                {
                                    //计算充满电池组数
                                    fullNum++;
                                    
                                    //电池组运行接触器闭合
                                    if(FALSE == GetPGroupRunSwhAllOffStateAPI(i))
                                    {
                                        //记录需要切除的电池组(原因:充电限制)
                                        sGroupStateInfo.sepGroup[sepNum] = i;
                                        sGroupStateInfo.sepReason[sepNum] = eSEP_REASON_CHGLIM;
                                        sepNum++;
                                    }
                                }
                                //电池组充电限制
                                else if((ePRO_SYSLIM == GetPGroupProStateAPI(i))
                                    && (GetPGroupDhgLimCurrAPI(i) > 0))
                                {
                                    //计算充电限制电池组数
                                    cLimNum++;
                                    
                                    //电池组运行接触器闭合
                                    if(FALSE == GetPGroupRunSwhAllOffStateAPI(i))
                                    {
                                        //记录需要切除的电池组(原因:充电限制)
                                        sGroupStateInfo.sepGroup[sepNum] = i;
                                        sGroupStateInfo.sepReason[sepNum] = eSEP_REASON_CHGLIM;
                                        sepNum++;
                                    }
                                }
                                //电池组充电保护
                                else if((ePRO_SYSPRO == GetPGroupProStateAPI(i))
                                    && (GetPGroupDhgLimCurrAPI(i) > 0))
                                {
                                    //计算充电保护电池组数
                                    cProNum++;
                                    
                                    //电池组运行接触器闭合
                                    if(FALSE == GetPGroupRunSwhAllOffStateAPI(i))
                                    {
                                        //记录需要切除的电池组(原因:充电限制)
                                        sGroupStateInfo.sepGroup[sepNum] = i;
                                        sGroupStateInfo.sepReason[sepNum] = eSEP_REASON_CHGLIM;
                                        sepNum++;
                                    }
                                }
                                //电池组放空
                                else if(ePRO_EMPTY == GetPGroupProStateAPI(i))
                                {
                                    //计算放空电池组数
                                    emptyNum++;
                                    
                                    //电池组运行接触器闭合
                                    if(FALSE == GetPGroupRunSwhAllOffStateAPI(i))
                                    {
                                        //记录需要切除的电池组(原因:放电限制)
                                        sGroupStateInfo.sepGroup[sepNum] = i;
                                        sGroupStateInfo.sepReason[sepNum] = eSEP_REASON_DHGLIM;
                                        sepNum++;
                                    }
                                }
                                //电池组放电限制
                                else if((ePRO_SYSLIM == GetPGroupProStateAPI(i))
                                    && (GetPGroupChgLimCurrAPI(i) > 0))
                                {
                                    //计算放电限制电池组数
                                    dLimNum++;
                                    
                                    //电池组运行接触器闭合
                                    if(FALSE == GetPGroupRunSwhAllOffStateAPI(i))
                                    {
                                        //记录需要切除的电池组(原因:放电限制)
                                        sGroupStateInfo.sepGroup[sepNum] = i;
                                        sGroupStateInfo.sepReason[sepNum] = eSEP_REASON_DHGLIM;
                                        sepNum++;
                                    }
                                }
                                //电池组放电保护
                                else if((ePRO_SYSPRO == GetPGroupProStateAPI(i))
                                    && (GetPGroupChgLimCurrAPI(i) > 0))
                                {
                                    //计算放电保护电池组数
                                    dProNum++;
                                    
                                    //电池组运行接触器闭合
                                    if(FALSE == GetPGroupRunSwhAllOffStateAPI(i))
                                    {
                                        //记录需要切除的电池组(原因:放电限制)
                                        sGroupStateInfo.sepGroup[sepNum] = i;
                                        sGroupStateInfo.sepReason[sepNum] = eSEP_REASON_DHGLIM;
                                        sepNum++;
                                    }
                                }
                                //电池组充放电限制
                                else if(ePRO_SYSLIM == GetPGroupProStateAPI(i))
                                {
                                    //计算充放电限制电池组数
                                    sLimNum++;
                                    
                                    //电池组运行接触器闭合
                                    if(FALSE == GetPGroupRunSwhAllOffStateAPI(i))
                                    {
                                         //记录需要切除的电池组(原因:充放电限制)
                                         sGroupStateInfo.sepGroup[sepNum] = i;
                                        sGroupStateInfo.sepReason[sepNum] = eSEP_REASON_SYSLIM;
                                        sepNum++;
                                    }
                                }
                                //电池组正常适合运行
                                else
                                {
                                    //记录完全正常的电池组
                                    fitNum++;
                                }
                            }
                        }
                        //电池堆非运行状态
                        else
                        {
                            //计算运行的电池组数目
                            if(eWORK_RUN == GetPGroupWorkStateAPI(i))
                            {
                                //正在运行电池组
                                sGroupStateInfo.runGroup[runNum] = i;
                                runNum++;
                            }
                            //电池组充满
                            if(ePRO_FULL == GetPGroupProStateAPI(i))
                            {
                                //计算充满电池组数
                                fullNum++;
                            }
                            //电池组充电限制
                            else if((ePRO_SYSLIM == GetPGroupProStateAPI(i))
                                && (GetPGroupDhgLimCurrAPI(i) > 0))
                            {
                                //计算充电限制电池组数
                                cLimNum++;
                            }
                            //电池组充电保护
                            else if((ePRO_SYSPRO == GetPGroupProStateAPI(i))
                                && (GetPGroupDhgLimCurrAPI(i) > 0))
                            {
                                //计算充电保护电池组数
                                cProNum++;
                            }
                            //电池组放空
                            else if(ePRO_EMPTY == GetPGroupProStateAPI(i))
                            {
                                //计算放空电池组数
                                emptyNum++;
                            }
                            //电池组放电限制
                            else if((ePRO_SYSLIM == GetPGroupProStateAPI(i))
                            	&& (GetPGroupChgLimCurrAPI(i) > 0))
                            {
                                //计算放电限制电池组数
                                dLimNum++;
                            }
                            //电池组放电限制/放电保护
                            else if((ePRO_SYSPRO == GetPGroupProStateAPI(i))
                                && (GetPGroupChgLimCurrAPI(i) > 0))
                            {
                                //计算放电保护电池组数
                                dProNum++;
                            }
                            //电池组充放电限制
                            else if(ePRO_SYSLIM == GetPGroupProStateAPI(i))
                            {
                                //计算充放电限制电池组数
                                sLimNum++;
                                
                                //电池组运行接触器闭合
                                if(FALSE == GetPGroupRunSwhAllOffStateAPI(i))
                                {
                                    //记录需要切除的电池组(原因:充放电限制)
                                    sGroupStateInfo.sepGroup[sepNum] = i;
                                    sGroupStateInfo.sepReason[sepNum] = eSEP_REASON_SYSLIM;
                                    sepNum++;
                                }
                            }
                            //电池组正常适合运行
                            else
                            {
                                //记录完全正常的电池组
                                fitNum++;
                            }
                        }
                    }
                    //电池组充放电保护
                    else
                    {
                        //计算充放电保护电池组数
                        sProNum++;
                        
                        //电池组运行接触器闭合
                        if(FALSE == GetPGroupRunSwhAllOffStateAPI(i))
                        {
                            //记录需要切除的电池组(原因:电池组故障)
                            sGroupStateInfo.sepGroup[sepNum] = i;
                            sGroupStateInfo.sepReason[sepNum] = eSEP_REASON_ERR;
                            sepNum++;
                        }
                    }
                }
                //电池组未初始化完成或已关机
                else
                {
                    //记录电池组未初始化结束
                    intFlag = 1;
                }
            }
            //通讯中断
            else
            {
                //该组电池运行或运行接触器闭合
                if((eWORK_RUN == GetPGroupWorkStateAPI(i))
                    || (FALSE == GetPGroupRunSwhAllOffStateAPI(i)))
                {
                    //记录需要切除的电池组(原因:通讯中断)
                    sGroupStateInfo.sepGroup[sepNum] = i;
                    sGroupStateInfo.sepReason[sepNum] = eSEP_REASON_OFF;
                    sepNum++;
                }
            }
        }
        //该组未激活使能
        else
        {
            //该组电池运行或运行接触器闭合
            if((1 == GetPGroupCanRcvStateAPI(i))
                && ((eWORK_RUN == GetPGroupWorkStateAPI(i))
                || (FALSE == GetPGroupRunSwhAllOffStateAPI(i))))
            {
                //记录需要切除的电池组(原因:未使能)
                sGroupStateInfo.sepGroup[sepNum] = i;
                sGroupStateInfo.sepReason[sepNum] = eSEP_REASON_DIS;
                sepNum++;
            }
        }

        //系统故障(包括未激活的)
        if(eWORK_ERR == GetPGroupWorkStateAPI(i))
        {
        	BitSet(errGroup, i);
        }
    }
    
    //显示电池组状态
    gPGroupState_57[ePGroupState57_NorG] = norGroup;
    gPGroupState_57[ePGroupState57_RunG] = useGroup;
    gPGroupState_57[ePGroupState57_ErrG] = errGroup;
    
    //记录电池组各状态组数
    sGroupStateInfo.norGNum = norNum;
    sGroupStateInfo.runGNum = runNum;
    sGroupStateInfo.sepGNum = sepNum;
    
    //判断电池组状态
    //有电池组在初始化
    if(intFlag > 0)
    {
        //电池组未初始化就绪
        sGroupStateInfo.workGState = 0;
        sGroupStateInfo.groupState = ePGPRO_STA_WAIT;
    }
    //电池组数据就绪
    else
    {
        //使能通讯正常的电池组均正常或无需要保护的电池组
        if((norNum >= allNum)
            || (0 == sProNum))
        {
            //电池组正常
            sGroupStateInfo.workGState = 1;
        }
        else
        {
            //故障隔离使能
            if(1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
            {
                //正常电池组符合最小运行组数
                if(norNum >= gSysGenParaRO_100[eSGenPara100_MinGNum])
                {
                    //电池组正常
                    sGroupStateInfo.workGState = 1;
                }
                //最小运行组数中有故障组
                else
                {
                    //电池组正常
                    sGroupStateInfo.workGState = 2;
                }
            }
            //故障隔离禁用
            else
            {
                //1组故障电池堆故障
                sGroupStateInfo.workGState = 2;
            }
        }
        
        //故障隔离使能且适合运行的电池组符合最小运行组数(或上电组全部适合运行)
        if((1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
            && ((fitNum >= gSysGenParaRO_100[eSGenPara100_MinGNum])
            || (fitNum >= allNum)))
        {
            //电池组运行数目符合最小运行组(或上电组全部运行)
            if((runNum >= gSysGenParaRO_100[eSGenPara100_MinGNum])
                || (runNum >= allNum))
            {
                //电池组整体正常就绪
                sGroupStateInfo.groupState = ePGPRO_STA_READY;
            }
            //电池组运行数目小于最小运行组
            else
            {
                //电池组整体未就绪
                sGroupStateInfo.groupState = ePGPRO_STA_WAIT;
            }
        }
        //故障禁止或适合运行组数小于最小运行组数(判断是否需要系统故障保护)
        else
        {
            //有电池组需要保护
            if(sProNum > 0)
            {
                //故障隔离使能则判断能否隔离
                if(1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
                {
                    //系统运行正常运行电池组小于最小运行组数
                    if((eWORK_RUN == GetPileWorkStateAPI())
                        && (runNum < gSysGenParaRO_100[eSGenPara100_MinGNum]))
                    {
                        //电池组整体保护
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSPRO;
                    }
                    //系统未运行正常电池组小于最小运行组数
                    else if((eWORK_RUN != GetPileWorkStateAPI())
                        && (norNum < gSysGenParaRO_100[eSGenPara100_MinGNum]))
                    {
                        //电池组整体保护
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSPRO;
                    }
                    //电池组未充放电保护
                    else
                    {
                        //电池组状态未确定
                        batFlag = 1;
                    }
                }
                //故障隔离禁用则1组异常电池堆异常
                else
                {
                    //电池组整体保护
                    sGroupStateInfo.groupState = ePGPRO_STA_SYSPRO;
                }
            }
            //无充放电保护电池组
            else
            {
                //电池组状态未确定
                batFlag = 1;
            }
        }
        
        //电池组状态未确定(判断是否需要系统充放电保护)
        if(batFlag > 0)
        {
            batFlag = 0;
            
            //电池组有放电保护和充电保护
            if((dProNum > 0) && (cProNum > 0))
            {
                //故障隔离使能则判断能否隔离
                if(1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
                {
                    //充放电保护组数和过多
                    if((dProNum + cProNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                    {
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSPRO;
                    }
                    //充放电保护剩余电池组数超过最小运行组数
                    else
                    {
                        //电池组状态未确定
                        batFlag = 1;
                    }
                }
                //故障隔离禁用则有电池组充电和放电保护则系统充放电保护
                else
                {
                    //电池组整体保护
                    sGroupStateInfo.groupState = ePGPRO_STA_SYSPRO;
                }
            }
            //不同时出现有充电保护和放电保护电池组
            else
            {
                //电池组状态未确定
                batFlag = 1;
            }
        }
        
        //电池组状态未确定(判断是否需要系统放电保护)
        if(batFlag > 0)
        {
            batFlag = 0;
            
            //电池组有放电保护
            if(dProNum > 0)
            {
                //故障隔离使能则判断能否隔离
                if(1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
                {
                    //放电保护组数过多(剩余组小于最小运行组数)
                    if((dProNum + sLimNum+ dLimNum + emptyNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                    {
                        //同时有充电保护
                        if((cProNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_SYSPRO;
                        }
                        //同时有充电限制
                        else if((cProNum + sLimNum+ cLimNum + fullNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                        }
                        //只有放电保护
                        else
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_DHGPRO;
                        }
                    }
                    //放电保护剩余电池组数超过最小运行组数
                    else
                    {
                        //电池组状态未确定
                        batFlag = 1;
                    }
                }
                //故障隔离禁用则有电池组放电保护则系统放电保护
                else
                {
                    //同时有充电保护
                    if(cProNum > 0)
                    {
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSPRO;
                    }
                    //同时有充电限制
                    else if((cProNum + sLimNum+ cLimNum + fullNum) > 0)
                    {
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                    }
                    //只有放电保护
                    else
                    {
                        //电池组整体保护
                        sGroupStateInfo.groupState = ePGPRO_STA_DHGPRO;
                    }
                }
            }
            //不出现有放电保护电池组
            else
            {
                //电池组状态未确定
                batFlag = 1;
            }
        }
        
        //电池组状态未确定(判断是否需要系统充电保护)
        if(batFlag > 0)
        {
            batFlag = 0;
            
            //电池组有充电保护
            if(cProNum > 0)
            {
                //故障隔离使能则判断能否隔离
                if(1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
                {
                    //充电保护组数过多(剩余组小于最小运行组数)
                    if((cProNum + sProNum + sLimNum+ cLimNum + fullNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                    {
                        //同时有放电保护
                        if((dProNum + sProNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_SYSPRO;
                        }
                        //同时有放电限制
                        else if((dProNum + sProNum + sLimNum+ dLimNum + emptyNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                        }
                        //只有充电保护
                        else
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_CHGPRO;
                        }
                    }
                    //充电保护剩余电池组数超过最小运行组数
                    else
                    {
                        //电池组状态未确定
                        batFlag = 1;
                    }
                }
                //故障隔离禁用则有电池组充电保护则系统充电保护
                else
                {
                    //同时有放电保护
                    if((dProNum + sProNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                    {
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSPRO;
                    }
                    //同时有放电限制
                    else if((dProNum + sProNum + sLimNum+ dLimNum + emptyNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                    {
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                    }
                    //只有充电保护
                    else
                    {
                        //电池组整体保护
                        sGroupStateInfo.groupState = ePGPRO_STA_CHGPRO;
                    }
                }
            }
            //不出现有充电保护电池组
            else
            {
                //电池组状态未确定
                batFlag = 1;
            }
        }
        
        //电池组状态未确定(判断是否需要系统充放电限制)
        if(batFlag > 0)
        {
            batFlag = 0;
            
            //电池组有充放电限制
            if(sLimNum > 0)
            {
                //故障隔离使能则判断能否隔离
                if(1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
                {
                    //充放电限制组数过多(剩余组小于最小运行组数)
                    if((sLimNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                    {
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                    }
                    //充放电限制剩余电池组数超过最小运行组数
                    else
                    {
                        //电池组状态未确定
                        batFlag = 1;
                    }
                }
                //故障隔离禁用则有电池组充放电限制则系统充放电限制
                else
                {
                    //电池组整体保护
                    sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                }
            }
            //不出现有充放电限制电池组
            else
            {
                //电池组状态未确定
                batFlag = 1;
            }
        }
        
        //电池组状态未确定(判断是否需要系统放电限制)
        if(batFlag > 0)
        {
            batFlag = 0;
            
            //电池组有放电限制
            if((dLimNum + sLimNum) > 0)
            {
                //故障隔离使能则判断能否隔离
                if(1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
                {
                    //放电限制组数过多(剩余组小于最小运行组数)
                    if((dLimNum + sLimNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                    {
                        //同时也有充电限制
                        if((cLimNum + sLimNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                        }
                        //只有放电限制
                        else
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_DHGLIM;
                        }
                    }
                    //放电限制剩余电池组数超过最小运行组数
                    else
                    {
                        //电池组状态未确定
                        batFlag = 1;
                    }
                }
                //故障隔离禁用则有电池组放电限制则系统放电限制
                else
                {
                    //同时也有充电限制
                    if(cLimNum > 0)
                    {
                        //电池组整体限制
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                    }
                    //只有放电限制
                    else
                    {
                        sGroupStateInfo.groupState = ePGPRO_STA_DHGLIM;
                    }
                }
            }
            //不出现有放电限制电池组
            else
            {
                //电池组状态未确定
                batFlag = 1;
            }
        }
        
        //电池组状态未确定(判断是否需要系统充电限制)
        if(batFlag > 0)
        {
            batFlag = 0;
            
            //电池组有充电限制
            if((cLimNum + sLimNum) > 0)
            {
                //故障隔离使能则判断能否隔离
                if(1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
                {
                    //充电限制组数过多(剩余组小于最小运行组数)
                    if((cLimNum + sLimNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                    {
                        //同时也有放电限制不能切除
                        if((dLimNum + sLimNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                        }
                        //只有充电限制
                        else
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_CHGLIM;
                        }
                    }
                    //充电限制剩余电池组数超过最小运行组数
                    else
                    {
                        //电池组状态未确定
                        batFlag = 1;
                    }
                }
                //故障隔离禁用则有电池组充电限制则系统充电限制
                else
                {
                    //同时也有放电限制
                    if(dLimNum > 0)
                    {
                        //电池组整体保护
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                    }
                    //只有充电限制
                    else
                    {
                        sGroupStateInfo.groupState = ePGPRO_STA_CHGLIM;
                    }
                }
            }
            //不出现有充电限制电池组
            else
            {
                //电池组状态未确定
                batFlag = 1;
            }
        }
        
        //电池组状态未确定(判断是否系统放空)
        if(batFlag > 0)
        {
            batFlag = 0;
            
            //电池组有放空
            if((emptyNum + dLimNum + sLimNum) > 0)
            {
                //故障隔离使能则判断能否隔离
                if(1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
                {
                    //放空组数过多(剩余组小于最小运行组数)
                    if((emptyNum + dLimNum + sLimNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                    {
                        //同时也有充满
                        if((fullNum + cLimNum + sLimNum) > 0)
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                        }
                        //只有放空
                        else
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_EMPTY;
                        }
                    }
                    //未放空剩余电池组数超过最小运行组数
                    else
                    {
                        //电池组状态未确定
                        batFlag = 1;
                    }
                }
                //故障隔离禁用则有电池组放空则系统放空
                else
                {
                    //同时也有充满
                    if((fullNum + cLimNum) > 0)
                    {
                        //电池组整体限制
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                    }
                    //只有放空
                    else
                    {
                        sGroupStateInfo.groupState = ePGPRO_STA_EMPTY;
                    }
                }
            }
            //不出现有放电限制电池组
            else
            {
                //电池组状态未确定
                batFlag = 1;
            }
        }
        
        //电池组状态未确定(判断是否系统充满)
        if(batFlag > 0)
        {
            batFlag = 0;
            
            //电池组有充满
            if((fullNum + cLimNum + sLimNum) > 0)
            {
                //故障隔离使能则判断能否隔离
                if(1 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
                {
                    //充满组数过多(剩余组小于最小运行组数)
                    if((fullNum + cLimNum + sLimNum + gSysGenParaRO_100[eSGenPara100_MinGNum]) > norNum)
                    {
                        //同时也有放空
                        if((emptyNum + dLimNum + sLimNum) > 0)
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                        }
                        //只有充满
                        else
                        {
                            sGroupStateInfo.groupState = ePGPRO_STA_FULL;
                        }
                    }
                    //未充满剩余电池组数超过最小运行组数
                    else
                    {
                        //电池组运行数目符合最小运行组
                        if(runNum >= gSysGenParaRO_100[eSGenPara100_MinGNum])
                        {
                            //电池组整体正常就绪
                            sGroupStateInfo.groupState = ePGPRO_STA_READY;
                        }
                        //电池组运行数目小于最小运行组
                        else
                        {
                            //电池组整体未就绪
                            sGroupStateInfo.groupState = ePGPRO_STA_WAIT;
                        }
                    }
                }
                //故障隔离禁用则有电池组充满则系统充满
                else
                {
                    //同时也有放空
                    if((emptyNum + dLimNum) > 0)
                    {
                        //电池组整体限制
                        sGroupStateInfo.groupState = ePGPRO_STA_SYSLIM;
                    }
                    //只有充满
                    else
                    {
                        sGroupStateInfo.groupState = ePGPRO_STA_FULL;
                    }
                }
            }
            //不出现有放电限制电池组
            else
            {
                //电池组运行数目符合最小运行组
                if(runNum >= gSysGenParaRO_100[eSGenPara100_MinGNum])
                {
                    //电池组整体正常就绪
                    sGroupStateInfo.groupState = ePGPRO_STA_READY;
                }
                //电池组运行数目小于最小运行组
                else
                {
                    //电池组整体未就绪
                    sGroupStateInfo.groupState = ePGPRO_STA_WAIT;
                }
            }
        }
    }
}

//=============================================================================================
//函数名称	: void PileProStateCheckTask(void)
//输入参数	: void 
//输出参数	: void
//函数功能	: 检测电池堆保护状态任务
//注意事项	: 100ms任务
//=============================================================================================
void PileProStateCheckTask(void)
{
    u8 state = 0;
    u8 level = 0;
    u8 system = 0;
    
    level = GetPileNowErrSortLevelAPI();
    system = (u8)GetPileWorkStateAPI();
    
    //系统故障
    if((eWORK_ERR == system)
        || (eErr_SysErr == level))
    {
        //电池堆请求充放电保护
        state = ePRO_SYSPRO;
    }
    //系统有充放电保护故障请求保护或电池组请求充放电保护
    else if((eErr_SysPro == level)
        || (GetPileAskSysProState() > 0)
        || (ePGPRO_STA_SYSPRO == GetPileAllGProStateAPI()))
    {
        //电池堆请求充放电保护
        state = ePRO_SYSPRO;
    }
    //系统放电保护
    else if((eErr_DhgPro == level)
        || (GetPileAskDhgProState() > 0)
        || (ePGPRO_STA_DHGPRO == GetPileAllGProStateAPI()))
    {
        //电池堆请求充放电保护
        //state = ePRO_DHGPRO;
        state = ePRO_SYSPRO;
    }
    //系统充电保护
    else if((eErr_ChgPro == level)
        || (GetPileAskChgProState() > 0)
        || (ePGPRO_STA_CHGPRO == GetPileAllGProStateAPI()))
    {
        //电池堆请求充电保护
        //state = ePRO_CHGPRO;
        state = ePRO_SYSPRO;
    }
    //系统充放电限制
    else if((eErr_SysLim == level)
        || (ePGPRO_STA_SYSLIM == GetPileAllGProStateAPI()))
    {
        //电池堆请求充放电限制
        state = ePRO_SYSLIM;
    }
    //系统充放电限制
    else if(((eErr_ChgLim == level)
        && ((ePGPRO_STA_DHGLIM == GetPileAllGProStateAPI())
        || (ePGPRO_STA_EMPTY == GetPileAllGProStateAPI())))
        || ((eErr_DhgLim == level)
        && ((ePGPRO_STA_CHGLIM == GetPileAllGProStateAPI())
        || (ePGPRO_STA_FULL == GetPileAllGProStateAPI()))))
    {
        //电池堆请求充放电限制
        state = ePRO_SYSLIM;
    }
    //系统放电限制
    else if((eErr_DhgLim == level)
        || (ePGPRO_STA_DHGLIM == GetPileAllGProStateAPI()))
    {
        //电池堆请求放电限制
        //state = ePRO_DHGLIM;
        state = ePRO_SYSLIM;
    }
    //系统充电限制
    else if((eErr_ChgLim == level)
        || (ePGPRO_STA_CHGLIM == GetPileAllGProStateAPI()))
    {
        //电池堆请求充电限制
        state = ePRO_SYSLIM;
    }
    //系统电池放空
    else if(ePGPRO_STA_EMPTY == GetPileAllGProStateAPI())
    {
        //电池堆电池放空
        state = ePRO_EMPTY;
    }
    //系统电池充满
    else if(ePGPRO_STA_FULL == GetPileAllGProStateAPI())
    {
        //电池堆电池充满
        state = ePRO_FULL;
    }
    //系统电池就绪
    else if(ePGPRO_STA_READY == GetPileAllGProStateAPI())
    {
        //系统运行
        if(eWORK_RUN == system)
        {
            //电池堆电池正常
            state = ePRO_READY;
        }
        //系统未运行
        else
        {
            //电池堆电池未就绪
            state = ePRO_WAIT;
        }
    }
    //系统电池未就绪
    else
    {
        //电池堆电池未就绪(系统运行时有电池组上电)
        //state = ePRO_WAIT;
        //系统运行
        if(eWORK_RUN == system)
        {
            //电池堆电池正常
            state = ePRO_READY;
        }
        //系统未运行
        else
        {
            //电池堆电池未就绪
            state = ePRO_WAIT;
        }
    }
    
    if(state != gPSysStateInfo_50[ePStateInfo50_SysPro])
    {
        //BattProStateChanEventRecord(gPSysStateInfo_50[ePStateInfo50_SysPro], state);
    }
    
    gPSysStateInfo_50[ePStateInfo50_SysPro] = state;
}

//=============================================================================================
//函数名称	: void PileChgStateCheckTask(void)
//输入参数	: void 
//输出参数	: void
//函数功能	: 检测电池堆充放电状态任务
//注意事项	: 100ms任务
//=============================================================================================
void PileChgStateCheckTask(void)
{
    u8 i = 0;
    u8 num = 0;
    u8 chgNum = 0;
    u8 disNum = 0;
    s16 curr = 0;
    u16 state = 0;
    static u8 sTimes = 0;
    static u8 sChgFlag = 0;
    static u8 sDisFlag = 0;
    
    curr = GetPileOutSumCurrAPI();                                  //电池堆电流
    
    if(curr > (s16)gPSmuGenParaR_102[eSmuGenPara102_IdleDhgC])      //堆放电电流大于5A放电
    {
        sDisFlag++;
    }
    else if(curr < (0 - (s16)gPSmuGenParaR_102[eSmuGenPara102_IdleChgC]))//堆充电电流大于5A充电
    {
        sChgFlag++;
    }
    else                                                            //堆小电流则检测组间是否环流
    {
        for(i = 0; i < GetPileRunGroupNumAPI(); i++)                //检测运行组
        {
            num = GetPileRunGroupIdAPI(i);
            if(eCURR_DHG == GetPGroupChgStateAPI(num))
            {
                disNum++;
            }
            else if(eCURR_CHG == GetPGroupChgStateAPI(num))
            {
                chgNum++;
            }
        }
        
        if((disNum > 0) && (0 == chgNum))       //有组放电且无充电则认为堆放电(无环流)
        {
            sDisFlag++;
        }
        else if((chgNum > 0) && (0 == disNum))  //无有组充电无放电则认为堆充电(无环流)
        {
            sChgFlag++;
        }
    }
    
    if(sTimes >= 20)                            //检测2s
    {
        if(sDisFlag >= 15)                      //长时间处于放电状态
        {
            state = eCURR_DHG;
        }
        else if(sChgFlag >= 15)                 //长时间处于充电状态
        {
            state = eCURR_CHG;
        }
        else                                    //长时间无电流
        {
            state = eCURR_IDLE;
        }
        
        //状态清零
        sChgFlag = 0;
        sDisFlag = 0;
        sTimes = 0;   
    }
    else                                        //未达到检测周期保持原状态
    {
        state = gPSysStateInfo_50[ePStateInfo50_SysChg];
    }
    
    //充放电状态变化
    if(state != gPSysStateInfo_50[ePStateInfo50_SysChg])
    {
        //ChgDchgStateChanEventRecord(gPSysStateInfo_50[ePStateInfo50_SysChg], state);
    }
    
    sTimes++;
    gPSysStateInfo_50[ePStateInfo50_SysChg] = state;
}

//=============================================================================================
//函数名称	: u8 GetPileGCurrCircStateAPI(void)
//输入参数	: void
//输出参数	: 电池组电流环流状态 0:未环流 1环流
//函数功能	: 检测电池堆内部电池组电流环流状态
//注意事项	: 
//=============================================================================================
u8 GetPileGCurrCircStateAPI(void)
{
	u8 i = 0;
	u8 group = 0;
	u8 state = 0;
	u8 runNum = 0;
	
	runNum = GetPileRunGroupNumAPI();               //正在运行的电池数
	
	//只有系统运行时才有环流
	if((eWORK_RUN == GetPileWorkStateAPI()) && (runNum > 0))
	{
		for(i = 0; i < runNum; i++)					//找到组电流是否有充电和放电
		{
			group = GetPileRunGroupIdAPI(i);        //运行电池组号
			
			if(GetPGroupOutCurrAPI(group) < -10)	//电池组充电(大于1A)
			{
				if(2 == state)						//已记录有放电电池组
				{
					return(1);						//返回有环流
				}
				state = 1;							//记录有电池组充电
			}
			else if(GetPGroupOutCurrAPI(group) > 10)//电池组放电(大于1A)
			{
				if(1 == state)						//已记录有充电电池组
				{
					return(1);						//返回有环流
				}
				state = 2;							//记录有电池组放电
			}
		}
	}
	
	return(0);
}

//=============================================================================================
//函数名称	: u8 GetPileAllGSysStateAPI(void)
//输入参数	: void
//输出参数	: 电池组系统总状态 0初始化 1正常 2故障
//函数功能	: 获取电池堆内电池组系统总状态
//注意事项	: 
//=============================================================================================
u8 GetPileAllGSysStateAPI(void)
{
    return(sGroupStateInfo.workGState);
}

//=============================================================================================
//函数名称	: u8 GetPileAllGProStateAPI(void)
//输入参数	: void
//输出参数	: 电池组总保护状态 0未就绪 1正常 2充满 3放空 4充电限制 5放电限制 6充放电限制 7充电保护 8放电保护 9充放电保护
//函数功能	: 获取系统电池组总保护状态
//注意事项	: 
//=============================================================================================
u8 GetPileAllGProStateAPI(void)
{
    return(sGroupStateInfo.groupState);
}

//=============================================================================================
//函数名称	: u8 GetPileNorGroupNumAPI(void)
//输入参数	: void
//输出参数	: 正常无故障的电池组数
//函数功能	: 获取系统正常无故障可用电池组组数目
//注意事项	: 非充放电保护组
//=============================================================================================
u8 GetPileNorGroupNumAPI(void)
{
    return(sGroupStateInfo.norGNum);
}

//=============================================================================================
//函数名称	: u8 GetPileNorGroupIdAPI(u8 index)
//输入参数	: index:正常组序列索引号
//输出参数	: 正常无故障的电池组组号
//函数功能	: 获取电池堆正常无故障的可用电池组组号
//注意事项	: 非充放电保护组
//=============================================================================================
u8 GetPileNorGroupIdAPI(u8 index)
{
    u8 reslut = 0;
    
    if((index < gSysGenParaRO_100[eSGenPara100_AllGNum])
    	&& (index < sGroupStateInfo.norGNum))
    {
        reslut = sGroupStateInfo.norGroup[index];
    }
    else
    {
        reslut = 0xff;
    }
    
    return(reslut);
}

//=============================================================================================
//函数名称	: u8 GetPileRunGroupNumAPI(void)
//输入参数	: void
//输出参数	: 正在运行的电池组数
//函数功能	: 获取系统电池组运行组数目
//注意事项	: 正常无保护组
//=============================================================================================
u8 GetPileRunGroupNumAPI(void)
{
    return(sGroupStateInfo.runGNum);
}

//=============================================================================================
//函数名称	: u8 GetPileRunGroupIdAPI(u8 index)
//输入参数	: index:运行组序列索引号
//输出参数	: 正在运行的电池组组号
//函数功能	: 获取电池堆正在运行的电池组组号
//注意事项	: 正常无保护组
//=============================================================================================
u8 GetPileRunGroupIdAPI(u8 index)
{
    u8 reslut = 0;
    
    if((index < gSysGenParaRO_100[eSGenPara100_AllGNum])
    	&& (index < sGroupStateInfo.runGNum))
    {
        reslut = sGroupStateInfo.runGroup[index];
    }
    else
    {
        reslut = 0xff;
    }
    
    return(reslut);
}

//=============================================================================================
//函数名称	: u8 GetPileSepGroupNumAPI(void)
//输入参数	: void
//输出参数	: 需要切除的电池组数
//函数功能	: 获取系统需要切除的电池组数目
//注意事项	: 
//=============================================================================================
u8 GetPileSepGroupNumAPI(void)
{
    return(sGroupStateInfo.sepGNum);
}

//=============================================================================================
//函数名称	: u8 GetPileSepGroupIdAPI(u8 index)
//输入参数	: index:需切除组序列索引号
//输出参数	: 需要切除的电池组组号
//函数功能	: 获取电池堆正在运行的电池组组号
//注意事项	:
//=============================================================================================
u8 GetPileSepGroupIdAPI(u8 index)
{
    u8 reslut = 0;

    if((index < gSysGenParaRO_100[eSGenPara100_AllGNum])
    	&& (index < sGroupStateInfo.sepGNum))
    {
        reslut = sGroupStateInfo.sepGroup[index];
    }
    else
    {
        reslut = 0xff;
    }

    return(reslut);
}

//=============================================================================================
//函数名称	: u8 GetPileSepGroupReasonAPI(u8 index)
//输入参数	: index:需切除组序列索引号
//输出参数	: 需要切除的电池组切除原因
//函数功能	: 获取电池堆需要切除的电池组切除原因
//注意事项	: 
//=============================================================================================
u8 GetPileSepGroupReasonAPI(u8 index)
{
    u8 reslut = 0;
    
    if((index < gSysGenParaRO_100[eSGenPara100_AllGNum])
    	&& (index < sGroupStateInfo.sepGNum))
    {
        reslut = sGroupStateInfo.sepReason[index];
    }
    else
    {
        reslut = 0;
    }
    
    return(reslut);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
