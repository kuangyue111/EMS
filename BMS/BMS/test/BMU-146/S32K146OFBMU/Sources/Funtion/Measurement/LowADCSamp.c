/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : LowADCSamp.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : MCU的ADC通道采集功能函数模块
** @Instructions :
**===========================================================================================*/
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "osif.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "HigAFESamp.h"
#include "CurrSample.h"
#include "LowADCSamp.h"
 
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u8 sLVSampFinState = 0;                  /*ADC通道采样完成一遍标志(1完成)*/
static u8 sADCSampExpFlag = 0;                  /*ADC通道采样异常标(按位表示)*/
static u8 sADCCurrBackFlag = 0;                 /*ADC采样通道回检驱动电流标志(0:未回检 1:开始回检 2:正在回检 3:回检完成)*/

static u16 gADCSampAD[eBMU_ADC_SampNum] = {0};  /*采样AD值*/
static u16 gADCCurrAD[LV_CAD_FILT_TIME] = {0};  /*电流AD值滤波*/

/*环境温度传感器,阻值10K电阻*/
static const u32 sNTCEnvTTabR[] =               /*与亿纬电池NTC(10K)温度阻值取值表一致[ENV_T_TAB_LEN],[-41~125]*/
{
	221500,//-41℃
	204700, 193400, 182700, 172700, 163300, 154400, 146100, 138400, 131000, 124100,
	117600, 111500, 105800, 100400, 95250, 90440, 85900, 81610, 77570, 73750,
	70150, 66740, 63520, 60480,57600, 54870,52290, 49850, 47540, 45350,
	43270, 41300, 39440, 37670, 35990, 34390, 32880, 31440, 30070, 28770,
	27530, 26360, 25240, 24180, 23160, 22200, 21280, 20410, 19570, 18780,
	18020, 17300, 16610, 15950, 15320, 14720, 14150, 13600, 13080, 12580,
	12100, 11640, 11200, 10780, 10380, 10000, 9633, 9281, 8944, 8621,
	8311, 8015, 7730, 7457, 7195, 6944, 6703, 6471, 6249, 6035,
	5830, 5633, 5443, 5261, 5086, 4918, 4756, 4600, 4450, 4305,
	4166, 4033, 3904, 3780, 3660, 3545, 3434, 3327, 3224, 3124,
	3028, 2935, 2846, 2760, 2677, 2596, 2519, 2444, 2371, 2301,
	2234, 2169, 2106, 2045, 1986, 1929, 1874, 1820, 1769, 1719,
	1671, 1624, 1579, 1535, 1492, 1451, 1412, 1373, 1336, 1300,
	1265, 1231, 1198, 1166, 1135, 1105, 1076, 1048, 1020, 994,
	968, 943, 919, 895, 872, 850, 828, 808, 787, 768,
	748, 730, 712, 694, 677, 660, 644, 629, 613, 598,
	584, 570, 556, 543, 530, 518
};

/*低压电源板温温度传感器,阻值47K电阻(新规格)*/
static const u32 sNTCPwrTTabR[] =               /*板温NTC(47K)温度阻值取值表[PWR_T_TAB_LEN],[-41~125]*/
{
	2154500,//-41℃
	1888700, 1756900, 1635300, 1523000, 1419300, 1323400, 1234700, 1152700, 1076700, 1006300,
	941000, 880400, 824200, 771900, 723400, 678300, 636300, 597200, 560800, 526900,
	495310, 465810, 438290, 412580, 388570, 366120, 345130, 325490, 307100, 289890,
	273750, 258630, 244450, 231140, 218650, 206920, 195900, 185540, 175800, 166640,
	158020, 149900, 142250, 135040, 128250, 121850, 115800, 110100, 104710, 99630,
	94820, 90280, 85980, 81920, 78070, 74430, 70990, 67720, 64630, 61700,
	58920, 56280, 53770, 51400, 49140, 47000, 44964, 43029, 41189, 39439,
	37775, 36190, 34683, 33247, 31879, 30576, 29334, 28150, 27021, 25944,
	24917, 23936, 23000, 22106, 21252, 20436, 19657, 18911, 18198, 17517,
	16864, 16240, 15643, 15071, 14523, 13999, 13496, 13014, 12552, 12110,
	11685, 11278, 10887, 10512, 10152, 9806, 9474, 9155, 8849, 8554,
	8271, 7999, 7738, 7486, 7244, 7011, 6787, 6571, 6363, 6163,
	5971, 5785, 5606, 5434, 5268, 5107, 4953, 4804, 4660, 4522,
	4388, 4259, 4134, 4014, 3898, 3785, 3677, 3572, 3471, 3373,
	3278, 3187, 3099, 3013, 2930, 2850, 2773, 2698, 2625, 2555,
	2487, 2421, 2357, 2296, 2236, 2178, 2122, 2067, 2014, 1963,
	1913, 1865, 1819, 1773, 1729, 1687
};

