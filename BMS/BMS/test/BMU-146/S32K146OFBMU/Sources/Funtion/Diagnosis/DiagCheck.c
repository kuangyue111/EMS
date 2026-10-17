/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : DiagMaker.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池故障诊断模块启动滤波接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicPara.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "PortCheck.h"
#include "TacUser.h"
#include "InsuCheck.h"
#include "ChgTactics.h"
#include "CellAFESamp.h"
#include "TacInterface.h"
#include "CalcInterface.h"
#include "DiagInterface.h"
#include "DiagFilter.h"
#include "DiagUser.h"
#include "DiagMaker.h"
#include "DiagCheck.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
static u8 sRcvExtCount = 0;                                 /*接收外部设备(PCS)信息计数*/
static u8 sRcvPerImluCount = 0;                             /*接收外围辅助设备(IMLU)信息计数*/
static u8 sRcvPerImeuCount = 0;                             /*接收外围辅助设备(IMEU)信息计数*/
static u8 sRcvIntCount[DIAG_SLAVE_NUM] = {0};               /*接收内部从控信息计数*/

static u8 sHigVoltExpFlag = 0;                              /*高压采样值异常标志 bit0:总电压异常 bit1:输出端电压异常 [0:正常 1:异常]*/
static u8 sHVSubDiffFlag = 0;                               /*高压采样内外压差过大标志 [0:正常 1:压差过大]*/
static u8 sSwhOnVExpFlag = 0;                               /*电压判断接触器闭合异常标志 按位表示 Bit0:主接触器 bit1:预充接触器  bit2:负极接触器 [0:正常 1:异常]*/
static u8 sSwhOffVExpFlag = 0;                              /*电压判断接触器断开异常标志 按位表示 Bit0:主接触器 bit1:预充接触器  bit2:负极接触器 [0:正常 1:异常]*/
static u8 sSwhOnCExpFlag = 0;                               /*接触器带载闭合标志(按位表示各接触器)*/
static u8 sSwhOffCExpFlag = 0;                              /*接触器带载断开标志(按位表示各接触器)*/

static u8 sPeripDevExpFlag = 0;                             /*电压判断外围器件异常标志 按位表示 Bit0:熔断器 bit1:断路器 [0:正常 1:异常]*/
static u8 sDcSideLeakFlag = 0;                              /*DC侧绝缘故障标志(0:正常 1:一级(严重)故障 2:二级(一般)故障 3:采样失效)*/

static u32 sRcvSlaCellVBit[MAX_BSU_NUM] = {0};              /*接收各个从控单体电压的电池(按位表示:0未接收 1:已接收)*/
static u16 sRcvSlaCellTBit[MAX_BSU_NUM] = {0};              /*接收各个从控单体温度的电池(按位表示:0未接收 1:已接收)*/

static u16 sRcvAllCellVNum = 0;                             /*接收全部单体电压节数计数[0,400]*/
static u16 sRcvAllCellTNum = 0;                             /*接收全部单体温度节数计数[0,400]*/
static u8 sRcvAllSlaveFlag = 0;                             /*接收全部从控信息标志(0未接收 1正在接收 2接收完成)*/
static u8 sRcvAllCellFlag = 0;                              /*接收全部单体信息标志(0未接收 1正在接收 2接收完成)*/
static u8 sRcvAllPeripFlag = 0;                             /*接收到所有外围设备信息标志(0未接收 1正在接收 2接收完成)*/
static u8 sGroupEcuRepFlag = 0;                             /*电池组主控设备地址冲突标志(0正常 1重复)*/

static u8 sPerOffCommuBit = 0;                              /*内部外设子系统通信故障位(按位表示1类子设备:Bit0:IMLU Bit1:IMEU2)*/
static u8 sPerGasErrBitFlag = 0;                            /*内部外设气体浓度异常标志(bit0:干接点 bit1:H2气体 bit1:CO气体 0:正常 1:异常)*/

static u8 sAFESErrSlaveId = 0;                              /*模拟前端采样异常从控序号[1-32]*/
static u8 sAFESErrFlag = 0;                                 /*模拟前端采样异常从控内部故障(0:正常 1:异常)*/
static u8 sAFEVOffSlaveId = 0;                              /*模拟前端检测单体电压采样断线从控序号[1-32]*/
static u32 sAFEVOffCellBit = 0;                             /*模拟前端检测单体电压采样断线从控内部故障位(按位表示1个电压通道)*/
static u8 sCellVOffSlaveId = 0;                             /*采样值检测单体电压采样断线从控序号[1-32]*/
static u32 sCellVOffCellBit = 0;                            /*采样值检测单体电压采样断线从控内部故障位(按位表示1个电压通道)*/
static u8 sCellTOffSlaveId = 0;                             /*采样值检测单体温度采样断线从控序号[1-32]*/
static u16 sCellTOffCellBit = 0;                            /*采样值检测单体温度采样断线从控内部故障位(按位表示1个温度通道)*/

static u8 sBalTOffSlaveId = 0;                              /*均衡板温断线从控序号[1-32]*/
static u16 sBalTOffCellBit = 0;                             /*均衡板温断线从控内部故障位(按位表示1个温度点)*/
static u8 sBalErrSlaveId = 0;                               /*均衡控制故障从控序号[1-32]*/
static u32 sBalErrCellBit = 0;                              /*均衡控制故障从控内部故障位(按位表示1个均衡通道)*/

extern u16 gGCtrlResult_81[];                               /*控制结果状态信息数组*/
extern u16 gSysGenParaRO_100[];                             /*系统通用设置*/
extern u16 gGBmuGenPara_102[];                              /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];                           /*主控高级参数值*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void DiagRcvDevMsgAddAPI(u8 addr)
** @Input     : addr:设备地址
** @Output    : void
** @Function  : 接收外设通信信息报文计数
** @The notes : 接收到报文时调用
**===============================================================================================*/
void DiagRcvDevMsgAddAPI(u8 addr)
{
    /*内部从机(BSU,地址不含组间偏移量)*/
    if((addr < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]) && (addr < DIAG_SLAVE_NUM) && (sRcvIntCount[addr] < 254)
        && (addr < (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum])))
    {
    	sRcvIntCount[addr]++;
    }
    /*内部外设(IMLU1风扇控制设备[IMLU1只有一个地址])*/
    //else if((addr == (IMLU_BAS_DEVADDR + GetGroupOffsetDevAddrAPI())) && (sRcvPerImluCount < 254))
    else if((addr == IMLU_BAS_DEVADDR) && (sRcvPerImluCount < 254))
    {
    	sRcvPerImluCount++;
    }
    /*内部外设(IMEU2空调控制设备[要求IMEU2必须与BMU一一对应])*/
    else if((addr == (IMEU_BAS_DEVADDR + GetGroupOffsetDevAddrAPI())) && (sRcvPerImeuCount < 254))
    {
    	sRcvPerImeuCount++;
    }
    /*外部通信设备(BSMU或PCS)*/
    else if((gSysGenParaRO_100[eSGenPara100_ExtAddr] == addr)
    	&& (sRcvExtCount < 254))
    {
    	sRcvExtCount++;
    }
}

/*=================================================================================================
** @Name      : void DiagRcvEcuRepCheckAPI(u8 addr)
** @Input     : addr:设备地址
** @Output    : void
** @Function  : 判断主控设备地址是否重复冲突
** @The notes : 出现相同的设备地址
**===============================================================================================*/
void DiagRcvEcuRepCheckAPI(u8 addr)
{
	static u8 sRcvNum = 0;
	static u32 sRcvTime = 0;

	/*出现相同的设备地址*/
    if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == addr)
    {
    	sRcvNum++;
    	if(sRcvNum >= 3)
    	{
    		sGroupEcuRepFlag = 1;
    	}
		sRcvTime = GetSysSecondTimeAPI();
    }
    else
    {
    	/*30s未接收到相同设备地址*/
    	if(ABS(GetSysSecondTimeAPI(), sRcvTime) >= 30)
    	{
    		sRcvNum = 0;
    		sGroupEcuRepFlag = 0;
    		sRcvTime = GetSysSecondTimeAPI();
    	}
    }
}

/*=================================================================================================
** @Name      : void DiagRcvCellMissCheckAPI(u8 type, u8 ecu, u8 cell)
** @Input     : type:单体类型[0:单体电压 1:单体温度] ecu:从控号[0,31] cell:单体信息编号[0,17]
** @Output    : void
** @Function  : 判断是否接收所有单体信息
** @The notes :
**===============================================================================================*/
void DiagRcvCellMissCheckAPI(u8 type, u8 ecu, u8 cell)
{
	u8 i = 0;

	if((type >= 2) || (ecu >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]))
	{
		return;
	}

	/*首次*/
    if(0 == sRcvAllCellFlag)
    {
    	for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; i++)
    	{
    		sRcvSlaCellVBit[i] = 0;
    		sRcvSlaCellTBit[i] = 0;
    	}
    	sRcvAllCellVNum = 0;
    	sRcvAllCellTNum = 0;
    	sRcvAllCellFlag = 1;
    }

    /*单体电压*/
    if(0 == type)
    {
    	if(cell >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum])
    	{
    		return;
    	}

    	/*新接收到该电芯电压*/
    	if(0 == BitGet(sRcvSlaCellVBit[ecu], cell))
    	{
    		/*增加一节*/
    		sRcvAllCellVNum++;
    		BitSet(sRcvSlaCellVBit[ecu], cell);
    	}
    }
	/*单体温度*/
    else
    {
    	if(cell >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum])
    	{
    		return;
    	}

    	/*新接收到该电芯温度*/
    	if(0 == BitGet(sRcvSlaCellTBit[ecu], cell))
    	{
    		/*增加一节*/
    		sRcvAllCellTNum++;
    		BitSet(sRcvSlaCellTBit[ecu], cell);
    	}
    }

	/*接收到最大节数*/
	if((sRcvAllCellVNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
		&& (sRcvAllCellTNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum]))
	{
		sRcvAllCellFlag = 2;
	}
}

