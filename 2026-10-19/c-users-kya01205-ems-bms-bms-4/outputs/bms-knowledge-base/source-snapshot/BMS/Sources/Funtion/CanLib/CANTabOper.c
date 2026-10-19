/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CANTabOper.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : CAN通信包表格读写操作接口
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "CANHandle.h"
#include "CANInterface.h"
#include "CANUser.h"
#include "CANTabOper.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
/* SAE J1850 crc lookup table */
static const u8 Crc8Table[256] =
{
	0x00u, 0x1du, 0x3au, 0x27u, 0x74u, 0x69u, 0x4eu, 0x53u, 0xe8u, 0xf5u, 0xd2u, 0xcfu, 0x9cu, 0x81u, 0xa6u, 0xbbu,
	0xcdu, 0xd0u, 0xf7u, 0xeau, 0xb9u, 0xa4u, 0x83u, 0x9eu, 0x25u, 0x38u, 0x1fu, 0x02u, 0x51u, 0x4cu, 0x6bu, 0x76u,
	0x87u, 0x9au, 0xbdu, 0xa0u, 0xf3u, 0xeeu, 0xc9u, 0xd4u, 0x6fu, 0x72u, 0x55u, 0x48u, 0x1bu, 0x06u, 0x21u, 0x3cu,
	0x4au, 0x57u, 0x70u, 0x6du, 0x3eu, 0x23u, 0x04u, 0x19u, 0xa2u, 0xbfu, 0x98u, 0x85u, 0xd6u, 0xcbu, 0xecu, 0xf1u,
	0x13u, 0x0eu, 0x29u, 0x34u, 0x67u, 0x7au, 0x5du, 0x40u, 0xfbu, 0xe6u, 0xc1u, 0xdcu, 0x8fu, 0x92u, 0xb5u, 0xa8u,
	0xdeu, 0xc3u, 0xe4u, 0xf9u, 0xaau, 0xb7u, 0x90u, 0x8du, 0x36u, 0x2bu, 0x0cu, 0x11u, 0x42u, 0x5fu, 0x78u, 0x65u,
	0x94u, 0x89u, 0xaeu, 0xb3u, 0xe0u, 0xfdu, 0xdau, 0xc7u, 0x7cu, 0x61u, 0x46u, 0x5bu, 0x08u, 0x15u, 0x32u, 0x2fu,
	0x59u, 0x44u, 0x63u, 0x7eu, 0x2du, 0x30u, 0x17u, 0x0au, 0xb1u, 0xacu, 0x8bu, 0x96u, 0xc5u, 0xd8u, 0xffu, 0xe2u,
	0x26u, 0x3bu, 0x1cu, 0x01u, 0x52u, 0x4fu, 0x68u, 0x75u, 0xceu, 0xd3u, 0xf4u, 0xe9u, 0xbau, 0xa7u, 0x80u, 0x9du,
	0xebu, 0xf6u, 0xd1u, 0xccu, 0x9fu, 0x82u, 0xa5u, 0xb8u, 0x03u, 0x1eu, 0x39u, 0x24u, 0x77u, 0x6au, 0x4du, 0x50u,
	0xa1u, 0xbcu, 0x9bu, 0x86u, 0xd5u, 0xc8u, 0xefu, 0xf2u, 0x49u, 0x54u, 0x73u, 0x6eu, 0x3du, 0x20u, 0x07u, 0x1au,
	0x6cu, 0x71u, 0x56u, 0x4bu, 0x18u, 0x05u, 0x22u, 0x3fu, 0x84u, 0x99u, 0xbeu, 0xa3u, 0xf0u, 0xedu, 0xcau, 0xd7u,
	0x35u, 0x28u, 0x0fu, 0x12u, 0x41u, 0x5cu, 0x7bu, 0x66u, 0xddu, 0xc0u, 0xe7u, 0xfau, 0xa9u, 0xb4u, 0x93u, 0x8eu,
	0xf8u, 0xe5u, 0xc2u, 0xdfu, 0x8cu, 0x91u, 0xb6u, 0xabu, 0x10u, 0x0du, 0x2au, 0x37u, 0x64u, 0x79u, 0x5eu, 0x43u,
	0xb2u, 0xafu, 0x88u, 0x95u, 0xc6u, 0xdbu, 0xfcu, 0xe1u, 0x5au, 0x47u, 0x60u, 0x7du, 0x2eu, 0x33u, 0x14u, 0x09u,
	0x7fu, 0x62u, 0x45u, 0x58u, 0x0bu, 0x16u, 0x31u, 0x2cu, 0x97u, 0x8au, 0xadu, 0xb0u, 0xe3u, 0xfeu, 0xd9u, 0xc4u
};

extern t_CANDevTab gCanDevTable[];                                  /*CAN通信设备表*/
extern t_CANTxRcvTab gCanTxRcvTable[];                              /*CAN通信收发处理表*/

