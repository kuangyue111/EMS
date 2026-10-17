/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : LoadApp.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 在线升级通信包模块
** @Instructions : BOOT升级程序使用
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "S32K146.h"
#include "osif.h"
#include "system_S32K146.h"
#include "interrupt_manager.h"
#include "ftm_common.h"
#include "lpspiCom1.h"
#include "flexio_spi1.h"
#include "flexcan_driver.h"
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "SST26VF064.h"
#include "LoadApp.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sFlashBootPara[eBOOT_PARA_FLASH_ID_LEN] = {0};    /*在线升级BOOT参数用Flash保存*/

u16 gLoadDevAddr = 0xff;                                    /*设备地址*/

u8 gLoadWorkModel = 0;                                      /*工作模式[0:应用正常模式(APP) 1:远程传输模式(APP) 2:应用缺失模式(BOOT) 3:本地升级模式(BOOT) 4:远程升级模式(BOOT)]*/

/*0:通信路由 1:握手成功 2:握手失败 3:擦除成功 4:擦除失败 5:开始升级 6:正在传输 7:传输成功 8:传输丢帧 9:写入成功 10:写入失败 11:等待安装
 *12:无源程序 13:格式化成功 14:格式化失败 15:正在安装 16:安装成功 17:安装失败 18:等待备份 19:正在备份 20:备份成功 21:备份失败 22:无源程序*/
u8 gLoadWorkState = 0;                                      /*工作状态[0,22]*/

u8 gFlashEraseStep = 0xff;                                  /*擦除APP程序区步骤 [0,9]:擦除运行区  [10,19]:擦除备份区 */

u8  gFlashAppData[LOAD_APP_BUF_LEN] = {0};                  /*用于存储接收到的烧写应用程序一段数据*/
u32 gLoadStartAddr = 0;                                     /*接收到的段起始地址*/
u16 gLoadSectionCRC = 0;                                    /*接收到的数据的CRC*/
u16 gLoadSectionLen = 0;                                    /*接收到的段长度*/
u32 gHisFlashAddr = 0;                                      /*已烧写的段地址*/

u8 gRemLoadState = 0;                                       /*远程升级状态[0,6]0:下载完成 1:下载失败 2:正在下载 3:空闲 4:安装失败 5:正在安装 6:安装完成*/
u32 gRemLoadVers = 0;                                       /*远程升级下载识别到的固件版本号*/
u32 gRemLoadIndex = 0;                                      /*远程升级已下载数据包序号(帧序号)*/
u32 gRemLoadSumByte = 0;                                    /*远程升级固件总字节数*/
u32 gRemLoadRcvByte = 0;                                    /*远程升级已接收固件字节数*/

#if(LOAD_CAN_NUM > 1)                                       /*多路CAN端口定义*/
const s_LoadRouteCfg sLoadCanRouteCfg[LOAD_CAN_NUM] =
{
    {eCAN0, eCANROUTE_UP},
    {eCAN1, eCANROUTE_DN},
    {eCAN2, eCANROUTE_UP}
};
#endif

flexio_spi_master_state_t flexio_spi_master_state;

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void LoadHandleCmdDeal(u16 type, u16 ecu, u8 *data, u8 canNum)
** @Input     : type:命令类型  ecu:设备地址 data:命令数据 canNum:通讯端口号
** @Output    : void
** @Function  : 在线升级握手处理
** @The notes :
**===============================================================================================*/
static void LoadHandleCmdDeal(u16 type, u16 ecu, u8 *data, u8 canNum);

/*=================================================================================================
** @Name      : static void LoadRemDownMsgDeal(u8 ecu, u8 *data, u8 len)
** @Input     : ecu:目标设备 data:下载通信数据 len:通信数据长度
** @Output    : void
** @Function  : 远程下载故障数据报文处理
** @The notes :
**===============================================================================================*/
static void LoadRemDownMsgDeal(u8 ecu, u8 *data, u8 len);

#if(LOAD_CAN_NUM > 1)
/*=================================================================================================
** @Name      : static u8 GetCanRouteType(u8 canNum)
** @Input     : canNum,CAN通道号
** @Output    : 0xff:未使用，1：向下 0:向上
** @Function  : 获取CAN端口通讯路由方向
** @The notes :
**===============================================================================================*/
static u8 GetCanRouteType(u8 canNum);

/*=================================================================================================
** @Name      : static void CanLoadMsgTransmist(t_CANMSG *msg, u8 canNum)
** @Input     : msg:通讯信息 canNum:CAN通道号
** @Output    : void
** @Function  : 转发在线升级CAN信息
** @The notes :
**===============================================================================================*/
static void CanLoadMsgTransmist(t_CANMSG *msg, u8 canNum);
#endif

/*=================================================================================================
** @Name      : static u32 BuildLoadAREMsgID(u8 type, u8 sDev, u8 sAddr)
** @Input     : type:报文类型 sDev:源设备类型[0:从控 1:高压模块 2:主控 3:堆控 0xff:所有设备] sAddr:源设备地址
** @Output    : 在线升级回复报文ID
** @Function  : 构建在线升级回复报文ID
** @The notes :
**===============================================================================================*/
static u32 BuildLoadAREMsgID(u8 type, u8 sDev, u8 sAddr);

/*=================================================================================================
 ** @Name      : static void ChangeWorkModeHook(u8 mode)
 ** @Input     : mode:目标工作模式
 ** @Output    : void
 ** @Function  : 切换在线升级工作模式处理
 ** @The notes : 根据当前所处程序选择切换或跳转
 **===============================================================================================*/
 static void ChangeWorkModeHook(u8 mode);

 /*=================================================================================================
 ** @Name      : static void IntoCanLoadModelHook(void)
 ** @Input     : void
 ** @Output    : void
 ** @Function  : 请求进入程序烧录状态处理
 ** @The notes : 擦除内部FLASH或备份FLASH
 **===============================================================================================*/
 static void IntoCanLoadModelHook(void);

 /*=================================================================================================
 ** @Name      : static void CanLoaderOkHook(void)
 ** @Input     : void
 ** @Output    : void
 ** @Function  : 烧录完成处理
 ** @The notes : 记录运行程序成功或备份程序成功
 **===============================================================================================*/
 static void CanLoaderOkHook(void);

/*=================================================================================================
** @Name      : static void EnterBootPro(void)
** @Input     : void
** @Output    : void
** @Function  : 跳转进入BOOT程序
** @The notes :
**===============================================================================================*/
static void EnterBootPro(void);

/*=================================================================================================
** @Name      : static void EnterAppPro(void)
** @Input     : void
** @Output    : void
** @Function  : 跳转进入应用程序
** @The notes :
**===============================================================================================*/
static void EnterAppPro(void);

/*=================================================================================================
** @Name      : static u8 CheckFlashAppProCode(u8 sort)
** @Input     : sort:校验FLASH类型 0:内部FLASH 1:备份FLASH 2:远程下载FLASH
** @Output    : TRUE:正常 FALSE:异常
** @Function  : 检测Flash中应用程序代码
** @The notes :
**===============================================================================================*/
static u8 CheckFlashAppProCode(u8 sort);

/*=================================================================================================
** @Name      : static u16 CalcCodeCRC(u8 *data, u16 len)
** @Input     : data:代码段数据指针起始地址, length,段数据长度
** @Output    : CRC校验码
** @Function  : 计算代码段校验码
** @The notes :
**===============================================================================================*/
static u16 CalcCodeCRC(u8 *data, u16 len);

/*=================================================================================================
** @Name      : static void ClrLoadAppBuffData(void)
** @Input     : void
** @Output    : void
** @Function  : 清除在线升级应用程序接收缓冲区数据
** @The notes :
**===============================================================================================*/
static void ClrLoadAppBuffData(void);

/*=================================================================================================
** @Name      : static u8 CheckBootHWParaAvail(void)
** @Input     : void
** @Output    : 检测结果 TRUE:正常 FALSE:异常(需要重写FLASH)
** @Function  : 检测BOOT程序硬件参数有效性
** @The notes : 仅检测全局变量是否有效(未写FLASH)
**===============================================================================================*/
static u8 CheckBootHWParaAvail(void);

/*=================================================================================================
** @Name      : static void CfgBootHWParaHandle(u8 index, u16 data)
** @Input     : index:配置索引 data:配置目标值
** @Output    : void
** @Function  : 接收BOOT程序配置硬件参数处理
** @The notes : 记录到BOOT参数区和APP参数区
**===============================================================================================*/
static void CfgBootHWParaHandle(u8 index, u16 data);

/*=============================================================================================
** @Name      : static u8 LoadCopyFlashEraseSector(u8 fstSec, u8 endSec)
** @Input     : fstSec:块内起始扇区号(包括) endSec:块内结束扇区号(不包括)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 在线升级擦除内部或外部FLASH备份程序扇区
** @The notes : 按扇区擦除FLASH
**===========================================================================================*/
static u8 LoadCopyFlashEraseSector(u8 fstSec, u8 endSec);

