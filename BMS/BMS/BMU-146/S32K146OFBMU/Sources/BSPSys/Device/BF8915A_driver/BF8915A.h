/**
  ******************************************************************************
  * @file    BF8915A.h
  * @version V1.1.4
  * @date    2021-12-20
  * @brief   This file provides the BF8915A functions(该文件提供BF8915A功能).
 */
#ifndef __BF8915A_H__
#define __BF8915A_H__

//#include "sys.h"

#include <stdlib.h>
#include "stdint.h"
#include "DEVGPIO.h"

/*
typedef enum
{
	FALSE = 0U,
	TRUE =  1U,
}BOOLEAN_Type;

typedef BOOLEAN_Type BOOLEAN;//布尔类型的值，代替了stdbool中定义的bool值，方便客户做移植
*/
extern BOOLEAN	 isospi_reverse;//改变该变量可以实现菊花链SPI正反向通信。
                                //FALSE:相对的正向通信，本SDK中的SPI1为相对正向通信，SPI1菊花链连接的第一颗IC看作是IC0;
                                //TURE: 相对的反向通信，本SDK中的SPI2为相对反向通信，但也是SPI1菊花链连接的第一颗IC看作是IC0。

extern const u16 crc15Table[256];//precomputed CRC15 Table(预计算15位循环冗余校验值的表)


#define TOTAL_IC					20U	 //菊花链上串接IC(8915A)的个数，需根据实际情况来修改此数


#define CELL_CHANNELS				16U	 //16个电池通道
#define GPIO_CHANNELS				9U	 //8个GPIO + 1个VD33T
#define STAT_CHANNELS				8U	 //8个16位的电压数据
#define NUM_CV_REG					6U	 //Cell Voltage Register Group(电池电压寄存器组) A B C D E F共6个
#define NUM_GOIO_REG				3U	 //Auxiliary Register Group(含有GPIOx电压信息)A B C 共3个
#define NUM_STAT_REG				4U	 //Status Register Group(状态寄存器组) A B C D共4个

//ADC转换的电池选择
#define CSEL_CH_ALL 			0U
#define CSEL_CH_1and9 			1U
#define CSEL_CH_2and10 			2U
#define CSEL_CH_3and11 			3U
#define CSEL_CH_4and12 			4U
#define CSEL_CH_5and13 			5U
#define CSEL_CH_6and14 			6U
#define CSEL_CH_7and15 			7U
#define CSEL_CH_8and16			8U

//ADC转换的GPIO选择
#define GSEL_CH_ALL 			0U   //所有GPIO0~GPIO7,VD33T
#define GSEL_CH_GPIO0 			1U
#define GSEL_CH_GPIO1 			2U
#define GSEL_CH_GPIO2 			3U
#define GSEL_CH_GPIO3 			4U
#define GSEL_CH_VD33T 			5U
#define GSEL_CH_GPIO4 			6U
#define GSEL_CH_GPIO5 			7U
#define GSEL_CH_GPIO6 			8U
#define GSEL_CH_GPIO7 			9U

//ADC转换内部参数选择	STSEL
#define STSEL_CH_ALL 			0U   //所有内部参数VD33、VH、VD7、VD5、VD4、VTEMP、VREF2、VREF_LP
#define STSEL_CH_VD33 			1U
#define STSEL_CH_VH 			2U
#define STSEL_CH_VD7 			3U
#define STSEL_CH_VD5 			4U
#define STSEL_CH_VD4			5U
#define STSEL_CH_VTEMP 			6U
#define STSEL_CH_VREF2 			7U
#define STSEL_CH_VREF_LP 		8U

//ADC频率选择
#define CLKSEL_2M 				1U

//CFGAR2
#define OSR_SEL 		        FALSE	//0U ADC过采样率为64、128、256、512(四种过采样率通过指令的OSR选择)

//ADC过采样率选择
#define OSR_64 				      0U
#define OSR_128 				  1U
#define OSR_256 				  2U
#define OSR_512 				  3U
	
//放电允许	
#define DISCP_DISABLE				0U   //DISCP=0，当对应的电池和相邻的电池被测量时，S脚的放电状态将不使能
#define DISCP_ENABLE				1U   //DISCP=1，在电池测量期间S脚的放电状态不改变	
	
//自测试模式选择	
#define SELFTEST_6666 				0U   //ST = 0,输出样本0x6666
#define SELFTEST_9999 				1U   //ST = 1,输出样本0x9999
	