extern u8 gGHardPara_104[]; 		            /*主控硬件参数配置信息*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static s32 ChangeSampADtoData(u8 channel, u16 ADdata)
** @Input     : channel:通道号 ADdata:采样AD值
** @Output    : 采样目标值 10mV/1mA
** @Function  : 将采样AD值转换为采样目标值
** @The notes : 电压值或电流值
**===========================================================================================*/
static s32 ChangeSampADtoData(u8 channel, u16 ADdata);

/*=============================================================================================
** @Name      : static u16 CalcHalCSampFiltAD(u16 ADdata)
** @Input     : ADdata:滤波前霍尔电流采样AD值
** @Output    : 滤波后霍尔电流采样AD值
** @Function  : 计算霍尔电流采样AD值滤波值
** @The notes : 采用窗口滤波
**===========================================================================================*/
static u16 CalcHalCSampFiltAD(u16 ADdata);

/*=============================================================================================
** @Name      : static u8 SearchEnvTempTabByR(u32 data)
** @Input     : data:要查表的数据阻值 1Ω
** @Output    : 应温度下标值
** @Function  : 通过NTC阻值查询环境温度表得到对应温度值
** @The notes :
**===========================================================================================*/
static u8 SearchEnvTempTabByR(u32 data);

/*=============================================================================================
** @Name      : static u8 SearchPwrTempTabByR(u32 data)
** @Input     : data:要查表的数据阻值 1Ω
** @Output    : 应温度下标值
** @Function  : 通过NTC阻值查询低压电源温度表得到对应板温温度值
** @The notes :
**===========================================================================================*/
static u8 SearchPwrTempTabByR(u32 data);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void ADCSampleInit(void)
** @Input     : void
** @Output    : void
** @Function  : ADC通道采样初始化
** @The notes :
**===========================================================================================*/
void ADCSampleInit(void)
{
    ;
}

