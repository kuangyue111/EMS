/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : BalanTactic.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 均衡控制策略模块接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "GroupInfo.h"
#include "DiagInterface.h"
#include "BalanTactic.h"

//菊花链通信从控或自研CAN通信从控
#if((0 == BMU_BSU_COM_TYPE) || (0 == BMU_BSU_CAN_TYPE))
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static s32 sGBalanSubVolt[GROUP_MAX_CELVNUM] = {0};         /*保存单节电压与平均电压差累积值*/
static u32 sGPasBalanReqState[SLAVE_MAX_NUM] = {0};         /*各从控被动均衡请求状态(按位表示[0关闭 1开启])*/

extern u16 gGChgBalanInfo_90[];	                            /*充电均衡状态信息数组*/
extern u16 gGDhgBalanInfo_91[];                             /*放电均衡状态信息数组*/

extern u16 gGBmuGenPara_102[];                              /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];							/*主控高级参数值*/
extern u8 gGHardPara_104[];                                 /*主控硬件参数配置信息*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void ClrBalanCheckData(void)
** @Input     : void
** @Output    : void
** @Function  : 清除均衡检测数据
** @The notes : 压差计算值和均衡请求状态
**===============================================================================================*/
static void ClrBalanCheckData(void);

/*=================================================================================================
** @Name      : static void ClrBalanSubVolt(void)
** @Input     : void
** @Output    : void
** @Function  : 清除均衡压差计算值
** @The notes :
**===============================================================================================*/
static void ClrBalanSubVolt(void);

/*=================================================================================================
** @Name      : static void CalcCellVoltSubInte(void)
** @Input     : void
** @Output    : void
** @Function  : 计算单体压差积分
** @The notes :
**===============================================================================================*/
static void CalcCellVoltSubInte(void);

/*=================================================================================================
** @Name      : static void JudgeBalanReqByPasCir(void)
** @Input     : void
** @Output    : void
** @Function  : 判断被动放电均衡电路均衡请求
** @The notes :
**===============================================================================================*/
static void JudgeBalanReqByPasCir(void);

/*=================================================================================================
** @Name      : static u8 GetDhgBalanSubVolt(u16 cell)
** @Input     : cell:电池号
** @Output    : 开启放电均衡的压差 1mv
** @Function  : 获取开启放电均衡的压差
** @The notes :
**===============================================================================================*/
static u8 GetDhgBalanSubVolt(u16 cell);