/*=================================================================================================
** @Name      : void DiagCommuCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 检测通信状态任务
** @The notes : 1s周期调用
**===============================================================================================*/
void DiagCommuCheckTask(void)
{
    u8 i = 0;
    static u8 sExtTime = 0;
    static u8 sPerImluTime = 0;
    static u8 sPerImeuTime = 0;

    /*CAN通信单体采样方式(才判断从控CAN通信状态)*/
    #if(1 == BMU_BSU_COM_TYPE)
    u8 j = 0;
    u8 slaNum = 0;
    u8 rcvSlaNum = 0;
    static u8 sHisRcvSlaAll = 0;
    static u8 sClrSlaOffTime = 0;
    static u8 sSlaTime[DIAG_SLAVE_NUM] = {0};
    #endif

    //上电前3s等待单体信息
    if(((BSPGetOSMillSecTimeAPI() / 1000) <= DIAG_WAIT_CELL_T) && (2 != sRcvAllCellFlag))
    {
        for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; i++)
        {
            //未接收到信息则等待否则开始检测
            if(0 == sRcvIntCount[i])
            {
            	#if(1 == BMU_BSU_COM_TYPE)
            	sSlaTime[i] = 0;
            	#endif
                return;
            }
        }
    }

    //判断主控设备地址是否冲突
    if(1 == sGroupEcuRepFlag)
    {
    	DiagSetErrFilStateAPI(eERR_ID_MECU_REP, eErr_B);                    //确定设备地址冲突稳定状态
    }
    else
    {
    	DiagSetErrFilStateAPI(eERR_ID_MECU_REP, eErr_A);                    //确定设备地址正常稳定状态
    }

    //判断上层BSMU或PCS通讯告警状态
    if(0 == sRcvExtCount)                                                   //未接收到信息
    {
        if(sExtTime < 255)                                                  //防止越界
        {
        	sExtTime++;                                                     //外部通讯异常延时
        }

        //外部告警1级检测(限制充放电)
        if(0 != DiagGetFilErrSortAPI(eERR_ID_EXT_COFF))                     //需要检测外部通讯异常
        {
            if(sExtTime >= (DiagGetFilAtoBTimeAPI(eERR_ID_EXT_COFF) / 10))  //达到检测时间
            {
            	DiagSetErrFilStateAPI(eERR_ID_EXT_COFF, eErr_B);            //确定与PCS通讯断线稳定状态
            }
        }
        else
        {
        	DiagSetErrNowStateAPI( eERR_ID_EXT_COFF, eErr_A);               //设置与PCS通讯正常
        }
    }
    else
    {
    	sExtTime = 0;
    	sRcvExtCount = 0;
        DiagSetErrFilStateAPI(eERR_ID_EXT_COFF, eErr_A);                    //设置与外部通讯正常
    }

    //判断内部外设通讯告警状态
    if(BSPGetOSMillSecTimeAPI() <= 2000)                                    //上电前2s等待通信信息
    {
    	sPerImluTime = 0;                                                   //不检测
    	sPerImeuTime = 0;                                                   //不检测
    }
    else if((sRcvPerImluCount > 0)                                          //IMLU和IMEU均通信正常
    	&& (sRcvPerImeuCount > 0))
	{
    	DiagSetErrFilStateAPI(eERR_ID_PER_COFF, eErr_A);                    //与内部外设通讯正常状态

		sRcvPerImluCount = 0;
		sRcvPerImeuCount = 0;
		//if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_PER_COFF))            //已确认MLU和IMEU均通信正常
		{
		    sPerImluTime = 0;
		    sPerImeuTime = 0;
			sPerOffCommuBit = 0;											//清除MLU和IMEU通信中断标志
			sRcvAllPeripFlag = 2;                                           //接收标志置2
		}
		/*else if(0 == sRcvAllPeripFlag)								    //未记录接收报文
		{
			sRcvAllPeripFlag = 1;                                           //接收标志置1
		}*/
	}
    else																	//IMLU或IMEU通信中断
    {
    	//检测IMLU1通信状态
    	if(0 == sRcvPerImluCount)                                           //未接收到IMLU信息
		{
			if(0 != DiagGetFilErrSortAPI(eERR_ID_PER_COFF))                 //检测内部外设通讯故障
			{
				if(sPerImluTime < 255)                                      //防止计时越界
				{
					sPerImluTime++;                                         //外部通讯异常延时
				}

				if(sPerImluTime > 35)                                       //35s未收到信息
				{
					if(1 == BitGet(sPerOffCommuBit, 1))     				//IMEU已通信中断
					{
						sRcvAllPeripFlag = 0;                               //接收标志清零
					}
					else if(2 == sRcvAllPeripFlag)							//另外设备有信息
					{
						sRcvAllPeripFlag = 1;                               //接收标志正在接收
					}
				}

				//if(sPerImluTime >= (DiagGetFilAtoBTimeAPI(eERR_ID_PER_COFF) / 10))//达到检测时间
				if(sPerImluTime >= 70)										//达到检测时间
				{
					//刚产生通信中断则清除接收数据和记录事件
					if(0 == BitGet(sPerOffCommuBit, 0))
					{
						BitSet(sPerOffCommuBit, 0);
						DiagUserSelfErrRecord(eUDef_ErrRcd_LuComErr, eERR_RCDSORT_WARSET, 0,  0);
						ClrGPerImlu1AllInfoAPI();
					}
				}
			}
			else                                                            //不检测内部外设通讯故障
			{
				sPerImluTime = 0;
				sRcvAllPeripFlag = 2;                                       //接收标志置2
				BitClr(sPerOffCommuBit, 0);									//当做通信正常
			}
		}
		else                                                                /*接收到IMLU信息报文*/
		{
			sPerImluTime = 0;
			sRcvPerImluCount = 0;
			BitClr(sPerOffCommuBit, 0);										//IMLU通信正常

			if(0 == sRcvAllPeripFlag)										//未记录接收报文
			{
				sRcvAllPeripFlag = 1;                                       //接收标志置1
			}
			else if((0 == BitGet(sPerOffCommuBit, 1))     					//IMEU通信正常
				&& (0 == sPerImeuTime))
			{
				sRcvAllPeripFlag = 2;                                       //接收标志置2
			}
		}

    	//检测IMEU2通信状态
    	if(0 == sRcvPerImeuCount)                                           //未接收到IMEU信息
		{
			if(0 != DiagGetFilErrSortAPI(eERR_ID_PER_COFF))                 //检测内部外设通讯故障
			{
				if(sPerImeuTime < 255)                                      //防止计时越界
				{
					sPerImeuTime++;                                         //外部通讯异常延时
				}

				if(sPerImeuTime > 5)                                        //5s未收到信息
				{
					if(1 == BitGet(sPerOffCommuBit, 0))     				//IMEU已通信中断
					{
						sRcvAllPeripFlag = 0;                               //接收标志清零
					}
					else if(2 == sRcvAllPeripFlag)							//另外设备有信息
					{
						sRcvAllPeripFlag = 1;                               //接收标志正在接收
					}
				}

				if(sPerImeuTime >= (DiagGetFilAtoBTimeAPI(eERR_ID_PER_COFF) / 10))//达到检测时间
				{
					//刚产生通信中断则清除接收数据和记录事件
					if(0 == BitGet(sPerOffCommuBit, 1))
					{
						BitSet(sPerOffCommuBit, 1);
						DiagUserSelfErrRecord(eUDef_ErrRcd_EuComErr, eERR_RCDSORT_WARSET, 0,  0);
						ClrGPerImeu2AllInfoAPI();
					}
				}
			}
			else                                                            //不检测内部外设通讯故障
			{
				sPerImeuTime = 0;
				sRcvAllPeripFlag = 2;                                       //接收标志置2
				BitClr(sPerOffCommuBit, 1);									//当做通信正常
			}
		}
		else                                                                /*接收到IMEU信息报文*/
		{
			sPerImeuTime = 0;
			sRcvPerImeuCount = 0;
			BitClr(sPerOffCommuBit, 1);										//IMEU通信正常

			if(0 == sRcvAllPeripFlag)										//未记录接收报文
			{
				sRcvAllPeripFlag = 1;                                       //接收标志置1
			}
			else if((0 == BitGet(sPerOffCommuBit, 0))     					//IMLU通信正常
				&& (0 == sPerImluTime))
			{
				sRcvAllPeripFlag = 2;                                       //接收标志置2
			}
		}

    	//有外设通信异常
    	if(sPerOffCommuBit > 0)
    	{
			DiagSetErrFilStateAPI(eERR_ID_PER_COFF, eErr_B);            	//与内部外设通讯断线确定状态
    	}
	}

    /*CAN通信单体采样方式(才判断从控CAN通信状态)*/
	#if(1 == BMU_BSU_COM_TYPE)
    slaNum = (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] - 1) / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum];
    //判断下层从机通讯状态(非每个Pack的从控,只是CAN通信的从机,每个从机管理多个Pack)
    //for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; i++)
    for(i = 0; i < slaNum; i++)
    {
        if(0 == sRcvIntCount[i])                                            //未接收到信息
        {
        	sHisRcvSlaAll = 0;                                              /*未收到全部报文*/

            if(sSlaTime[i] < 255)                                           //防止越界
            {
            	sSlaTime[i]++;                                              //通讯异常计时
            }

            if(0 != DiagGetFilErrSortAPI(eERR_ID_INT_COFF))                 //检测从控通讯故障
            {
            	if(sSlaTime[i] >= 5)                                        //5s未收到信息
                {
            		//新产生
            		if((sRcvAllCellFlag > 0) || (sRcvAllSlaveFlag > 0))
            		{
                	    //由正常转入断线
                	    if((2 == sRcvAllCellFlag) || (2 == sRcvAllSlaveFlag))
                	    {
                	    	//记录事件
                	    	DiagUserSelfErrRecord(eUDef_ErrRcd_SlaveOff, eERR_RCDSORT_WARSET, 0,  0);
                	    }
                	    //接收标志清零
                    	sRcvAllCellFlag = 0;
                	    sRcvAllSlaveFlag = 0;

            		}
                }

            	if(sSlaTime[i] >= (DiagGetFilAtoBTimeAPI(eERR_ID_INT_COFF) / 10))//达到检测时间
            	{
            		DiagSetErrFilStateAPI(eERR_ID_INT_COFF, eErr_B);        //与从控通讯断线确定

					/*无需不停清零*/
            		if(sSlaTime[i] <= ((DiagGetFilAtoBTimeAPI(eERR_ID_INT_COFF) / 10) + 5))
            		{
						/*采样失效清除单体电压温度内阻信息(输出单体电压/内阻值为0/单体温度为-41)*/
            			for(j = 0; j < gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum]; j++)
            			{
							ClrSampCellVoltHook(i * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + j);
							ClrSampCellTempHook(i * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + j);
							ClrSampCellResisHook(i * gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] + j);
            			}
            		}
            	}
            }
            else                                                            //不检测从控通讯故障
            {
            	rcvSlaNum++;                                                //该从控当做接收到报文
            }
        }
        else                                                                //接收到信息
        {
        	rcvSlaNum++;
        	sSlaTime[i] = 0;                                                //清除计时

            if(1 == sHisRcvSlaAll)                                          //上次也是接收全部
            {
            	sRcvIntCount[i] = 0;                                        //清除报文计数
            }
        }
    }

    //if(rcvSlaNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])        //轮询一遍均接收到信息
    if(rcvSlaNum >= slaNum)                                                 //轮询一遍均接收到信息
    {
    	sHisRcvSlaAll = 1;
    	sRcvAllSlaveFlag = 2;                                               //记录接收到所有从控信息
    	DiagSetErrNowStateAPI(eERR_ID_INT_COFF, eErr_A);                    //清除与从控通讯断线故障
    }

    if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_INT_COFF))                   //已上报故障
    {
    	if(sClrSlaOffTime >= 5)                                             //间隔5s
    	{
    	    //for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; i++)
    	    for(i = 0; i < slaNum; i++)
    	    {
            	sRcvIntCount[i] = 0;                                        //清除所有从控报文计数
    	    }

    	    sClrSlaOffTime = 0;
    	}
    	else                                                                //等待间隔时间
    	{
    	    sClrSlaOffTime++;
    	}
    }
    else                                                                    //通信正常
    {
    	sClrSlaOffTime = 0;
    }
    //菊花链通信方式
	#else
    //电压温度均采样完成则接收到所有从控信息
    if(1 == GetCellVSampFinFlagAPI())
    {
    	if(1 == GetCellTSampFinFlagAPI())
		{
    		sRcvAllSlaveFlag = 2;
		}
    	else
    	{
    		sRcvAllSlaveFlag = 1;
    	}
    }
    else if(1 == GetCellTSampFinFlagAPI())
    {
		sRcvAllSlaveFlag = 1;
    }
    else
    {

		sRcvAllSlaveFlag = 0;
    }
	#endif
}