/*=============================================================================================
** @Name      : static u8 LoadCopyFLASHWriteByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数[1, 1024](必须是8的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 在线升级按字节方式向内部或外部FLASH写入备份程序(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数
**===========================================================================================*/
static u8 LoadCopyFlashWriteByte(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : static u8 LoadCopyFlashReadByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入FLASH地址[0,1048575](1M) *pData:读取到的数据指针 len:读取数据的个数[1, 1024]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 在线升级按字节方式从内部或外部FLASH读取备份程序(8位)
** @The notes : 驱动内部没有数据缓冲,要求读取过程数据指针对应的数据不能改变
**===========================================================================================*/
static u8 LoadCopyFlashReadByte(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : static u8 LoadDownFlashEraseSector(u8 fstSec, u8 endSec)
** @Input     : fstSec:块内起始扇区号(包括) endSec:块内结束扇区号(不包括)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 在线升级擦除内部或外部FLASH下载程序扇区
** @The notes : 按扇区擦除FLASH
**===========================================================================================*/
static u8 LoadDownFlashEraseSector(u8 fstSec, u8 endSec);

/*=============================================================================================
** @Name      : static u8 LoadDownFlashWriteByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数[1, 1024](必须是8的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 远程升级按字节方式向外部FLASH写入下载程序(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数
**===========================================================================================*/
static u8 LoadDownFlashWriteByte(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : static u8 LoadDownFlashReadByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入FLASH地址[0,1048575](1M) *pData:读取到的数据指针 len:读取数据的个数[1, 1024]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 远程升级按字节方式从外部FLASH读取下载程序(8位)
** @The notes : 驱动内部没有数据缓冲,要求读取过程数据指针对应的数据不能改变
**===========================================================================================*/
static u8 LoadDownFlashReadByte(u32 addr, u8 *pData, u32 len);

/*=================================================================================================
** @Name      : static u8 LoadDownFlashReadCodeAddrLen(u16 index, u32 *rAddr, u32 *wAddr, u16 *len)
** @Input     : index:段索引号 *rAddr:读段代码起始地址(下载区FLASH) *wAddr:写段代码起始地址(运行区FLASH) *len:段代码长度
** @Output    : TRUE:读取成功 FALSE:读取失败
** @Function  : 读取下载FLASH区中程序段代码起始地址和段代码长度
** @The notes : 各段代码烧写地址和长度及代码流均记录在外部下载FLASH区
**===============================================================================================*/
static u8 LoadDownFlashReadCodeAddrLen(u16 index, u32 *rAddr, u32 *wAddr, u16 *len);

/*=================================================================================================
** @Name      : static void TimeDelay(u32 time)
** @Input     : time:循环次数
** @Output    : void
** @Function  : 延时处理函数
** @The notes :
**===============================================================================================*/
static void TimeDelay(u32 time);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void BootLoaderInit(void)
** @Input     : void
** @Output    : void
** @Function  : 在线升级模块初始化
** @The notes : BOOT和APP有区别
**===============================================================================================*/
void BootLoaderInit(void)
{
    u16 ecu = 0;
    u16 flag = 0;
    u16 runFlag = 0;
    u16 tryFlag = 0;
    //u8 eepAppPara[eAPP_PARA_SEC2_ID_LEN] = {0};

    //清除应用程序代码接收缓存区
    BSPWatchDogOutput();
    ClrLoadAppBuffData();

    gLoadStartAddr = 0;
    gLoadSectionLen = 0;
    gLoadSectionCRC = 0xffff;

    /*******************************读Flash中BOOT参数初始化********************************/
    //读BOOT参数扇区Flash中参数
    BSPWatchDogOutput();
    (void)BSPINTFLASHReadByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);

    /*******************************设备地址初始化****************************************/
    //参数小端模式存储(低字节在前)
    flag = ((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID];
    ecu = sFlashBootPara[eBOOT_EDV_ECU_L_ID];

    //Flash中BOOT参数有配置标志且设备地址有效
    if((flag == LOAD_APP_PARA_FLAG) && (ecu < 0xfe))
    {
        //默认BOOT参数区域设备地址首先有效(应用程序会将该参数拷贝到应用程序参数区域后将该参数擦除)
    	gLoadDevAddr = ecu;
    }
    //BOOT参数扇区Flash无参数配置
    else
    {
    	/*APP会将应用参数设备地址拷贝到BOOT参数区域,BOOT不用读APP参数
        //读APP中通参数中设备地址
        BSPWatchDogOutput();
        (void)BSPINTEEPReadByte(APP_PARA_GEN_EEP_ADDR, eepAppPara, eAPP_PARA_SEC2_ID_LEN);

        //参数小端模式存储(低字节在前)
        flag = ((u16)flashAppPara[eAPP_PARA_FLAG_H_ID] << 8) | flashAppPara[eAPP_PARA_FLAG_L_ID];

        //Flash有应用程序参数配置
        if(flag == LOAD_APP_PARA_FLAG)
        {
        	gLoadDevAddr = ((u16)flashAppPara[eAPP_DEV_ECU_H_ID] << 8) | flashAppPara[eAPP_DEV_ECU_L_ID];
        }
        //Flash无应用程序参数配置
        else
        {
            //使用默认值
        	gLoadDevAddr = 0xfe;
        }*/
        //使用默认值
    	gLoadDevAddr = 0xfe;
    }

    /*默认本地升级模式/通信路由状态*/
    BSPWatchDogOutput();
    gLoadWorkModel = eLoMod_NotApp;
    gLoadWorkState = eLoSta_NotProg;

    /*******************************根据BOOT参数进行状态和跳转初始化*****************************/
    //BOOT参数标志和程序可运行标志
    flag = ((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID];
    runFlag = ((u16)sFlashBootPara[eBOOT_APP_RUN_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_APP_RUN_FLAG_L_ID];

    //接收到的是从应用程序跳转过来的进入BOOT程序指令，则保持在BOOT程序
    if((flag == LOAD_APP_PARA_FLAG) && (0xff != sFlashBootPara[eBOOT_JUM_FLAG_MODE_ID]))
    {
        /*从应用程序进入BOOT特定工作状态*/
        if((eLoSta_WaitInst == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID])          //等待安装
        	|| (eLoSta_DoInstal == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID])      //正在安装
        	|| (eLoSta_WaitCopy == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID])      //等待备份
        	|| (eLoSta_Copying == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID])       //正在备份
        	|| (eLoSta_Backing == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID]))      //正在回滚
        {
            /*显示特定状态状态*/
        	gLoadWorkState = sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID];

            /*正在安装状态*/
        	if(eLoSta_DoInstal == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID])
        	{
                /*正在安装状态*/
                gRemLoadState = 5;
        	}
        }
        /*非指定状态*/
        else
        {
            /*默认等待握手状态*/
        	gLoadWorkState = eLoSta_HandWait;
        }

        /*从应用程序进入BOOT远程升级模式*/
        if(eLoMod_RemBoot == sFlashBootPara[eBOOT_JUM_FLAG_MODE_ID])
        {
        	gLoadWorkModel = eLoMod_RemBoot;
        }
        /*从应用程序进入BOOT本地升级模式*/
        else
        {
        	gLoadWorkModel = eLoMod_LocBoot;
        }

        //从应用程序跳转进入BOOT程序后清除跳转模式标志(保留原始工作状态)防止重启再次进入BOOT
        sFlashBootPara[eBOOT_JUM_FLAG_MODE_ID] = 0xff;
        //sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID] = 0xff;

        /*检测BOOT硬件参数有效性*/
        (void)CheckBootHWParaAvail();

        (void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
        BSPWatchDogOutput();

        (void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
        BSPWatchDogOutput();
    }
    //有应用程序运行标志且Flash应用程序区域有代码
    else if((flag == LOAD_APP_PARA_FLAG) && (runFlag == LOAD_APP_RUN_FLAG) && (TRUE == CheckFlashAppProCode(0)))
    {
        //直接跳转进入应用程序
    	BSPWatchDogOutput();

       //进入应用程序
        EnterAppPro();

    }
    //应用程序运行标志或检测Flash应用程序区域代码不正确
    else
    {
        //尝试跳转进入一次，并且写入尝试跳转标志
        tryFlag = ((u16)sFlashBootPara[eBOOT_APP_TRY_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_APP_TRY_FLAG_L_ID];

        //未记录尝试跳转标志则尝试跳转
        if((flag != LOAD_APP_PARA_FLAG) || (tryFlag != LOAD_APP_TRY_FLAG))
        {
            //写入参数标志和已尝试跳转标志
            sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
            sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

            sFlashBootPara[eBOOT_APP_TRY_FLAG_L_ID] = LOAD_APP_TRY_FLAG & 0xff;
            sFlashBootPara[eBOOT_APP_TRY_FLAG_H_ID] = (LOAD_APP_TRY_FLAG >> 8) & 0xff;

        	/*无BOOT硬件参数标志*/
        	if(flag != LOAD_APP_PARA_FLAG)
			{
            	/*检测BOOT硬件参数有效性*/
        		(void)CheckBootHWParaAvail();
			}

            (void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
            BSPWatchDogOutput();

            (void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
            BSPWatchDogOutput();

            //延时再跳转
            TimeDelay(100);
            BSPWatchDogOutput();

            //尝试跳转至应用程序
            EnterAppPro();
        }
        //已尝试跳转失败则不再跳转
        else
        {
        	/*下载FLASH有程序*/
        	if(LOAD_APP_DOWN_FLAG == sFlashBootPara[eBOOT_APP_DOWN_FLAG_ID])
        	{
        		/*进行远程模式安装程序*/
        		gLoadWorkModel = eLoMod_RemBoot;
            	gLoadWorkState = eLoSta_WaitInst;

	            /*安装失败状态*/
	            gRemLoadState = 4;
        	}
        	/*备份FLASH有程序*/
        	else if(LOAD_APP_COPY_FLAG == sFlashBootPara[eBOOT_APP_COPY_FLAG_ID])
        	{
        		/*进行远程模式安装程序*/
        		gLoadWorkModel = eLoMod_LocBoot;
            	gLoadWorkState = eLoSta_CopySucc;
        	}
        	BSPWatchDogOutput();

        	/*检测BOOT硬件参数有效性(异常)*/
        	if(FALSE == CheckBootHWParaAvail())
        	{
        		/*参数重写FLASH*/
                (void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
                BSPWatchDogOutput();

                (void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
                BSPWatchDogOutput();
        	}
        }
    }

    BSPWatchDogOutput();
}

/*=================================================================================================
** @Name      : void CanRcvLoadMsgHandleAPI(t_CANMSG *msg, u8 canNum)
** @Input     : msg:接收到的报文 canNum:接收CAN通道号
** @Output    : void
** @Function  : 在线升级模块CAN报文接收处理
** @The notes : CAN接收回调函数调用
**===============================================================================================*/
void CanRcvLoadMsgHandleAPI(t_CANMSG *msg, u8 canNum)
{
    u16 i = 0;          //临时变量
    u16 dir = 0;        //帧方向 0:上行信息 1:下行信息
    u16 fun = 0;        //功能码
    u16 type = 0;       //信息类型
    u16 dDev = 0;       //目标设备类型
    u16 dAddr = 0;      //目标设备地址
    u16 handletype = 0; //握手类型
    u16 handleCmd = 0;  //命令类型
    u16 dataIndex = 0;  //数据的序号
    u16 datalength = 0; //数据长度
    u16 overflag = 0;   //段结束还是整体烧录结束
    u16 flashResult = 0;//flash烧录结果保存
    u32 flashWrAddr = 0;//flash读写地址

    dAddr = msg->ID & 0xff;                 //目标设备地址
    dDev = (msg->ID >> 8) & 0xff;           //目标设备类型
    type = (msg->ID >> 16) & 0xff;          //信息类型
    dir = (msg->ID >> 24) & 0x01;           //传输方向
    fun = (msg->ID >> 25) & 0x0f;           //功能码

    //非CAN升级信息不处理不转发
    if((fun < LOAD_REQ_FUN) || (fun > LOAD_RUP_FUN)
        || (type < eSEARCHDEV_MSG) || (type > eREMFIN_MSG))
    {
        return;
    }

    //多路CAN口通讯
    #if(LOAD_CAN_NUM > 1)
    //上行信息(只转发不处理)
    if(dir == 0)
    {
        //来自下层上传通讯口
        if(GetCanRouteType(canNum) == eCANROUTE_DN)
        {
            //向上转发信息
            CanLoadMsgTransmist(msg, canNum);
        }
        return;
    }

    /*下行信息 来自上层通信口*/
    if(GetCanRouteType(canNum) == eCANROUTE_UP)
    {
        //广播下行信息转发并处理
        if(dAddr == 0xff)
        {
            //向下转发信息
            CanLoadMsgTransmist(msg, canNum);
        }
        //非本设备下行信息
        else if(dAddr != gLoadDevAddr)
        {
            //仅向下转发此条信息
            CanLoadMsgTransmist(msg, canNum);
            return;
        }
    }
    /*下行信息 来自下层通信口(不转发)*/
    else
    {
        //非本设备下行信息
    	if((dAddr != gLoadDevAddr) && (dAddr != 0xff))
        {
            //不处理本条信息
            return;
        }
    }
    //非多路CAN通讯口
    #else
    //上行信息(不处理)
    if(dir == 0)
    {
        return;
    }

    //下行信息但不是本设备信息也不是广播信息
    if((dAddr != gLoadDevAddr) && (dAddr != 0xff))
    {
        //不处理本条信息
        return;
    }
    #endif

    //非自身设备类型
    if((dDev != (LOAD_DEV_ID % 10)) && (dDev != 0xff))
    {
        //不处理本条信息
        return;
    }

    //判断报文功能类型
    switch(type)
    {
        //广播查询设备报文
        case eSEARCHDEV_MSG:
            /*回复自身的设备信息和状态信息，包括芯片类型/通讯口/设备识别码/软件版本号/工作模式/工作状态*/
            //msg->ID = BuildLoadAREMsgID(eSEARCHDEV_MSG, dDev, (u8)gLoadDevAddr);
            msg->ID = BuildLoadAREMsgID(eSEARCHDEV_MSG, (LOAD_DEV_ID % 10), (u8)gLoadDevAddr);
            msg->Data[0] = (u8)eNXP_S32K146;   /*芯片类型*/
            msg->Data[1] = canNum;
            msg->Data[2] = gLoadWorkModel;
            msg->Data[3] = gLoadWorkState;
            msg->Data[4] = (u8)(LOAD_DEV_ID & 0xff);
            msg->Data[5] = (u8)(LOAD_DEV_ID >> 8 & 0xff);
            msg->Data[6] = (u8)(LOAD_SW_VERSION & 0xff);
            msg->Data[7] = (u8)(LOAD_SW_VERSION >> 8 & 0xff);
            msg->Len = 8;
            (void)BSPCANSendMsg((e_CANNUM)canNum, msg);
            break;

		//本地升级握手报文
		case eHANDLE_MSG:
		//远程升级握手报文
		case eREMHAND_MSG:
            handletype = msg->Data[0];        /*握手类型 1:上位机请求 2:下位机回复*/
            handleCmd = msg->Data[1];         /*握手命令*/

            //非上位机请求本设备握手不处理
            if(handletype != 1)
            {
                return;
            }

            //握手命令处理
            LoadHandleCmdDeal(handleCmd, dAddr, msg->Data, canNum);
            break;

        //启动本地升级传输报文(发送起始段信息)
        case eLOADSTART_MSG:
            /*非本地升级模式或非传输状态,不接收处理此条信息*/
            if((eLoMod_LocBoot != gLoadWorkModel)           /*非本地升级模式*/
                || ((eLoSta_FormSucc != gLoadWorkState)     /*非本地格式化成功(用于flash刚擦除完，第一次接收到段信息)*/
                && (eLoSta_TranSucc != gLoadWorkState)      /*非传输正常(用于上一帧刚传输完成，接收烧录NextSection)*/
                && (eLoSta_WritSucc != gLoadWorkState))     /*非写入正常(用于上一段刚烧录完成，接收烧录NextSection)*/
                )
            {
                return;
            }

            /*初始化数据域*/
            gHisFlashAddr = 0;
            ClrLoadAppBuffData();
            gLoadWorkState = eLoSta_LoaStart;              /*段烧录启动烧写*/

            /*记录段起始地址，段数据长度及CRC，重新设置接收到的数据长度*/
            gLoadStartAddr = (u32)msg->Data[0] | ((u32)msg->Data[1] << 8) | ((u32)msg->Data[2] << 16) | ((u32)msg->Data[3] << 24);
            gLoadSectionLen = (u16)msg->Data[4] | ((u16)msg->Data[5] << 8);
            gLoadSectionCRC = (u16)msg->Data[6] | ((u16)msg->Data[7] << 8);
            break;

        //下发本地升级数据报文
        case eLOADDATA_MSG:
            /*非本地升级模式或非传输状态,不接收处理此条信息*/
        	if((eLoMod_LocBoot != gLoadWorkModel)           /*非本地升级模式*/
				|| ((eLoSta_LoaStart != gLoadWorkState)     /*开始传输(用于刚接收段信息，第一次接收到数据流)*/
				&& (eLoSta_LoaTrans != gLoadWorkState)      /*正在传输(用于正在传输数据流)*/
				&& (eLoSta_TranSucc != gLoadWorkState)      /*传输成功(用于传输数据流到下一段)*/
				&& (eLoSta_TranFail != gLoadWorkState))     /*传输失败(用于传输数据流丢帧后重发)*/
				)
            {
                return;
            }

        	/*非传输失败则转为正在传输(失败后会重发)*/
            //if(eLoSta_TranFail != gLoadWorkState)
            {
            	gLoadWorkState = eLoSta_LoaTrans;
            }

            /*获取CAN信息中包括的数据序号、长度及数据*/
            dataIndex = (u16)msg->Data[0] | (((u16)msg->Data[1] & 0x0f) << 8);
            datalength = (u16)(((u16)msg->Data[1] & 0xf0) >> 4);

            /*发送的数据不合法,发送的长度不合法*/
            if((datalength == 0) || (datalength > 6)
                || (dataIndex + datalength > LOAD_APP_BUF_LEN))
            {
            	gLoadWorkState = eLoSta_TranFail;
                return;
            }

            /*存储接收到的flash数据*/
            for(i = 0; i < datalength; i++)
            {
                gFlashAppData[dataIndex + i] = msg->Data[2 + i];
            }
            break;

        //本地升级段传输结束校验报文
        case eLOADFIN_MSG:
        	/*若非正在升级，不接收处理此条信息*/
			if((eLoMod_NorApp == gLoadWorkModel)
				|| (eLoMod_NotApp == gLoadWorkModel)        /*非升级模式*/
				|| ((eLoSta_LoaTrans != gLoadWorkState)     /*正在传输(用于正在传输数据流)*/
				&& (eLoSta_TranSucc != gLoadWorkState)))    /*传输成功(用于传输数据流到下一段)*/
            {
                return;
            }

            overflag = msg->Data[0]; /*烧录结束标志  */

            /*接收到的数据长度与实际的一致且校验码OK，设置状态为"传输成功",否则设置为"传输失败"*/
            if(CalcCodeCRC(gFlashAppData, gLoadSectionLen) == gLoadSectionCRC)
            {
                /*
                if(gLoadStartAddr >= 0x7FFF10)    //中断向量地址存储区域，转移到DBC0->BDFF之间
                {
                    for(i = 0; i < gLoadSectionLen; i++)
                    {
                        gFlashAppIntData[gLoadStartAddr - 0x7fff10 + i] = gFlashAppData[i];
                    }

                    flashResult = 1;
                }
                else
                {
                    flashResult = BSPINTFLASHWriteByte(gLoadStartAddr, gFlashAppData, gLoadSectionLen);
                }

                if(flashResult == 1)
                {
                    //若为最后一段，则显示"烧写成功",若为段信息，则显示"传输成功"
                    if(overflag == 2)
                    {
                    	//本地升级
                    	if(eLoMod_LocBoot == gLoadWorkModel)
                    	{
                            interResult = BSPINTFLASHWriteByte(0x7fd810, gFlashAppIntData, 240);
                    		//等待备份
                    		gLoadWorkState = eLoSta_WaitCopy;
                    	}
                    	//远程升级
                    	else
                    	{
                            interResult = BSPINTFLASHWriteByte(0x7fd810, gFlashAppIntData, 240);
                    		//等待安装
                    		gLoadWorkState = eLoSta_WaitInst;
                    	}
                		//更新记录应用程序状态
                        CanLoaderOkHook();
                    }
                    else
                    {
                        gLoadWorkState = eLoSta_WritSucc;
                    }
                }
                else
                {
                    gLoadWorkState = eLoSta_WritFail;
                }
                */

                //为KEAZ128 Flash地址
                //if(gLoadStartAddr >= LOAD_MCU_BASE_FLASH_ADDR)
                //{
                    //转化为读写Flash相对地址(FLASH接口读写地址)
                    flashWrAddr = gLoadStartAddr - BOOT_MCU_BASE_FLASH_ADDR;

                    //不在APP程序存放地址内
                    if((flashWrAddr < APP_PRO_BLOCK1_FLASH_ADDR)
                        || ((flashWrAddr + gLoadSectionLen) > APP_PRO_BLOCK1_FIN_ADDR))
                    {
                        //烧写失败
                        flashResult = 0;
                    }
                    //烧写地址合法
                    else
                    {
                        //该段未烧写
                        if(flashWrAddr > gHisFlashAddr)
                        {
                            //烧写本段并记录已烧写
                            gHisFlashAddr = flashWrAddr;

							/*内部FLASH写长度必须是8的倍数*/
							// need to append post-staffing uint16_t
							while((gLoadSectionLen & 0x000007) != 0)
							{
								gFlashAppData[gLoadSectionLen] = 0xff;
								gLoadSectionLen++;
							}

                            /*本地升级*/
							if(eLoMod_LocBoot == gLoadWorkModel)
							{
								/*写入内部FLASH*/
								flashResult = BSPINTFLASHWriteByte(flashWrAddr, gFlashAppData, gLoadSectionLen);
							}
							/*远程升级*/
							else
							{
			                    //转化为相对偏移地址(相对应用程序区起始地址)
			                    flashWrAddr = flashWrAddr - APP_PRO_BLOCK1_FLASH_ADDR;

								/*写入备份FLASH*/
								flashResult = LoadCopyFlashWriteByte(APP_PRO_INTCOPY_FLASH_ADDR + flashWrAddr, gFlashAppData, gLoadSectionLen);
							}

                            BSPWatchDogOutput();
                        }
                        //该段已烧写(上位机未接到回复并重发)
                        else
                        {
                            //直接认为烧写成功
                            flashResult = 1;
                        }
                    }
                //}
                //非KEAZ128 Flash地址
                //else
                //{
                //    flashResult = 0;
                //}

                //写Flash成功
                if(flashResult == 1)
                {
                    //若为最后一段，则显示"烧写成功",若为段信息，则显示"传输成功"
                    if(2 == overflag)
                    {
                    	/*本地升级*/
                    	if(eLoMod_LocBoot == gLoadWorkModel)
                    	{
                    		/*等待备份*/
                    		gLoadWorkState = eLoSta_WaitCopy;
                    	}
                    	/*远程升级*/
                    	else
                    	{
                    		/*等待安装*/
                    		gLoadWorkState = eLoSta_WaitInst;
                    	}
                		/*更新记录应用程序状态*/
                        CanLoaderOkHook();
                    }
                    //非最后一段
                    else
                    {
                    	/*写入成功*/
                    	gLoadWorkState = eLoSta_WritSucc;
                    }
                }
                //写Flash失败
                else
                {
                	/*写入失败*/
                	gLoadWorkState = eLoSta_WritFail;
                }
            }
            /*校验码不对*/
            else
            {
            	/*传输丢帧*/
            	gLoadWorkState =  eLoSta_TranFail;
            }
            break;

		//远程升级下载报文
		case eREMDATA_MSG:
			/*非远程模式握手成功状态,不接收处理此条信息*/
			if((eLoMod_RemBoot != gLoadWorkModel)           /*非远程升级模式*/
				|| ((eLoSta_ErasSucc != gLoadWorkState)     /*非远程擦除成功(用于flash刚擦除完，第一次接收到段信息)*/
				&& (eLoSta_TranSucc != gLoadWorkState)      /*非传输正常(用于上一帧刚传输完成，接收烧录NextSection)*/
				&& (eLoSta_WritSucc != gLoadWorkState)      /*非写入正常(用于上一段刚烧录完成，接收烧录NextSection)*/
				&& (eLoSta_LoaStart != gLoadWorkState)      /*开始传输(用于刚接收段信息，第一次接收到数据流)*/
				&& (eLoSta_LoaTrans != gLoadWorkState)      /*正在传输(用于正在传输数据流)*/
				&& (eLoSta_TranFail != gLoadWorkState))     /*传输失败(用于传输数据流丢帧后重发)*/
				)
			{
				return;
			}

			/*非传输失败则转为正在传输*/
			//if(eLoSta_TranFail != gLoadWorkState)
			{
				gLoadWorkState = eLoSta_LoaTrans;
			}

			/*对报文进行解析*/
			LoadRemDownMsgDeal(dDev, msg->Data, msg->Len);

			/*返回当前状态信息(应用程序不支持传输速度过快)*/
			//CanSendRemLoadMsgAPI();
			break;

		//远程升级完成报文
		case eREMFIN_MSG:
			/*非远程模式正在传输状态,不接收处理此条信息*/
			if((eLoMod_RemBoot != gLoadWorkModel)           /*非远程升级模式*/
				|| ((eLoSta_LoaTrans != gLoadWorkState)     /*非正在传输(用于正在传输数据流)*/
				&& (eLoSta_TranSucc != gLoadWorkState)      /*非传输正常(用于上一帧刚传输完成，接收烧录NextSection)*/
				&& (eLoSta_WritSucc != gLoadWorkState))     /*非写入正常(用于上一段刚烧录完成，接收烧录NextSection)*/
				)
			{
				return;
			}

			/*缓存最后下载数据未写FLASH*/
			if(0 != gLoadSectionLen)
			{
				/*将数据写FLASH(写FLASH失败)*/
				if(FALSE == LoadDownFlashWriteByte(gLoadStartAddr, gFlashAppData, gLoadSectionLen))
				{
					/*下载失败*/
					gRemLoadState = 1;

					/*写失败*/
					gLoadWorkState = eLoSta_WritFail;
				}
				gLoadSectionLen = 0;
			}

			/*校验全部代码数据正常*/
			if((gLoadWorkState != eLoSta_WritFail)
				&& (TRUE == CheckFlashAppProCode(2)))
			{
				/*下载成功*/
				gRemLoadState = 0;

				/*等待安装*/
				gLoadWorkState = eLoSta_WaitInst;

				/*更新记录应用程序状态*/
				CanLoaderOkHook();
			}
			else
			{
				/*下载失败*/
				gRemLoadState = 1;

				/*传输失败*/
				gLoadWorkState = eLoSta_TranFail;
			}

			/*返回当前状态信息(应用程序不支持传输速度过快)*/
			//CanSendRemLoadMsgAPI();
			break;

        default:
            break;
    }
}

/*=================================================================================================
** @Name      : void CanSendRemLoadMsgAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 远程升级状态CAN报文发送接口(仅进入远程模式后发送)
** @The notes : 500ms周期
**===============================================================================================*/
void CanSendRemLoadMsgAPI(void)
{
	u16 data = 0;
	t_CANMSG msg;

	/*非远程升级模式不发送*/
	if((eLoMod_RemTra != gLoadWorkModel) && (eLoMod_RemBoot != gLoadWorkModel))
	{
		return;
	}

    /*远程升级状态报文*/
    msg.ID = BuildLoadAREMsgID(eREMSTATE_MSG, (LOAD_DEV_ID % 10), (u8)gLoadDevAddr);
    msg.Len = 8;

    /*下载数据包索引号只上传[0,4095],满4096则回头*/
    data = gRemLoadIndex % 4096;

    msg.Data[0] = (u8)(data & 0xff);
    msg.Data[1] = (gRemLoadState << 4) | ((data >> 8) & 0x0f);
    msg.Data[2] = gLoadWorkModel;
    msg.Data[3] = gLoadWorkState;
    msg.Data[4] = (u8)(gRemLoadVers & 0xff);
    msg.Data[5] = (u8)(gRemLoadVers >> 8 & 0xff);
    msg.Data[6] = (u8)(gRemLoadVers >> 16 & 0xff);
    msg.Data[7] = (u8)(gRemLoadVers >> 24 & 0xff);

    //每路CAN均发送
    for(data = 0; data < LOAD_CAN_NUM; data++)
    {
        (void)BSPCANSendMsg((e_CANNUM)data, &msg);
    }
}

/*=================================================================================================
** @Name      : void LoadAppFlashErasHandleAPI(void)
** @Input     : void
** @Output    : void
** @Function  : APP程序FLASH擦除处理
** @The notes : 擦除FLASH APP运行区或备份区,擦除区域较大时分次擦除(5ms周期调用)
**===============================================================================================*/
void LoadAppFlashErasHandleAPI(void)
{
	static u16 sFstSec = 0;
	static u16 sEndSec = 0;
	static u16 sSecNum = 0;

	switch(gFlashEraseStep)
	{
		/*计算参数*/
		case 0:
		case 10:
		case 20:
			//每次擦3个扇区(20K,扇区过多会导致复位)
			sSecNum = (u16)((APP_PRO_BLOCK1_FIN_ADDR - APP_PRO_BLOCK1_FLASH_ADDR) / FLASH_PAGE_SIZE);
			if(sSecNum > 3)
			{
				sEndSec = 3;
			}
			else
			{
				sEndSec = sSecNum;
			}
			sFstSec = 0;
			gFlashEraseStep++;
			break;

		/*擦除APP运行区*/
		case 1:
			BSPWatchDogOutput();
			if(TRUE == BSPINTFLASHEraseBlock(1, sFstSec, sEndSec))
			{
				//紧跟上次结束扇区继续擦除
				sFstSec = sEndSec;
				//擦到最后扇区
				if(sFstSec >= sSecNum)
				{
					/*非"正在安装"且非"正在回滚"状态(执行过程状态保持不变)*/
					if((eLoSta_DoInstal != gLoadWorkState) && (eLoSta_Backing != gLoadWorkState))
					{
						/*内部FLASH格式化成功*/
						gLoadWorkState = eLoSta_FormSucc;
					}
					gFlashEraseStep = 0xff;
				}
				else
				{
					//每次擦3个扇区(12K)
					sEndSec = sFstSec + 3;
					//擦到最后扇区
					if(sEndSec > sSecNum)
					{
						sEndSec = sSecNum;
					}
				}
			}
			else
			{
				/*内部FLASH格式化失败功*/
				gLoadWorkState = eLoSta_FormFail;
				gFlashEraseStep = 0xff;
			}
			break;

		/*擦除APP备份区*/
		case 11:
			BSPWatchDogOutput();
			if(TRUE == LoadCopyFlashEraseSector(sFstSec, sEndSec))
			{
				//紧跟上次结束扇区继续擦除
				sFstSec = sEndSec;
				//擦到最后扇区
				if(sFstSec >= sSecNum)
				{
					/*非"正在备份"(执行过程状态保持不变)*/
					if(eLoSta_Copying != gLoadWorkState)
					{
						/*备份FLASH擦除成功*/
						gLoadWorkState = eLoSta_ErasSucc;
					}
					gFlashEraseStep = 0xff;
				}
				else
				{
					//每次擦8个扇区(32K)
					sEndSec = sFstSec + 8;
					//擦到最后扇区
					if(sEndSec > sSecNum)
					{
						sEndSec = sSecNum;
					}
				}
			}
			else
			{
				/*备份FLASH擦除失败*/
				gLoadWorkState = eLoSta_ErasFail;
				gFlashEraseStep = 0xff;
			}
			break;

		/*擦除APP下载区*/
		case 21:
			BSPWatchDogOutput();
			if(TRUE == LoadDownFlashEraseSector(sFstSec, sEndSec))
			{
				//紧跟上次结束扇区擦除
				sFstSec = sEndSec;
				//擦到最后扇区
				if(sFstSec >= sSecNum)
				{
					/*下载FLASH擦除成功*/
					gLoadWorkState = eLoSta_ErasSucc;
					gFlashEraseStep = 0xff;

		            /*空闲状态*/
		            gRemLoadState = 3;
				}
				else
				{
					//每次擦8个扇区(32K)
					sEndSec = sFstSec + 8;
					//擦到最后扇区
					if(sEndSec > sSecNum)
					{
						sEndSec = sSecNum;
					}
				}
			}
			else
			{
				/*备份FLASH擦除失败*/
				gLoadWorkState = eLoSta_ErasFail;
				gFlashEraseStep = 0xff;
			}
			break;

		default:
			break;
	}
}

/*=================================================================================================
** @Name      : void LoadFlashProgCopyHandleAPI(void)
** @Input     : void
** @Output    : void
** @Function  : FLASH程序拷贝处理(只在BOOT程序"正在备份"或"正在安装"状态下进行处理)
** @The notes : 程序安装或备份,仅BOOT程序5ms周期调用
**===============================================================================================*/
void LoadFlashProgCopyHandleAPI(void)
{
	u16 i = 0;
	u32 addr = 0;
	u8 rdCrc[2] = {0};
	static u8 sState = 0;
	static u16 sTimes = 0;
	static u16 sSumLen = 0;
	static u16 sRemLen = 0;
	static u32 sRunAddr = 0;

    /*非"安装完成"或"回滚完成"或"正在安装"或"正在备份"或"正在回滚"状态*/
    if((eLoSta_InstSucc != gLoadWorkState) && (eLoSta_BackSucc != gLoadWorkState)
    	&& (eLoSta_DoInstal != gLoadWorkState) && (eLoSta_Copying != gLoadWorkState) && (eLoSta_Backing != gLoadWorkState))
    {
    	sTimes = 0;
    	sState = 0;
    	sRemLen = 0;
    	sRunAddr = 0;
    	return;
    }

    /*远程升级模式安装程序完成延迟1s直接跳转至APP*/
    /*本地升级模式回滚程序完成延迟1s直接跳转至APP*/
    if(((eLoMod_RemBoot == gLoadWorkModel) && (eLoSta_InstSucc == gLoadWorkState) && (gRemLoadIndex >= 100))
    	|| ((eLoMod_LocBoot == gLoadWorkModel) && (eLoSta_BackSucc == gLoadWorkState)))
    {
		/*等待1.5s*/
		if(sTimes >= 300)
		{
			sTimes = 0;
			/*跳转进入应用程序*/
			//EnterAppPro();
			/*复位程序*/
			EnterBootPro();
		}
		else
		{
			sTimes++;
		}
    }
    /*远程升级模式安装程序*/
    else if((eLoMod_RemBoot == gLoadWorkModel) && (eLoSta_DoInstal == gLoadWorkState))
    {
    	/*从下载FLASH拷贝程序到内部FLASH*/
    	switch(sState)
    	{
    		/*检测备份FLASH程序是否正常*/
    		case 0:
    			/*下载FLASH区域有程序标志有效*/
    			if(LOAD_APP_DOWN_FLAG == sFlashBootPara[eBOOT_APP_DOWN_FLAG_ID])
    			{
					/*下载FLASH上APP代码校验正常*/
					if(TRUE == CheckFlashAppProCode(2))
					{
	        			/*进入下一步*/
						sState = 1;
					}
					else
					{
	        			/*无源程序*/
	    	    		gLoadWorkState = eLoSta_NotProg;

	    	    		/*安装失败*/
						gRemLoadState = 4;
					}
    			}
    			else
    			{
        			/*无源程序*/
    	    		gLoadWorkState = eLoSta_NotProg;

    	    		/*安装失败*/
					gRemLoadState = 4;
    			}

	    		/*无源程序安装失败安装失败*/
    			if((eLoSta_NotProg == gLoadWorkState)
    				&& ((eLoSta_WaitInst == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID])
    				|| (eLoSta_DoInstal == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID])))
    			{
    				//清除等待安装工作状态
    			    sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID] = 0xff;

					BSPWatchDogOutput();
					(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);                           //擦除BOOT参数扇区

					BSPWatchDogOutput();
					(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);//将BOOT参数重新写Flash
    			}
    			break;

			/*开始格式化内部FLASH程序*/
			case 1:
				/*未开始进行内部FLASH格式化*/
				if(0xff == gFlashEraseStep)
				{
					/*安装前内部运行FLASH格式化处理*/
					IntoCanLoadModelHook();
				}

//				//格式化成功(分次进行格式化非一次全部完成)
//			    if(eLoSta_FormSucc == gLoadWorkState)
//				{
//        			/*切换回正在安装,进入下一步*/
//			    	gLoadWorkState = eLoSta_DoInstal;
//					sState = 2;
//				}
				sState = 2;
				break;

			/*等待分次格式化内部FLASH程序完成*/
			case 2:
				//格式化成功
				if(0xff == gFlashEraseStep)
				{
					/*读取下载FLASH中程序总段数*/
					if(TRUE == LoadDownFlashReadByte(APP_PRO_EXTDOWN_FLASH_ADDR + 2, gFlashAppData, 2))
					{
						/*解析固件总段数*/
						sSumLen = (u16)gFlashAppData[1] | ((u16)gFlashAppData[0] << 8);

						//用于显示安装进度(%)
						gRemLoadIndex = 0;
						sTimes = 0;

						/*进入下一步拷贝程序*/
						sState = 3;
					}
					else
					{
						/*安装失败*/
						gLoadWorkState = eLoSta_InstFail;

						/*安装失败*/
						gRemLoadState = 4;
					}
				}
				break;

			/*从下载FLASH拷贝程序到内部运行FLASH*/
			case 3:
			    BSPWatchDogOutput();
			    /*获取读取下载FLASH中固件代码的地址和长度*/
			    if(TRUE == LoadDownFlashReadCodeAddrLen(sTimes, &addr, &sRunAddr, &sRemLen))
			    {
				    BSPWatchDogOutput();
				    //读下载FLASH中的程序段和校验码
					if((TRUE == LoadDownFlashReadByte(addr, gFlashAppData, sRemLen))
						&& (TRUE == LoadDownFlashReadByte(addr - 2, rdCrc, 2)))
					{
						/*重新计算读取到的程序段校验码与记录校验码一致*/
						if(CalcCodeCRC(gFlashAppData, sRemLen) == (((u16)rdCrc[0] << 8) | rdCrc[1]))
						{
							/*内部FLASH写长度必须是8的倍数*/
							// need to append post-staffing uint16_t
							while((sRemLen & 0x000007) != 0)
							{
								gFlashAppData[sRemLen] = 0xff;
								sRemLen++;
							}

							/*读取成功则进入下一步*/
							sState = 4;
						}
						else
						{
							/*安装失败*/
							gLoadWorkState = eLoSta_InstFail;

							/*安装失败*/
							gRemLoadState = 4;
						}
					}
					else
					{
						/*安装失败*/
						gLoadWorkState = eLoSta_InstFail;

						/*安装失败*/
						gRemLoadState = 4;
					}
			    }
				else
				{
					/*安装失败*/
					gLoadWorkState = eLoSta_InstFail;

					/*安装失败*/
					gRemLoadState = 4;
				}
				break;

			/*将下载FLASH程序写内部运行区*/
			case 4:
				//写程序到内部运行FLASH中
				BSPWatchDogOutput();
				if(TRUE == BSPINTFLASHWriteByte(sRunAddr, gFlashAppData, sRemLen))
				{
					/*下一段写入地址*/
					sTimes++;

					//显示安装进度(%)
					gRemLoadIndex = (sTimes * 100) / sSumLen;

					/*整个区域全部段拷贝完成*/
					if(sTimes >= sSumLen)
					{
						/*安装完成*/
						sState = 5;
					}
					/*应用程序至少784K*/
					else if(sTimes < 784)
					{
						/*直接返回拷贝*/
						sState = 3;
					}
					else
					{
//						/*检测指定长度全为0xFF则认为备份完成*/
//					    for(i = 0; i < APP_PRO_BLOCK1_CHECK_LEN; i++)
//					    {
//					        //检测应用程序区域有一个地址非0xff和0(认为有应用程序)
//					        if((0xff != gFlashAppData[i]) && (0 != gFlashAppData[i])
//					        	&& (0xff != gFlashAppData[LOAD_APP_BUF_LEN - APP_PRO_BLOCK1_CHECK_LEN + i]) && (0 != gFlashAppData[LOAD_APP_BUF_LEN - APP_PRO_BLOCK1_CHECK_LEN + i]))
//					        {
//								/*返回继续拷贝*/
//								sState = 3;
//								return;
//					        }
//					    }

					    /*所有地址全部是0xff或0则备份完成*/
						sState = 5;
					}
				}
				else
				{
					/*安装失败*/
					gLoadWorkState = eLoSta_InstFail;

					/*安装失败*/
					gRemLoadState = 4;
				}
				break;

			/*记录程序安装完成*/
			case 5:
				/*安装成功*/
				gLoadWorkState = eLoSta_InstSucc;

				/*安装成功*/
				gRemLoadState = 6;

				/*记录内部程序已安装*/
			    CanLoaderOkHook();
			    sTimes = 0;
				sState = 0;
				break;

			default:
				break;
    	}
    }
    /*本地升级模式备份程序*/
    else if((eLoMod_LocBoot == gLoadWorkModel) && (eLoSta_Copying == gLoadWorkState))
    {
    	switch(sState)
    	{
    		/*检测程序是否正常*/
    		case 0:
    			/*已记录程序运行正常*/
    			if(LOAD_APP_RUN_FLAG == (((u16)sFlashBootPara[eBOOT_APP_RUN_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_APP_RUN_FLAG_L_ID]))
    			{
					/*内部FLASH上APP代码校验正常功*/
					if(TRUE == CheckFlashAppProCode(0))
					{
	        			/*进入下一步*/
						sState = 1;
					}
					else
					{
	        			/*无源程序*/
	    	    		gLoadWorkState = eLoSta_NotProg;
					}
    			}
    			else
    			{
        			/*无源程序*/
    	    		gLoadWorkState = eLoSta_NotProg;
    			}
    			break;

			/*擦除备份FLASH程序区*/
			case 1:
				/*未开始进行备份FLASH擦除*/
				if(0xff == gFlashEraseStep)
				{
					/*烧录前备份FLASH擦除处理*/
					IntoCanLoadModelHook();
				}

//				//擦除成功
//			    if(eLoSta_ErasSucc == gLoadWorkState)
//				{
//        			/*切换回正在备份,进入下一步*/
//			    	gLoadWorkState = eLoSta_Copying;
//					sState = 2;
//				}
				sState = 2;
				break;

			/*等待分次擦除备份FLASH程序区完成*/
			case 2:
				//擦除成功
				if(0xff == gFlashEraseStep)
				{
					/*进入下一步*/
					sState = 3;
				}
				break;

			/*从内部FLASH拷贝程序到备份程序区*/
			case 3:
			    //读内部FLASH中的程序
			    BSPWatchDogOutput();
				if(TRUE == BSPINTFLASHReadByte(APP_PRO_BLOCK1_FLASH_ADDR + (LOAD_APP_BUF_LEN * sTimes), gFlashAppData, LOAD_APP_BUF_LEN))
				{
					/*读取成功则进入下一步*/
					sState = 4;
				}
				else
				{
					/*备份失败*/
					gLoadWorkState = eLoSta_CopyFail;
				}
				break;

			/*将内部FLASH程序写备份程序区*/
			case 4:
				//写程序到备份FLASH中
				BSPWatchDogOutput();
				if(TRUE == LoadCopyFlashWriteByte(APP_PRO_INTCOPY_FLASH_ADDR + (LOAD_APP_BUF_LEN * sTimes), gFlashAppData, LOAD_APP_BUF_LEN))
				{
					sTimes++;
					/*整个区域拷贝完成*/
					if(sTimes > ((APP_PRO_BLOCK1_FIN_ADDR - APP_PRO_BLOCK1_FLASH_ADDR) / LOAD_APP_BUF_LEN))
					{
						/*备份完成*/
						sState = 5;
					}
					/*应用程序至少784K*/
					else if(sTimes < 784)
					{
						/*直接返回拷贝*/
						sState = 3;
					}
					else
					{
						/*检测指定长度全为0xFF则认为备份完成*/
					    for(i = 0; i < APP_PRO_BLOCK1_CHECK_LEN; i++)
					    {
					        //检测应用程序区域有一个地址非0xff和0(认为有应用程序)
					    	if((0xff != gFlashAppData[i]) && (0 != gFlashAppData[i])
								&& (0xff != gFlashAppData[LOAD_APP_BUF_LEN - APP_PRO_BLOCK1_CHECK_LEN + i]) && (0 != gFlashAppData[LOAD_APP_BUF_LEN - APP_PRO_BLOCK1_CHECK_LEN + i]))
							{
								/*返回继续拷贝*/
								sState = 3;
								return;
					        }
					    }

					    /*所有地址全部是0xff或0则备份完成*/
						sState = 5;
					}
				}
				else
				{
					/*备份失败*/
					gLoadWorkState = eLoSta_CopyFail;
				}
				break;

			/*记录程序备份完成*/
			case 5:
				/*备份成功*/
				gLoadWorkState = eLoSta_CopySucc;

				/*记录内部程序已备份*/
			    CanLoaderOkHook();
				sState = 0;
				break;

			default:
				break;
    	}
    }
    /*本地升级模式回滚程序*/
	else if((eLoMod_LocBoot == gLoadWorkModel) && (eLoSta_Backing == gLoadWorkState))
    {
    	/*从备份FLASH拷贝程序到内部FLASH*/
    	switch(sState)
    	{
    		/*检测备份FLASH程序是否正常*/
    		case 0:
    			/*备份FLASH有备份程序*/
    			if(LOAD_APP_COPY_FLAG == sFlashBootPara[eBOOT_APP_COPY_FLAG_ID])
    			{
					/*备份FLASH上APP代码校验正常*/
					if(TRUE == CheckFlashAppProCode(1))
					{
	        			/*进入下一步*/
						sState = 1;
					}
					else
					{
	        			/*无源程序*/
	    	    		gLoadWorkState = eLoSta_NotProg;
					}
    			}
    			else
    			{
        			/*无源程序*/
    	    		gLoadWorkState = eLoSta_NotProg;
    			}
    			break;

			/*开始格式化内部FLASH程序*/
			case 1:
				/*未开始进行内部FLASH格式化*/
				if(0xff == gFlashEraseStep)
				{
					/*烧录前内部FLASH格式化处理*/
					IntoCanLoadModelHook();
				}

//				//格式化成功
//			    if(eLoSta_FormSucc == gLoadWorkState)
//				{
//        			/*切换回正在安装,进入下一步*/
//			    	gLoadWorkState = eLoSta_DoInstal;
//					sState = 2;
//				}
				sState = 2;
				break;

			/*等待分次格式化内部FLASH程序完成*/
			case 2:
				//格式化成功
				if(0xff == gFlashEraseStep)
				{
					/*进入下一步*/
					sState = 3;
				}
				break;

			/*从备份FLASH拷贝程序到内部程序区*/
			case 3:
			    //读备份FLASH中的程序
			    BSPWatchDogOutput();
				if(TRUE == LoadCopyFlashReadByte(APP_PRO_INTCOPY_FLASH_ADDR + (LOAD_APP_BUF_LEN * sTimes), gFlashAppData, LOAD_APP_BUF_LEN))
				{
					/*读取成功则进入下一步*/
					sState = 4;
				}
				else
				{
					/*回滚失败*/
					gLoadWorkState = eLoSta_BackFail;
				}
				break;

			/*将备份FLASH程序写内部程序区*/
			case 4:
				//写程序到内部FLASH中
				BSPWatchDogOutput();
				if(TRUE == BSPINTFLASHWriteByte(APP_PRO_BLOCK1_FLASH_ADDR + (LOAD_APP_BUF_LEN * sTimes), gFlashAppData, LOAD_APP_BUF_LEN))
				{
					sTimes++;
					/*整个区域拷贝完成*/
					if(sTimes > ((APP_PRO_BLOCK1_FIN_ADDR - APP_PRO_BLOCK1_FLASH_ADDR) / LOAD_APP_BUF_LEN))
					{
						/*安装完成*/
						sState = 5;
					}
					/*应用程序至少784K*/
					else if(sTimes < 784)
					{
						/*直接返回拷贝*/
						sState = 3;
					}
					else
					{
						/*检测指定长度全为0xFF则认为备份完成*/
					    for(i = 0; i < APP_PRO_BLOCK1_CHECK_LEN; i++)
					    {
					        //检测应用程序区域有一个地址非0xff和0(认为有应用程序)
					        if((0xff != gFlashAppData[i]) && (0 != gFlashAppData[i])
					        	&& (0xff != gFlashAppData[LOAD_APP_BUF_LEN - APP_PRO_BLOCK1_CHECK_LEN + i]) && (0 != gFlashAppData[LOAD_APP_BUF_LEN - APP_PRO_BLOCK1_CHECK_LEN + i]))
					        {
								/*返回继续拷贝*/
								sState = 3;
								return;
					        }
					    }

					    /*所有地址全部是0xff或0则备份完成*/
						sState = 5;
					}
				}
				else
				{
					/*回滚失败*/
					gLoadWorkState = eLoSta_BackFail;
				}
				break;

			/*记录程序回滚完成*/
			case 5:
				/*回滚成功*/
				gLoadWorkState = eLoSta_BackSucc;

				/*记录内部程序已安装*/
			    CanLoaderOkHook();
				sState = 0;
				break;

			default:
				break;
    	}
    }
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static void LoadHandleCmdDeal(u16 type, u16 ecu, u8 *data, u8 canNum)
** @Input     : type:命令类型  ecu:设备地址 data:命令数据 canNum:通讯端口号
** @Output    : void
** @Function  : 在线升级握手处理
** @The notes :
**===============================================================================================*/
static void LoadHandleCmdDeal(u16 type, u16 ecu, u8 *data, u8 canNum)
{
    (void)canNum;

    //分辨握手命令类型
    switch(type)
    {
        //设备参数设置命令
        case eLoCmd_ParaCfg:
            //只能点对点(非广播)设置设备地址
            if((ecu == gLoadDevAddr) && (0 == data[2]) && (data[3] < 0xfe))
            {
            	//更新当前地址
            	gLoadDevAddr = data[3];

            	//配置APP参数设备地址
				//ParaWriteStoreGroupGenParaInfo(1, &gLoadDevAddr, 1);

            	//BOOT参数有效才能配置BOOT参数区
                //if(LOAD_APP_PARA_FLAG == (((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID]))
                {
					//参数标志(BOOT必须写参数)
					sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
					sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;
					//写入设备地址
					sFlashBootPara[eBOOT_EDV_ECU_L_ID] = gLoadDevAddr & 0xff;
					sFlashBootPara[eBOOT_EDV_ECU_H_ID] = 1;

					//参数重新写Flash
					(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
					BSPWatchDogOutput();

					(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
					BSPWatchDogOutput();

					//延时跳转复位
					//TimeDelay(500);
                }

				//程序复位
				//EnterBootPro();
            }
            /*点对点配置硬件参数*/
            else if((ecu == gLoadDevAddr) && (data[2] >= 1) && (data[2] <= 9))
            {
            	CfgBootHWParaHandle(data[2], (((u16)data[4] << 8) | data[3]));
            }
            break;

        //设备程序复位命令
        case eLoCmd_ProgRes:
        	/*跳转进入BOOT复位*/
        	EnterBootPro();
            break;

        //工作模式切换
        case eLoCmd_ChanMod:
            /*切换工作模式处理*/
        	if(data[2] < eLoMod_Fin)
        	{
        		ChangeWorkModeHook(data[2]);
        	}
            break;

		/*程序读写操作命令*/
		case eLoCmd_ProRdWr:
			/*请求安装程序命令*/
			if(eCtrlCmd_ReqInst == data[2])
			{
				/*正在执行连续任务则不处理*/
				if((eLoSta_LoaTrans == gLoadWorkState)
					|| (eLoSta_TranSucc == gLoadWorkState)
					|| (eLoSta_WritSucc == gLoadWorkState)
					|| (eLoSta_DoInstal == gLoadWorkState)
					|| (eLoSta_Copying == gLoadWorkState)
					|| (eLoSta_Backing == gLoadWorkState))
				{
					return;
				}

				/*当前BOOT程序直接切换远程升级模式/正在安装状态*/
				gLoadWorkModel = eLoMod_RemBoot;
				gLoadWorkState = eLoSta_DoInstal;

				/*正在安装*/
				gRemLoadState = 5;
			}
			/*终止安装程序命令*/
			else if(eCtrlCmd_EndInst == data[2])
			{
				/*仅正在安装状态有效*/
				if(eLoSta_DoInstal == gLoadWorkState)
				{
		            /*终止擦除FLASH(可能正在擦除)*/
		    		gFlashEraseStep = 0xff;

					/*等待安装状态*/
					gLoadWorkState = eLoSta_WaitInst;
		        	/*安装失败状态*/
		        	//gLoadWorkState = eLoSta_InstFail;

		            /*安装失败状态*/
		            gRemLoadState = 4;
				}
			}
			/*请求备份程序命令*/
			else if(eCtrlCmd_ReqCopy == data[2])
			{
				/*正在执行连续任务则不处理*/
				if((eLoSta_LoaTrans == gLoadWorkState)
					|| (eLoSta_TranSucc == gLoadWorkState)
					|| (eLoSta_WritSucc == gLoadWorkState)
					|| (eLoSta_DoInstal == gLoadWorkState)
					|| (eLoSta_Copying == gLoadWorkState)
					|| (eLoSta_Backing == gLoadWorkState))
				{
					return;
				}

				/*当前BOOT程序直接切换本地升级模式/正在备份状态*/
				gLoadWorkModel = eLoMod_LocBoot;
				gLoadWorkState = eLoSta_Copying;
			}
			/*终止备份程序命令*/
			else if(eCtrlCmd_EndCopy == data[2])
			{
				/*仅正在备份状态有效*/
				if(eLoSta_Copying == gLoadWorkState)
				{
		            /*终止擦除FLASH(可能正在擦除)*/
		    		gFlashEraseStep = 0xff;

					/*等待备份状态*/
					gLoadWorkState = eLoSta_WaitCopy;
				}
			}
			/*请求回滚程序命令*/
			else if(eCtrlCmd_ReqBack == data[2])
			{
				/*正在执行连续任务则不处理*/
				if((eLoSta_LoaTrans == gLoadWorkState)
					|| (eLoSta_TranSucc == gLoadWorkState)
					|| (eLoSta_WritSucc == gLoadWorkState)
					|| (eLoSta_DoInstal == gLoadWorkState)
					|| (eLoSta_Copying == gLoadWorkState)
					|| (eLoSta_Backing == gLoadWorkState))
				{
					return;
				}

				/*当前BOOT程序直接切换本地升级模式/正在回滚状态*/
				gLoadWorkModel = eLoMod_LocBoot;
				gLoadWorkState = eLoSta_Backing;
			}
			/*终止回滚程序命令*/
			else if(eCtrlCmd_EndBack == data[2])
			{
				/*仅正在回滚状态有效*/
				if(eLoSta_Backing == gLoadWorkState)
				{
		            /*终止擦除FLASH(可能正在擦除)*/
		    		gFlashEraseStep = 0xff;

					/*回滚失败状态*/
					gLoadWorkState = eLoSta_BackFail;
				}
			}
			break;

        //程序升级控制命令(握手)
        case eLoCmd_LoaCtrl:
        	//请求程序升级命令
        	if(eCtrlCmd_ReqLoad == data[2])
        	{
				/*未选定本地模式/远程模式或正在安装或备份程序或回滚程序则不处理*/
				if(((eLoMod_LocBoot != gLoadWorkModel)
					&& (eLoMod_RemBoot != gLoadWorkModel))
					|| (eLoSta_DoInstal == gLoadWorkState)
					|| (eLoSta_Copying == gLoadWorkState)
					|| (eLoSta_Backing == gLoadWorkState))
				{
					return;
				}

				//识别码正确
				if((LOAD_DEV_ID == ((u16)data[3] | ((u16)data[4] << 8)))
					|| (0xffff == ((u16)data[3] | ((u16)data[4] << 8))))
				{
					//未握手未擦除FLASH
					if((eLoSta_HandSucc != gLoadWorkState)
						&& (eLoSta_ErasSucc != gLoadWorkState)
						&& (eLoSta_FormSucc != gLoadWorkState))
					{
						//由上位机命令确认下载完成
						gRemLoadSumByte = 0xffffffff;

						//握手成功
						gLoadWorkState = eLoSta_HandSucc;
						//擦除FLASH
						IntoCanLoadModelHook();

			        	/*远程升级状态清零*/
			            gRemLoadIndex = 0;
			            gRemLoadVers = 0;

			            /*空闲状态(擦除FLASH再显示空闲)*/
			            //gRemLoadState = 3;
					}
				}
				//识别码不正确
				else
				{
					//非广播
					if(0xff != ecu)
					{
						//握手失败
						gLoadWorkState = eLoSta_HandFail;
					}
				}
        	}
            /*终止程序升级命令*/
        	else if(eCtrlCmd_EndLoad == data[2])
        	{
	            /*终止擦除FLASH(可能正在擦除)*/
	    		gFlashEraseStep = 0xff;

            	/*退回等待握手状态*/
            	gLoadWorkState = eLoSta_HandWait;

	            /*下载完成*/
	            gRemLoadState = 0;
        	}
            break;

		//请求远程传输命令(APP远程传输)
		case eLoCmd_IntRemTra:
        	/*切换远程传输模式(跳转到APP)*/
    		ChangeWorkModeHook(eCtrlCmd_RemTra);
            break;

		/*远程升级请求(BOOT远程升级)*/
		case eLoCmd_IntRemLoa:
			/*切换远程升级模式处理*/
			ChangeWorkModeHook(eCtrlCmd_RemBoot);

			/*切换远程升级模式成功*/
			if(eLoMod_RemBoot == gLoadWorkModel)
			{
                /*请求远程传输程序升级准备*/
				//识别码正确
				if((LOAD_DEV_ID == ((u16)data[3] | ((u16)data[2] << 8)))
					|| (0xffff == ((u16)data[3] | ((u16)data[2] << 8))))
				{
					//由下位机检测是否下载完成
					if(0xff == data[4])
					{
						gRemLoadSumByte = (u32)data[7] | ((u32)data[6] << 8) | ((u32)data[5] << 16);
					}
					//由上位机命令确认下载完成
					else
					{
						gRemLoadSumByte = 0xffffffff;
					}

					//未握手未擦除FLASH
					if((eLoSta_HandSucc != gLoadWorkState)
						&& (eLoSta_ErasSucc != gLoadWorkState)
						&& (eLoSta_FormSucc != gLoadWorkState))
					{
						//握手成功
						gLoadWorkState = eLoSta_HandSucc;
						//擦除FLASH
						IntoCanLoadModelHook();

			        	/*远程升级状态清零*/
			            gRemLoadIndex = 0;
			            gRemLoadVers = 0;
						gRemLoadRcvByte = 0;

			            /*空闲状态(擦除FLASH再显示空闲)*/
			            //gRemLoadState = 3;
					}
				}
				//识别码不正确
				else
				{
					//非广播
					if(0xff != ecu)
					{
						//握手失败
						gLoadWorkState = eLoSta_HandFail;
					}
				}
			}
			break;

		/*退出远程升级*/
		case eLoCmd_ExtRemLoa:
            /*终止擦除FLASH(可能正在擦除)*/
    		gFlashEraseStep = 0xff;

			/*切换正常应用模式处理(跳转到APP)*/
			ChangeWorkModeHook(eCtrlCmd_NorApp);
			break;

		/*请求安装程序(BOOT本地升级模式)*/
		case eLoCmd_ReqInst:
			/*正在执行连续任务则不处理*/
			if((eLoSta_LoaTrans == gLoadWorkState)
				|| (eLoSta_TranSucc == gLoadWorkState)
				|| (eLoSta_WritSucc == gLoadWorkState))
			{
				return;
			}

			/*当前BOOT程序直接切换远程升级模式/正在安装状态*/
			gLoadWorkModel = eLoMod_RemBoot;
			gLoadWorkState = eLoSta_DoInstal;

        	/*远程升级状态清零*/
            gRemLoadIndex = 0;
            gRemLoadVers = 0;

            /*正在安装状态*/
            gRemLoadState = 5;
			break;

		/*终止安装程序(远程)*/
		case eLoCmd_EndInst:
			/*正在安装*/
			if(eLoSta_DoInstal == gLoadWorkState)
			{
	            /*终止擦除FLASH(可能正在擦除)*/
	    		gFlashEraseStep = 0xff;

	        	/*安装失败状态*/
	        	gLoadWorkState = eLoSta_InstFail;

	            /*安装失败状态*/
	            gRemLoadState = 4;
			}
			else
			{
				/*退回等待握手状态*/
				gLoadWorkState = eLoSta_HandWait;

	            /*下载完成状态*/
	            gRemLoadState = 0;
			}
			break;

        default:
            break;
    }
}

/*=================================================================================================
** @Name      : static void LoadRemDownMsgDeal(u8 ecu, u8 *data, u8 len)
** @Input     : ecu:目标设备 data:下载通信数据 len:通信数据长度
** @Output    : void
** @Function  : 远程下载故障数据报文处理
** @The notes :
**===============================================================================================*/
static void LoadRemDownMsgDeal(u8 ecu, u8 *data, u8 len)
{
	u8 i = 0;
    u16 dataIndex = 0;  //数据的序号
    u16 datalength = 0; //数据长度

    /*获取CAN信息中帧索引、数据长度及数据*/
    dataIndex = (u16)data[0] | (((u16)data[1] & 0x0f) << 8);
    datalength = (u16)(((u16)data[1] & 0xf0) >> 4);

    /*发送的数据不合法,发送的长度不合法*/
    if((datalength == 0) || (datalength > 6) || (datalength >= (len + 2)))
    {
        return;
    }

    /*首帧固件信息*/
    if((0 == gRemLoadIndex) && (1 == dataIndex))
    {
        /*初始化数据域*/
        ClrLoadAppBuffData();
        /*固件传输启动*/
        gLoadWorkState = eLoSta_LoaStart;

        //必须6个字节且识别码正确
		if((6 == datalength)
			&& ((LOAD_DEV_ID == ((u16)data[3] | ((u32)data[2] << 8)))
			|| (0xffff == ((u16)data[3] | ((u32)data[2] << 8)))))
		{
			gLoadStartAddr = 0;
            gLoadSectionLen = 0;
            gLoadSectionCRC = 0;

            /*正在下载*/
            gRemLoadState = 2;

            /*记录本帧要存储到flash的数据*/
            for(i = 0; i < datalength; i++)
            {
                gFlashAppData[gLoadSectionLen] = data[2 + i];
                gLoadSectionLen++;
            }

            /*接收一帧成功并累计接收字节数*/
            gRemLoadIndex++;
            gRemLoadRcvByte += datalength;
		}
		//识别码不正确
		else
		{
			//非广播
			if(0xff != ecu)
			{
				//握手失败
				gLoadWorkState = eLoSta_HandFail;

				/*下载失败*/
				gRemLoadState = 1;
			}
		}
    }
    /*第二帧帧固件版本和时间*/
    else if((1 == gRemLoadIndex) && (2 == dataIndex))
    {
        /*固件正在传输*/
        gLoadWorkState = eLoSta_LoaTrans;

        //必须6个字节
		if(6 == datalength)
		{
            /*记录版本号*/
			gRemLoadVers = (u32)data[5] | ((u32)data[4] << 8) | ((u32)data[3] << 16) | ((u32)data[2] << 24);

            /*记录本帧要存储到flash的数据*/
            for(i = 0; i < datalength; i++)
            {
                gFlashAppData[gLoadSectionLen] = data[2 + i];
                gLoadSectionLen++;
            }
            /*接收一帧成功并累计接收字节数*/
            gRemLoadIndex++;
            gRemLoadRcvByte += datalength;
		}
		//字符数不正确
		else
		{
			//非广播
			if(0xff != ecu)
			{
				//传输失败
				gLoadWorkState = eLoSta_TranFail;

				/*下载失败*/
				gRemLoadState = 1;
			}
		}
    }
    /*第三帧帧固件时间和固件起始地址*/
    else if((2 == gRemLoadIndex) && (3 == dataIndex))
    {
        /*固件正在传输*/
        gLoadWorkState = eLoSta_LoaTrans;

        //必须6个字节
		if(6 == datalength)
		{
            /*获取起始FLASH地址*/
			gLoadStartAddr = (u32)data[7] | ((u32)data[6] << 8) | ((u32)data[5] << 16) | ((u32)data[4] << 24);

            /*验证起始FLASH地址正确*/
			if(APP_PRO_BLOCK1_FLASH_ADDR == gLoadStartAddr)
			{
				/*记录本帧要存储到flash的数据*/
				for(i = 0; i < datalength; i++)
				{
					gFlashAppData[gLoadSectionLen] = data[2 + i];
					gLoadSectionLen++;
				}

				/*将首行写FLASH(写FLASH失败)*/
    			if(FALSE == LoadDownFlashWriteByte(APP_PRO_EXTDOWN_FLASH_ADDR, gFlashAppData, gLoadSectionLen))
    			{
    				/*下载失败*/
    				gRemLoadState = 1;

            		/*写失败*/
            		gLoadWorkState = eLoSta_WritFail;
    			}
				gLoadSectionLen = 0;

				/*下一帧开始记录FLASH地址*/
				gLoadStartAddr = APP_PRO_EXTDOWN_FLASH_ADDR + APP_PRO_DOWN_FLASH_SEC_LEN;

				/*接收一帧成功并累计接收字节数*/
				gRemLoadIndex++;
		        gRemLoadRcvByte += datalength;
			}
			//固件起始地址不正确
			else
			{
				//非广播
				if(0xff != ecu)
				{
					//传输失败
					gLoadWorkState = eLoSta_TranFail;

					/*下载失败*/
					gRemLoadState = 1;
				}
			}
		}
		//字符数不正确
		else
		{
			//非广播
			if(0xff != ecu)
			{
				//传输失败
				gLoadWorkState = eLoSta_TranFail;

				/*下载失败*/
				gRemLoadState = 1;
			}
		}
    }
    /*第四帧起开始为固件数据(帧序号必须一致,不解析校验,直接写FLASH)*/
    else if(((gRemLoadIndex + 1) % 4096) == dataIndex)
    {
        /*存储接收到的固件flash数据*/
        for(i = 0; i < datalength; i++)
        {
            gFlashAppData[gLoadSectionLen] = data[2 + i];
            gLoadSectionLen++;

    		/*接收满1K数据*/
            if(gLoadSectionLen >= LOAD_APP_BUF_LEN)
            {
    			/*将数据写FLASH(写FLASH失败)*/
    			if(FALSE == LoadDownFlashWriteByte(gLoadStartAddr, gFlashAppData, gLoadSectionLen))
    			{
    				/*下载失败*/
    				gRemLoadState = 1;

            		/*写失败*/
            		gLoadWorkState = eLoSta_WritFail;
    			}

				/*下一帧开始记录FLASH地址*/
				gLoadStartAddr += gLoadSectionLen;
    			gLoadSectionLen = 0;

    			/*返回当前状态信息*/
    			CanSendRemLoadMsgAPI();
            }
        }

		/*接收一帧成功并累计接收字节数*/
		gRemLoadIndex++;
        gRemLoadRcvByte += datalength;

        /*接收固件全部字节数完成*/
        if(gRemLoadRcvByte >= gRemLoadSumByte)
        {
			/*缓存最后下载数据未写FLASH*/
			if(0 != gLoadSectionLen)
			{
				/*将数据写FLASH(写FLASH失败)*/
				if(FALSE == LoadDownFlashWriteByte(gLoadStartAddr, gFlashAppData, gLoadSectionLen))
				{
					/*下载失败*/
					gRemLoadState = 1;

					/*写失败*/
					gLoadWorkState = eLoSta_WritFail;
				}
				gLoadSectionLen = 0;
			}

			/*校验全部代码数据正常*/
			if((gLoadWorkState != eLoSta_WritFail)
				&& (TRUE == CheckFlashAppProCode(2)))
			{
				/*下载成功*/
				gRemLoadState = 0;

				/*等待安装*/
				gLoadWorkState = eLoSta_WaitInst;

				/*更新记录应用程序状态*/
				CanLoaderOkHook();
			}
			else
			{
				/*下载失败*/
				gRemLoadState = 1;

				/*传输失败*/
				gLoadWorkState = eLoSta_TranFail;
			}
        }
    }
}

#if(LOAD_CAN_NUM > 1)
/*=================================================================================================
** @Name      : static u8 GetCanRouteType(u8 canNum)
** @Input     : canNum,CAN通道号
** @Output    : 0xff:未使用，1：向下 0:向上
** @Function  : 获取CAN端口通讯路由方向
** @The notes :
**===============================================================================================*/
static u8 GetCanRouteType(u8 canNum)
{
    u8 i = 0;

    for(i = 0; i < LOAD_CAN_NUM; i++)
    {
        if(sLoadCanRouteCfg[i].canNum == canNum)
        {
            return(sLoadCanRouteCfg[i].canType);
        }
    }

    return(0xff);
}

/*=================================================================================================
** @Name      : static void CanLoadMsgTransmist(t_CANMSG *msg, u8 canNum)
** @Input     : msg:通讯信息 canNum:CAN通道号
** @Output    : void
** @Function  : 转发在线升级CAN信息
** @The notes :
**===============================================================================================*/
static void CanLoadMsgTransmist(t_CANMSG *msg, u8 canNum)
{
    u8 i = 0;
    u8 type = 0;

    //通讯端口路由类型
    type = GetCanRouteType(canNum);

    //未使能的CAN通道号信息不处理
    if(type == 0xff)
    {
        return;
    }

    //此路CAN口为向上端口
    if(type == eCANROUTE_UP)
    {
        //向下转发
        for(i = 0; i < LOAD_CAN_NUM; i++)
        {
            //找到对下的端口使能
            if(sLoadCanRouteCfg[i].canType == eCANROUTE_DN)
            {
                //转发此条信息
                (void)BSPCANSendMsg((e_CANNUM)sLoadCanRouteCfg[i].canNum, msg);
            }
        }
    }
    //此路CAN口为向下端口
    else if(type == eCANROUTE_DN)
    {
        //向上转发
        for(i = 0; i < LOAD_CAN_NUM; i++)
        {
            //找到对上的端口使能
            if(sLoadCanRouteCfg[i].canType == eCANROUTE_UP)
            {
                //转发此条信息
                (void)BSPCANSendMsg((e_CANNUM)sLoadCanRouteCfg[i].canNum, msg);
            }
        }
    }
}
#endif

/*=================================================================================================
** @Name      : static u32 BuildLoadAREMsgID(u8 type, u8 sDev, u8 sAddr)
** @Input     : type:报文类型 sDev:源设备类型[0:从控 1:高压模块 2:主控 3:堆控 0xff:所有设备] sAddr:源设备地址
** @Output    : 在线升级回复报文ID
** @Function  : 构建在线升级回复报文ID
** @The notes :
**===============================================================================================*/
static u32 BuildLoadAREMsgID(u8 type, u8 sDev, u8 sAddr)
{
    u32 id = 0;

    //合成功能码(回复14)/传输方向(上行0)/报文类型/目标设备地址/源设备地址
    id = ((u32)LOAD_AEQ_FUN << 25) | ((u32)0 << 24)
        | ((u32)type << 16) | ((u32)sDev << 8) | (u32)sAddr;

    return(id);
}

/*=================================================================================================
** @Name      : static void ChangeWorkModeHook(u8 mode)
** @Input     : mode:目标工作模式
** @Output    : void
** @Function  : 切换在线升级工作模式处理
** @The notes : 根据当前所处程序选择切换或跳转
**===============================================================================================*/
static void ChangeWorkModeHook(u8 mode)
{
    /*请求切换BOOT远程升级模式*/
    if(eCtrlCmd_RemBoot == mode)
    {
    	/*当前非BOOT远程升级模式*/
    	if(eLoMod_RemBoot != gLoadWorkModel)
    	{
            /*终止擦除FLASH(可能正在擦除)*/
    		gFlashEraseStep = 0xff;

			/*直接切换至远程升级模式/等待握手状态*/
			gLoadWorkModel = eLoMod_RemBoot;
			gLoadWorkState = eLoSta_HandWait;
    	}
    }
    /*请求切换BOOT本地升级模式*/
    else if(eCtrlCmd_LocBoot == mode)
    {
    	/*当前非BOOT本地升级模式*/
    	if(eLoMod_LocBoot != gLoadWorkModel)
    	{
            /*终止擦除FLASH(可能正在擦除)*/
    		gFlashEraseStep = 0xff;

			/*直接切换至本地升级模式/等待握手状态*/
			gLoadWorkModel = eLoMod_LocBoot;
			gLoadWorkState = eLoSta_HandWait;
    	}
    }
    /*请求切换APP远程传输模式*/
    else if(eCtrlCmd_RemTra == mode)
    {
    	/*正处于BOOT无应用模式*/
    	if(eLoMod_NotApp == gLoadWorkModel)
    	{
            /*终止擦除FLASH(可能正在擦除)*/
    		gFlashEraseStep = 0xff;

            /*保持无应用程序模式/无源程序状态*/
        	gLoadWorkModel = eLoMod_NotApp;
            gLoadWorkState = eLoSta_NotProg;
    	}
    	/*有APP应用程序*/
    	else
    	{
        	/*记录远程传输模式/等待握手状态*/
	        sFlashBootPara[eBOOT_JUM_FLAG_MODE_ID] = eLoMod_RemTra;
	        sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID] = eLoSta_HandWait;

	        (void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
	        BSPWatchDogOutput();

	        (void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
	        BSPWatchDogOutput();

            /*跳转进入应用程序*/
    		EnterAppPro();
    	}
    }
    /*请求切换APP正常应用模式*/
    else if(eCtrlCmd_NorApp == mode)
    {
    	/*正处于BOOT无应用模式*/
    	if(eLoMod_NotApp == gLoadWorkModel)
    	{
            /*终止擦除FLASH(可能正在擦除)*/
    		gFlashEraseStep = 0xff;

            /*保持无应用程序模式/无源程序状态*/
        	gLoadWorkModel = eLoMod_NotApp;
            gLoadWorkState = eLoSta_NotProg;
    	}
    	/*有APP应用程序*/
    	else
    	{
        	/*记录正常应用模式/通信路由状态*/
	        sFlashBootPara[eBOOT_JUM_FLAG_MODE_ID] = eLoMod_NorApp;
	        sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID] = eLoSta_Route;

	        (void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
	        BSPWatchDogOutput();

	        (void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
	        BSPWatchDogOutput();

            /*跳转进入应用程序*/
    		EnterAppPro();
    	}
    }
}

/*=================================================================================================
** @Name      : static void IntoCanLoadModelHook(void)
** @Input     : void
** @Output    : void
** @Function  : 请求进入程序烧录状态处理
** @The notes : 擦除内部FLASH或备份FLASH
**===============================================================================================*/
static void IntoCanLoadModelHook(void)
{
    u8 result = TRUE;

	/*本地升级握手或远程安装或本地回滚(程序写入内部FLASH应用程序运行区域)*/
	if(((eLoMod_LocBoot == gLoadWorkModel) && (eLoSta_HandSucc == gLoadWorkState))
		|| (eLoSta_DoInstal == gLoadWorkState) || (eLoSta_Backing == gLoadWorkState))
	{
	    BSPWatchDogOutput();
		/*擦除内部FLASH程序*/
	    sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
	    sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;                //记录参数配置标志
	    //sFlashBootPara[eBOOT_EDV_ECU_L_ID] = gLoadDevAddr & 0xff;
	    //sFlashBootPara[eBOOT_EDV_ECU_H_ID] = 0;                                               //记录设备地址(不改变)
	    //sFlashBootPara[eBOOT_JUM_FLAG_MODE_ID] = 0xff;                                        //初始工作模式(初始化已清除)

	    sFlashBootPara[eBOOT_APP_RUN_FLAG_L_ID] = 0xff;
	    sFlashBootPara[eBOOT_APP_RUN_FLAG_H_ID] = 0xff;                                         //清除应用程序运行标志
	    sFlashBootPara[eBOOT_APP_TRY_FLAG_L_ID] = LOAD_APP_TRY_FLAG & 0xff;
	    sFlashBootPara[eBOOT_APP_TRY_FLAG_H_ID] = (LOAD_APP_TRY_FLAG >> 8) & 0xff;              //记录尝试标志
	    //sFlashBootPara[eBOOT_APP_COP_FLAG_L_ID] = 0xff;
	    //sFlashBootPara[eBOOT_APP_COP_FLAG_H_ID] = 0xff;                                       //清除程序备份标志(不改变)

		/*只擦除原先特定工作状态*/
		if((eLoSta_WaitCopy == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID])                          //等待备份
			|| (eLoSta_Copying == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID]))                      //正在备份
		{
		    sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID] = 0xff;                                      //清除初始工作状态
		}

	    BSPWatchDogOutput();
	    result = BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);                           //擦除BOOT参数扇区

	    BSPWatchDogOutput();
	    result &= BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);//将BOOT参数重新写Flash

//	    BSPWatchDogOutput();
//	    result &= BSPINTFLASHEraseBlock(1);                                                     //擦除APP代码所在Flash块
//
//	    BSPWatchDogOutput();
//	    //result &= BSPINTFLASHEraseBlock(5);                                                   //擦除APP参数所在Flash块

	    if(TRUE == result)
	    {
//	    	/*内部FLASH格式化成功*/
//	    	gLoadWorkState = eLoSta_FormSucc;
	    	/*内部FLASH分步进行格式化*/
	    	if(0xff == gFlashEraseStep)
	    	{
	    		gFlashEraseStep = 0;
	    	}
	    }
	    else
	    {
	    	/*内部FLASH格式化失败功*/
	    	gLoadWorkState = eLoSta_FormFail;
	    }
	}
	/*本地备份(程序写入备份FLASH应用程序备份区域)*/
	else if(eLoSta_Copying == gLoadWorkState)
	{
		/*只擦除原先特定工作状态和程序备份标志*/
		if((eLoSta_Backing == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID])                           //正在回滚
			|| (0xff != sFlashBootPara[eBOOT_APP_COPY_FLAG_ID]))                                //已记录程序备份标志
		{
		    sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
		    sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

			sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID] = 0xff;
			sFlashBootPara[eBOOT_APP_COPY_FLAG_ID] = 0xff;

		    BSPWatchDogOutput();
		    result = BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);                       //擦除BOOT参数扇区

		    BSPWatchDogOutput();
		    result &= BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);//将BOOT参数重新写Flash
		}

//		/*擦除备份FLASH程序备份区*/
//	    BSPWatchDogOutput();
//	    result &= BSPINTFLASHEraseBlock(2);                                                    //擦除APP代码备份所在备份Flash块
//
	    if(TRUE == result)
	    {
//	    	/*擦除备份FLASH成功*/
//	    	gLoadWorkState = eLoSta_ErasSucc;
	    	/*备份FLASH分步进行擦除*/
	    	if(0xff == gFlashEraseStep)
	    	{
	    		gFlashEraseStep = 10;
	    	}
	    }
	    else
	    {
	    	/*擦除备份FLASH失败*/
	    	gLoadWorkState = eLoSta_ErasFail;
	    }
	}
	/*远程升级(程序写入下载FLASH应用程序下载区域)*/
	else
	{
		/*只擦除原先特定工作状态和程序下载标志*/
		if((eLoSta_WaitInst == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID])                          //等待安装
			|| (eLoSta_DoInstal == sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID])                      //正在安装
			|| (0xff != sFlashBootPara[eBOOT_APP_DOWN_FLAG_ID]))                                //已记录程序下载标志
		{
		    sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
		    sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

			sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID] = 0xff;
			sFlashBootPara[eBOOT_APP_DOWN_FLAG_ID] = 0xff;

		    BSPWatchDogOutput();
		    result = BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);                       //擦除BOOT参数扇区

		    BSPWatchDogOutput();
		    result &= BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);//将BOOT参数重新写Flash
		}

