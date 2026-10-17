/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileTactics.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控策略控制任务模块
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
#include "PileAutoCtrl.h"
#include "PileWorkState.h"
#include "PileSysState.h"
#include "PileTactInterface.h"
#include "PileTactics.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
t_SYSCTRL_INFO gPileCtrlInfo = {0};     //系统启动停机控制信息

u8 gPileSysCtrlCmd = 0;                 //记录电池堆系统控制命令[0,9]
u8 gPileSysStartStep = 0xff;            //电池堆启动步骤
u8 gPileSysStopStep = 0xff;             //电池堆停机步骤
u8 gPileEmergStopFlag = 0;              //电池堆紧急停机标志(按位表示):Bit0:系统内部急停 Bit1:外部命令急停

u8 gPileSysStartErrFlag = 0;            //电池堆启动异常标志(每bit表示一个异常原因)[0正常,1异常]
u8 gPileSysStopErrFlag = 0;             //电池堆停机异常标志(每bit表示一个异常原因)[0正常,1异常]

extern u16 gSysGenParaRO_100[];  			/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];  			/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
//=============================================================================================
//函数名称	: static void FindSysStopNeedCtrlGroup(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 获取需要控制停机的电池组和电流大小排序 
//注意事项	: 电池堆系统停机时使用
//=============================================================================================
static void FindSysStopNeedCtrlGroup(void);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void PileAllUserInit(void)
** @Input     : void
** @Output    : void
** @Function  : 堆控所有任务初始化
** @The notes : 用户初始化调用
**===============================================================================================*/
//=============================================================================================
//函数名称	: void TacSetPileWorkStateCtrlCmdAPI(u8 cmd)
//输入参数	: cmd:控制命令 0:正常停止 1:正常预充 2:正常启动 3:充电启动 4:放电启动 5:充电停止 6:放电停止 7:紧急停止 8:关机 9:开机
//输出参数	: void
//函数功能	: 设置堆控工作状态切换控制命令
//注意事项	: 
//=============================================================================================
void TacSetPileWorkStateCtrlCmdAPI(u8 cmd)
{
    //命令无效
    if(cmd >= eCTRL_SYSCMD)
    {
        return;
    }
    
    switch(cmd)
    {
        //正常停机命令
        case eCTRL_NORSTOP:
        //强制停机命令
        case eCTRL_FORCSTOP:
            if((eWORK_START == GetPileWorkStateAPI())   //正在启动
                || (eWORK_RUN == GetPileWorkStateAPI()))//正在运行
            {
                gPileSysCtrlCmd = cmd;                  //记录控制命令
                gPileSysStartStep = 0xff;               //清除启动步骤
                gPileSysStopStep = 0;                   //复位停机步骤
                
                TacClrPileCtrlErrFlagAPI();             //清除启停控制故障
                SetPileSysWorkStateAPI(eWORK_STOP);     //切换为正在停机
            }/*
            else if(eWORK_OFF == GetPileWorkStateAPI())//正在关机
            {
                gPileSysCtrlCmd = eCTRL_POWERON;   //记录为开机命令
                gPileSysStartStep = 0xff;               //清除启动步骤
                gPileSysStopStep = 0xff;                //清除停机步骤
                
                TacClrPileCtrlErrFlagAPI();             //清除控制失败标志
                //SetPileSysWorkStateAPI(eWORK_INIT);//切换为初始化状态(由开机完成后切换)
            }*/
            else if((eWORK_ERR == GetPileWorkStateAPI())  //系统故障
                || (eWORK_IDLE == GetPileWorkStateAPI())) //系统空闲
            {
                gPileSysCtrlCmd = cmd;                  //记录控制命令
                gPileSysStartStep = 0xff;               //清除启动步骤
                gPileSysStopStep = 0xff;                //清除停机步骤
                
                if(GetPAllGroupRunStateAPI() > 0)       //有电池组接触器闭合
                {
                    BitSet(gPileEmergStopFlag, ePEStopReason_SysInt);//设置内部急停命令
                }
                else                                    //电池组接触器全断开
                {
                	TacClrPileCtrlErrFlagAPI();         //清除控制失败标志
                }
            }
            break;
            
        //正常预充命令
        case eCTRL_PRECHG:
        //正常启动命令
        case eCTRL_NORSTART:
        //充电启动命令
        case eCTRL_CHGSTART:
        //放电启动命令
        case eCTRL_DHGSTART:
            if((eWORK_IDLE == GetPileWorkStateAPI())  //电池堆空闲
               && (1 == PileGetSysFitStartStateHook()))//适合启动
            {
                gPileSysStartStep = 0;                  //复位启动步骤
                gPileSysStopStep = 0xff;                //清除停机步骤
                
                gPileSysCtrlCmd = cmd;                  //记录控制命令
                TacClrPileCtrlErrFlagAPI();             //清除控制失败标志
                SetPileSysWorkStateAPI(eWORK_START);    //切换为正在启动
            }
            else if((eWORK_START == GetPileWorkStateAPI())//正在启动
                && (eCTRL_PRECHG == gPileSysCtrlCmd)//正在预充
                && (eCTRL_NORSTART == cmd))    //只有预充允许变换正常启动命令
            {
                gPileSysCtrlCmd = cmd;                  //更新控制命令
                
                gPileSysStopStep = 0xff;                //清除停机步骤
            }
            break;
            
        //紧急停机命令
        case eCTRL_EMERSTOP:
            gPileSysStopStep = 0xff;
            gPileSysStartStep = 0xff;
            
            if((eWORK_START == GetPileWorkStateAPI())        //正在启动
                || (eWORK_RUN == GetPileWorkStateAPI())      //正在运行
                || (eWORK_STOP == GetPileWorkStateAPI())     //正在停机
                || ((eWORK_ERR == GetPileWorkStateAPI())     //系统故障
                && (GetPAllGroupRunStateAPI() > 0)))         //有电池组接触器闭合
            {
                gPileSysCtrlCmd = cmd;                       //记录控制命令
                SetPileSysWorkStateAPI(eWORK_ERR);           //切换为故障状态
                BitSet(gPileEmergStopFlag, ePEStopReason_ExtCmd);//记录系统外部命令急停
            }
            break;
            
        //关机机命令
        case eCTRL_POWEROFF:
            gPileSysStartStep = 0xff;
            
            if((eWORK_START == GetPileWorkStateAPI())        //正在启动
                || (eWORK_RUN == GetPileWorkStateAPI()))     //正在运行
            {
                gPileSysStopStep = 0;
                gPileSysCtrlCmd = cmd;                      //记录控制命令
                SetPileSysWorkStateAPI(eWORK_STOP);         //切换为正在停止状态
            }
            else if(eWORK_IDLE == GetPileWorkStateAPI())    //系统空闲
            {
                gPileSysStopStep = 0xff;
                gPileSysCtrlCmd = cmd;                      //记录控制命令
                SetPileSysWorkStateAPI(eWORK_OFF);          //切换为关机状态
            }
            else if((eWORK_ERR == GetPileWorkStateAPI())     //系统故障
                || (eWORK_STOP == GetPileWorkStateAPI()))    //正在停机
            {
                gPileSysCtrlCmd = cmd;                      //记录控制命令
                //SetPileSysWorkStateAPI(eWORK_OFF);        //切换为关机状态(由停机完成后切换)
            }
            break;
            
        //开机命令
        case eCTRL_POWERON:
            
            if((eWORK_OFF == GetPileWorkStateAPI())       //系统关机
                || (eMODE_DEBUG == GetPileWorkModeAPI())) //正在调试
            {
                gPileSysStopStep = 0xff;
                gPileSysStartStep = 0xff;
                
                gPileSysCtrlCmd = cmd;                      //记录控制命令
                TacClrPileCtrlErrFlagAPI();                 //清除控制失败标志
                //SetPileSysWorkStateAPI(eWORK_INIT);    //切换为初始化状态(由开机完成后切换)
            }
            break;
            
        default:
            break;
    }
}