/*=================================================================================================
** @Name      : void DiagPerDevCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 检测外围设备故障状态任务
** @The notes : 500ms周期调用
**===============================================================================================*/
void DiagPerDevCheckTask(void)
{
    u16 sumVolt = 0;
    u16 outVolt = 0;
    static s16 sHisCurr = 0;
    static u8 sCurrSamTime = 0;
    static u8 sPreVErrTime = 0;
    static u16 sGasNorTime = 0;
    static u32 sGasWaitTime = 1800000;//可燃气体检测上电等待时间(默认0.5H)

    sumVolt = GetGSampSumVoltAPI();
    outVolt = GetGSampPreVoltAPI();

    //电流未变化(10A内变化)
    if(ABS(sHisCurr, GetGSampOutCurrAPI())  <= 100)
    {
    	//未运行认为电流持续未变化
    	if(eWORK_RUN != GetGWorkStateAPI())
    	{
    		sCurrSamTime = 250;
    	}
    	//计算电流稳定持续时间
    	else if(sCurrSamTime < 250)
    	{
    		sCurrSamTime++;
    	}
    }
    else
    {
    	sCurrSamTime = 0;
    }
    sHisCurr = GetGSampOutCurrAPI();

    //内外高压压差检测(判断是否内外压差过大不能上高压)
    //非接触器变化状态(预充电压不确定)
    //非快充枪插入请求停机(充电枪输出电压自检)
    //总电压和外端电压均正常(单组运行不受其他组影响)
    if((eWORK_START != GetGWorkStateAPI())
        && (eWORK_STOP != GetGWorkStateAPI())
		&& (0 == GetFstChgStopReqFlagAPI())
    	&& (gSysGenParaRO_100[eSGenPara100_AllGNum] <= 1)
        && (sumVolt >= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV])
        && (outVolt >= gGBmuHigLevPara_103[eBmuHigLevPara103_MSwhOffV]))
    {
        //内外压差超过安全启动压差
        if(ABS(sumVolt, outVolt) > gGBmuHigLevPara_103[eBmuHigLevPara103_SafeSubV])
        {
            //内外电压差异过大异常
            sHVSubDiffFlag = 1;
        }
        //总电压或外端电压采样无电压
        else if(ABS(sumVolt, outVolt) <= (gGBmuHigLevPara_103[eBmuHigLevPara103_SafeSubV] - 15))
        {
            //无内外电压差异过大异常
            sHVSubDiffFlag = 0;
        }
    }
    //总电压或外端电压采样无电压
    else
    {
        //无内外电压差异过大异常
        sHVSubDiffFlag = 0;
    }

    //外端高压采样检测(高压接触器闭合无外端电压报异常)
    //正极(或预充)和负极接触器闭合
    if(((ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))
    	|| (ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre)))
    #if(1 == BMU_NEG_SWH_EN)
        && (ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))
    #endif
    )
    {
        //有总压无外端电压--外端采样异常
        if((sumVolt >= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV])
            && (outVolt <= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV]))
        {
            //非启动状态或外端电压异常超时
        	if((eWORK_START != GetGWorkStateAPI()) || (sPreVErrTime >= 3))
        	{
        		//设置预充电压采样值异常
        		BitSet(sHigVoltExpFlag, 1);
        	}
            //外端电压异常计时
        	else
        	{
        		sPreVErrTime++;
        	}
        }
        else
        {
            //清除外端电压异常计时
            sPreVErrTime = 0;
            //清除外端电压采样值异常
            BitClr(sHigVoltExpFlag, 1);
        }
    }
    //主接触器(和预充)或负极接触器断开
    else
    {
        //清除外端电压异常计时
        sPreVErrTime = 0;
        //清除外端电压采样值异常
        BitClr(sHigVoltExpFlag, 1);
    }

    //绝缘检测
    if(0 == gGBmuHigLevPara_103[eBmuHigLevPara103_LeakMonitType])               //不检测不上报
    {
        //无故障
        sDcSideLeakFlag = 0;
    }
    else if((1 == gGBmuHigLevPara_103[eBmuHigLevPara103_LeakMonitType])         //非运行检测不上报
        || (3 == gGBmuHigLevPara_103[eBmuHigLevPara103_LeakMonitType])          //全程检测不上报
        || (6 == gGBmuHigLevPara_103[eBmuHigLevPara103_LeakMonitType]))         //运行检测不上报
    {
        //外部请求检测或已出现绝缘异常
        if((1 == gGCtrlResult_81[eCtrlRes81_LeakCheck])
            || (3 == gGCtrlResult_81[eCtrlRes81_LeakCheck])
            || (sDcSideLeakFlag > 0))
        {
            if((3 == GetGLeakIoExpStateAPI())                                   //绝缘报警IO异常状态
                || (3 == GetInsuErrStateFlagAPI()))                             //绝缘报警检测异常状态
            {
                sDcSideLeakFlag = 3;
            }
            else if((2 == GetGLeakIoExpStateAPI())                              //绝缘报警IO2级状态
                || (2 == GetInsuErrStateFlagAPI()))                             //绝缘报警检测2级状态
            {
                sDcSideLeakFlag = 2;
            }
            else if((1 == GetGLeakIoExpStateAPI())                              //绝缘报警IO1级状态
                || (1 == GetInsuErrStateFlagAPI()))                             //绝缘报警检测1级状态
            {
                sDcSideLeakFlag = 1;
            }
            else
            {
                sDcSideLeakFlag = 0;
            }
        }
        else
        {
            //无故障
            sDcSideLeakFlag = 0;
        }
    }
    else if((2 == gGBmuHigLevPara_103[eBmuHigLevPara103_LeakMonitType])         //非运行检测非运行上报
        || (4 == gGBmuHigLevPara_103[eBmuHigLevPara103_LeakMonitType]))         //全程检测非运行上报
    {
        //直流接触器均断开,或外部请求检测或外部检测已出现绝缘异常
        if((1 == TacGetGroupRunSwhAllOffStateAPI())                              //运行接触器断开
            || (1 == gGCtrlResult_81[eCtrlRes81_LeakCheck])
        	|| (3 == gGCtrlResult_81[eCtrlRes81_LeakCheck])
            || (sDcSideLeakFlag > 0))
        {
            if((3 == GetGLeakIoExpStateAPI())                                   //绝缘报警IO异常状态
                || (3 == GetInsuErrStateFlagAPI()))                             //绝缘报警检测异常状态
            {
                sDcSideLeakFlag = 3;
            }
            else if((2 == GetGLeakIoExpStateAPI())                              //绝缘报警IO2级状态
                || (2 == GetInsuErrStateFlagAPI()))                             //绝缘报警检测2级状态
            {
                sDcSideLeakFlag = 2;
            }
            else if((1 == GetGLeakIoExpStateAPI())                              //绝缘报警IO1级状态
                || (1 == GetInsuErrStateFlagAPI()))                             //绝缘报警检测1级状态
            {
                sDcSideLeakFlag = 1;
            }
            else
            {
                sDcSideLeakFlag = 0;
            }
        }
        //有直流接触器闭合
        else
        {
            //无故障
            sDcSideLeakFlag = 0;
        }
    }
    //全程检测或全程上报故障
    else
    {
        if((3 == GetGLeakIoExpStateAPI())                                       //绝缘报警IO异常状态
            || (3 == GetInsuErrStateFlagAPI()))                                 //绝缘报警检测异常状态
        {
            sDcSideLeakFlag = 3;
        }
        else if((2 == GetGLeakIoExpStateAPI())                                  //绝缘报警IO2级状态
            || (2 == GetInsuErrStateFlagAPI()))                                 //绝缘报警检测2级状态
        {
            sDcSideLeakFlag = 2;
        }
        else if((1 == GetGLeakIoExpStateAPI())                                  //绝缘报警IO1级状态
            || (1 == GetInsuErrStateFlagAPI()))                                 //绝缘报警检测1级状态
        {
            sDcSideLeakFlag = 1;
        }
        else
        {
            sDcSideLeakFlag = 0;
        }
    }

    //利用外端电压进行接触器闭合故障检测(闭合状态不会误报需要检测)
    //if(1 == BitGet(gGBmuHigLevPara_103[eBmuHigLevPara103_AdhMonitType], 1))
    {
		//正极(或预充)和负极接触器闭合
		if(((ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))
			|| (ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre)))
		#if(1 == BMU_NEG_SWH_EN)
			&& (ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))
		#endif
		)
		{
			//有总压无外电压--主接触器或预充可能异常
			if((sumVolt >= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV])
				&& (outVolt <= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV]))
			{
				//判定外端电压异常再报接触器异常
				if(1 == BitGet(sHigVoltExpFlag, 1))
				{
					//正极接触器闭合
					if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))
					{
						//可能是主正接触器闭合异常
						BitSet(sSwhOnVExpFlag, eGroupSwh_Mai);
					}
					else
					{
						//清除主正接触器闭合异常
						BitClr(sSwhOnVExpFlag, eGroupSwh_Mai);
					}

					//预充接触器闭合
					if(ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre))
					{
						//可能是预充接触器闭合异常
						BitSet(sSwhOnVExpFlag, eGroupSwh_Pre);
					}
					else
					{
						//清除预充接触器闭合异常
						BitClr(sSwhOnVExpFlag, eGroupSwh_Pre);
					}
				}

				//清除负极接触器闭合异常(负极接触器断开也能采外端电压)
				BitClr(sSwhOnVExpFlag, eGroupSwh_Neg);
			}
			//无总压或有外电压时
			else
			{
				//主正接触器闭合且有总压且内外电压采样不一致(压差超过安全压差[小电流])
				if((ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))
					&& (sumVolt >= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV])
					&& ((ABS(sumVolt, outVolt) >= (gGBmuHigLevPara_103[eBmuHigLevPara103_SafeSubV] * 3))
					|| ((ABS(sumVolt, outVolt) >= gGBmuHigLevPara_103[eBmuHigLevPara103_SafeSubV])
					&& (ABS(GetGSampOutCurrAPI(), 0) <= 1000))))
				{
					//设置接触器闭合异常
					BitSet(sSwhOnVExpFlag, eGroupSwh_Mai);
					#if(1 == BMU_NEG_SWH_EN)
					BitSet(sSwhOnVExpFlag, eGroupSwh_Neg);
					#endif
				}
				//无总压或内外电压一致
				else
				{
					//清除接触器闭合异常
					BitClr(sSwhOnVExpFlag, eGroupSwh_Mai);
					BitClr(sSwhOnVExpFlag, eGroupSwh_Neg);
				}

				//清除预充接触器闭合异常
				BitClr(sSwhOnVExpFlag, eGroupSwh_Pre);
			}
		}
		//主正(和预充)接触器或负极接触器断开
		else
		{
			//清除接触器闭合异常
			BitClr(sSwhOnVExpFlag, eGroupSwh_Mai);
			BitClr(sSwhOnVExpFlag, eGroupSwh_Pre);
			BitClr(sSwhOnVExpFlag, eGroupSwh_Neg);
		}
    }

    //达到可燃气体浓度检测时间(气体传感器上电1H后数据才可信)
    if(BSPGetOSMillSecTimeAPI() >= sGasWaitTime)
    {
    	//干接点报警(多个传感器串联成1个干接点)
    	if(1 == GetGGasConIoExpStateAPI())
		{
			BitSet(sPerGasErrBitFlag, 0);
		}
		//无干接点告警
		else
		{
			//H2度告警消除
			BitClr(sPerGasErrBitFlag, 0);
		}

		//H2浓度告警
		if(GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1) >= BMU_EU_GAS1_CONC)
		{
			BitSet(sPerGasErrBitFlag, 1);
		}
		//H2度告警消除
		else if(GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1) <= (BMU_EU_GAS1_CONC / 2))
		{
			BitClr(sPerGasErrBitFlag, 1);
		}

		//C0浓度告警
		if(GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2) >= BMU_EU_GAS2_CONC)
		{
			BitSet(sPerGasErrBitFlag, 2);
		}
		//C0浓度告警消除
		else if(GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2) <= (BMU_EU_GAS2_CONC / 2))
		{
			BitClr(sPerGasErrBitFlag, 2);
		}
    }
    //上电可燃气体浓度异常(气体传感器上电可能前1H数据无效会误报单独处理)
    else
    {
    	//连续1min均为正常数据则认为传感器已就绪不需再等
    	if((0 == GetGGasConIoExpStateAPI())
    		&& (GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1) < BMU_EU_GAS1_CONC)
    		&& (GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2) < BMU_EU_GAS2_CONC))
    	{
    		if(sGasNorTime >= 120)
    		{
    			//记录此时时间无效再等待
    			sGasWaitTime = BSPGetOSMillSecTimeAPI();
    			sGasNorTime = 0;
    		}
    		else
    		{
    			sGasNorTime++;
    		}
    	}
    	//传感器数据异常
    	else
    	{
    		//正常计时清零
    		sGasNorTime = 0;
    	}
    }

    //从控正常才检测总压是否异常和接触器断开是否异常
    if((DiagGetCellVoltExpStateHOOK() > 0)
       || (0 == sRcvAllSlaveFlag)
       || (0 == sRcvAllCellFlag))
    {
        //从控异常只检测是否采集到总电压
        if(sumVolt < gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV])
        {
            //与HVCU通讯有负极接触器
            #if((2 == BMU_DC_VH_TYPE) && (1 == BMU_NEG_SWH_EN))
            //GND接内侧
            if(1 == GetGHigPGNDSwhStateAPI())
            {
                //设置总电压采样值异常
                BitSet(sHigVoltExpFlag, 0);
            }
            //GND接外侧(可能是负极接触器异常)
            else
            {
                //清除总电压采样值异常
                BitClr(sHigVoltExpFlag, 0);
            }
            #else
            //设置总电压采样值异常
            BitSet(sHigVoltExpFlag, 0);
            #endif
        }
        else
        {
            //清除总电压采样值异常
            BitClr(sHigVoltExpFlag, 0);
        }

        return;
    }

    //未采集到总电压或与累加总电压不一致(前提是必须有累加总电压且电流持续稳定[排除大电流引起总压较大压降单体更新不及时])
    if((GetGCellSumVoltAPI() >= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV]) && (sCurrSamTime >= 20)
        && ((ABS(sumVolt, GetGCellSumVoltAPI()) >= gGBmuHigLevPara_103[eBmuHigLevPara103_SumDiffV])
        || (sumVolt < gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV])))
    {
        //与HVCU通讯有负极接触器
        #if((2 == BMU_DC_VH_TYPE) && (1 == BMU_NEG_SWH_EN))
        //GND接内侧
        if(1 == GetGHigPGNDSwhStateAPI())
        {
            //设置总电压采样值异常
            BitSet(sHigVoltExpFlag, 0);
        }
        //GND接外侧(可能是负极接触器异常)
        else
        {
            //清除总电压采样值异常
            BitClr(sHigVoltExpFlag, 0);
        }
        #else
        //设置总电压采样值异常
        BitSet(sHigVoltExpFlag, 0);
        #endif
    }
    else
    {
        //清除总电压采样值异常
        BitClr(sHigVoltExpFlag, 0);
    }

    //利用外端电压进行接触器断开故障检测(通过外端电压无法判断负极接触器是否断开故障)
    if(1 == BitGet(gGBmuHigLevPara_103[eBmuHigLevPara103_AdhMonitType], 1))
    {
		//正极和预充接触器断开负极闭合-->>内外电压电压一致则主正异常
		if((ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))
			&& (ePSTATE_OFF == TacGetGroupSwhBackStateAPI(eGroupSwh_Pre)))
		{
			//非快充枪插入请求停机(充电枪输出电压自检)
			//非多组电池并机(可能其它组高压已闭合)
			//内外电压正常且外端电压与总电压一样
			if((0 == GetFstChgStopReqFlagAPI())
				&& (gSysGenParaRO_100[eSGenPara100_AllGNum] <= 1)
				&& (GetGCellSumVoltAPI() >= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV])
				&& (outVolt >= gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV])
				&& (ABS(outVolt, GetGCellSumVoltAPI()) <= gGBmuHigLevPara_103[eBmuHigLevPara103_SumDiffV]))
			{
				BitSet(sSwhOffVExpFlag, eGroupSwh_Mai);
			}
			else
			{
				BitClr(sSwhOffVExpFlag, eGroupSwh_Mai);
			}
		}
		//主接触器或预充接触器闭合
		else
		{
			BitClr(sSwhOffVExpFlag, eGroupSwh_Mai);
		}
    }
}

