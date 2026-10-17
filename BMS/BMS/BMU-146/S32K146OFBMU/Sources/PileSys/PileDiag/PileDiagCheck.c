/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileDiagCheck.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控故障诊断故障判定接口模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicPara.h"
#include "PilePara.h"
#include "PublicInfo.h"
#include "PileInfo.h"
#include "PileUserAPI.h"
#include "PileGroupRcv.h"
#include "PileSysState.h"
#include "PileDiagUser.h"
#include "PileDiagMaker.h"
#include "PileDiagCheck.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sCanRcvBsuMsgFlag = 0;            //接收到BSMU信息标志(本电池堆)
static u8 sBsuEcuRepFlag = 0;               //BSMU设备地址冲突标志[0正常 1重复]

static u8 sBsuRcvPcsCount = 0;              //接收到上层PCS信息计数器
static u8 sBsuPcsCanOffFlag = 0;            //BSMU与PCS CAN断线标志[0正常 1断线]

static u16 sBsuRcvBcuCount = 0;             //BSMU接收到BCU信息计数(按位表示)
static u16 sBsuBcuCanOffFlag = 0;           //BSMU与BCU CAN断线标志(按位表示)
static u16 sBsuRcvBcuMsgFlag = 0;           //BSMU接收CAN信息状态(按位表示[0未接收到 1接收到])

static u16 sSwhErrGroup = 0;                //接触器状态异常的电池组(按位表示)
static u16 sPwrLOffGroup = 0;               //动力线断线故障的电池组(按位表示)
static u8 sGSumVDiffFlag = 0;               //电池组间压差过大告警标志

extern u16 gSysGenParaRO_100[];  			/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];  			/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void PileRcvPcsMsgCountAddAPI(u8 addr)
//输入参数	: void
//输出参数	: void
//函数功能	: 接收到上层充放电设备通讯信息计数自加
//注意事项	: 与PCS通讯接收信息
//=============================================================================================
void PileRcvPcsMsgCountAddAPI(u8 addr)
{
    //if(addr == gSysGenParaRO_100[eSGenPara100_ExtAddr])
    if(addr == (PCS_BAS_DEVADDR + (gPSmuGenParaR_102[eSmuGenPara102_SmuDev] - SMU_BAS_DEVADDR)))
    {
        sBsuRcvPcsCount++;
        
        if(sBsuRcvPcsCount > 200)       //防止越界
        {
            sBsuRcvPcsCount = 200;
        }
    }
}

//=============================================================================================
//函数名称	: void PileRcvGroupMsgCountAddAPI(u8 group)
//输入参数	: group:电池组号[0,15]
//输出参数	: void
//函数功能	: 接收到各电池组主控CAN信息计数自加
//注意事项	: 堆控与堆下电池组主控通讯状态
//=============================================================================================
void PileRcvGroupMsgCountAddAPI(u8 group)
{
    if(group >= PILE_MAX_G_NUM)
    {
        return;
    }
    
    BitSet(sBsuRcvBcuCount, group);
}

//=============================================================================================
//函数名称	: void CheckRcvPileAddrRepAPI(u8 addr)
//输入参数	: addr:接收到的设备地址
//输出参数	: void
//函数功能	: 堆控设备地址重复冲突检测
//注意事项	: 接收堆控设备地址报文时调用
//=============================================================================================
void CheckRcvPileAddrRepAPI(u8 addr)
{
    static u16 sTime = 0;
    
    //接收到BSMU设备地址与自身设备地址相同
    if(addr == gPSmuGenParaR_102[eSmuGenPara102_SmuDev])
    {
        sTime = 0;
        
        //记录接收到BSMU信息
        sCanRcvBsuMsgFlag = 1;
        
        //本系统已成为BSMU
        if(1 == GetPileSysRunFlagAPI())
        {
            //记录设备地址冲突
            sBsuEcuRepFlag = 1;
        }
    }
    //预留接收3000条报文的时间
    else if(sTime >= 3000)
    {
        //记录未接收到BSMU信息
        sCanRcvBsuMsgFlag = 0;
        
        //清除BSMU设备地址冲突该组
        sBsuEcuRepFlag = 0;
    }
    else
    {
        sTime++;
    }
}