/*=================================================================================================
** @Name      : static void UpdateBalanShowState(void)
** @Input     : void
** @Output    : void
** @Function  : 更新电池均衡显示状态
** @The notes :
**===============================================================================================*/
static void UpdateBalanShowState(void);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void GroupVoltBalanTacticTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电压均衡策略任务
** @The notes : 100ms周期调用
**===============================================================================================*/
void GroupVoltBalanTacticTask(void)
{
    static u8 sState = 0;
    static u16 sTimer = 0;
    static u16 sHisMode = 0;

    switch(sState)
    {
        //检测当前均衡条件
        case 0:
            if(eMODE_DEBUG == GetGWorkModeAPI())                            //调试模式
            {
            	/*刚进入调试模式关闭所有均衡*/
            	if(sHisMode != eMODE_DEBUG)
            	{
                	ClrBalanCheckData();                                    //清除均衡检测数据
            	}

            	UpdateBalanShowState();                                     //更新均衡状态显示
                sHisMode = eMODE_DEBUG;                                     //记录调试模式
                sState = 0;                                                 //不需检测均衡
            }
            else if(0 == gGHardPara_104[eGHardPara104_BalTact])             //禁用均衡
            {
            	ClrBalanCheckData();                                        //清除均衡检测数据
            	UpdateBalanShowState();                                     //更新均衡状态显示
                sState = 0;                                                 //不需检测均衡
            }
            else                                                            //非调试模式
            {
                if((eWORK_INIT == GetGWorkStateAPI())                       //系统初始化
                    /*|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_INT_LOFF)) //从控级联断线*/
	                || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_INT_COFF)) //从控通信断线
                    || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_SAMP_VOFF))//从控采样断线
                    || (eErr_B == DiagGetErrLastStateAPI(eERR_ID_SAMP_EXP)) //从控采样失效
                    //|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_BAL_CEXP))//从控均衡控制失效
                    || (sHisMode == eMODE_DEBUG))                           //刚退出调试模式
                {
                	ClrBalanCheckData();                                    //清除均衡检测数据
                	UpdateBalanShowState();                                 //更新均衡状态显示
                    sState = 0;                                             //不需检测均衡
                }
                else
                {
                    sState = 1;                                             //进入均衡检测
                }

                sHisMode = GetGWorkModeAPI();
            }
            break;

        //检测当前均衡策略
        case 1:
            if(1 == gGHardPara_104[eGHardPara104_BalTact])                  //压差积分均衡
            {
                CalcCellVoltSubInte();                                      //计算单体压差积分
                sTimer++;
                if((sTimer / 5) >= (u16)gGHardPara_104[eGHardPara104_BalCycT] * 6)//积分定时为200ms周期(每分钟积分300次)
                {
                    sTimer = 0;
                    sState = 2;                                             //进入均衡判断
                }
                else
                {
                    sState = 0;                                             //返回上一步花费一半积分时间
                }
            }
            else
            {
                sState = 0;                                                 //不需检测均衡
            }
            break;

        //检测均衡状态需求
        case 2:
            if(1 == gGHardPara_104[eGHardPara104_BalTact])                  //压差积分均衡
            {
                if(0 == gGHardPara_104[eGHardPara104_BalCirc])              //被动放电均衡电路
                {
                    JudgeBalanReqByPasCir();                                //判断被动放电均衡电路均衡请求
                }
                else if(1 == gGHardPara_104[eGHardPara104_BalCirc])         //单向主动充电+被动放电均衡电路
                {
                    ;
                }
                else                                                        //参数错误
                {
                	ClrBalanCheckData();                                    //清除均衡检测数据
                }

                sState = 3;
            }
            else
            {
                sState = 0;                                                 //不需检测均衡
            }
            break;

        //记录均衡状态需求和数据复位
        case 3:
        	UpdateBalanShowState();                                         //更新均衡状态显示
        	ClrBalanSubVolt();                                              //清除压差积分值
            sState = 0;
            break;

        default:
            sState = 0;
            break;
    }
}

