/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : OSUserHOOK.c
** @Founder      : ZSY342
** @Date         : 2019.10.31
** @Function     : BSP通用队列函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPIRQ.h"
#include "BSPQueue.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 QueInit(void *pQ, u8 unitSize, u8 unitNum)
** @Input     : pQ:队列地址指针，unitSize:单元大小，unitNum:单元数目
** @Output    : TRUE OR FALSE
** @Function  : 将pQ指向的地址初始化为队列
** @The notes : pQ分配的空间要和参数符合
**===========================================================================================*/
u8 QueInit(void *pQ, u8 unitSize, u8 unitNum)
{
    if(IsEqual(pQ, (void *)0))
    {
        return(FALSE);
    }
    
    if(IsEqual(unitSize, 0))
    {
        return(FALSE);
    }
    
    if(IsEqual(unitNum, 0))
    {
        return(FALSE);
    }
    
    BSP_DISABLE_IRQ();
    QUE_CTRLFLAG(pQ) = 0;
    QUE_UNITSIZE(pQ) = unitSize;
    QUE_UNITNUM(pQ)  = unitNum;
    QUE_INPOINT(pQ)  = 0;
    QUE_OUTPOINT(pQ) = 0;
    QUE_CURNUM(pQ)   = 0;
    BSP_ENABLE_IRQ();
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 QueIn(void *pQ, void *pU)
** @Input     : pQ:队列指针，pU:待写入单元指针
** @Output    : TRUE OR FALSE
** @Function  : 将pu指向单元压入pQ队列
** @The notes :
**===========================================================================================*/
u8 QueIn(void *pQ, void *pU)
{
    u8 i = 0;
    
    if(IsEqual(pQ, (void*)0))                               //指向空间为空错误
    {
        return(FALSE);
    }
    
    if(IsEqual(QUE_UNITNUM(pQ), QUE_CURNUM(pQ)))            //队列满
    {
        return(FALSE);
    }
    
    BSP_DISABLE_IRQ();
    
    QUE_CURNUM(pQ) += 1;                                    //队列单元数++
    
    for(i = 0; i < QUE_UNITSIZE(pQ); i++)
    {
        *(QUEP_UNIT(pQ) + QUE_INPOINT(pQ) * QUE_UNITSIZE(pQ) + i) = *((u8*)pU+i);
    }
    
    QUE_INPOINT(pQ) += 1;
    
    if(QUE_INPOINT(pQ) >= QUE_UNITNUM(pQ))                  //如果到头
    {
        QUE_INPOINT(pQ) = 0;
    }
    
    BSP_ENABLE_IRQ();
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 QueInDiff(void *pQ, void *pU)
** @Input     : pQ:队列指针，pU:待写入单元指针
** @Output    : TRUE OR FALSE
** @Function  : 将pu指向单元与队列中现有单元对比，如果没有则压入pQ队列
** @The notes : 仅用于u8单位成员
**===========================================================================================*/
u8 QueInDiff(void *pQ, void *pU)
{
    u8 j = 0;
    u8 size = 0;
    u8 isDiff = 1;
    u8 curNum = 0;
    u8 curPoint = 0;
    
    if(IsNotEqual(QUE_UNITSIZE(pQ), 1))                     //非u32成员
    {
        return(FALSE);
    }
    
    if(IsEqual(QUE_UNITNUM(pQ), QUE_CURNUM(pQ)))            //队列满
    {
        return(FALSE);
    }
    
    BSP_DISABLE_IRQ();
    
    curNum = QUE_CURNUM(pQ);                                //获得成员数目
    curPoint = QUE_OUTPOINT(pQ);
    size = QUE_UNITNUM(pQ);
    
    //检查当前队列中是否有相同成员
    for(j = 0; j < curNum; j++)
    {
        //如果有相同成员
        if(*(QUEP_UNIT(pQ) + curPoint) == *((u8 *)pU))
        {
            //设置标志
            isDiff = 0;
            break;
        }
        
        curPoint++;
        if(curPoint >= size)
        {
            curPoint = 0;
        }
    }
    
    //如果没有相同的成员
    if(isDiff == 1)
    {
        //插入此成员
        (void)QueIn(pQ,pU);
    }
    
    BSP_ENABLE_IRQ();
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 QueOut(void *pQ, void *pU)
** @Input     : pQ:队列指针，pU:读出单元写入指针
** @Output    : TRUE OR FALSE
** @Function  : 将pQ中数据读出到pU中
** @The notes :
**===========================================================================================*/
u8 QueOut(void *pQ, void *pU)
{
    u8 i = 0;
    
    if(IsEqual(pQ, (void*)0))                               //指向空间为空错误
    {
        return(FALSE);
    }
    
    if(IsEqual(QUE_UNITNUM(pQ), 0))                         //队列空
    {
        return(FALSE);
    }
    if(IsEqual(QUE_CURNUM(pQ), 0))                          //队列空
    {
        return(FALSE);
    }
    
    BSP_DISABLE_IRQ();
    
    QUE_CURNUM(pQ) -= 1;                                    //队列单元数--
    
    for(i = 0; i < QUE_UNITSIZE(pQ); i++)
    {
        *((u8*)pU+i) = *(QUEP_UNIT(pQ) + QUE_OUTPOINT(pQ) * QUE_UNITSIZE(pQ) + i);
    }
    
    QUE_OUTPOINT(pQ) += 1;
    
    if(QUE_OUTPOINT(pQ) >= QUE_UNITNUM(pQ))                 //如果到头
    {
        QUE_OUTPOINT(pQ) = 0;
    }    
    
    BSP_ENABLE_IRQ();
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 QueGetUnit(void *pQ, void *pU, u32 index)
** @Input     : pQ:队列指针，pU:读出单元写入指针
** @Output    : TRUE OR FALSE
** @Function  : 将pQ中数据读出到pU中,但是队列中不取消
** @The notes :
**===========================================================================================*/
u8 QueGetUnit(void *pQ, void *pU, u32 index)
{
    u8 i = 0;
    u8 num = 0;
    
    num = QUE_UNITNUM(pQ);
    
    if(IsEqual(pQ, (void *)0))                              //指向空间为空错误
    {
        return(FALSE);
    }
    
    if(IsEqual(num, 0))                                     //队列空
    {
        return(FALSE);
    }
    
    if(index >= QUE_CURNUM(pQ))                             //目前队列中没有这么多元素
    {
        return(FALSE);
    }
    
    BSP_DISABLE_IRQ();
    
    index += QUE_OUTPOINT(pQ);
    
    if(index >= num)
    {
        index -= num;
    }
    
    for(i = 0; i < QUE_UNITSIZE(pQ); i++)
    {
        *((u8*)pU+i) = *(QUEP_UNIT(pQ) + index * QUE_UNITSIZE(pQ) + i);
    }
    
    BSP_ENABLE_IRQ();
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 QueClear(void *pQ)
** @Input     : pQ:队列指针
** @Output    : TRUE OR FALSE
** @Function  : 清零队列
** @The notes :
**===========================================================================================*/
u8 QueClear(void *pQ)
{
    if(IsEqual(pQ, (void *)0))            //指向空间为空错误
    {
        return(FALSE);
    }
    
    //BSP_DISABLE_IRQ();
    QUE_CTRLFLAG(pQ) = 0;
    QUE_INPOINT(pQ)  = 0;
    QUE_OUTPOINT(pQ) = 0;
    QUE_CURNUM(pQ)   = 0;
    //BSP_ENABLE_IRQ();
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 QueGetUnitNum(void *pQ)
** @Input     : pQ:队列指针
** @Output    : TRUE OR FALSE
** @Function  : 获得当前队列长度
** @The notes :
**===========================================================================================*/
u8 QueGetUnitNum(void *pQ)
{
    if(IsEqual(pQ, (void *)0))          //指向空间为空错误
    {
        return(FALSE);
    }
    
    return(QUE_CURNUM(pQ));
}