//		/*擦除下载FLASH程序下载区*/
//	    BSPWatchDogOutput();
//	    result &= BSPINTFLASHEraseBlock(3);                                                    //擦除APP代码下载所在下载Flash块
//
	    if(TRUE == result)
	    {
//	    	/*擦除下载FLASH成功*/
//	    	gLoadWorkState = eLoSta_ErasSucc;
	    	/*下载FLASH分步进行擦除*/
	    	if(0xff == gFlashEraseStep)
	    	{
	    		gFlashEraseStep = 20;
	    	}
	    }
	    else
	    {
	    	/*擦除下载FLASH失败*/
	    	gLoadWorkState = eLoSta_ErasFail;
	    }
	}
}

/*=================================================================================================
** @Name      : static void CanLoaderOkHook(void)
** @Input     : void
** @Output    : void
** @Function  : 烧录完成处理
** @The notes : 记录运行程序成功或备份程序成功
**===============================================================================================*/
static void CanLoaderOkHook(void)
{
	/*记录内部程序已烧录成功*/
	BSPWatchDogOutput();
	//参数标志
	sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
	sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;
	//不跳转
	sFlashBootPara[eBOOT_JUM_FLAG_MODE_ID] = 0xff;
	//目标状态
	sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID] = gLoadWorkState;

	//本地升级成功(等待备份)或远程安装成功或本地回滚成功
	if((eLoSta_WaitCopy == gLoadWorkState) || (eLoSta_InstSucc == gLoadWorkState) || (eLoSta_BackSucc == gLoadWorkState))
	{
		//记录应用程序可运行标志
		sFlashBootPara[eBOOT_APP_RUN_FLAG_L_ID] = LOAD_APP_RUN_FLAG & 0xff;
		sFlashBootPara[eBOOT_APP_RUN_FLAG_H_ID] = (LOAD_APP_RUN_FLAG >> 8) & 0xff;

		//清除尝试运行标志
		sFlashBootPara[eBOOT_APP_TRY_FLAG_L_ID] = 0xff;
		sFlashBootPara[eBOOT_APP_TRY_FLAG_H_ID] = 0xff;
	}
	//本地备份成功
	else if(eLoSta_CopySucc == gLoadWorkState)
	{
		//记录应用程序已备份标志
		sFlashBootPara[eBOOT_APP_COPY_FLAG_ID] = LOAD_APP_COPY_FLAG;
	}
	//远程升级/远程传输成功(等待安装)或远程备份成功
	else if(eLoSta_WaitInst == gLoadWorkState)
	{
		//记录应用程序已备份标志
		sFlashBootPara[eBOOT_APP_DOWN_FLAG_ID] = LOAD_APP_DOWN_FLAG;
	}

	BSPWatchDogOutput();
	(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);

	BSPWatchDogOutput();
	(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
}

