/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : SocDisplay.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : SOC平滑显示模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "ParaInterface.h"
#include "CapInfoCalc.h"
#include "CurrIntegral.h"
#include "EnerInterface.h"
#include "SocDisplay.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u16 sSlowDisplaySoc = 0xffff;    /*平滑显示SOC中间计算值(万分之一)*/

extern u16 gGSysCapInfo_61[];			/*系统电量信息数组*/

extern u16 gGBmuGenPara_102[];		    /*主控通用参数值*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 CalcSocChgDhgEndCorr(u16 nowSoc)
** @Input     : aimSoc:当前SOC(万分之一)
** @Output    : 修正后的SOC值(万分之一)
** @Function  : 计算SOC充放电末端修正值
** @The notes :
**===========================================================================================*/
static u16 CalcSocChgDhgEndCorr(u16 nowSoc);

/*=============================================================================================
** @Name      : static u16 CalcSocSlowChang(u16 aimSoc)
** @Input     : aimSoc:目标SOC(万分之一)
** @Output    : 平滑后的SOC值(万分之一)
** @Function  : 计算SOC平滑值
** @The notes :
**===========================================================================================*/
static u16 CalcSocSlowChang(u16 aimSoc);

/*=============================================================================================
** @Name      : static void UpdateSocSlowDisplay(u16 slowSoc)
** @Input     : slowSoc:平滑后待显示的值
** @Output    : void
** @Function  : 将平滑值更新到显示值
** @The notes :
**===========================================================================================*/
static void UpdateSocSlowDisplay(u16 slowSoc);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void GroupSocDisplayInit(void)
** @Input     : void
** @Output    : void
** @Function  : SOC平滑显示初始化
** @The notes : 用户初始化调用
**===========================================================================================*/
void GroupSocDisplayInit(void)
{
    u16 soc = 0;

    soc = gGBmuGenPara_102[eBmuGenPara102_NowSoc] * 10;
    
    /*上电不显示100%和0%*/
    if(soc > 9940)
    {
        soc = 9940;
    }
    else if(soc < 100)
    {
        soc = 100;
    }

    sSlowDisplaySoc = soc;
}

/*=============================================================================================
** @Name      : void GroupSocDisplayTask(void)
** @Input     : void
** @Output    : void
** @Function  : SOC平滑显示任务
** @The notes : 100ms
**===========================================================================================*/
void GroupSocDisplayTask(void)
{
    u16 aimSoc = 0;
    u16 slowSoc = 0;
    
    aimSoc = GetGRealSocTenThousAPI();          /*取当前SOC*/
    
    aimSoc = CalcSocChgDhgEndCorr(aimSoc);      /*充放电末端修正当前SOC*/

    if(0xffff == sSlowDisplaySoc)               /*平滑值未初始化*/
    {
        slowSoc = aimSoc;
    }
    else
    {
        slowSoc = CalcSocSlowChang(aimSoc);     /*计算SOC平滑变化值*/
    }
    
    UpdateSocSlowDisplay(slowSoc);              /*将平滑值更新到显示值*/
}

