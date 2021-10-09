#pragma once

#include <cstdio>
#include <cppcore.h>
#include <protocol.h>

enum PacketTarget {
    SERVER,
    AGENT
};

enum PacketType {
    RESPONSE,
    RESQUEST
};

enum PacketOpcode {
    TEST,
    TEST1,
    TEST2,
    TEST3,
    TEST4
};

struct ST_PACKET_INFO : public core::IFormatterObject
{
    int source;
    int destination;
    int type;
    int opcode;
    std::tstring data;

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Source"), source)
            + core::sPair(TEXT("Destination"), destination)
            + core::sPair(TEXT("Type"), type)
            + core::sPair(TEXT("Opcode"), opcode)
            + core::sPair(TEXT("data"), data)
            ;
    }
};


struct ST_DATA_DEVICE_INFO : public core::IFormatterObject
{
    std::tstring name;
    std::tstring modelNumber;
    std::tstring serialNumber;
    std::tstring ip;
    std::tstring mac;
    std::tstring architecture;
    std::tstring os;

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Name"), name)
            + core::sPair(TEXT("ModelNumber"), modelNumber)
            + core::sPair(TEXT("SerialNumber"), serialNumber)
            + core::sPair(TEXT("Ip"), ip)
            + core::sPair(TEXT("Mac"), mac)
            + core::sPair(TEXT("Architecture"), architecture)
            + core::sPair(TEXT("Os"), os)
            ;
    }
};
