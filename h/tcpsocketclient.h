//
// Created by zlb on 16-6-24.
//

#ifndef PROJECT_TCPSOCKETCLIENT_H
#define PROJECT_TCPSOCKETCLIENT_H

#include "pritcpsocket.h"

typedef T_TcpClient*  (*NextClient)(T_TcpClient* ptPreClient,T_TcpClient* ptTcpClient);

VOID TcpSockeDelClientFromClientLink(T_TcpClient** pcLink,T_TcpClient* ptTcpClient,NextClient pfNext);

#endif //PROJECT_TCPSOCKETCLIENT_H
