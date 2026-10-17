/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : InsuCheck.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 绝缘检测模块头文件
** @Instructions :
**===========================================================================================*/
#ifndef FUNTION_MEASUREMENT_INSUCHECK_H_
#define FUNTION_MEASUREMENT_INSUCHECK_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define ISO_ERR_DIAGTIME		(60)        /*绝缘检测故障时周期1Min,(1s)*/
#define ISO_MIN_BAT_V			(36.0)    	/*绝缘检测最低总压,(V)*/
#define ISO_R2					(2200.0)    /*绝缘计算电阻值R2,(KΩ)*/
#define ISO_R1					(20.0)      /*绝缘计算电阻值R1,(KΩ)*/
#define ISORp_Inval_Value		(3000.0)	/*正极对地默认无穷值 1kΩ*/
#define ISORn_Inval_Value		(3000.0)	/*负极对地默认无穷值 1kΩ*/

#define Isu_Calc_Dec_Places 	(1000)      /*绝缘采样小数点计算位数 10：一位小数点  100：两位小数点   1000：三位小数点    以此类推
 	 	 	 	 	 	 	 	 	 	 	 注意：如果绝缘采样值跳动幅度大，建议减少小数点位数(同时绝缘采样值的精度反而会减少)*/

#define ADH_EXE_DIAGTIME		(60)        /*粘连检测(采Link电压)连续执行时长 1Min,(1s)*/
#define ADH_ERR_DIAGTIME		(600)       /*粘连检测(采Link电压)故障时周期10Min,(1s)*/

//#define Adhesion_Link1_Vref	(3.025)     /*粘连检测Link+基准电压值(高于该值粘连)*/
#define Adhesion_Link1_Vref		(3.150)     /*粘连检测Link+基准电压值(高于该值粘连)*/
#define Adhesion_Link2_Vref		(2.973)     /*粘连检测Link-基准电压值(该值±0.005内粘连)*/
#define Adhesion_Link2_Voff		(0.005)     /*粘连检测Link-偏移电压值(基准值±该值内粘连)*/
#define Adhesion_Check_DiffV 	(10.0)		/*粘连检测判断压差*/
#define Adhesion_Check_R1 		(2200.0)	/*粘连检测电阻R1*/
#define Adhesion_Check_R2 		(3.3)		/*粘连检测电阻R2*/

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*绝缘检测计算结构体*/
typedef struct
{
	float SampVbat;
	float SampV1;
	float SampV2;
	float CalcCoeA;/*计算系数A*/
	float CalcCoeB;/*计算系数B*/

}t_IsoCalcMsg;

/*接触器粘连检测计算结构体*/
typedef struct
{
	u8 SwhState;
	float SampLoadV;
	float SampLinkV1;
	float SampLinkV2;

}t_AdhesionCalcMsg;

/*绝缘采样通道*/
typedef enum
{
    eInsu_Bat = 0,     /*电池总电压*/
	eInsu_V1 = 1,      /*绝缘电压V1*/
	eInsu_V2 = 2,      /*绝缘电压V2*/

    eInsu_Num          /*绝缘采样通道个数*/
}e_InsuChan;

/*粘连检测电压采样通道*/
typedef enum
{
	eAdhSamp_LoadV = 0,     /*负载电压,LTC2949 V7口电压，不经过倍数关系*/

	eAdhSamp_Link1 = 1,     /*Link+电压,LTC2949端口电压，不经过倍数关系*/
	eAdhSamp_Link2 = 2,     /*Link-电压,LTC2949端口电压，不经过倍数关系*/

    eAdhSamp_Num            /*粘连检测电压采样通道个数*/
}e_AdhesSampChan;

/*绝缘检测Vpgnd采样步骤*/
typedef enum
{
    eIsoVpgnd_Init = 0,    /*初始化*/
    eIsoVpgnd_SeleV1 = 1,  /*片选V1*/
    eIsoVpgnd_SampV1 = 2,  /*采集V1*/
    eIsoVpgnd_SeleV2 = 3,  /*片选V2*/
    eIsoVpgnd_SampV2 = 4,  /*采集V2*/
    eIsoVpgnd_SampOK = 5,  /*采集完成*/
    eIsoVpgnd_DiagOK = 6,  /*检测完成*/

	eIsoVpgnd_Fin          /*绝缘检测Vpgnd采样步骤结束*/
}e_IsoVpgndSampStep;

/*接触器粘连检测Link+/Link-采样步骤*/
typedef enum
{
    eAdhVlink_Init = 0,    /*初始化*/
    eAdhVlink_SeleV1 = 1,  /*片选V1(断开Rly1闭合Rly2)*/
    eAdhVlink_SampV1 = 2,  /*采集Link+电压V1*/
    eAdhVlink_SeleV2 = 3,  /*片选V2(闭合Rly1断开Rly2)*/
    eAdhVlink_SampV2 = 4,  /*采集Link-电压V2*/
    eAdhVlink_SampOK = 5,  /*采集完成*/
    eAdhVlink_DiagOK = 6,  /*诊断完成*/

	eAdhVlink_Fin          /*接触器粘连检测Link+/Link-采样步骤结束*/
}e_AdhVlinkDiagStep;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void InsuAdhesMeasurInit(void)
** @Input     : void
** @Output    : void
** @Function  : 绝缘采样和接触器粘连检测采样控制模块初始化
** @The notes : 初始化调用
**===========================================================================================*/
void InsuAdhesMeasurInit(void);