/*=============================================================================================
** @Name      : void DiagSwhChangCStateCheckTask(void)
** @Input     : void
** @Output    : void
** @Function  : 检测接触器动作时电流状态(是否带载动作)
** @The notes : 100ms调用,检测主正/主负接触器
**===========================================================================================*/
void DiagSwhChangCStateCheckTask(void)
{
    static u8 sMaiOnTime = 0;
    static u8 sNegOnTime = 0;
    static u8 sHisSwhState = 0;
    static s16 sHisOutCurr = 0;
    static u16 sTime = 0;

	/*主正接触器闭合*/
	if(ePSTATE_ON == TacGetGroupSwhCtrlStateAPI(eGroupSwh_Mai))
	{
		/*历史状态断开且当前有不安全电流*/
		if((0 == BitGet(sHisSwhState, eGroupSwh_Mai))
			&& (ABS(GetGSampOutCurrAPI(), 0) > gGBmuHigLevPara_103[eBmuHigLevPara103_MainSwhC]))
		{
			/*记录主正接触器带载闭合*/
			BitSet(sSwhOnCExpFlag, eGroupSwh_Mai);
		}

		/*清除主正接触器带载断开告警*/
		BitClr(sSwhOffCExpFlag, eGroupSwh_Mai);

		/*闭合连续检测1s*/
		if(sMaiOnTime >= 10)
		{
			/*记录确认主正接触器闭合历史状态*/
			BitSet(sHisSwhState, eGroupSwh_Mai);
		}
		else
		{
			sMaiOnTime++;
		}
	}
	/*主正接触器断开*/
	else
	{
		/*历史状态闭合且有不安全历史电流*/
		if((1 == BitGet(sHisSwhState, eGroupSwh_Mai))
			&& (ABS(sHisOutCurr, 0) > gGBmuHigLevPara_103[eBmuHigLevPara103_MainSwhC]))
		{
			/*记录主正接触器带载断开*/
			BitSet(sSwhOffCExpFlag, eGroupSwh_Mai);
		}

		/*清除主正接触器带载闭合告警*/
		BitClr(sSwhOnCExpFlag, eGroupSwh_Mai);

		/*记录主正接触器断开历史状态*/
		BitClr(sHisSwhState, eGroupSwh_Mai);
		sMaiOnTime = 0;
	}

    //有负极接触器
    #if(1 == BMU_NEG_SWH_EN)
	/*主负接触器闭合*/
	if(ePSTATE_ON == TacGetGroupSwhCtrlStateAPI(eGroupSwh_Neg))
	{
		/*历史状态断开且当前有不安全电流*/
		if((0 == BitGet(sHisSwhState, eGroupSwh_Neg))
			&& (ABS(GetGSampOutCurrAPI(), 0) > gGBmuHigLevPara_103[eBmuHigLevPara103_MainSwhC]))
		{
			/*记录主正接触器带载闭合*/
			BitSet(sSwhOnCExpFlag, eGroupSwh_Neg);
		}

		/*清除主正接触器带载断开告警*/
		BitClr(sSwhOffCExpFlag, eGroupSwh_Neg);

		/*闭合连续检测1s*/
		if(sNegOnTime >= 10)
		{
			/*记录主正接触器闭合历史状态*/
			BitSet(sHisSwhState, eGroupSwh_Neg);
		}
		else
		{
			sNegOnTime++;
		}
	}
	/*负极接触器断开*/
	else
	{
		/*历史状态闭合且有不安全历史电流*/
		if((1 == BitGet(sHisSwhState, eGroupSwh_Neg))
			&& (ABS(sHisOutCurr, 0) > gGBmuHigLevPara_103[eBmuHigLevPara103_MainSwhC]))
		{
			/*记录主正接触器带载断开*/
			BitSet(sSwhOffCExpFlag, eGroupSwh_Neg);
		}

		/*清除主正接触器带载闭合告警*/
		BitClr(sSwhOnCExpFlag, eGroupSwh_Neg);

		/*记录负极接触器断开历史状态*/
		BitClr(sHisSwhState, eGroupSwh_Neg);
		sNegOnTime = 0;
	}
	#endif

	/*记录历史电流*/
	sHisOutCurr = GetGSampOutCurrAPI();

	/*告警30s*/
	if(((eErr_B == DiagGetErrLastStateAPI(eERR_ID_SWHON_CH))
		|| (eErr_B == DiagGetErrLastStateAPI(eERR_ID_SWHOFF_CH)))
		&& ((sSwhOnCExpFlag > 0) || (sSwhOffCExpFlag > 0)))
	{
		if(sTime >= 300)
		{
			/*延时30s清除告警*/
			sSwhOnCExpFlag = 0;
			sSwhOffCExpFlag = 0;
		}
		else
		{
			sTime++;
		}
	}
	else
	{
		sTime = 0;
	}
}

