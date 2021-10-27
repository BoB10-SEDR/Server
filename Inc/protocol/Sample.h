#pragma once
#include <stdio.h>
#include <cppcore.h>

struct ST_SAMPLE_INFO : public core::IFormatterObject
{
    std::tstring name;
    int value;

    ST_SAMPLE_INFO(void)
    {}
    ST_SAMPLE_INFO(std::string _name, int _value)
        : name(_name), value(_value)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Name"), name)
            + core::sPair(TEXT("Value"), value)
            ;
    }
};