/*=============================================================================================
** @Name      : void void InsuAdhesMeasurTask(void)
** @Input     : void
** @Output    : void
** @Function  : 绝缘采样和接触器粘连采样控制任务
** @The notes : 100ms周期函数
**===========================================================================================*/
void InsuAdhesMeasurTask(void);

/*=============================================================================================
** @Name      : void InputSwhAdhesSampData(e_AdhesSampChan chan, float data)
** @Input     : chan:通道    data：采样值
** @Output    : void
** @Function  : 输入接触器粘连检测采样值
** @The notes : Link+/Link-采样值
**===========================================================================================*/
void InputSwhAdhesSampData(e_AdhesSampChan chan, float data);

/*=============================================================================================
** @Name      : void UserReqSwhAdhesDiagAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 设置用户请求接触器粘连检测
** @The notes :
**===========================================================================================*/
void UserReqSwhAdhesDiagAPI(void);

/*=============================================================================================
** @Name      : void SetSwhAdhesDiagFlagAPI(e_AdhVlinkDiagStep flag)
** @Input     : flag:接触器粘连检测步骤标志 0:初始化 1:设置继电器    2:Link+采样   3:Link-采样 4:采样完成  5:检测完成
** @Output    : void
** @Function  : 设置接触器粘连检测步骤标志
** @The notes :
**===========================================================================================*/
void SetSwhAdhesDiagFlagAPI(e_AdhVlinkDiagStep flag);

/*=============================================================================================
** @Name      : u8 GetSwhAdhesDiagFlagAPI(void)
** @Input     : void
** @Output    : 接触器粘连检测步骤标志:0:初始化 1:设置继电器    2:Link+/Link-采样 3:采样完成  4:检测完成
** @Function  : 获取接触器粘连检测步骤标志
** @The notes :
**===========================================================================================*/
u8 GetSwhAdhesDiagFlagAPI(void);

/*=============================================================================================
 ** @Name      : u8 GetSwhAdhesErrFlagAPI(void)
 ** @Input     : void
 ** @Output    : 按bit位表示接触器粘连异常状态:bit0:主正接触器 bit1:预充接触器 bit2:负极接触器 [0:正常 1:粘连]
 ** @Function  : 获取接触器粘连异常状态标志
 ** @The notes :
 **===========================================================================================*/
 u8 GetSwhAdhesErrFlagAPI(void);

 /*=============================================================================================
 ** @Name      : void InputInsuSampDataAPI(e_InsuChan chan, float data)
 ** @Input     : chan:通道    data：采样电压值 1V
 ** @Output    : void
 ** @Function  : 记录绝缘检测需要的采样电压值数据
 ** @The notes : LTC2949高压采样调用
 **===========================================================================================*/
 void InputInsuSampDataAPI(e_InsuChan chan, float data);

 /*=============================================================================================
 ** @Name      : void UserReqInsuCheckAPI(void)
 ** @Input     : void
 ** @Output    : void
 ** @Function  : 设置用户请求绝缘检测
 ** @The notes :
 **===========================================================================================*/
 void UserReqInsuCheckAPI(void);

/*=============================================================================================
** @Name      : void SetInsuVpgndSampFlagAPI(e_IsoVpgndSampStep flag)
** @Input     : flag:绝缘Vpgnd电压采样步骤标志 0:不进行采样  1:片选Vpgnd-V1电压 2:Vpgnd-V1电压采样    3:片选Vpgnd-V2电压 4:Vpgnd-V2电压采样 5:Vpgnd电压采样完成
** @Output    : void
** @Function  : 设置绝缘Vpgnd电压采样步骤标志(状态)
** @The notes :
**===========================================================================================*/
void SetInsuVpgndSampFlagAPI(e_IsoVpgndSampStep flag);

/*=============================================================================================
** @Name      : u8 GetInsuVpgndSampFlagAPI(void)
** @Input     : void
** @Output    : 绝缘Vpgnd电压采样步骤标志 0:不进行采样  1:片选Vpgnd-V1电压 2:Vpgnd-V1电压采样    3:片选Vpgnd-V2电压 4:Vpgnd-V2电压采样 5:Vpgnd电压采样完成 6:绝缘检测完成
** @Function  : 获取绝缘Vpgnd电压采样步骤标志
** @The notes :
**===========================================================================================*/
u8 GetInsuVpgndSampFlagAPI(void);

/*=============================================================================================
** @Name      : u8 GetInsuErrStateFlagAPI(void)
** @Input     : void
** @Output    : 绝缘状态标志:0:正常 1：一级故障(严重) 2:二级故障 (轻微) 3:绝缘检测异常(失效)
** @Function  : 获取绝缘状态标志
** @The notes :
**===========================================================================================*/
u8 GetInsuErrStateFlagAPI(void);

/*=============================================================================================
** @Name      : void ClrInsuAdhesErrFlagAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除接触器粘连和绝缘异常状态标志
** @The notes :
**===========================================================================================*/
void ClrInsuAdhesErrFlagAPI(void);

/*=============================================================================================
** @Name      : u8 GetInsuAdhesFinStateAPI(void)
** @Input     : void
** @Output    : 绝缘和接触器粘连检测第一遍检测完成状态  TRUE:完成 FALSE:未完成
** @Function  : 获取绝缘和接触器粘连检测第一遍检测完成状态
** @The notes :
**===========================================================================================*/
u8 GetInsuAdhesFinStateAPI(void);

#endif
