#include "stdafx.h"

int _tmain(int nArgc, LPTSTR pszArgv[])
{
    core::CSyncServer server;

    core::ST_SYNCSERVER_INIT stInit;
    stInit.wPort = 15000;
    stInit.Connections.push_back(new CTestConnection());
    stInit.Connections.push_back(new CTestConnection());
    server.StartUp(stInit);

    Sleep(100000);

    server.ShutDown();
    return 0;
}