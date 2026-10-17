/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : LoadApp.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 在线升级通信包模块
** @Instructions : APP应用程序使用
**===============================================================================================*/
#ifndef _LOADAPP_H
#define _LOADAPP_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicInfo.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
#define LOAD_DEV_ID                 (BMU_DEV_ID)    /*设备识别码(个位设备类型:0:从控 1:高压模块 2:主控 3:堆控 4:主控辅助芯片)*/
#define LOAD_SW_VERSION             (BMU_SW_VERL)   /*程序软件版本号*/

#define LOAD_CAN_NUM                (3)             /*定义芯片的CAN总数*/

#define LOAD_APP_RUN_FLAG           (0x5A5A)        /*记录应用程序运行标志*/
#define LOAD_APP_TRY_FLAG           (0x5A5A)        /*记录应用程序尝试跳转标志*/
#define LOAD_APP_PARA_FLAG          (0x5A5A)        /*应用程序参数配置标志*/
#define LOAD_APP_COPY_FLAG          (0x5A)          /*应用程序备份有效标志*/
#define LOAD_APP_DOWN_FLAG          (0x5A)          /*应用程序下载有效标志*/
#define LOAD_APP_COPY_FLASH         (1)             /*应用程序备份FLASH选择 0:内部FLASH 1:外部FLASH*/

#define BOOT_ENRTY_POINT_FLASH_ADDR (0x0UL)         /*BOOT程序入口起始地址(存放在FLASH)*/
#define APP_ENRTY_POINT_FLASH_ADDR  (0x001000UL)    /*应用程序入口起始地址(存放在FLASH，中断向量表为U16,APP跳转地址不能超过0xffff)*/

#define BOOT_MCU_BASE_FLASH_ADDR    (0x0)           /*读写芯片FLASH起始地址(Flash读写接口地址偏移量)*/

#define APP_PRO_BLOCK1_FLASH_ADDR   (0x001000UL)    /*应用程序起始地址(以Flash块1地址0x001000为基地址,1个扇区4096Byte[4K])*/
#define APP_PRO_BLOCK1_CHECK_LEN    (8)             /*检测应用程序段有效的地址长度(不能小于8)*/

#if(0 == LOAD_APP_COPY_FLASH)                       /*程序备份到内部FLASH*/
#define APP_PRO_BLOCK1_FIN_ADDR     (0x063000UL)    /*应用程序结束地址(以Flash块1地址0x001000为基地址,1个扇区4096Byte[4K]*/

#define APP_PRO_INTCOPY_FLASH_ADDR  (0x063000UL)    /*应用程序备份在内部FLASH起始地址(以Flash块2地址0x063000为基地址,1个扇区4096Byte[4K]*/
#else                                               /*程序备份到外部FLASH*/
#define APP_PRO_BLOCK1_FIN_ADDR     (0x0C5000UL)    /*应用程序结束地址(以Flash块1地址0x001000为基地址,1个扇区4096Byte[4K]*/

#define APP_PRO_INTCOPY_FLASH_ADDR  (0x000000UL)    /*应用程序备份在外部FLASH起始地址(以Flash块0地址0x000000为基地址,1个扇区4096Byte[4K],可用区间不能小于APP运行区域*/
#define APP_PRO_COPY_FLASH_SEC_LEN  (4096)          /*应用程序备份在外部FLASH扇区长度[4K]*/
#endif

#define APP_PRO_EXTDOWN_FLASH_ADDR  (0x100000UL)    /*应用程序下载在外部FLASH起始地址(以Flash块1地址0x100000为基地址,1个扇区4096Byte[4K],可用区间不能小于APP运行区域*/
#define APP_PRO_DOWN_FLASH_SEC_LEN  (4096)          /*应用程序下载在外部FLASH扇区长度[4K]*/

#define BOOT_PARA_BLOCK4_FLASH_ADDR (0x0F7000UL)    /*Boot参数写FLASH起始地址(以Flash块4地址0x0F7000为基地址,1个扇区4096Byte[4K])*/

