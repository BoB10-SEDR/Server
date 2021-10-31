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
    PROCESS_LIST,
    FD_LIST,
    MONITOR_ADD,
    MONITOR_REMOVE,
    MONITOR_RESULT,
    MONITOR_INFO,
    DEVICE_INFO,
    MODULE_INFO,
    POLICY_ACTIVATE,
    POLICY_INACTIVATE,
    POLICY_STATE,
    CHECK_ACTIVATE,
    CHECK_STATE,
    MESSAGE
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
    int ppid;
    std::tstring name;
    std::tstring state;
    std::tstring cmdline;
    std::tstring startTime;

    ST_PROCESS_INFO(void)
    {}
    ST_PROCESS_INFO(int _pid, int _ppid, std::tstring _name, std::tstring _state, std::tstring _cmdline, std::tstring _startTime)
        : pid(_pid), ppid(_ppid), name(_name), state(_state), cmdline(_cmdline), startTime(_startTime)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Pid"), pid)
            + core::sPair(TEXT("PPid"), ppid)
            + core::sPair(TEXT("Name"), name)
            + core::sPair(TEXT("State"), state)
            + core::sPair(TEXT("Cmdline"), cmdline)
            + core::sPair(TEXT("StartTime"), startTime)
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
    std::string fdName;
    std::string realPath;

    ST_FD_INFO(void)
    {}
    ST_FD_INFO(int _pid, std::string _fdName, std::string _realPath)
        : pid(_pid), fdName(_fdName), realPath(_realPath)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Pid"), pid)
            + core::sPair(TEXT("FdName"), fdName)
            + core::sPair(TEXT("ReadPath"), realPath)
            ;
    }
};

struct ST_FD_LIST : public core::IFormatterObject
{
    int pid;
    std::vector<ST_FD_INFO> fdLists;

    ST_FD_LIST(void)
    {}
    ST_FD_LIST(int _pid, std::vector<ST_FD_INFO> _fdLists)
        : pid(_pid), fdLists(_fdLists)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Pid"), pid)
            + core::sPair(TEXT("FdLists"), fdLists)
            ;
    }
};

struct ST_MONITOR_TARGET : public core::IFormatterObject
{
    std::string processName;
    std::string logPath;

    ST_MONITOR_TARGET(void)
    {}
    ST_MONITOR_TARGET(std::string _processName, std::string _logPath)
        : processName(_processName), logPath(_logPath)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("ProcessName"), processName)
            + core::sPair(TEXT("LogPath"), logPath)
            ;
    }
};

struct ST_MONITOR_LIST : public core::IFormatterObject
{
    std::vector <ST_MONITOR_TARGET> targetLists;

    ST_MONITOR_LIST(void)
    {}
    ST_MONITOR_LIST(std::vector<ST_MONITOR_TARGET> _targetLists)
        : targetLists(_targetLists)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("TargetLists"), targetLists)
            ;
    }
};

struct ST_MONITOR_RESULT : public core::IFormatterObject
{
    std::tstring logPath;
    bool result;

    ST_MONITOR_RESULT(void)
    {}
    ST_MONITOR_RESULT(std::tstring _logPath, bool _result)
        : logPath(_logPath), result(_result)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("LogPath"), logPath)
            + core::sPair(TEXT("Result"), result)
            ;
    }
};

struct ST_MONITOR_INFO : public core::IFormatterObject
{
    std::string processName;
    std::string logPath;
    std::string changeData;

    ST_MONITOR_INFO(void)
    {}
    ST_MONITOR_INFO(std::string _processName, std::string _logPath, std::string _changeData)
        : processName(_processName), logPath(_logPath), changeData(_changeData)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("ProcessName"), processName)
            + core::sPair(TEXT("LogPath"), logPath)
            + core::sPair(TEXT("ChangeData"), changeData)
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

struct ST_CHECK_INFO : public core::IFormatterObject
{
    int idx;
    std::string name;
    int logID;

    ST_CHECK_INFO(void)
    {}
    ST_CHECK_INFO(int _idx, std::string _name, int _logID)
        : idx(_idx), name(_name), logID(_logID)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Idx"), idx)
            + core::sPair(TEXT("Name"), name)
            + core::sPair(TEXT("LogID"), logID)
            ;
    }
};

struct ST_CHECK_RESULT : public core::IFormatterObject
{
    int logID;
    int step;
    bool result;
    std::string time;


    ST_CHECK_RESULT(void)
    {}
    ST_CHECK_RESULT(int _logID, int _step, bool _result, std::string _time)
        : logID(_logID), step(_step), result(_result), time(_time)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("LogID"), logID)
            + core::sPair(TEXT("Step"), step)
            + core::sPair(TEXT("Result"), result)
            + core::sPair(TEXT("Time"), time)
            ;
    }
};

struct ST_MESSAGE : public core::IFormatterObject
{
    int opcode;
    bool status;
    std::string data;

    ST_MESSAGE(void)
    {}
    ST_MESSAGE(int _opcode, bool _status, std::string _data)
        : opcode(_opcode), status(_status), data(_data)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("Opcode"), opcode)
            + core::sPair(TEXT("Status"), status)
            + core::sPair(TEXT("Data"), data)
            ;
    }
};
void test();