/*=================================================================================================
** @Name      : void SetGroupVoltPasBalanReqState(u8 ecu, u32 cellState)
** @Input     : ecu:从控设备节点号 cellState:从控电压被动均衡请求状态:按bit位表示,每bit代表从控内一个电池(0:关闭均衡 :开启被动均衡)
** @Output    : 电池组内各
** @Function  : 设置电池组内各从控电压被动均衡请求状态
** @The notes : 建议只在调试模式调用,否则会与均衡策略计算值冲突
**===============================================================================================*/
void SetGroupVoltPasBalanReqState(u8 ecu, u32 cellState)
{
	if(ecu < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
	{
		sGPasBalanReqState[ecu] = cellState;
	}
}

/*=================================================================================================
** @Name      : u32 GetGroupVoltPasBalanReqState(u8 ecu)
** @Input     : ecu:从控设备节点号(即Pack序号)
** @Output    : 电池组内各从控电压被动均衡请求状态:按bit位表示,每bit代表从控内一个电池(0:关闭均衡 :开启被动均衡)
** @Function  : 获取电池组内各从控电压被动均衡请求状态
** @The notes : 包括均衡策略计算值和调试值
**===============================================================================================*/
u32 GetGroupVoltPasBalanReqState(u8 ecu)
{
	if(ecu < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
	{
		return(sGPasBalanReqState[ecu]);
	}
	else
	{
		return(0);
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void ClrBalanCheckData(void)
** @Input     : void
** @Output    : void
** @Function  : 清除均衡检测数据
** @The notes : 压差计算值和均衡请求状态
**===============================================================================================*/
static void ClrBalanCheckData(void)
{
    u16 i = 0;

    for(i = 0; i < GROUP_MAX_CELVNUM; i++)
    {
    	sGBalanSubVolt[i] = 0;
    }

    for(i = 0; i < SLAVE_MAX_NUM; i++)
    {
    	sGPasBalanReqState[i] = 0;
    }
}

/*=================================================================================================
** @Name      : static void ClrBalanSubVolt(void)
** @Input     : void
** @Output    : void
** @Function  : 清除均衡压差计算值
** @The notes :
**===============================================================================================*/
static void ClrBalanSubVolt(void)
{
    u16 i = 0;

    for(i = 0; i < GROUP_MAX_CELVNUM; i++)
    {
    	sGBalanSubVolt[i] = 0;
    }
}

/*=================================================================================================
** @Name      : static void CalcCellVoltSubInte(void)
** @Input     : void
** @Output    : void
** @Function  : 计算单体压差积分
** @The notes :
**===============================================================================================*/
static void CalcCellVoltSubInte(void)
{
    u16 i = 0;

    for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum]; i++)
    {
        if(0 == gGHardPara_104[eGHardPara104_BalCirc])                                          //仅被动放电均衡电路
        {
            //最低电压必须达到最小限制值
            if(GetGCellMinVoltAPI() >= BALAN_DHG_MIN_VOLT)
            {
                sGBalanSubVolt[i] += ((s32)GetGCellVoltAPI(i) - (s32)GetGCellMinVoltAPI());     //单节与最小值压差累加和
            }
            //平均电压必须达到最小限制值(且有严重高电压的电池)
            else if((GetGCellMinVoltAPI() >= gGBmuGenPara_102[eBmuGenPara102_VL2Lim]) && (GetGCellAvgVoltAPI() >= BALAN_DHG_MIN_VOLT) && (GetGCellMaxVoltAPI() >= (GetGCellAvgVoltAPI() + ((u16)gGHardPara_104[eGHardPara104_BalDhgV] * 2))))
            {
                sGBalanSubVolt[i] += ((s32)GetGCellVoltAPI(i) - (s32)GetGCellAvgVoltAPI());     //单节与平均值压差累加和
            }
            //平均电压必须达到最小限制值(且最小值高于低压保护,且压差严重大)
            else if((GetGCellAvgVoltAPI() >= BALAN_DHG_MIN_VOLT) && (GetGCellMinVoltAPI() >= (gGBmuGenPara_102[eBmuGenPara102_VL2Lim] + 80)) && (GetGCellMaxVoltAPI() >= (GetGCellMinVoltAPI() + ((u16)gGHardPara_104[eGHardPara104_BalDhgV] * 2))))
            {
                sGBalanSubVolt[i] += ((s32)GetGCellVoltAPI(i) - (s32)GetGCellMinVoltAPI());     //单节与最小值压差累加和
            }
            else
            {
                sGBalanSubVolt[i] = 0;
            }
        }
        else                                                                                    //有主动均衡电路
        {
            sGBalanSubVolt[i] += ((s32)GetGCellVoltAPI(i) - (s32)GetGCellAvgVoltAPI());         //单节与平均值压差累加和
        }
    }
}

/*=================================================================================================
** @Name      : static void JudgeBalanReqByPasCir(void)
** @Input     : void
** @Output    : void
** @Function  : 判断被动放电均衡电路均衡请求
** @The notes :
**===============================================================================================*/
static void JudgeBalanReqByPasCir(void)
{
    u8 slave = 0;           //从控序号
    u8 slaNum = 0;          //BMU数目
    u8 slaCelNum = 0;       //BMU内单体节数
    u8 slaCel = 0;          //BMU内单体电池号
    u16 slaFstCel = 0;      //BMU第一节单体对应电池号
    u16 clusterCel = 0;     //单体所在电池组电池号

    u8 onNum = 0;           //开启节数
    u8 index0 = 0;
    u8 index1 = 0;
    u8 temp1 = 0;
    s32 temp0 = 0;
    u8 subCell[SLAVE_MAX_CELVNUM] = {0};    //从控各单体序号
    s32 subVolt[SLAVE_MAX_CELVNUM] = {0};   //从控各单体压差

    slaNum = (u8)gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum];
    slaCelNum = (u8)gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum];
    //allCelNum = gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum];

    for(slave = 0; slave < slaNum; slave++)
    {
    	/*均衡过温保护*/
        if((eErr_B == DiagGetErrLastStateAPI(eERR_ID_BAL_TH))                       /*从控均衡过温告警*/
            && (GetGBalanSlavMaxTAPI(slave) >= BALAN_PAS_PRO_TEMP))                 /*该从控均衡过温*/
        {
            sGPasBalanReqState[slave] = 0;                                          /*设置该从控被动放电均衡放电电路全部关闭*/
        }
        /*未均衡过温保护*/
        else
        {
        	onNum = 0;
			slaFstCel = slaCelNum * slave;                                          /*获取各个电池模块第1节电池在整组中的位置*/

			/*找各从控的单节电池电压累积超过阈值的电池*/
			for(slaCel = 0; slaCel < slaCelNum; slaCel++)
			{
				clusterCel = slaFstCel + slaCel;

				if((sGBalanSubVolt[clusterCel] > 0)                                 /*需要放电均衡*/
					&& (ABS(sGBalanSubVolt[clusterCel], 0) > ((u32)GetDhgBalanSubVolt(clusterCel)
					* ((u32)gGHardPara_104[eGHardPara104_BalCycT] * 30))))          /*周期内差值积分达到放电阀值(每分钟积分300次)*/
				{
					BitSet(sGPasBalanReqState[slave], slaCel);                      /*设置被动放电均衡放电电路开启*/

					subVolt[onNum] = sGBalanSubVolt[clusterCel];                    /*记录需要开启均衡电池压差和电池号*/
				    subCell[onNum] = slaCel;
					onNum++;
				}
				else
				{
					BitClr(sGPasBalanReqState[slave], slaCel);                      /*清除该BMU被动均衡放电电路*/
				}
			}

			/*均衡温度偏高且需开启节数超限(8节)*/
			if((GetGBalanSlavMaxTAPI(slave) >= BALAN_PAS_LIM_TEMP)                  /*该从控均衡温度偏高*/
				&& (onNum > BALAN_PAS_LIM_CELL))                                    /*该从控需开启节数超限(8节)*/
			{
				sGPasBalanReqState[slave] = 0;                                      /*开启电池号清零重新计算*/

				/*由大到小排序*/
			    //for(index0 = 0; index0 < onNum; index0++)
				for(index0 = 0; index0 < BALAN_PAS_LIM_CELL; index0++)              /*只需找到允许开启的电池数(8节)*/
			    {
			        for(index1 = index0; index1 < onNum; index1++)
			        {
			            if(subVolt[index0] < subVolt[index1])
			            {
							/*将最大的放在最前面*/
			            	temp0 = subVolt[index0];
			            	subVolt[index0] = subVolt[index1];
			            	subVolt[index1] = temp0;

							/*电池号跟随调换*/
			                temp1 = subCell[index0];
			                subCell[index0] = subCell[index1];
			                subCell[index1] = temp1;
			            }
			        }

					BitSet(sGPasBalanReqState[slave], subCell[index0]);             /*设置被动放电均衡放电电路开启*/
			    }
			}
    	}
    }
}