/*=================================================================================================
** @Name      : static void EnterBootPro(void)
** @Input     : void
** @Output    : void
** @Function  : 跳转进入BOOT程序
** @The notes :
**===============================================================================================*/
static void EnterBootPro(void)
{
//	typedef void (*bootloader_fun)(void);
//	bootloader_fun jump2app;

	/*延时(确保转发报文已发送)*/
	TimeDelay(2000);

	/*FreeRTOS中断优先级分成两组：一组受 FreeRTOS管理，在临界区里会被屏蔽，另一组不受RTOS影响，永远都是使能的。*/
	BSP_DISABLE_IRQ();

	/* SystemSoftwareReset */
	SystemSoftwareReset();/*软复位直接进入boot*/

//	/* 函数指针指向app的复位向量表的地址。注意将地址强转成函数入口地址 */
//	jump2app = (bootloader_fun)*(uint32_t*)(BOOT_ENRTY_POINT_FLASH_ADDR + 4);
//
//	/* 初始化中断向量表地址 */
//	//S32_SCB->VTOR = (u32)BOOT_ENRTY_POINT_FLASH_ADDR;
//
//	/* 将pc指针指向入函数地址（app地址）运行 */
//	jump2app();
}

/*=================================================================================================
** @Name      : static void EnterAppPro(void)
** @Input     : void
** @Output    : void
** @Function  : 跳转进入应用程序
** @The notes :
**===============================================================================================*/
static void EnterAppPro(void)
{
    void (*entry)(void);
    u32 pc;
    u32 __attribute__((unused)) sp;

	//WDOG_disable();/*关闭看门狗*/
	BSP_DISABLE_IRQ();/*禁止全局中断*/
	//vPortEnterCritical();/*RTOS禁止中断*/

	/*注销所有外设，如boot有增加外设，需要在此添加*/
	/*FTM*/
	FTM_DRV_Deinit(1);
	/*CAN*/
	FLEXCAN_DRV_Deinit(0);
	FLEXCAN_DRV_Deinit(1);
	FLEXCAN_DRV_Deinit(2);
	/*SPI*/
	LPSPI_DRV_MasterDeinit(0);
	LPSPI_DRV_MasterDeinit(1);
	LPSPI_DRV_MasterDeinit(2);
	/*flexspi*/
	FLEXIO_SPI_DRV_MasterDeinit(&flexio_spi_master_state);

    S32_SCB->VTOR = (u32)(APP_ENRTY_POINT_FLASH_ADDR);      /*Relocate interrupt table ptr*/
    sp = *((volatile u32*)APP_ENRTY_POINT_FLASH_ADDR);
    asm(" ldr sp, [r0,#0]");
    pc = *((volatile u32 *)(APP_ENRTY_POINT_FLASH_ADDR + 4));
    entry = (void (*)(void))pc;
    entry();
}

