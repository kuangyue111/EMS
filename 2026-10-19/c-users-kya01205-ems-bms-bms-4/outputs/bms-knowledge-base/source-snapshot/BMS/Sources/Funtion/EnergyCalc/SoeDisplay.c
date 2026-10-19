/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : SoeDisplay.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : soe平滑显示模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "ParaInterface.h"
#include "CurrIntegral.h"
#include "EnerInfoCalc.h"
#include "EnerInterface.h"
#include "SoeDisplay.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u16 sSlowDisplaySoe = 0xffff;    /*平滑显示SOE中间计算值(万分之一)*/

extern u16 gGSysCapInfo_61[];			/*系统电量信息数组*/

extern u16 gGBmuGenPara_102[];		    /*主控通用参数值*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 CalcSoeChgDhgEndCorr(u16 nowSoe)
** @Input     : aimSoe:当前SOE(万分之一)
** @Output    : 修正后的SOE值(万分之一)
** @Function  : 计算SOE充放电末端修正值
** @The notes :
**===========================================================================================*/
static u16 CalcSoeChgDhgEndCorr(u16 nowSoe);

/*=============================================================================================
** @Name      : static u16 CalcSoeSlowChang(u16 aimSoe)
** @Input     : aimSoe:目标SOE(万分之一)
** @Output    : 平滑后的SOE值(万分之一)
** @Function  : 计算SOE平滑值
** @The notes :
**===========================================================================================*/
static u16 CalcSoeSlowChang(u16 aimSoe);

/*=============================================================================================
** @Name      : static void UpdateSoeSlowDisplay(u16 slowSoe)
** @Input     : slowSoe:平滑后待显示的值
** @Output    : void
** @Function  : 将平滑值更新到显示值
** @The notes :
**===========================================================================================*/
static void UpdateSoeSlowDisplay(u16 slowSoe);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void GroupSoeDisplayInit(void)
** @Input     : void
** @Output    : void
** @Function  : SOE平滑显示初始化
** @The notes : 用户初始化调用
**===========================================================================================*/
void GroupSoeDisplayInit(void)
{
    u16 soe = 0;

    soe = gGBmuGenPara_102[eBmuGenPara102_NowSoe] * 10;

    /*上电不显示100%和0%*/
    if(soe > 9940)
    {
    	soe = 9940;
    }
    else if(soe < 100)
    {
    	soe = 100;
    }

    sSlowDisplaySoe = soe;
}

/*=============================================================================================
** @Name      : void GroupSoeDisplayTask(void)
** @Input     : void
** @Output    : void
** @Function  : SOE平滑显示任务
** @The notes : 100ms
**===========================================================================================*/
void GroupSoeDisplayTask(void)
{
    u16 aimSoe = 0;
    u16 slowSoe = 0;

    aimSoe = GetGRealSoeTenThousAPI();          /*取当前SOE*/

    aimSoe = CalcSoeChgDhgEndCorr(aimSoe);      /*充放电末端修正当前SOE*/

    if(0xffff == sSlowDisplaySoe)               /*平滑值未初始化*/
    {
        slowSoe = aimSoe;
    }
    else
    {
        slowSoe = CalcSoeSlowChang(aimSoe);     /*计算SOE平滑值*/
    }

    UpdateSoeSlowDisplay(slowSoe);              /*将平滑值更新到显示值*/
}