extern u16 gCANDevInfo_1[];                                         /*设备信息*/
extern u8 gCANBaseDef_3[];                                          /*通信包基本配置信息*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u8 CANFindDevTabMIndexDim(u8 findIndex, u8 mindexNum, const t_CANIndexTab *pindextable)
** @Input     : findIndex:查找的主索引号  mindexNum:主索引数目 *pindextable:所要查找的主索引表
** @Output    : 对应主索引所在位置
** @Function  : 查找主索引所在设备表中偏移位置(第几个主索引)
** @The notes : 二分查找法
**===============================================================================================*/
static u8 CANFindDevTabMIndexDim(u8 findIndex, u8 mindexNum, const t_CANIndexTab *pindextable);

/*=================================================================================================
** @Name      : static u8 CANCalcRemainFrameNum(u8 framSort, u8 dataType, u8 sIndexNum)
** @Input     : framSort:帧类型[0长帧 1单帧] dataType:主索引数据类型 sIndexNum:子索引数目
** @Output    : 剩余帧数
** @Function  : 计算剩余帧数
** @The notes : 根据数据类型和子索引数目计算
**===============================================================================================*/
static u8 CANCalcRemainFrameNum(u8 framSort, u8 dataType, u8 sIndexNum);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : const t_CANIndexTab *CanScanIndexTabInfo(u8 devaddr, u8 mindex, u8 sIndex, u8 sIndexNum, u8 *errorCode)
** @Input     : devaddr:索引表设备地址 mindex:主索引号  sindex:子索引号  sIndexNum:子索引数目 *errorCode:存储错误码地址
** @Output    : 对应主索引信息
** @Function  : 查找主索引表信息
** @The notes : 并且判断子索引是否越界
**===============================================================================================*/
const t_CANIndexTab *CanScanIndexTabInfo(u8 devaddr, u8 mindex, u8 sIndex, u8 sIndexNum, u8 *errorCode)
{
    u8 i = 0;
    u8 offset = 0;
    t_CANDevTab *pDeviceTable;
    const t_CANIndexTab *pIndexTable;

    *errorCode = eCANERR_SUCCESSFUL;

    for(i = 0; i < gCANBaseDef_3[eCANRBas3_DevNum]; i++)
    {
        //获得设备表
        pDeviceTable = &gCanDevTable[i];

        if(pDeviceTable == NULL)
        {
            //该设备不存在
            *errorCode = eCANERR_NOSUCH_DEV;

            return(NULL);
        }

        //广播报文当作自身报文
        if(0xff == devaddr)
        {
        	devaddr = gCANDevInfo_1[eCANDev1_Addr];
        }

        //找到当前设备表
        if(pDeviceTable->devAdrr == devaddr)
        {
            //二分查找主索引表
            offset = CANFindDevTabMIndexDim(mindex, pDeviceTable->mIndexNum, pDeviceTable->pIndexTable);

            /*未找到对应主索引*/
            if(offset == 0xff)
            {
                *errorCode = eCANERR_NOSUCH_MID;
            }
            else
            {
                //对应主索引所在位置
                pIndexTable = pDeviceTable->pIndexTable + offset;

                //判断查找的子索引是否越界
                if((pIndexTable->sIndexNum >= (sIndex + sIndexNum)) && (pIndexTable->sIndexNum > sIndex))
                {
                    //找到主索引表
                    return(pIndexTable);
                }
                else
                {
                    /*子索引越界*/
                    *errorCode = eCANERR_SIDLEN_OVE;
                }
            }
            break;
        }
    }

    if(i >= gCANBaseDef_3[eCANRBas3_DevNum])
    {
        //该设备不存在
        *errorCode = eCANERR_NOSUCH_DEV;
    }

    //未找到主索引表
    return(NULL);
}

/*=================================================================================================
** @Name      : void CANReadCanIndexData(const t_CANIndexTab *ptrTable, u8 subIndex, u8 sIndexNum, u8 *Data)
** @Input     : *ptrTable:主索引信息  sindex:子索引号  sIndexNum:子索引数目 *Data:读出的数据(多字节则低位在前)
** @Output    : void
** @Function  : 读索引表中对应子索引当前数据值
** @The notes : 必须确保主索引存在且子索引长度合法
**===============================================================================================*/
void CANReadCanIndexData(const t_CANIndexTab *ptrTable, u8 subIndex, u8 sIndexNum, u8 *Data)
{
    u8 i = 0;
    u8 dataType = 0;
    u16 Datatemp = 0;
    u32 Datatemp1 = 0;
    void *pObject = NULL;

    //主索引数据地址
    pObject = ptrTable->pObject;

    //主索引数据类型
    dataType = ptrTable->dataType;

    /*主索引有效*/
    if(pObject != NULL)
    {
        /*逐个读取各个子索引*/
        for(i = 0; i < sIndexNum; i++)
        {
            /*8位*/
        	if(dataType == 1)
            {
                pObject = (u8*)(ptrTable->pObject) + (subIndex + i);
                *(Data + i) = *((u8*)pObject);
            }
        	/*16位*/
            else if(dataType == 2)
            {
                pObject = (u16*)(ptrTable->pObject) + (subIndex + i);
                Datatemp = *((u16*)pObject);;
                *(Data + 2 * i) = (u8)(Datatemp & 0x00FF);
                *(Data + 2 * i + 1) = (u8)((Datatemp & 0xFF00) >> 8);
            }
        	/*32位*/
        	else if(dataType == 4)
            {
                pObject = (u32*)(ptrTable->pObject) + (subIndex + i);
                Datatemp1 = *((u32*)pObject);
                *(Data + 4 * i) = (u8)(Datatemp1 & 0x000000FF);
                *(Data + 4 * i + 1) = (u8)((Datatemp1 & 0x0000FF00) >> 8);
                *(Data + 4 * i + 2) = (u8)((Datatemp1 & 0x00FF0000) >> 16);
                *(Data + 4 * i + 3) = (u8)((Datatemp1 & 0xFF000000) >> 24);
            }
        }
    }
}