/*=============================================================================================
** @Name      : void DiagClrPerDevExpFlagAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除外部设备检测异常标志
** @The notes : 高压侧器件故障标志
**===========================================================================================*/
void DiagClrPerDevExpFlagAPI(void)
{
	sHigVoltExpFlag = 0;                            /*高压采样值异常标志 bit0:总电压异常 bit1:输出端电压异常 [0:正常 1:异常]*/
	sHVSubDiffFlag = 0;                             /*高压采样内外压差过大标志 [0:正常 1:压差过大]*/
	sSwhOnVExpFlag = 0;                             /*电压判断接触器闭合异常标志 按位表示 Bit0:主接触器 bit1:预充接触器  bit2:负极接触器 [0:正常 1:异常]*/
	sSwhOffVExpFlag = 0;                            /*电压判断接触器断开异常标志 按位表示 Bit0:主接触器 bit1:预充接触器  bit2:负极接触器 [0:正常 1:异常]*/
	sSwhOnCExpFlag = 0;                             /*接触器带载闭合标志(按位表示各接触器)*/
	sSwhOffCExpFlag = 0;                            /*接触器带载断开标志(按位表示各接触器)*/

	sPeripDevExpFlag = 0;                           /*电压判断外围器件异常标志 按位表示 Bit0:熔断器 bit1:断路器 [0:正常 1:异常]*/
	sDcSideLeakFlag = 0;                            /*DC侧绝缘故障标志(0:正常 1:一级(严重)故障 2:二级(一般)故障 3:采样失效)*/

	sGroupEcuRepFlag = 0;                           /*电池组主控设备地址冲突标志(0正常 1重复)*/

	sAFESErrSlaveId = 0;                            /*模拟前端采样异常从控序号[1-32]*/
	sAFESErrFlag = 0;                               /*模拟前端采样异常从控内部故障(0:正常 1:异常)*/
	sAFEVOffSlaveId = 0;                            /*模拟前端检测单体电压采样断线从控序号[1-32]*/
	sAFEVOffCellBit = 0;                            /*模拟前端检测单体电压采样断线从控内部故障位(按位表示1个电压通道)*/
	sCellVOffSlaveId = 0;                           /*采样值检测单体电压采样断线从控序号[1-32]*/
	sCellVOffCellBit = 0;                           /*采样值检测单体电压采样断线从控内部故障位(按位表示1个电压通道)*/
	sCellTOffSlaveId = 0;                           /*采样值检测单体温度采样断线从控序号[1-32]*/
	sCellTOffCellBit = 0;                           /*采样值检测单体温度采样断线从控内部故障位(按位表示1个温度通道)*/

	sBalTOffSlaveId = 0;                            /*均衡板温断线从控序号[1-32]*/
	sBalTOffCellBit = 0;                            /*均衡板温断线从控内部故障位(按位表示1个温度点)*/
	sBalErrSlaveId = 0;                             /*均衡控制故障从控序号[1-32]*/
	sBalErrCellBit = 0;                             /*均衡控制故障从控内部故障位(按位表示1个均衡通道)*/

}