//检测模式选择	
#define MNMOD_DISABLE				0U   //不进监测模式，指令无效
#define MNMOD_1						1U   //监测模式1,模拟电源和时钟打开，pll_100m打开
#define MNMOD_2						2U   //监测模式2,模拟电源和时钟打开，pll_100m关闭
#define MNMOD_3						3U   //监测模式3,低功耗监测，模拟电源和时钟关闭

//检测模式开关
#define	STRMN_ENABLE				1U   //开启监测模式
#define STRMN_DISABLE				0U   //关闭监测模式

//GPIO断线检测电流源选择
#define	DOWN_GPIO_UP				0U   //上拉电流源
#define DOWN_GPIO_DOWN				1U   //下拉电流源

//均衡放电电池奇偶选择
#define	OESEL_ODD					0U   //奇数节
#define OESEL_EVEN					1U   //偶数节

//均衡开关总控制信号2
#define	BLEN_ON						1U   //开启
#define BLEN_OFF					0U   //关闭

/***命令的bit[12:9]位***/
#define CMD_ADCV					0x01U
#define CMD_ADCOW					0x02U
#define CMD_CVST					0x03U
#define CMD_ADOL					0x04U
#define CMD_ADGP					0x05U
#define CMD_ADGOW					0x06U
#define CMD_GPST					0x07U
#define CMD_ADSTAT					0x08U
#define CMD_STATST					0x09U
#define CMD_ADCVGP					0x0aU
#define CMD_ADCVVH					0x0bU
#define CMD_MNT						0x0cU
#define CMD_STRBL					0x0dU

/***配置寄存器***/
#define RDCFGA						0x05U
#define RDCFGB						0x06U
#define RDCFGC						0x07U
#define RDCFGD						0x08U

/***电池电压寄存器对应的命令***/
#define CVAR						0x01U    //	 0x01 + 0x08 = 0x09U	函数BF8915A_rdcv_reg中用到
#define CVBR						0x02U    //	 0x02 + 0x08 = 0x0AU
#define CVCR						0x03U    //	 0x03 + 0x08 = 0x0BU
#define CVDR						0x04U    //	 0x04 + 0x08 = 0x0CU
#define CVER						0x05U    //	 0x05 + 0x08 = 0x0DU
#define CVFR						0x06U    //	 0x06 + 0x08 = 0x0EU
/***GPIOG电压***/
#define GVAR						0x01U		 //  0x01 + 0x0E = 0x0FU 函数BF8915A_rdgv_reg中用到
#define GVBR						0x02U		 //  0x02 + 0x0E = 0x10U
#define GVCR						0x03U		 //  0x03 + 0x0E = 0x11U
/***状态寄存器RDSTAT***/              
#define STATA						0x01U		 //	 0x01 + 0x11 = 0x12U
#define STATB						0x02U		 //	 0x02 + 0x11 = 0x13U
#define STATC						0x03U		 //	 0x03 + 0x11 = 0x14U
#define STATD						0x04U		 //	 0x04 + 0x11 = 0x15U

#define REG_ALL 					0x00U
#define NUM_RX_BYT  				0x08U	   //接收到数据的字节数，6个数据+2个PEC(检错码)

typedef struct//Register data structure(寄存器数据结构).
{
	u8 tx_data[6];				  	//由于每个寄存器组有六byte(字节)，所以数组中元素个数为6
	u8 rx_data[8];				  	//接收到的寄存器组中 6 byte(字节)+ 2 byte PEC(检错码)
	u8 rx_pec_match; 	  			//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
}Reg_TypeDef;

typedef struct//ConfigA data structure(配置A数据结构).
{
	u8 		gpio_pd_en;				//GPIO下拉使能，每一位对应一个IO口
	u8 		gpio_analog_en;			//GPIO0~7模式选择		0:模拟输入		1:普通GPIO
	BOOLEAN rsvd2;				    //保留位，永远置0
	BOOLEAN rsvd1;		        	//保留位，永远置0
	BOOLEAN osr_sel;				//ADC过采样率选择  1:ADC过采样率为1024、2048、4096、4096	 0:ADC过采样率为64、128、256、512
	BOOLEAN refon;					//VREF1(基准电压)配置 1 :VREF1保持上电直到WDT溢出  0 :ADC转换完之后VREF1关闭 (默认)
	u16 	uv_th;					//欠压阈值
	u16 	ov_th;					//过压阈值
	u8   	rx_pec_match;			//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
}ConfigA_TypeDef;