/*=================================================================================================
** @Name      : static u8 GetDhgBalanSubVolt(u16 cell)
** @Input     : cell:电池号
** @Output    : 开启放电均衡的压差 1mv
** @Function  : 获取开启放电均衡的压差
** @The notes :
**===============================================================================================*/
static u8 GetDhgBalanSubVolt(u16 cell)
{
    //u8 num = 0;
    u8 index = 0;
    u8 subVolt = 0;

    //num = cell % 16;
    index = cell / 16;

    //索引错误
    if(index >= GINDEX91_LEN)
    {
    	subVolt = 0xff;
    }
    //未开启均衡取开启压差
    //else if(0 == BitGet(gGDhgBalanInfo_91[index], num))
    else
    {
        //非平台区取配置值
        if((GetGCellAvgVoltAPI() >= (gGBmuGenPara_102[eBmuGenPara102_CellPlaUpV] - 70))
            || (GetGCellAvgVoltAPI() <= (gGBmuGenPara_102[eBmuGenPara102_CellPlaDnV] + 50)))
        {
        	subVolt = gGHardPara_104[eGHardPara104_BalDhgV];
        }
        //平台区取配置值2/3
        else
        {
        	subVolt = (u8)((u16)gGHardPara_104[eGHardPara104_BalDhgV] * 2 / 3);
        }
    }
//    //已开启均衡则取一半压差
//    else
//    {
//        //非平台区取配置值
//        if((GetGCellAvgVoltAPI() >= gGBmuGenPara_102[eBmuGenPara102_CellPlaUpV])
//            || (GetGCellAvgVoltAPI() <= gGBmuGenPara_102[eBmuGenPara102_CellPlaDnV]))
//        {
//        	subVolt = gGHardPara_104[eGHardPara104_BalDhgV] / 2;
//        }
//        //平台区取配置值1/4
//        else
//        {
//        	subVolt = gGHardPara_104[eGHardPara104_BalDhgV] / 4;
//        }
//    }

    return(subVolt);
}