/*=================================================================================================
** @Name      : u8 DiagGetHigVoltExpFlagAPI(void)
** @Input     : void
** @Output    : 按位表示高压采样值异常[bit0:总电压,bit1:预充电压]
** @Function  : 获取各高压采样值异常标志
** @The notes :
**===============================================================================================*/
u8 DiagGetHigVoltExpFlagAPI(void)
{
	return(sHigVoltExpFlag);
}

/*=================================================================================================
** @Name      : u8 DiagGetHigVSubDiffFlagAPI(void)
** @Input     : void
** @Output    : 高压内外压差过大标志[0:正常,1:压差过大]
** @Function  : 获取高压内外压差过大标志
** @The notes :
**===============================================================================================*/
u8 DiagGetHigVSubDiffFlagAPI(void)
{
	return(sHVSubDiffFlag);
}

/*=================================================================================================
** @Name      : u8 DiagGetSwhStateVExpFlagAPI(void)
** @Input     : void
** @Output    : 按位表示各接触器状态电压值异常[bit0:正极接触器断开状态,bit1:预充接触器断开状态,bit2:负极接触器断开状态;bit4:正极接触器闭合状态,bit5:预充接触器闭合状态,bit6:负极接触器闭合状态]
** @Function  : 获取各接触器状态电压值异常标志
** @The notes : 高4位闭合状态电压异常,低4位断开状态电压异常
**===============================================================================================*/
u8 DiagGetSwhStateVExpFlagAPI(void)
{
	return((sSwhOnVExpFlag << 4) | (sSwhOffVExpFlag & 0x0f));
}

/*=================================================================================================
** @Name      : u8 DiagGetSwhChangCExpFlagAPI(void)
** @Input     : void
** @Output    : 按位表示各接触器动作带载异常[bit0:正极接触器断开,bit1:预充接触器断开,bit2:负极接触器断开;bit4:正极接触器闭合,bit5:预充接触器闭合,bit6:负极接触器闭合]
** @Function  : 获取各接触器带载动作异常标志
** @The notes : 高4位闭合带载异常,低4位断开带载异常
**===============================================================================================*/
u8 DiagGetSwhChangCExpFlagAPI(void)
{
	return((sSwhOnCExpFlag << 4) | (sSwhOffCExpFlag & 0x0f));
}

/*=================================================================================================
** @Name      : u8 DiagGetPeripDevExpFlagAPI(void)
** @Input     : void
** @Output    : 按位表示各外围器件异常[Bit0:熔断器 bit1:断路器 [0:正常 1:异常]]
** @Function  : 获取各外围器件异常标志
** @The notes :
**===============================================================================================*/
u8 DiagGetPeripDevExpFlagAPI(void)
{
	return(sPeripDevExpFlag);
}

/*=================================================================================================
** @Name      : u8 DiagGetDcSideLeakFlagAPI(void)
** @Input     : void
** @Output    : 绝缘检测异常标志[0:正常 1:一级故障(严重) 2:二级告警(轻微) 3:检测失效]
** @Function  : 获取高压侧绝缘检测异常标志
** @The notes :
**===============================================================================================*/
u8 DiagGetDcSideLeakFlagAPI(void)
{
	return(sDcSideLeakFlag);
}

/*=================================================================================================
** @Name      : u8 DiagGetRcvAllSlaveFlagAPI(void)
** @Input     : void
** @Output    : 接收到全部从控报文标志[0:未接收,1:正在接收,2:全部接收]
** @Function  : 获取接收到全部从控报文标志
** @The notes : 任一帧报文
**===============================================================================================*/
u8 DiagGetRcvAllSlaveFlagAPI(void)
{
	//不检测从控通讯故障直接认为接收完成
	if(0 == DiagGetFilErrSortAPI(eERR_ID_INT_COFF))
	{
		return(2);
	}

	return(sRcvAllSlaveFlag);
}

/*=================================================================================================
** @Name      : u8 DiagGetRcvAllCellFlagAPI(void)
** @Input     : void
** @Output    : 接收到全部单体信息报文标志[0:未接收,1:正在接收,2:全部接收]
** @Function  : 获取接收到全部单体信息报文标志
** @The notes : 全部单体电压和温度
**===============================================================================================*/
u8 DiagGetRcvAllCellFlagAPI(void)
{
	//不检测从控通讯故障直接认为接收完成
	if(0 == DiagGetFilErrSortAPI(eERR_ID_INT_COFF))
	{
		return(2);
	}

	return(sRcvAllCellFlag);
}

/*=================================================================================================
** @Name      : u8 DiagGetRcvAllPeripFlagAPI(void)
** @Input     : void
** @Output    : 接收到全部外设报文标志[0:未接收,1:正在接收,2:全部接收]
** @Function  : 获取接收到全部外设报文标志
** @The notes : 高压模块
**===============================================================================================*/
u8 DiagGetRcvAllPeripFlagAPI(void)
{
	//不检测内设通讯故障直接认为接收完成
	if(0 == DiagGetFilErrSortAPI(eERR_ID_PER_COFF))
	{
		return(2);
	}

	return(sRcvAllPeripFlag);
}

/*=================================================================================================
** @Name      : u8 DiagGetRcvPerOffCommuBitAPI(void)
** @Input     : void
** @Output    : 通信中断的内部子系统[Bit0:IMLU Bit1:IMEU]
** @Function  : 获取通信中断的内部子系统号
** @The notes : 按BIT位表示
**===============================================================================================*/
u8 DiagGetRcvPerOffCommuBitAPI(void)
{
	return(sPerOffCommuBit);
}

/*=================================================================================================
** @Name      : u8 DiagGetPerImluOffCommuStateAPI(void)
** @Input     : void
** @Output    : IMLU1通信中断状态[0:正常 1:通信中断]
** @Function  : 获取外设IMLU1通信中断状态
** @The notes :
**===============================================================================================*/
u8 DiagGetPerImluOffCommuStateAPI(void)
{
	return(BitGet(sPerOffCommuBit, 0));
}

/*=================================================================================================
** @Name      : u8 DiagGetPerImeuOffCommuStateAPI(void)
** @Input     : void
** @Output    : IMEU2通信中断状态[0:正常 1:通信中断]
** @Function  : 获取外设IMEU2通信中断状态
** @The notes :
**===============================================================================================*/
u8 DiagGetPerImeuOffCommuStateAPI(void)
{
	return(BitGet(sPerOffCommuBit, 1));
}

/*=================================================================================================
** @Name      : u8 DiagGetPerGasErrFlagAPI(void)
** @Input     : void
** @Output    : 可燃气体浓度异常标志[bit0:干接点 bit1:H2气体 bit2:CO气体  0:正常 1:异常]
** @Function  : 获取外设可燃气体浓度异常标志
** @The notes :
**===============================================================================================*/
u8 DiagGetPerGasErrFlagAPI(void)
{
	return(sPerGasErrBitFlag);
}