typedef struct//ConfigB data structure(配置B数据结构).
{
	u16 	dcc;					//放电电池开关1:打开Cell x 的开关  0:关闭Cell x 的开关 (默认)
	u8 		bl_duty_sel;			//主动均衡占空比选择  占空比=(BL_DUTY_SEL+1)*10%，配置范围10%~100%，当大于9时是100%。
	u8 		dcto;					//放电定时溢出值配置
	u16 	vuv_bl_th;				//均衡欠压阈值
	BOOLEAN dtmen;					//在自动均衡下，如果DTMEN=1，当欠压(小于VUV_BL)关闭放电开关，如果DTMEN=0，不进行欠压比较
	u8 		rx_pec_match; 			//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
}ConfigB_TypeDef;

typedef struct//ConfigC data structure(配置C数据结构).
{
	u16 	vuv_m_th;				//监测模式下，欠压阈值
	u16 	vov_m_th;				//监测模式下，过压阈值
	u16 	itov_m_th;				//监测模式下，内部过温阈值
	u16 	extov_m_th; 			//监测模式下，外部过温阈值
	u8 		rx_pec_match; 			//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
}ConfigC_TypeDef;

typedef struct//ConfigD data structure(配置D数据结构).
{
	u16 		csel_m;					//监测模式下，电池电压监测比较选择 1:相应的电池就会在监测模式下进行过欠压比较，置标志位
															// 															 	 0:相应的电池就会在监测模式下不进行过欠压比较，不置标志位
	u8 			gsel_m;					//监测模式下，GPIO0~7比较选择
	BOOLEAN 	isospi_wake_t;			//isospi(菊花链SPI)唤醒时间，0:10us		1:20us
	BOOLEAN 	adc_rst_sel;			//测量第1个通道ADC复位选择，0:所有测量通道都复位ADC，1:只有第1个通道复位ADC，之后2~n通道不复位ADC
	u8 			time_sel_m;				//监测模式下，定时监测时间配置，定时时间=(TIME_SEL_M+1)*1s(2~64s)，最小配置值是1(2s)
	u8 			adc_init_t2;			//ADC测量第2~n个通道ADC初始化时间，(ADC_INIT_T2+1)*20us(20~320us)
	u8 			adc_init_t1;			//ADC测量第1个通道ADC初始化时间(ADC_INIT_T1+1)*100us(100~1600us)
	BOOLEAN 	trim2_en;				//修调2使能，1:使能，0:不使能
	BOOLEAN 	trim1_en;				//修调1使能，1:使能，0:不使能
	u8 			err_t_sel_m;			//监测模式下发生异常，上报的间隔时间  00:200ms，01:400ms，10:600ms，11:800ms
	u8 			vd33t_on_m;				//监测模式下，VREF1和VD33T的启动时间配置(5+VD33T_ON_M*10)ms，范围是5~155ms
	u8 			rx_pec_match; 			//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
}ConfigD_TypeDef;

typedef struct//Cell Voltage data structure(电池电压数据结构).
{
	//因为有16节电池，所以数组的有效数为16个
	u16 	cellVoltage[16];
	//因为有ABCDEF六个电池电压寄存器
	u8 	pec_match[6]; 		//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
}CellVoltage_TypeDef;

typedef struct//AUX Reg Voltage Data structure(GPIO寄存器电压数据结构)
{
	u16 	gpioVoltage[9]; 	//GPIO Voltage Codes(辅助寄存器中的G0V~G7V)GPIOA、B、C(8个GPIO + 1个VD33T)
	u8 		pec_match[3]; 		//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
}GPIOReg_TypeDef;

