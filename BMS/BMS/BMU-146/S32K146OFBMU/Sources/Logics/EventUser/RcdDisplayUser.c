/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : RcdDisplayUser.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池历史记录显示用户处理模块
** @Instructions : 读取历史记录显示
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "CANUser.h"
#include "Authority.h"
#include "RecordInterface.h"
#include "RcdDisplayUser.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sDataRcdDisplayDate = 0;                          /*当前显示运行数据记录日期*/

extern u16 gCANRcdNum_30[];                                 /*历史记录条数*/
extern u16 gCANSofInfo_31[];                                /*故障记录详细信息*/
extern u16 gCANSoeInfo_32[];                                /*事件记录详细信息*/
extern u16 gCANSowInfo_33[];                                /*故障录波详细信息*/
extern u16 gCANDodInfo_34[];                                /*DOD记录详细信息*/
extern u16 gCANSodNum_35[];                                 /*运行数据存储总状态信息*/
extern u16 gCANSodDay_36[];                                 /*运行数据存储日状态信息*/
extern u16 gCANSodInfo_37[];                                /*运行数据存储详细信息*/
extern u16 gCANSoxOper_39[];                                /*历史记录读写操作命令*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 ErrRcdUpdateReadInfoAPI(void)
** @Input     : void
** @Output    : TRUE:更新成功 FALSE:更新失败
** @Function  : 读取一条故障记录更新显示
** @The notes : 用户读取故障记录信息前调用
**===============================================================================================*/
u8 ErrRcdUpdateReadInfoAPI(void)
{
	u8 result = 0;
    t_RcdStoreInfo rcdInfo;

    /*需要确认才更新读下一条*/
    if(0x5b != gCANSoxOper_39[2])
    {
    	return(TRUE);
    }
    gCANSoxOper_39[2] = 0;

	/*读取一条记录到监控区*/
	result = RcdReadOneErrRcdUserAPI(1, &rcdInfo);

	/*读取成功*/
	if(TRUE == result)
	{
		/*赋值到监控区*/
		gCANSofInfo_31[0] = rcdInfo.id;
		gCANSofInfo_31[1] = rcdInfo.codeH;
		gCANSofInfo_31[2] = rcdInfo.codeL;
		gCANSofInfo_31[3] = rcdInfo.year;
		gCANSofInfo_31[4] = rcdInfo.MD;
		gCANSofInfo_31[5] = rcdInfo.HM;
		gCANSofInfo_31[6] = rcdInfo.sec;
		gCANSofInfo_31[7] = rcdInfo.info1;
		gCANSofInfo_31[8] = rcdInfo.info2;
		gCANSofInfo_31[9] = rcdInfo.info3;
		gCANSofInfo_31[10] = rcdInfo.info4;
		gCANSofInfo_31[11] = rcdInfo.info5;
	}

	/*更新记录状态显示(剩余总数目)*/
	gCANRcdNum_30[0] = RcdGetErrRcdReadRemNumAPI();

	return(result);
}

/*=================================================================================================
** @Name      : u8 EventRcdUpdateReadInfoAPI(void)
** @Input     : void
** @Output    : TRUE:更新成功 FALSE:更新失败
** @Function  : 读取一条事件记录更新显示
** @The notes : 用户读取事件记录信息前调用
**===============================================================================================*/
u8 EventRcdUpdateReadInfoAPI(void)
{
	u8 result = 0;
    t_RcdStoreInfo rcdInfo;

    /*需要确认才更新读下一条*/
    if(0x5b != gCANSoxOper_39[3])
    {
    	return(TRUE);
    }
    gCANSoxOper_39[3] = 0;

	/*读取一条记录到监控区*/
	result = RcdReadOneEventRcdUserAPI(1, &rcdInfo);

	/*读取成功*/
	if(TRUE == result)
	{
		/*赋值到监控区*/
		gCANSoeInfo_32[0] = rcdInfo.id;
		gCANSoeInfo_32[1] = rcdInfo.codeH;
		gCANSoeInfo_32[2] = rcdInfo.codeL;
		gCANSoeInfo_32[3] = rcdInfo.year;
		gCANSoeInfo_32[4] = rcdInfo.MD;
		gCANSoeInfo_32[5] = rcdInfo.HM;
		gCANSoeInfo_32[6] = rcdInfo.sec;
		gCANSoeInfo_32[7] = rcdInfo.info1;
		gCANSoeInfo_32[8] = rcdInfo.info2;
		gCANSoeInfo_32[9] = rcdInfo.info3;
		gCANSoeInfo_32[10] = rcdInfo.info4;
		gCANSoeInfo_32[11] = rcdInfo.info5;
	}

	/*更新记录状态显示(剩余总数目)*/
	gCANRcdNum_30[1] = RcdGetEventRcdReadRemNumAPI();

	return(result);
}