/*=================================================================================================
** @Name      : u8 CANWriteCanIndexData(const t_CANIndexTab *ptrTable, u8 subIndex, u8 sIndexNum, u8 *Data, u8 cannum)
** @Input     : *ptrTable:主索引信息  sindex:子索引号  sIndexNum:子索引数目 *Data:需写入的数据(多字节则低位在前) cannum:CAN通道号
** @Output    : 写结果(0:成功 其它:错误码)
** @Function  : 向索引表中对应子索引写入数据值
** @The notes : 必须确保主索引存在且子索引长度合法
**===============================================================================================*/
u8 CANWriteCanIndexData(const t_CANIndexTab *ptrTable, u8 subIndex, u8 sIndexNum, u8 *Data, u8 cannum)
{
    u8 i = 0;
    u8 dataType = 0;
    u16 Datatemp = 0;
    u32 Datatemp1 =0;
    void *pObject = NULL;

    //主索引数据地址
    pObject = ptrTable->pObject;

    //主索引数据类型
    dataType = ptrTable->dataType;

    //不允许该CAN口修改本主索引
    if(((ptrTable->acceType >> cannum) & 0x01) != 1)
    {
        return(eCANERR_CANPW_DIS);
    }

    if(pObject != NULL)
    {
        //逐个修改各个子索引
        for(i = 0; i < sIndexNum; i++)
        {
            /*8位*/
            if(dataType == 1)
            {
                pObject = (u8*)(ptrTable->pObject) + (subIndex + i);
                *((u8*)pObject) = *(Data + i);
            }
            else if(dataType == 2)
            {
                Datatemp = *(Data + 2 * i + 1);
                Datatemp = (Datatemp << 8) | *(Data + 2 * i);
                pObject = (u16*)(ptrTable->pObject) + (subIndex + i);
                *((u16*)pObject) = Datatemp;
            }
            else if(dataType == 4)
            {
                Datatemp1 = *(Data + 4 * i + 3);
                Datatemp1 = (Datatemp1 << 8) | *(Data + 4 * i + 2);
                Datatemp1 = (Datatemp1 << 8) | *(Data + 4 * i + 1);
                Datatemp1 = (Datatemp1 << 8) | *(Data + 4 * i);
                pObject = (u32*)(ptrTable->pObject) + (subIndex + i);
                *((u32*)pObject) = Datatemp1;
            }
        }
    }

    return(eCANERR_SUCCESSFUL);
}

