//
// Created by zlb on 16-6-23.
//
#include "tcpsocketserver.h"

VOID TcpsOnAcceptDefault(T_TcpClient* ptTcpClient, WORD32 dwClientAddr, WORD16 wClientPort)
{
}

VOID TcpsOnIdleDefault(T_TcpServer* ptTcpServer)
{
}

BOOL CreateServerSocket(T_TcpServer* ptTcpServer)
{
    struct sockaddr_in tServerAddr;
    INT                iRet = 0;
    INT                iOpt = 1;

    ptTcpServer->tServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(ptTcpServer->tServerSocket, SOL_SOCKET, SO_REUSEADDR, (VOID*)&iOpt, sizeof(INT));

    tServerAddr.sin_port = htons(ptTcpServer->wServerPort);
    tServerAddr.sin_addr.s_addr = htonl(ptTcpServer->dwServerAddr);
    tServerAddr.sin_family = AF_INET;
    iRet = bind(ptTcpServer->tServerSocket,(struct sockaddr*)&tServerAddr,sizeof(struct sockaddr_in));
    if (iRet < 0)
    {
        return FALSE;
    }

    iRet = listen(ptTcpServer->tServerSocket,10);
    if (iRet < 0)
    {
        return FALSE;
    }
    ptTcpServer->pfTcpsOnAccept = TcpsOnAcceptDefault;
    ptTcpServer->pfTcpsOnIdle = TcpsOnIdleDefault;

    return TRUE;
}

BOOL AddTcpServerToMgr(CHAR* pServerName,T_TcpServer* ptTcpServer)
{
    T_TcpSocketMgr* ptTcpSocketMgr = NULL;

    ptTcpSocketMgr = GetTcpSocketMgrByName(pServerName);
    if (NULL == ptTcpSocketMgr)
    {
        return FALSE;
    }

    ptTcpServer->ucMgrIndex = ptTcpSocketMgr->ucMgrIndex;

    if (ptTcpSocketMgr->pServerLink != NULL)
    {
        ptTcpServer->psLink = ptTcpSocketMgr->pServerLink;
    }
    ptTcpSocketMgr->pServerLink = ptTcpServer;

    return TRUE;
}

BOOL AddTcpClientToMgrByIndex(BYTE ucMgrIndex,T_TcpClient* ptTcpClient)
{
    T_TcpSocketMgr* ptTcpSocketMgr = NULL;

    ptTcpSocketMgr = GetTcpSocketMgr(ucMgrIndex);
    if (NULL == ptTcpSocketMgr)
    {
        return FALSE;
    }

    ptTcpClient->ucMgrIndex = ptTcpSocketMgr->ucMgrIndex;

    if (ptTcpSocketMgr->pClientLink != NULL)
    {
        ptTcpClient->pmLink = ptTcpSocketMgr->pClientLink;
    }
    ptTcpSocketMgr->pClientLink = ptTcpClient;

    return TRUE;
}

BOOL AddTcpClientToMgrByName(CHAR *pServerName, T_TcpClient *ptTcpClient)
{
    T_TcpSocketMgr* ptTcpSocketMgr = NULL;

    ptTcpSocketMgr = GetTcpSocketMgrByName(pServerName);
    if (NULL == ptTcpSocketMgr)
    {
        return FALSE;
    }

    return AddTcpClientToMgrByIndex(ptTcpSocketMgr->ucMgrIndex,ptTcpClient);
}

BOOL AddTcpClientToServer(T_TcpServer* ptTcpServer,T_TcpClient* ptTcpClient)
{
    if (!AddTcpClientToMgrByIndex(ptTcpServer->ucMgrIndex,ptTcpClient))
    {
        return FALSE;
    }

    ptTcpClient->ptTcpServer = ptTcpServer;

    if (ptTcpServer->pClientLink != NULL)
    {
        ptTcpClient->pcLink = ptTcpServer->pClientLink;
    }
    ptTcpServer->pClientLink = ptTcpClient;

    return TRUE;
}

SOCKET TcpSocketCreateAcceptFDSET(T_TcpServer *ptTcpServer, fd_set *ptfdAccept)
{
    T_TcpServer* ptServerLink = NULL;
    SOCKET          tMaxSocket = 0;
    fd_set          tfdAccept;

    FD_ZERO(&tfdAccept);

    ptServerLink = ptTcpServer;
    while(ptServerLink)
    {
        FD_SET(ptServerLink->tServerSocket,&tfdAccept);

        if (tMaxSocket < ptServerLink->tServerSocket)
        {
            tMaxSocket = ptServerLink->tServerSocket;
        }

        ptServerLink = ptServerLink->psLink;
    }

    *ptfdAccept = tfdAccept;

    return tMaxSocket;
}