//=============================================================================================
//函数名称	: void PileSysNorStopCtrlTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆系统正常停机控制任务
//注意事项	: 停机状态100ms调用
//=============================================================================================
void PileSysNorStopCtrlTask(void)
{
    u8 index = 0;
    u8 ctrlGoup = 0;
    u8 failGoup = 0;
    static u8 sCheckTime = 0;
    
    switch(gPileSysStopStep)
    {
        //数据复位
        case 0:
            sCheckTime = 0;
            gPileSysStopStep = 1;
            break;
            
        //查找需控制停机电池组(并将电池组号按电流从小到大排序)
        case 1:
            FindSysStopNeedCtrlGroup();
            
            //有电池组需要控制
            if(gPileCtrlInfo.CtrlGNum > 0)
            {
                gPileSysStopStep = 2;
            }
            //电池组停机完成
            else
            {
                gPileSysStopStep = 3;
            }
            break;
            
        //向需要停机电池组发送命令(正常停机需要考虑安全电流)
        case 2:
            for(index = 0; index < gPileCtrlInfo.CtrlGNum; index++)
            {
                //获取电池组组号(先断大电流电池组)
                ctrlGoup = gPileCtrlInfo.CtrlGId[index];
                
                //正常停机或关机(必须安全电流)
                if((eCTRL_NORSTOP == gPileSysCtrlCmd)
                    || (eCTRL_POWEROFF == gPileSysCtrlCmd))
                {
                    //该组电池组电流安全
                    if(1 == GetPGroupMainSafeCStateAPI(ctrlGoup))
                    {
                        //发送正常停机命令
                        PileSetGroupWorkStateCtrlCmdHook(ctrlGoup, eCTRL_NORSTOP);
                    }
                }
                //充电停机或放电停机
                else
                {
                    //直接向该组发送命令
                    PileSetGroupWorkStateCtrlCmdHook(ctrlGoup, gPileSysCtrlCmd);
                }
            }
            
            gPileSysStopStep = 3;
            break;
            
        //判断电池组是否成功
        case 3:
            sCheckTime++;
            
            for(index = 0; index < gPileCtrlInfo.CtrlGNum; index++)
            {
                ctrlGoup = gPileCtrlInfo.CtrlGId[index];
                
                //电池组激活但接触器仍未断开
                if((1 == GetPGroupEnStateAPI(ctrlGoup))
                    && ((eWORK_RUN == GetPGroupWorkStateAPI(ctrlGoup))
                    || (FALSE == GetPGroupRunSwhAllOffStateAPI(ctrlGoup))))
                {
                    BitSet(failGoup, ctrlGoup);
                }
            }
            
            //接触器都已断开或者停机超时
            if((0 == failGoup)
                || (sCheckTime >= gPSmuGenParaR_102[eSmuGenPara102_StopFailT]))
            {
                //记录控制失败的电池组
                gPileCtrlInfo.CtrlFailGroup = failGoup;
                
                //进入停机结果检测
                sCheckTime = 0;
                gPileSysStopStep = 4;
            }
            //仍需继续控制
            else
            {
                //返回判断需要控制的电流
                gPileSysStopStep = 1;
            }
            break;
            
        //判断停机结果
        case 4: 
            sCheckTime++;
            //无停机失败的电池组
            if(0 == gPileCtrlInfo.CtrlFailGroup)
            {
                //等待系统运行信息更新(运行组数为0)
                if((sCheckTime >= 50) 
                    || (0 == GetPileRunGroupNumAPI()))
                {
                    sCheckTime = 0;
                    gPileSysStopStep = 0xff;
                    
                    //关机命令
                    if(eCTRL_POWEROFF == gPileSysCtrlCmd)
                    {
                        //进入关机状态
                        SetPileSysWorkStateAPI(eWORK_OFF);
                    }
                    //非关机命令
                    else
                    {
                        //进入空闲状态
                        SetPileSysWorkStateAPI(eWORK_IDLE);
                    }
                }
            }
            //有电池组停机失败
            else
            {
                //记录电池组停机失败原因
                for(index = 0; index < gSysGenParaRO_100[eSGenPara100_AllGNum]; index++)
                {
                    //该组停机失败
                    if(1 == BitGet(gPileCtrlInfo.CtrlFailGroup, index))
                    {
                        //组电流过大
                        if(0 == GetPGroupMainSafeCStateAPI(index))
                        {   
                            BitSet(gPileSysStopErrFlag, e_CtrlExp_DnCOver);
                        }
                        //组系统故障
                        else if(eWORK_ERR == GetPGroupWorkStateAPI(index))
                        {   
                            BitSet(gPileSysStopErrFlag, e_CtrlExp_DnGroupErr);
                        }
                        //组控制失败
                        else
                        {
                            BitSet(gPileSysStopErrFlag, e_CtrlExp_DnMainCmd);
                        }
                    }
                }
                
                //切换为故障状态
                sCheckTime = 0;
                gPileSysStopStep = 0xff;
                SetPileSysWorkStateAPI(eWORK_ERR);
                        
                //设置内部急停命令
                BitSet(gPileEmergStopFlag, ePEStopReason_SysInt);
            }
            break;
            
        default:
            break;
    }
}

