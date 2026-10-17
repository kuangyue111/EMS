/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : MbQueue.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : Modbus通信包队列定义模块
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPUserAPI.h"
#include "MbQueue.h"
                       
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 MBQueInit(void* pQ, u16 unitSize, u16 unitNum)
** @Input     : pQ:队列地址指针，unitSize:单元大小，unitNum:单元数目
** @Output    : TRUE OR FALSE
** @Function  : 将pQ指向的地址初始化为队列
** @The notes : pQ分配的空间要和参数符合
**===============================================================================================*/
u8 MBQueInit(void* pQ, u8 unitSize, u8 unitNum)
{	
	if(pQ == (void*)0)
	{
		return(FALSE);
	}

	if(unitSize == 0)
	{
		return(FALSE);
	}

	if(unitNum == 0)
	{
		return(FALSE);
	}

	//初始化队列
	MB_QUE_CTRLFLAG(pQ) = 0;									
	MB_QUE_UNITSIZE(pQ) = unitSize;							
	MB_QUE_UNITNUM(pQ) = unitNum;								
	MB_QUE_INPOINT(pQ) = 0;									
	MB_QUE_OUTPOINT(pQ) = 0;									
	MB_QUE_CURNUM(pQ) = 0;										
	
	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 MBQueIn(void* pQ, void * pU)
** @Input     : pQ:队列指针，pU:待写入单元指针
** @Output    : 通信报文进入队列结果:TRUE OR FALSE
** @Function  : 将pu指向单元压入pQ队列
** @The notes :
**===============================================================================================*/
u8 MBQueIn(void* pQ, void * pU)
{
	u8 i = 0;
	
	//指向空间为空错误
	if(pQ == (void*)0)							
	{
		return(FALSE);
	}
	
	//队列满
	if(MB_QUE_UNITNUM(pQ) == MB_QUE_CURNUM(pQ))	
	{
		return(FALSE);
	}
	
	//队列单元数++
	MB_QUE_CURNUM(pQ) += 1;									

	for(i = 0; i < MB_QUE_UNITSIZE(pQ); i++)
	{
		*(MB_QUEP_UNIT(pQ) + MB_QUE_INPOINT(pQ) * MB_QUE_UNITSIZE(pQ) + i) = *((u16*)pU + i);
	}
	MB_QUE_INPOINT(pQ) += 1;		
	
	//如果到头
	if(MB_QUE_INPOINT(pQ) >= MB_QUE_UNITNUM(pQ))					
	{
		MB_QUE_INPOINT(pQ) = 0;
	}
		
	return(TRUE);
}

/*=================================================================================================
** @Name      : u8 MBQueOut(void* pQ, void * pU)
** @Input     : pQ:队列指针，pU:读出单元写入指针
** @Output    : TRUE OR FALSE
** @Function  : 将pQ中数据读出到pU中
** @The notes :
**===============================================================================================*/
u8 MBQueOut(void* pQ, void * pU)
{
	u8 i = 0;
	
	//指向空间为空错误
	if(pQ == (void*)0)							
	{
		return(FALSE);
	}
	
	//队列空
	if(MB_QUE_UNITNUM(pQ) == 0)						
	{
		return(FALSE);
	}
	
	//队列空
	if(MB_QUE_CURNUM(pQ) == 0)						
	{
		return(FALSE);
	}
	
	//队列单元数--
	MB_QUE_CURNUM(pQ) -= 1;									

	for(i = 0; i < MB_QUE_UNITSIZE(pQ); i++)
	{
		*((u16*)pU + i) = *(MB_QUEP_UNIT(pQ) + MB_QUE_OUTPOINT(pQ) * MB_QUE_UNITSIZE(pQ) + i);
	}
	
	MB_QUE_OUTPOINT(pQ) += 1;
	
	//如果到头
	if(MB_QUE_OUTPOINT(pQ) >= MB_QUE_UNITNUM(pQ))					
	{
		MB_QUE_OUTPOINT(pQ) = 0;
	}	
	
	return(TRUE);
}

/*=================================================================================================
** @Name      : u16 MBQueGetUnitNum(void * pQ)
** @Input     : pQ:队列指针
** @Output    : 当前队列长度
** @Function  : 获得当前队列长度
** @The notes :
**===============================================================================================*/
u16 MBQueGetUnitNum(void * pQ)
{
	//指向空间为空错误
	if(pQ == (void*)0)							
	{
		return(FALSE);
	}
	
	return(MB_QUE_CURNUM(pQ));
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