/*=============================================================================================
** @Name      : void CorrGDisplaySocByUser(u16 soc)
** @Input     : soc:目标SOC(0.01%)
** @Output    : void
** @Function  : 用户修正当前显示SOC
** @The notes :
**===========================================================================================*/
void CorrGDisplaySocByUser(u16 soc)
{
    if(soc > 10000)
    {
        return;
    }
    
    sSlowDisplaySoc = soc;
    
    UpdateSocSlowDisplay(soc);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 CalcSocChgDhgEndCorr(u16 nowSoc)
** @Input     : aimSoc:当前SOC(万分之一)
** @Output    : 修正后的SOC值(万分之一)
** @Function  : 计算SOC充放电末端修正值
** @The notes :
**===========================================================================================*/
static u16 CalcSocChgDhgEndCorr(u16 nowSoc)
{
	u16 aimSoc = nowSoc;
    static u8 sCorrFlag = 0;
    static u32 sIdleTime = 0;

    if(0 == EnerGetGroupBattReadyHook())                    /*单体信息异常未就绪*/
	{
		/*不执行末端修正*/
		sCorrFlag = 0;
		sIdleTime = 0;
		aimSoc = nowSoc;                                    /*返回原值*/
	}
	else if(EnerGetGroupVHProStateHook() > 0)               /*高压一级保护*/
    {
    	/*SOC未达到目标值且未执行修正*/
    	if((1 != sCorrFlag)
    		&& ((nowSoc < SOC_SLOW_VHER_SOC)
    		|| (sSlowDisplaySoc < SOC_SLOW_VHER_SOC)))
    	{
    		CorrGRemainCapBySocAPI(SOC_SLOW_VHER_SOC / 10); /*强制修正为100%*/
    		CorrGNowCapBySocAPI(SOC_SLOW_VHER_SOC / 10);    /*强制修正为100%*/
    		sSlowDisplaySoc = SOC_SLOW_VHER_SOC;            /*强制显示为100%*/
    		aimSoc = SOC_SLOW_VHER_SOC;                     /*记录显示目标100%*/
    		sCorrFlag = 1;                                  /*记录已修正显示*/
    	}
    	else
    	{
    		aimSoc = nowSoc;                               /*记录显示当前值*/
    	}
    	sIdleTime = 0;
    }
    else if(EnerGetGroupVLProStateHook() > 0)               /*低压一级保护*/
    {
    	/*SOC未达到目标值且未执行修正*/
    	if((2 != sCorrFlag)
    		&& ((nowSoc > SOC_SLOW_VLER_SOC)
    		|| (sSlowDisplaySoc > SOC_SLOW_VLER_SOC)))
    	{
    		CorrGRemainCapBySocAPI(SOC_SLOW_VLER_SOC / 10); /*强制修正为0%*/
    		CorrGNowCapBySocAPI(SOC_SLOW_VLER_SOC / 10);    /*强制修正为0%*/
    		sSlowDisplaySoc = SOC_SLOW_VLER_SOC;            /*强制显示为0%*/
    		aimSoc = SOC_SLOW_VLER_SOC;                     /*记录显示目标0%*/
    		sCorrFlag = 2;                                  /*记录已修正显示*/
    	}
    	else
    	{
    		aimSoc = nowSoc;                               /*记录显示当前值*/
    	}
    	sIdleTime = 0;
    }
    else if(EnerGetGroupVHLimStateHook() > 0)               /*高压二级保护*/
    {
    	/*SOC未达到目标值且未执行修正*/
    	if((1 != sCorrFlag) && (3 != sCorrFlag)
    		&& ((nowSoc < SOC_SLOW_VH_SOC)
    		|| (sSlowDisplaySoc < SOC_SLOW_VH_SOC)))
    	{
    		CorrGNowCapBySocAPI(SOC_SLOW_VH_SOC / 10);      /*强制修正为100%*/
    		aimSoc = SOC_SLOW_VH_SOC;                       /*记录显示目标100%*/
    		sCorrFlag = 3;                                  /*记录已修正显示*/

    		if(GetGSampOutCurrAPI() < -5)                   /*正在充电*/
    		{
    			sSlowDisplaySoc = SOC_SLOW_VH_SOC;          /*强制显示为100%*/
    		}
    	}
    	else
    	{
    		aimSoc = nowSoc;                               /*记录显示当前值*/
    	}
    	sIdleTime = 0;
    }
    else if(EnerGetGroupVLLimStateHook() > 0)               /*低压二级保护*/
    {
    	/*SOC未达到目标值且未执行修正*/
    	if((2 != sCorrFlag) && (4 != sCorrFlag)
    		&& ((nowSoc > SOC_SLOW_VL_SOC)
    		|| (sSlowDisplaySoc > SOC_SLOW_VL_SOC)))
    	{
    		CorrGNowCapBySocAPI(SOC_SLOW_VL_SOC / 10);      /*强制修正为3%*/
    		aimSoc = SOC_SLOW_VL_SOC;                       /*记录显示目标3%*/
    		sCorrFlag = 4;                                  /*记录已修正显示*/

    		if(GetGSampOutCurrAPI() > 5)                    /*正在放电*/
    		{
    			sSlowDisplaySoc = SOC_SLOW_VL_SOC;          /*强制显示为3%*/
    		}
    	}
    	else
    	{
    		aimSoc = nowSoc;                                /*记录显示当前值*/
    	}
    	sIdleTime = 0;
    }
    else
    {
		sCorrFlag = 0;                                      /*清除已修正显示*/

    	/*系统待机时修正SOC可信度*/
		if(eCURR_IDLE == GetGChgDhgStateAPI())
		{
	    	/*达到静置时长*/
            if(sIdleTime >= SOC_SLOW_CORR_IDLE_T)
            {
            	/*电压很低处于下区间*/
                if(GetGCellMaxVoltAPI() < SOC_V_SEC_DN_MAX_V)
                {
                	/*当前SOC过高*/
                    if(nowSoc > SOC_V_SEC_DN_MOS_SOC)
                    {
                        /*修正当前SOC为至大可能值*/
                		CorrGNowCapBySocAPI(SOC_V_SEC_DN_MOS_SOC / 10);
                		aimSoc = SOC_V_SEC_DN_MOS_SOC;
                    }
                	/*显示SOC过高*/
                    else if(sSlowDisplaySoc > SOC_V_SEC_DN_MOS_SOC)
                    {
                        /*修正当前SOC为至大可能值*/
                    	sSlowDisplaySoc = SOC_V_SEC_DN_MOS_SOC;
                		aimSoc = nowSoc;
                    }
                    else
                    {
                		aimSoc = nowSoc;
                    }
                }
            	/*电压很高处于上区间*/
                else if(GetGCellMinVoltAPI() > SOC_V_SEC_UP_MIN_V)
                {
                	/*当前SOC过低*/
                    if(nowSoc < SOC_V_SEC_UP_LES_SOC)
                    {
                        /*修正当前SOC为至小可能值*/
                		CorrGNowCapBySocAPI(SOC_V_SEC_UP_LES_SOC / 10);
                		aimSoc = SOC_V_SEC_UP_LES_SOC;
                    }
                	/*显示SOC过低*/
                    else if(sSlowDisplaySoc < SOC_V_SEC_UP_LES_SOC)
                    {
                        /*修正当前SOC为至大可能值*/
                    	sSlowDisplaySoc = SOC_V_SEC_UP_LES_SOC;
                		aimSoc = nowSoc;
                    }
                    else
                    {
                		aimSoc = nowSoc;
                    }
                }
            	/*电压稍高高于下区间*/
                else if(GetGCellMinVoltAPI() > SOC_V_SEC_DN_MIN_V)
                {
                	/*当前SOC过低*/
                    if(nowSoc < SOC_V_SEC_DN_LES_SOC)
                    {
                        /*修正当前SOC为至小可能值*/
                		CorrGNowCapBySocAPI(SOC_V_SEC_DN_LES_SOC / 10);
                		aimSoc = SOC_V_SEC_DN_LES_SOC;
                    }
                	/*显示SOC过低*/
                    else if(sSlowDisplaySoc < SOC_V_SEC_DN_LES_SOC)
                    {
                        /*修正当前SOC为至大可能值*/
                    	sSlowDisplaySoc = SOC_V_SEC_DN_LES_SOC;
                		aimSoc = nowSoc;
                    }
                    else
                    {
                		aimSoc = nowSoc;
                    }
                }
            	/*电压稍低低于上区间*/
                else if(GetGCellMaxVoltAPI() < SOC_V_SEC_UP_MAX_V)
                {
                	/*当前SOC过高*/
                    if(nowSoc > SOC_V_SEC_UP_MOS_SOC)
                    {
                        /*修正当前SOC为至大可能值*/
                		CorrGNowCapBySocAPI(SOC_V_SEC_UP_MOS_SOC / 10);
                		aimSoc = SOC_V_SEC_UP_MOS_SOC;
                    }
                	/*显示SOC过高*/
                    else if(sSlowDisplaySoc > SOC_V_SEC_UP_MOS_SOC)
                    {
                        /*修正当前SOC为至大可能值*/
                    	sSlowDisplaySoc = SOC_V_SEC_UP_MOS_SOC;
                		aimSoc = nowSoc;
                    }
                    else
                    {
                		aimSoc = nowSoc;
                    }
                }
                else
                {
            		aimSoc = nowSoc;
                }
            }
            else
            {
            	sIdleTime++;
        		aimSoc = nowSoc;
            }
		}
    	else
    	{
        	sIdleTime = 0;

        	/*充电末端修正SOC可信度*/
        	if(eCURR_CHG == GetGChgDhgStateAPI())
        	{
            	/*充电已到达末端电压,当前SOC过小*/
        		if((GetGCellMaxVoltAPI() >= SOC_V_CHG_UP_MAX_V) && (nowSoc < SOC_V_CHG_UP_LES_SOC))
        		{
                    /*修正为末端至小值*/
            		CorrGNowCapBySocAPI(SOC_V_CHG_UP_LES_SOC / 10);
        			aimSoc = SOC_V_CHG_UP_LES_SOC;
        		}
            	/*充电只到达起始端电压,当前SOC过小*/
        		else if((GetGCellMaxVoltAPI() >= SOC_V_CHG_DN_MAX_V) && (nowSoc < SOC_V_CHG_DN_LES_SOC))
        		{
                    /*修正为起始端至小值*/
            		CorrGNowCapBySocAPI(SOC_V_CHG_DN_LES_SOC / 10);
        			aimSoc = SOC_V_CHG_DN_LES_SOC;
        		}
        		else
        		{
        			/*使用SOC控制截止充电,电压未达到截止电压而SOC提前到达(SOC跑快了)*/
        	    	if((gGBmuGenPara_102[eBmuGenPara102_CapHig1Soc] < (SOC_SLOW_VH_SOC / 10))
        	    		&& (GetGCellMaxVoltAPI() < gGBmuGenPara_102[eBmuGenPara102_CapHig1Volt])
						&& (nowSoc >= ((gGBmuGenPara_102[eBmuGenPara102_CapHig1Soc] - 6) * 10)))
        	    	{
        	    		/*不能到达截止SOC*/
        	    		aimSoc = (gGBmuGenPara_102[eBmuGenPara102_CapHig1Soc] - 6) * 10;
        	    	}
        			/*未使用SOC控制截止充电*/
        	    	else
        	    	{
        	    		aimSoc = nowSoc;
        	    	}
        		}
        	}
        	/*放电末端修正SOC可信度*/
        	else
        	{
            	/*放电已到达末端电压,当前SOC过大*/
        		if((GetGCellMinVoltAPI() <= SOC_V_DHG_DN_MAX_V) && (nowSoc > SOC_V_DHG_DN_MOS_SOC))
        		{
                    /*修正为末端至大值*/
            		CorrGNowCapBySocAPI(SOC_V_DHG_DN_MOS_SOC / 10);
        			aimSoc = SOC_V_DHG_DN_MOS_SOC;
        		}
            	/*放电只到达起始端电压,当前SOC过大*/
        		else if((GetGCellMinVoltAPI() <= SOC_V_DHG_UP_MAX_V) && (nowSoc > SOC_V_DHG_UP_MOS_SOC))
        		{
                    /*修正为起始端至大值*/
            		CorrGNowCapBySocAPI(SOC_V_DHG_UP_MOS_SOC / 10);
        			aimSoc = SOC_V_DHG_UP_MOS_SOC;
        		}
        		else
        		{
        			/*使用SOC控制截止放电,电压未达到截止电压而SOC提前到达(SOC跑快了)*/
        	    	if((gGBmuGenPara_102[eBmuGenPara102_CapLow1Soc] > (SOC_SLOW_VL_SOC / 10))
        	    		&& (GetGCellMinVoltAPI() > gGBmuGenPara_102[eBmuGenPara102_CapLow1Volt])
						&& (nowSoc <= ((gGBmuGenPara_102[eBmuGenPara102_CapLow1Soc] + 10) * 10)))
        	    	{
        	    		/*不能到达截止SOC*/
        	    		aimSoc = (gGBmuGenPara_102[eBmuGenPara102_CapLow1Soc] + 10) * 10;
        	    	}
        			/*未使用SOC控制截止充电*/
        	    	else
        	    	{
        	    		aimSoc = nowSoc;
        	    	}
        		}
        	}
    	}
    }

    return(aimSoc);
}

/*=============================================================================================
** @Name      : static u16 CalcSocSlowChang(u16 aimSoc)
** @Input     : aimSoc:目标SOC(万分之一)
** @Output    : 平滑后的SOC值(万分之一)
** @Function  : 计算SOC平滑值
** @The notes :
**===========================================================================================*/
static u16 CalcSocSlowChang(u16 aimSoc)
{
    u16 state = 0;                                  /*充放电状态*/
    s32 inteCap = 0;                                /*积分容量变化量(1mAH)*/
    u32 changCap = 0;                               /*容量变化量(1mAH)*/
    u32 totalCap = 0;                               /*总容量(1mAH)*/
    u16 changSoc = 0;                               /*变化SOC(万分之一)*/
    u16 copySoc = 0;                                /*计算的SOC(万分之一)*/
    u16 stepSoc = 0;                                /*SOC增长步长*/
    static u16 sHisState = 0;                       /*历史充放电状态*/
    static s32 sHisCap = 0;                         /*历史充放电量*/
    static u8 sTime = 0;                            /*修正变化0.01%的时间*/
    
    copySoc = sSlowDisplaySoc;                      /*备份平滑值*/
    totalCap = GetGroupTotalCapAPI();               /*总容量*/
    inteCap = GetChgDhgChangCapAPI();               /*积分容量变化量*/
    
    if(GetGSampOutCurrAPI() < 0)
    {
        state = 1;                                  /*充电状态*/
    }
    else if(GetGSampOutCurrAPI() > 0)
    {
        state = 2;                                  /*放电状态*/
    }
    else
    {
        state = 0;
    }
    
    if((1 == state) && (1 == sHisState))            /*持续充电(SOC增大)*/
    {
		changCap = ABS(sHisCap, inteCap);           /*充电负数绝对值增大*/

		changSoc = (u16)(changCap * 10000 / totalCap);/*实际变化的SOC*/

        if(0 == changSoc)                           /*变化过小*/
        {
            inteCap = sHisCap;                      /*不更新本次容量变化值*/
        }

        if(copySoc < aimSoc)                        /*显示平滑值低于实际目标值*/
        {
        	if(aimSoc >= 9000)                      /*目标SOC大于90%(快要充满)*/
        	{
            	if(aimSoc >= 9750)                  /*目标SOC大于97.5%(接近充满)*/
            	{
            		stepSoc = (aimSoc - copySoc) / 50;/*计算5s内增长到真实值步长*/
            	}
            	else if(aimSoc >= 9500)             /*目标SOC大于95%(快要充满)*/
            	{
            		stepSoc = (aimSoc - copySoc) / 150; /*计算15s内增长到真实值步长*/
            	}
            	else if(aimSoc >= 9300)             /*目标SOC大于93%(快要充满)*/
            	{
            		stepSoc = (aimSoc - copySoc) / 200; /*计算20s内增长到真实值步长*/
            	}
            	else                                /*目标SOC大于90%(快要充满)*/
            	{
            		stepSoc = (aimSoc - copySoc) / 300; /*计算30s内增长到真实值步长*/
            	}

				if(0 == stepSoc)                    /*必须要有增量*/
				{
					stepSoc = 1;
				}
        	}
        	else if(aimSoc >= 8000)                 /*目标SOC大于80%*/
        	{
        		stepSoc = (aimSoc - copySoc) / 600; /*计算60s内增长到真实值步长*/
				if(0 == stepSoc)
				{
	        		if(sTime >= 2)                  /*计时0.2s*/
	        		{
	    				stepSoc = 1;                /*增加0.01%*/
	    				sTime = 0;
	        		}
	        		else                            /*累计时间*/
	        		{
	    				stepSoc = 0;                /*SOC不变化*/
	    				sTime++;
	        		}
				}
        	}
        	else                                    /*目标SOC比较低*/
        	{
        		stepSoc = (aimSoc - copySoc) / 1200;/*计算120s内增长到真实值步长*/
				if(0 == stepSoc)
				{
	        		if(sTime >= 3)                  /*计时0.3s*/
	        		{
	    				stepSoc = 1;                /*增加0.01%*/
	    				sTime = 0;
	        		}
	        		else                            /*累计时间*/
	        		{
	    				stepSoc = 0;                /*SOC不变化*/
	    				sTime++;
	        		}
				}
        	}

        	changSoc += stepSoc;                    /*原变化基础上增加加速变化量*/
            
            copySoc += changSoc;                    /*计算本次变化SOC*/
            
            if(copySoc > aimSoc)                    /*防止追赶过界*/
            {
                copySoc = aimSoc;
            }

            if(copySoc > 10000)
            {
                copySoc = 10000;
            }
        }
        else if(copySoc > aimSoc)                   /*显示平滑值高于真实目标值*/
        {
            changSoc = changSoc / SOC_SLOW_SPEED;   /*减缓速度变化的SOC*/
            
            if(0 == changSoc)                       /*变化过小*/
            {
                inteCap = sHisCap;                  /*不更新本次容量变化值*/
            }
            
            copySoc += changSoc;                    /*平滑值放慢上升速度等待真实值*/
            
            if(copySoc > 10000)
            {
                copySoc = 10000;
            }
        }
    }
    else if((2 == state) && (2 == sHisState))       /*持续放电(SOC减小)*/
    {
        changCap = ABS(sHisCap, inteCap);           /*放电正数值增大*/

        changSoc = (u16)(changCap * 10000 / totalCap);/*实际变化的SOC*/

        if(0 == changSoc)                           /*变化过小*/
        {
            inteCap = sHisCap;                      /*不更新本次容量变化值*/
        }

        if(copySoc > aimSoc)                        /*显示平滑值高于实际目标值*/
        {
        	if(aimSoc <= 2000)                      /*目标SOC低于20%(快要放空)*/
        	{
            	if(aimSoc <= 350)                   /*目标SOC低于3.5%(接近放空)*/
            	{
            		stepSoc = (copySoc - aimSoc) / 50;  /*计算5s内增长到真实值步长*/
            	}
            	else if(aimSoc <= 800)              /*目标SOC低于8%(快要放空)*/
            	{
            		stepSoc = (copySoc - aimSoc) / 150; /*计算15s内增长到真实值步长*/
            	}
            	else if(aimSoc <= 1400)             /*目标SOC低于14%(快要放空)*/
            	{
            		stepSoc = (copySoc - aimSoc) / 200; /*计算20s内增长到真实值步长*/
            	}
            	else                                /*目标SOC低于20%(快要放空)*/
            	{
            		stepSoc = (copySoc - aimSoc) / 300; /*计算30s内增长到真实值步长*/
            	}

				if(0 == stepSoc)                    /*必须要有增量*/
				{
					stepSoc = 1;
				}
        	}
        	else if(aimSoc <= 2500)                 /*目标SOC低于25%*/
        	{
        		stepSoc = (copySoc - aimSoc) / 600; /*计算60s内增长到真实值步长*/
				if(0 == stepSoc)
				{
	        		if(sTime >= 2)                  /*计时0.2s*/
	        		{
	    				stepSoc = 1;                /*增加0.01%*/
	    				sTime = 0;
	        		}
	        		else                            /*累计时间*/
	        		{
	    				stepSoc = 0;                /*SOC不变化*/
	    				sTime++;
	        		}
				}
        	}
        	else                                    /*目标SOC比较高*/
        	{
        		stepSoc = (copySoc - aimSoc) / 1200;/*计算120s内增长到真实值步长*/
				if(0 == stepSoc)
				{
	        		if(sTime >= 3)                  /*计时0.3s*/
	        		{
	    				stepSoc = 1;                /*增加0.01%*/
	    				sTime = 0;
	        		}
	        		else                            /*累计时间*/
	        		{
	    				stepSoc = 0;                /*SOC不变化*/
	    				sTime++;
	        		}
				}
        	}
            
        	changSoc += stepSoc;                    /*原变化基础上增加加速变化量*/

            if(copySoc > changSoc)                  /*防止减过界*/
            {
                copySoc -= changSoc;                /*计算本次变化SOC*/
            }
            else
            {
                copySoc = 0;
            }
            
            if(copySoc < aimSoc)                    /*平滑值不应该下降低于真实值*/
            {
                copySoc = aimSoc;
            }
        }
        else if(copySoc < aimSoc)                   /*平滑值低于实际值*/
        {
            changSoc = changSoc / SOC_SLOW_SPEED;   /*平滑值放慢下降速度等待真实值*/
            
            if(0 == changSoc)                       /*变化过小*/
            {
                inteCap = sHisCap;                  /*不更新本次容量变化值*/
            }
            
            if(copySoc > changSoc)                  /*防止减过界*/
            {
                copySoc -= changSoc;
            }
            else
            {
                copySoc = 0;
            }
        }
    }
    
    sHisState = state;                              /*记录历史状态*/
    sHisCap = inteCap;                              /*记录此次积分容量*/

    return(copySoc);
}

/*=============================================================================================
** @Name      : static void UpdateSocSlowDisplay(u16 slowSoc)
** @Input     : slowSoc:平滑后待显示的值
** @Output    : void
** @Function  : 将平滑值更新到显示值
** @The notes :
**===========================================================================================*/
static void UpdateSocSlowDisplay(u16 slowSoc)
{
    static u16 sShowCopy = 0;

    if(slowSoc > 10000)
    {
        slowSoc = 10000;
    }
    
    if(EnerGetGroupVHProStateHook() > 0)                    /*高压一级保护*/
    {
        sSlowDisplaySoc = 10000;                            /*高压保护直接显示为100%*/
    }
    else if(EnerGetGroupVLProStateHook() > 0)               /*低压一级保护*/
    {
        sSlowDisplaySoc = 0;                                /*低压保护直接显示为0%*/
    }
    else if((slowSoc >= 9900)                               /*平滑值高于99%*/
        && (0 == EnerGetGroupVHLimStateHook()))             /*SOC高于99%但是未到一般高压报警*/
    {
        if(sSlowDisplaySoc <= 9900)                         /*显示值未高于99%*/
        {
            sSlowDisplaySoc = 9900;                         /*保持显示99%*/
        }
        else if(slowSoc < sSlowDisplaySoc)                  /*平滑值变小(放电)*/
        {
            sSlowDisplaySoc = slowSoc;                      /*显示平滑值*/
        }
    }
    else if((slowSoc <= 100)                                /*平滑值低于1%*/
        && (0 == EnerGetGroupVLLimStateHook()))             /*SOC低于1%但是未到低压放电截止报警 */
    {
        if(sSlowDisplaySoc >= 100)                          /*显示值未低于1%*/
        {
            sSlowDisplaySoc = 100;                          /*保持显示为1%*/
        }
        else if(slowSoc > sSlowDisplaySoc)                  /*平滑值变大(充电)*/
        {
            sSlowDisplaySoc = slowSoc;                      /*显示平滑值*/
        }
    }
    else
    {
        sSlowDisplaySoc = slowSoc;                          /*显示平滑值*/
    }
    
    //gGSysCapInfo_61[eSysCapInfo61_SOC] = (sSlowDisplaySoc + 5) / 10;/*四舍五入转化为显示单位(千分之一)*/
    if(FALSE == EnerGetUserSocStopReqHook())                /*不需要停止显示*/
    {
        gGSysCapInfo_61[eSysCapInfo61_SOC] = (sSlowDisplaySoc + 5) / 10;/*四舍五入转化为显示单位(千分之一)*/
    }

    if((sShowCopy / 100) != (sSlowDisplaySoc / 100))        /*变化超过1%*/
    {
        EnerChangEepGNowSocHook((sSlowDisplaySoc / 10));    /*保存显示SOC*/
        sShowCopy =  sSlowDisplaySoc;
    } 
}