/*=============================================================================================
** @Name      : void LowInfoSampleTask(void *p)
** @Input     : *p:形参,任务函数参数需要
** @Output    : void
** @Function  : ADC通道电压电流采样任务
** @The notes : 通过MCU的ADC通道读取
**===========================================================================================*/
void LowInfoSampleTask(void *p)
{
	s32 sampData = 0;
    static u8 sNum = 0;
    static u8 sState = 0;
    static u8 sErrTime = 0;
    static u8 sCurrTime = 0;
    static u8 sBeforeNum = 0;

    p = p;
    
	for(;;)
	{
		/*立刻回检高边驱动电流*/
		if(1 == sADCCurrBackFlag)
		{
			/*直接进入回检电流采样*/
			sState = 0;
			sErrTime = 0;
			sNum = eBMU_ADC_HS0C;
			sADCCurrBackFlag = 2;
		}

		switch(sState)
		{
			/*等待ADC通道空闲*/
			case 0:
				/*获取当前AD工作状态(空闲)*/
				if(0 == BSPADCGetWorkState())
				{
					/*清除采样失效故障*/
					BitClr(sADCSampExpFlag, 0);

					/*启动采样通道*/
					if(sNum >= eBMU_ADC_HS0C)
					{
						DEVADCStart(eDEVADC_MOD1, eADC_HS_AD);
					}
					else if((sNum == eBMU_ADC_ACCV) || (sNum == eBMU_ADC_PWMV)
						|| (sNum == eBMU_ADC_ENVIT) || (sNum == eBMU_ADC_MUXV))
					{
						DEVADCStart(eDEVADC_MOD0, (e_DevADCId)sNum);
					}
					else
					{
						DEVADCStart(eDEVADC_MOD1, (e_DevADCId)sNum);
					}
					OSIF_TimeDelay(5);
					sErrTime = 0;
					sState = 1;
				}
				else
				{
					sErrTime++;
					if(sErrTime >= 50)
					{
						BitSet(sADCSampExpFlag, 0);
					}
					OSIF_TimeDelay(10);
				}
				break;

			case 1:
				/*读取当前AD值*/
				if(sNum >= eBMU_ADC_HS0C)
				{
					gADCSampAD[sNum] = DEVADCGetSampAD(eDEVADC_MOD1, eADC_HS_AD);
				}
				else if((sNum == eBMU_ADC_ACCV) || (sNum == eBMU_ADC_PWMV)
					|| (sNum == eBMU_ADC_ENVIT) || (sNum == eBMU_ADC_MUXV))
				{
					gADCSampAD[sNum] = DEVADCGetSampAD(eDEVADC_MOD0, (e_DevADCId)sNum);
				}
				else if(sNum == eBMU_ADC_HalSIG)
				{
					/*霍尔电流AD值需要滤波*/
					gADCSampAD[sNum] = CalcHalCSampFiltAD(DEVADCGetSampAD(eDEVADC_MOD1, (e_DevADCId)sNum));
				}
				else
				{
					gADCSampAD[sNum] = DEVADCGetSampAD(eDEVADC_MOD1, (e_DevADCId)sNum);
				}

				/*将AD值转化为电压或电流值(10mV或1mA)*/
				sampData = ChangeSampADtoData(sNum, gADCSampAD[sNum]);

				/*ADC通道采样完成(接触器驱动电流回检值检测一次)*/
				if(sNum >= eBMU_ADC_HS0C)
				{
					/*根据当前片选的采集高边驱动通道记录对应电流值*/
					if(1 == DEVGPIOGetOutPortState(ePOut_DRVK1K2EN))
					{
						/*芯片通道1*/
						if(0 == DEVGPIOGetOutPortState(ePOut_DRVDSELALL))
						{
							sNum = eBMU_ADC_HS0C;
						}
						/*芯片通道2*/
						else
						{
							sNum = eBMU_ADC_HS1C;
						}
					}
					/*接触器3/4通道高边驱动电流选通*/
					else if(1 == DEVGPIOGetOutPortState(ePOut_DRVK3K4EN))
					{
						/*芯片通道1*/
						if(0 == DEVGPIOGetOutPortState(ePOut_DRVDSELALL))
						{
							sNum = eBMU_ADC_HS2C;
						}
						/*芯片通道2*/
						else
						{
							sNum = eBMU_ADC_HS3C;
						}
					}
					/*接触器5/6通道高边驱动电流选通*/
					else if(1 == DEVGPIOGetOutPortState(ePOut_DRVK5K6EN))
					{
						/*芯片通道1*/
						if(0 == DEVGPIOGetOutPortState(ePOut_DRVDSELALL))
						{
							sNum = eBMU_ADC_HS4C;
						}
						/*芯片通道2*/
						else
						{
							sNum = eBMU_ADC_HS5C;
						}
					}
					/*接触器7/8通道高边驱动电流选通*/
					else if(1 == DEVGPIOGetOutPortState(ePOut_DRVK7K8EN))
					{
						/*芯片通道1*/
						if(0 == DEVGPIOGetOutPortState(ePOut_DRVDSELALL))
						{
							sNum = eBMU_ADC_HS6C;
						}
						/*芯片通道2*/
						else
						{
							sNum = eBMU_ADC_HS7C;
						}
					}
					/*错误*/
					else
					{
						sNum = 0xf0;
					}

					/*记录HS驱动电流值*/
					InputGroupLVSideData(sNum, sampData);

					/*根据电流判断接触器状态*/
					DEVGPIOCheckHSCurrState((sNum - eBMU_ADC_HS0C), sampData);

					/*记录该次采样通道*/
					sBeforeNum = sNum;/*记录该次采样通道*/
					sNum = eBMU_ADC_HalSIG;/*回到电流采样*/

					/*正在回检高边驱动电流*/
					if(2 == sADCCurrBackFlag)
					{
						/*电流回检完成再查询一次所有通道*/
						sADCCurrBackFlag = 3;
					}
					/*不是在回检驱动电流*/
					else
					{
						/*500ms查询一次所有通道*/
						sADCCurrBackFlag = 0;
					}
				}
				/*HALL电流信号采样*/
				else if(sNum == eBMU_ADC_HalSIG)
				{
					/*记录电流值*/
					InputGroupLVSideData(sNum, sampData);
					CalcSampleADToCurr(eCADC_Hall, (s32)gADCSampAD[sNum]);

					/*准备下一通道*/
					//sNum++;
					/*未使能霍尔采样或第5次采电流(每采5次电流采1次其它通道,电流5ms周期,其它通道600ms周期)*/
					if((0 == BitGet(gGHardPara_104[eGHardPara104_CSampChan], eCADC_Hall))
						|| (sCurrTime >= LV_CAD_FILT_TIME))
					{
						sCurrTime = 0;
						sNum = sBeforeNum + 1;          /*回到其他通道采样的下一个通道采样*/
						if(sNum >= eBMU_ADC_SampNum)
						{
							sLVSampFinState = 1;
							sNum = eBMU_ADC_HalSIG + 1;
							sBeforeNum = 0;
						}
					}
					else
					{
						sCurrTime++;
					}
				}
				/*其他通道采样*/
				else
				{
					/*记录电压或电流值*/
					InputGroupLVSideData(sNum, sampData);

					/*准备下一通道*/
					//sNum++;
					sBeforeNum = sNum;/*记录该次采样通道*/
					sNum = eBMU_ADC_HalSIG;/*回到电流采样*/
				}

				/*立刻返回下一通道采样*/
				OSIF_TimeDelay(1);

				/*返回重新采样*/
				sState = 0;
				break;

			default:
				sNum = 0;
				sState = 0;
				sErrTime = 0;
				sBeforeNum = 0;
				OSIF_TimeDelay(100);
				break;
		}
	}
}

