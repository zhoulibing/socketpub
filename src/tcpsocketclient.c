//
// Created by zlb on 16-6-24.
//

#include "tcpsocketclient.h"

WORD16 TcpcOnRecvDefault(T_TcpClient* ptTcpClient,BYTE* ucBuff, WORD16 wLen)
{
    return wLen;
}

VOID TcpcOnIdleDefault(T_TcpClient* ptTcpClient)
{
}

VOID TcpcOnCloseDefault(T_TcpClient* ptTcpClient)
{
}

T_TcpClient* NextServerClient(T_TcpClient* ptPreClient,T_TcpClient* ptClientLink)
{
    if (ptPreClient != NULL)
    {
        ptPreClient->pcLink = ptClientLink->pcLink;
    }
    return ptClientLink->pcLink;
}

T_TcpClient* NextMgrClient(T_TcpClient* ptPreClient,T_TcpClient* ptClientLink)
{
    if (ptPreClient != NULL)
    {
        ptPreClient->pmLink = ptClientLink->pmLink;
    }

    return ptClientLink->pmLink;
}

VOID TcpSockeDelClientFromServer(T_TcpClient* ptTcpClient)
{
    T_TcpServer* ptTcpServer = ptTcpClient->ptTcpServer;

    if (NULL != ptTcpServer->pClientLink)
    {
        TcpSockeDelClientFromClientLink(&ptTcpServer->pClientLink, ptTcpClient,NextServerClient);
    }

    return;
}

VOID TcpSockeDelClientFromMgr(T_TcpClient* ptTcpClient)
{
    T_TcpSocketMgr* ptTcpSocketMgr = GetTcpSocketMgr(ptTcpClient->ucMgrIndex);

    if ((NULL != ptTcpSocketMgr)&&(NULL != ptTcpSocketMgr->pClientLink))
    {
        TcpSockeDelClientFromClientLink(&ptTcpSocketMgr->pClientLink, ptTcpClient,NextMgrClient);
    }

    return;
}

VOID TcpSockeDelClientFromClientLink(T_TcpClient** pLink,T_TcpClient* ptTcpClient,NextClient pfNext)
{
    T_TcpClient* ptCurTcpClient = *pLink;
    T_TcpClient* ptPreTcpClient = ptCurTcpClient;

    while(ptCurTcpClient)
    {
        if (ptCurTcpClient->tClientSocket == ptTcpClient->tClientSocket)
        {
            if (ptPreTcpClient == ptCurTcpClient)
            {
                *pLink = (*pfNext)(NULL,ptCurTcpClient);
            }
            else
            {
                (*pfNext)(ptPreTcpClient,ptCurTcpClient);
            }
            break;
        }
        ptPreTcpClient = ptCurTcpClient;
        ptCurTcpClient = (*pfNext)(NULL,ptCurTcpClient);
    }

    return;
}

BOOL TcpSocketDelClient(T_TcpClient* ptTcpClient)
{
    if (NULL != ptTcpClient->ptTcpServer)
        TcpSockeDelClientFromServer(ptTcpClient);

    TcpSockeDelClientFromMgr(ptTcpClient);
#ifdef VOS_WINNT
    closesocket(ptTcpClient->tClientSocket);
#else
    close(ptTcpClient->tClientSocket);
#endif
    OSS_RetUB(ptTcpClient);

    return TRUE;
}

SOCKET TcpSocketCreateRecvFDSET(T_TcpClient* ptTcpClient,fd_set* ptfdRecv)
{
    T_TcpClient*    ptClientLink = NULL;
    SOCKET          tMaxSocket = 0;
    fd_set          tfdRecv;

    FD_ZERO(&tfdRecv);

    ptClientLink = ptTcpClient;
    while(ptClientLink)
    {
        FD_SET(ptClientLink->tClientSocket,&tfdRecv);

        if (tMaxSocket < ptClientLink->tClientSocket)
        {
            tMaxSocket = ptClientLink->tClientSocket;
        }
        ptClientLink = ptClientLink->pmLink;
    }

    *ptfdRecv = tfdRecv;

    return tMaxSocket;
}

VOID TcpSocketRecv(T_TcpClient *ptClientLink)
{
    BYTE acBuff[1500] = {0};
    INT iRetSize = 0;

    iRetSize = (INT) recv(ptClientLink->tClientSocket, acBuff, 1500, 0);
    if (iRetSize > 0)
    {
        (*ptClientLink->pfTcpcOnRecv)(ptClientLink, acBuff, (WORD16) iRetSize);
    }
    else
    {
        (*ptClientLink->pfTcpcOnClose)(ptClientLink);

        TcpSocketDelClient(ptClientLink);
    }

    return;
}

