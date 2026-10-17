/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CANInterface.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : CAN通信包用户接口处理
** @Instructions :
**===============================================================================================*/
#ifndef _CANINTERFACE_H
#define _CANINTERFACE_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
/*=======================================设备索引表声明宏定义=====================================*/
#define DeclareIndexTableEntry(acesstype, mindex, pdata, size, sIndexNum) \
{(u8)acesstype, (u8)mindex, (void*)pdata, (u8)size, (u8)sIndexNum}

/*=================================================================================================
** @Data type definition
**===============================================================================================*/
/*CAN通信功能码定义*/
typedef enum _CAN_FUNCODE_
{
    eCANFUN_VFT = 0,                    //对时帧
    eCANFUN_BRE = 1,                    //心跳帧
    eCANFUN_RUP = 2,                    //请求修改
    eCANFUN_AUP = 3,                    //回复修改
    eCANFUN_CUP = 4,                    //确认修改
    eCANFUN_REQ = 5,                    //请求查询
    eCANFUN_ARQ = 6,                    //回复查询
    eCANFUN_CRQ = 7,                    //确认查询
    eCANFUN_UPM = 8,                    //主动更新(长帧)
    eCANFUN_ERR = 9,                    //读写错误应答
    eCANFUN_UPS = 10,                   //单帧上传(单帧)
    eCANFUN_UPC = 11,                   //单体上传(单帧)
    eCANFUN_UPP = 12,                   //参数上传(单帧)
    //eCANFUN_LOR = 13,                 //升级查询(单帧)[通信包不支持]
    //eCANFUN_LOA = 14,                 //升级回复(单帧)[通信包不支持]
    //eCANFUN_LOW = 15,                 //升级写入(单帧)[通信包不支持]
}e_CANFunCode;

/*CAN通信帧报文定义*/
typedef struct _CAN_FRAM_
{
    u32 ID;                             //帧标识
    u8 DLC;                             //长度
    u8 Data[8];                         //数据
}t_CANFram;

/*CAN通信ID规约定义*/
typedef union _CAN_ID_
{
    u32 Word;
    struct
    {
        u16          saddr    :8;       //源地址
        u16          daddr    :8;       //目的地址
        u16          mindex   :8;       //主索引
        u16          attrib   :1;       //帧属性
        e_CANFunCode funcode  :4;       //功能码
        u16          rsv      :3;       //保留
    }Bits;
}u_CANId;

/*CAN通信多帧传输命令字*/
typedef union _CAN_CMDBYTE_
{
    u8 Byte;
    struct
    {
        u16 NUM : 4;                    //子索引数目

        u16 CON : 2;                    //确认标志 1表示需要确认 0表示不需要
        u16 FIR : 1;                    //首帧标志位，1表示首帧
        u16 FIN : 1;                    //最后帧标志位，1表示最后帧
    }Bits;
}u_CANCmdByte;

/*CAN通信包计时结构体*/
typedef struct _CAN_TIMER_
{
    u16 timeResend;                     //当前剩余时间
    u16 timeElasped;                    //已消耗的时间
}t_CANTimer;

/*CAN包主索引表*/
typedef struct _CANINDEX_TAB_
{
    u8 acceType;                        //读写属性 0：只读 ，0x01:CAN通道0可写，第N位为1表示CAN通道N可写
    u8 mIndex;                          //主索引
    void *pObject;                      //数据地址
    u8 dataType;                        //数据类型
    u8 sIndexNum;                       //子索引数目
}t_CANIndexTab;

/*CAN通信结构体设备表*/
typedef struct _CANDEV_TAB_
{
    u8 devAdrr;                         //设备地址
    u8 mIndexNum;                       //主索引数目
    const t_CANIndexTab *pIndexTable;   //主索引表指针
}t_CANDevTab;

/*CAN通信收发交互结构体*/
typedef struct _CANTXRCV_TAB_
{
    u8 ready;                           //就绪标志 0初始化，1发送就绪 2等待确认
    e_CANFunCode funcode;               //功能码
    u16 attrib;                         //帧属性
    void *pdata;                        //数据首地址
    u8 dataType;                        //数据类型
    u8 mIndex;                          //主索引
    u8 beginIndex;                      //起始子索引
    u8 sIndex;                          //当前帧子索引
    u8 nextsIndex;                      //下一帧子索引
    u8 sIndexNum;                       //子索引数目
    u8 remframe;                        //剩余帧数目
    u8 daddr;                           //目的地址
    u8 cannum;                          //CNA通道
    u_CANCmdByte cmd;                   //命令字
    t_CANTimer timer;                   //定时器
}t_CANTxRcvTab;

