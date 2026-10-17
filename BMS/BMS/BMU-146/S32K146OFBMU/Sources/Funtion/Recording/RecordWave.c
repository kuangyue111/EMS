/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : RecordWave.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 故障录波记录模块处理
** @Instructions : 电压、电流和温度故障录波记录
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"
#include "RecordUser.h"
#include "RecordInterface.h"
#include "RecordWave.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
#if(RCD_CV_WAV_CYCLE > 0)
static t_RcdFatWavBuff sWaveCVHBuff = {0};                  /*单体过压故障录波记录缓存区信息(需要初始化确定缓存区指针)*/
static t_RcdFatWavBuff sWaveCVLBuff = {0};                  /*单体欠压故障录波记录缓存区信息(需要初始化确定缓存区指针)*/
#endif
#if(RCD_CT_WAV_CYCLE > 0)
static t_RcdFatWavBuff sWaveCTHBuff = {0};                  /*单体过温故障录波记录缓存区信息(需要初始化确定缓存区指针)*/
static t_RcdFatWavBuff sWaveCTLBuff = {0};                  /*单体低温故障录波记录缓存区信息(需要初始化确定缓存区指针)*/
#endif
#if(RCD_HV_WAV_CYCLE > 0)
static t_RcdFatWavBuff sWaveHVBuff = {0};                   /*总压高低压故障录波记录缓存区信息(需要初始化确定缓存区指针)*/
#endif
#if(RCD_CO_WAV_CYCLE > 0)
static t_RcdFatWavBuff sWaveCOBuff = {0};                   /*充放电过流故障录波记录缓存区信息(需要初始化确定缓存区指针)*/
#endif

extern t_RcdPara gRcdPara;                                  /*存储模块参数*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void RcdFaultWaveBuffInit(void)
** @Input     : void
** @Output    : void
** @Function  : 故障录波缓存区初始化
** @The notes : 模块参数后/用户读写记录前调用
**===============================================================================================*/
void RcdFaultWaveBuffInit(void)
{
	/*故障录波初始化*/
	if(1 == gRcdPara.wavRcdEn)
	{
		/*初始化录波缓存区指针*/
		#if(RCD_CV_WAV_CYCLE > 0)
		sWaveCVHBuff.pWavBuf = RcdGetFatWavStoreCVHBuffPtrHook(&sWaveCVHBuff.pWavNum);
		sWaveCVLBuff.pWavBuf = RcdGetFatWavStoreCVLBuffPtrHook(&sWaveCVLBuff.pWavNum);
		#endif
		#if(RCD_CT_WAV_CYCLE > 0)
		sWaveCTHBuff.pWavBuf = RcdGetFatWavStoreCTHBuffPtrHook(&sWaveCTHBuff.pWavNum);
		sWaveCTLBuff.pWavBuf = RcdGetFatWavStoreCTLBuffPtrHook(&sWaveCTLBuff.pWavNum);
		#endif
		#if(RCD_HV_WAV_CYCLE > 0)
		sWaveHVBuff.pWavBuf = RcdGetFatWavStoreHVBuffPtrHook(&sWaveHVBuff.pWavNum);
		#endif
		#if(RCD_CO_WAV_CYCLE > 0)
		sWaveCOBuff.pWavBuf = RcdGetFatWavStoreCOBuffPtrHook(&sWaveCOBuff.pWavNum);
		#endif
	}
}