//=============================================================================================
//函数名称	: void PileSysErrStopCtrlTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆系统故障停机(急停)控制任务
//注意事项	: 故障状态100ms调用
//=============================================================================================
void PileSysErrStopCtrlTask(void)
{
    u8 index = 0;
    
    //有接触器闭合运行电池组
    if(GetPAllGroupRunStateAPI() > 0)
    {
    	//有停机命令
    	if((eCTRL_NORSTOP == gPileSysCtrlCmd)
    		|| (eCTRL_FORCSTOP == gPileSysCtrlCmd)
    		|| (eCTRL_EMERSTOP == gPileSysCtrlCmd))
    	{
			//检测电池组是否全部停机
			for(index = 0; index < gSysGenParaRO_100[eSGenPara100_AllGNum]; index++)
			{
				//该组激活使能通讯正常或有通讯
				if(((1 == GetPGroupEnStateAPI(index))
					&& (0 == GetPGroupCanOffStateAPI(index)))
					|| (1 == GetPGroupCanRcvStateAPI(index)))
				{
					//电池组运行
					if((eWORK_RUN == GetPGroupWorkStateAPI(index))
						|| (FALSE == GetPGroupRunSwhAllOffStateAPI(index)))
					{
						//外部急停(紧急停机)
						if((1 == GetPGroupEmStopStateAPI(index))
							|| (1 == BitGet(gPileEmergStopFlag, ePEStopReason_ExtCmd)))
						{
							//发送紧急停机命令
							PileSetGroupWorkStateCtrlCmdHook(index, eCTRL_EMERSTOP);
						}
						//内部急停(正常停机)
						else
						{
							//发送强制停机命令
							PileSetGroupWorkStateCtrlCmdHook(index, eCTRL_FORCSTOP);
						}
					}
				}
			}
    	}
    }
    //无电池组运行
    else
    {
        //关机命令
        if(eCTRL_POWEROFF == gPileSysCtrlCmd)
        {
            //进入关机状态
            SetPileSysWorkStateAPI(eWORK_OFF);
        }
    }
}