/*CAN通信包周期上传发送表定义*/
typedef struct _CANTXCYC_TAB_
{
    void *pdata;                        //发送数据地址
    u8 dataType;                        //数据类型
    u8 funcode;                         //功能码
    u8 index;                           //所在配置表索引
    u32 nextTime;                       //下次发送时间
}t_CANTxCycTab;

/*CAN通信包主索引映射参数定义*/
typedef struct _CAN_INDEXMAP_
{
    u8 map90Index;                      /*主索引90映射起始主索引(单体充电均衡状态)*/
    u8 map91Index;                      /*主索引91映射起始主索引(单体放电均衡状态)*/
    u8 map92Index;                      /*主索引92映射起始主索引(单体电压信息1)*/
    u8 map93Index;                      /*主索引93映射起始主索引(单体电压信息2)*/
    u8 map94Index;                      /*主索引94映射起始主索引(单体温度信息1)*/
    u8 map95Index;                      /*主索引94映射起始主索引(单体温度信息2)*/

    u8 map100Index;                     /*主索引100映射起始主索引(系统通用参数)*/
    u8 map101Index;                     /*主索引101映射起始主索引(系统硬件参数)*/
    u8 map102Index;                     /*主索引102映射起始主索引(设备通用参数)*/
    u8 map103Index;                     /*主索引103映射起始主索引(设备高级参数)*/
    u8 map104Index;                     /*主索引104映射起始主索引(设备硬件参数)*/

    u8 index90Type;                     /*主索引90数据类型(单体充电均衡状态)*/
    u8 index91Type;                     /*主索引91数据类型(单体放电均衡状态)*/
    u8 index92Type;                     /*主索引92数据类型(单体电压信息1)*/
    u8 index93Type;                     /*主索引93数据类型(单体电压信息2)*/
    u8 index94Type;                     /*主索引94数据类型(单体温度信息1)*/
    u8 index95Type;                     /*主索引95数据类型(单体温度信息2)*/

    u8 index100Type;                    /*主索引100数据类型(系统通用参数)*/
    u8 index101Type;                    /*主索引101数据类型(系统硬件参数)*/
    u8 index102Type;                    /*主索引102数据类型(设备通用参数)*/
    u8 index103Type;                    /*主索引103数据类型(设备高级参数)*/
    u8 index104Type;                    /*主索引104数据类型(设备硬件参数)*/
}t_CANLibIndexMap;

/*CAN通信包主索引映射转换定义*/
typedef struct _CAN_MAPTRANS_
{
    u8 realMIndex;                       /*映射主索引转换成真实主索引值*/
    u8 realSIndex;                       /*映射主索引转换成真实子索引值*/
    u8 realSIndexNum;                    /*映射主索引转换成真实子索引数目*/
}t_CANMapTrans;

/*CAN通信错误代码定义*/
typedef enum _CAN_ERRCODE_
{
    eCANERR_SUCCESSFUL = 0,             /*正常*/
    eCANERR_NOSUCH_CAN = 1,             /*无该CAN通道*/
    eCANERR_NOSUCH_FUN = 2,             /*无该功能码*/
    eCANERR_NOSUCH_DEV = 3,             /*无该设备*/
    eCANERR_NOSUCH_MID = 4,             /*无该主索引*/
    eCANERR_SIDLEN_OVE = 5,             /*子索引超长*/
    eCANERR_NOSUCH_TAB = 6,             /*无该CAN收发处理通信表*/
    eCANERR_NOIDLE_TAB = 7,             /*无空闲CAN收发处理通信表*/
    eCANERR_CANPW_DIS = 8,              /*该CAN口禁止写入*/
    eCANERR_RUPW_FAIL = 9,              /*请求修改写入失败*/
    eCANERR_REQW_FAIL = 10,             /*查询回复写入失败*/
    eCANERR_CYCW_FAIL = 11,             /*周期更新写入失败*/
    eCANERR_RCVMSG_ERR = 12,            /*接收报文异常*/
    eCANERR_MSGCRC_ERR = 13,            /*报文CRC校验码错误*/
    eCANERR_CYCUP_DIS = 14,             /*禁止周期发送*/
    eCANERR_KEEP_FUN = 15               /*非法使用保留功能*/
}e_CANErrCode;