/*=================================================================================================
** @Name      : u8 CANReqIdleTxRcvTab(e_CANFunCode funcode, u8 attrib, void *pdata, u8 dataType, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conflag, u8 daddr, u8 cannum)
** @Input     : funcode:功能码 attrib:帧属性 pdata:数据首地址 dataType:数据类型 mIndex:主索引  sindex:子索引 sindexnum:子索引数目 conflag:确认标志[修改/查询]或映射起始主索引[单体/参数]  daddr:目的地址, cannum:CAN通道
** @Output    : 申请结果(0:成功 其它:错误码)
** @Function  : 请求空闲的收发表记录发送信息
** @The notes : 设置定时发送
**===============================================================================================*/
u8 CANReqIdleTxRcvTab(e_CANFunCode funcode, u8 attrib, void *pdata, u8 dataType, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 conflag, u8 daddr, u8 cannum)
{
    u8 i = 0;
    u8 errcode = eCANERR_NOIDLE_TAB;

    /*禁止中断*/
    BSP_DISABLE_IRQ();

    /*查找空闲发送区*/
    for(i = 0; i < gCANBaseDef_3[eCANRBas3_TRTabNum]; i++)
    {
        /*空闲监听状态*/
        if(gCanTxRcvTable[i].ready == eCANTAB_LEISUREL)
        {
            gCanTxRcvTable[i].funcode = funcode;
            gCanTxRcvTable[i].attrib = attrib;
            gCanTxRcvTable[i].pdata = pdata;
            gCanTxRcvTable[i].dataType = dataType;

            /*单体信息功能码和参数信息功能码命令字当做映射起始主索引使用*/
            if((eCANFUN_UPC == funcode) || (eCANFUN_UPP == funcode))
            {
            	gCanTxRcvTable[i].mIndex = conflag;
                gCanTxRcvTable[i].cmd.Bits.CON = 0;
            }
            /*只有请求修改和请求查询有确认标准*/
            else
            {
            	gCanTxRcvTable[i].mIndex = mIndex;
                gCanTxRcvTable[i].cmd.Bits.CON = conflag & 0x01;
            }

            gCanTxRcvTable[i].beginIndex = sIndex;
            gCanTxRcvTable[i].sIndex = sIndex;
            gCanTxRcvTable[i].nextsIndex = sIndex;
            gCanTxRcvTable[i].sIndexNum = sIndexNum;
            gCanTxRcvTable[i].daddr = daddr;
            gCanTxRcvTable[i].cannum = cannum;
            gCanTxRcvTable[i].ready = eCANTAB_WAIT;

            //发送信息数据包
            errcode = CANSendTxRcvTabMsgPack(i);
            break;
        }
    }

    /*使能中断*/
    BSP_ENABLE_IRQ();

    return(errcode);
}