/*=================================================================================================
** @Name      : static u8 CheckFlashAppProCode(u8 sort)
** @Input     : sort:校验FLASH类型 0:内部FLASH 1:备份FLASH 2:远程下载FLASH
** @Output    : TRUE:正常 FALSE:异常
** @Function  : 检测Flash中应用程序代码
** @The notes :
**===============================================================================================*/
static u8 CheckFlashAppProCode(u8 sort)
{
    u16 i = 0;
    u16 sumLen = 0;
    u16 sumCrc = 0;
    u16 calcCrc = 0;
    u8 flashData[APP_PRO_BLOCK1_CHECK_LEN] = {0};

    BSPWatchDogOutput();

	/*应用程序和备份程序只校验程序起始代码*/
    if(sort < 2)
	{
		/*校验内部FLASH程序*/
		if(0 == sort)
		{
			//读取应用程序入口代码
			(void)BSPINTFLASHReadByte(APP_PRO_BLOCK1_FLASH_ADDR, flashData, APP_PRO_BLOCK1_CHECK_LEN);
		}
		/*校验备份FLASH程序*/
		else
		{
			//读取备份程序入口代码
			(void)LoadCopyFlashReadByte(APP_PRO_INTCOPY_FLASH_ADDR, flashData, APP_PRO_BLOCK1_CHECK_LEN);
		}

		for(i = 0; i < APP_PRO_BLOCK1_CHECK_LEN; i++)
		{
			//检测应用程序区域有一个地址非0xff和0(认为有应用程序)
			if((0xff != flashData[i]) && (0 != flashData[i]))
			{
				break;
			}
		}

		//所有地址全部是0xff或0
		if(i >= APP_PRO_BLOCK1_CHECK_LEN)
		{
			//认为无应用程序
			return(FALSE);
		}
	}
	/*远程下载程序校验全部程序代码*/
    else
    {
		//读取远程下载固件信息(前2byte为识别码)
		(void)LoadDownFlashReadByte(APP_PRO_EXTDOWN_FLASH_ADDR + 2, flashData, 4);

		//解析固件总段数和总校验码
		sumLen = (u16)flashData[1] | ((u16)flashData[0] << 8);
		sumCrc = (u16)flashData[3] | ((u16)flashData[2] << 8);

		//开始读固件数据信息地址(前4byte为地址)
		gLoadStartAddr = APP_PRO_EXTDOWN_FLASH_ADDR + APP_PRO_DOWN_FLASH_SEC_LEN + 4;

		//读取每段长度和CRC
    	for(i = 0; i < sumLen; i++)
    	{
    		//读取段长度和CRC
    		(void)LoadDownFlashReadByte(gLoadStartAddr, flashData, 4);
            gLoadSectionLen = (u16)flashData[1] | ((u16)flashData[0] << 8);
    		gLoadSectionCRC = (u16)flashData[3] | ((u16)flashData[2] << 8);

    		//读取到的长度异常
    		if(gLoadSectionLen > LOAD_APP_BUF_LEN)
    		{
    			break;
    		}
    		else
    		{
    			//计算总CRC
    	    	calcCrc += gLoadSectionCRC;

    			//读下一段固件长度和CRC地址
            	gLoadStartAddr += (gLoadSectionLen + 8);
    		}
    	}

		//校验码不一致则校验失败
    	if(sumCrc != calcCrc)
    	{
    	    return(FALSE);
    	}
    }

    return(TRUE);
}