/*=============================================================================================
** @Name      : void PwmInfoCaptureTask(void)
** @Input     : void
** @Output    : void
** @Function  : PWM信息捕获任务
** @The notes : 100ms周期
**===========================================================================================*/
void PwmInfoCaptureTask(void)
{
	u32 frequency = 0;
	static u8 sErrTime = 0;
	static u32 sHisFrequency = 0;

	frequency = BSPReadPWMfrequencyAPI();
	if(frequency > 0xffff)
	{
		frequency = 0xffff;
	}

	/*频率不变化(采样值小于0.5V)*/
	if((frequency == sHisFrequency) && (GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmAdcV) <= 50))
	{
		/*连续3S*/
		if((sErrTime >= 30) || (0 == frequency))
		{
			frequency = 0;
		}
		else
		{
			sErrTime++;
		}
	}
	else
	{
		sErrTime = 0;
		sHisFrequency = frequency;
	}

    InputGroupDaisyPwmData(frequency);
}

/*=============================================================================================
** @Name      : void SampADCCurrBackHSStart(u8 flag)
** @Input     : flag:0-正常回检 1-立刻回检
** @Output    : void
** @Function  : 启动高边驱动电流回检采样
** @The notes : 仅在需要回检接触器驱动电流时调用
**===========================================================================================*/
void SampADCCurrBackHSStart(u8 flag)
{
    if(1 == flag)
    {
        /*立刻开始驱动电流回检采样*/
        sADCCurrBackFlag = 1;
    }
    else
    {
        /*正常巡查驱动电流回检采样*/
        sADCCurrBackFlag = 3;
    }
}

