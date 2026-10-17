/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileLimCalc.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控限值计算模块
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
#include "PileSysState.h"
#include "PileTactInterface.h"
#include "PileDataCalc.h"
#include "PileLimCalc.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u32 sExtReqSafeLimCurr = 0xffff;    //外部请求目标安全电流 0.1A

extern u16 gPLimCPInfo_51[];				/*电流功率限值数组*/
extern u16 gPLimVInfo_52[];					/*电压限值信息数组*/

extern u16 gSysGenParaRO_100[];  			/*系统通用参数设置*/
extern u16 gPSmuGenParaR_102[];  			/*堆控通用参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
//=============================================================================================
//函数名称	: static void CalcPileNorChgDhgLimCurr(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 计算电池堆正常状态充放电电流限值
//注意事项	: 包括充放电功率限值
//=============================================================================================
static void CalcPileNorChgDhgLimCurr(void);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void PileSysVoltLimCalcTask(void)
//输入参数	: void 
//输出参数	: void 
//函数功能	: 计算电池堆充放电电压限值任务
//注意事项	: 100ms
//=============================================================================================
void PileSysVoltLimCalcTask(void)
{
	u8 i = 0;
	u8 num = 0;
	u8 group = 0;
	u16 chgLimV = 0xefff;
	u16 dhgLimV = 0;
	
    num = GetPileRunGroupNumAPI();              //获取正在运行的电池组数目
    
    //无电池组运行按照电池组总压限制电压
    if((eWORK_RUN != GetPileWorkStateAPI())     //系统未处于运行状态
        || (0 == num))					        //无在线电池组
    {
        //显示电池堆电压
    	gPLimVInfo_52[ePLimVInfo52_ChgV] = GetPileSampSumVoltAPI();
    	gPLimVInfo_52[ePLimVInfo52_DhgV] = GetPileSampSumVoltAPI();
    }
    //有电池组运行取电池组最小电压限值
    else
    {
        //检测剩余电池组
        for(i = 0; i < num; i++)
        {
            group = GetPileRunGroupIdAPI(i);                    //运行电池组号
            
            if((1 == GetPGroupEnStateAPI(group))                //组使能
                && (0 == GetPGroupCanOffStateAPI(group))        //组通讯正常
                && (1 == GetPGroupIsoSwhStateAPI(group))        //组隔离开关闭合
                && (eWORK_RUN == GetPGroupWorkStateAPI(group))) //组运行
            {
                //充电求非0最小值
                if((GetPGroupChgLimVoltAPI(group) > 0)
                	&& (GetPGroupChgLimVoltAPI(group) < chgLimV))
                {
                    chgLimV = GetPGroupChgLimVoltAPI(group);
                }

                //放电求非0最大值
                if((GetPGroupDhgLimVoltAPI(group) > 0)
                    && (GetPGroupDhgLimVoltAPI(group) > dhgLimV))
                {
                    dhgLimV = GetPGroupDhgLimVoltAPI(group);
                }
            }	
        }
        
        gPLimVInfo_52[ePLimVInfo52_ChgV] = chgLimV;
        gPLimVInfo_52[ePLimVInfo52_DhgV] = dhgLimV;
    }
}

//=============================================================================================
//函数名称	: void PileSysCurrLimCalcTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 计算堆充放电电流限值任务
//注意事项	: 100ms
//=============================================================================================
void PileSysCurrLimCalcTask(void)
{
	//无电池组运行充放电限值为0
    if((ePGPRO_STA_SYSPRO == GetPileAllGProStateAPI())      //系统电池组故障保护
        || (ePGPRO_STA_SYSLIM == GetPileAllGProStateAPI())  //系统电池组充放电限制
        //|| (ePGPRO_STA_WAIT == GetPileAllGProStateAPI())  //系统电池组未就绪(允许运行过程有电池组上电)
        || (eWORK_RUN != GetPileWorkStateAPI())             //系统未处于运行状态
        || (0 == GetPileRunGroupNumAPI()))					//无在线电池组
    {
    	gPLimCPInfo_51[ePLimCPInfo51_ChgC] = 0;
    	gPLimCPInfo_51[ePLimCPInfo51_DhgC] = 0;
    	gPLimCPInfo_51[ePLimCPInfo51_ChgP] = 0;
    	gPLimCPInfo_51[ePLimCPInfo51_DhgP] = 0;
    }
    //有电池组运行正常计算
    else
    {
        //正常计算充放电限值
        CalcPileNorChgDhgLimCurr();
        
        //系统充电限制
        if((ePGPRO_STA_FULL == GetPileAllGProStateAPI())
            || (ePGPRO_STA_CHGLIM == GetPileAllGProStateAPI())
            || (ePGPRO_STA_CHGPRO == GetPileAllGProStateAPI()))
        {
            //充电限值为0
        	gPLimCPInfo_51[ePLimCPInfo51_ChgC] = 0;
            gPLimCPInfo_51[ePLimCPInfo51_ChgP] = 0;
        }
        //系统放电限制
        else if((ePGPRO_STA_EMPTY == GetPileAllGProStateAPI())
            || (ePGPRO_STA_DHGLIM == GetPileAllGProStateAPI())
            || (ePGPRO_STA_DHGPRO == GetPileAllGProStateAPI()))
        {
            //放电限值为0
        	gPLimCPInfo_51[ePLimCPInfo51_DhgC] = 0;
        	gPLimCPInfo_51[ePLimCPInfo51_DhgP] = 0;
        }
        
        //充电功率限值超过系统允许最大功率
        if(gPLimCPInfo_51[ePLimCPInfo51_ChgP] > gPSmuGenParaR_102[eSmuGenPara102_ChgMaxP])
        {
            //取系统最大功率限值
        	gPLimCPInfo_51[ePLimCPInfo51_ChgP] = gPSmuGenParaR_102[eSmuGenPara102_ChgMaxP];
            
            //计算系统最大功率对应的电流限值
        	gPLimCPInfo_51[ePLimCPInfo51_ChgC] = (u16)((u32)gPLimCPInfo_51[ePLimCPInfo51_ChgP] * 10000 / (u32)GetPileSampSumVoltAPI());
        }
        
        //放电功率限值超过系统允许最大功率
        if(gPLimCPInfo_51[ePLimCPInfo51_DhgP] > gPSmuGenParaR_102[eSmuGenPara102_DhgMaxP])
        {
            //取系统最大功率限值
        	gPLimCPInfo_51[ePLimCPInfo51_DhgP] = gPSmuGenParaR_102[eSmuGenPara102_DhgMaxP];
            
            //计算系统最大功率对应的电流限值
        	gPLimCPInfo_51[ePLimCPInfo51_DhgC] = (u16)((u32)gPLimCPInfo_51[ePLimCPInfo51_DhgP] * 10000 / (u32)GetPileSampSumVoltAPI());
        }
    }
}

//=============================================================================================
//函数名称	: void SetExtReqPileSafeLimCAPI(u16 curr)
//输入参数	: curr:外部请求目标安全电流值 0.1A
//输出参数	: void
//函数功能	: 设置外部请求目标安全电流值
//注意事项	: 
//=============================================================================================
void SetExtReqPileSafeLimCAPI(u16 curr)
{
    sExtReqSafeLimCurr = curr;
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: static void CalcPileNorChgDhgLimCurr(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 计算电池堆正常状态充放电电流限值
//注意事项	: 包括充放电功率限值
//=============================================================================================
static void CalcPileNorChgDhgLimCurr(void)
{
    u8 i = 0;
    u8 num = 0;
    u8 group = 0;
    u8 chgNum = 0;
    u8 dhgNum = 0;
    u8 chgFit = 0;
    u8 dhgFit = 0;
    u16 tempC = 0;
    u16 chgOvrC = 0;
    u16 dhgOvrC = 0;
    u16 chgLimC = 0;
    u16 dhgLimC = 0;
    u16 chgMinC = 0xffff;
    u16 dhgMinC = 0xffff;
    u16 realChgLimC = 0;
    u16 realDhgLimC = 0;
    
    num = GetPileRunGroupNumAPI();
    
    //没有正在运行的组
    if(0 == num)
    {
    	gPLimCPInfo_51[ePLimCPInfo51_ChgC] = 0;
    	gPLimCPInfo_51[ePLimCPInfo51_DhgC] = 0;
    	gPLimCPInfo_51[ePLimCPInfo51_ChgP] = 0;
    	gPLimCPInfo_51[ePLimCPInfo51_DhgP] = 0;
    }
    else
    {
        //逐组找出运行组不能充放电的组数和最小充电限值
        for(i = 0; i < num; i++)
        {
            group = GetPileRunGroupIdAPI(i);                //运行电池组号
        
            //检测正常运行电池组
            if((1 == GetPGroupEnStateAPI(group))            //组使能
                && (0 == GetPGroupCanOffStateAPI(group))    //组通讯正常
                && (eWORK_RUN == GetPGroupWorkStateAPI(group)))//组运行
            {
                chgLimC = GetPGroupChgLimCurrAPI(group);
                dhgLimC = GetPGroupDhgLimCurrAPI(group);
                
                //计算出充电电流限值为0的个数及最小充电电流限值
                if(0 == chgLimC)
                {
                    chgNum++;
                }
                //电流限值非0
                else
                {
                    //求最小充电电流限值
                    if(chgLimC < chgMinC)
                    {
                        chgMinC = chgLimC;
                    }
                    
                    //当前为充电状态
                    if(GetPGroupOutCurrAPI(group) < 0)
                    {
                        //出现电池组充电过流(超出组充电限值)
                        if((0 - GetPGroupOutCurrAPI(group)) > chgLimC)
                        {
							//求超出的电流值
							tempC = (0 - GetPGroupOutCurrAPI(group)) - chgLimC;

							//超出的电流最大值
							if(tempC > chgOvrC)
							{
								chgOvrC = tempC;
							}
                        }
                        //电池组充电电流低于限值且在5A内
                        else if(ABS((0 - GetPGroupOutCurrAPI(group)), chgLimC) <= G_CURR_FIT_SUBLIM)
                        {
                            //记录电池组充电电流合适
                        	chgFit = 1;
                        }
                    }
                }
                
                //计算出放电电流限值为0的个数及最小放电电流限值
                if(0 == dhgLimC)
                {
                    dhgNum++;
                }
                //电流限值非0
                else
                {
                    //求最小电流限值
                    if(dhgLimC < dhgMinC)
                    {
                        dhgMinC = dhgLimC;
                    }
                    
                    //当前为放电状态
                    if(GetPGroupOutCurrAPI(group) > 0)
                    {
                        //出现电池组放电过流(超出组放电限值)
                        if(GetPGroupOutCurrAPI(group) > dhgLimC)
                        {
							//求超出的电流值
							tempC = GetPGroupOutCurrAPI(group) - dhgLimC;

							//超出的电流最大值
							if(tempC > dhgOvrC)
							{
								dhgOvrC = tempC;
							}
                        }
                        //电池组放电电流低于限值且在5A内
                        else if(ABS(GetPGroupOutCurrAPI(group), dhgLimC) <= G_CURR_FIT_SUBLIM)
                        {
                            //记录电池组放电电流合适
                        	dhgFit = 1;
                        }
                    }
                }
            }
        }
        
        //有电池组不能充电且不能切除
        if((chgNum > 0) && (((0 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
            && (0 == gSysGenParaRO_100[eSGenPara100_ChgSepa]))
            || ((num - chgNum) < gSysGenParaRO_100[eSGenPara100_MinGNum])))
        {
        	gPLimCPInfo_51[ePLimCPInfo51_ChgC] = 0;
        }
        //计算剩余电池组充电限值
        else
        {
            //响应降限值需求
            if(chgMinC > sExtReqSafeLimCurr)
            {
                chgMinC = sExtReqSafeLimCurr;
            }
            
            //计算组总最小电流限值
            tempC = chgMinC * (num - chgNum);

            //电池组未过流或过流部分可减小
            if(chgMinC > chgOvrC)
            {
                //堆控充电且总电流过流(5A偏差)
                if((GetPileOutSumCurrAPI() < 0)
                	&& ((0 - GetPileOutSumCurrAPI()) > (gPLimCPInfo_51[ePLimCPInfo51_ChgC] + 50)))
                {
                    //堆控已限流到0或组总电流限值变小(组降限值)
                	if((0 == gPLimCPInfo_51[ePLimCPInfo51_ChgC])
                		|| (tempC < gPLimCPInfo_51[ePLimCPInfo51_ChgC]))
                	{
                		//取组总最小电流限值
                		//gPLimCPInfo_51[ePLimCPInfo51_ChgC] = tempC;
                		realChgLimC = tempC;
                	}
                    //堆控已无过流电池组(组释放限值)
                	else if(0 == chgOvrC)
                	{
                		/*当前有电池组电流合适(与限值接近[5A内])或有电池组需要切除*/
                		if((1 == chgFit) || ((chgNum > 0) && (tempC > gPLimCPInfo_51[ePLimCPInfo51_ChgC])))
                		{
                            //保持当前请求电流限值
                        	//gPLimCPInfo_51[ePLimCPInfo51_ChgC] = gPLimCPInfo_51[ePLimCPInfo51_ChgC];
                    		realChgLimC = gPLimCPInfo_51[ePLimCPInfo51_ChgC];
                		}
    					//电池组电流不合适(小于限值5A以上)
                		else
                		{
                			//取组总最小电流限值
                			//gPLimCPInfo_51[ePLimCPInfo51_ChgC] = tempC;
                    		realChgLimC = tempC;
                		}
                	}
                    //组总电流限值变大且仍有电池组过流
                	else
                	{
                        //保持当前请求电流限值
                    	//gPLimCPInfo_51[ePLimCPInfo51_ChgC] = gPLimCPInfo_51[ePLimCPInfo51_ChgC];
                		realChgLimC = gPLimCPInfo_51[ePLimCPInfo51_ChgC];
                	}
                }
                //堆控放电或充电总电流未过流
                else
                {
					//有电池组过流
                	if(chgOvrC > 0)
                	{
						//计算削去过流部分后的限值
						//gPLimCPInfo_51[ePLimCPInfo51_ChgC] = (chgMinC - chgOvrC) * (num - chgNum);
						//gPLimCPInfo_51[ePLimCPInfo51_ChgC] = gPLimCPInfo_51[ePLimCPInfo51_ChgC] - (chgOvrC * (num - chgNum));
						//gPLimCPInfo_51[ePLimCPInfo51_ChgC] = gPLimCPInfo_51[ePLimCPInfo51_ChgC] - chgOvrC;//梯次下降
                		realChgLimC = gPLimCPInfo_51[ePLimCPInfo51_ChgC] - chgOvrC;//梯次下降
                	}
					//无电池组过流
                	else
                	{
    					//电池组电流合适(与限值接近[5A内])或有电池组需要切除
                		if((1 == chgFit) || ((chgNum > 0) && (tempC > gPLimCPInfo_51[ePLimCPInfo51_ChgC])))
                		{
                            //保持当前请求电流限值
                        	//gPLimCPInfo_51[ePLimCPInfo51_ChgC] = gPLimCPInfo_51[ePLimCPInfo51_ChgC];
                    		realChgLimC = gPLimCPInfo_51[ePLimCPInfo51_ChgC];
                		}
    					//电池组电流不合适(小于限值5A以上)
                		else
                		{
                            //按照最小限值请求(放电或组充电限值放大)
                        	//gPLimCPInfo_51[ePLimCPInfo51_ChgC] = tempC;
                    		realChgLimC = tempC;
                		}
                	}
                }
            }
            //电池组严重不均流过流
            else
            {
                //按照最小限值请求
            	//gPLimCPInfo_51[ePLimCPInfo51_ChgC] = tempC;
        		realChgLimC = tempC;
            }

            //限制电流限值增大时梯度增大
            if((gPLimCPInfo_51[ePLimCPInfo51_ChgC] > 0)
            	&& (realChgLimC > (gPLimCPInfo_51[ePLimCPInfo51_ChgC] + 10)))
            {
            	//20s内递增完成
            	realChgLimC = (realChgLimC - gPLimCPInfo_51[ePLimCPInfo51_ChgC]) / 200;
            	if(0 == realChgLimC)
            	{
            		realChgLimC = 1;
            	}

            	gPLimCPInfo_51[ePLimCPInfo51_ChgC] += realChgLimC;
            }
            //电流限值变小或由0释放
            else
            {
            	//直接显示目标值
            	gPLimCPInfo_51[ePLimCPInfo51_ChgC] = realChgLimC;
            }
        }
        
        //有电池组不能放电且不能切除
        if((dhgNum > 0) && (((0 == gSysGenParaRO_100[eSGenPara100_ErrSepa])
            && (0 == gSysGenParaRO_100[eSGenPara100_DhgSepa]))
            || ((num - dhgNum) < gSysGenParaRO_100[eSGenPara100_MinGNum])))
        {
        	gPLimCPInfo_51[ePLimCPInfo51_DhgC] = 0;
        }
        //计算剩余电池组放电限值
        else
        {
            //响应降限值需求
            if(dhgMinC > sExtReqSafeLimCurr)
            {
                dhgMinC = sExtReqSafeLimCurr;
            }
            
            //计算组总最小电流限值
            tempC = dhgMinC * (num - dhgNum);

            //电池组未过流或过流部分可减小
            if(dhgMinC > dhgOvrC)
            {
                //堆控放电且总电流过流(5A偏差)
                if((GetPileOutSumCurrAPI() > 0)
                	&& (GetPileOutSumCurrAPI() > (gPLimCPInfo_51[ePLimCPInfo51_DhgC] + 50)))
                {
                    //堆控已限流到0或组总电流限值变小(组降限值)
                	if((0 == gPLimCPInfo_51[ePLimCPInfo51_DhgC])
                		|| (tempC < gPLimCPInfo_51[ePLimCPInfo51_DhgC]))
                	{
                        //取组总最小电流限值
                    	//gPLimCPInfo_51[ePLimCPInfo51_DhgC] = tempC;
                		realDhgLimC = tempC;
                	}
                    //堆控已无过流电池组(组释放限值)
                	else if(0 == chgOvrC)
                	{
    					//电池组电流合适(与限值接近[5A内])或有电池组需要切除
                		if((1 == dhgFit) || ((dhgNum > 0) && (tempC > gPLimCPInfo_51[ePLimCPInfo51_DhgC])))
                		{
                            //保持当前请求电流限值
                        	//gPLimCPInfo_51[ePLimCPInfo51_DhgC] = gPLimCPInfo_51[ePLimCPInfo51_DhgC];
                    		realDhgLimC = gPLimCPInfo_51[ePLimCPInfo51_DhgC];
                		}
    					//电池组电流不合适(小于限值5A以上)
                		else
                		{
                            //按照最小限值请求(充电或组放电限值放大)
                        	//gPLimCPInfo_51[ePLimCPInfo51_DhgC] = tempC;
                    		realDhgLimC = tempC;
                		}
                	}
                    //组总电流限值变大且仍有电池组过流
                	else
                	{
                        //保持当前请求电流限值
                    	//gPLimCPInfo_51[ePLimCPInfo51_DhgC] = gPLimCPInfo_51[ePLimCPInfo51_DhgC];
                		realDhgLimC = gPLimCPInfo_51[ePLimCPInfo51_DhgC];
                	}
                }
                //堆控充电或放电总电流未过流
                else
                {
					//有电池组过流
                	if(dhgOvrC > 0)
                	{
						//计算削去过流部分后的限值
						//gPLimCPInfo_51[ePLimCPInfo51_DhgC] = (dhgMinC - dhgOvrC) * (num - dhgNum);
						//gPLimCPInfo_51[ePLimCPInfo51_DhgC] = gPLimCPInfo_51[ePLimCPInfo51_DhgC] - (dhgOvrC * (num - dhgNum));
						//gPLimCPInfo_51[ePLimCPInfo51_DhgC] = gPLimCPInfo_51[ePLimCPInfo51_DhgC] - dhgOvrC;//梯次下降
                		realDhgLimC = gPLimCPInfo_51[ePLimCPInfo51_DhgC] - dhgOvrC;//梯次下降
                	}
					//无电池组过流
                	else
                	{
    					//电池组电流合适(与限值接近[5A内])或有电池组需要切除
                		if((1 == dhgFit) || ((dhgNum > 0) && (tempC > gPLimCPInfo_51[ePLimCPInfo51_DhgC])))
                		{
                            //保持当前请求电流限值
                        	//gPLimCPInfo_51[ePLimCPInfo51_DhgC] = gPLimCPInfo_51[ePLimCPInfo51_DhgC];
                    		realDhgLimC = gPLimCPInfo_51[ePLimCPInfo51_DhgC];
                		}
    					//电池组电流不合适(小于限值5A以上)
                		else
                		{
                            //按照最小限值请求(放电或组充电限值放大)
                        	//gPLimCPInfo_51[ePLimCPInfo51_DhgC] = tempC;
                    		realDhgLimC = tempC;
                		}
                	}
                }
            }
            //电池组严重不均流过流
            else
            {
                //按照最小限值请求
            	//gPLimCPInfo_51[ePLimCPInfo51_DhgC] = tempC;
        		realDhgLimC = tempC;
            }

            //限制电流限值增大时梯度增大
            if((gPLimCPInfo_51[ePLimCPInfo51_DhgC] > 0)
            	&& (realDhgLimC > (gPLimCPInfo_51[ePLimCPInfo51_DhgC] + 10)))
            {
            	//20s内递增完成
            	realDhgLimC = (realDhgLimC - gPLimCPInfo_51[ePLimCPInfo51_DhgC]) / 200;
            	if(0 == realDhgLimC)
            	{
            		realDhgLimC = 1;
            	}

            	gPLimCPInfo_51[ePLimCPInfo51_DhgC] += realDhgLimC;
            }
            //电流限值变小或由0释放
            else
            {
            	//直接显示目标值
            	gPLimCPInfo_51[ePLimCPInfo51_DhgC] = realDhgLimC;
            }
        }

        //计算堆控对应功率限值
        gPLimCPInfo_51[ePLimCPInfo51_ChgP] = (u16)((u32)gPLimCPInfo_51[ePLimCPInfo51_ChgC] * (u32)GetPileSampSumVoltAPI() / 10000);
        gPLimCPInfo_51[ePLimCPInfo51_DhgP] = (u16)((u32)gPLimCPInfo_51[ePLimCPInfo51_DhgC] * (u32)GetPileSampSumVoltAPI() / 10000);
    }
}
#endif