//=============================================================================================
//函数名称	: void PileSysPwrOffCtrlTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆系统关机控制任务
//注意事项	: 关机状态100ms调用
//=============================================================================================
void PileSysPwrOffCtrlTask(void)
{
    u8 index = 0;
    u8 ctrlNum = 0;
    
    //关机命令
    if(eCTRL_POWEROFF == gPileSysCtrlCmd)
    {
        //检测电池组是否全部停机
        for(index = 0; index < gSysGenParaRO_100[eSGenPara100_AllGNum]; index++)
        {
            //该组激活使能
            if(1 == GetPGroupEnStateAPI(index))
            {
                //电池组运行
                if((eWORK_RUN == GetPGroupWorkStateAPI(index))
                    || (FALSE == GetPGroupRunSwhAllOffStateAPI(index)))
                {
                    //计算运行组数
                    ctrlNum++;
                    
                    //发送正常停机命令
                    PileSetGroupWorkStateCtrlCmdHook(index, eCTRL_NORSTOP);
                }
            }
            
            //该组有通讯报文
            if(1 == GetPGroupCanRcvStateAPI(index))
            {
                //电池组运行
                if((eWORK_RUN == GetPGroupWorkStateAPI(index))
                    || (FALSE == GetPGroupRunSwhAllOffStateAPI(index)))
                {
                    //发送正常停机命令
                    PileSetGroupWorkStateCtrlCmdHook(index, eCTRL_NORSTOP);
                }
            }
        }
        
        //电池组全部停机
        if(0 == ctrlNum)
        {
            //控制电池组关机
            for(index = 0; index < gSysGenParaRO_100[eSGenPara100_AllGNum]; index++)
            {
                //非自身组号且该组有通讯报文且未进入关机状态
                if((index != GetSysSelfGroupIdAPI())
                    && (1 == GetPGroupCanRcvStateAPI(index))
					&& (eWORK_OFF != GetPGroupWorkStateAPI(index)))
                {
                    //计算组数
                    ctrlNum++;
                    
                    //发送关机命令
                    PileSetGroupWorkStateCtrlCmdHook(index, eCTRL_POWEROFF);
                }
            }
            
            //其他电池组均已关机
            if(0 == ctrlNum)
            {
                //发送自身关机命令
                PileSetGroupWorkStateCtrlCmdHook(GetSysSelfGroupIdAPI(), eCTRL_POWEROFF);
            }
        }
    }
    //开机命令
    else
    {
        //向激活使能组发送开机命令
        for(index = 0; index < gSysGenParaRO_100[eSGenPara100_AllGNum]; index++)
        {
            //该组激活使能通讯正常或有通讯
            if(((1 == GetPGroupEnStateAPI(index))
                && (0 == GetPGroupCanOffStateAPI(index)))
                || (1 == GetPGroupCanRcvStateAPI(index)))
            {
                //电池组关机状态
                if(eWORK_OFF == GetPGroupWorkStateAPI(index))
                {
                    //计算运行组数
                    ctrlNum++;
                    
                    //发送开机命令
                    PileSetGroupWorkStateCtrlCmdHook(index, eCTRL_POWERON);
                }
            }
        }
        
        //电池组全部开机
        if(0 == ctrlNum)
        {
            //清除启停控制故障
        	TacClrPileCtrlErrFlagAPI();
            
            //电池堆系统进入开机初始化状态
            SetPileSysWorkStateAPI(eWORK_INIT);
        }
	}
}

