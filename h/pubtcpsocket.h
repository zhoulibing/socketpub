//
// Created by zlb on 16-6-22.
//

#ifndef PROJECT_TCPSOCKET_H
#define PROJECT_TCPSOCKET_H

#include "pub_typedef.h"

typedef struct T_TcpServer T_TcpServer;
typedef struct T_TcpClient T_TcpClient;

typedef VOID   (*TcpsOnAccept)(T_TcpClient* ptTcpClient, WORD32 dwClientAddr, WORD16 wClientPort);
typedef VOID   (*TcpsOnIdle)(T_TcpServer* ptTcpServer);
typedef VOID   (*TcpcOnClose)(T_TcpClient* ptTcpClient);
typedef VOID   (*TcpcOnIdle)(T_TcpClient* ptTcpClient);
typedef WORD16 (*TcpcOnRecv)(T_TcpClient* ptTcpClient,BYTE* ucBuff, WORD16 wLen);


typedef enum
{
    eTcpsOnIdle,
    eTcpsOnAccept,
    eTcpsAcceptTime,
    eTcpsMaxClientCnt,
}E_TcpServerCfg;

typedef enum
{
    eTcpcOnIdle,
    eTcpcOnRecv,
    eTcpcOnClose,
    eTcpcRecvTime,
}E_TcpClientCfg;

BOOL InitTcpSocket(CHAR *pSocketName);

T_TcpServer* CreateTcpServer(CHAR *pServerName, WORD32 dwServerAddr, WORD16 wServerPort);

T_TcpClient *CreateTcpClient(CHAR *pServerName, WORD32 dwServerAddr, WORD16 wServerPort);

T_TcpClient *CreateTcpClientWithClientAddr(CHAR *pServerName, WORD32 dwServerAddr, WORD16 wServerPort,WORD32 dwClientAddr, WORD16 wClientPort);

WORD16 TcpSend(T_TcpClient *ptTcpClient, BYTE *pucBuff, WORD16 wSize);

INT* GetClientSocketId(T_TcpClient *ptTcpClient);

VOID SetTcpServerCfg(T_TcpServer *ptTcpServer, BYTE ucType, VOID *pValue);

VOID SetTcpClientCfg(T_TcpClient* ptTcpClient, BYTE ucType, VOID* pValue);

BOOL TcpServerCloseClients(T_TcpServer* ptTcpServer);

BOOL TcpServerCloseClient(T_TcpClient* ptTcpClient);

#endif //PROJECT_TCPSOCKET_H
