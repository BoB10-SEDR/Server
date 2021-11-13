#pragma once
#include <cppcore.h>

enum OPCODE {
    PROCESS_LIST,
    FD_LIST,
    MONITORING_ACTIVATE,
    MONITORING_INACTIVATE,
    DEVICE,
    MODULE,
    POLICY_ACTIVATE,
    POLICY_INACTIVATE,
    INSPECTION_ACTIVATE,
    MONITORING_RESULT,
    MONITORING_LOG,
    POLICY_RESULT,
    INSPECTION_RESULT,
    DEVICE_DEAD
};

struct ST_MESSAGE_INFO
{
    int agentSocket;
    OPCODE opcode;
    std::tstring metaInfo;

    ST_MESSAGE_INFO() {}
    ST_MESSAGE_INFO(int _agentSocket, OPCODE _opcode, std::tstring _metaInfo) :agentSocket(_agentSocket), opcode(_opcode), metaInfo(_metaInfo) {}
};

struct ST_NEW_PACKET_INFO : public core::IFormatterObject
{
    int opcode;
    std::tstring data;

    ST_NEW_PACKET_INFO(void)
    {}
    ST_NEW_PACKET_INFO(int _opcode, std::tstring _data)
        : opcode(_opcode), data(_data)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Opcode"), opcode)
            + core::sPair(TEXT("Data"), data)
            ;
    }
};

template <typename METAINFO>
struct ST_NEW_INFO : public core::IFormatterObject
{
    std::tstring serialNumber;
    std::tstring timestamp;
    METAINFO data;

    ST_NEW_INFO(void)
    {}
    ST_NEW_INFO(std::tstring _serialNumber, std::tstring _timestamp, METAINFO _data)
        : serialNumber(_serialNumber), timestamp(_timestamp), data(_data)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("SerialNumber"), serialNumber)
            + core::sPair(TEXT("TimeStamp"), timestamp)
            + core::sPair(TEXT("Data"), data)
            ;
    }
};

struct ST_NEW_NETWORK_INFO : public core::IFormatterObject
{
    std::tstring if_name;
    std::tstring m_addr;
    std::tstring inet_addr;
    std::tstring inet6_addr;

    ST_NEW_NETWORK_INFO(void)
    {}
    ST_NEW_NETWORK_INFO(std::tstring _if_name, std::tstring _m_addr, std::tstring _inet_addr, std::tstring _inet6_addr)
        : if_name(_if_name), m_addr(_m_addr), inet_addr(_inet_addr), inet6_addr(_inet6_addr)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Interface_Name"), if_name)
            + core::sPair(TEXT("Mac_Address"), m_addr)
            + core::sPair(TEXT("Inet_Address"), inet_addr)
            + core::sPair(TEXT("Inet6_Address"), inet6_addr)
            ;
    }
};

struct ST_NEW_DEVICE_INFO : public core::IFormatterObject
{
    std::tstring name;
    std::tstring modelNumber;
    std::tstring connectMethod;
    std::vector<ST_NEW_NETWORK_INFO> networkInfo;

    ST_NEW_DEVICE_INFO(void)
    {}
    ST_NEW_DEVICE_INFO(std::tstring _name, std::tstring _modelNumber, std::tstring _connectMethod, std::vector<ST_NEW_NETWORK_INFO> _networkInfo)
        : name(_name), modelNumber(_modelNumber), connectMethod(_connectMethod), networkInfo(_networkInfo)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Name"), name)
            + core::sPair(TEXT("ModelNumber"), modelNumber)
            + core::sPair(TEXT("ConnectMethod"), connectMethod)
            + core::sPair(TEXT("NetworkInfo"), networkInfo)
            ;
    }
};