/*=============================================================================================
** @Name      : s32 CorrRealHalCurrByKB(s32 volt)
** @Input     : volt:修正前采样电流值 0.01mV
** @Output    : 修正后实际电流值  0.01mV
** @Function  : 通过板温修正霍尔实际电压采样值
** @The notes : 板温影响霍尔电压采样
**===========================================================================================*/
s32 CorrRealHalVoltByTemp(s32 volt)
{
    s32 chgdhg = 1;
    s32 realvolt = 0;

    //只修正50mV以上30℃以上产生的偏移
    if((volt >= 5000)
    	&& (GetGDcPwrSampTempAPI() >= 30)
    	&& (GetGDcPwrSampTempAPI() <= 100))
    {
    	//放电电压
    	if(volt >= 249000)
    	{
    		//修正原比例
    		chgdhg = 1;
    	}
    	//充电电压
    	else
    	{
    		//只修正一半比例
    		chgdhg = 2;
    	}

        //超过56℃修正6mV
    	if(GetGDcPwrSampTempAPI() >= 56)
    	{
    		realvolt = volt - (600 / chgdhg);
    	}
        //超过54℃修正5.5mV
    	else if(GetGDcPwrSampTempAPI() >= 54)
    	{
    		realvolt = volt - (550 / chgdhg);
    	}
        //超过53℃修正5mV
    	else if(GetGDcPwrSampTempAPI() >= 53)
    	{
    		realvolt = volt - (500 / chgdhg);
    	}
        //超过52℃修正4.5mV
    	else if(GetGDcPwrSampTempAPI() >= 52)
    	{
    		realvolt = volt - (450 / chgdhg);
    	}
        //超过50℃修正4mV
    	else if(GetGDcPwrSampTempAPI() >= 50)
    	{
    		realvolt = volt - (400 / chgdhg);
    	}
        //超过48℃修正3.5mV
    	else if(GetGDcPwrSampTempAPI() >= 48)
    	{
    		realvolt = volt - (350 / chgdhg);
    	}
        //超过45℃修正3mV
    	else if(GetGDcPwrSampTempAPI() >= 45)
    	{
    		realvolt = volt - (300 / chgdhg);
    	}
        //超过42℃修正2.5mV
    	else if(GetGDcPwrSampTempAPI() >= 42)
    	{
    		realvolt = volt - (250 / chgdhg);
    	}
        //超过40℃修正2mV
    	else if(GetGDcPwrSampTempAPI() >= 40)
    	{
    		realvolt = volt - (200 / chgdhg);
    	}
        //超过38℃修正1.5mV
    	else if(GetGDcPwrSampTempAPI() >= 38)
    	{
    		realvolt = volt - (150 / chgdhg);
    	}
        //超过35℃修正1mV
    	else if(GetGDcPwrSampTempAPI() >= 35)
    	{
    		realvolt = volt - (100 / chgdhg);
    	}
        //超过30℃修正0.5mV
    	else
    	{
    		realvolt = volt - (50 / chgdhg);
    	}
    }
    //电压温度正常范围
    else
    {
        //返回原始值
    	realvolt = volt;
    }

    return(realvolt);
}

/*=============================================================================================
** @Name      : u8 GetADCLVSampFinState(void)
** @Input     : void
** @Output    : ADC通道低压采样完成一遍标志 1:完成
** @Function  : 获取ADC通道低压采样完成一遍状态
** @The notes :
**===========================================================================================*/
u8 GetADCLVSampFinState(void)
{
    return(sLVSampFinState);
}

/*=============================================================================================
** @Name      : u8 GetADCSampHSBackCurrState(void)
** @Input     : void
** @Output    : 高边驱动电流回检采样状态:0:未回检 1:开始回检 2:正在回检 3:回检完成
** @Function  : 获取高边驱动电流回检采样状态
** @The notes :
**===========================================================================================*/
u8 GetADCSampHSBackCurrState(void)
{
    return(sADCCurrBackFlag);
}

