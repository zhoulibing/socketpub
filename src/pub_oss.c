//
// Created by root on 16-6-29.
//

#include "pub_oss.h"
#ifdef VOS_UNIX
#include "pthread.h"
#else
#include <process.h>
#include <windows.h>
#endif

OSS_ULONG Vos_GetCurTaskID()
{
#ifdef VOS_UNIX
    return (OSS_ULONG)pthread_self();
#else
    return (OSS_ULONG)GetCurrentThreadId();
#endif
}

VOID OSS_Delay(WORD16 wCnt)
{
#ifdef VOS_UNIX
    usleep(wCnt*1000);
#else
    Sleep(wCnt);
#endif
}


VOS_TASK_T Vos_StartTask(
        CHAR           *pucName,
        WORD16         wPriority,
        WORD32         dwStacksize,
        SWORD32        sdwOptions,
        TaskEntryProto tTaskEntry,
        OSS_ULONG      dwTaskPara1
)
{
#ifdef VOS_UNIX
    pthread_t             threadID;
    INT                   errcode   = 0;

    errcode = pthread_create(&threadID,NULL,(VOID*)tTaskEntry,(VOID *)NULL);
    if (0 != errcode)
    {
        return INVALID_SYS_TASKID;
    }

    return threadID;
#else

    WORD32 dwTaskId = INVALID_SYS_TASKID;

    if (CreateThread(NULL,
                    dwStacksize,
                    (LPTHREAD_START_ROUTINE)tTaskEntry,
                    (LPVOID)dwTaskPara1,
                    0,
                    &dwTaskId
                    ) == NULL)
    {
        return INVALID_SYS_TASKID;
    }

    return dwTaskId;

#endif
}