/*=================================================================================================
** @Name      : u8 CANSendTxRcvTabMsgPack(u8 id)
** @Input     : id:CAN收发通信表序号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 发送CAN收发通信表中需要发送的数据包
** @The notes : CAN收发通信表有报文需要发送时调用
**===============================================================================================*/
u8 CANSendTxRcvTabMsgPack(u8 id)
{
    u8 i = 0;
    u32 ID = 0;
    u16 msgLen = 0;
    u8 data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u8 errcode = eCANERR_SUCCESSFUL;

    /*通信收发表序号越界*/
    if(id >= gCANBaseDef_3[eCANRBas3_TRTabNum])
    {
        return(eCANERR_NOSUCH_TAB);
    }

    /*发送单帧信息(周期单帧/校时/心跳)*/
    if((gCanTxRcvTable[id].funcode == eCANFUN_UPS) || (gCanTxRcvTable[id].funcode <= eCANFUN_BRE))
    {
    	/*计算需要发送的字节数*/
        msgLen = (u16)gCanTxRcvTable[id].sIndexNum * gCanTxRcvTable[id].dataType;

    	/*需要发送的字节数不能超过1帧8个字节*/
    	if(msgLen <= 8)
        {
            /*生成信息ID*/
            ID = CANBuildMsgID_API(gCanTxRcvTable[id].funcode, gCanTxRcvTable[id].mIndex, gCanTxRcvTable[id].daddr, gCanTxRcvTable[id].attrib);

    	    /*读通信收发表中子索引数据*/
            CANReadTxRcvTabData(id, gCanTxRcvTable[id].sIndexNum, data);

            /*发送报文*/
            CANSendMsgUser_Hook(ID, data, (u8)msgLen, gCanTxRcvTable[id].cannum);
        }
    	/*需要发送的字节数超过1帧8个字节*/
    	else
    	{
    		/*子索引越界*/
    	    errcode = eCANERR_SIDLEN_OVE;
    	}

        /*单帧则发送完成直接清零收发通信表*/
        CANClrTxRcvTabInfo(id);
    }
    //发送单体信息和参数信息*/
    else if((gCanTxRcvTable[id].funcode == eCANFUN_UPC) || (gCanTxRcvTable[id].funcode == eCANFUN_UPP))
    {
    	/*生成信息ID(使用映射主索引作为主索引)*/
        ID = CANBuildMsgID_API(gCanTxRcvTable[id].funcode, gCanTxRcvTable[id].mIndex, gCanTxRcvTable[id].daddr, gCanTxRcvTable[id].attrib);

        /*判断是否为第一帧*/
        if(gCanTxRcvTable[id].beginIndex == gCanTxRcvTable[id].nextsIndex)
        {
            gCanTxRcvTable[id].remframe = CANCalcRemainFrameNum(1, gCanTxRcvTable[id].dataType, gCanTxRcvTable[id].sIndexNum);
        }

        /*获得当前帧的起始子索引*/
        gCanTxRcvTable[id].sIndex = gCanTxRcvTable[id].nextsIndex;

        /*计算当前帧需要读取子索引长度*/
        for(i = 0; i < gCanTxRcvTable[id].sIndexNum; i++)
        {
            /*计一帧发送8个字节数据*/
            if(gCanTxRcvTable[id].dataType > (8 - msgLen))
            {
                break;
            }
            msgLen += gCanTxRcvTable[id].dataType;
        }

        /*计算下一次发送起始子索引和剩余子索引数目，映射主索引递增1*/
        gCanTxRcvTable[id].sIndexNum -=  i;
        gCanTxRcvTable[id].nextsIndex = gCanTxRcvTable[id].sIndex + i;
        gCanTxRcvTable[id].mIndex++;

        /*通信收发表中子索引数据*/
        CANReadTxRcvTabData(id, (u8)(msgLen / gCanTxRcvTable[id].dataType), data);

        /*发送数据*/
        CANSendMsgUser_Hook(ID, data, (u8)msgLen, gCanTxRcvTable[id].cannum);

        /*剩余帧数目-1*/
        if(gCanTxRcvTable[id].remframe > 0)
        {
            gCanTxRcvTable[id].remframe--;
        }

        /*最后一帧则清除收发通信表*/
        if(gCanTxRcvTable[id].remframe == 0)
        {
            CANClrTxRcvTabInfo(id);
        }
    }
    /*发送长帧请求查询信息*/
    else if(gCanTxRcvTable[id].funcode == eCANFUN_REQ)
    {
        /*生成信息ID*/
        ID = CANBuildMsgID_API(gCanTxRcvTable[id].funcode, gCanTxRcvTable[id].mIndex, gCanTxRcvTable[id].daddr, gCanTxRcvTable[id].attrib);

        /*帧命令字清零*/
        gCanTxRcvTable[id].cmd.Bits.FIR = 0;
        gCanTxRcvTable[id].cmd.Bits.FIN = 0;

        data[0] = gCanTxRcvTable[id].cmd.Byte;
        data[1] = gCanTxRcvTable[id].beginIndex;
        data[2] = gCanTxRcvTable[id].sIndexNum;

        /*发送报文(接收回复完成才清除收发通信表)*/
        CANSendMsgUser_Hook(ID, data, 3, gCanTxRcvTable[id].cannum);
    }
    /*发送长帧其他类型信息*/
    else if((gCanTxRcvTable[id].funcode >= eCANFUN_RUP) && (gCanTxRcvTable[id].funcode <= eCANFUN_UPM))
    {
        /*生成信息ID*/
        ID = CANBuildMsgID_API(gCanTxRcvTable[id].funcode, gCanTxRcvTable[id].mIndex, gCanTxRcvTable[id].daddr, gCanTxRcvTable[id].attrib);

        /*帧命令字清零*/
        gCanTxRcvTable[id].cmd.Bits.FIR = 0;
        gCanTxRcvTable[id].cmd.Bits.FIN = 0;

        /*判断是否为第一帧*/
        if(gCanTxRcvTable[id].cmd.Bits.NUM == 0)
        {
        	gCanTxRcvTable[id].cmd.Bits.FIR = 1;
        	gCanTxRcvTable[id].remframe = CANCalcRemainFrameNum(0, gCanTxRcvTable[id].dataType, gCanTxRcvTable[id].sIndexNum);
        }

        /*判断是否为最后一帧*/
        if(gCanTxRcvTable[id].remframe == 1)
        {
        	gCanTxRcvTable[id].cmd.Bits.FIN = 1;
        }

        /*获得当前帧的起始子索引*/
        gCanTxRcvTable[id].sIndex = gCanTxRcvTable[id].nextsIndex;

        /*计算当前帧需要读取子索引长度*/
        for(i = 0; i < gCanTxRcvTable[id].sIndexNum; i++)
        {
            /*计一帧发送6个字节数据*/
            if(gCanTxRcvTable[id].dataType > (6 - msgLen))
            {
                break;
            }
            msgLen += gCanTxRcvTable[id].dataType;
        }

        /*计算下一次发送起始子索引和剩余子索引数目*/
        gCanTxRcvTable[id].sIndexNum -=  i;
        gCanTxRcvTable[id].nextsIndex = gCanTxRcvTable[id].sIndex + i;

        /*命令字*/
        gCanTxRcvTable[id].cmd.Bits.NUM = (gCanTxRcvTable[id].nextsIndex - gCanTxRcvTable[id].sIndex);
        data[0] = gCanTxRcvTable[id].cmd.Byte;

        /*起始子索引*/
        data[1] = gCanTxRcvTable[id].sIndex;

        /*通信收发表中子索引数据*/
        (void)CANReadTxRcvTabData(id, (u8)(msgLen / gCanTxRcvTable[id].dataType), &data[2]);

        /*发送请求修改命令未满8byte时需要增加校验码*/
        if((gCanTxRcvTable[id].funcode == eCANFUN_RUP) && (msgLen < 6))
        {
        	/*保留byte补0*/
        	for(i = msgLen; i < 5; i++)
        	{
        		data[2 + i] = 0;
        	}

        	/*最后byte为校验码*/
        	data[7] = CANComputeRequestCRC8((ID & 0xffff), &data[0], 7);

        	/*帧长度满8byte*/
        	msgLen = 6;
        }

        /*发送数据*/
        CANSendMsgUser_Hook(ID, data, (u8)(msgLen + 2), gCanTxRcvTable[id].cannum);

        /*剩余帧数目-1*/
        if(gCanTxRcvTable[id].remframe > 0)
        {
        	gCanTxRcvTable[id].remframe--;
        }

        /*如果是主动发送信息和查询回复无确认的最后一帧，则清除收发通信表*/
        if((gCanTxRcvTable[id].remframe == 0) && ((gCanTxRcvTable[id].funcode == eCANFUN_UPM) || (((gCanTxRcvTable[id].funcode == eCANFUN_ARQ) && (gCanTxRcvTable[id].cmd.Bits.CON == 0)))))
        {
        	CANClrTxRcvTabInfo(id);
        }
    }
    /*其它不支持的功能码*/
    else
    {
    	/*无此功能码,直接清除该收发通信表*/
        CANClrTxRcvTabInfo(id);

    	errcode = eCANERR_NOSUCH_FUN;
    }

    return(errcode);
}