/*=================================================================================================
** @Name      : void RcdFaultWaveHandleTask(void)
** @Input     : void
** @Output    : void
** @Function  : 故障录波处理任务
** @The notes : 100ms周期
**===============================================================================================*/
void RcdFaultWaveHandleTask(void)
{
	u16 i = 0;
	#if(RCD_CV_WAV_CYCLE > 0)
	static u16 sCVHTime = 0;
	static u16 sCVHIndex = 0;
	static u16 sCVLTime = 0;
	static u16 sCVLIndex = 0;
	#endif
	#if(RCD_CT_WAV_CYCLE > 0)
	static u16 sCTHTime = 0;
	static u16 sCTHIndex = 0;
	static u16 sCTLTime = 0;
	static u16 sCTLIndex = 0;
	#endif
	#if(RCD_HV_WAV_CYCLE > 0)
	static u16 sHVTime = 0;
	static u16 sHVIndex = 0;
	#endif
	#if(RCD_CO_WAV_CYCLE > 0)
	static u16 sCOTime = 0;
	static u16 sCOIndex = 0;
	#endif

	/*未使能录波数据*/
	if(0 == gRcdPara.wavRcdEn)
	{
		return;
	}

	/*用户请求暂停缓存录波数据*/
	if(1 == RcdGetFatWavStoreStopReqHook())
	{
		return;
	}

	/*单体电压录波有效*/
	#if(RCD_CV_WAV_CYCLE > 0)
	/*单体过压录波*/
	if(sWaveCVHBuff.pWavNum > 0)
	{
		/*达到滤波周期或单体电压变化5mv(或刚刚触发故障录波)*/
		if((sCVHTime >= RCD_CV_WAV_CYCLE) || (eFatWavState_Trig == sWaveCVHBuff.pWavFlg)
			|| ((sCVHIndex > 0) && (ABS(sWaveCVHBuff.pWavBuf[sCVHIndex - 1], RcdGetGroupMaxVoltHook()) >= 5))
			|| ((0 == sCVHIndex) && (ABS(sWaveCVHBuff.pWavBuf[sWaveCVHBuff.pWavNum - 1], RcdGetGroupMaxVoltHook()) >= 5)))
		{
			/*开始触发故障录波*/
			if(eFatWavState_Trig == sWaveCVHBuff.pWavFlg)
			{
				/*将数据最新一半数据重头录入(刚好录一半位置不用移动)*/
				/*已录超过一半*/
				if(sCVHIndex > (sWaveCVHBuff.pWavNum / 2))
				{
					/*将数据最新一半往前移动*/
					for(i = 0; i < (sWaveCVHBuff.pWavNum / 2); i++)
					{
						sWaveCVHBuff.pWavBuf[i] = sWaveCVHBuff.pWavBuf[sCVHIndex - (sWaveCVHBuff.pWavNum / 2) + i];
					}
				}
				/*未录满一半*/
				else if(sCVHIndex < (sWaveCVHBuff.pWavNum / 2))
				{
					/*将数据后移到中间位置*/
					for(i = (sWaveCVHBuff.pWavNum / 2); i > 0; i--)
					{
						if(sCVHIndex > 0)
						{
							sCVHIndex--;
						}
						else
						{
							sCVHIndex = sWaveCVHBuff.pWavNum - 1;
						}
						sWaveCVHBuff.pWavBuf[i - 1] = sWaveCVHBuff.pWavBuf[sCVHIndex];
					}
				}

				/*索引移到一半位置*/
				sCVHIndex = sWaveCVHBuff.pWavNum / 2;

				/*转为正在故障录波*/
				sWaveCVHBuff.pWavFlg = eFatWavState_Doig;
			}

			/*非故障录波完成*/
			if(eFatWavState_Fin != sWaveCVHBuff.pWavFlg)
			{
				/*记录一个单体电压*/
				sWaveCVHBuff.pWavBuf[sCVHIndex] = RcdGetGroupMaxVoltHook();
				sCVHIndex++;
				if(sCVHIndex >= sWaveCVHBuff.pWavNum)
				{
					/*正在故障录波*/
					if(eFatWavState_Doig == sWaveCVHBuff.pWavFlg)
					{
						/*故障录波完成*/
						sWaveCVHBuff.pWavFlg = eFatWavState_Fin;
					}
					sCVHIndex = 0;
				}
			}

			sCVHTime = 0;
		}
		else
		{
			/*计算周期*/
			sCVHTime++;
		}
	}

	/*单体欠压录波*/
	if(sWaveCVLBuff.pWavNum > 0)
	{
		/*达到滤波周期或单体电压变化5mv(或刚刚触发故障录波)*/
		if((sCVLTime >= RCD_CV_WAV_CYCLE) || (eFatWavState_Trig == sWaveCVLBuff.pWavFlg)
			|| ((sCVLIndex > 0) && (ABS(sWaveCVLBuff.pWavBuf[sCVLIndex - 1], RcdGetGroupMinVoltHook()) >= 5))
			|| ((0 == sCVLIndex) && (ABS(sWaveCVLBuff.pWavBuf[sWaveCVLBuff.pWavNum - 1], RcdGetGroupMinVoltHook()) >= 5)))
		{
			/*开始触发故障录波*/
			if(eFatWavState_Trig == sWaveCVLBuff.pWavFlg)
			{
				/*将数据最新一半数据重头录入(刚好录一半位置不用移动)*/
				/*已录超过一半*/
				if(sCVLIndex > (sWaveCVLBuff.pWavNum / 2))
				{
					/*将数据最新一半往前移动*/
					for(i = 0; i < (sWaveCVLBuff.pWavNum / 2); i++)
					{
						sWaveCVLBuff.pWavBuf[i] = sWaveCVLBuff.pWavBuf[sCVLIndex - (sWaveCVLBuff.pWavNum / 2) + i];
					}
				}
				/*未录满一半*/
				else if(sCVLIndex < (sWaveCVLBuff.pWavNum / 2))
				{
					/*将数据后移到中间位置*/
					for(i = (sWaveCVLBuff.pWavNum / 2); i > 0; i--)
					{
						if(sCVLIndex > 0)
						{
							sCVLIndex--;
						}
						else
						{
							sCVLIndex = sWaveCVLBuff.pWavNum - 1;
						}
						sWaveCVLBuff.pWavBuf[i - 1] = sWaveCVLBuff.pWavBuf[sCVLIndex];
					}
				}

				/*索引移到一半位置*/
				sCVLIndex = sWaveCVLBuff.pWavNum / 2;

				/*转为正在故障录波*/
				sWaveCVLBuff.pWavFlg = eFatWavState_Doig;
			}

			/*非故障录波完成*/
			if(eFatWavState_Fin != sWaveCVLBuff.pWavFlg)
			{
				/*记录一个单体电压*/
				sWaveCVLBuff.pWavBuf[sCVLIndex] = RcdGetGroupMinVoltHook();
				sCVLIndex++;
				if(sCVLIndex >= sWaveCVLBuff.pWavNum)
				{
					/*正在故障录波*/
					if(eFatWavState_Doig == sWaveCVLBuff.pWavFlg)
					{
						/*故障录波完成*/
						sWaveCVLBuff.pWavFlg = eFatWavState_Fin;
					}
					sCVLIndex = 0;
				}
			}

			sCVLTime = 0;
		}
		else
		{
			/*计算周期*/
			sCVLTime++;
		}
	}
	#endif

	/*单体温度录波有效*/
	#if(RCD_CT_WAV_CYCLE > 0)
	/*单体过温录波*/
	if(sWaveCTHBuff.pWavNum > 0)
	{
		/*达到滤波周期或单体温度变化2℃(或刚刚触发故障录波)*/
		if((sCTHTime >= RCD_CV_WAV_CYCLE) || (eFatWavState_Trig == sWaveCTHBuff.pWavFlg)
			|| ((sCTHIndex > 0) && (ABS((s16)sWaveCTHBuff.pWavBuf[sCTHIndex - 1], RcdGetGroupMaxTempHook()) >= 2))
			|| ((0 == sCTHIndex) && (ABS((s16)sWaveCTHBuff.pWavBuf[sWaveCTHBuff.pWavNum - 1], RcdGetGroupMaxTempHook()) >= 2)))
		{
			/*开始触发故障录波*/
			if(eFatWavState_Trig == sWaveCTHBuff.pWavFlg)
			{
				/*将数据最新一半数据重头录入(刚好录一半位置不用移动)*/
				/*已录超过一半*/
				if(sCTHIndex > (sWaveCTHBuff.pWavNum / 2))
				{
					/*将数据最新一半往前移动*/
					for(i = 0; i < (sWaveCTHBuff.pWavNum / 2); i++)
					{
						sWaveCTHBuff.pWavBuf[i] = sWaveCTHBuff.pWavBuf[sCTHIndex - (sWaveCTHBuff.pWavNum / 2) + i];
					}
				}
				/*未录满一半*/
				else if(sCTHIndex < (sWaveCTHBuff.pWavNum / 2))
				{
					/*将数据后移到中间位置*/
					for(i = (sWaveCTHBuff.pWavNum / 2); i > 0; i--)
					{
						if(sCTHIndex > 0)
						{
							sCTHIndex--;
						}
						else
						{
							sCTHIndex = sWaveCTHBuff.pWavNum - 1;
						}
						sWaveCTHBuff.pWavBuf[i - 1] = sWaveCTHBuff.pWavBuf[sCTHIndex];
					}
				}

				/*索引移到一半位置*/
				sCTHIndex = sWaveCTHBuff.pWavNum / 2;

				/*转为正在故障录波*/
				sWaveCTHBuff.pWavFlg = eFatWavState_Doig;
			}

			/*非故障录波完成*/
			if(eFatWavState_Fin != sWaveCTHBuff.pWavFlg)
			{
				/*记录一个单体温度*/
				sWaveCTHBuff.pWavBuf[sCTHIndex] = (u16)RcdGetGroupMaxTempHook();
				sCTHIndex++;
				if(sCTHIndex >= sWaveCTHBuff.pWavNum)
				{
					/*正在故障录波*/
					if(eFatWavState_Doig == sWaveCTHBuff.pWavFlg)
					{
						/*故障录波完成*/
						sWaveCTHBuff.pWavFlg = eFatWavState_Fin;
					}
					sCTHIndex = 0;
				}
			}

			sCTHTime = 0;
		}
		else
		{
			/*计算周期*/
			sCTHTime++;
		}
	}

	/*单体低温录波*/
	if(sWaveCTLBuff.pWavNum > 0)
	{
		/*达到滤波周期或单体温度变化2℃(或刚刚触发故障录波)*/
		if((sCTLTime >= RCD_CV_WAV_CYCLE) || (eFatWavState_Trig == sWaveCTLBuff.pWavFlg)
			|| ((sCTLIndex > 0) && (ABS((s16)sWaveCTLBuff.pWavBuf[sCTLIndex - 1], RcdGetGroupMinTempHook()) >= 2))
			|| ((0 == sCTLIndex) && (ABS((s16)sWaveCTLBuff.pWavBuf[sWaveCTLBuff.pWavNum - 1], RcdGetGroupMinTempHook()) >= 2)))
		{
			/*开始触发故障录波*/
			if(eFatWavState_Trig == sWaveCTLBuff.pWavFlg)
			{
				/*将数据最新一半数据重头录入(刚好录一半位置不用移动)*/
				/*已录超过一半*/
				if(sCTLIndex > (sWaveCTLBuff.pWavNum / 2))
				{
					/*将数据最新一半往前移动*/
					for(i = 0; i < (sWaveCTLBuff.pWavNum / 2); i++)
					{
						sWaveCTLBuff.pWavBuf[i] = sWaveCTLBuff.pWavBuf[sCTLIndex - (sWaveCTLBuff.pWavNum / 2) + i];
					}
				}
				/*未录满一半*/
				else if(sCTLIndex < (sWaveCTLBuff.pWavNum / 2))
				{
					/*将数据后移到中间位置*/
					for(i = (sWaveCTLBuff.pWavNum / 2); i > 0; i--)
					{
						if(sCTLIndex > 0)
						{
							sCTLIndex--;
						}
						else
						{
							sCTLIndex = sWaveCTLBuff.pWavNum - 1;
						}
						sWaveCTLBuff.pWavBuf[i - 1] = sWaveCTLBuff.pWavBuf[sCTLIndex];
					}
				}

				/*索引移到一半位置*/
				sCTLIndex = sWaveCTLBuff.pWavNum / 2;

				/*转为正在故障录波*/
				sWaveCTLBuff.pWavFlg = eFatWavState_Doig;
			}

			/*非故障录波完成*/
			if(eFatWavState_Fin != sWaveCTLBuff.pWavFlg)
			{
				/*记录一个单体温度*/
				sWaveCTLBuff.pWavBuf[sCTLIndex] = (u16)RcdGetGroupMinTempHook();
				sCTLIndex++;
				if(sCTLIndex >= sWaveCTLBuff.pWavNum)
				{
					/*正在故障录波*/
					if(eFatWavState_Doig == sWaveCTLBuff.pWavFlg)
					{
						/*故障录波完成*/
						sWaveCTLBuff.pWavFlg = eFatWavState_Fin;
					}
					sCTLIndex = 0;
				}
			}

			sCTLTime = 0;
		}
		else
		{
			/*计算周期*/
			sCTLTime++;
		}
	}
	#endif

	/*总电压录波有效*/
	#if(RCD_HV_WAV_CYCLE > 0)
	/*总压过欠压录波*/
	if(sWaveHVBuff.pWavNum > 0)
	{
		/*达到滤波周期或总压电压变化2.0v(或刚刚触发故障录波)*/
		if((sHVTime >= RCD_HV_WAV_CYCLE) || (eFatWavState_Trig == sWaveHVBuff.pWavFlg)
			|| ((sHVIndex > 0) && (ABS(sWaveHVBuff.pWavBuf[sHVIndex - 1], RcdGetGroupSumVoltHook()) >= 20))
			|| ((0 == sHVIndex) && (ABS(sWaveHVBuff.pWavBuf[sWaveHVBuff.pWavNum - 1], RcdGetGroupSumVoltHook()) >= 20)))
		{
			/*开始触发故障录波*/
			if(eFatWavState_Trig == sWaveHVBuff.pWavFlg)
			{
				/*将数据最新一半数据重头录入(刚好录一半位置不用移动)*/
				/*已录超过一半*/
				if(sHVIndex > (sWaveHVBuff.pWavNum / 2))
				{
					/*将数据最新一半往前移动*/
					for(i = 0; i < (sWaveHVBuff.pWavNum / 2); i++)
					{
						sWaveHVBuff.pWavBuf[i] = sWaveHVBuff.pWavBuf[sHVIndex - (sWaveHVBuff.pWavNum / 2) + i];
					}
				}
				/*未录满一半*/
				else if(sHVIndex < (sWaveHVBuff.pWavNum / 2))
				{
					/*将数据后移到中间位置*/
					for(i = (sWaveHVBuff.pWavNum / 2); i > 0; i--)
					{
						if(sHVIndex > 0)
						{
							sHVIndex--;
						}
						else
						{
							sHVIndex = sWaveHVBuff.pWavNum - 1;
						}
						sWaveHVBuff.pWavBuf[i - 1] = sWaveHVBuff.pWavBuf[sHVIndex];
					}
				}

				/*索引移到一半位置*/
				sHVIndex = sWaveHVBuff.pWavNum / 2;

				/*转为正在故障录波*/
				sWaveHVBuff.pWavFlg = eFatWavState_Doig;
			}

			/*非故障录波完成*/
			if(eFatWavState_Fin != sWaveHVBuff.pWavFlg)
			{
				/*记录一个总电压*/
				sWaveHVBuff.pWavBuf[sHVIndex] = RcdGetGroupSumVoltHook();
				sHVIndex++;
				if(sHVIndex >= sWaveHVBuff.pWavNum)
				{
					/*正在故障录波*/
					if(eFatWavState_Doig == sWaveHVBuff.pWavFlg)
					{
						/*故障录波完成*/
						sWaveHVBuff.pWavFlg = eFatWavState_Fin;
					}
					sHVIndex = 0;
				}
			}

			sHVTime = 0;
		}
		else
		{
			/*计算周期*/
			sHVTime++;
		}
	}
	#endif

	/*输出电流录波有效*/
	#if(RCD_CO_WAV_CYCLE > 0)
	/*充放电过流录波*/
	if(sWaveCOBuff.pWavNum > 0)
	{
		/*达到滤波周期或输出电流极大或变化3.0A(或刚刚触发故障录波)*/
		if((sCOTime >= RCD_CO_WAV_CYCLE) || (eFatWavState_Trig == sWaveCOBuff.pWavFlg)
			|| (RcdGetGroupOutCurrHook() >= (s32)BATT_EXEREM_MAXC)
			|| (RcdGetGroupOutCurrHook() <= (0 - (s32)BATT_EXEREM_MAXC))
			|| ((sCOIndex > 0) && (ABS((s16)sWaveCOBuff.pWavBuf[sCOIndex - 1], RcdGetGroupOutCurrHook()) >= 30))
			|| ((0 == sCOIndex) && (ABS((s16)sWaveCOBuff.pWavBuf[sWaveCOBuff.pWavNum - 1], RcdGetGroupOutCurrHook()) >= 30)))
		{
			/*开始触发故障录波*/
			if(eFatWavState_Trig == sWaveCOBuff.pWavFlg)
			{
				/*将数据最新一半数据重头录入(刚好录一半位置不用移动)*/
				/*已录超过一半*/
				if(sCOIndex > (sWaveCOBuff.pWavNum / 2))
				{
					/*将数据最新一半往前移动*/
					for(i = 0; i < (sWaveCOBuff.pWavNum / 2); i++)
					{
						sWaveCOBuff.pWavBuf[i] = sWaveCOBuff.pWavBuf[sCOIndex - (sWaveCOBuff.pWavNum / 2) + i];
					}
				}
				/*未录满一半*/
				else if(sCOIndex < (sWaveCOBuff.pWavNum / 2))
				{
					/*将数据后移到中间位置*/
					for(i = (sWaveCOBuff.pWavNum / 2); i > 0; i--)
					{
						if(sCOIndex > 0)
						{
							sCOIndex--;
						}
						else
						{
							sCOIndex = sWaveCOBuff.pWavNum - 1;
						}
						sWaveCOBuff.pWavBuf[i - 1] = sWaveCOBuff.pWavBuf[sCOIndex];
					}
				}

				/*索引移到一半位置*/
				sCOIndex = sWaveCOBuff.pWavNum / 2;

				/*转为正在故障录波*/
				sWaveCOBuff.pWavFlg = eFatWavState_Doig;
			}

			/*非故障录波完成*/
			if(eFatWavState_Fin != sWaveCOBuff.pWavFlg)
			{
				/*记录一个输出电流*/
				sWaveCOBuff.pWavBuf[sCOIndex] = (u16)RcdGetGroupOutCurrHook();

				sCOIndex++;
				if(sCOIndex >= sWaveCOBuff.pWavNum)
				{
					/*正在故障录波*/
					if(eFatWavState_Doig == sWaveCOBuff.pWavFlg)
					{
						/*故障录波完成*/
						sWaveCOBuff.pWavFlg = eFatWavState_Fin;
					}
					sCOIndex = 0;
				}
			}

			sCOTime = 0;
		}
		else
		{
			/*计算周期*/
			sCOTime++;
		}
	}
	#endif
}