/*以下APP通信包有效,用于初始化在线升级通信设备地址*/
#define APP_PARA_GEN_EEP_ADDR       (1000)          /*应用常规参数写EEPROM起始地址(记录设备地址有效标志地址)*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/
typedef struct
{
    u8 canNum;                              /*CAN通道号*/
    u8 canType;                             /*CAN通道类型,0:上行通道 1:下行通道*/
}s_LoadRouteCfg;

typedef enum                                /*CAN通道路由类型*/
{
    eCANROUTE_UP = 0,                       /*上行通道*/
    eCANROUTE_DN = 1,                       /*下行通道*/
}e_CanRouteType;

typedef enum                                /*芯片类型*/
{
    eNXP_S32K146 = 0,                      /*S32K146*/
    eNXP_KEA128 = 1,                       /*KEA128*/
}e_McuType;

typedef enum                                /*功能码*/
{
    LOAD_REQ_FUN = 13,                      /*升级查询*/
    LOAD_AEQ_FUN = 14,                      /*升级回复*/
    LOAD_RUP_FUN = 15,                      /*升级修改*/
}e_MsgFunCode;

typedef enum                                /*信息类型主索引*/
{
    eSEARCHDEV_MSG = 0xf0,                  /*广播查询设备报文*/
    eHANDLE_MSG    = 0xf1,                  /*升级握手报文*/
    eLOADSTART_MSG = 0xf2,                  /*启动升级传输报文*/
    eLOADDATA_MSG  = 0xf3,                  /*发送升级数据报文*/
    eLOADFIN_MSG   = 0xf4,                  /*结束升级传输报文*/

    eREMSTATE_MSG  = 0xfa,                  /*远程升级状态报文*/
    eREMHAND_MSG   = 0xfb,                  /*远程升级握手报文*/
    eREMDATA_MSG   = 0xfd,                  /*远程升级数据报文*/
    eREMFIN_MSG    = 0xfe,                  /*远程升级结束报文*/
}e_MsgCmdType;

typedef enum                                /*握手命令*/
{
    eLoCmd_ParaCfg = 1,                     /*设备参数设置命令*/
    eLoCmd_ProgRes = 2,                     /*设备程序复位命令*/
    eLoCmd_ChanMod = 3,                     /*切换工作模式命令*/
    eLoCmd_ProRdWr = 4,                     /*程序读写命令(安装或备份)*/
    eLoCmd_LoaCtrl = 5,                     /*程序升级命令*/

    eLoCmd_IntRemTra = 10,                  /*请求远程传输*/
    eLoCmd_IntRemLoa = 11,                  /*请求远程升级*/
    eLoCmd_ExtRemLoa = 12,                  /*退出远程升级*/
    eLoCmd_ReqInst   = 13,                  /*请求安装程序*/
    eLoCmd_EndInst   = 14,                  /*终止安装程序*/
}e_HandleCmdType;

typedef enum                                /*操作命令*/
{
    eCtrlCmd_NorApp  = 0,                   /*切换正常应用模式命令*/
    eCtrlCmd_RemTra  = 1,                   /*切换远程传输模式命令*/
    eCtrlCmd_LocBoot = 3,                   /*切换本地升级模式命令*/
    eCtrlCmd_RemBoot = 4,                   /*切换本地升级模式命令*/

    eCtrlCmd_ReqInst = 1,                   /*请求安装命令(下载FLASH到内部FLASH)*/
    eCtrlCmd_EndInst = 2,                   /*终止安装命令*/
    eCtrlCmd_ReqCopy = 3,                   /*请求备份命令(内部FLASH到备份FLASH)*/
    eCtrlCmd_EndCopy = 4,                   /*终止备份命令*/
    eCtrlCmd_ReqBack = 5,                   /*请求回滚命令(备份FLASH到内部FLASH)*/
    eCtrlCmd_EndBack = 6,                   /*终止回滚命令*/

    eCtrlCmd_ReqLoad = 1,                   /*请求升级命令(上位机到设备)*/
    eCtrlCmd_EndLoad = 2                    /*终止升级命令*/
}e_CtrlCmdType;