/*=================================================================================================
** @Name      : u8 WaveRcdUpdateReadInfoAPI(void)
** @Input     : void
** @Output    : TRUE:更新成功 FALSE:更新失败
** @Function  : 读取一条故障录波记录更新显示
** @The notes : 用户读取故障录波记录信息前调用
**===============================================================================================*/
u8 WaveRcdUpdateReadInfoAPI(void)
{
	u8 result = 0;

    /*需要确认才更新读下一条*/
    if(0x5b != gCANSoxOper_39[4])
    {
    	return(TRUE);
    }
    gCANSoxOper_39[4] = 0;

	/*读取一条记录到监控区*/
	result = RcdReadOneWaveRcdUserAPI(1, gCANSowInfo_33, CANINDEX33_LEN);

	/*更新记录状态显示(剩余总数目)*/
	gCANRcdNum_30[2] = RcdGetWaveRcdReadRemNumAPI();

	return(result);
}

/*=================================================================================================
** @Name      : u8 DodRcdUpdateReadInfoAPI(void)
** @Input     : void
** @Output    : TRUE:更新成功 FALSE:更新失败
** @Function  : 读取一条DOD记录更新显示
** @The notes : 用户读取DOD记录信息前调用
**===============================================================================================*/
u8 DodRcdUpdateReadInfoAPI(void)
{
	u8 result = 0;
	t_DodStoreInfo rcdInfo;

    /*需要确认才更新读下一条*/
    if(0x5b != gCANSoxOper_39[5])
    {
    	return(TRUE);
    }
    gCANSoxOper_39[5] = 0;

	/*读取一条记录到监控区*/
	result = RcdReadOneDodRcdUserAPI(1, &rcdInfo);

	/*读取成功*/
	if(TRUE == result)
	{
		/*赋值到监控区*/
		gCANDodInfo_34[0] = rcdInfo.id;
		gCANDodInfo_34[1] = rcdInfo.year;
		gCANDodInfo_34[2] = rcdInfo.MD;
		gCANDodInfo_34[3] = rcdInfo.HM;
		gCANDodInfo_34[4] = rcdInfo.sec;
		gCANDodInfo_34[5] = rcdInfo.dod;
		gCANDodInfo_34[6] = rcdInfo.chgC;
		gCANDodInfo_34[7] = rcdInfo.dhgC;
		gCANDodInfo_34[8] = rcdInfo.chgE;
		gCANDodInfo_34[9] = rcdInfo.dhgE;
	}

	/*更新记录状态显示(剩余数目)*/
	gCANRcdNum_30[3] = RcdGetWaveRcdReadRemNumAPI();

	return(result);
}

/*=================================================================================================
** @Name      : void ErrEvntRcdUpdateStateAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 更新当前故障记录和事件记录状态信息
** @The notes : 用户读取故障记录和事件记录数目前调用
**===============================================================================================*/
void ErrEvntRcdUpdateStateAPI(void)
{
	/*更新记录状态显示*/
	gCANRcdNum_30[0] = RcdGetErrRcdReadRemNumAPI();
	gCANRcdNum_30[1] = RcdGetEventRcdReadRemNumAPI();
	gCANRcdNum_30[2] = RcdGetWaveRcdReadRemNumAPI();
	gCANRcdNum_30[3] = RcdGetDodRcdReadRemNumAPI();
}

/*=================================================================================================
** @Name      : u8 DataRcdUpdateDisplayAPI(void)
** @Input     : void
** @Output    : TRUE:更新成功 FALSE:更新失败
** @Function  : 更新显示一条数据记录
** @The notes : 用户读取数据记录信息前调用
**===============================================================================================*/
u8 DataRcdUpdateDisplayAPI(void)
{
	u8 i = 0;
	u8 result = 0;

	/*读取一条记录到显示区*/
	result = RcdDisplayOneDataRcdUserAPI(sDataRcdDisplayDate, gCANSodInfo_37, CANINDEX37_LEN);

	/*读失败*/
	if(FALSE == result)
	{
		/*清零*/
		for(i = 0; i < CANINDEX37_LEN; i++)
		{
			gCANSodInfo_37[i] = 0;
		}
	}

	/*更新记录状态显示*/
	gCANSodNum_35[0] = RcdGetDataRcdNowDaysAPI();		//记录天数
	gCANSodNum_35[1] = RcdGetDataRcdAllNumAPI();		//总条数
	gCANSodNum_35[2] = RcdGetDataRcdDateNumAPI(0);		//当天记录条数

	gCANSodDay_36[0] = RcdGetDataRcdDisplayDateAPI();   //当前显示日期
	gCANSodDay_36[1] = RcdGetDataRcdDisPlayRemNumAPI();	//当前显示日期剩余条数

	return(result);
}