/*=================================================================================================
** @Name      : void RcdSetFaultWaveTrigAPI(e_RcdFatWavSort sort)
** @Input     : sort:录波类型[0:无效记录 1:单体二级过压,2:单体一级过压,3:单体二级欠压,4:单体一级欠压;5:总压二级过压,6:总压一级过压,7:总压二级欠压,8:总压一级欠压;9:充电二级过温;10:充电一级过温;11:充电二级低温;12:充电一级低温;13:放电二级过温;14:放电一级过温;15:放电二级低温;16:放电一级低温;17:充电二级过流;18:充电一级过流;19:放电二级过流;20:放电一级过流;21:充电超限值;22:放电超限值]
** @Output    : void
** @Function  : 设置触发开始录波
** @The notes : 产生故障需要录波时调用
**===============================================================================================*/
void RcdSetFaultWaveTrigAPI(e_RcdFatWavSort sort)
{
	t_SysClock sysClock;

	/*未使能录波数据*/
	if(0 == gRcdPara.wavRcdEn)
	{
		return;
	}

	/*获取当前系统时间*/
	sysClock = GetSysDateClockAPI();

	/*单体电压录波有效*/
	#if(RCD_CV_WAV_CYCLE > 0)
	/*单体过压录波*/
	if(sort <= eFatWavSort_CVH1)
	{
		/*使能录波且未触发录波*/
		if((sWaveCVHBuff.pWavNum > 0) && (eFatWavState_Wait == sWaveCVHBuff.pWavFlg))
		{
			/*记录故障录波时间信息*/
			sWaveCVHBuff.pWavYear = sysClock.year;
			sWaveCVHBuff.pWavMD = ((u16)sysClock.month << 8) | sysClock.day;
			sWaveCVHBuff.pWavHM = ((u16)sysClock.hour << 8) | sysClock.minute;
			sWaveCVHBuff.pWavSec = sysClock.second;

			/*记录故障录波类型*/
			sWaveCVHBuff.pWavSort = sort;

			/*设置触发录波*/
			sWaveCVHBuff.pWavFlg = eFatWavState_Trig;
		}
	}
	/*单体欠压录波*/
	else if(sort <= eFatWavSort_CVL1)
	{
		/*使能录波且未触发录波*/
		if((sWaveCVLBuff.pWavNum > 0) && (eFatWavState_Wait == sWaveCVLBuff.pWavFlg))
		{
			/*记录故障录波时间信息*/
			sWaveCVLBuff.pWavYear = sysClock.year;
			sWaveCVLBuff.pWavMD = ((u16)sysClock.month << 8) | sysClock.day;
			sWaveCVLBuff.pWavHM = ((u16)sysClock.hour << 8) | sysClock.minute;
			sWaveCVLBuff.pWavSec = sysClock.second;

			/*记录故障录波类型*/
			sWaveCVLBuff.pWavSort = sort;

			/*设置触发录波*/
			sWaveCVLBuff.pWavFlg = eFatWavState_Trig;
		}
	} else
	#endif
	/*总电压录波有效*/
	#if(RCD_HV_WAV_CYCLE > 0)
	/*总压过欠压录波*/
	if((sort >= eFatWavSort_HVH2) && (sort <= eFatWavSort_HVL1))
	{
		/*使能录波且未触发录波*/
		if((sWaveHVBuff.pWavNum > 0) && (eFatWavState_Wait == sWaveHVBuff.pWavFlg))
		{
			/*记录故障录波时间信息*/
			sWaveHVBuff.pWavYear = sysClock.year;
			sWaveHVBuff.pWavMD = ((u16)sysClock.month << 8) | sysClock.day;
			sWaveHVBuff.pWavHM = ((u16)sysClock.hour << 8) | sysClock.minute;
			sWaveHVBuff.pWavSec = sysClock.second;

			/*记录故障录波类型*/
			sWaveHVBuff.pWavSort = sort;

			/*设置触发录波*/
			sWaveHVBuff.pWavFlg = eFatWavState_Trig;
		}
	} else
	#endif
	/*单体温度录波有效*/
	#if(RCD_CT_WAV_CYCLE > 0)
	/*充放电过温录波*/
	if(((sort >= eFatWavSort_CTH2) && (sort <= eFatWavSort_CTH1))
		|| ((sort >= eFatWavSort_DTH2) && (sort <= eFatWavSort_DTH1)))
	{
		/*使能录波且未触发录波*/
		if((sWaveCTHBuff.pWavNum > 0) && (eFatWavState_Wait == sWaveCTHBuff.pWavFlg))
		{
			/*记录故障录波时间信息*/
			sWaveCTHBuff.pWavYear = sysClock.year;
			sWaveCTHBuff.pWavMD = ((u16)sysClock.month << 8) | sysClock.day;
			sWaveCTHBuff.pWavHM = ((u16)sysClock.hour << 8) | sysClock.minute;
			sWaveCTHBuff.pWavSec = sysClock.second;

			/*记录故障录波类型*/
			sWaveCTHBuff.pWavSort = sort;

			/*设置触发录波*/
			sWaveCTHBuff.pWavFlg = eFatWavState_Trig;
		}
	}
	/*充放电低温录波*/
	else if(((sort >= eFatWavSort_CTL2) && (sort <= eFatWavSort_CTL1))
		|| ((sort >= eFatWavSort_DTL2) && (sort <= eFatWavSort_DTL1)))
	{
		/*使能录波且未触发录波*/
		if((sWaveCTLBuff.pWavNum > 0) && (eFatWavState_Wait == sWaveCTLBuff.pWavFlg))
		{
			/*记录故障录波时间信息*/
			sWaveCTLBuff.pWavYear = sysClock.year;
			sWaveCTLBuff.pWavMD = ((u16)sysClock.month << 8) | sysClock.day;
			sWaveCTLBuff.pWavHM = ((u16)sysClock.hour << 8) | sysClock.minute;
			sWaveCTLBuff.pWavSec = sysClock.second;

			/*记录故障录波类型*/
			sWaveCTLBuff.pWavSort = sort;

			/*设置触发录波*/
			sWaveCTLBuff.pWavFlg = eFatWavState_Trig;
		}
	} else
	#endif
	/*充放电电流录波有效*/
	#if(RCD_CO_WAV_CYCLE > 0)
	/*充放电过流录波*/
	if((sort >= eFatWavSort_CCO2) && (sort <= eFatWavSort_DCOL))
	{
		/*使能录波且未触发录波*/
		if((sWaveCOBuff.pWavNum > 0) && (eFatWavState_Wait == sWaveCOBuff.pWavFlg))
		{
			/*记录故障录波时间信息*/
			sWaveCOBuff.pWavYear = sysClock.year;
			sWaveCOBuff.pWavMD = ((u16)sysClock.month << 8) | sysClock.day;
			sWaveCOBuff.pWavHM = ((u16)sysClock.hour << 8) | sysClock.minute;
			sWaveCOBuff.pWavSec = sysClock.second;

			/*记录故障录波类型*/
			sWaveCOBuff.pWavSort = sort;

			/*设置触发录波*/
			sWaveCOBuff.pWavFlg = eFatWavState_Trig;
		}
	}
	#endif
}