/*=================================================================================================
** @Name      : u8 DiagGetRcvSlaveMsgSlaveIdAPI(void)
** @Input     : void
** @Output    : 未接收到从控信息报文的从控序号[1,32]
** @Function  : 获取未接收到从控信息报文从控序号
** @The notes : 只记录1个从控
**===============================================================================================*/
u8 DiagGetRcvSlaveMsgSlaveIdAPI(void)
{
	u8 i = 0;
	u8 slaNum = 0;

	/*接收到全部从控信息*/
	if(2 == sRcvAllSlaveFlag)
	{
		return(0);
	}

	slaNum = (gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum] - 1) / gGBmuHigLevPara_103[eBmuHigLevPara103_ConNum];
	/*未接收完从控信息*/
	//for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; i++)
	for(i = 0; i < slaNum; i++)
	{
		if(0 == sRcvIntCount[i])
		{
			return(i + 1);
		}
	}

	return(0);
}

/*=================================================================================================
** @Name      : u8 DiagGetRcvCellInfoSlaveIdAPI(void)
** @Input     : void
** @Output    : 未接收到单体信息的从控序号[1,32]
** @Function  : 获取未接收到单体信息从控序号
** @The notes : 只记录1个从控
**===============================================================================================*/
u8 DiagGetRcvCellInfoSlaveIdAPI(void)
{
	/*接收到全部单体信息*/
	if(2 == sRcvAllCellFlag)
	{
		return(0);
	}
	/*接收完单体电压信息*/
	else if(sRcvAllCellVNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
	{
		/*接收完单体温度信息*/
		if(sRcvAllCellTNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum])
		{
			return(0);
		}
		/*未接收完单体温度信息*/
		else
		{
			/*返回未接收到温度的从控序号*/
			return(sRcvAllCellTNum / gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum] + 1);
		}
	}
	/*未接收完单体电压信息*/
	else
	{
		/*返回未接收到电压的从控序号*/
		return(sRcvAllCellVNum / gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum] + 1);
	}
}

