/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : HigAFESamp.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 高压采样模块
** @Instructions :
**===========================================================================================*/
#ifndef FUNTION_MEASUREMENT_HIGAFESAMP_H_
#define FUNTION_MEASUREMENT_HIGAFESAMP_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define SHUNT_T_SAMP_EN    (1)      /*分流器温度表采样使能*/
#define SHUNT_T_TAB_LEN    (223)    /*分流器温度表长度[-41,182]*/

#define SHUNT_V_COMP_EN    (0)      /*分流器温度补偿电压使能*/
#define SHUNT_V_TAB_LEN    (20)     /*分流器温度补偿电压表长度[-30,160]*/

#define HVLV_SAMP_TIMES    (10)     /*高压低压采样次数比*/
#define RUN_PREV_SAMP_CHAN (1)      /*运行状态(接触器闭合)预充电压采样通道 0:负载电压V7通道, 1:Link+电压通道*/

#define INSO_VFIT_ENABLE   (1)		/*绝缘采样滤波使能*/
#define INSO_VFIT_TIMES    (10)	    /*绝缘电压采样滤波次数,注意：次数不能太大，容易越界*/

// some more error codes, additionally to
// the internal error codes from the LTC2949 library
// (e.g. LTC2949_ERRCODE_COMMERR... see LTC2949.h for
// error codes reported from library functions)
#define LTCDEF_ERRCODE_MUX_READBACK 0x0100U
#define LTCDEF_ERRCODE_ABSRELCHK    0x0200U
#define LTCDEF_ERRCODE_ADCCONF_RB   0x0400U
#define LTCDEF_ERRCODE_HS_SET_CHK   0x0800U
#define LTCDEF_ERRCODE_HS_CLR_CHK   0x1000U
#define LTCDEF_ERRCODE_STAT_FAULTS  0x2000U

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*LTC2949采样通道数目*/
typedef enum
{
	HIG_AFE_Shunt = 0,	/*分流器电流采样*/
	HIG_AFE_BatV,	    /*电池总电压采样*/
	HIG_AFE_PreV,	    /*预充电压采样*/
	HIG_AFE_Link1V,    	/*link+电压采样,外端电压(预充电压)正极电压采集*/
	HIG_AFE_Link2V,		/*link-电压采样,外端电压(预充电压)负极电压采集*/
	HIG_AFE_PgndV,    	/*PGND电压采样,机壳相对对负母线电压*/
	HIG_AFE_Temp,		/*分流器温度采样*/
	HIG_AFE_Vref,		/*采样芯片基准源,LTC2949基准回采*/
	HIG_AFE_Out5V,		/*采样芯片5V输出电压采样,高压侧5V电压采集*/
	HIG_AFE_LoadV,	    /*负载电压采样,负载电压采集*/
	HIG_AFE_LoadV7,	    /*负载电压V7原始采样值*/

	HIG_AFE_CHAN
}e_HAfeSampChan;

/*LTC2949   4路DO输出控制*/
typedef enum
{
	AFE_IOCTRL_BAT = 1,	/*总正采样回路控制(RLY8)*/
	AFE_IOCTRL_Link2,  	/*Link-,外端电压(预充电压)负极采样回路控制(RLY1)*/
	AFE_IOCTRL_Link1,  	/*Link+,外端电压(预充电压)正极采样回路控制(RLY2)*/
	AFE_IOCTRL_ISO,  	/*绝缘采样(RLY9)*/

	AFE_IOCTRL_CHAN
}e_AfeIoCtrlChan;

/*LTC2949采样步骤*/
typedef enum
{
	LTC2949_STEP_SHUV = 0,		/*分流器电流采样*/
	LTC2949_STEP_SUMV = 5,		/*电池总电压采样*/
	LTC2949_STEP_LODV = 10,     /*外端负载电压采样*/
	LTC2949_STEP_LNKV = 15,		/*接触器粘连Link±电压采样*/
	LTC2949_STEP_ISOV = 25,		/*绝缘PGND电压采样*/
	LTC2949_STEP_PGND = 35,		/*负母线电压采样*/
	LTC2949_STEP_SHUT = 40,		/*分流器温度采样*/
	LTC2949_STEP_REFV = 45,		/*芯片基准Vref电压采样*/
	LTC2949_STEP_IC5V = 50,		/*芯片5V输出电压采样*/

	LTC2949_STEP_FIN
}e_Ltc2949SampStep;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void HVSampLTC2949Init(void)
** @Input     : void
** @Output    : void
** @Function  : 高压采样芯片2949初始化
** @The notes : 初始化调用
**===========================================================================================*/
void HVSampLTC2949Init(void);

/*=============================================================================================
** @Name      : void HigInfoSampleTask(void *P)
** @Input     : void
** @Output    : void
** @Function  : 高压采/分流器电流样任务
** @The notes : 周期任务
**===========================================================================================*/
void HigInfoSampleTask(void *P);

/*=============================================================================================
** @Name      : float Ltc2949ChangeADtoVolt(e_HAfeSampChan index, float data)
** @Input     : index:采样通道 data:采样值
** @Output    : void
** @Function  : 将LTC2949采样值转化为实际值
** @The notes : 15bits
**===========================================================================================*/
float Ltc2949ChangeADtoVolt(e_HAfeSampChan index, float data);

/*=============================================================================================
** @Name      : void Ltc2949GpioSetOutPutState(e_AfeIoCtrlChan chan, u8 state)
** @Input     : chan端口     state: 0 或 1
** @Output    : void
** @Function  : 设置gpio端口号输出状态
** @The notes :
**===========================================================================================*/
void Ltc2949GpioSetOutPutState(e_AfeIoCtrlChan chan, u8 state);

/*=============================================================================================
** @Name      : u8 CheckDeviceIsReady(void)
** @Input     : void
** @Output    : void
** @Function  : 检测LTC2949是否处于唤醒和正确配置状态
** @The notes :
**===========================================================================================*/
u8 CheckDeviceIsReady(void);

/*=============================================================================================
** @Name      : void SampSetOutHVSampFinStateAPI(u8 state)
** @Input     : state:[0未采样 1采样完成]
** @Output    : void
** @Function  : 设置外端高压采样完成状态
** @The notes : 负载电压或预充电压
**===========================================================================================*/
void SampSetOutHVSampFinStateAPI(u8 state);

/*=============================================================================================
** @Name      : u8 SampGetOutHVSampFinStateAPI(void)
** @Input     : void
** @Output    : 外端高压采样完成标志 [0未采样 1采样完成]
** @Function  : 获取外端高压采样完成状态
** @The notes : 负载电压或预充电压
**===========================================================================================*/
u8 SampGetOutHVSampFinStateAPI(void);

/*=============================================================================================
** @Name      : u8 SampGetHigVSampExpFlagAPI(void)
** @Input     : void
** @Output    : 高压采样失效标志[按Bit位表示:Bit0:初始化配置异常 Bit1:通信异常](0:正常 1:异常)
** @Function  : 获取高压采样失效标志
** @The notes :
**===========================================================================================*/
u8 SampGetHigVSampExpFlagAPI(void);

/*=============================================================================================
** @Name      : u8 SampGetHigAFERelyStateAPI(void)
** @Input     : void
** @Output    : 高压采样外围继电器控制状态(按位表示1个继电器) 0:断开 1:闭合
** @Function  : 获取高压高压采样各个外围继电器控制状态
** @The notes : bit0:总压  bit1:Link-RLY1 bit2:Link+RLY2 bit3:绝缘
**===========================================================================================*/
u8 SampGetHigAFERelyStateAPI(void);

#endif