/*=================================================================================================
** @Name      : e_RcdFatWavState RcdGetFaultWaveStateAPI(void)
** @Input     : void
** @Output    : 当前故障录波状态:0:等待录波 1:触发录波 2:正在录波 3:录波完成
** @Function  : 获取当前故障录波状态
** @The notes : 判断当前是否需要故障录波存储
**===============================================================================================*/
e_RcdFatWavState RcdGetFaultWaveStateAPI(void)
{
	e_RcdFatWavState state = eFatWavState_Wait;

	/*未使能录波数据*/
	if(0 == gRcdPara.wavRcdEn)
	{
		return(state);
	}

	/*单体电压录波有效*/
	#if(RCD_CV_WAV_CYCLE > 0)
	/*已触发单体过压录波*/
	if(sWaveCVHBuff.pWavFlg >= eFatWavState_Trig)
	{
		/*更高的状态*/
		if(sWaveCVHBuff.pWavFlg > state)
		{
			/*记录更高的状态*/
			state = sWaveCVHBuff.pWavFlg;
		}
	}
	/*已触发单体欠压录波*/
	if(sWaveCVLBuff.pWavFlg >= eFatWavState_Trig)
	{
		/*更高的状态*/
		if(sWaveCVLBuff.pWavFlg > state)
		{
			/*记录更高的状态*/
			state = sWaveCVLBuff.pWavFlg;
		}
	}
	#endif
	/*总电压录波有效*/
	#if(RCD_HV_WAV_CYCLE > 0)
	/*已触发总压过欠压录波*/
	if(sWaveHVBuff.pWavFlg >= eFatWavState_Trig)
	{
		/*更高的状态*/
		if(sWaveHVBuff.pWavFlg > state)
		{
			/*记录更高的状态*/
			state = sWaveHVBuff.pWavFlg;
		}
	}
	#endif
	/*单体温度录波有效*/
	#if(RCD_CT_WAV_CYCLE > 0)
	/*已触发充放电过温录波*/
	if(sWaveCTHBuff.pWavFlg >= eFatWavState_Trig)
	{
		/*更高的状态*/
		if(sWaveCTHBuff.pWavFlg > state)
		{
			/*记录更高的状态*/
			state = sWaveCTHBuff.pWavFlg;
		}
	}
	/*已触发充放电低温录波*/
	if(sWaveCTLBuff.pWavFlg >= eFatWavState_Trig)
	{
		/*更高的状态*/
		if(sWaveCTLBuff.pWavFlg > state)
		{
			/*记录更高的状态*/
			state = sWaveCTLBuff.pWavFlg;
		}
	}
	#endif
	/*充放电电流录波有效*/
	#if(RCD_CO_WAV_CYCLE > 0)
	/*已触发充放电过流录波*/
	if(sWaveCOBuff.pWavFlg >= eFatWavState_Trig)
	{
		/*更高的状态*/
		if(sWaveCOBuff.pWavFlg > state)
		{
			/*记录更高的状态*/
			state = sWaveCOBuff.pWavFlg;
		}
	}
	#endif

	return(state);
}

