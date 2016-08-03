//
// Created by root on 16-6-22.
//
#include "pritcpsocket.h"


T_TcpSocketMgr  gtTcpSocketMgr[TCP_MGR_MAX] = {0};

VOID  TcpSocketTask(VOID)
{
    T_TcpSocketMgr* ptTcpSocketMgr = NULL;
    OSS_ULONG  dwTaskId = Vos_GetCurTaskID();

    while(1)
    {
        ptTcpSocketMgr = GetTcpSocketMgrByTaskId(dwTaskId);

        if (NULL != ptTcpSocketMgr)
        {
            TcpSocketDealAccept(ptTcpSocketMgr);
            TcpSocketDealRecv(ptTcpSocketMgr);
        }
        else
        {
            OSS_Delay(1000);
        }
    }
}

BOOL InitTcpSocket(CHAR *pSocketName)
{
    OSS_ULONG dwTaskId = 0;
    T_TcpSocketMgr* ptTcpSocketMgr = GetTcpSocketMgr(0);

#ifdef VOS_WINNT
    WSADATA  wsaData;

    WSAStartup(MAKEWORD(2,2),&wsaData);
#endif

    dwTaskId = Vos_StartTask(pSocketName, 120, 1024 * 20, 0, (TaskEntryProto) TcpSocketTask, 0);
    if (INVALID_SYS_TASKID == dwTaskId)
    {
        return FALSE;
    }

    ptTcpSocketMgr->dwTaskId = dwTaskId;
    ptTcpSocketMgr->ucMgrIndex = 0;
    ptTcpSocketMgr->pServerLink = NULL;

    strncpy(ptTcpSocketMgr->acSocketName,pSocketName,64);

    return TRUE;
}

T_TcpSocketMgr* GetTcpSocketMgrByTaskId(OSS_ULONG dwTaskId)
{
    BYTE ucIndex = 0;
    T_TcpSocketMgr* ptTcpSocketMgr = NULL;

    for (ucIndex=0; ucIndex<16; ucIndex++)
    {
        ptTcpSocketMgr = GetTcpSocketMgr(ucIndex);
        if (ptTcpSocketMgr->dwTaskId == dwTaskId)
        {
            return ptTcpSocketMgr;
        }
    }

    return NULL;
}

T_TcpSocketMgr* GetTcpSocketMgrByName(CHAR *pName)
{
    BYTE ucIndex = 0;
    T_TcpSocketMgr* ptTcpSocketMgr = NULL;

    for (ucIndex=0; ucIndex<16; ucIndex++)
    {
        ptTcpSocketMgr = GetTcpSocketMgr(ucIndex);
        if (!strcmp(ptTcpSocketMgr->acSocketName,pName))
        {
            return ptTcpSocketMgr;
        }
    }

    return NULL;
}