VOID TcpSocketRecvISSET(T_TcpClient* ptTcpClient, fd_set* ptfdRecv)
{
    T_TcpClient*         ptClientLink = NULL;
    T_TcpClient*         ptNextLink = NULL;

    ptClientLink = ptTcpClient;
    while(ptClientLink)
    {
        ptNextLink = ptClientLink->pmLink;
        if (FD_ISSET(ptClientLink->tClientSocket,ptfdRecv))
        {
            TcpSocketRecv(ptClientLink);
        }
        else
        {
            (*ptClientLink->pfTcpcOnIdle)(ptClientLink);
        }

        ptClientLink = ptNextLink;
    }
}

VOID TcpSocketDealRecv(T_TcpSocketMgr* ptTcpSocketMgr)
{
    fd_set          tfdRecv;
    SOCKET          tMaxSocket = 0;
    INT             iRet = 0;
    struct timeval  tv = {1, 0};

    tMaxSocket = TcpSocketCreateRecvFDSET(ptTcpSocketMgr->pClientLink,&tfdRecv);
    if (0 == tMaxSocket)
    {
        OSS_Delay(500);
        return;
    }

    select(tMaxSocket+1, &tfdRecv, NULL, NULL, &tv);

    TcpSocketRecvISSET(ptTcpSocketMgr->pClientLink,&tfdRecv);

    return;
}

VOID SetTcpClientCfg(T_TcpClient* ptTcpClient, BYTE ucType, VOID* pValue)
{
    if (NULL == ptTcpClient)
    {
        return;
    }
    switch(ucType)
    {
        case eTcpcOnIdle:
            ptTcpClient->pfTcpcOnIdle = (TcpcOnIdle)pValue;
            break;
        case eTcpcOnRecv:
            ptTcpClient->pfTcpcOnRecv = (TcpcOnRecv)pValue;
            break;
        case eTcpcOnClose:
            ptTcpClient->pfTcpcOnClose = (TcpcOnClose)pValue;
            break;
        default:
            break;
    }

    return;
}

INT CreateClientSocket(T_TcpClient* ptTcpClient)
{
    struct sockaddr_in tServerAddr;
    struct sockaddr_in tClientAddr;
    INT                iOpt = 1;

    ptTcpClient->pfTcpcOnRecv = TcpcOnRecvDefault;
    ptTcpClient->pfTcpcOnIdle = TcpcOnIdleDefault;
    ptTcpClient->pfTcpcOnClose = TcpcOnCloseDefault;

    ptTcpClient->tClientSocket = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(ptTcpClient->tClientSocket, SOL_SOCKET, SO_REUSEADDR, (VOID*)&iOpt, sizeof(INT));

    tClientAddr.sin_addr.s_addr = htonl(ptTcpClient->dwClientAddr);
    tClientAddr.sin_port = htons(ptTcpClient->wClientPort);
    tClientAddr.sin_family = AF_INET;

    bind(ptTcpClient->tClientSocket,(struct sockaddr*)&tClientAddr,sizeof(struct sockaddr));

    tServerAddr.sin_port = htons(ptTcpClient->wServerPort);
    tServerAddr.sin_addr.s_addr = htonl(ptTcpClient->dwServerAddr);
    tServerAddr.sin_family = AF_INET;

    return connect(ptTcpClient->tClientSocket,(struct sockaddr*)&tServerAddr,sizeof(struct sockaddr));
}

T_TcpClient *CreateTcpClientWithClientAddr(CHAR *pServerName, WORD32 dwServerAddr, WORD16 wServerPort,WORD32 dwClientAddr, WORD16 wClientPort)
{
    T_TcpClient* ptTcpClient = NULL;
    INT          iRet = 0;

    ptTcpClient = (T_TcpClient*)OSS_GetUB(sizeof(T_TcpClient));
    if (NULL == ptTcpClient)
    {
        return NULL;
    }

    memset(ptTcpClient,0x00,sizeof(T_TcpClient));

    ptTcpClient->dwServerAddr = dwServerAddr;
    ptTcpClient->wServerPort = wServerPort;
    ptTcpClient->dwClientAddr = dwClientAddr;
    ptTcpClient->wClientPort = wClientPort;

    iRet = CreateClientSocket(ptTcpClient);
    if (iRet < 0)
    {
        OSS_RetUB(ptTcpClient);
        return NULL;
    }

    if (!AddTcpClientToMgrByName(pServerName, ptTcpClient))
    {
        OSS_RetUB(ptTcpClient);
        return NULL;
    }

    return ptTcpClient;
}

T_TcpClient *CreateTcpClient(CHAR *pServerName, WORD32 dwServerAddr, WORD16 wServerPort)
{
    return CreateTcpClientWithClientAddr(pServerName,dwServerAddr,wServerPort,0,0);
}

WORD16 TcpSend(T_TcpClient *ptTcpClient, BYTE *pucBuff, WORD16 wSize)
{
    return (WORD16)send(ptTcpClient->tClientSocket,pucBuff,wSize,0);
}

INT* GetClientSocketId(T_TcpClient *ptTcpClient)
{
    if (NULL != ptTcpClient)
    {
        return NULL;
    }

    return (INT*)&ptTcpClient->tClientSocket;
}
