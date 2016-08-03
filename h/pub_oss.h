//
// Created by root on 16-6-29.
//

#ifndef SOCKET_PUB_OSS_H
#define SOCKET_PUB_OSS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef VOS_UNIX
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif
#include "pub_typedef.h"

/*
BYTE* OSS_GetUB(WORD32 dwSize);
VOID  OSS_RetUB(VOID* pucBuff);
*/

#define OSS_GetUB       malloc
#define OSS_RetUB       free

#ifdef VOS_UNIX
typedef pthread_t       VOS_TASK_T;
#else
typedef int       VOS_TASK_T;
#endif

#define INVALID_SYS_TASKID 0
#define THRSTACK_PROTECT_PAGENUM  1

typedef VOID (*TaskEntryProto)(LPVOID);

OSS_ULONG Vos_GetCurTaskID();

VOID OSS_Delay(WORD16 wCnt);

VOS_TASK_T Vos_StartTask(
        CHAR           *pucName,
        WORD16         wPriority,
        WORD32         dwStacksize,
        SWORD32        sdwOptions,
        TaskEntryProto tTaskEntry,
        OSS_ULONG      dwTaskPara1
);

#endif //SOCKET_PUB_OSS_H