typedef struct//Status Reg data structure(状态寄存器数据结构)
{
	u16 	stat_codes[8];		//A two dimensional array of the stat voltage codes(统计电压代码的二维数组)
	u8  	uvOvFlags[4]; 		//byte array that contains the uv/ov flag data(包含uv/ov标志数据的字节数组)
	u16 	cellUvFlag;			//电池欠压标志位，每个位用来标志每个电池的欠压标志位
	u16 	cellOvFlag;			//电池过压标志位，每个位用来标志每个电池的过压标志位
	u8  	cfg_err;	 		//配置字校验出错标志(包括奇偶异或校验和ECC校验)
	u8  	pec_err;	 		//指令数据的PEC校验出错标志
	u8  	extov;		 		//监测模式下，外部温度(gpio)过温标志
	u8  	itov;			 	//监测模式下，内部温度(vtemp)过温标志
	u8  	htov2;		 		//硬件温度(95℃)过温标志， 1:硬件温度过温，0:硬件温度不过温
	u8  	htov1;     			//硬件温度(125℃)过温标志，1:硬件温度过温，0:硬件温度不过温
	u8  	mux_fail;  			//开关译码测试结果， 0 : 开关译码正确		1: 开关译码错误
	u8  	mode_state;			//模式状态寄存器，   00:standby模式 01:测量模式 10:监测模式 11:保留位(sleep模式关闭时钟，不能读出相关状态)
	u8  	rst_state; 			//软件复位标志寄存器
	u8  	pec_match[4]; 		//如果在最近的读取cmd(指令)期间检测到PEC(检错码)错误
}StateReg_TypeDef;						//A、B、C、D四个状态寄存器

/***************************需要修改的函数********************************************
 *其中包括
 *片选CS      :相对正向:BF8915_CS_LOW()和cs_high()；相对反向:BF8915_CS_LOW_Reverse()和cs_high_reverse()
 *延时函数    :delay_us
 *SPI接口     :SPI1_ReadWriteByte和SPI2_ReadWriteByte
 *********************************END************************************************/

#define BF8915_CS_LOW()				{(void)DEVGPIOSetPortState(ePOut_LTC6820_CS1, ePSTATE_OFF);}	//将相对正向通信的片选CS设置为低电平
#define BF8915_CS_HIGH()			{(void)DEVGPIOSetPortState(ePOut_LTC6820_CS1, ePSTATE_ON);}		//将相对正向通信的片选CS设置为高电平
#define BF8915_CS_LOW_Reverse()		{(void)DEVGPIOSetPortState(ePOut_LTC6820_CS2, ePSTATE_OFF);}	//将相对反向通信的片选CS设置为低电平
#define	BF8915_CS_HIGH_Reverse()	{(void)DEVGPIOSetPortState(ePOut_LTC6820_CS2, ePSTATE_ON);}		//将相对反向通信的片选CS设置为高电平

//延时函数
extern void delay_us(u32 nus);

//TxData 可以是8位或16位的，在启用SPI之前就确定好数据帧格式
extern u8 SPI1_ReadWriteByte(u8 TxData);

//TxData 可以是8位或16位的，在启用SPI之前就确定好数据帧格式
extern u8 SPI2_ReadWriteByte(u8 TxData);

//Wake isoSPI up from idle state(将菊花链SPI从空闲状态唤醒)
void wakeup_idle(u8 total_ic);

///Generic wakeup command to wake the BF8915A from sleep(将BF8915A从睡眠中唤醒的通用唤醒命令)
void wakeup_sleep(u8 total_ic);

//Writes an array of bytes out of the SPI port(将字节数组写入SPI端口)
void spi_write_array(u8 len, u8 writeData[]);

//Generic function to write BF8915A commands. Function calculated PEC for tx_cmd data(编写BF8915A命令的通用函数。函数为cmd数据计算了检错码)
void BF8915A_cmd(u8 tx_cmd[2]);

//Generic function to write BF8915A commands and write payload data. Function calculated PEC for tx_cmd data(写BF8915A命令和写有效载荷数据的通用函数。tx_cmd数据的函数计算检错码)
void BF8915A_write(u8 total_ic, u8 tx_cmd[2], u8 write_data[]);//Writes an array of data to the daisy chain	(将一组数据写入菊花链)

//Issues a command onto the daisy chain and reads back 6*total_ic data in the rx_data array(向菊花链发出命令，回读rx_data数组中的6倍IC(8915A)数据)
u8 BF8915A_read(u8 total_ic, u8 tx_cmd[2], u8 *rx_data);

//Calculates and returns the CRC15(计算并返回15位循环冗余校验值)
u16 Bf8915pec15_calc(u8 len, u8 *pecData);

//Write the BF8915A CFGRA(写BF8915A的配置寄存器组A)
void BF8915A_wrcfga(u8 total_ic, ConfigA_TypeDef configAData[]);

//Write the BF8915A CFGRB(写BF8915A的配置寄存器组B)
void BF8915A_wrcfgb(u8 total_ic, ConfigB_TypeDef configBData[]);

//Write the BF8915A CFGRC(写BF8915A的配置寄存器组C)
void BF8915A_wrcfgc(u8 total_ic, ConfigC_TypeDef configCData[]);