/*=================================================================================================
** @Name      : void CANClrTxRcvTabInfo(u8 id)
** @Input     : id:CAN收发通信表序号
** @Output    : void
** @Function  : 清除CAN收发通信表中的记录
** @The notes :
**===============================================================================================*/
void CANClrTxRcvTabInfo(u8 id)
{
    /*通信收发表序号越界*/
    if(id >= gCANBaseDef_3[eCANRBas3_TRTabNum])
    {
        return;
    }

    gCanTxRcvTable[id].ready = eCANTAB_LEISUREL;

	gCanTxRcvTable[id].pdata = NULL;

	gCanTxRcvTable[id].funcode = eCANFUN_VFT;
	gCanTxRcvTable[id].beginIndex = 0;
	gCanTxRcvTable[id].sIndex = 0;
	gCanTxRcvTable[id].nextsIndex = 0;
	gCanTxRcvTable[id].sIndexNum = 0;
	gCanTxRcvTable[id].daddr = 0;
	gCanTxRcvTable[id].cmd.Byte = 0;
	gCanTxRcvTable[id].mIndex = 0;
	gCanTxRcvTable[id].dataType = 0;
	gCanTxRcvTable[id].cannum = 0;
	gCanTxRcvTable[id].timer.timeResend = 0;
	gCanTxRcvTable[id].timer.timeElasped = 0;
}

/*=================================================================================================
** @Name      : void CANClrTxRcvTabTimer(u8 id)
** @Input     : id:CAN收发通信表序号
** @Output    : void
** @Function  : 清除CAN收发通信表中的计时
** @The notes : 两帧交互正常计时结束
**===============================================================================================*/
void CANClrTxRcvTabTimer(u8 id)
{
    /*通信收发表序号越界*/
    if(id >= gCANBaseDef_3[eCANRBas3_TRTabNum])
    {
        return;
    }

	gCanTxRcvTable[id].timer.timeResend = 0;
	gCanTxRcvTable[id].timer.timeElasped = 0;
}

/*=================================================================================================
** @Name      : void CANMulFramTxTimerCheck(u8 id, u16 timer)
** @Input     : id:通信表序号, timer:发送帧间隔时间(1ms)
** @Output    : void
** @Function  : 多帧发送帧间间隔计时检测(长帧/单帧/单体/参数)
** @The notes : 通讯表状态为等待发送长帧时1ms定时调用
**===============================================================================================*/
void CANMulFramTxTimerCheck(u8 id, u16 timer)
{
    /*通信收发表序号越界*/
    if(id >= gCANBaseDef_3[eCANRBas3_TRTabNum])
    {
        return;
    }

    /*发送间隔时间到则发送剩余帧*/
    if(gCanTxRcvTable[id].timer.timeElasped >= timer)
    {
        /*如果是最后一帧(发送完成)，则清除收发通信表*/
        if(gCanTxRcvTable[id].remframe == 0)
        {
        	CANClrTxRcvTabInfo(id);
        }
        /*还有长帧未发送，则继续发送*/
        else
        {
        	CANSendTxRcvTabMsgPack(id);
        }

        /*间隔时间清零,重新等待下一帧发送*/
    	gCanTxRcvTable[id].timer.timeElasped = 0;

    }
    /*未达到间隔时间则继续计时*/
    else
    {
        /*计时*/
    	gCanTxRcvTable[id].timer.timeElasped++;
    }
}