//=============================================================================================
//函数名称	: void PileAllCommuExpDiagTask(void)
//输入参数	: void 
//输出参数	: void
//函数功能	: 堆控系统通信断线诊断任务
//注意事项	: 1s周期
//=============================================================================================
void PileAllCommuExpDiagTask(void)
{
    u8 i = 0;
    static u8 sPcsTime = 0;
    static u8 sNetTime[PILE_MAX_G_NUM] = {0};
    
    //上电前5s等待CAN信息
    if((BSPGetOSMillSecTimeAPI() / 1000) <= SMU_WAIT_GCAN_TIME)
    {
        for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
        {
            //未接收到信息则等待否则开始检测
            if((i != GetSysSelfGroupIdAPI())
                && (0 == BitGet(sBsuRcvBcuCount, i)))
            {
                return;
            }
            else
            {
            	//记录接收到该组CAN信息
                BitSet(sBsuRcvBcuMsgFlag, i);
            }
        }
    }
    
    //判断上层PCS通讯状态
    if(1 == SMU_EXT_COM_DEV)                                                //需要检测外部PCS通讯异常
    {
        sPcsTime++;
        
        if(0 == sBsuRcvPcsCount)                                            //未接收到信息
        {
            if(sPcsTime >= PERR_SYS_PCS_CANOFF_T)                           //达到检测时间
            {
                sBsuPcsCanOffFlag = 1;                                      //设置与PCS通讯断线
                
                sPcsTime = PERR_SYS_PCS_CANOFF_T;                           //防止越界
            }
        }
        else
        {
            sBsuPcsCanOffFlag = 0;                                          //设置与PCS通讯正常
            
            sBsuRcvPcsCount = 0;
            sPcsTime = 0;
        }
    }
    else
    {
        sBsuPcsCanOffFlag = 0;                                              //设置与PCS通讯正常
    }
    
    //检测同网主控通讯状态
    for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)            //对同网所有BCU进行CAN断线检测
    {
        sNetTime[i]++;
        
        if((i != GetSysSelfGroupIdAPI())                                    //非自身节点号
            && (0 == BitGet(sBsuRcvBcuCount, i)))                           //未接收到信息
        {
            if(sNetTime[i] >= 3)                                            //3s未收到报文
            {
                BitClr(sBsuRcvBcuMsgFlag, i);                               //记录未接收到该组CAN信息
            }
            
            if(sNetTime[i] >= PERR_SYS_BCU_CANOFF_T)                        //达到检测时间
            {
                BitClr(sBsuRcvBcuMsgFlag, i);                               //记录未接收到该组CAN信息
                
                BitSet(sBsuBcuCanOffFlag, i);                               //设置同网该BCUCAN断线
                
                sNetTime[i] = PERR_SYS_BCU_CANOFF_T;                        //防止越界
            }
        }
        else
        {
            BitSet(sBsuRcvBcuMsgFlag, i);                                   //记录接收到该组CAN信息
            
            BitClr(sBsuBcuCanOffFlag, i);                                   //清除该BCU CAN断线
            
            sNetTime[i] = 0;
        }
    }
    sBsuRcvBcuCount = 0;                                                    //清除同网该BCUCAN接收标志
}

