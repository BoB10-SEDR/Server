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
    REQUEST
};

enum PacketOpcode {
    OPCODE1,
    OPCODE2,
    OPCODE3,
    OPCODE4,
    OPCODE5,
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