/*=================================================================================================
** @Name      : u8 RcdGetFaultWaveInfoAPI(u16 *wavInfo, u8 len)
** @Input     : *wavInfo:获取到的故障录波信息 len:信息长度
** @Output    : TRUE:获取成功(信息有效) FALSE:获取失败(信息无效)
** @Function  : 获取当前故障录波信息
** @The notes : 当有故障录波完成时获取有效(获取后重新等待录波)
**===============================================================================================*/
u8 RcdGetFaultWaveInfoAPI(u16 *wavInfo, u8 len)
{
	u8 i = 0;
	static u16 rcdWavNum = 1;

	/*未使能录波数据*/
	if(0 == gRcdPara.wavRcdEn)
	{
		return(FALSE);
	}

	/*参数无效*/
	if((NULL == wavInfo) || (len < 7))
	{
		return(FALSE);
	}

	/*单体电压录波有效*/
	#if(RCD_CV_WAV_CYCLE > 0)
	/*单体过压录波完成*/
	if(eFatWavState_Fin == sWaveCVHBuff.pWavFlg)
	{
		/*录波记录数目累加*/
		rcdWavNum++;

		/*取得录波信息(必须按记录存储格式顺序赋值)*/
		wavInfo[0] = rcdWavNum;
		wavInfo[1] = sWaveCVHBuff.pWavSort;
		wavInfo[2] = sWaveCVHBuff.pWavYear;
		wavInfo[3] = sWaveCVHBuff.pWavMD;
		wavInfo[4] = sWaveCVHBuff.pWavHM;
		wavInfo[5] = sWaveCVHBuff.pWavSec;

		/*取得录波缓存区的值*/
		for(i = 0; (i < sWaveCVHBuff.pWavNum) && (i < (len - 6)); i++)
		{
			wavInfo[6+i]= sWaveCVHBuff.pWavBuf[i];
		}

		/*重新等待录波*/
		sWaveCVHBuff.pWavFlg = eFatWavState_Wait;

		return(TRUE);
	}

	/*单体欠压录波完成*/
	if(eFatWavState_Fin == sWaveCVLBuff.pWavFlg)
	{
		/*录波记录数目累加*/
		rcdWavNum++;

		/*取得录波信息(必须按记录存储格式顺序赋值)*/
		wavInfo[0] = rcdWavNum;
		wavInfo[1] = sWaveCVLBuff.pWavSort;
		wavInfo[2] = sWaveCVLBuff.pWavYear;
		wavInfo[3] = sWaveCVLBuff.pWavMD;
		wavInfo[4] = sWaveCVLBuff.pWavHM;
		wavInfo[5] = sWaveCVLBuff.pWavSec;

		/*取得录波缓存区的值*/
		for(i = 0; (i < sWaveCVLBuff.pWavNum) && (i < (len - 6)); i++)
		{
			wavInfo[6+i]= sWaveCVLBuff.pWavBuf[i];
		}

		/*重新等待录波*/
		sWaveCVLBuff.pWavFlg = eFatWavState_Wait;

		return(TRUE);
	}
	#endif

	/*总电压录波有效*/
	#if(RCD_HV_WAV_CYCLE > 0)
	/*已触发总压过欠压录波完成*/
	if(eFatWavState_Fin == sWaveHVBuff.pWavFlg)
	{
		/*录波记录数目累加*/
		rcdWavNum++;

		/*取得录波信息(必须按记录存储格式顺序赋值)*/
		wavInfo[0] = rcdWavNum;
		wavInfo[1] = sWaveHVBuff.pWavSort;
		wavInfo[2] = sWaveHVBuff.pWavYear;
		wavInfo[3] = sWaveHVBuff.pWavMD;
		wavInfo[4] = sWaveHVBuff.pWavHM;
		wavInfo[5] = sWaveHVBuff.pWavSec;

		/*取得录波缓存区的值*/
		for(i = 0; (i < sWaveHVBuff.pWavNum) && (i < (len - 6)); i++)
		{
			wavInfo[6+i]= sWaveHVBuff.pWavBuf[i];
		}

		/*重新等待录波*/
		sWaveHVBuff.pWavFlg = eFatWavState_Wait;

		return(TRUE);
	}
	#endif

	/*单体温度录波有效*/
	#if(RCD_CT_WAV_CYCLE > 0)
	/*充放电过温录波完成*/
	if(eFatWavState_Fin == sWaveCTHBuff.pWavFlg)
	{
		/*录波记录数目累加*/
		rcdWavNum++;

		/*取得录波信息(必须按记录存储格式顺序赋值)*/
		wavInfo[0] = rcdWavNum;
		wavInfo[1] = sWaveCTHBuff.pWavSort;
		wavInfo[2] = sWaveCTHBuff.pWavYear;
		wavInfo[3] = sWaveCTHBuff.pWavMD;
		wavInfo[4] = sWaveCTHBuff.pWavHM;
		wavInfo[5] = sWaveCTHBuff.pWavSec;

		/*取得录波缓存区的值*/
		for(i = 0; (i < sWaveCTHBuff.pWavNum) && (i < (len - 6)); i++)
		{
			wavInfo[6+i]= sWaveCTHBuff.pWavBuf[i];
		}

		/*重新等待录波*/
		sWaveCTHBuff.pWavFlg = eFatWavState_Wait;

		return(TRUE);
	}

	/*充放电低温录波完成*/
	if(eFatWavState_Fin == sWaveCTLBuff.pWavFlg)
	{
		/*录波记录数目累加*/
		rcdWavNum++;

		/*取得录波信息(必须按记录存储格式顺序赋值)*/
		wavInfo[0] = rcdWavNum;
		wavInfo[1] = sWaveCTLBuff.pWavSort;
		wavInfo[2] = sWaveCTLBuff.pWavYear;
		wavInfo[3] = sWaveCTLBuff.pWavMD;
		wavInfo[4] = sWaveCTLBuff.pWavHM;
		wavInfo[5] = sWaveCTLBuff.pWavSec;

		/*取得录波缓存区的值*/
		for(i = 0; (i < sWaveCTLBuff.pWavNum) && (i < (len - 6)); i++)
		{
			wavInfo[6+i]= sWaveCTLBuff.pWavBuf[i];
		}

		/*重新等待录波*/
		sWaveCTLBuff.pWavFlg = eFatWavState_Wait;

		return(TRUE);
	}
	#endif
	/*充放电电流录波有效*/
	#if(RCD_CO_WAV_CYCLE > 0)
	/*充放电过流录波完成*/
	if(eFatWavState_Fin == sWaveCOBuff.pWavFlg)
	{
		/*录波记录数目累加*/
		rcdWavNum++;

		/*取得录波信息(必须按记录存储格式顺序赋值)*/
		wavInfo[0] = rcdWavNum;
		wavInfo[1] = sWaveCOBuff.pWavSort;
		wavInfo[2] = sWaveCOBuff.pWavYear;
		wavInfo[3] = sWaveCOBuff.pWavMD;
		wavInfo[4] = sWaveCOBuff.pWavHM;
		wavInfo[5] = sWaveCOBuff.pWavSec;

		/*取得录波缓存区的值*/
		for(i = 0; (i < sWaveCOBuff.pWavNum) && (i < (len - 6)); i++)
		{
			wavInfo[6+i]= sWaveCOBuff.pWavBuf[i];
		}

		/*重新等待录波*/
		sWaveCOBuff.pWavFlg = eFatWavState_Wait;

		return(TRUE);
	}
	#endif

	return(FALSE);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