/*=================================================================================================
** @Name      : static void UpdateBalanShowState(void)
** @Input     : void
** @Output    : void
** @Function  : 更新电池均衡显示状态
** @The notes :
**===============================================================================================*/
static void UpdateBalanShowState(void)
{
	//仅自研菊花链时需要更新显示均衡状态(CAN通信通过接收从控均衡状态)
	#if(0 == BMU_BSU_COM_TYPE)
	u8 cell = 0;
	u8 index = 0;
	u8 lengh = 0;
	u8 slave = 0;

	/*被动放电均衡电路*/
    if(0 == gGHardPara_104[eGHardPara104_BalCirc])
    {
		/*从控刚好16串电芯*/
		if(16 == gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum])
		{
			/*逐个从控赋值*/
			for(index = 0; index < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; index++)
			{
				gGChgBalanInfo_90[index] = 0;
				gGDhgBalanInfo_91[index] = sGPasBalanReqState[index];
			}
		}
		/*从控8串电芯*/
		else if(8 == gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum])
		{
			/*计算索引长度*/
			lengh = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / 2;

			/*两个从控合并到一个索引*/
			for(index = 0; index < lengh; index++)
			{
				slave = index * 2;
				gGChgBalanInfo_90[index] = 0;
				gGDhgBalanInfo_91[index] = ((sGPasBalanReqState[slave + 1] & 0x00ff) << 8) | (sGPasBalanReqState[slave] & 0x00ff);
			}
		}
		/*从控4串电芯*/
		else if(4 == gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum])
		{
			/*计算索引长度*/
			lengh = gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / 4;

			/*两个从控合并到一个索引*/
			for(index = 0; index < lengh; index++)
			{
				slave = index * 4;
				gGChgBalanInfo_90[index] = 0;
				gGDhgBalanInfo_91[index] = ((sGPasBalanReqState[slave + 3] & 0x000f) << 12) | ((sGPasBalanReqState[slave + 2] & 0x000f) << 8) | ((sGPasBalanReqState[slave + 1] & 0x000f) << 4) | (sGPasBalanReqState[slave] & 0x000f);
			}
		}
		/*从控电芯串数非4的倍数*/
		else
		{
			index = 0;
			lengh = 0;

			/*每个从控逐个bit位取值赋值*/
			for(slave = 0; slave < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; slave++)
			{
				/*每个从控逐个bit位取值赋值*/
				for(cell = 0; cell < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum]; cell++)
				{
					if(1 == BitGet(sGPasBalanReqState[slave], cell))
					{
						BitSet(gGDhgBalanInfo_91[index], lengh);
					}
					else
					{
						BitClr(gGDhgBalanInfo_91[index], lengh);
					}

					lengh++;
					/*满1个显示索引*/
					if(lengh >= 16)
					{
						gGChgBalanInfo_90[index] = 0;
						index++;
						lengh = 0;
					}
				}
			}
		}
    }
    /*单向主动充电+被动放电均衡电路*/
    else if(1 == gGHardPara_104[eGHardPara104_BalCirc])
    {
        ;
    }
    /*参数错误*/
    else
    {
		/*计算索引长度*/
		lengh = (gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] + 15) / 16;

        for(index = 0; index < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; index++)
        {
        	gGChgBalanInfo_90[index] = 0;
        	gGDhgBalanInfo_91[index] = 0;
        }
    }
    #endif
}
#endif