//=============================================================================================
//函数名称	: void TacClrPileCtrlErrFlagAPI(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 清除电池堆系统控制异常故障标志
//注意事项	: 
//=============================================================================================
void TacClrPileCtrlErrFlagAPI(void)
{
    gPileSysStartErrFlag = 0;           //启动失败
    gPileSysStopErrFlag = 0;            //停机失败
    gPileEmergStopFlag = 0;             //急停标志
    
    gPileCtrlInfo.CtrlFailGroup = 0;    //控制失败电池组
    
    ClrPileAutoRunErrFlag();            //动态投切故障不知
}

//=============================================================================================
//函数名称	: u16 GetPileSysCtrlFailGroup(void)
//输入参数	: void
//输出参数	: 电池堆控制失败的电池组(按位表示):0:正常 1:失败
//函数功能	: 获取电池堆控制失败的电池组
//注意事项	: 
//=============================================================================================
u16 GetPileSysCtrlFailGroup(void)
{
    return(gPileCtrlInfo.CtrlFailGroup);
}

//=============================================================================================
//函数名称	: u8 GetPileSysStartErrFlag(void)
//输入参数	: void
//输出参数	: 电池堆启动异常标志(每bit表示1个启动异常原因)[0正常,1异常]
//函数功能	: 获取电池堆启动异常标志
//注意事项	: 
//=============================================================================================
u8 GetPileSysStartErrFlag(void)
{
    return(gPileSysStartErrFlag);
}