/*CAN通信错误原因定义*/
typedef enum _CAN_ERRREAS_
{
    eCANERRREAS_NORMAL = 0,             /*正常无原因*/
    eCANERRREAS_INDEXERR = 1,           /*读写索引非法(包括长度)*/
    eCANERRREAS_CRCERR = 2,             /*CRC校验错误*/
    eCANERRREAS_AUTHERR = 3,            /*写入权限非法*/
    eCANERRREAS_STATEERR = 4,           /*写入状态非法*/
    eCANERRREAS_RANGEERR = 5,           /*写入参数范围非法*/
}e_CANErrReas;

/*CAN通信收发状态定义*/
typedef enum _CAN_TABSTATE_
{
    eCANTAB_LEISUREL = 0,               /*空闲监听*/
    eCANTAB_READYOK = 1,                /*发送就绪*/
    eCANTAB_WAIT = 2,                   /*忙等待*/
}e_CANTRTabState;

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void BSPUserCfgInit(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信包所有初始化
** @The notes : 用户必须初始化调用
**===============================================================================================*/
void CANLibAllInit_API(void);

/*=================================================================================================
** @Name      : void CANDevTabEntryInit_API(u8 devAddr, u8 mIndexNum const s_CANIndexTab  *pIndexTab, u8 devId)
** @Input     : devAdd:设备地址 mIndexNum:主索引数目 pIndexTab:设备主动引表 devId:创建设备表序号
** @Output    : void
** @Function  : CAN通信创建设备表初始化
** @The notes : 用户配置CAN通信设备表信息
**===============================================================================================*/
extern void CANDevTabEntryInit_API(u8 devAddr, u8 mIndexNum, const t_CANIndexTab *pIndexTab, u8 devId);

/*=================================================================================================
** @Name      : u8 CANLibRcvMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : 处理结果(0:正常,其它:错误码)
** @Function  : CAN通信报文接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
extern u8 CANLibRcvMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum);

/*=================================================================================================
** @Name      : void CANLibHandleTask_API(void)
** @Input     : void
** @Output    : void
** @Function  : CAN所有处理任务
** @The notes : 用户必须5ms周期调用
**===============================================================================================*/
extern void CANLibHandleTask_API(void);

/*=================================================================================================
** @Name      : void CANLibTxRcvHandleTask_API(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信报文收发处理任务(收发通信表处理)
** @The notes : 用户必须1ms周期调用
**===============================================================================================*/
extern void CANLibTxRcvHandleTask_API(void);

/*=================================================================================================
** @Name      : u8 CANBuildSendVerMsg_API(u8 canNum)
** @Input     : canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送校时报文帧
** @The notes : 用户自行发送校时帧
**===============================================================================================*/
extern u8 CANBuildSendVerMsg_API(u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendBreMsg_API(u8 canNum)
** @Input     : canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送心跳报文帧
** @The notes : 用户自行发送心跳帧
**===============================================================================================*/
extern u8 CANBuildSendBreMsg_API(u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendRupMsg_API(u8 destAddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum)
** @Input     : destAddr:目标设备地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 conFlag:是否需要确认 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送请求修改报文帧
** @The notes : 用户自行发送请求修改报文帧
**===============================================================================================*/
extern u8 CANBuildSendRupMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendAupMsg_API(t_CANFram *canFram, u8 canNum)
** @Input     : *canFram:接收到的CAN通信帧 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送修改回复报文帧
** @The notes : 用户自行发送修改回复报文帧
**===============================================================================================*/
extern u8 CANBuildSendAupMsg_API(t_CANFram *canFram, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendCupMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : destAddr:目标设备地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 conFlag:是否需要确认 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送修改确认报文帧
** @The notes : 用户自行发送修改确认报文帧
**===============================================================================================*/
extern u8 CANBuildSendCupMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendReqMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 conFlag:是否需要确认 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送请求查询报文帧
** @The notes : 用户自行发送请求查询报文帧
**===============================================================================================*/
extern u8 CANBuildSendReqMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conFlag, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendArqMsg_API(t_CANFram *canFram, u8 canNum)
** @Input     : *canFram:接收到的CAN通信帧 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送查询回复报文帧
** @The notes : 用户自行发送查询回复报文帧
**===============================================================================================*/
extern u8 CANBuildSendArqMsg_API(t_CANFram *canFram, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendCrqMsg_API(t_CANFram *canFram, u8 canNum)
** @Input     : *canFram:接收到的CAN通信帧 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送查询确认报文帧
** @The notes : 用户自行发送查询确认报文帧
**===============================================================================================*/
extern u8 CANBuildSendCrqMsg_API(t_CANFram *canFram, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendUpmMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目  canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新长帧报文帧
** @The notes : 用户自行发送主动更新长帧报文帧
**===============================================================================================*/
extern u8 CANBuildSendUpmMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendUpsMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新单帧报文帧
** @The notes : 用户自行发送主动更新单帧报文帧
**===============================================================================================*/
extern u8 CANBuildSendUpsMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendUpcMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 mapIndex:映射起始主索引号 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新单体报文帧
** @The notes : 用户自行发送主动更新单体报文帧
**===============================================================================================*/
extern u8 CANBuildSendUpcMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANBuildSendUppMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 mapIndex:映射起始主索引号 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 组建和发送主动更新参数报文帧
** @The notes : 用户自行发送主动更新参数报文帧
**===============================================================================================*/
extern u8 CANBuildSendUppMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 mapIndex, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANRcvShieldDeal_Hook(u32 canID, u8 canNum)
** @Input     : canID:报文ID  canNum:CAN通道号
** @Output    : TURE:需屏蔽 FALSE:不需屏蔽
** @Function  : 接收到需屏蔽的报文用户处理
** @The notes : 用户需要时补充完善
**===============================================================================================*/
u8 CANRcvShieldDeal_Hook(u32 canID, u8 canNum);

/*=================================================================================================
** @Name      : void CANRcvVerUserDeal_Hook(u8 saddr, u8 canNum)
** @Input     : saddr:源地址  canNum:CAN通道号
** @Output    : void
** @Function  : 接收到校时报文后用户处理
** @The notes : 用户需要时补充完善
**===============================================================================================*/
void CANRcvVerUserDeal_Hook(u8 saddr, u8 canNum);

/*=================================================================================================
** @Name      : void CANRcvBreUserDeal_Hook(u8 saddr, u8 canNum)
** @Input     : saddr:源地址  canNum:CAN通道号
** @Output    : void
** @Function  : 接收到心跳报文后用户处理
** @The notes : 用户需要时补充完善
**===============================================================================================*/
void CANRcvBreUserDeal_Hook(u8 saddr, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANRcvRupUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : saddr:源地址  mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 canNum:CAN通道号
** @Output    : 修改执行结果:0:正常 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收到请求修改报文后用户处理
** @The notes : 用户需要时补充完善,对相应索引值进行处理
**===============================================================================================*/
u8 CANRcvRupUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum);

/*=================================================================================================
** @Name      : void CANRcvReqUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : saddr:源地址  mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 canNum:CAN通道号
** @Output    : void
** @Function  : 接收到请求查询报文后(查询回复前)用户处理
** @The notes : 用户需要时补充完善,更新相应索引值(不自动跟新的值)
**===============================================================================================*/
void CANRcvReqUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum);

/*=================================================================================================
** @Name      : void CANRcvCycUpUserDeal_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域 DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 接收到周期更新报文后用户处理
** @The notes : 用户需要时补充完善,处理接收周期发送的长帧、单帧、单体和参数信息
**===============================================================================================*/
void CANRcvCycUpUserDeal_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum);

/*=================================================================================================
** @Name      : void CANSendMsgUser_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域 DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : CAN通信包发送报文处理
** @The notes : 用户必须补充完善
**===============================================================================================*/
void CANSendMsgUser_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum);

/*=================================================================================================
** @Name      : u8 CANTxCycUpUserEnState_Hook(e_CANFunCode funCode, u8 mIndex, u8 canNum)
** @Input     : funCode:功能码  mIndex:主索引号 canNum:CAN通道号
** @Output    : 用户允许发送状态 TRUE:允许 FALSE:禁止
** @Function  : 判断用户允许发送指定主索引报文状态
** @The notes : 用户需要时补充完善,对指定主索引在指定状态下才允许发送(比如调试信息)
**===============================================================================================*/
u8 CANTxCycUpUserEnState_Hook(e_CANFunCode funCode, u8 mIndex, u8 canNum);

/*=================================================================================================
** @Name      : t_CANMapTrans CANIndexMapTransUser_Hook(e_CANFunCode funCode, u8 mIndex, u8 DLC)
** @Input     : funCode:功能码  mIndex:映射主索引号 DLC:报文长度
** @Output    : void
** @Function  : CAN通信包将映射报文信息转换成真实信息
** @The notes : 用户必须补充完善,单体信息和参数信息报文的映射
**===============================================================================================*/
t_CANMapTrans CANIndexMapTransUser_Hook(e_CANFunCode funCode, u8 mIndex, u8 DLC);

#endif
