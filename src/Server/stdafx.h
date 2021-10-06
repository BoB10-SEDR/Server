#pragma once

#include <cstdio>
#include <cppcore.h>
#include <protocol.h>

struct ST_NETWORK_INFO : public core::IFormatterObject
{
    std::tstring strIP;
    std::tstring strMac;
    WORD wPort;

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("IP"), strIP)
            + core::sPair(TEXT("Mac"), strMac)
            + core::sPair(TEXT("Port"), wPort)
            ;
    }
};