/*=============================================================================================
** @Name      : void CorrGDisplaySoeByUser(u16 soe)
** @Input     : soe:目标SOE(0.01%)
** @Output    : void
** @Function  : 用户修正当前显示SOE
** @The notes :
**===========================================================================================*/
void CorrGDisplaySoeByUser(u16 soe)
{
    if(soe > 10000)
    {
        return;
    }

    sSlowDisplaySoe = soe;

    UpdateSoeSlowDisplay(soe);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u16 CalcSoeChgDhgEndCorr(u16 nowSoe)
** @Input     : aimSoe:当前SOE(万分之一)
** @Output    : 修正后的SOE值(万分之一)
** @Function  : 计算SOE充放电末端修正值
** @The notes :
**===========================================================================================*/
static u16 CalcSoeChgDhgEndCorr(u16 nowSoe)
{
	u16 aimSoe = nowSoe;
    static u8 sCorrFlag = 0;
    static u32 sIdleTime = 0;

    if(0 == EnerGetGroupBattReadyHook())                    /*单体信息异常未就绪*/
    {
    	/*不执行末端修正*/
        sCorrFlag = 0;
        sIdleTime = 0;
		aimSoe = nowSoe;                                    /*返回原值*/
    }
    else if(EnerGetGroupVHProStateHook() > 0)               /*高压一级保护*/
    {
    	/*SOE未达到目标值且未执行修正*/
    	if((1 != sCorrFlag)
    		&& ((nowSoe < SOE_SLOW_VHER_SOE)
    		|| (sSlowDisplaySoe < SOE_SLOW_VHER_SOE)))
    	{
    		CorrGRemainEnerBySoeAPI(SOE_SLOW_VHER_SOE / 10);/*强制修正为100%*/
    		CorrGNowEnerBySoeAPI(SOE_SLOW_VHER_SOE / 10);   /*强制修正为100%*/
    		sSlowDisplaySoe = SOE_SLOW_VHER_SOE;            /*强制显示为100%*/
    		aimSoe = SOE_SLOW_VHER_SOE;                     /*记录显示目标100%*/
    		sCorrFlag = 1;                                  /*记录已修正显示*/
    	}
    	else
    	{
    		aimSoe = nowSoe;                               /*记录显示当前值*/
    	}
    	sIdleTime = 0;
    }
    else if(EnerGetGroupVLProStateHook() > 0)               /*低压一级保护*/
    {
    	/*SOE未达到目标值且未执行修正*/
    	if((2 != sCorrFlag)
    		&& ((nowSoe > SOE_SLOW_VLER_SOE)
    		|| (sSlowDisplaySoe > SOE_SLOW_VLER_SOE)))
    	{
    		CorrGRemainEnerBySoeAPI(SOE_SLOW_VLER_SOE / 10);/*强制修正为0%*/
    		CorrGNowEnerBySoeAPI(SOE_SLOW_VLER_SOE / 10);   /*强制修正为0%*/
    		sSlowDisplaySoe = SOE_SLOW_VLER_SOE;            /*强制显示为0%*/
    		aimSoe = SOE_SLOW_VLER_SOE;                     /*记录显示目标0%*/
    		sCorrFlag = 2;                                  /*记录已修正显示*/
    	}
    	else
    	{
    		aimSoe = nowSoe;                               /*记录显示当前值*/
    	}
    	sIdleTime = 0;
    }
    else if(EnerGetGroupVHLimStateHook() > 0)               /*高压二级保护*/
    {
    	/*SOE未达到目标值且未执行修正*/
    	if((1 != sCorrFlag) && (3 != sCorrFlag)
    		&& ((nowSoe < SOE_SLOW_VH_SOE)
    		|| (sSlowDisplaySoe < SOE_SLOW_VH_SOE)))
    	{
    		CorrGNowEnerBySoeAPI(SOE_SLOW_VH_SOE / 10);     /*强制修正为100%*/
    		aimSoe = SOE_SLOW_VH_SOE;                       /*记录显示目标100%*/
    		sCorrFlag = 3;                                  /*记录已修正显示*/

    		if(GetGSampOutCurrAPI() < -5)                   /*正在充电*/
			{
				sSlowDisplaySoe = SOE_SLOW_VH_SOE;          /*强制显示为100%*/
			}
    	}
    	else
    	{
    		aimSoe = nowSoe;                                /*记录显示当前值*/
    	}
    	sIdleTime = 0;
    }
    else if(EnerGetGroupVLLimStateHook() > 0)               /*低压二级保护*/
    {
    	/*SOE未达到目标值且未执行修正*/
    	if((2 != sCorrFlag) && (4 != sCorrFlag)
    		&& ((nowSoe > SOE_SLOW_VL_SOE)
    		|| (sSlowDisplaySoe > SOE_SLOW_VL_SOE)))
    	{
    		CorrGNowEnerBySoeAPI(SOE_SLOW_VL_SOE / 10);     /*强制修正为3%*/
    		aimSoe = SOE_SLOW_VL_SOE;                       /*记录显示目标3%*/
    		sCorrFlag = 4;                                  /*记录已修正显示*/

    		if(GetGSampOutCurrAPI() > 5)                    /*正在放电*/
			{
				sSlowDisplaySoe = SOE_SLOW_VL_SOE;          /*强制显示为3%*/
			}
    	}
    	else
    	{
    		aimSoe = nowSoe;                                /*记录显示当前值*/
    	}
    	sIdleTime = 0;
    }
    else
    {
		sCorrFlag = 0;                                      /*清除已修正显示*/

    	/*系统待机时修正SOE可信度*/
		if(eCURR_IDLE == GetGChgDhgStateAPI())
		{
	    	/*达到静置时长*/
            if(sIdleTime >= SOE_SLOW_CORR_IDLE_T)
            {
        		/*SOE与SOC偏差过大(SOC修正后SOE才修正)*/
        		if(ABS(nowSoe, GetGRealSocTenThousAPI()) >= 500)
        		{
					/*电压很低处于下区间*/
					if(GetGCellMaxVoltAPI() < SOE_V_SEC_DN_MAX_V)
					{
						/*当前SOE过高*/
						if(nowSoe > SOE_V_SEC_DN_MOS_SOE)
						{
							/*修正当前SOE为至大可能值*/
							CorrGNowEnerBySoeAPI(SOE_V_SEC_DN_MOS_SOE / 10);
							aimSoe = SOE_V_SEC_DN_MOS_SOE;
						}
						/*显示SOE过高*/
						else if(sSlowDisplaySoe > SOE_V_SEC_DN_MOS_SOE)
						{
							/*修正当前SOE为至大可能值*/
							sSlowDisplaySoe = SOE_V_SEC_DN_MOS_SOE;
							aimSoe = nowSoe;
						}
						else
						{
							aimSoe = nowSoe;
						}
					}
					/*电压很高处于上区间*/
					else if(GetGCellMinVoltAPI() > SOE_V_SEC_UP_MIN_V)
					{
						/*当前SOE过低*/
						if(nowSoe < SOE_V_SEC_UP_LES_SOE)
						{
							/*修正当前SOE为至小可能值*/
							CorrGNowEnerBySoeAPI(SOE_V_SEC_UP_LES_SOE / 10);
							aimSoe = SOE_V_SEC_UP_LES_SOE;
						}
						/*显示SOE过低*/
						else if(sSlowDisplaySoe < SOE_V_SEC_UP_LES_SOE)
						{
							/*修正当前SOE为至大可能值*/
							sSlowDisplaySoe = SOE_V_SEC_UP_LES_SOE;
							aimSoe = nowSoe;
						}
						else
						{
							aimSoe = nowSoe;
						}
					}
					/*电压稍高高于下区间*/
					else if(GetGCellMinVoltAPI() > SOE_V_SEC_DN_MIN_V)
					{
						/*当前SOE过低*/
						if(nowSoe < SOE_V_SEC_DN_LES_SOE)
						{
							/*修正当前SOE为至小可能值*/
							CorrGNowEnerBySoeAPI(SOE_V_SEC_DN_LES_SOE / 10);
							aimSoe = SOE_V_SEC_DN_LES_SOE;
						}
						/*显示SOE过低*/
						else if(sSlowDisplaySoe < SOE_V_SEC_DN_LES_SOE)
						{
							/*修正当前SOE为至大可能值*/
							sSlowDisplaySoe = SOE_V_SEC_DN_LES_SOE;
							aimSoe = nowSoe;
						}
						else
						{
							aimSoe = nowSoe;
						}
					}
					/*电压稍低低于上区间*/
					else if(GetGCellMaxVoltAPI() < SOE_V_SEC_UP_MAX_V)
					{
						/*当前SOE过高*/
						if(nowSoe > SOE_V_SEC_UP_MOS_SOE)
						{
							/*修正当前SOE为至大可能值*/
							CorrGNowEnerBySoeAPI(SOE_V_SEC_UP_MOS_SOE / 10);
							aimSoe = SOE_V_SEC_UP_MOS_SOE;
						}
						/*显示SOE过高*/
						else if(sSlowDisplaySoe > SOE_V_SEC_UP_MOS_SOE)
						{
							/*修正当前SOE为至大可能值*/
							sSlowDisplaySoe = SOE_V_SEC_UP_MOS_SOE;
							aimSoe = nowSoe;
						}
						else
						{
							aimSoe = nowSoe;
						}
					}
					else
					{
						aimSoe = nowSoe;
					}
				}
				else
				{
					aimSoe = nowSoe;
				}
            }
            else
            {
            	sIdleTime++;
        		aimSoe = nowSoe;
            }
		}
    	else
    	{
        	sIdleTime = 0;

        	/*充电末端修正SOE可信度*/
        	if(eCURR_CHG == GetGChgDhgStateAPI())
        	{
        		/*SOE与SOC偏差过大(SOC修正后SOE才修正)*/
        		if(ABS(nowSoe, GetGRealSocTenThousAPI()) >= 500)
        		{
					/*充电已到达末端电压,当前SOE过小*/
					if((GetGCellMaxVoltAPI() >= SOE_V_CHG_UP_MAX_V) && (nowSoe < SOE_V_CHG_UP_LES_SOE))
					{
						/*修正为末端至小值*/
						CorrGNowEnerBySoeAPI(SOE_V_CHG_UP_LES_SOE / 10);
						aimSoe = SOE_V_CHG_UP_LES_SOE;
					}
					/*充电只到达起始端电压,当前SOE过小*/
					else if((GetGCellMaxVoltAPI() >= SOE_V_CHG_DN_MAX_V) && (nowSoe < SOE_V_CHG_DN_LES_SOE))
					{
						/*修正为起始端至小值*/
						CorrGNowEnerBySoeAPI(SOE_V_CHG_DN_LES_SOE / 10);
						aimSoe = SOE_V_CHG_DN_LES_SOE;
					}
					else
					{
						aimSoe = nowSoe;
					}
				}
				else
				{
					aimSoe = nowSoe;
				}
        	}
        	/*放电末端修正SOE可信度*/
        	else
        	{
        		/*SOE与SOC偏差过大(SOC修正后SOE才修正)*/
        		if(ABS(nowSoe, GetGRealSocTenThousAPI()) >= 500)
        		{
					/*放电已到达末端电压,当前SOE过大*/
					if((GetGCellMinVoltAPI() <= SOE_V_DHG_DN_MAX_V) && (nowSoe > SOE_V_DHG_DN_MOS_SOE))
					{
						/*修正为末端至大值*/
						CorrGNowEnerBySoeAPI(SOE_V_DHG_DN_MOS_SOE / 10);
						aimSoe = SOE_V_DHG_DN_MOS_SOE;
					}
					/*放电只到达起始端电压,当前SOE过大*/
					else if((GetGCellMinVoltAPI() <= SOE_V_DHG_UP_MAX_V) && (nowSoe > SOE_V_DHG_UP_MOS_SOE))
					{
						/*修正为起始端至大值*/
						CorrGNowEnerBySoeAPI(SOE_V_DHG_UP_MOS_SOE / 10);
						aimSoe = SOE_V_DHG_UP_MOS_SOE;
					}
					else
					{
						aimSoe = nowSoe;
					}
				}
				else
				{
					aimSoe = nowSoe;
				}
        	}
    	}
    }

    return(aimSoe);
}

/*=============================================================================================
** @Name      : static u16 CalcSoeSlowChang(u16 aimSoe)
** @Input     : aimSoe:目标SOE(万分之一)
** @Output    : 平滑后的SOE值(万分之一)
** @Function  : 计算SOE平滑值
** @The notes :
**===========================================================================================*/
static u16 CalcSoeSlowChang(u16 aimSoe)
{
    u16 state = 0;                                  /*充放电状态*/
    s32 inteEner = 0;                               /*积分容量(1wH)*/
    u32 changEner = 0;                              /*能量变化量(1wH)*/
    u32 totalEner = 0;                              /*总能量(1wH)*/
    u16 changSoe = 0;                               /*变化SOE(万分之一)*/
    u16 copySoe = 0;                                /*计算的SOE(万分之一)*/
    u16 stepSoe = 0;                                /*SOE增长步长*/
    static u16 sHisState = 0;                       /*历史充放电状态*/
    static s32 sHisEner = 0;                        /*历史充放电能*/
    static u8 sTime = 0;                            /*修正变化0.01%的时间*/

    copySoe = sSlowDisplaySoe;                      /*备份平滑值*/
    totalEner = GetGroupTotalEnerAPI();             /*总能量*/
    inteEner = GetChgDhgChangEnerAPI();             /*积分能量*/

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

    if((1 == state) && (1 == sHisState))            /*持续充电(SOE增大)*/
    {
		changEner = ABS(sHisEner, inteEner);        /*充电负数绝对值增大*/

		changSoe = (u16)(changEner * 10000 / totalEner);/*实际变化的SOE*/

		if(0 == changSoe)                           /*变化过小*/
		{
			inteEner = sHisEner;                    /*不更新本次能量变化值*/
		}

        if(copySoe < aimSoe)                        /*显示平滑值低于实际目标值*/
        {
        	if(aimSoe >= 9000)                      /*目标SOE大于90%(快要充满)*/
        	{
            	if(aimSoe >= 9750)                  /*目标SOE大于97.5%(接近充满)*/
            	{
            		stepSoe = (aimSoe - copySoe) / 50;/*计算5s内增长到真实值步长*/
            	}
            	else if(aimSoe >= 9500)             /*目标SOE大于95%(快要充满)*/
            	{
            		stepSoe = (aimSoe - copySoe) / 150; /*计算15s内增长到真实值步长*/
            	}
            	else if(aimSoe >= 9300)             /*目标SOE大于93%(快要充满)*/
            	{
            		stepSoe = (aimSoe - copySoe) / 200; /*计算20s内增长到真实值步长*/
            	}
            	else                                /*目标SOE大于90%(快要充满)*/
            	{
            		stepSoe = (aimSoe - copySoe) / 300; /*计算30s内增长到真实值步长*/
            	}

				if(0 == stepSoe)                    /*必须要有增量*/
				{
					stepSoe = 1;
				}
        	}
        	else if(aimSoe >= 8000)                 /*目标SOE大于80%*/
        	{
        		stepSoe = (aimSoe - copySoe) / 600; /*计算60s内增长到真实值步长*/
				if(0 == stepSoe)
				{
	        		if(sTime >= 2)                  /*计时0.2s*/
	        		{
	    				stepSoe = 1;                /*增加0.01%*/
	    				sTime = 0;
	        		}
	        		else                            /*累计时间*/
	        		{
	    				stepSoe = 0;                /*SOE不变化*/
	    				sTime++;
	        		}
				}
        	}
        	else                                    /*目标SOE比较低*/
        	{
        		stepSoe = (aimSoe - copySoe) / 1200;/*计算120s内增长到真实值步长*/
				if(0 == stepSoe)
				{
	        		if(sTime >= 3)                  /*计时0.3s*/
	        		{
	    				stepSoe = 1;                /*增加0.01%*/
	    				sTime = 0;
	        		}
	        		else                            /*累计时间*/
	        		{
	    				stepSoe = 0;                /*SOE不变化*/
	    				sTime++;
	        		}
				}
        	}

        	changSoe += stepSoe;                    /*原变化基础上增加加速变化量*/

            copySoe += changSoe;                    /*计算本次变化SOE*/

            if(copySoe > aimSoe)                    /*防止追赶过界*/
            {
                copySoe = aimSoe;
            }
        }
        else if(copySoe > aimSoe)                   /*显示平滑值高于真实目标值*/
        {
            changSoe = changSoe / SOE_SLOW_SPEED;   /*减缓速度变化的SOE*/

            if(0 == changSoe)                       /*变化过小*/
            {
                inteEner = sHisEner;                /*不更新本次能量变化值*/
            }

            copySoe += changSoe;                    /*平滑值放慢上升速度等待真实值*/

            if(copySoe > 10000)
            {
                copySoe = 10000;
            }
        }
    }
    else if((2 == state) && (2 == sHisState))       /*持续放电(SOE减小)*/
    {
        changEner = ABS(sHisEner, inteEner);        /*放电正数值增大*/

        changSoe = (u16)(changEner * 10000 / totalEner);/*实际变化的SOE*/

        if(0 == changSoe)                           /*变化过小*/
        {
        	inteEner = sHisEner;                    /*不更新本次能量变化值*/
        }

        if(copySoe > aimSoe)                        /*显示平滑值高于实际目标值*/
        {
        	if(aimSoe <= 2000)                      /*目标SOE低于20%(快要放空)*/
        	{
            	if(aimSoe <= 400)                   /*目标SOE低于4%(接近放空)*/
            	{
            		stepSoe = (copySoe - aimSoe) / 50;/*计算5s内增长到真实值步长*/
            	}
            	else if(aimSoe <= 800)              /*目标SOE低于8%(快要放空)*/
            	{
            		stepSoe = (copySoe - aimSoe) / 150;/*计算30s内增长到真实值步长*/
            	}
            	else if(aimSoe <= 1400)             /*目标SOE低于14%(快要放空)*/
            	{
            		stepSoe = (copySoe - aimSoe) / 200;/*计算20s内增长到真实值步长*/
            	}
            	else                                /*目标SOE低于20%(快要放空)*/
            	{
            		stepSoe = (copySoe - aimSoe) / 300;/*计算30s内增长到真实值步长*/
            	}

				if(0 == stepSoe)                    /*必须要有增量*/
				{
					stepSoe = 1;
				}
        	}
        	else if(aimSoe <= 2500)                 /*目标SOE低于20%*/
        	{
        		stepSoe = (copySoe - aimSoe) / 600; /*计算60s内增长到真实值步长*/
				if(0 == stepSoe)
				{
	        		if(sTime >= 2)                  /*计时0.2s*/
	        		{
	    				stepSoe = 1;                /*增加0.01%*/
	    				sTime = 0;
	        		}
	        		else                            /*累计时间*/
	        		{
	    				stepSoe = 0;                /*SOE不变化*/
	    				sTime++;
	        		}
				}
        	}
        	else                                    /*目标SOE比较高*/
        	{
        		stepSoe = (copySoe - aimSoe) / 1200;/*计算120s内增长到真实值步长*/
				if(0 == stepSoe)
				{
	        		if(sTime >= 3)                  /*计时0.3s*/
	        		{
	    				stepSoe = 1;                /*增加0.01%*/
	    				sTime = 0;
	        		}
	        		else                            /*累计时间*/
	        		{
	    				stepSoe = 0;                /*SOE不变化*/
	    				sTime++;
	        		}
				}
        	}

        	changSoe += stepSoe;                    /*原变化基础上增加加速变化量*/

            if(copySoe > changSoe)                  /*防止减过界*/
            {
                copySoe -= changSoe;                /*计算本次变化SOE*/
            }
            else
            {
                copySoe = 0;
            }

            if(copySoe < aimSoe)                    /*平滑值不应该下降低于真实值*/
            {
                copySoe = aimSoe;
            }
        }
        else if(copySoe < aimSoe)                   /*显示平滑值低于实际目标值*/
        {
            changSoe = changSoe / SOE_SLOW_SPEED;   /*平滑值放慢下降速度等待真实值*/

            if(0 == changSoe)                       /*变化过小*/
            {
            	inteEner = sHisEner;                /*不更新本次能量变化值*/
            }

            if(copySoe > changSoe)                  /*防止减过界*/
            {
                copySoe -= changSoe;
            }
            else
            {
                copySoe = 0;
            }
        }
    }

    sHisState = state;                              /*记录历史状态*/
    sHisEner = inteEner;                            /*记录此次积分能量*/

    return(copySoe);
}

/*=============================================================================================
** @Name      : static void UpdateSoeSlowDisplay(u16 slowSoe)
** @Input     : slowSoe:平滑后待显示的值
** @Output    : void
** @Function  : 将平滑值更新到显示值
** @The notes :
**===========================================================================================*/
static void UpdateSoeSlowDisplay(u16 slowSoe)
{
    static u16 sShowCopy = 0;

    if(slowSoe > 10000)
    {
        slowSoe = 10000;
    }

    if(EnerGetGroupVHProStateHook() > 0)
    {
        sSlowDisplaySoe = 10000;                            /*高压保护直接显示为100%*/
    }
    else if(EnerGetGroupVLProStateHook() > 0)
    {
        sSlowDisplaySoe = 0;                                /*低压保护直接显示为0%*/
    }
    else if((slowSoe >= 9900)                               /*平滑值高于99%*/
        && (0 == EnerGetGroupVHLimStateHook()))             /*SOE高于99%但是未到一般高压报警*/
    {
        if(sSlowDisplaySoe <= 9900)                         /*显示值未高于99%*/
        {
            sSlowDisplaySoe = 9900;                         /*保持显示99%*/
        }
        else if(slowSoe < sSlowDisplaySoe)                  /*平滑值变小(放电)*/
        {
            sSlowDisplaySoe = slowSoe;                      /*显示平滑值*/
        }
    }
    else if((slowSoe <= 100)                                /*平滑值低于1%*/
        && (0 == EnerGetGroupVLLimStateHook()))             /*SOE低于1%但是未到低压放电截止报警*/
    {
        if(sSlowDisplaySoe >= 100)                          /*显示值未低于1%*/
        {
            sSlowDisplaySoe = 100;                          /*保持显示为1%*/
        }
        else if(slowSoe > sSlowDisplaySoe)                  /*平滑值变大(充电)*/
        {
            sSlowDisplaySoe = slowSoe;                      /*显示平滑值*/
        }
    }
    else
    {
        sSlowDisplaySoe = slowSoe;                          /*显示平滑值*/
    }

    //gGSysCapInfo_61[eSysCapInfo61_SOE] = (sSlowDisplaySoe + 5) / 10;/*四舍五入转化为显示单位(千分之一)*/
    if(FALSE == EnerGetUserSoeStopReqHook())                /*不需要停止显示*/
    {
        gGSysCapInfo_61[eSysCapInfo61_SOE] = (sSlowDisplaySoe + 5) / 10;/*四舍五入转化为显示单位(千分之一)*/
    }

    if((sShowCopy / 100) != (sSlowDisplaySoe / 100))        /*变化超过1%*/
    {
        EnerChangEepGNowSoeHook((sSlowDisplaySoe / 10));    /*保存显示SOE*/
        sShowCopy =  sSlowDisplaySoe;
    }
}