/*=================================================================================================
** @Name      : static u16 CalcCodeCRC(u8 *data, u16 len)
** @Input     : data:代码段数据指针起始地址, length,段数据长度
** @Output    : CRC校验码
** @Function  : 计算代码段校验码
** @The notes :
**===============================================================================================*/
static u16 CalcCodeCRC(u8 *data, u16 len)
{
    u16 i = 0;
    u16 j = 0;
    u16 sCRC = 0xffff;

    for(i = 0; i < len; i++)
    {
        sCRC = sCRC ^ data[i];
        for(j = 0; j < 8; j++)
        {
            if(sCRC & 0x01)
            {
                sCRC = sCRC ^ 0xA001;
            }
            sCRC = sCRC >> 1;
        }
    }

    return(sCRC);
}

/*=================================================================================================
** @Name      : static void ClrLoadAppBuffData(void)
** @Input     : void
** @Output    : void
** @Function  : 清除在线升级应用程序接收缓冲区数据
** @The notes :
**===============================================================================================*/
static void ClrLoadAppBuffData(void)
{
    u16 i = 0;

    for(i = 0; i < LOAD_APP_BUF_LEN; i++)
    {
        gFlashAppData[i] = 0xff;
    }
}

/*=================================================================================================
** @Name      : static u8 CheckBootHWParaAvail(void)
** @Input     : void
** @Output    : 检测结果 TRUE:正常 FALSE:异常(需要重写FLASH)
** @Function  : 检测BOOT程序硬件参数有效性
** @The notes : 仅检测全局变量是否有效(未写FLASH)
**===============================================================================================*/
static u8 CheckBootHWParaAvail(void)
{
	u16 data = 0;
	u8 result = TRUE;

	/*BOOT参数标志*/
	data = ((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID];
	/*标志无效*/
	if(LOAD_APP_PARA_FLAG != data)
	{
		//BOOT配置参数标志有效
		data = LOAD_APP_PARA_FLAG;
		sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = data & 0xff;
		sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (data >> 8) & 0xff;

		sFlashBootPara[eBOOT_EDV_ECU_L_ID] = 0xfe;
		sFlashBootPara[eBOOT_EDV_ECU_H_ID] = 1;

		sFlashBootPara[eBOOT_HW_CFGBIT_L_ID] = 0;
		sFlashBootPara[eBOOT_HW_CFGBIT_H_ID] = 0;

		//检测失败(需重写FLASH)
		result = FALSE;
	}

    /*BOOT配置CAN0通信波特率参数*/
	data = ((u16)sFlashBootPara[eBOOT_CAN0_RATE_H_ID] << 8) | sFlashBootPara[eBOOT_CAN0_RATE_L_ID];
	//参数无效
	if((125 != data) && (250 != data) && (500 != data) && (1000 != data))
	{
		//将BOOT配置参数修改为默认值
		data = CAN0_BPS;
		sFlashBootPara[eBOOT_CAN0_RATE_L_ID] = data & 0xff;
		sFlashBootPara[eBOOT_CAN0_RATE_H_ID] = (data >> 8) & 0xff;

		/*记录BOOT配置CAN0波特率修改标志*/
		BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 0);

		//检测失败(需重写FLASH)
		result = FALSE;
	}

	if(data != CAN0_BPS)
	{
		/*重新初始化CAN0通信波特率*/
		DEVCANResetInit(eCAN0, data);
	}

    /*BOOT配置CAN1通信波特率参数*/
	data = ((u16)sFlashBootPara[eBOOT_CAN1_RATE_H_ID] << 8) | sFlashBootPara[eBOOT_CAN1_RATE_L_ID];
	//参数无效
	if((125 != data) && (250 != data) && (500 != data) && (1000 != data))
	{
		//将BOOT配置参数修改为默认值
		data = CAN1_BPS;
		sFlashBootPara[eBOOT_CAN1_RATE_L_ID] = data & 0xff;
		sFlashBootPara[eBOOT_CAN1_RATE_H_ID] = (data >> 8) & 0xff;

		/*记录BOOT配置CAN1波特率修改标志*/
		BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 1);

		//检测失败(需重写FLASH)
		result = FALSE;
	}

	if(data != CAN1_BPS)
	{
		/*重新初始化CAN1通信波特率*/
		DEVCANResetInit(eCAN1, data);
	}

    /*BOOT配置CAN2通信波特率参数*/
	data = ((u16)sFlashBootPara[eBOOT_CAN2_RATE_H_ID] << 8) | sFlashBootPara[eBOOT_CAN2_RATE_L_ID];
	//参数无效
	if((125 != data) && (250 != data) && (500 != data) && (1000 != data))
	{
		//将BOOT配置参数修改为默认值
		data = CAN2_BPS;
		sFlashBootPara[eBOOT_CAN2_RATE_L_ID] = data & 0xff;
		sFlashBootPara[eBOOT_CAN2_RATE_H_ID] = (data >> 8) & 0xff;

		/*记录BOOT配置CAN2波特率修改标志*/
		BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 2);

		//检测失败(需重写FLASH)
		result = FALSE;
	}

	if(data != CAN2_BPS)
	{
		/*重新初始化CAN2通信波特率*/
		DEVCANResetInit(eCAN2, data);
	}

    /*BOOT配置RS485-0通信站地址参数*/
	data = ((u16)sFlashBootPara[eBOOT_SCI0_ADDR_H_ID] << 8) | sFlashBootPara[eBOOT_SCI0_ADDR_L_ID];
	//参数无效
	if((data < 1) || (data > 254))
	{
		//将BOOT配置参数修改为默认值
		data = 50;
		sFlashBootPara[eBOOT_SCI0_ADDR_L_ID] = data & 0xff;
		sFlashBootPara[eBOOT_SCI0_ADDR_H_ID] = (data >> 8) & 0xff;

		/*记录BOOT配置RS485-0通信站地址修改标志*/
		BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 3);

		//检测失败(需重写FLASH)
		result = FALSE;
	}

    /*BOOT配置RS485-0通信波特率参数*/
	data = ((u16)sFlashBootPara[eBOOT_SCI0_RATE_H_ID] << 8) | sFlashBootPara[eBOOT_SCI0_RATE_L_ID];
	//参数无效
	if((4800 != data) && (9600 != data) && (19200 != data) && (38400 != data))
	{
		//将BOOT配置参数修改为默认值
		data = SCI0_BPS;
		sFlashBootPara[eBOOT_SCI0_RATE_L_ID] = data & 0xff;
		sFlashBootPara[eBOOT_SCI0_RATE_H_ID] = (data >> 8) & 0xff;

		/*记录BOOT配置RS485-0通信波特率修改标志*/
		BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 4);

		//检测失败(需重写FLASH)
		result = FALSE;
	}

    //RS485-0口Modbus初始化和串口复位
    //ModbusStationAllInit(eSCI0, data, eModbus_Server, (((u16)sFlashBootPara[eBOOT_SCI0_ADDR_H_ID] << 8) | sFlashBootPara[eBOOT_SCI0_ADDR_L_ID]));
    //(void)DEVSCIResetInit(eSCI0, data);

    /*BOOT配置RS485-1通信站地址参数*/
	data = ((u16)sFlashBootPara[eBOOT_SCI1_ADDR_H_ID] << 8) | sFlashBootPara[eBOOT_SCI1_ADDR_L_ID];
	//参数无效
	if((data < 1) || (data > 254))
	{
		//将BOOT配置参数修改为默认值
		data = 70;
		sFlashBootPara[eBOOT_SCI1_ADDR_L_ID] = data & 0xff;
		sFlashBootPara[eBOOT_SCI1_ADDR_H_ID] = (data >> 8) & 0xff;

		/*记录BOOT配置RS485-1通信站地址修改标志*/
		BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 5);

		//检测失败(需重写FLASH)
		result = FALSE;
	}

    /*BOOT配置RS485-1通信波特率参数*/
	data = ((u16)sFlashBootPara[eBOOT_SCI1_RATE_H_ID] << 8) | sFlashBootPara[eBOOT_SCI1_RATE_L_ID];
	//参数无效
	if((4800 != data) && (9600 != data) && (19200 != data) && (38400 != data))
	{
		//将BOOT配置参数修改为默认值
		data = SCI1_BPS;
		sFlashBootPara[eBOOT_SCI1_RATE_L_ID] = data & 0xff;
		sFlashBootPara[eBOOT_SCI1_RATE_H_ID] = (data >> 8) & 0xff;

		/*记录BOOT配置RS485-1通信波特率修改标志*/
		BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 6);

		//检测失败(需重写FLASH)
		result = FALSE;
	}

    //RS485-1口Modbus初始化和串口复位
    //ModbusStationAllInit(eSCI1, data, eModbus_Server, (((u16)sFlashBootPara[eBOOT_SCI1_ADDR_H_ID] << 8) | sFlashBootPara[eBOOT_SCI1_ADDR_L_ID]));
    //(void)DEVSCIResetInit(eSCI1, data);

    /*BOOT配置RS485-2通信站地址参数*/
	data = ((u16)sFlashBootPara[eBOOT_SCI2_ADDR_H_ID] << 8) | sFlashBootPara[eBOOT_SCI2_ADDR_L_ID];
	//参数无效
	if((data < 1) || (data > 254))
	{
		//将BOOT配置参数修改为默认值
		data = 100;
		sFlashBootPara[eBOOT_SCI2_ADDR_L_ID] = data & 0xff;
		sFlashBootPara[eBOOT_SCI2_ADDR_H_ID] = (data >> 8) & 0xff;

		/*记录BOOT配置RS485-2通信站地址修改标志*/
		BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 7);

		//检测失败(需重写FLASH)
		result = FALSE;
	}

    /*BOOT配置RS485-2通信波特率参数*/
	data = ((u16)sFlashBootPara[eBOOT_SCI2_RATE_H_ID] << 8) | sFlashBootPara[eBOOT_SCI2_RATE_L_ID];
	//参数无效
	if((4800 != data) && (9600 != data) && (19200 != data) && (38400 != data))
	{
		//将BOOT配置参数修改为默认值
		data = SCI2_BPS;
		sFlashBootPara[eBOOT_SCI2_RATE_L_ID] = data & 0xff;
		sFlashBootPara[eBOOT_SCI2_RATE_H_ID] = (data >> 8) & 0xff;

		/*记录BOOT配置RS485-0通信站地址修改标志*/
		BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_H_ID], 0);

		//检测失败(需重写FLASH)
		result = FALSE;
	}

    //RS485-2口Modbus初始化和串口复位
    //ModbusStationAllInit(eSCI2, data, eModbus_Server, (((u16)sFlashBootPara[eBOOT_SCI2_ADDR_H_ID] << 8) | sFlashBootPara[eBOOT_SCI2_ADDR_L_ID]));
    //(void)DEVSCIResetInit(eSCI2, data);

	return(result);
}

