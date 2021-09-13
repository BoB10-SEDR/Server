#pragma once

#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <cppcore.h>

class CTestConnection : public core::CSyncConnection
{
    core::CSyncTCPSocket m_Socket;
public:
    CTestConnection(void)
        : core::CSyncConnection(&m_Socket)
    {}

    void	OnConnect(void)
    {
        printf("connection established.\n");
    }
    void	OnClose(void)
    {
        printf("connection disconnected.\n");
    }
    void	OnRecv(void)
    {
        char szBuffer[100];

        size_t tRead = 0;
        m_Socket.Recv(szBuffer, 100, 5000, &tRead);
        szBuffer[tRead] = 0;
        printf("data recved [%s]\n", szBuffer);
    }
};