/*=============================================================================================
** @Name      : u8 SampGetLowVSampExpFlagAPI(void)
** @Input     : void
** @Output    : 按位表示采样异常标志
** @Function  : 获取低压ADC通道采样异常标志
** @The notes :
**===========================================================================================*/
u8 SampGetLowVSampExpFlagAPI(void)
{
    return(sADCSampExpFlag);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static s32 ChangeSampADtoData(u8 channel, u16 ADdata)
** @Input     : channel:通道号 ADdata:采样AD值
** @Output    : 采样目标值 10mV/1mA
** @Function  : 将采样AD值转换为采样目标值
** @The notes : 电压值或电流值
**===========================================================================================*/
static s32 ChangeSampADtoData(u8 channel, u16 ADdata)
{
	s32 tempvolt = 0;
    
    /*MCU12位AD采样最大值4096*/
    if(ADdata > 4096)
    {
        ADdata = 0;
        BitSet(sADCSampExpFlag, 1);
    }
    else
    {
        BitClr(sADCSampExpFlag, 1);
    }
    
    switch(channel)
    {
        /*霍尔原始采样电压值(0~5V,0.01mV)*/
		case eBMU_ADC_HalSIG:
			//V=AD*3.3V/4096
			tempvolt = (u16)((u32)ADdata * 3300 / 4096);
			//tempvolt = tempvolt * 136;/*Y=37.5*X/27.5(A样板(4.5V量程)) */
			tempvolt = tempvolt * 150;/*Y=1.5*X(4.9V量程) */
			tempvolt = CorrRealHalVoltByTemp(tempvolt);/*通过温度修正线路电压偏移*/
			break;

		/*输入电源电压采样(0~36V,0.01V)*/
        case eBMU_ADC_PwrV:
            //V=AD*3.3V*10/4096
            tempvolt = (u16)((u32)ADdata * 3300 / 4096);
			tempvolt = tempvolt * 21 / 10;/*实际电压值,公式：Y=21*X   0.01V*/
            break;

        /*ACC信号电压采样*/
        case eBMU_ADC_ACCV:
            //V=AD*3.3V*10/4096(0~36V,0.01V)
            tempvolt = (u16)((u32)ADdata * 3300 / 4096);
			tempvolt = tempvolt * 209 / 100;/*实际电压值,公式：Y=21*X   0.01V*/
            break;
            
		/*霍尔供电电压采样(0~5V,0.01V)*/
		case eBMU_ADC_HalV:
			//V=AD*3.3V/4096
			tempvolt = (u16)((u32)ADdata * 3300 / 4096);
			tempvolt = tempvolt * 2 / 10;/*实际电压值,公式：Y=2*X   0.01V*/
			break;

		/*充电枪供电KC输入电压采样(0~36V,0.01V)*/
		case eBMU_ADC_CdcV:
			//V=AD*3.3V*10/4096
			tempvolt = (u16)((u32)ADdata * 3300 / 4096);
			tempvolt = tempvolt * 21 / 10;/*实际电压值,公式：Y=21*X   0.01V*/
			break;

		/*CC2信号电压采样*/
		case eBMU_ADC_CC2V:
			//V=AD*3.3V*2/4096(0~2.5V,0.01V)
			tempvolt = (u16)((u32)ADdata * 3300 / 4096);
			tempvolt = tempvolt * 2 / 10;/*实际电压值,公式：Y=2*X   0.01V*/
			break;

		/*MCU5V电源信号*/
		case eBMU_ADC_MCU5V:
			//V=AD*3.3V*10/4096
			tempvolt = (u16)((u32)ADdata * 3300 / 4096);
			tempvolt = tempvolt * 2 / 10;/*实际电压值,公式：Y=2*X   0.01V*/
			break;

        /*电源管理芯片6503输出信号电压采样(0~5V,0.01V)*/
		case eBMU_ADC_Aux5V:
			//V=AD*3.3V/4096
			tempvolt = (u16)((u32)ADdata * 3300 / 4096);
			tempvolt = tempvolt * 2 / 10;/*实际电压值,公式：Y=2*X   0.01V*/
			break;

		/*MUX电压检测*/
		case eBMU_ADC_MUXV:
			//V=AD*3.3V/4096
			tempvolt = (u16)((u32)ADdata * 3300 / 4096);/*硬件决定   */
			tempvolt = tempvolt / 10;/*1:1关系,0.01V*/
			break;

        /*PWM电压检测*/
		case eBMU_ADC_PWMV:
			//V=AD*3.3V/4096
			tempvolt = (u16)((u32)ADdata * 3300 / 4096);/*硬件决定*/
			tempvolt = tempvolt * 142 / 750;/*实际电压值   0.01V*/
			break;

		/*环境温度*/
		case eBMU_ADC_ENVIT:
			//V=AD*3.3V/4096
			tempvolt = (u16)((u32)ADdata * 3300 / 4096);/*硬件决定*/
			//tempvolt = ((tempvolt - 500) + 5) / 10;/*电压对应温度关系(A样板(不需查表))：Y = (X - 500) / 10 */

			/*10K阻值对应电压关系：Y=10*X/(3.3-X) */
			if(tempvolt >= 3300)
			{
				tempvolt = 0xefffffff;
			}
			else
			{
				tempvolt = (tempvolt * 10000) / (3300 - tempvolt);
			}

			/*根据阻值查表获取温度值 */
			tempvolt = SearchEnvTempTabByR(tempvolt) - 41;
			break;

        /*低压电源温度*/
		case eBMU_ADC_LPWRT:
			//V=AD*3.3V/4096
			tempvolt = (u16)((u32)ADdata * 3300 / 4096);/*硬件决定*/
			//tempvolt = ((tempvolt - 500) + 5) / 10;/*电压对应温度关系(A样板(不需查表))：Y = (X - 500) / 10 */

			/*47K阻值对应电压关系：Y=47*X/(3.3-X) */
			if(tempvolt >= 3300)
			{
				tempvolt = 0xefffffff;
			}
			else
			{
				tempvolt = (tempvolt * 47000) / (3300 - tempvolt);
			}

			/*根据阻值查表获取温度值 */
			tempvolt = SearchPwrTempTabByR(tempvolt) - 41;
			break;

        /*K1/2高边驱动电流采样*/
        case eBMU_ADC_HS0C:
        case eBMU_ADC_HS1C:
        /*K3/4高边驱动电流采样*/
        case eBMU_ADC_HS2C:
        case eBMU_ADC_HS3C:
        /*K5/6高边驱动电流采样*/
        case eBMU_ADC_HS4C:
        case eBMU_ADC_HS5C:
        /*K7/8高边驱动电流采样*/
        case eBMU_ADC_HS6C:
        case eBMU_ADC_HS7C:
            //A=V=AD*3.3V/4096(0~3.3V/3A,0.01A)
            tempvolt = (u16)((u32)ADdata * 3300 / 4096);
            tempvolt = tempvolt * 55 / 560;/*对应公式:Y=550/560*X*/
            break;
            
        default:
            tempvolt = 0xffff;
            break;
    }
    
    return(tempvolt);
}

/*=============================================================================================
** @Name      : static u16 CalcHalCSampFiltAD(u16 ADdata)
** @Input     : ADdata:滤波前霍尔电流采样AD值
** @Output    : 滤波后霍尔电流采样AD值
** @Function  : 计算霍尔电流采样AD值滤波值
** @The notes : 采用窗口滤波
**===========================================================================================*/
static u16 CalcHalCSampFiltAD(u16 ADdata)
{
	u8 i = 0;
	u32 sumAD = 0;
	static u8 sTime = 0;
	static u8 sIndex = 0;
	#if(LV_CAD_FILT_TIME > 3)
	u16 maxAD = 0;
	u16 minAD = 0xffff;
	#endif

	/*不滤波*/
	#if((0 == LV_CAD_FILT_TIME) || (1 == LV_CAD_FILT_TIME))
	{
		return(ADdata);
	}
	/*滤波次数少*/
	#elif((2 == LV_CAD_FILT_TIME) || (3 == LV_CAD_FILT_TIME))
	{
		//循环记录AD值到滤波数组
		gADCCurrAD[sIndex] = ADdata;
		sIndex++;
		if(sIndex >= LV_CAD_FILT_TIME)
		{
			sIndex = 0;
		}
		if(sTime < LV_CAD_FILT_TIME)
		{
			sTime++;
		}

		//根据当前数组有效长度计算平均值
		for(i = 0; i < sTime; i++)
		{
			sumAD += gADCCurrAD[i];
		}

		sumAD = sumAD / sTime;

		return((u16)sumAD);
	}
	#else
	{
		//循环记录AD值到滤波数组
		gADCCurrAD[sIndex] = ADdata;
		sIndex++;
		if(sIndex >= LV_CAD_FILT_TIME)
		{
			sIndex = 0;
		}
		if(sTime < LV_CAD_FILT_TIME)
		{
			sTime++;
		}

		//根据当前数组有效长度计算平均值
		for(i = 0; i < sTime; i++)
		{
			sumAD += gADCCurrAD[i];
			if(gADCCurrAD[i] > maxAD)
			{
				maxAD = gADCCurrAD[i];
			}
			if(gADCCurrAD[i] < minAD)
			{
				minAD = gADCCurrAD[i];
			}
		}

		//多于4次去掉最大最小值计算平均值
		if(sTime >= 4)
		{
			sumAD = (sumAD - maxAD - minAD) / (sTime - 2);
		}
		else
		{
			sumAD = sumAD / sTime;
		}

		return((u16)sumAD);
	}
	#endif
}

/*=============================================================================================
** @Name      : static u8 SearchEnvTempTabByR(u32 data)
** @Input     : data:要查表的数据阻值 1Ω
** @Output    : 应温度下标值
** @Function  : 通过NTC阻值查询环境温度表得到对应温度值
** @The notes :
**===========================================================================================*/
static u8 SearchEnvTempTabByR(u32 data)
{
    u8 i = 0;

	/*断线阻值很大(-41℃)*/
    if(data >= sNTCEnvTTabR[0])
    {
    	return(0);
    }
	/*非断线(-40℃以上)*/
    else
    {
    	/*从-40℃开始检测*/
		for(i = 1; i < ENV_T_TAB_LEN; i++)
		{
			/*达到当前阻值(阻值表降序)*/
			if(data >= sNTCEnvTTabR[i])
			{
				/*判断与前一个值的中间值*/
				if(data <= ((sNTCEnvTTabR[i] + sNTCEnvTTabR[i - 1]) / 2))
				{
					/*没有超过中间值取当前值*/
					return(i);
				}
				/*超过中间值说明偏大取前一个更大阻值*/
				else
				{
					return(i - 1);
				}
			}
		}
    }

	/*阻值很小低于表中阻值*/
    return(ENV_T_TAB_LEN);
}

/*=============================================================================================
** @Name      : static u8 SearchPwrTempTabByR(u32 data)
** @Input     : data:要查表的数据阻值 1Ω
** @Output    : 应温度下标值
** @Function  : 通过NTC阻值查询低压电源温度表得到对应板温温度值
** @The notes :
**===========================================================================================*/
static u8 SearchPwrTempTabByR(u32 data)
{
    u8 i = 0;

	/*断线阻值很大(-41℃)*/
    if(data >= sNTCPwrTTabR[0])
    {
    	return(0);
    }
	/*非断线(-40℃以上)*/
    else
    {
    	/*从-40℃开始检测*/
		for(i = 1; i < PWR_T_TAB_LEN; i++)
		{
			/*达到当前阻值(阻值表降序)*/
			if(data >= sNTCPwrTTabR[i])
			{
				/*判断与前一个值的中间值*/
				if(data <= ((sNTCPwrTTabR[i] + sNTCPwrTTabR[i - 1]) / 2))
				{
					/*没有超过中间值取当前值*/
					return(i);
				}
				/*超过中间值说明偏大取前一个更大阻值*/
				else
				{
					return(i - 1);
				}
			}
		}
    }

	/*阻值很小低于表中阻值*/
    return(PWR_T_TAB_LEN);
}