VOID TcpSocketAcceptClient(T_TcpServer *ptTcpServer)
{
    T_TcpClient*      ptClientLink = NULL;
    struct sockaddr_in   tClientAddr;
    INT                  iAddrLen;

    ptClientLink = (T_TcpClient*)OSS_GetUB(sizeof(T_TcpClient));
    if (ptClientLink == NULL)
    {
        return;
    }

    ptClientLink->pfTcpcOnRecv = TcpcOnRecvDefault;
    ptClientLink->pfTcpcOnIdle = TcpcOnIdleDefault;
    ptClientLink->pfTcpcOnClose = TcpcOnCloseDefault;
#ifdef VOS_WINNT
    ptClientLink->tClientSocket = accept(ptTcpServer->tServerSocket,(SOCKADDR*)&tClientAddr, &iAddrLen);
#else
    ptClientLink->tClientSocket = accept(ptTcpServer->tServerSocket,(struct sockaddr *)&tClientAddr, (socklen_t *)&iAddrLen);
#endif
    (*ptTcpServer->pfTcpsOnAccept)(ptClientLink,tClientAddr.sin_addr.s_addr,tClientAddr.sin_port);

    AddTcpClientToServer(ptTcpServer,ptClientLink);
}

VOID TcpSocketAcceptISSET(T_TcpServer* ptTcpServer,fd_set* ptfdAccept)
{
    T_TcpServer*      ptServerLink = NULL;

    ptServerLink = ptTcpServer;
    while(ptServerLink)
    {
        if (FD_ISSET(ptServerLink->tServerSocket,ptfdAccept))
        {
            TcpSocketAcceptClient(ptServerLink);
        }
        else
        {
            if (NULL != ptServerLink->pfTcpsOnIdle)
            {
                (*ptServerLink->pfTcpsOnIdle)(ptServerLink);
            }
        }

        ptServerLink = ptServerLink->psLink;
    }
}

VOID TcpSocketDealAccept(T_TcpSocketMgr *ptTcpSocketMgr)
{
    fd_set          tfdAccept;
    SOCKET          tMaxSocket = 0;
    struct timeval tv = {1, 0};

    tMaxSocket = TcpSocketCreateAcceptFDSET(ptTcpSocketMgr->pServerLink, &tfdAccept);
    if (0 == tMaxSocket)
    {
        OSS_Delay(500);
        return;
    }

    select(tMaxSocket+1, &tfdAccept, NULL, NULL, &tv);

    TcpSocketAcceptISSET(ptTcpSocketMgr->pServerLink,&tfdAccept);

    return;
}

T_TcpServer* CreateTcpServer(CHAR *pSocketName, WORD32 dwServerAddr, WORD16 wServerPort)
{
    T_TcpServer*    ptTcpServer = NULL;

    ptTcpServer = (T_TcpServer*)OSS_GetUB(sizeof(T_TcpServer));
    if (NULL == ptTcpServer)
    {
        return NULL;
    }

    ptTcpServer->dwServerAddr = dwServerAddr;
    ptTcpServer->wServerPort = wServerPort;
    if (!CreateServerSocket(ptTcpServer))
    {
        OSS_RetUB(ptTcpServer);
        return NULL;
    }

    if(!AddTcpServerToMgr(pSocketName,ptTcpServer))
    {
        OSS_RetUB(ptTcpServer);
        return NULL;
    }

    return ptTcpServer;
}

VOID SetTcpServerCfg(T_TcpServer *ptTcpServer, BYTE ucType, VOID *pValue)
{
    if (NULL == ptTcpServer)
    {
        return;
    }
    switch(ucType)
    {
        case eTcpsOnIdle:
            ptTcpServer->pfTcpsOnIdle = (TcpsOnIdle)pValue;
            break;
        case eTcpsOnAccept:
            ptTcpServer->pfTcpsOnAccept = (TcpsOnAccept)pValue;
            break;
        default:
            break;
    }

    return;
}

BOOL TcpServerCloseClient(T_TcpClient* ptTcpClient)
{
    if (NULL == ptTcpClient)
    {
        return FALSE;
    }

    TcpSocketDelClient(ptTcpClient);

    return TRUE;
}
