/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPTypeDef.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : BSP通用数据类型定义模块
** @Instructions :
**===========================================================================================*/
#ifndef _BSPTYPEDEF_H
#define _BSPTYPEDEF_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/

/*=============================================================================================
** @Macro definition
**===========================================================================================*/

/*=============================================================================================
** @Data type definition
**===========================================================================================*/

typedef unsigned char       uint8;      /*无符号8位整型变量 */
typedef signed   char       int8;       /*有符号8位整型变量 */
typedef unsigned short int  uint16;     /*无符号16位整型变量 */
typedef signed   short int  int16;      /*有符号16位整型变量 */
typedef unsigned long int   uint32;     /*无符号32位整型变量 */
typedef signed   long int   int32;      /*有符号32位整型变量 */

typedef uint8   BOOLEAN;
typedef uint8   INT8U;
typedef int8    INT8S;
typedef uint16  INT16U;
typedef int16   INT16S;
typedef uint32  INT32U;
typedef int32   INT32S;

typedef uint8   UINT8;
typedef int8    SINT8;
typedef uint16  UINT16;
typedef int16   SINT16;
typedef uint32  UINT32;
typedef int32   SINT32;

typedef INT8U   u8;
typedef INT8S   s8;
typedef INT16U  u16;
typedef INT16S  s16;
typedef INT32U  u32;
typedef INT32S  s32;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

/*
#ifndef FAR_NULL
#define FAR_NULL ((void * far)0)
#endif
*/
/*
typedef signed char int8_t;
typedef short int   int16_t;
typedef long int    int32_t;
typedef long long   int64_t;

typedef unsigned char		uint8_t;
typedef unsigned short int  uint16_t;
typedef unsigned long int   uint32_t;
typedef unsigned long long  uint64_t;
*/
typedef enum                    /*功能参数常用状态返回*/
{
    ok = 0,                     /*执行正确*/
    failed = 1,                 /*失败*/
    errP0 = 2,                  /*参数0错误*/
    errP1 = 3,                  /*参数1错误*/
    errP2 = 4,                  /*参数2错误*/
    NOUSE = 5                   /*调用无效*/
}e_FunStatus;

#define IsEqual(a,b)            ((a) == (b))                                /*a等于b返回1 否则返回0*/
#define IsNotEqual(a,b)         ((a)!=(b))                                  /*a不等于b返回1 否则返回0*/
#define BitSet(target,index)    ((target) |= (u32)0x01<<(index))            /*将数target的第index位设置为1*/
#define BitClr(target,index)    ((target) &= (~((u32)0x01<<(index))))       /*将数target的第index位清0*/
#define BitGet(target,index)    (((target) & (u32)0x01<<(index))>>(index))  /*获得数据target的第index位的值*/
#define BitSet16(target,index)  ((target) |= (u16)0x01<<(index))
#define BitClr16(target,index)  ((target) &= (~((u16)0x01<<(index))))
#define BitGet16(target,index)  (((target) & (u16)0x01<<(index))>>(index))
#define BitSet8(target,index)   ((target) |= (u8)0x01<<(index))             /*将数target的第index位设置为1*/
#define BitClr8(target,index)   ((target) &= (~((u8)0x01<<(index))))        /*将数target的第index位清0*/
#define BitGet8(target,index)   (((target) & (u8)0x01<<(index))>>(index))   /*获得数据target的第index位的值*/

#define IsInside(x1,x,x2)       (((x1)<=(x))&&((x)<=(x2)))                  /*判断数x是否在x1与x2之间*/
#define ABS(target,index)       ((target) > (index)?((target) - (index)):((index) - (target)))/*两数相减正数差值*/

#define BSPDelay(Count)         {volatile u16 i = Count; while(i--);}       /*BSP延时函数*/

#define UPPER_BYTE(x)           ((x >> 8) & 0xff)
#define LOWER_BYTE(x)       	(x & 0xff)
#define UPPER_WORD(x)       	((x >> 16) & 0xffff)
#define LOWER_WORD(x)       	(x & 0xffff)
#define UPPER_NIBBLE(x)     	((x >> 4) & 0x0f)
#define LOWER_NIBBLE(x)     	(x & 0x0f)

#endif
