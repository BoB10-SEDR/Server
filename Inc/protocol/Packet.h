#pragma once
#include <stdio.h>
#include <cppcore.h>

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

    ST_PACKET_INFO(void)
    {}
    ST_PACKET_INFO(int _source, int _destination, int _type, int _opcode, std::string _data)
        : source(_source), destination(_destination), type(_type), opcode(_opcode), data(_data)
    {}

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

void test();