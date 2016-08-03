//
// Created by root on 16-6-29.
//

#ifndef SOCKET_PUB_TYPEDEF_H
#define SOCKET_PUB_TYPEDEF_H

#ifdef VOS_UNIX
#define BYTE        unsigned char
#define INT         int
#define WORD16      unsigned short
#define WORD32      unsigned int
#define CHAR        char
#define OSS_ULONG   unsigned long
#define OSS_LONG    long
#define VOID        void
#define BOOL        bool
#define BOOLEAN     bool
#define SWORD32     signed int

typedef void*       LPVOID;

#define NULL        0

#define TRUE        true
#define FALSE       false

#else

#include <windows.h>
#include <stdarg.h>
#include <windef.h>
#include <stdio.h>

/******************************* Windows NT专用的宏 ***********************/
/* OSS_STATUS类型的具体取值见Pub_ErrorDef.h */
#define WAIT_FOREVER    (WORD32)0xFFFFFFFF
#define NO_WAIT         (WORD32)0
#define CEOF            (BYTE)(0xFF)
#define LEOF            (WORD32)(0xFFFFFFFF)
#define PACKED
#define SOCKET_ERROR_VALUE     SOCKET_ERROR       /* socket库调用出错值 */
#define INVALID_SOCKET_VALUE   INVALID_SOCKET     /* 无效的socket */
#define FILE_ERROR_VALUE       SOCKET_ERROR       /* 文件调用出错值 */
#define FILE_RDWR_VALUE        (_S_IREAD|_S_IWRITE)/* 创建文件类型可读写 */
#define OssMsgQId            WORD32                /* msgQId类型 */
#define EV_MSG_SEND_BY_OSS   WM_USER+1            /* 进程间通讯的消息号 */
#define NULL_MSG_QID     (WORD32)0                 /* 无效的msgQId */
#define INADDR_NONE_VALUE      INADDR_NONE        /* 无效的IP地址 */
/**************************end of Windows NT专用的宏 ***********************/



/***********************************************************
 *                     全局数据类型                        *
***********************************************************/
/*
含义                 类型名
8位整数              CHAR
无符号8位整数        BYTE
无符号16位整数       WORD16
无符号32位整数       WORD32
无符号64位整数       WORD64
有符号16位整数       SWORD16
有符号32位整数       SWORD32
有符号64位整数       SWORD64
双精度浮点数         DOUBLE
单精度浮点数         FLOAT
无类型               VOID
整型                 INT
无符号整型           尚未提供
无类型指针           LPVOID
布尔型               BOOLEAN
*/
/********************* Windows NT和VxWorks下公用的数据类型 *********************/
typedef unsigned short      WORD16;
typedef signed short        SWORD16;
/* 2015-07-30 luyijun modify for vc compile 
typedef unsigned long       WORD32;*/
typedef unsigned int       WORD32;
typedef signed long         SWORD32;
typedef unsigned long       OSS_STATUS; 
typedef struct fd_set	    SOCKET_SET; /* OSS调用的一般返回类型 */

/********************* end of Windows NT和VxWorks下公用的数据类型 *********************/


/*************************** Windows NT下专用的数据类型 ***************************/
typedef int (*FUNCPTR)();
typedef LONGLONG  SWORD64;
typedef ULONGLONG WORD64;
typedef unsigned long OSS_ULONG;
typedef  long OSS_LONG;
/***********************end of Windows NT下专用的数据类型 ***************************/


#endif


#endif //SOCKET_PUB_TYPEDEF_H