//=============================================================================================
//函数名称	: void PileGroupExpDiagTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池组异常状态诊断任务
//注意事项	: 100ms
//=============================================================================================
void PileGroupExpDiagTask(void)
{
    u8 i = 0;
    u8 group = 0;
    u8 runNum = 0;
    u8 offGroup = 0;
    static u8 sOffLErrT = 0;
    static u8 sOffLClrT = 0;
    static u16 sRunGroup = 0;
    
    u16 value = 0;
    u16 maxVolt = 0;
    u16 minVolt = 0x7fff;
    u8 maxGroup = 0;
    u8 minGroup = 0;
    
    runNum = GetPileRunGroupNumAPI();                       //正在运行的电池数
    
    //检测电池组动力线断线
    if((eWORK_RUN == GetPileWorkStateAPI()) && (runNum > 1))//电池堆两组以上运行
    {
        //非充放电状态最大最小值压差超过安全压差
        if((eCURR_IDLE == GetPileChgStateAPI()) && (ABS(GetPileGMaxSumVoltAPI(), GetPileGMinSumVoltAPI()) > (gPSmuGenParaR_102[eSmuGenPara102_UpSafeV] + 30)))
        {
        	//找出运行组最大最小总电压
            for(i = 0; i < runNum; i++)
            {
                group = GetPileRunGroupIdAPI(i);        //运行电池组号

                if(GetPGroupSSumVoltAPI(group) > maxVolt)
                {
                	maxVolt = GetPGroupSSumVoltAPI(group);
                	maxGroup = group;
                }

                if(GetPGroupSSumVoltAPI(group) < minVolt)
                {
                	minVolt = GetPGroupSSumVoltAPI(group);
                	minGroup = group;
                }
            }

            //堆运行组总电压压压差过大可能有动力线断线
            if(ABS(maxVolt, minVolt) > (gPSmuGenParaR_102[eSmuGenPara102_UpSafeV] + 30))
            {
                //最高电压组与电池堆压差比较大为动力线断线
                if(ABS(maxVolt, GetPileSampSumVoltAPI()) > ABS(minVolt, GetPileSampSumVoltAPI()))
                {
                    BitSet(offGroup, maxGroup);//记录断线电池组
                }
                else
                {
                    BitSet(offGroup, minGroup);//记录断线电池组
                }
            }
        }
//        //堆运行充放电时组电流为0(会误报)
//        else if(eCURR_IDLE != GetPileChgStateAPI())
//        {
//            for(i = 0; i < runNum; i++)                 //找到组电流变为0的组号
//            {
//                group = GetPileRunGroupIdAPI(i);        //运行电池组号
//
//                //求运行组最大电流绝对值
//                value = ABS(GetPileGMaxOutCurrAPI(), 0);
//
//                if((1 == IsInside(-10, GetPGroupOutCurrAPI(group), 10))//电池组空闲
//                    && (0 == GetPileGCurrCircStateAPI())              //电池堆无环流
//                    && ((value >= (PWR_L_OFF_G_CURR * 2))             //最大电流电池组超过20A
//                    || ((value >= PWR_L_OFF_G_CURR)                   //最大电流电池组超过10A
//                    && (((eCURR_CHG == GetPileChgStateAPI())          //电池堆充电
//                    && (GetPGroupSSumVoltAPI(group) <= GetPileGMinSumVoltAPI()))//空闲电池组总电压最小
//                    || ((eCURR_DHG == GetPileChgStateAPI())           //电池堆放电
//                    && (GetPGroupSSumVoltAPI(group) >= GetPileGMaxSumVoltAPI()))))))//空闲电池组总电压最大
//                {
//                    BitSet(offGroup, group);                          //记录断线电池组
//                    break;
//                }
//            }
//        }
    }
    
    //发生疑似动力线断线
    if(offGroup > 0)
    {
    	sOffLClrT = 0;

        //滤波5s判定故障
    	if(sOffLErrT >= 50)
    	{
    		sOffLErrT = 0;
    	    sPwrLOffGroup = offGroup;
    	}
    	else
    	{
    		sOffLErrT++;
    	}
    }
    //动力线正常
    else
    {
    	sOffLErrT = 0;

        //延迟10s清除故障
    	if(sOffLClrT >= 100)
    	{
    		sOffLClrT = 0;
    	    sPwrLOffGroup = 0;
    	}
    	else
    	{
    		sOffLClrT++;
    	}
    }
    
    //判断电池组间压差过大
    group = 0;
    maxVolt = 0;
    minVolt = 0x7fff;
    for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
    {
        if((GetPGroupEnStateAPI(i) == 1)                //组使能
            && (GetPGroupCanOffStateAPI(i) == 0)        //组通讯正常
            && (eWORK_INIT != GetPGroupWorkStateAPI(i)) //组非初始化
            && (eWORK_ERR != GetPGroupWorkStateAPI(i))) //组非故障
        {
            //求运行组总电压最大值和最小值
            value = GetPGroupCSumVoltAPI(i);
            if(value > maxVolt)
            {
                maxVolt = value;
            }
            if(value < minVolt)
            {
                minVolt = value;
            }
            //计算正常电池组
            group++;
        }
    }
    
    //有正常电池组
    if(group > 0)
    {
        //组间压差超过启动安全压差则告警
        if(ABS(maxVolt, minVolt) > gPSmuGenParaR_102[eSmuGenPara102_UpSafeV])
        {
            sGSumVDiffFlag = 1;
        }
        //组间压差低于启动安全压差-1v则消除告警
        else if((ABS(maxVolt, minVolt) + 10) <= gPSmuGenParaR_102[eSmuGenPara102_UpSafeV])
        {
            sGSumVDiffFlag = 0;
        }
    }

	//判断电池组接触器状态
	//堆初始化/待命状态:组接触器全断开(初始化控制断开)
	if(/*(eWORK_INIT == GetPileWorkStateAPI())
		||*/ (eWORK_IDLE == GetPileWorkStateAPI()))
	{
		for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
		{
			//组正常
			if(((1 == GetPGroupEnStateAPI(i))               //组使能
				|| (1 == GetPGroupIsoSwhStateAPI(i)))       //断路器闭合
				&& (0 == GetPGroupCanOffStateAPI(i)))       //组通讯正常
			{
				//组接触器未全断开
				if(0 == GetPGroupRunSwhAllOffStateAPI(i))
				{
					//记录接触器异常
					BitSet(sSwhErrGroup, i);
				}
				else
				{
					//清除接触器异常
					BitClr(sSwhErrGroup, i);
				}
			}
		}
		sRunGroup = 0;
	}
	//堆运行状态:在堆控运行序列但组接触器断开
	else if(eWORK_RUN == GetPileWorkStateAPI())
	{
		//未记录运行组电池
		if(0 == sRunGroup)
		{
			//获取运行组电池
			sRunGroup = GetPAllGroupRunStateAPI();
		}
		//运行组变化
		else if(GetPAllGroupRunStateAPI() != sRunGroup)
		{
			for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
			{
				//电池组原来运行但当前未运行
				if((1 == BitGet(sRunGroup, i)) && (0 == BitGet(GetPAllGroupRunStateAPI(), i)))
				{
					//原运行组仍正常
					if(((1 == GetPGroupEnStateAPI(i))       //组使能
						|| (1 == GetPGroupIsoSwhStateAPI(i)))//断路器闭合
						&& (0 == GetPGroupCanOffStateAPI(i)))//组通讯正常
					{
						//非故障切除
						if((ePRO_WAIT == GetPGroupProStateAPI(i)
							|| ((0 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
							&& (0 == gSysGenParaRO_100[eSGenPara100_ChgSepa])
							&& (0 == gSysGenParaRO_100[eSGenPara100_DhgSepa]))))
						{
							//记录接触器异常
							BitSet(sSwhErrGroup, i);
						}
					}
				}
			}
			//更新运行组电池
			sRunGroup = GetPAllGroupRunStateAPI();
		}
	}
	//堆故障状态:组接触器全断开则消除故障
	else if(eWORK_ERR == GetPileWorkStateAPI())
	{
		for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
		{
			//组正常
			if(((1 == GetPGroupEnStateAPI(i))               //组使能
				|| (1 == GetPGroupIsoSwhStateAPI(i)))       //断路器闭合
				&& (0 == GetPGroupCanOffStateAPI(i)))       //组通讯正常
			{
				//组接触器未全断开
				if(1 == GetPGroupRunSwhAllOffStateAPI(i))
				{
					//清除接触器异常
					BitClr(sSwhErrGroup, i);
				}
			}
		}
	}
}

//=============================================================================================
//函数名称	: u8 GetPileAddrRepErrFlag(void)
//输入参数	: void
//输出参数	: 0正常 1堆控设备地址重复
//函数功能	: 获取堆控设备地址重复标志
//注意事项	: 
//=============================================================================================
u8 GetPileAddrRepErrFlag(void)
{
    return(sBsuEcuRepFlag);
}

/*=================================================================================================
** @Name      : u8 GetPileRcvSmuMsgFlag(void)
** @Input     : void
** @Output    : 接收到相同电池堆堆控信息标志 0未接收 1接收到
** @Function  : 获取接收到相同电池堆堆控信息标志
** @The notes : 判别是否已有堆控启用
**===============================================================================================*/
u8 GetPileRcvSmuMsgFlag(void)
{
    return(sCanRcvBsuMsgFlag);
}

//=============================================================================================
//函数名称	: u8 GetPRcvGroupMsgFlagAPI(void)
//输入参数	: void
//输出参数	: 接收到主控通讯CAN信息标志(按位表示[0未接收 1接收到])
//函数功能	: 获取接收到主控通讯CAN信息标志
//注意事项	: 包括自身和未激活使能组
//=============================================================================================
u16 GetPRcvGroupMsgFlagAPI(void)
{
    return(sBsuRcvBcuMsgFlag);
}

//=============================================================================================
//函数名称	: u16 GetPileGroupCanOffFlag(void)
//输入参数	: void
//输出参数	: 各组主控通讯CAN断线标志(按位表示) 0正常 1断线
//函数功能	: 获取各组主控通讯CAN断线标志
//注意事项	: 包括自身,未激活使能组不上报
//=============================================================================================
u16 GetPileGroupCanOffFlag(void)
{
    return(sBsuBcuCanOffFlag);
}

//=============================================================================================
//函数名称	: u8 GetPilePcsCanOffFlag(void)
//输入参数	: void
//输出参数	: PCS CAN断线标志  0:正常 1:断线
//函数功能	: 获取堆控系统与PCS CAN断线标志
//注意事项	: 
//=============================================================================================
u8 GetPilePcsCanOffFlag(void)
{
    return(sBsuPcsCanOffFlag);
}

//=============================================================================================
//函数名称	: u8 GetPileGSumVDiffFlag(void)
//输入参数	: void
//输出参数	: 电池组压差过大标志 0:正常 1:压差过大
//函数功能	: 获取电池组压差过大标志
//注意事项	: 
//=============================================================================================
u8 GetPileGSumVDiffFlag(void)
{
    return(sGSumVDiffFlag);
}

//=============================================================================================
//函数名称	: u16 GetPileGroupPwrLOffFlag(void)
//输入参数	: void
//输出参数	: 按位表示电池组动力线断线标志 1:断线 0:正常
//函数功能	: 获取电池组动力线断线标志
//注意事项	: 
//=============================================================================================
u16 GetPileGroupPwrLOffFlag(void)
{
    return(sPwrLOffGroup);
}

//=============================================================================================
//函数名称	: u16 GetPileGSwhStateErrFlag(void)
//输入参数	: void
//输出参数	: 按位表示电池组接触器状态异常标志 1:异常 0:正常
//函数功能	: 获取电池组接触器状态异常标志
//注意事项	:
//=============================================================================================
u16 GetPileGSwhStateErrFlag(void)
{
    return(sSwhErrGroup);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