//=============================================================================================
//函数名称	: u8 GetPileSysStopErrFlag(void)
//输入参数	: void
//输出参数	: 电池堆停机异常标志(每bit表示1个停机异常原因)[0正常,1异常]
//函数功能	: 获取电池堆停机异常标志
//注意事项	: 
//=============================================================================================
u8 GetPileSysStopErrFlag(void)
{
    return(gPileSysStopErrFlag);
}

//=============================================================================================
//函数名称	: u8 GetPileSysEmergStopFlag(void)
//输入参数	: void
//输出参数	: 系统紧急停机标志:0:正常 >0:急停原因
//函数功能	: 获取系统紧急停机标志
//注意事项	: 按位表示系统紧急停机原因:Bit0:系统内部急停 Bit1:外部命令急停
//=============================================================================================
u8 GetPileSysEmergStopFlag(void)
{
    return(gPileEmergStopFlag);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: static void FindSysStopNeedCtrlGroup(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 获取需要控制停机的电池组和电流大小排序 
//注意事项	: 电池堆系统停机时使用
//=============================================================================================
static void FindSysStopNeedCtrlGroup(void)
{
    u8 i = 0;
    u8 j = 0;
    u16 temp = 0;
    u16 groupCurr[PILE_MAX_G_NUM] = {0};
    
    //控制信息清零
    gPileCtrlInfo.CtrlGNum = 0;
    gPileCtrlInfo.CtrlFailGroup = 0;
    
    //计算需要控制的组数和记录电池组电流
    for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
    {
        //激活或能接收到报文的电池组运行或接触器闭合
        if(((1 == GetPGroupEnStateAPI(i))
            || (1 == GetPGroupCanRcvStateAPI(i)))
            && ((eWORK_RUN == GetPGroupWorkStateAPI(i))
            || (FALSE == GetPGroupRunSwhAllOffStateAPI(i))))
        {
            //电池组电流绝对值大小
            groupCurr[gPileCtrlInfo.CtrlGNum] = ABS(GetPGroupOutCurrAPI(i), 0);
            gPileCtrlInfo.CtrlGId[gPileCtrlInfo.CtrlGNum] = i;
            gPileCtrlInfo.CtrlGNum++;
        }
    }
    
    //电流按照从大到小重新排序
    for(i = 0; i < gPileCtrlInfo.CtrlGNum; i++)
    {
        for(j = i; j < gPileCtrlInfo.CtrlGNum; j++)
        {
            //找出最大的一个放到前面
            if(groupCurr[j] > groupCurr[i])
            {
                //电流位置交换
                temp = groupCurr[i];                //备份前面小的
                groupCurr[i] = groupCurr[j];        //将大的放到前面
                groupCurr[j] = temp;                //将小的放到后面
                
                //组号位置相应交换
                temp = gPileCtrlInfo.CtrlGId[i];
                gPileCtrlInfo.CtrlGId[i] = gPileCtrlInfo.CtrlGId[j];
                gPileCtrlInfo.CtrlGId[j] = (u8)temp;
            }
        }
    }
}

#endif