/*=================================================================================================
** @Name      : void CANWaitAnsFramTimerCheck(u8 id, u16 timer, u8 times)
** @Input     : id:通信表序号, timer:发送帧间隔时间(1ms) times:重发次数(1次)
** @Output    : void
** @Function  : 等待回复帧间间隔计时检测
** @The notes : 通讯表状态为等待接收回复时1ms定时调用
**===============================================================================================*/
void CANWaitAnsFramTimerCheck(u8 id, u16 timer, u8 times)
{
    /*通信收发表序号越界*/
    if(id >= gCANBaseDef_3[eCANRBas3_TRTabNum])
    {
        return;
    }

    gCanTxRcvTable[id].timer.timeElasped++;

    //重发时间到
    if(gCanTxRcvTable[id].timer.timeElasped >= timer)
    {
        //超时重发前，通讯表状态返回上一帧
    	gCanTxRcvTable[id].sIndexNum += gCanTxRcvTable[id].cmd.Bits.NUM;

        //判断重发帧是否为首帧
        if(gCanTxRcvTable[id].cmd.Bits.FIR == 1)
        {
        	gCanTxRcvTable[id].cmd.Bits.NUM = 0;
        }

        //剩余帧数目加1
        gCanTxRcvTable[id].remframe++;
        gCanTxRcvTable[id].nextsIndex = gCanTxRcvTable[id].sIndex;

        //发送数据
        CANSendTxRcvTabMsgPack(id);

        //时间清零
        gCanTxRcvTable[id].timer.timeElasped = 0;

        //重发次数加1
        gCanTxRcvTable[id].timer.timeResend++;

        //重发次数超时
        if(gCanTxRcvTable[id].timer.timeResend >= times)
        {
            //清除收发通信表
            CANClrTxRcvTabInfo(id);
        }
    }
}

/*=================================================================================================
** @Name      : void CANReadTxRcvTabData(u8 id, u8 readlen, u8 *pdata)
** @Input     : id:CAN收发通信表序号 readlen:读数据长度 *pdata:读到的数据
** @Output    : void
** @Function  : 读取CAN收发通信表中缓存区的数据
** @The notes : 确保读取长度必须有效
**===============================================================================================*/
void CANReadTxRcvTabData(u8 id, u8 readlen, u8 *pdata)
{
    u8 i = 0;

    /*通信收发表序号越界*/
    if(id >= gCANBaseDef_3[eCANRBas3_TRTabNum])
    {
        return;
    }

    //读通讯缓存中数据
    if(gCanTxRcvTable[id].pdata != NULL)
    {
        switch(gCanTxRcvTable[id].dataType)
        {
            case 1:
                for(i = 0; i < readlen; i++)
                {
                    *(pdata + i) = *((u8*)gCanTxRcvTable[id].pdata + gCanTxRcvTable[id].sIndex + i);
                }
                break;

            case 2:
                for(i = 0; i < readlen; i++)
                {
                    *(pdata + 2 * i) = (u8)(*((u16*)gCanTxRcvTable[id].pdata + gCanTxRcvTable[id].sIndex + i) & 0x00FF);
                    *(pdata + 2 * i + 1) = (u8)((*((u16*)gCanTxRcvTable[id].pdata + gCanTxRcvTable[id].sIndex + i) & 0xFF00) >> 8);
                }
                break;

            case 4:
                for(i = 0; i < readlen; i++)
                {
                    *(pdata + 4 * i) = (u8)(*((u32*)gCanTxRcvTable[id].pdata + gCanTxRcvTable[id].sIndex + i) & 0x000000FF);
                    *(pdata + 4 * i + 1) = (u8)((*((u32*)gCanTxRcvTable[id].pdata + gCanTxRcvTable[id].sIndex + i) & 0x0000FF00) >> 8);
                    *(pdata + 4 * i + 2) = (u8)((*((u32*)gCanTxRcvTable[id].pdata + gCanTxRcvTable[id].sIndex + i) & 0x00FF0000) >> 16);
                    *(pdata + 4 * i + 3) = (u8)((*((u32*)gCanTxRcvTable[id].pdata + gCanTxRcvTable[id].sIndex + i) & 0xFF000000) >> 24);
                }
                break;

            default:
                break;
        }
    }
}

/*=================================================================================================
** @Name      : u8 CANFindTxRcvTabMatching(u8 destAddr, e_CANFunCode funcode, u8 mindex, u8 sindex)
** @Input     : destAddr:源地址 funcode:功能码 mindex:主索引 sindex:子索引
** @Output    : void
** @Function  : 查找CAN收发通信表中与输入参数匹配的序号
** @The notes : 查找是否正处于收发状态
**===============================================================================================*/
u8 CANFindTxRcvTabMatching(u8 destAddr, e_CANFunCode funcode, u8 mindex, u8 sindex)
{
    u8 i = 0;

    /*轮询所有CAN收发通信表*/
    for(i = 0; i < gCANBaseDef_3[eCANRBas3_TRTabNum]; i++)
    {
        /*要查找的参数与该CAN收发通信表匹配*/
        if((gCanTxRcvTable[i].daddr == destAddr) && (gCanTxRcvTable[i].funcode == funcode) && (gCanTxRcvTable[i].mIndex == mindex)
            && ((gCanTxRcvTable[i].sIndex == sindex) || (gCanTxRcvTable[i].nextsIndex == sindex)))
        {
            /*返回该CAN收发通信表序号*/
            return(i);
        }
    }

    return(0xff);
}