typedef enum                                /*工作模式*/
{
    eLoMod_NorApp = 0,                      /*0:应用正常模式(APP)*/
    eLoMod_RemTra,                          /*1:远程传输模式(APP)*/
    eLoMod_NotApp,                          /*2:应用缺失模式(BOOT)*/
    eLoMod_LocBoot,                         /*3:本地升级模式(BOOT)*/
    eLoMod_RemBoot,                         /*4:远程升级模式(BOOT)*/

    eLoMod_Fin,                             /*工作模式结束*/
}e_LoadWorkMode;

typedef enum                                /*工作状态*/
{
    eLoSta_Route = 0,                       /*0:通信路由状态*/
    eLoSta_HandWait,                        /*1:等待握手状态(切换到可升级模式)*/
    eLoSta_HandSucc,                        /*2:握手成功状态(匹配固件识别码成功)*/
    eLoSta_HandFail,                        /*3:握手失败状态(匹配固件识别码失败)*/
    eLoSta_ErasSucc,                        /*4:擦除成功状态(擦除备份Flash成功)*/
    eLoSta_ErasFail,                        /*5:擦除失败状态(擦除备份Flash失败)*/
    eLoSta_LoaStart,                        /*6:开始升级状态(接收到段数据开始升级数据)*/
    eLoSta_LoaTrans,                        /*7:正在传输状态(下载固件数据流)*/
    eLoSta_TranSucc,                        /*8:传输成功*/
    eLoSta_TranFail,                        /*9:传输丢帧*/
    eLoSta_WritSucc,                        /*10:写入成功(写备份/内部Flash成功)*/
    eLoSta_WritFail,                        /*11:写入失败(写备份/内部Flash失败)*/
    eLoSta_WaitInst,                        /*12:等待安装(远程升级传输完成)*/
    eLoSta_FormSucc,                        /*13:格式化成功(擦除内部Flash成功)*/
    eLoSta_FormFail,                        /*14:格式化失败(擦除内部Flash失败)*/
    eLoSta_DoInstal,                        /*15:正在安装(从下载Flash拷贝到运行FLASH)*/
    eLoSta_InstSucc,                        /*16:安装成功*/
    eLoSta_InstFail,                        /*17:安装失败*/
    eLoSta_WaitCopy,                        /*18:等待备份(本地升级传输完成)*/
    eLoSta_Copying,                         /*19:正在备份(从运行Flash拷贝到备份FLASH)*/
    eLoSta_CopySucc,                        /*20:备份成功*/
    eLoSta_CopyFail,                        /*21:备份失败*/
    eLoSta_NotProg,                         /*22:无源程序*/
    eLoSta_Backing,                         /*23:正在回滚(从备份Flash拷贝到运行FLASH)*/
    eLoSta_BackSucc,                        /*24:回滚成功*/
    eLoSta_BackFail,                        /*25:回滚失败*/

}e_LoadWorkState;


