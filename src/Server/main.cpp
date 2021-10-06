#include "stdafx.h"

int main()
{
    ST_NETWORK_INFO stNetworkInfo;
    stNetworkInfo.strIP = TEXT("1.2.3.4");
    stNetworkInfo.strMac = TEXT("00:01:02:03:04:05");
    stNetworkInfo.wPort = 1234;
    test();
    std::tstring strContext;
    core::WriteJsonToString(&stNetworkInfo, strContext);
    tprintf("%s", strContext.c_str());
    return 0;
}