//Write the BF8915A CFGRD(写BF8915A的配置寄存器组D)
void BF8915A_wrcfgd(u8 total_ic, ConfigD_TypeDef configDData[]);

//Reads the BF8915A CFGRA register(读BF8915A的配置寄存器组A)
u8 BF8915A_rdcfga(u8 total_ic, ConfigA_TypeDef configAData[]);

//Reads the BF8915A CFGRB register(读BF8915A的配置寄存器组B)
u8 BF8915A_rdcfgb(u8 total_ic, ConfigB_TypeDef configBData[]);

//Reads the BF8915A CFGRC register(读BF8915A的配置寄存器组C)
u8 BF8915A_rdcfgc(u8 total_ic, ConfigC_TypeDef configCData[]);

//Reads the BF8915A CFGRD register(读BF8915A的配置寄存器组D)
u8 BF8915A_rdcfgd(u8 total_ic, ConfigD_TypeDef configDData[]);

// Reads the raw cell voltage register data(读取原始电池电压寄存器数据)
void BF8915A_rdcv_reg(u8 reg, u8 total_ic, u8 *cellData);

//reads BF8915A GPIO registers(读取BF8915A的GPIO寄存器)
void BF8915A_rdgv_reg(u8 reg, u8 total_ic, u8 *gpioData);

//Reads BF8915A stat registers(读取BF8915A状态寄存器)
void BF8915A_rdstat_reg(u8 reg, u8 total_ic, u8 *statData);

//ADCV CMD(电池电压ADC转换指令)
void BF8915A_adcv(u8 CLKSEL, u8 OSR, u8 DISCP, u8 CSEL);

//CVST CMD(电池电压自测试命令)
void BF8915A_cvst(u8 CLKSEL, u8 OSR, u8 SFTEST);

//ADOL CMD(电池重叠单元测量指令)
void BF8915A_adol(u8 CLKSEL, u8 OSR, u8 DISCP);

//ADGP CMD(GPIO ADC转换指令)
void BF8915A_adgp(u8 CLKSEL, u8 OSR, u8 GSEL);

//GPST CMD(GPIO输入自测试指令)
void BF8915A_gpst(u8 CLKSEL, u8 OSR, u8 SFTEST);

//ADSTAT CMD(测量内部设备参数指令)
void BF8915A_adstat(u8 CLKSEL, u8 OSR, u8 STSEL);

//STATST CMD(自测试内部设备参数指令)
void BF8915A_statst(u8 CLKSEL, u8 OSR, u8 SFTEST);

//ADCVGP CMD(测量电池电压和GPIO输入指令)
void BF8915A_adcvgp(u8 CLKSEL, u8 OSR, u8 DISCP);

//MNT CMD(监测指令)
void BF8915A_mnt(u8 CLKSEL, u8 OSR, u8 MNMOD, u8 STRMN, u8 BLEN);

//STRBL CMD(被动均衡指令)
void BF8915A_strbl(u8 CLKSEL, u8 OSR, u8 OESEL, u8 BLEN);

//Clear Cell Register(清除单元寄存器)
void BF8915A_clrcell(void);

//Clear GPIO Register(清除GPIO寄存器)
void BF8915A_clrgp(void);

//Clear STATE Register(清除状态寄存器)
void BF8915A_clrstat(void);

//PLADC CMD(轮询ADC转换状态指令)
u8 BF8915A_pladc(void);

//soft reset(软件复位)
void BF8915A_softrst(void);

//Helper function that parses voltage measurement registers(解析电压测量寄存器的辅助函数)
u8 parse_cells(u8 current_ic, u8 cell_reg, u8 cell_data[], u16 *cell_codes, u8 *ic_pec);

//Reads and parses the BF8915A cell voltage registers(读取和解析BF8915A电池电压寄存器)
u8 BF8915A_rdcv(u8 reg, u8 total_ic, CellVoltage_TypeDef *cellVoltage);

//Reads and parses the BF8915A GPIO registers(读取和解析BF8915A的GPIO寄存器)
u8 BF8915A_rdgv(u8 reg, u8 total_ic, GPIOReg_TypeDef *gpioVoltage);

//Reads and parses the BF8915A stat registers(读取和解析BF8915A状态寄存器)
u8 BF8915A_rdstat(u8 reg, u8 total_ic, StateReg_TypeDef *stateReg);

#endif