/*按照小端模式(低位在前)存储数据*/
typedef enum                                /*BOOT参数写FLASH(扇区0)存放序号*/
{
    eBOOT_PARA_FLAG_L_ID = 0,               /*BOOT参数配置标志低8位序号*/
    eBOOT_PARA_FLAG_H_ID = 1,               /*BOOT参数配置标志高8位序号*/
    eBOOT_EDV_ECU_L_ID = 2,                 /*BOOT配置设备地址低8位序号*/
    eBOOT_EDV_ECU_H_ID = 3,                 /*BOOT配置设备地址高8位序号*/

    eBOOT_JUM_FLAG_MODE_ID = 4,             /*程序跳转标志工作模式序号*/
    eBOOT_JUM_FLAG_STAT_ID = 5,             /*程序跳转标志工作状态序号*/
    eBOOT_APP_RUN_FLAG_L_ID = 6,            /*应用程序运行标志低8位序号*/
    eBOOT_APP_RUN_FLAG_H_ID = 7,            /*应用程序运行标志高8位序号*/
    eBOOT_APP_TRY_FLAG_L_ID = 8,            /*应用程序尝试跳转标志低8位序号*/
    eBOOT_APP_TRY_FLAG_H_ID = 9,            /*应用程序尝试跳转标志高8位序号*/
    eBOOT_APP_COPY_FLAG_ID = 10,            /*应用程序备份有效标志序号*/
    eBOOT_APP_DOWN_FLAG_ID = 11,            /*应用程序下载有效标志序号*/

    eBOOT_HW_CFGBIT_L_ID = 12,              /*硬件参数配置记录位低8位序号*/
    eBOOT_HW_CFGBIT_H_ID = 13,              /*硬件参数配置记录位高8位序号*/
    eBOOT_CAN0_RATE_L_ID = 14,              /*硬件参数CAN0波特率低8位序号*/
    eBOOT_CAN0_RATE_H_ID = 15,              /*硬件参数CAN0波特率高8位序号*/
    eBOOT_CAN1_RATE_L_ID = 16,              /*硬件参数CAN1波特率低8位序号*/
    eBOOT_CAN1_RATE_H_ID = 17,              /*硬件参数CAN1波特率高8位序号*/
    eBOOT_CAN2_RATE_L_ID = 18,              /*硬件参数CAN2波特率低8位序号*/
    eBOOT_CAN2_RATE_H_ID = 19,              /*硬件参数CAN2波特率高8位序号*/
    eBOOT_SCI0_ADDR_L_ID = 20,              /*硬件参数RS485-0站地址低8位序号*/
    eBOOT_SCI0_ADDR_H_ID = 21,              /*硬件参数RS485-0站地址高8位序号*/
    eBOOT_SCI0_RATE_L_ID = 22,              /*硬件参数RS485-0波特率低8位序号*/
    eBOOT_SCI0_RATE_H_ID = 23,              /*硬件参数RS485-0波特率高8位序号*/
    eBOOT_SCI1_ADDR_L_ID = 24,              /*硬件参数RS485-1站地址低8位序号*/
    eBOOT_SCI1_ADDR_H_ID = 25,              /*硬件参数RS485-1站地址高8位序号*/
    eBOOT_SCI1_RATE_L_ID = 26,              /*硬件参数RS485-1波特率低8位序号*/
    eBOOT_SCI1_RATE_H_ID = 27,              /*硬件参数RS485-1波特率高8位序号*/
    eBOOT_SCI2_ADDR_L_ID = 28,              /*硬件参数RS485-2站地址低8位序号*/
    eBOOT_SCI2_ADDR_H_ID = 29,              /*硬件参数RS485-2站地址高8位序号*/
    eBOOT_SCI2_RATE_L_ID = 30,              /*硬件参数RS485-2波特率低8位序号*/
    eBOOT_SCI2_RATE_H_ID = 31,              /*硬件参数RS485-2波特率高8位序号*/

    eBOOT_PARA_FLASH_ID_LEN = 32            /*Boot参数写FLASH(扇区0)长度(写FLASH长度必须是8的倍数)*/
}e_BootParaFlashID;

typedef enum                                /*应用程序参数写FLASH扇区2存放序号*/
{
    eAPP_PARA_FLAG_L_ID = 0,                /*应用程序参数配置标志低8位序号*/
    eAPP_PARA_FLAG_H_ID = 1,                /*应用程序参数配置标志高8位序号*/
    eAPP_DEV_ECU_L_ID = 2,                  /*应用程序设备地址低8位序号*/
    eAPP_DEV_ECU_H_ID = 3,                  /*应用程序设备地址高8位序号*/
    eAPP_PARA_SEC2_ID_LEN                   /*应用程序参数写FLASH扇区2长度*/
}e_AppParaFlashID;