/*=================================================================================================
** @Name      : void DataRcdUpdateRcdStateAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 更新当前数据记录状态信息
** @The notes : 用户读取数据记录状态前调用
**===============================================================================================*/
void DataRcdUpdateRcdStateAPI(void)
{
	u8 i = 0;

	/*更新记录状态显示*/
	gCANSodNum_35[0] = RcdGetDataRcdNowDaysAPI();		//记录天数
	gCANSodNum_35[1] = RcdGetDataRcdAllNumAPI();		//总条数
	gCANSodNum_35[2] = RcdGetDataRcdDateNumAPI(0);		//当天记录条数(今天)

	gCANSodDay_36[0] = RcdGetDataRcdDisplayDateAPI();   //当前显示日期
	gCANSodDay_36[1] = RcdGetDataRcdDisPlayRemNumAPI(); //当前显示日期剩余条数

	//每天记录条数(前1天[昨天]开始)
	for(i = 1; i < RcdGetDataRcdNowDaysAPI(); i++)
	{
		gCANSodDay_36[1 + i] = RcdGetDataRcdDateNumAPI(i);
	}
}

/*=================================================================================================
** @Name      : void DataRcdSetReadDateAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 设置需要读取的数据记录信息日期
** @The notes : 用户接收切换数据记录读取日期命令时调用
**===============================================================================================*/
void DataRcdSetReadDateAPI(void)
{
	/*记录设置的日期*/
	sDataRcdDisplayDate = gCANSoxOper_39[0];
}

/*=================================================================================================
** @Name      : void DataRcdClrDateInfoAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除指定日期数据记录信息
** @The notes : 用户接收清除数据记录命令时调用
**===============================================================================================*/
void DataRcdClrDateInfoAPI(void)
{
	u8 i = 0;

	/*清除指定当天数据记录*/
	(void)RcdClrDateDataRcdUserAPI(gCANSoxOper_39[1]);

	/*清除全部记录*/
	if(0xff == gCANSoxOper_39[1])
	{
		//每天记录条数清零
		for(i = 1; i < CANINDEX36_LEN; i++)
		{
			gCANSodDay_36[i] = 0;
		}
	}

	//更新记录状态信息显示
	DataRcdUpdateRcdStateAPI();
}

/*=================================================================================================
** @Name      : void ErrRcdClrDateInfoAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除指定日期故障记录信息
** @The notes : 用户接收清除故障记录命令时调用
**===============================================================================================*/
void ErrRcdClrDateInfoAPI(void)
{
	/*清除指定当天数据记录*/
	(void)RcdClrDateErrRcdUserAPI(gCANSoxOper_39[2]);

	/*更新记录状态显示*/
	gCANRcdNum_30[0] = RcdGetErrRcdReadRemNumAPI();
}

/*=================================================================================================
** @Name      : void EventRcdClrDateInfoAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除指定日期事件记录信息
** @The notes : 用户接收清除事件记录命令时调用
**===============================================================================================*/
void EventRcdClrDateInfoAPI(void)
{
	/*清除指定当天数据记录*/
	(void)RcdClrDateEventRcdUserAPI(gCANSoxOper_39[3]);

	/*更新记录状态显示*/
	gCANRcdNum_30[1] = RcdGetEventRcdReadRemNumAPI();
}

/*=================================================================================================
** @Name      : void WaveRcdClrDateInfoAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除指定日期故障录波信息
** @The notes : 用户接收清除故障录波命令时调用
**===============================================================================================*/
void WaveRcdClrDateInfoAPI(void)
{
	/*清除指定当天数据记录*/
	(void)RcdClrDateWaveRcdUserAPI(gCANSoxOper_39[4]);

	/*更新记录状态显示*/
	gCANRcdNum_30[2] = RcdGetWaveRcdReadRemNumAPI();
}

/*=================================================================================================
** @Name      : void DodRcdClrAllInfoAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除全部DOD记录信息
** @The notes : 用户接收清除历史DOD记录命令时调用
**===============================================================================================*/
void DodRcdClrAllInfoAPI(void)
{
	if(0xff == gCANSoxOper_39[5])
	{
		/*清除全部数据记录*/
		(void)RcdClrAllDodRcdUserAPI();
	}

	/*更新记录状态显示*/
	gCANRcdNum_30[3] = RcdGetDodRcdReadRemNumAPI();
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