/*=================================================================================================
** @Name      : static void CfgBootHWParaHandle(u8 index, u16 data)
** @Input     : index:配置索引 data:配置目标值
** @Output    : void
** @Function  : 接收BOOT程序配置硬件参数处理
** @The notes : 记录到BOOT参数区和APP参数区
**===============================================================================================*/
static void CfgBootHWParaHandle(u8 index, u16 data)
{
	/*参数配置标志未清零*/
	if((0xff == sFlashBootPara[eBOOT_HW_CFGBIT_L_ID])
		&& (0xff == sFlashBootPara[eBOOT_HW_CFGBIT_H_ID]))
	{
		/*参数配置标志清零*/
		sFlashBootPara[eBOOT_HW_CFGBIT_L_ID] = 0;
		sFlashBootPara[eBOOT_HW_CFGBIT_H_ID] = 0;
	}

    /*BOOT配置CAN0通信波特率参数*/
	if(1 == index)
	{
		//参数有效
		if((125 == data) || (250 == data) || (500 == data) || (1000 == data))
		{
			//将BOOT配置参数更新到APP参数
			//(void)ParaWriteStoreSystemHWParaInfo(eSHWPara106_CAN0Rate, &data, 1);

			//BOOT参数有效才能配置BOOT参数区
			//if(LOAD_APP_PARA_FLAG == (((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID]))
			{
				//参数标志(BOOT必须写参数)
				sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
				sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

				//将配置参数更新到BOOT参数
				sFlashBootPara[eBOOT_CAN0_RATE_L_ID] = data & 0xff;
				sFlashBootPara[eBOOT_CAN0_RATE_H_ID] = (data >> 8) & 0xff;

				/*记录BOOT配置CAN0波特率修改标志*/
				BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 0);

				//参数重新写Flash
				(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
				BSPWatchDogOutput();

				(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
				BSPWatchDogOutput();
			}
		}
	}
    /*BOOT配置CAN1通信波特率参数*/
	else if(2 == index)
	{
		//参数有效
		if((125 == data) || (250 == data) || (500 == data) || (1000 == data))
		{
			//将BOOT配置参数更新到APP参数
			//(void)ParaWriteStoreSystemHWParaInfo(eSHWPara106_CAN1Rate, &data, 1);

			//BOOT参数有效才能配置BOOT参数区
			//if(LOAD_APP_PARA_FLAG == (((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID]))
			{
				//参数标志(BOOT必须写参数)
				sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
				sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

				//将配置参数更新到BOOT参数
				sFlashBootPara[eBOOT_CAN1_RATE_L_ID] = data & 0xff;
				sFlashBootPara[eBOOT_CAN1_RATE_H_ID] = (data >> 8) & 0xff;

				/*设置BOOT配置CAN1波特率标志*/
				BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 1);

				//参数重新写Flash
				(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
				BSPWatchDogOutput();

				(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
				BSPWatchDogOutput();
			}
		}
	}
    /*BOOT配置CAN2通信波特率参数*/
	else if(3 == index)
	{
		//参数有效
		if((125 == data) || (250 == data) || (500 == data) || (1000 == data))
		{
			//将BOOT配置参数更新到APP参数
			//(void)ParaWriteStoreSystemHWParaInfo(eSHWPara106_CAN2Rate, &data, 1);

			//BOOT参数有效才能配置BOOT参数区
			//if(LOAD_APP_PARA_FLAG == (((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID]))
			{
				//参数标志(BOOT必须写参数)
				sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
				sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

				//将配置参数更新到BOOT参数
				sFlashBootPara[eBOOT_CAN2_RATE_L_ID] = data & 0xff;
				sFlashBootPara[eBOOT_CAN2_RATE_H_ID] = (data >> 8) & 0xff;

				/*设置BOOT配置CAN2波特率标志*/
				BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 2);

				//参数重新写Flash
				(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
				BSPWatchDogOutput();

				(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
				BSPWatchDogOutput();
			}
		}
	}
    /*BOOT配置RS485-0通信站地址参数*/
	else if(4 == index)
	{
		//参数有效
		if((data >= 1) && (data <= 254))
		{
			//将BOOT配置参数更新到APP参数
			//(void)ParaWriteStoreSystemHWParaInfo(eSHWPara106_RemoteAddr, &data, 1);

			//BOOT参数有效才能配置BOOT参数区
			//if(LOAD_APP_PARA_FLAG == (((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID]))
			{
				//参数标志(BOOT必须写参数)
				sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
				sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

				//将配置参数更新到BOOT参数
				sFlashBootPara[eBOOT_SCI0_ADDR_L_ID] = data & 0xff;
				sFlashBootPara[eBOOT_SCI0_ADDR_H_ID] = (data >> 8) & 0xff;

				/*设置BOOT配置RS485-0波特率标志*/
				BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 3);

				//参数重新写Flash
				(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
				BSPWatchDogOutput();

				(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
				BSPWatchDogOutput();
			}
		}
	}
    /*BOOT配置RS485-0通信波特率参数*/
	else if(5 == index)
	{
		//参数有效
		if((4800 == data) || (9600 == data) || (19200 == data) || (38400 == data))
		{
			//将BOOT配置参数更新到APP参数
			//(void)ParaWriteStoreSystemHWParaInfo(eSHWPara106_RemoteRate, &data, 1);

			//BOOT参数有效才能配置BOOT参数区
			//if(LOAD_APP_PARA_FLAG == (((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID]))
			{
				//参数标志(BOOT必须写参数)
				sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
				sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

				//将配置参数更新到BOOT参数
				sFlashBootPara[eBOOT_SCI0_RATE_L_ID] = data & 0xff;
				sFlashBootPara[eBOOT_SCI0_RATE_H_ID] = (data >> 8) & 0xff;

				/*设置BOOT配置RS485-0波特率标志*/
				BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 4);

				//参数重新写Flash
				(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
				BSPWatchDogOutput();

				(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
				BSPWatchDogOutput();
			}
		}
	}
    /*BOOT配置RS485-1通信站地址参数*/
	else if(6 == index)
	{
		//参数有效
		if((data >= 1) && (data <= 254))
		{
			//将BOOT配置参数更新到APP参数
			//(void)ParaWriteStoreSystemHWParaInfo(eSHWPara106_LocalAddr, &data, 1);

			//BOOT参数有效才能配置BOOT参数区
			//if(LOAD_APP_PARA_FLAG == (((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID]))
			{
				//参数标志(BOOT必须写参数)
				sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
				sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

				//将配置参数更新到BOOT参数
				sFlashBootPara[eBOOT_SCI1_ADDR_L_ID] = data & 0xff;
				sFlashBootPara[eBOOT_SCI1_ADDR_H_ID] = (data >> 8) & 0xff;

				/*设置BOOT配置RS485-1站地址标志*/
				BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 5);

				//参数重新写Flash
				(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
				BSPWatchDogOutput();

				(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
				BSPWatchDogOutput();
			}
		}
	}
    /*BOOT配置RS485-1通信波特率参数*/
	else if(7 == index)
	{
		//参数有效
		if((4800 == data) || (9600 == data) || (19200 == data) || (38400 == data))
		{
			//将BOOT配置参数更新到APP参数
			//(void)ParaWriteStoreSystemHWParaInfo(eSHWPara106_LocalRate, &data, 1);

			//BOOT参数有效才能配置BOOT参数区
			//if(LOAD_APP_PARA_FLAG == (((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID]))
			{
				//参数标志(BOOT必须写参数)
				sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
				sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

				//将配置参数更新到BOOT参数
				sFlashBootPara[eBOOT_SCI1_RATE_L_ID] = data & 0xff;
				sFlashBootPara[eBOOT_SCI1_RATE_H_ID] = (data >> 8) & 0xff;

				/*设置BOOT配置RS485-1波特率标志*/
				BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 6);

				//参数重新写Flash
				(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
				BSPWatchDogOutput();

				(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
				BSPWatchDogOutput();
			}
		}
	}
    /*BOOT配置RS485-2通信站地址参数*/
	else if(8 == index)
	{
		//参数有效
		if((data >= 1) && (data <= 254))
		{
			//将BOOT配置参数更新到APP参数
			//(void)ParaWriteStoreSystemHWParaInfo(eSHWPara106_MonitAddr, &data, 1);

			//BOOT参数有效才能配置BOOT参数区
			//if(LOAD_APP_PARA_FLAG == (((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID]))
			{
				//参数标志(BOOT必须写参数)
				sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
				sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

				//将配置参数更新到BOOT参数
				sFlashBootPara[eBOOT_SCI2_ADDR_L_ID] = data & 0xff;
				sFlashBootPara[eBOOT_SCI2_ADDR_H_ID] = (data >> 8) & 0xff;

				/*设置BOOT配置RS485-2站地址标志*/
				BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_L_ID], 7);

				//参数重新写Flash
				(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
				BSPWatchDogOutput();

				(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
				BSPWatchDogOutput();
			}
		}
	}
    /*BOOT配置RS485-2通信波特率参数*/
	else if(9 == index)
	{
		//参数有效
		if((4800 == data) || (9600 == data) || (19200 == data) || (38400 == data))
		{
			//将BOOT配置参数更新到APP参数
			//(void)ParaWriteStoreSystemHWParaInfo(eSHWPara106_MonitRate, &data, 1);

			//BOOT参数有效才能配置BOOT参数区
			//if(LOAD_APP_PARA_FLAG == (((u16)sFlashBootPara[eBOOT_PARA_FLAG_H_ID] << 8) | sFlashBootPara[eBOOT_PARA_FLAG_L_ID]))
			{
				//参数标志(BOOT必须写参数)
				sFlashBootPara[eBOOT_PARA_FLAG_L_ID] = LOAD_APP_PARA_FLAG & 0xff;
				sFlashBootPara[eBOOT_PARA_FLAG_H_ID] = (LOAD_APP_PARA_FLAG >> 8) & 0xff;

				//将配置参数更新到BOOT参数
				sFlashBootPara[eBOOT_SCI2_RATE_L_ID] = data & 0xff;
				sFlashBootPara[eBOOT_SCI2_RATE_H_ID] = (data >> 8) & 0xff;

				/*设置BOOT配置RS485-2波特率标志*/
				BitSet(sFlashBootPara[eBOOT_HW_CFGBIT_H_ID], 0);

				//参数重新写Flash
				(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
				BSPWatchDogOutput();

				(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
				BSPWatchDogOutput();
			}
		}
	}
}

/*=============================================================================================
** @Name      : static u8 LoadCopyFlashEraseSector(u8 fstSec, u8 endSec)
** @Input     : fstSec:块内起始扇区号(包括) endSec:块内结束扇区号(不包括)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 在线升级擦除内部或外部FLASH备份程序扇区
** @The notes : 按扇区擦除FLASH
**===========================================================================================*/
static u8 LoadCopyFlashEraseSector(u8 fstSec, u8 endSec)
{
	u8 result = FALSE;

	/*程序备份到内部FLASH*/
	#if(0 == LOAD_APP_COPY_FLASH)
	result = BSPINTFLASHEraseBlock(2, fstSec, endSec);

	/*程序备份到外部FLASH*/
	#else
	u16 i = 0;
	u32 regAddr = 0;

	/*外部FLASH读写正常*/
	if((fstSec < endSec)
		&& (1 == DEVEXTFLASHGetReadyFlag()))
	{
		result = TRUE;
		for(i = 0; i < (endSec - fstSec); i++)
		{
			/*转化为外部FLASH地址*/
			regAddr = APP_PRO_INTCOPY_FLASH_ADDR + ((fstSec + i) * APP_PRO_COPY_FLASH_SEC_LEN);
			/*擦除外部FLASH*/
			result &= DEVEXTFLASHEraseSector(regAddr);
		}
	}
	#endif

	return(result);
}

/*=============================================================================================
** @Name      : static u8 LoadCopyFLASHWriteByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数[1, 1024](必须是8的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 在线升级按字节方式向内部或外部FLASH写入备份程序(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数
**===========================================================================================*/
static u8 LoadCopyFlashWriteByte(u32 addr, u8 *pData, u32 len)
{
	u16 i = 0;
	u16 num = 0;
	u32 index = 0;
	u8 result = FALSE;

	/*程序备份到内部FLASH*/
	#if(0 == LOAD_APP_COPY_FLASH)
	result = BSPINTFLASHWriteByte(addr, pData, len);

	/*程序备份到外部FLASH*/
	#else
	/*外部FLASH读写正常*/
	if(1 == DEVEXTFLASHGetReadyFlag())
	{
		result = TRUE;
		/*外部FLASH支持单次最大写256长度*/
		for(i = 0; i < ((len + 255) / 256); i++)
		{
			num = len - index;
			if(num > 256)
			{
				num = 256;
			}
			/*分多次写入外部FLASH*/
			result &= DEVEXTFLASHWriteByte(addr + index, &pData[index], num);
			index += num;
		}
	}
	#endif

	return(result);
}

/*=============================================================================================
** @Name      : static u8 LoadCopyFlashReadByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入FLASH地址[0,1048575](1M) *pData:读取到的数据指针 len:读取数据的个数[1, 1024]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 在线升级按字节方式从内部或外部FLASH读取备份程序(8位)
** @The notes : 驱动内部没有数据缓冲,要求读取过程数据指针对应的数据不能改变
**===========================================================================================*/
static u8 LoadCopyFlashReadByte(u32 addr, u8 *pData, u32 len)
{
	u8 result = FALSE;

	/*程序备份到内部FLASH*/
	#if(0 == LOAD_APP_COPY_FLASH)
	result = BSPINTFLASHReadByte(addr, pData, len);

	/*程序备份到外部FLASH*/
	#else
	/*外部FLASH读写正常*/
	if(1 == DEVEXTFLASHGetReadyFlag())
	{
		/*读取外部FLASH*/
		result = DEVEXTFLASHReadByte(addr, pData, len);
	}
	#endif

	return(result);
}

/*=============================================================================================
** @Name      : static u8 LoadDownFlashEraseSector(u8 fstSec, u8 endSec)
** @Input     : fstSec:块内起始扇区号(包括) endSec:块内结束扇区号(不包括)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 在线升级擦除内部或外部FLASH下载程序扇区
** @The notes : 按扇区擦除FLASH
**===========================================================================================*/
static u8 LoadDownFlashEraseSector(u8 fstSec, u8 endSec)
{
	u16 i = 0;
	u32 regAddr = 0;
	u8 result = FALSE;

	/*外部FLASH读写正常*/
	if((fstSec < endSec)
		&& (1 == DEVEXTFLASHGetReadyFlag()))
	{
		result = TRUE;
		for(i = 0; i < (endSec - fstSec); i++)
		{
			/*转化为外部FLASH地址*/
			regAddr = APP_PRO_EXTDOWN_FLASH_ADDR + (((u32)fstSec + i) * APP_PRO_DOWN_FLASH_SEC_LEN);
			/*擦除外部FLASH*/
			result &= DEVEXTFLASHEraseSector(regAddr);
		}
	}

	return(result);
}

/*=============================================================================================
** @Name      : static u8 LoadDownFlashWriteByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入FLASH地址([0,1048575](1M),必须是8的倍数,Flash一次写8Byte) *pData:写入数据指针 len:写入数据的个数[1, 1024](必须是8的倍数,Flash一次写8Byte)
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 远程升级按字节方式向外部FLASH写入下载程序(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变;要求地址和byte数必须是8的倍数
**===========================================================================================*/
static u8 LoadDownFlashWriteByte(u32 addr, u8 *pData, u32 len)
{
	u16 i = 0;
	u16 num = 0;
	u32 index = 0;
	u8 result = FALSE;

	/*外部FLASH读写正常*/
	if(1 == DEVEXTFLASHGetReadyFlag())
	{
		result = TRUE;
		/*外部FLASH支持单次最大写256长度*/
		for(i = 0; i < ((len + 255) / 256); i++)
		{
			num = len - index;
			if(num > 256)
			{
				num = 256;
			}
			/*分多次写入外部FLASH*/
			result &= DEVEXTFLASHWriteByte(addr + index, &pData[index], num);
			index += num;
		}
	}

	return(result);
}

/*=============================================================================================
** @Name      : static u8 LoadDownFlashReadByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入FLASH地址[0,1048575](1M) *pData:读取到的数据指针 len:读取数据的个数[1, 1024]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 远程升级按字节方式从外部FLASH读取下载程序(8位)
** @The notes : 驱动内部没有数据缓冲,要求读取过程数据指针对应的数据不能改变
**===========================================================================================*/
static u8 LoadDownFlashReadByte(u32 addr, u8 *pData, u32 len)
{
	u8 result = FALSE;

	/*外部FLASH读写正常*/
	if(1 == DEVEXTFLASHGetReadyFlag())
	{
		/*读取外部FLASH*/
		result = DEVEXTFLASHReadByte(addr, pData, len);
	}

	return(result);
}

/*=================================================================================================
** @Name      : static u8 LoadDownFlashReadCodeAddrLen(u16 index, u32 *rAddr, u32 *wAddr, u16 *len)
** @Input     : index:段索引号 *rAddr:读段代码起始地址(下载区FLASH) *wAddr:写段代码起始地址(运行区FLASH) *len:段代码长度
** @Output    : TRUE:读取成功 FALSE:读取失败
** @Function  : 读取下载FLASH区中程序段代码起始地址和段代码长度
** @The notes : 各段代码烧写地址和长度及代码流均记录在外部下载FLASH区
**===============================================================================================*/
static u8 LoadDownFlashReadCodeAddrLen(u16 index, u32 *rAddr, u32 *wAddr, u16 *len)
{
    u16 i = 0;
    u8 data[6] = {0};
    static u16 sHisIndex = 0;
    static u32 sHisRaddr = 0;
    static u32 sHisWaddr = 0;
    static u16 sHisLen = 0;

    /*起始首段*/
    if(0 == index)
    {
    	//下载区起始1扇区为总信息,第二扇区开始为段信息(前8Byte为地址/长度/CRC)
    	*rAddr = APP_PRO_EXTDOWN_FLASH_ADDR + APP_PRO_DOWN_FLASH_SEC_LEN + 8;

    	//读取记录的段起始地址和段长度(比代码起始地址后退8byte)
    	if(TRUE == LoadDownFlashReadByte(*rAddr - 8, data, 6))
    	{
    		*wAddr = (u32)data[3] | ((u32)data[2] << 8) | ((u32)data[1] << 16) | ((u32)data[0] << 24);
    		*len = (u16)data[5] | ((u16)data[4] << 8);

    		//读取到的写运行区地址或长度无效
    		if((*wAddr < APP_PRO_BLOCK1_FLASH_ADDR) || (*wAddr >= APP_PRO_BLOCK1_FIN_ADDR) || (*len > LOAD_APP_BUF_LEN))
    		{
        		return(FALSE);
    		}
    	}
    	else
    	{
    		return(FALSE);
    	}

    	//记录本段信息
    	sHisIndex = 0;
        sHisRaddr = *rAddr;
        sHisWaddr = *wAddr;
        sHisLen = *len;
    }
    /*段号跟上次一样*/
    else if(sHisIndex == index)
    {
    	//取上次值
    	*rAddr = sHisRaddr + 4;
    	*wAddr = sHisWaddr;
		*len = sHisLen;
    }
    //其他段号
    else
    {
        /*取段号在上次之前(强制认为上一次历史值取首段)*/
        if(index < sHisIndex)
        {
			//取首段起始信息
			sHisIndex = 0;
			sHisRaddr = APP_PRO_EXTDOWN_FLASH_ADDR + APP_PRO_DOWN_FLASH_SEC_LEN + 8;

			//读取记录的段起始地址和段长度(比代码起始地址后退8byte)
			if(TRUE == LoadDownFlashReadByte(sHisRaddr - 8, data, 6))
			{
				sHisWaddr = (u32)data[3] | ((u32)data[2] << 8) | ((u32)data[1] << 16) | ((u32)data[0] << 24);
	    		sHisLen = (u16)data[5] | ((u16)data[4] << 8);

	    		//读取到的写运行区地址无效
	    		if((sHisWaddr < APP_PRO_BLOCK1_FLASH_ADDR) || (sHisWaddr >= APP_PRO_BLOCK1_FIN_ADDR) || (sHisLen > LOAD_APP_BUF_LEN))
	    		{
	        		return(FALSE);
	    		}
			}
			else
			{
				return(FALSE);
			}
        }

    	//根据需要偏移段数逐段取本段长度往后递推
        for(i = 0; i < (index - sHisIndex); i++)
        {
        	//下一段地址=本段地址+本段长度+段头信息长度
        	*rAddr = sHisRaddr + sHisLen + 8;

        	//读取记录的段起始地址和段长度(比代码起始地址后退8byte)
			if(TRUE == LoadDownFlashReadByte(*rAddr - 8, data, 6))
			{
				*wAddr = (u32)data[3] | ((u32)data[2] << 8) | ((u32)data[1] << 16) | ((u32)data[0] << 24);
				*len = (u16)data[5] | ((u16)data[4] << 8);

				//读取到的写运行区地址无效
				if((*wAddr < APP_PRO_BLOCK1_FLASH_ADDR) || (*wAddr >= APP_PRO_BLOCK1_FIN_ADDR) || (*len > LOAD_APP_BUF_LEN))
				{
					return(FALSE);
				}
        	}
        	else
        	{
        		return(FALSE);
        	}

        	//记录本段信息
        	sHisIndex = index;
            sHisRaddr = *rAddr;
            sHisWaddr = *wAddr;
            sHisLen = *len;
        }
    }

    return(TRUE);
}

/*=================================================================================================
** @Name      : static void TimeDelay(u32 time)
** @Input     : time:循环次数
** @Output    : void
** @Function  : 延时处理函数
** @The notes :
**===============================================================================================*/
static void TimeDelay(u32 time)
{
    u32 i = 0;

    for(i = 0; i < time; i++)
    {
        ;
    }
}
