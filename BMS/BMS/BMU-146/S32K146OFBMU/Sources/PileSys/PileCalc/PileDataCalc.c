/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : PileDataCalc.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 堆控主要数据计算模块
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
#include "PileDataCalc.h"

#if(1 == PILE_MANAG_EN)
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern s16 gPSysMainInfo_60[];				/*系统整体信息数组*/
extern u16 gPSysCapInfo_61[];				/*系统电量信息数组*/
extern u16 gPVoltCharInfo_62[]; 			/*电压特征信息(最大值)数组*/
extern u16 gPVoltCharInfo_63[];				/*电压特征信息(最小值)数组*/
extern s16 gPTempCharInfo_64[]; 			/*温度特征信息(最大值)数组*/
extern s16 gPTempCharInfo_65[];				/*温度特征信息(最小值)数组*/
extern u16 gPSumVCharInfo_66[];				/*总压特征信息数组*/
extern s16 gPCurrCharInfo_67[];				/*电流特征信息数组*/

extern u16 gPStatistTime_70[]; 				/*时间统计信息数组*/
extern u16 gPStatistErr_71[]; 				/*故障统计信息数组*/
extern u16 gPStatistCapEner_72[];			/*当前能量统计信息数组*/

extern u16 gSysGenParaRO_100[];  			/*系统通用参数设置*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
//=============================================================================================
//函数名称	: void PileMainDataCalcTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 计算堆运行主要信息(电流、电压、功率、SOC、SOE、SOH)
//注意事项	: 100ms
//=============================================================================================
void PileMainDataCalcTask(void)
{   
    u8 index = 0;
    u8 group = 0;
    u8 runNum = 0;
    u8 groupNum = 0;
    u32 sumSoh = 0;
    u32 sumSoc = 0;
    u32 sumSoe = 0;
    u32 sumVolt = 0;
    s32 sumCurr = 0;
    s32 sumPower = 0;
    u32 sampVolt = 0;
    u16 pileVolt = 0;
    u16 pileSoh = 0;
    u32 pileSoe = 0;
    u32 pileSoc = 0;
    static u16 sClrGroup = 0;
    
    //初始化完成开始计算
    if(eWORK_INIT == GetPileWorkStateAPI())
    {
    	gPSysMainInfo_60[ePSysMainInfo60_SumV] = 0;     //设置堆采样总压为0
    	gPSysMainInfo_60[ePSysMainInfo60_OutC] = 0;     //设置堆电流为0
    	gPSysMainInfo_60[ePSysMainInfo60_OutP] = 0;     //设置堆功率为0
    	gPSysMainInfo_60[ePSysMainInfo60_PreV] = 0;     //设置堆预充电压为0

        gPVoltCharInfo_62[ePVCharInfo62_SumVolt] = 0;   //堆累计总压为0
        
        gPSysCapInfo_61[ePSysCapInfo61_SOC] = 0;        //设置堆电池SOC为0%
        gPSysCapInfo_61[ePSysCapInfo61_SOE] = 0;        //设置堆电池SOE为0%
        gPSysCapInfo_61[ePSysCapInfo61_SOH] = 1000;     //设置堆电池SOH为100%
        return;
    }
    
    runNum = GetPileRunGroupNumAPI();                   //正在运行的电池组
    if(0 == runNum)                                     //没有正在运行的组
    {
        for(index = 0; index < gSysGenParaRO_100[eSGenPara100_AllGNum]; index++)
        {
            if((1 == GetPGroupEnStateAPI(index))            //该组使能
                && (0 == GetPGroupCanOffStateAPI(index))    //通讯正常
                && (eWORK_INIT != GetPGroupWorkStateAPI(index)))//该组初始化完成
            {
                groupNum++;
                pileSoc += GetPGroupBattSocAPI(index);
                pileSoe += GetPGroupBattSoeAPI(index);
                sumVolt += (u32)GetPGroupCSumVoltAPI(index);//累计电压和
                sampVolt += (u32)GetPGroupSSumVoltAPI(index);//采样电压和
                
                sumSoh += GetPGroupBattSohAPI(index);
                
                if(eWORK_ERR != GetPGroupWorkStateAPI(index))//该组非故障
                {
                    runNum++;
                    sumSoc += GetPGroupBattSocAPI(index);
                    sumSoe += GetPGroupBattSoeAPI(index);
                }
                
                BitClr(sClrGroup, index);               //消除已清除该组信息
            }
            else if((1 == GetPGroupCanOffStateAPI(index))//通讯断线
                && (0 == GetPGroupCanRcvStateAPI(index)))//未接收到任何CAN信息
            {
                if(0 == BitGet(sClrGroup, index))       //未清除组信息
                {
                	ClrPileGroupInfoAPI(index);         //清除该组信息
                    BitSet(sClrGroup, index);           //记录已清除该组信息
                }
            }
            else
            {
                BitClr(sClrGroup, index);               //消除已清除该组信息
            }
        }
        
        if(groupNum == 0)
        {
            pileVolt = 0;
            sumVolt = 0;
            pileSoc = 0;
            pileSoe = 0;
        }
        else
        {
            pileVolt = (u16)(sampVolt / groupNum);      //堆采样总电压
            sumVolt = (u16)(sumVolt / groupNum);        //堆累计总电压
            pileSoc = pileSoc / groupNum;               //堆SOC
            pileSoe = pileSoe / groupNum;               //堆SOE
        }
        
        sumCurr = 0;
        sumPower = 0;
    }
    else                                                //有电池组运行
    {
        //计算运行组数据
        for(index = 0; index < gSysGenParaRO_100[eSGenPara100_AllGNum]; index++)
        {
            if((1 == GetPGroupEnStateAPI(index))            //该组使能
                && (0 == GetPGroupCanOffStateAPI(index))    //通讯正常
                && (eWORK_INIT != GetPGroupWorkStateAPI(index)))//该组初始化完成
            {
                groupNum++;
                sumSoh += GetPGroupBattSohAPI(index);

                BitClr(sClrGroup, index);                   //消除已清除该组信息
            }
            else if((1 == GetPGroupCanOffStateAPI(index))   //通讯断线
                && (0 == GetPGroupCanRcvStateAPI(index)))   //未接收到任何CAN信息
            {
                if(0 == BitGet(sClrGroup, index))           //未清除组信息
                {
                    ClrPileGroupInfoAPI(index);             //清除该组信息
                    BitSet(sClrGroup, index);               //记录已清除该组信息
                }
            }
            else
            {
                BitClr(sClrGroup, index);                   //消除已清除该组信息
            }
            
            //计算运行组功率、电流和SOC
            if(index < runNum)
            {
                group = GetPileRunGroupIdAPI(index);        //获取正在运行的电池组号
                sumCurr += (s32)GetPGroupOutCurrAPI(group); //电流和
                sumVolt += (u32)GetPGroupCSumVoltAPI(group);//累计总压和
                sampVolt += (u32)GetPGroupSSumVoltAPI(group);//采样总压和
                sumPower += (s32)GetPGroupOutPowerAPI(group);//功率和

                sumSoc += GetPGroupBattSocAPI(group);
                sumSoe += GetPGroupBattSoeAPI(group);
            }
        }
        
        sumVolt = (u16)(sumVolt / runNum);                  //堆累计总电压
        pileVolt = (u16)(sampVolt / runNum);          		//堆采样总电压

//        if((1 == IsInside(-50, sumCurr, 50))              //正在使用的组总电流小于等于5A
//          || ((sumCurr > 0) && (sumPower < 0))            //电流方向与功率方向不一致
//        	|| ((sumCurr < 0) && (sumPower > 0)))           //电流方向与功率方向不一致
//        {
//            pileVolt = (u16)(sampVolt / runNum);          //堆采样总电压
//        }
//        else
//        {
//            pileVolt = (u16)(sumPower * 10000 / sumCurr); //0.1kW*10000/0.1A=0.1V(这样计算功率变化时总压会跳动)
//        }
        
        if(sumCurr > 32760)                                 //超过s16最大值
        {
            sumCurr = 32760;
        }
        else if(sumCurr < -32760)                           //超过s16最小值
        {
            sumCurr = -32760;
        }
        else if((sumCurr > -10) && (sumCurr < 10))          //±1A内
        {
        	if(eCURR_IDLE == GetPileChgStateAPI())          //当前为空闲状态
        	{
        		sumCurr = 0;                                //显示0A
        	}
        }
        
        if(sumPower > 32760)                                //超过s16最大值
        {
            sumPower = 32760;
        }
        else if(sumPower < -32760)                          //超过s16最小值
        {
            sumPower = -32760;
        }
        else if(0 == sumCurr)                               //电流显示0A
        {
        	sumPower = 0;                                   //功率显示0
        }
        
        sClrGroup = 0;
    }
    
    if(0 == runNum)                                         //无运行的电池组则取接收到的电池组
    {
        pileSoc = pileSoc;
        pileSoe = pileSoe;
    }
    else                                                    //全部求平均  
    {
        pileSoc = (u16)(sumSoc / runNum);                   //全部求平均
        pileSoe = (u16)(sumSoe / runNum);                   //全部求平均
    }
    
    if(0 == groupNum)                                       //未找到符合计算SOH条件的电池组
    {
        pileSoh = 0;
    }
    else                                                    //全部求平均  
    {
        pileSoh = (u16)(sumSoh / groupNum);                 //全部求平均
    }
    
    if(pileSoe > 1000)
    {
        pileSoe = 1000;
    }
    
    if(pileSoc > 1000)
    {
        pileSoc = 1000;
    }
    
    if(pileSoh > 1000)
    {
        pileSoh = 1000;
    }

    gPSysCapInfo_61[ePSysCapInfo61_SOH] = pileSoh;          //电池SOH
    gPSysCapInfo_61[ePSysCapInfo61_SOE] = pileSoe;          //电池SOE
    gPSysCapInfo_61[ePSysCapInfo61_SOC] = pileSoc;          //电池SOC
    
    gPSysMainInfo_60[ePSysMainInfo60_OutC] = (s16)sumCurr;  //电流0.1A
    gPSysMainInfo_60[ePSysMainInfo60_OutP] = (s16)sumPower; //堆功率0.1KW
    gPSysMainInfo_60[ePSysMainInfo60_SumV] = pileVolt;      //堆采样总压0.1V
    gPVoltCharInfo_62[ePVCharInfo62_SumVolt] = sumVolt;     //堆累计总压0.1V
}

//=============================================================================================
//函数名称	: void PileCharaInfoCalcTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 计算电池堆特征信息(最大最小值)任务
//注意事项	: 1s
//=============================================================================================
void PileCharaInfoCalcTask(void)
{
    u8 i = 0;
    u8 num = 0;                             //电池组数
    u8 group = 0;                           //电池组号
    u8 sysState = 0;                        //堆状态
    s16 value = 0;                          //中间值
    u16 maxVolt = 0;                        //堆最大单节电压值
    u16 minVolt = 0x7fff;                   //堆最小单节电压值
    u32 avgVolt = 0;                      	//堆平均电压值
    u32 preVolt = 0;                      	//堆预充电压值

    s16 maxTemp = -40;                      //堆最大单节温度值
    s16 minTemp = 0x7fff;                   //堆最小单节温度值
    s32 avgTemp = 0;                        //堆平均温度值
    s32 envTemp = 0;                        //堆环境温度值
    
    u16 minVGroup = 0;                      //堆最小单节电压对应组号
    u16 maxVGroup = 0;                      //堆最大单节电压对应组号
    u16 minTGroup = 0;                      //堆最小单节温度对应组号
    u16 maxTGroup = 0;                      //堆最大单节温度对应组号
    
    s16 minSumVolt = 0x7fff;                //堆最小单组电压值
    u16 minSumVG = 0;                       //堆最小单组电压对应组号
    s16 maxSumVolt = 0;                     //堆最小单组电压值
    u16 maxSumVG = 0;                       //堆最小单组电压对应组号
    s16 minCurr = 0x7fff;                   //堆最小单组电流值
    u16 minCurrG = 0;                       //堆最小单组电流对应组号
    s16 maxCurr = 0;                        //堆最小单组电流值
    u16 maxCurrG = 0;                       //堆最小单组电流对应组号
    
    sysState = (u8)GetPileWorkStateAPI();
    
    if(sysState == eWORK_INIT)              //自检状态清零
    {
        maxVolt = 0;
        minVolt = 0;
        avgVolt = 0;
        preVolt = 0;

        maxTemp = 0;
        minTemp = 0;
        avgTemp = 0;
        envTemp = 0;
        
        minSumVolt = 0;
        maxSumVolt = 0;
        minCurr = 0;
        maxCurr = 0;
    }
    else
    {
        num = GetPileRunGroupNumAPI();                  //获取正在运行的电池组数目
        if((eWORK_RUN == sysState) && (num > 0))        //求运行组电池
        {
            for(i = 0; i < num; i++)
            {
                group = GetPileRunGroupIdAPI(i);        //运行电池组号
                
                value = GetPGroupCellMaxVoltAPI(group); //求运行组电压最大值中的最大值
                if(value > maxVolt)
                {
                    maxVolt = value;
                    maxVGroup = group;
                }
                
                value = GetPGroupCellMinVoltAPI(group); //求运行组电压最小值中的最小值
                if(value < minVolt)
                {
                    minVolt = value;
                    minVGroup = group;
                }
                
                value = GetPGroupCellMaxTempAPI(group); //求运行组温度最大值中的最大值
                if(value > maxTemp)
                {
                    maxTemp = value;
                    maxTGroup = group;
                }
                
                value = GetPGroupCellMinTempAPI(group); //求运行组温度最小值中的最小值
                if(value < minTemp)
                {
                    minTemp = value;
                    minTGroup = group;
                }
                
                value = GetPGroupSSumVoltAPI(group);    //求运行组总电压最大值和最小值
                if(value > maxSumVolt)
                {
                    maxSumVolt = value;
                    maxSumVG = group;
                }
                if(value < minSumVolt)
                {
                    minSumVolt = value;
                    minSumVG = group;
                }
                
                value = ABS(GetPGroupOutCurrAPI(group), 0);  //求运行组电流最大值和最小值(绝对值)
                if(value > ABS(maxCurr, 0))
                {
                    maxCurr = GetPGroupOutCurrAPI(group);
                    maxCurrG = group;
                }
                if(value < ABS(minCurr, 0))
                {
                    minCurr = GetPGroupOutCurrAPI(group);
                    minCurrG = group;
                }
                
                avgVolt += GetPGroupAvgVoltAPI(group);
                preVolt += GetPGroupPreVoltAPI(group);
                avgTemp += GetPGroupAvgTempAPI(group);
                envTemp += GetPGroupEnvirTempAPI(group);
            }

            avgVolt = avgVolt / num;
            preVolt = preVolt / num;
            avgTemp = avgTemp / num;
            envTemp = envTemp / num;
        }
        else                                    //求使能组电池
        {
        	num = 0;

            for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
            {
                if((1 == GetPGroupEnStateAPI(i))
                    && (0 == GetPGroupCanOffStateAPI(i))
                    && (eWORK_INIT != GetPGroupWorkStateAPI(i)))
                {
                	num++;

                    value = GetPGroupCellMaxVoltAPI(i); //求各组电压最大值中的最大值
                    if(value > maxVolt)
                    {
                        maxVolt = value;
                        maxVGroup = i;
                    }
                    
                    value = GetPGroupCellMinVoltAPI(i); //求各组电压最小值中的最小值
                    if(value < minVolt)
                    {
                        minVolt = value;
                        minVGroup = i;
                    }
                    
                    value = GetPGroupCellMaxTempAPI(i); //求各组温度最大值中的最大值
                    if(value > maxTemp)
                    {
                        maxTemp = value;
                        maxTGroup = i;
                    }
                    
                    value = GetPGroupCellMinTempAPI(i); //求各组温度最小值中的最小值
                    if(value < minTemp)
                    {
                        minTemp = value;
                        minTGroup = i;
                    }
                    
                    value = GetPGroupSSumVoltAPI(i);    //求各组总电压最大值和最小值
                    if(value > maxSumVolt)
                    {
                        maxSumVolt = value;
                        maxSumVG = i;
                    }
                    if(value < minSumVolt)
                    {
                        minSumVolt = value;
                        minSumVG = i;
                    }
                    
                    value = ABS(GetPGroupOutCurrAPI(i), 0);  //求运行组电流最大值和最小值(绝对值)
                    if(value > maxCurr)
                    {
                        maxCurr = value;
                        maxCurrG = i;
                    }
                    if(value < minCurr)
                    {
                        minCurr = value;
                        minCurrG = i;
                    }

                    avgVolt += GetPGroupAvgVoltAPI(i);
                    preVolt += GetPGroupPreVoltAPI(i);
                    avgTemp += GetPGroupAvgTempAPI(i);
                    envTemp += GetPGroupEnvirTempAPI(i);
                }
            }
            
            if(num > 0)
            {
                avgVolt = avgVolt / num;
                preVolt = preVolt / num;
                avgTemp = avgTemp / num;
                envTemp = envTemp / num;
            }

            if(0x7fff == minVolt)                //无使能可用组
            {
                minVolt = 0;
            }
            
            if(0x7fff == minTemp)                //无使能可用组
            {
                minTemp = 0;
            }
            
            if(0x7fff == minSumVolt)            //无使能可用组
            {
                minSumVolt = 0;
            }
            
            if(0x7fff == minCurr)                //无使能可用组
            {
                minCurr = 0;
            }
        }
    }

    gPVoltCharInfo_62[ePVCharInfo62_MaxVolt] = maxVolt;
    gPVoltCharInfo_63[ePVCharInfo63_MinVolt] = minVolt;
    gPVoltCharInfo_63[ePVCharInfo63_AvgVolt] = avgVolt;
    gPTempCharInfo_64[ePTCharInfo64_MaxTemp] = maxTemp;
    gPTempCharInfo_64[ePTCharInfo64_AvgTemp] = avgTemp;
    gPTempCharInfo_65[ePTCharInfo65_MinTemp] = minTemp;
    gPTempCharInfo_65[ePTCharInfo65_EnvTemp] = envTemp;
    
    gPVoltCharInfo_62[ePVCharInfo62_MaxVG] = maxVGroup + 1;
    gPVoltCharInfo_63[ePVCharInfo63_MinVG] = minVGroup + 1;
    gPTempCharInfo_64[ePTCharInfo64_MaxTG] = maxTGroup + 1;
    gPTempCharInfo_65[ePTCharInfo65_MinTG] = minTGroup + 1;
    
    gPVoltCharInfo_62[ePVCharInfo62_MaxVNum] = GetPGroupCellMaxVNumAPI(maxVGroup);
    gPVoltCharInfo_63[ePVCharInfo63_MinVNum] = GetPGroupCellMinVNumAPI(minVGroup);
    gPTempCharInfo_64[ePTCharInfo64_MaxTNum] = GetPGroupCellMaxTNumAPI(maxTGroup);
    gPTempCharInfo_65[ePTCharInfo65_MinTNum] = GetPGroupCellMinTNumAPI(minTGroup);
    
    gPSumVCharInfo_66[ePSCharInfo66_MinSumVolt] = minSumVolt;
    gPSumVCharInfo_66[ePSCharInfo66_MaxSumVolt] = maxSumVolt;
    gPSumVCharInfo_66[ePSCharInfo66_MaxSumVG] = minSumVG + 1;
    gPSumVCharInfo_66[ePSCharInfo66_MinSumVG] = maxSumVG + 1;
    
    gPCurrCharInfo_67[ePCCharInfo67_MinCurr] = minCurr;
    gPCurrCharInfo_67[ePCCharInfo67_MaxCurr] = maxCurr;
    gPCurrCharInfo_67[ePCCharInfo67_MinCG] = minCurrG + 1;
    gPCurrCharInfo_67[ePCCharInfo67_MaxCG] = maxCurrG + 1;
}

//=============================================================================================
//函数名称	: void PileCapEnerTimeCalcTask(void)
//输入参数	: void
//输出参数	: void
//函数功能	: 计算电池堆电能电量和可充放时间任务
//注意事项	: 1s
//=============================================================================================
void PileCapEnerTimeCalcTask(void)
{
    u8 i = 0;
    u8 sysState = 0;
    u32 groupEnergy = 0;
    u32 totalEnergy = 0;
    u32 remainEnergy = 0;
    u32 totalCap = 0;
    
    u8 num = 0;
    u8 group = 0;
    u16 chgTime = 0;
    u16 dhgTime = 0;
    u16 avgTimes = 0;
    u32 sumChgT = 0;
    u32 sumDhgT = 0;
    u32 sumTimes = 0;
    
    //系统上电时长(只取自身时间)
    gPStatistTime_70[ePTime70_SysRun] = GetSysSecondTimeAPI() / 3600;
    
    //系统工作状态
    sysState = (u8)GetPileWorkStateAPI();
    
    //初始化状态数据清零
    if(eWORK_INIT == sysState)
    {
    	gPSysCapInfo_61[ePSysCapInfo61_AllCap] = 0;
    	gPStatistCapEner_72[ePCapEner72_NowEner] = 0;
    	gPStatistCapEner_72[ePCapEner72_AllEner] = 0;
        
    	gPStatistTime_70[ePTime70_CanChg] = 0;
    	gPStatistTime_70[ePTime70_CanDhg] = 0;
    	gPStatistTime_70[ePTime70_ChgCycle] = 0;
        return;
    }
    
    //能量求和,时间求平均
    for(i = 0; i < gSysGenParaRO_100[eSGenPara100_AllGNum]; i++)
    {
        if((1 == GetPGroupEnStateAPI(i))                        //该组使能
            && (0 == GetPGroupCanOffStateAPI(i))                //该组通讯正常
            && (1 == GetPGroupIsoSwhStateAPI(i))                //该组隔离开关闭合
            && ((eWORK_ERR != GetPGroupWorkStateAPI(i))         //该组无故障
            || (0 == GetPAllGroupNorStateAPI())))               //电池堆无可用组
        {
            groupEnergy = GetPGroupAllEnerAPI(i);               //电池组总电能0.1kWh
            if(groupEnergy < GetPGroupRemainEnerAPI(i))         //计算总电能比剩余电能小
            {
                groupEnergy = GetPGroupRemainEnerAPI(i);        //总电能取剩余电能
            }
            remainEnergy += GetPGroupRemainEnerAPI(i);          //获得剩余电能:0.1kWh
            totalEnergy += groupEnergy ;                        //获得总电能:0.1kWh
            
            totalCap += GetPGroupAllCapAPI(i);                  //获得总电量:1AH
            
            group++;                                            //记录有可充放电的电池组
            sumChgT += GetPGroupCanChgTimeAPI(i);               //累计无故障电池组可充电时间
            sumDhgT += GetPGroupCanDhgTimeAPI(i);               //累计无故障电池组可放电时间
            sumTimes += GetPGroupChgTimesAPI(i);                //累计充放电次数
        }
    }
    
    num = GetPileRunGroupNumAPI();
    
    //有运行组则重新计算运行电池组时间
    if((num > 0) && (num != group))
    {
        sumChgT = 0;
        sumDhgT = 0;
        sumTimes = 0;
        
        for(i = 0; i < num; i++)
        {
            group = GetPileRunGroupIdAPI(i);                //获取正在运行电池组的组号
            
            sumChgT += GetPGroupCanChgTimeAPI(group);       //累计运行电池组可充电时间
            sumDhgT += GetPGroupCanDhgTimeAPI(group);       //累计运行电池组可放电时间
            sumTimes += GetPGroupChgTimesAPI(i);            //累计充放电次数
        }
        
        group = num;
    }
    
    if(group > 0)                                           //有可充放电的电池组
    {
        if((eWORK_RUN != sysState)                          //非运行状态
            || (GetPileChgLimCurrAPI() > 0))                //允许充电
        {
            chgTime = (u16)(sumChgT / group);               //计算实际可充电时间
        }
        else                                                //运行状态不允许充电
        {
            chgTime = 0;                                    //可充电时间为0
        }
        
        if((eWORK_RUN != sysState)                          //非运行状态
            || (GetPileDhgLimCurrAPI() > 0))                //允许放电
        {
            dhgTime = (u16)(sumDhgT / group);               //计算实际可放电时间
        }
        else                                                //运行状态不允许放电
        {
            dhgTime = 0;                                    //可放电时间为0
        }
        
        avgTimes = (u16)(sumTimes / group);                 //平均充放电次数
    }
    else                                                    //无可充放电的电池组
    {
        chgTime = 0;
        dhgTime = 0;
        avgTimes = 0;
    }
    
    gPSysCapInfo_61[ePSysCapInfo61_AllCap] = totalCap;      //总电量(1AH)
    gPStatistCapEner_72[ePCapEner72_NowEner] = remainEnergy;//剩余电能(0.1kWh)
    gPStatistCapEner_72[ePCapEner72_AllEner] = totalEnergy; //总电能(0.1kWh)
    
    gPStatistTime_70[ePTime70_CanChg] = chgTime;            //可充电时间
    gPStatistTime_70[ePTime70_CanDhg] = dhgTime;            //可放电时间
    gPStatistTime_70[ePTime70_ChgCycle] = avgTimes;         //充放电次数
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

#endif