/*=================================================================================================
** @Name      : u8 CANComputeRequestCRC8(u16 ApplicationId, const u8* Crc_DataPtr, u16 Crc_Length)
** @Input     : ApplicationId:帧ID低16位 Crc_DataPtr:需要计算校验码的数组 Crc_Length:数组长度
** @Output    : void
** @Function  : 按SAE J1850 方式计算CRC值
** @The notes : 请求修改命令帧长度不满8byte但发完8byte时使用
**===============================================================================================*/
u8 CANComputeRequestCRC8(u16 ApplicationId, const u8* Crc_DataPtr, u16 Crc_Length)
{
	u16 count = 0; 		/* loop counter for CRC calculation */
	u8 crc_value = 0; 	/* CRC initial value */
	u8 XOR_out = 0; 	/* Value to XOR output */

	crc_value = 0xffu;
	XOR_out = 0xffu;
	crc_value = Crc8Table[crc_value ^ (u8)(ApplicationId & 0x00ffu)]; 	/* Calculate CRC of low byte of ApplicationId */
	crc_value = Crc8Table[crc_value ^ (u8)(ApplicationId >> 8)]; 		/* Calculate CRC of high byte of ApplicationId */

	for (count = 0; count < (u16)Crc_Length; count++) 					/* Calculate CRC of data block */
	{
		crc_value = Crc8Table[crc_value ^ Crc_DataPtr[count]];
	}
	crc_value = crc_value ^ XOR_out;

	/* Calculate the official checksum.*/
	return(crc_value);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u8 CANFindDevTabMIndexDim(u8 findIndex, u8 mindexNum, const t_CANIndexTab *pindextable)
** @Input     : findIndex:查找的主索引号  mindexNum:主索引数目 *pindextable:所要查找的主索引表
** @Output    : 对应主索引所在位置
** @Function  : 查找主索引所在设备表中偏移位置(第几个主索引)
** @The notes : 二分查找法
**===============================================================================================*/
static u8 CANFindDevTabMIndexDim(u8 findIndex, u8 mindexNum, const t_CANIndexTab *pindextable)
{
    u8 low = 0;
    u8 high = mindexNum - 1;
    u8 mid = 0;
    u8 i = 0;

    for(i = 0; i < ((mindexNum / 2) + 1); i++)
    {
        if(low <= high)
        {
            mid = (u8)((low + high) / 2);
            if((pindextable + mid)->mIndex == findIndex)
            {
                return(mid);
            }
            else
            {
                if((pindextable + mid)->mIndex < findIndex)
                {
                    low = mid + 1;
                }
                else
                {
                    if((pindextable + mid)->mIndex > findIndex)
                    {
                        if(mid == 0)
                        {
                            return(0xff);
                        }

                        high = mid - 1;
                    }
                }
            }
        }
        else
        {
            return(0xff);
        }
    }

    return(0xff);
}

/*=================================================================================================
** @Name      : static u8 CANCalcRemainFrameNum(u8 framSort, u8 dataType, u8 sIndexNum)
** @Input     : framSort:帧类型[0长帧 1单帧] dataType:主索引数据类型 sIndexNum:子索引数目
** @Output    : 剩余帧数
** @Function  : 计算剩余帧数
** @The notes : 根据数据类型和子索引数目计算
**===============================================================================================*/
static u8 CANCalcRemainFrameNum(u8 framSort, u8 dataType, u8 sIndexNum)
{
    u8 remFrame = 0;

    switch(dataType)
    {
        /*U8/S8类型*/
        case 1:
        	/*长帧每帧发6个数据*/
            if(0 == framSort)
            {
            	/*不够整数帧必须多1帧*/
            	remFrame = (sIndexNum + 5) / 6;
            }
        	/*单帧每帧发8个数据*/
            else
            {
            	/*不够整数帧必须多1帧*/
                remFrame = (sIndexNum + 7) / 8;
            }
            break;

        /*U16/S16类型*/
        case 2:
        	/*长帧每帧发3个数据*/
            if(0 == framSort)
            {
            	/*不够整数帧必须多1帧*/
            	remFrame = (sIndexNum + 2) / 3;
            }
        	/*单帧每帧发4个数据*/
            else
            {
            	/*不够整数帧必须多1帧*/
                remFrame = (sIndexNum + 3) / 4;
            }
            break;

        /*U32/S32类型*/
        case 4:
        	/*长帧每帧发1个数据*/
            if(0 == framSort)
            {
            	/*一帧一个数据*/
            	remFrame = sIndexNum;
            }
        	/*单帧每帧发2个数据*/
            else
            {
            	/*不够整数帧必须多1帧*/
                remFrame = (sIndexNum + 1) / 2;
            }
            break;

        default:
            remFrame = 0;
            break;
    }

    return(remFrame);
}
