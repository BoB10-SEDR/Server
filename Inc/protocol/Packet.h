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
    PROCESS_LIST,
    FD_LIST,
    MONITOR_ACTIVATE,
    MONITOR_INACTIVATE,
    MONITOR_INFO,
    DEVICE_INFO,
    MODULE_INFO,
    POLICY_ACTIVATE,
    POLICY_INACTIVATE,
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

struct ST_PROCESS_INFO : public core::IFormatterObject
{
    int pid;
    std::string command;

    ST_PROCESS_INFO(void)
    {}
    ST_PROCESS_INFO(int _pid, std::string _command)
        : pid(_pid), command(_command)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Pid"), pid)
            + core::sPair(TEXT("Command"), command)
            ;
    }
};

struct ST_PROCESS_LIST : public core::IFormatterObject
{
    std::vector<ST_PROCESS_INFO> processLists;

    ST_PROCESS_LIST(void)
    {}
    ST_PROCESS_LIST(std::vector<ST_PROCESS_INFO> _processLists)
        : processLists(_processLists)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("ProcessLists"), processLists)
            ;
    }
};

struct ST_FD_INFO : public core::IFormatterObject
{
    int pid;
    std::string command;
    std::string path;

    ST_FD_INFO(void)
    {}
    ST_FD_INFO(int _pid, std::string _command, std::string _path)
        : pid(_pid), command(_command), path(_path)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Pid"), pid)
            + core::sPair(TEXT("Command"), command)
            + core::sPair(TEXT("Path"), path)
            ;
    }
};

struct ST_FD_LIST : public core::IFormatterObject
{
    std::vector<ST_FD_INFO> fdLists;

    ST_FD_LIST(void)
    {}
    ST_FD_LIST(std::vector<ST_FD_INFO> _fdLists)
        : fdLists(_fdLists)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("FdLists"), fdLists)
            ;
    }
};

struct ST_MONITOR_LIST: public core::IFormatterObject
{
    std::vector <std::string> pathLists;

    ST_MONITOR_LIST(void)
    {}
    ST_MONITOR_LIST(std::vector<std::string> _pathLists)
        : pathLists(_pathLists)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("PathLists"), pathLists)
            ;
    }
};

struct ST_MONITOR_INFO : public core::IFormatterObject
{
    std::string path;
    std::string data;

    ST_MONITOR_INFO(void)
    {}
    ST_MONITOR_INFO(std::string _path, std::string _data)
        : path(_path), data(_data)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Path"), path)
            + core::sPair(TEXT("Data"), data)
            ;
    }
};

struct ST_DEVICE_INFO : public core::IFormatterObject
{
    std::string name;
    std::string modelNumber;
    std::string serialNumber;
    std::string ip;
    std::string mac;
    std::string architecture;
    std::string os;

    ST_DEVICE_INFO(void)
    {}
    ST_DEVICE_INFO(std::string _name, std::string _modelNumber, std::string _serialNumber, std::string _ip, std::string _mac, std::string _architecture, std::string _os)
        : name(_name), modelNumber(_modelNumber), serialNumber(_serialNumber), ip(_ip), mac(_mac), architecture(_architecture), os(_os)
    {}

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

struct ST_MODULE_INFO : public core::IFormatterObject
{    
    std::string deviceSerialNumber;
    std::string deviceMac;
    std::string name;
    std::string modelNumber;
    std::string serialNumber;
    std::string mac;

    ST_MODULE_INFO(void)
    {}
    ST_MODULE_INFO(std::string _deviceSerialNumber, std::string _deviceMac, std::string _name, std::string _modelNumber, std::string _serialNumber, std::string _mac)
        : deviceSerialNumber(_deviceSerialNumber), deviceMac(_deviceMac), name(_name), modelNumber(_modelNumber), serialNumber(_serialNumber), mac(_mac)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("DeviceSerialNumber"), deviceSerialNumber)
            + core::sPair(TEXT("DeviceMac"), deviceMac)
            + core::sPair(TEXT("Name"), name)
            + core::sPair(TEXT("ModelNumber"), modelNumber)
            + core::sPair(TEXT("SerialNumber"), serialNumber)
            + core::sPair(TEXT("Mac"), mac)
            ;
    }
};

struct ST_POLICY_INFO : public core::IFormatterObject
{
    int idx;
    std::string name;
    std::string version;

    ST_POLICY_INFO(void)
    {}
    ST_POLICY_INFO(int _idx, std::string _name, std::string _version)
        : idx(_idx), name(_name), version(_version)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Idx"), idx)
            + core::sPair(TEXT("Name"), name)
            + core::sPair(TEXT("Version"), version)
            ;
    }
};

struct ST_POLICY_RESULT : public core::IFormatterObject
{
    int idx;
    bool result;
    std::string time;


    ST_POLICY_RESULT(void)
    {}
    ST_POLICY_RESULT(int _idx, bool _result, std::string _time)
        : idx(_idx), result(_result), time(_time)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Idx"), idx)
            + core::sPair(TEXT("Result"), result)
            + core::sPair(TEXT("Time"), time)
            ;
    }
};

struct ST_POLICY_STATE : public core::IFormatterObject
{
    std::vector<ST_POLICY_RESULT> stateLists;

    ST_POLICY_STATE(void)
    {}
    ST_POLICY_STATE(std::vector<ST_POLICY_RESULT> _stateLists)
        : stateLists(_stateLists)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("StateLists"), stateLists)
            ;
    }
};

struct ST_MESSAGE : public core::IFormatterObject
{
    bool status;
    std::string msg;

    ST_MESSAGE(void)
    {}
    ST_MESSAGE(int _status, std::string _msg)
        : status(_status), msg(_msg)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Status"), status)
            + core::sPair(TEXT("Msg"), msg)
            ;
    }
};
void test();