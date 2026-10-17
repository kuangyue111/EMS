/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileEnerStat.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控电能量统计模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicPara.h"
#include "PublicInfo.h"
#include "PileInfo.h"
#include "PileGroupRcv.h"
#include "PileSysState.h"
#include "PileEnerStat.h"

#if(1 == PILE_MANAG_EN)
#if(1 == PILE_MOR_F_EN)/*使能更多功能*/
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gPStatistCapEner_72[];			/*当前能量统计信息数组*/
extern u32 gPStatistHisCap_73[];			/*累计电量统计信息数组*/
extern u32 gPStatistHisEner_74[];			/*累计电能统计信息数组*/
extern u16 gPStatistDhgDeep_75[];			/*放电深度统计信息数组*/

extern u16 gSysGenParaRO_100[];  			/*系统通用参数设置*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void PileHisCapEnerCalcTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 电池堆历史充放电电量电能计算任务
//注意事项	: 100ms
//=============================================================================================
void PileHisCapEnerCalcTask(void)
{
    u8 i = 0;
    u8 num = 0;
    u8 runNum = 0;
    u8 dodNum = 0;
    u32 chgCap = 0;
    u32 dhgCap = 0;
    u32 hisChgC = 0;
    u32 hisDhgC = 0;
    u32 hisChgE = 0;
    u32 hisDhgE = 0;
    u32 nowDod = 1000;
    
    //初始化
    if(eWORK_INIT == GetPileWorkStateAPI())
    {
        //上传0
    	gPStatistCapEner_72[ePCapEner72_ChgCap] = 0;
    	gPStatistCapEner_72[ePCapEner72_DhgCap] = 0;
    	gPStatistHisCap_73[ePHisCap73_ChgCap] = 0;
    	gPStatistHisCap_73[ePHisCap73_DhgCap] = 0;
    	gPStatistHisEner_74[ePHisEner74_ChgEner] = 0;
    	gPStatistHisEner_74[ePHisEner74_DhgEner] = 0;
    	gPStatistDhgDeep_75[ePDhgDeep_NowDod] = 0;
    }
    //非初始化
    else
    {

        //正在运行的电池组数
        runNum = GetPileRunGroupNumAPI();

        //检测所有电池组
        for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
        {
            //计算使能电池组
            if((1 == GetPGroupEnStateAPI(i))
                && (0 == GetPGroupCanOffStateAPI(i)))
            {
                num++;
                chgCap += GetPGroupNowChgCapAPI(i);
                dhgCap += GetPGroupNowDhgCapAPI(i);
                hisChgC += GetPGroupHisChgCapAPI(i);
                hisDhgC += GetPGroupHisDhgCapAPI(i);
                hisChgE += GetPGroupHisChgEnerAPI(i);
                hisDhgE += GetPGroupHisDhgEnerAPI(i);

                if(0 == runNum)
                {
                    //没有正在运行的组计算非故障组DOD
                	if((eWORK_INIT != GetPGroupWorkStateAPI(i))
                		&& (eWORK_ERR != GetPGroupWorkStateAPI(i)))
                	{
                		/*求平均DOD*/
                		nowDod += GetPGroupNowChgCapAPI(i);
                		dodNum++;
                	}
                }
                else
                {
                    //有正在运行的组计算运行组DOD
                	if(eWORK_RUN == GetPGroupWorkStateAPI(i))
                	{
                		/*求最小DOD*/
                		if(GetPGroupNowChgCapAPI(i) < nowDod)
                		{
                			nowDod = GetPGroupNowChgCapAPI(i);
                    		dodNum = 1;
                		}
                	}
                }
            }
        }
        
        if(num > 0)
        {
        	gPStatistCapEner_72[ePCapEner72_ChgCap] = chgCap;
        	gPStatistCapEner_72[ePCapEner72_DhgCap] = dhgCap;
        	gPStatistHisCap_73[ePHisCap73_ChgCap] = hisChgC;
        	gPStatistHisCap_73[ePHisCap73_DhgCap] = hisDhgC;
        	gPStatistHisEner_74[ePHisEner74_ChgEner] = hisChgE;
            gPStatistHisEner_74[ePHisEner74_DhgEner] = hisDhgE;

            if(dodNum > 1)
            {
            	gPStatistDhgDeep_75[ePDhgDeep_NowDod] = nowDod / dodNum;
            }
            else if(dodNum == 1)
            {
            	gPStatistDhgDeep_75[ePDhgDeep_NowDod] = nowDod;
            }
            else
            {
            	gPStatistDhgDeep_75[ePDhgDeep_NowDod] = 0;
            }
        }
    }
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
#endif