/*=================================================================================================
** 从控采样异常检测
**===============================================================================================*/
/*=================================================================================================
** @Name      : void DiagRecordAFESampErrFlagAPI(u8 slave, u8 flag)
** @Input     : slave:从控序号[0,31] flag:异常标志(0:电压正常 1:电压异常 2:温度正常 3:温度异常)
** @Output    : void
** @Function  : 记录模拟前端检测到的采样异常标志
** @The notes : 芯片读取到的数据异常
**===============================================================================================*/
void DiagRecordAFESampErrFlagAPI(u8 slave, u8 flag)
{
	/*从控号错误*/
    if(slave >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
    {
        return;
    }

	/*故障错误*/
    if(flag >= 4)
    {
        return;
    }

    if(((0 == sAFESErrSlaveId) && (0 == sAFESErrFlag))       /*当前无断线记录*/
    	|| (sAFESErrSlaveId == (slave + 1)))                 /*已记录故障的从控*/
    {
    	sAFESErrSlaveId = slave + 1;                         /*记录芯片检测单体电压采样断线的从控号*/

    	if(0 == flag)                                        /*电压异常消除*/
    	{
    		BitClr(sAFESErrFlag, 0);
    	}
    	else if(1 == flag)                                   /*电压异常产生*/
    	{
    		//新产生
    		if(0 == BitGet(sAFESErrFlag, 0))
    		{
        		BitSet(sAFESErrFlag, 0);

        		//记录事件
    			DiagUserSelfErrRecord(eUDef_ErrRcd_VSampErr, eERR_RCDSORT_WARSET, 0,  0);
    		}
    	}
    	else if(2 == flag)                                   /*温度异常消除*/
    	{
    		BitClr(sAFESErrFlag, 1);
    	}
    	else                                                 /*温度异常产生*/
    	{
    		//新产生
    		if(0 == BitGet(sAFESErrFlag, 1))
    		{
        		BitSet(sAFESErrFlag, 1);

        		//记录事件
    			DiagUserSelfErrRecord(eUDef_ErrRcd_TSampErr, eERR_RCDSORT_WARSET, 0, 0);
    		}
    	}

    	if(0 == sAFESErrFlag)                                /*该从控无故障*/
    	{
    		sAFESErrSlaveId = 0;                             /*清除从控记录*/
    	}
    }
}

/*=================================================================================================
** @Name      : void DiagRecordAFEVOffLFlagAPI(u8 slave, u32 offCell)
** @Input     : slave:从控序号[0,31] offCell:断线的电池(按位表示1节电池)
** @Output    : void
** @Function  : 记录模拟前端检测到的单体电压断线标志
** @The notes : 芯片处理判断
**===============================================================================================*/
void DiagRecordAFEVOffLFlagAPI(u8 slave, u32 offCell)
{
	/*从控号错误*/
    if(slave >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
    {
        return;
    }

    if(((0 == sAFEVOffSlaveId) && (0 == sAFEVOffCellBit))    /*当前无断线记录*/
    	|| (sAFEVOffSlaveId == (slave + 1)))                 /*已记录故障的从控*/
    {
    	sAFEVOffSlaveId = slave + 1;                         /*记录芯片检测单体电压采样断线的从控号*/

		//新变化
		if(sAFEVOffCellBit != offCell)
		{
			sAFEVOffCellBit = offCell;                       /*记录芯片检测单体电压采样断线的从控内电压通道*/

			//新产生
			if(sAFEVOffCellBit > 0)
			{
				//记录事件
				DiagUserSelfErrRecord(eUDef_ErrRcd_CellVErr, eERR_RCDSORT_WARSET, 0, 0);
			}
		}

    	if(0 == sAFEVOffCellBit)                             /*该从控无故障*/
    	{
    		sAFEVOffSlaveId = 0;                             /*清除从控记录*/
    	}
    }
}

/*=================================================================================================
** @Name      : void DiagCheckCellVOffLFlagAPI(u8 slave, u8 cellNum, u16 volt)
** @Input     : slave:从控序号[0,31] cellNum:从控内电池号[0,17] volt:电压值[0,5000]mv
** @Output    : void
** @Function  : 检测单体电压采样断线标志
** @The notes : 采样电压判断
**===============================================================================================*/
void DiagCheckCellVOffLFlagAPI(u8 slave, u8 cellNum, u16 volt)
{
	static u32 sRcdFlag = 0;

	/*从控序号或电池号错误*/
	if((slave >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
		|| (cellNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum]))
    {
        return;
    }

	//兼容CATL液冷PACK电池52个电压解析
	#if(4 == PRJ_PACK_CELL_INFO)
	{
		//每3个从控首个少2S(仅前16个温度有效)
		if((0 == (slave % 3)) && (cellNum >= 16))
		{
			return;
		}
	}
	#endif

    if(((0 == sCellVOffSlaveId) && (0 == sCellVOffCellBit))     /*当前无断线记录*/
        || (sCellVOffSlaveId == (slave + 1)))                   /*已记录故障的从控*/
    {
        if((volt <= DEFAULT_LO_MINV)          					/*小于断线低压点阈值*/
            || (volt >= DEFAULT_LO_MAXV))     					/*大于断线高压点阈值*/
        {
            sCellVOffSlaveId = slave + 1;                       /*记录芯片检测单体电压采样断线的从控号*/

    		//新产生
    		if(0 == BitGet(sCellVOffCellBit, cellNum))
    		{
    			BitSet(sCellVOffCellBit, cellNum);              /*记录芯片检测单体电压采样断线的从控内电压通道*/

        		//记录事件
    			//DiagUserSelfErrRecord(eUDef_ErrRcd_CellVErr, eERR_RCDSORT_WARSET, volt, 0);
    		}
    		//重复产生
    		else
    		{
    			//未记录事件
    			if(0 == BitGet(sRcdFlag, cellNum))
    			{
    				//记录事件
    				DiagUserSelfErrRecord(eUDef_ErrRcd_CellVErr, eERR_RCDSORT_WARSET, volt, 0);
    				BitSet(sRcdFlag, cellNum);
    			}
    		}
        }
        else
        {
            BitClr(sCellVOffCellBit, cellNum);                  /*记录芯片检测单体电压采样断线的从控内电压通道*/
            BitClr(sRcdFlag, cellNum);

        	if(0 == sCellVOffCellBit)                           /*该从控无故障*/
        	{
        		sCellVOffSlaveId = 0;                           /*清除从控记录*/
        	}
        }
    }
}

/*=================================================================================================
** @Name      : void DiagCheckCellTOffLFlagAPI(u8 slave, u8 cellNum, s8 temp)
** @Input     : slave:从控序号[0,31] cellNum:从控内单体温度序号[0,15] temp:温度值[-40,105]℃
** @Output    : void
** @Function  : 检测单体电压采样断线标志
** @The notes : 单体电芯温度
**===============================================================================================*/
void DiagCheckCellTOffLFlagAPI(u8 slave, u8 cellNum, s8 temp)
{
	static u16 sRcdFlag = 0;

	/*从控序号或温度序号错误*/
	if((slave >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
		|| (cellNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuTNum]))
    {
        return;
    }

	//兼容CATL液冷PACK电池8个温度解析
	#if(4 == PRJ_PACK_CELL_INFO)
	{
		//每3个从控首个少1S(仅前2个温度有效)
		if((0 == (slave % 3)) && (cellNum >= 2))
		{
			return;
		}
	}
	#endif

	if(((0 == sCellTOffSlaveId) && (0 == sCellTOffCellBit))     /*当前无断线记录*/
		|| (sCellTOffSlaveId == (slave + 1)))                   /*已记录故障的从控*/
	{
		if((temp <= -41)          					            /*小于断线点温度阈值*/
			|| (temp >= 126))     					            /*大于短路点温度阈值*/
		{
			sCellTOffSlaveId = slave + 1;                       /*记录芯片检测单体电压采样断线的从控号*/

    		//新产生
    		if(0 == BitGet(sCellTOffCellBit, cellNum))
    		{
    			BitSet(sCellTOffCellBit, cellNum);              /*记录芯片检测单体电压采样断线的从控内温度通道*/

        		//记录事件
    			//DiagUserSelfErrRecord(eUDef_ErrRcd_CellTErr, eERR_RCDSORT_WARSET, (u16)temp, 0);
    		}
    		//重复产生
    		else
    		{
    			//未记录事件
    			if(0 == BitGet(sRcdFlag, cellNum))
    			{
    				//记录事件
    				DiagUserSelfErrRecord(eUDef_ErrRcd_CellTErr, eERR_RCDSORT_WARSET, (u16)temp, 0);
    				BitSet(sRcdFlag, cellNum);
    			}
    		}
		}
		else
		{
			BitClr(sCellTOffCellBit, cellNum);                  /*清除芯片检测单体温度采样断线的从控内温度通道*/
			BitClr(sRcdFlag, cellNum);

			if(0 == sCellTOffCellBit)                           /*该从控无故障*/
			{
				sCellTOffSlaveId = 0;                           /*清除从控记录*/
			}
		}
	}
}

/*=================================================================================================
** @Name      : void DiagCheckBalanTOffLFlagAPI(u8 slave, u8 cellNum, s8 temp)
** @Input     : slave:从控序号[0,31] cellNum:均衡温度序号[0,1] temp:温度值[-40,105]℃
** @Output    : void
** @Function  : 检测均衡板温采样断线标志
** @The notes : 均衡板温
**===============================================================================================*/
void DiagCheckBalanTOffLFlagAPI(u8 slave, u8 cellNum, s8 temp)
{
	/*从控序号或电池号错误*/
	if((slave >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
	    || (cellNum >= gGBmuHigLevPara_103[eBmuHigLevPara103_BalTNum]))
    {
        return;
    }

	if(((0 == sBalTOffSlaveId) && (0 == sBalTOffCellBit))       /*当前无断线记录*/
		|| (sBalTOffSlaveId == (slave + 1)))                    /*已记录故障的从控*/
	{
		if((temp <= -41)          					            /*小于断线点温度阈值*/
			|| (temp >= 126))     					            /*大于短路点温度阈值*/
		{
			sBalTOffSlaveId = slave + 1;                        /*记录芯片检测单体电压采样断线的从控号*/
			BitSet(sBalTOffCellBit, cellNum);                   /*记录芯片检测单体电压采样断线的从控内温度通道*/
		}
		else
		{
			BitClr(sBalTOffCellBit, cellNum);                   /*清除芯片检测单体温度采样断线的从控内温度通道*/

			if(0 == sBalTOffCellBit)                            /*该从控无故障*/
			{
				sBalTOffSlaveId = 0;                            /*清除从控记录*/
			}
		}
	}
}

/*=================================================================================================
 ** @Name      : void DiagRecordBalErrFlagAPI(u8 slave, u32 offCell)
 ** @Input     : slave:从控序号[0,31] errCell:均衡故障的电池通道(按位表示1节电池通道)
 ** @Output    : void
 ** @Function  : 记录均衡故障的电池通道标志
 ** @The notes : 只记录1个从控
 **===============================================================================================*/
void DiagRecordBalErrFlagAPI(u8 slave, u32 errCell)
{
 	/*从控号错误*/
    if(slave >= gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum])
    {
        return;
    }

    if(((0 == sBalErrSlaveId) && (0 == sBalErrCellBit))     /*当前无断线记录*/
        || (sBalErrSlaveId == (slave + 1)))                 /*已记录故障的从控*/
    {
    	sBalErrSlaveId = slave + 1;                         /*记录均衡故障的从控号*/
    	sBalErrCellBit = errCell;                           /*记录均衡故障的从控内均衡通道*/

    	if(0 == sBalErrCellBit)                             /*该从控无故障*/
    	{
    		sBalErrSlaveId = 0;                            /*清除从控记录*/
    	}
    }
}

/*=================================================================================================
** @Name      : u8 DiagGetCellSErrSlaveIdAPI(void)
** @Input     : void
** @Output    : 单体信息采样异常的从控序号[1,32]
** @Function  : 获取单体信息采样异常从控序号
** @The notes : 只记录1个从控
**===============================================================================================*/
u8 DiagGetCellSErrSlaveIdAPI(void)
{
	return(sAFESErrSlaveId);
}

/*=================================================================================================
** @Name      : u16 DiagGetCellSampErrFlagAPI(void)
** @Input     : void
** @Output    : 按bit位表示单体信息采样异常标志[bit0:单体电压 bit1:单体温度](0正常,1异常)
** @Function  : 获取单体信息采样异常标志
** @The notes :
**===============================================================================================*/
u16 DiagGetCellSampErrFlagAPI(void)
{
    return(sAFESErrFlag);
}

/*=================================================================================================
** @Name      : u8 DiagGetCellTOffSlaveIdAPI(void)
** @Input     : void
** @Output    : 单体电压采样线断线的从控序号[1,32]
** @Function  : 获取均单体电压采样线断线从控序号
** @The notes : 只记录1个从控
**===============================================================================================*/
u8 DiagGetCellVOffSlaveIdAPI(void)
{
	if(sCellVOffSlaveId > 0)
	{
		return(sCellVOffSlaveId);
	}
	else
	{
		return(sAFEVOffSlaveId);
	}
}

/*=================================================================================================
** @Name      : u32 DiagGetCellVOffCellBitAPI(void)
** @Input     : void
** @Output    : 单体电压采样线断线的从控内电压通道(按位表示1个电压通道[0正常,1异常])
** @Function  : 获取单体电压采样线断线的从控内电压通道
** @The notes : 按位表示1个电压通道
**===============================================================================================*/
u32 DiagGetCellVOffCellBitAPI(void)
{
    return(sAFEVOffCellBit | sCellVOffCellBit);
}

/*=================================================================================================
** @Name      : u8 DiagGetCellTOffSlaveIdAPI(void)
** @Input     : void
** @Output    : 单体温度采样线断线的从控序号[1,32]
** @Function  : 获取单体温度采样线断线从控序号
** @The notes : 只记录1个从控
**===============================================================================================*/
u8 DiagGetCellTOffSlaveIdAPI(void)
{
	return(sCellTOffSlaveId);
}

/*=================================================================================================
** @Name      : u16 DiagGetCellTOffCellBitAPI(void)
** @Input     : void
** @Output    : 单体温度采样线断线的从控内温度点位(按位表示1个温度点[0正常,1异常])
** @Function  : 获取单体温度采样线断线的从控内温度点位
** @The notes : 按位表示1个温度点
**===============================================================================================*/
u16 DiagGetCellTOffCellBitAPI(void)
{
    return(sCellTOffCellBit);
}

/*=================================================================================================
** @Name      : u8 DiagGetBalTOffSlaveIdAPI(void)
** @Input     : void
** @Output    : 均衡板温采样线断线的从控序号[1,32]
** @Function  : 获取均衡板温采样线断线从控序号
** @The notes : 只记录1个从控
**===============================================================================================*/
u8 DiagGetBalTOffSlaveIdAPI(void)
{
	return(sBalTOffSlaveId);
}

/*=================================================================================================
** @Name      : u16 DiagGetBalTOffCellBitAPI(void)
** @Input     : void
** @Output    : 均衡板温采样线断线的从控内温度点位(按位表示1个温度点[0正常,1异常])
** @Function  : 获取均衡板温采样线断线的从控内温度点位
** @The notes : 按位表示1个温度点
**===============================================================================================*/
u16 DiagGetBalTOffCellBitAPI(void)
{
	return(sBalTOffCellBit);
}

/*=================================================================================================
** @Name      : u8 DiagGetBalErrSlaveIdAPI(void)
** @Input     : void
** @Output    : 均衡控制失败的从控序号[1,32]
** @Function  : 获取均衡控制失败从控序号
** @The notes : 只记录1个从控
**===============================================================================================*/
u8 DiagGetBalErrSlaveIdAPI(void)
{
	return(sBalErrSlaveId);
}

/*=================================================================================================
** @Name      : u32 DiagGetBalErrCellBitAPI(void)
** @Input     : void
** @Output    : 均衡控制失败的从控内通道位(按位表示1个通道[0,1])
** @Function  : 获取均衡控制失败的从控内通道位
** @The notes : 按位表示1个通道
**===============================================================================================*/
u32 DiagGetBalErrCellBitAPI(void)
{
	return(sBalErrCellBit);
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/

