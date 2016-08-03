//
// Created by zlb on 16-6-23.
//

#ifndef PROJECT_PRITCPSOCKET_H
#define PROJECT_PRITCPSOCKET_H

#include "pubtcpsocket.h"
#include "pub_oss.h"

#ifdef VOS_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#else
#include <winsock.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#ifdef VOS_UNIX
#define SOCKET INT
#else
#endif

#define TCP_MGR_MAX              (BYTE)16

struct T_TcpServer
{
    BYTE            ucMgrIndex;
    SOCKET          tServerSocket;
    WORD32          dwServerAddr;
    WORD16          wServerPort;
    TcpsOnAccept    pfTcpsOnAccept;
    TcpsOnIdle      pfTcpsOnIdle;
    WORD16          wMaxClientCnt;
    T_TcpClient*    pClientLink;
    T_TcpServer*    psLink;
};

struct T_TcpClient
{
    BYTE            ucMgrIndex;
    T_TcpServer*    ptTcpServer;
    SOCKET          tClientSocket;
    WORD32          dwServerAddr;
    WORD16          wServerPort;
    WORD32          dwClientAddr;
    WORD16          wClientPort;
    TcpcOnRecv      pfTcpcOnRecv;
    TcpcOnIdle      pfTcpcOnIdle;
    TcpcOnClose     pfTcpcOnClose;
    T_TcpClient*    pcLink;
    T_TcpClient*    pmLink;
};

typedef struct T_TcpSocketMgr
{
    BYTE            ucMgrIndex;
    CHAR            acSocketName[64];
    OSS_ULONG       dwTaskId;
    BYTE            ucType;
    T_TcpServer*    pServerLink;
    T_TcpClient*    pClientLink;
}T_TcpSocketMgr;

extern T_TcpSocketMgr  gtTcpSocketMgr[TCP_MGR_MAX];

#define GetTcpSocketMgr(index) (gtTcpSocketMgr+(index))

VOID TcpSocketDealAccept(T_TcpSocketMgr *ptTcpSocketMgr);
VOID TcpSocketDealRecv(T_TcpSocketMgr* ptTcpSocketMgr);
T_TcpSocketMgr* GetTcpSocketMgrByTaskId(OSS_ULONG dwTaskId);
T_TcpSocketMgr* GetTcpSocketMgrByName(CHAR *pName);
BOOL AddTcpClientToMgrByName(CHAR *pName, T_TcpClient *ptTcpClient);
BOOL TcpSocketDelClient(T_TcpClient* ptTcpClient);

WORD16 TcpcOnRecvDefault(T_TcpClient* ptTcpClient,BYTE* ucBuff, WORD16 wLen);
VOID TcpcOnIdleDefault(T_TcpClient* ptTcpClient);
VOID TcpcOnCloseDefault(T_TcpClient* ptTcpClient);

#endif //PROJECT_PRITCPSOCKET_H