typedef enum                                /*应用程序硬件参数写FLASH扇区0存放序号*/
{
    eAPP_HWPARA_FLAG_L_ID = 0,              /*应用程序硬件参数配置标志低8位序号*/
    eAPP_HWPARA_FLAG_H_ID = 1,              /*应用程序硬件参数配置标志高8位序号*/
    eAPP_CAN0_RATE_L_ID = 4,                /*应用程序CAN0波特率低8位序号*/
    eAPP_CAN0_RATE_H_ID = 5,                /*应用程序CAN0波特率高8位序号*/
    eAPP_CAN1_RATE_L_ID = 6,                /*应用程序CAN1波特率低8位序号*/
    eAPP_CAN1_RATE_H_ID = 7,                /*应用程序CAN1波特率高8位序号*/
    eAPP_CAN2_RATE_L_ID = 8,                /*应用程序CAN2波特率低8位序号*/
    eAPP_CAN2_RATE_H_ID = 9,                /*应用程序CAN2波特率高8位序号*/
    eAPP_SCI0_ADDR_L_ID = 10,               /*应用程序RS485-0站地址低8位序号*/
    eAPP_SCI0_ADDR_H_ID = 11,               /*应用程序RS485-0站地址高8位序号*/
    eAPP_SCI0_RATE_L_ID = 12,               /*应用程序RS485-0波特率低8位序号*/
    eAPP_SCI0_RATE_H_ID = 13,               /*应用程序RS485-0波特率高8位序号*/
    eAPP_SCI1_ADDR_L_ID = 14,               /*应用程序RS485-1站地址低8位序号*/
    eAPP_SCI1_ADDR_H_ID = 15,               /*应用程序RS485-1站地址高8位序号*/
    eAPP_SCI1_RATE_L_ID = 16,               /*应用程序RS485-1波特率低8位序号*/
    eAPP_SCI1_RATE_H_ID = 17,               /*应用程序RS485-1波特率高8位序号*/
    eAPP_SCI2_ADDR_L_ID = 18,               /*应用程序RS485-2站地址低8位序号*/
    eAPP_SCI2_ADDR_H_ID = 19,               /*应用程序RS485-2站地址高8位序号*/
    eAPP_SCI2_RATE_L_ID = 20,               /*应用程序RS485-2波特率低8位序号*/
    eAPP_SCI2_RATE_H_ID = 21,               /*应用程序RS485-2波特率高8位序号*/

    eAPP_HWPARA_SEC0_ID_LEN                 /*应用程序硬件参数写FLASH扇区0长度*/
}e_AppHWParaID;

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void BootLoaderInit(void)
** @Input     : void
** @Output    : void
** @Function  : 在线升级模块初始化
** @The notes : BOOT和APP有区别
**===============================================================================================*/
void BootLoaderInit(void);

/*=================================================================================================
** @Name      : void CanRcvLoadMsgHandleAPI(t_CANMSG *msg, u8 canNum)
** @Input     : msg:接收到的报文 canNum:接收CAN通道号
** @Output    : void
** @Function  : 在线升级模块CAN报文接收处理
** @The notes : CAN接收回调函数调用
**===============================================================================================*/
void CanRcvLoadMsgHandleAPI(t_CANMSG *msg, u8 canNum);

/*=================================================================================================
** @Name      : void CanSendRemLoadMsgAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 远程升级状态CAN报文发送接口(仅进入远程模式后发送)
** @The notes : 50ms周期
**===============================================================================================*/
void CanSendRemLoadMsgAPI(void);

/*=================================================================================================
** @Name      : void LoadAppFlashErasHandleAPI(void)
** @Input     : void
** @Output    : void
** @Function  : APP程序FLASH擦除处理
** @The notes : 擦除FLASH APP下载区,擦除区域较大时分次擦除(300ms周期调用)
**===============================================================================================*/
void LoadAppFlashErasHandleAPI(void);

/*=================================================================================================
** @Name      : u8 LoadGetRdWrFlashStateAPI(void)
** @Input     : void
** @Output    : TRUE:正在读写 FALSE:未读写
** @Function  : 获取在线升级读写FLASH状态
** @The notes : 在线升级读写FLASH时APP不能读写FLASH
**===============================================================================================*/
u8 LoadGetRdWrFlashStateAPI(void);

#endif
