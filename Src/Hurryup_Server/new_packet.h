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

template <typename ST_INFO>
struct ST_NEW_INFO : public core::IFormatterObject
{
    std::tstring serialNumber;
    std::tstring timestamp;
    ST_INFO metaInfo;

    ST_NEW_INFO(void)
    {}
    ST_NEW_INFO(std::tstring _serialNumber, std::tstring _timestamp, ST_INFO _metaInfo)
        : serialNumber(_serialNumber), timestamp(_timestamp), metaInfo(_metaInfo)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("SerialNumber"), serialNumber)
            + core::sPair(TEXT("TimeStamp"), timestamp)
            + core::sPair(TEXT("MetaInfo"), metaInfo)
            ;
    }
};

template <typename ST_INFO>
struct ST_NEW_VECTOR_DATABASE : public core::IFormatterObject
{
	std::tstring key;
	std::vector<ST_INFO> metaInfo;
	
	ST_NEW_VECTOR_DATABASE(void)
	{}
	ST_NEW_VECTOR_DATABASE(std::tstring _key, std::vector<ST_INFO> _metaInfo)
		: key(_key), metaInfo(_metaInfo)
	{}

	void OnSync(core::IFormatter& formatter)
	{
		formatter
			+ core::sPair(TEXT(key.c_str()), metaInfo)
			;
	}
};

struct ST_NEW_NETWORK_INTERFACE_INFO : public core::IFormatterObject
{
	std::tstring if_name;
	std::tstring m_addr;
	std::tstring inet_addr;
	std::tstring inet6_addr;

	ST_NEW_NETWORK_INTERFACE_INFO(void)
	{}
	ST_NEW_NETWORK_INTERFACE_INFO(std::tstring _if_name, std::tstring _m_addr, std::tstring _inet_addr, std::tstring _inet6_addr)
		: if_name(_if_name), m_addr(_m_addr), inet_addr(_inet_addr), inet6_addr(_inet6_addr)
	{}

	bool operator== (const ST_NEW_NETWORK_INTERFACE_INFO& t)
	{
		return this->if_name == t.if_name &&
			this->m_addr == t.m_addr &&
			this->inet_addr == t.inet_addr &&
			this->inet6_addr == t.inet6_addr;
	}

	void OnSync(core::IFormatter& formatter)
	{
		formatter
			+ core::sPair(TEXT("If_name"), if_name)
			+ core::sPair(TEXT("Mac_addr"), m_addr)
			+ core::sPair(TEXT("Inet_addr"), inet_addr)
			+ core::sPair(TEXT("Inet6_addr"), inet6_addr)
			;
	}
};

struct ST_NEW_OS_INFO : public core::IFormatterObject
{
	std::tstring osName;
	std::tstring osRelease;

	ST_NEW_OS_INFO(void)
	{}
	ST_NEW_OS_INFO(std::tstring _osName, std::tstring _osRelease)
		: osName(_osName), osRelease(_osRelease)
	{}

	ST_NEW_OS_INFO* operator= (const ST_NEW_OS_INFO* t)
	{
		this->osName = t->osName;
		this->osRelease = t->osRelease;
	}

	bool operator== (const ST_NEW_OS_INFO& t)
	{
		return this->osName == t.osName &&
			this->osRelease == t.osRelease;
	}

	void OnSync(core::IFormatter& formatter)
	{
		formatter
			+ core::sPair(TEXT("OsName"), osName)
			+ core::sPair(TEXT("OsRelease"), osRelease)
			;
	}
};

struct ST_NEW_SERVICE_INFO : public core::IFormatterObject
{
	std::tstring serviceName;
	bool isActive;

	ST_NEW_SERVICE_INFO(void)
	{}
	ST_NEW_SERVICE_INFO(std::tstring _serviceName, bool _isActive)
		: serviceName(_serviceName), isActive(_isActive)
	{}

	bool operator== (const ST_NEW_SERVICE_INFO& t)
	{
		return this->serviceName == t.serviceName &&
			this->isActive == t.isActive;
	}

	void OnSync(core::IFormatter& formatter)
	{
		formatter
			+ core::sPair(TEXT("ServiceName"), serviceName)
			+ core::sPair(TEXT("IsActive"), isActive)
			;
	}
};

struct ST_NEW_DEVICE_INFO : public core::IFormatterObject
{
	std::tstring name;
	ST_NEW_OS_INFO osInfo;
	std::tstring modelNumber;
	std::tstring connectMethod; // 구분자 ','
	std::vector<ST_NEW_NETWORK_INTERFACE_INFO> networkInfo;

	uint32_t moduleCount = 0;

	std::vector<ST_NEW_SERVICE_INFO> serviceList;

	// template property; => TODO :: 템플릿화 구현 필요 (전상현 멘토님 11월 1주차 멘토링 참고)

	ST_NEW_DEVICE_INFO(void)
	{}
	ST_NEW_DEVICE_INFO(std::tstring _name, ST_NEW_OS_INFO _osInfo, std::tstring _modelNumber, std::tstring _connectMethod, std::vector<ST_NEW_NETWORK_INTERFACE_INFO> _networkInfo, uint32_t _moduleCount, std::vector<ST_NEW_SERVICE_INFO> _serviceList)
		: name(_name), osInfo(_osInfo), modelNumber(_modelNumber), connectMethod(_connectMethod), networkInfo(_networkInfo), moduleCount(_moduleCount), serviceList(_serviceList)
	{}

	ST_NEW_DEVICE_INFO & operator= (const ST_NEW_DEVICE_INFO& t)
	{
		this->name = t.name;
		this->osInfo = t.osInfo;
		this->modelNumber = t.modelNumber;
		this->connectMethod = t.connectMethod;
		this->moduleCount = t.moduleCount;

		this->networkInfo.clear();
		for (auto it : t.networkInfo)
			this->networkInfo.push_back(it);

		this->serviceList.clear();
		for (auto it : t.serviceList)
			this->serviceList.push_back(it);

		return *this;
	}

	bool operator== (const ST_NEW_DEVICE_INFO & t)
	{
		return this->name == t.name &&
			this->osInfo == t.osInfo &&
			this->modelNumber == t.modelNumber &&
			this->connectMethod == t.connectMethod &&
			this->moduleCount == t.moduleCount;
	}

	void OnSync(core::IFormatter& formatter)
	{
		formatter
			+ core::sPair(TEXT("Name"), name)
			+ core::sPair(TEXT("OsInfo"), osInfo)
			+ core::sPair(TEXT("ModelNumber"), modelNumber)
			+ core::sPair(TEXT("ConnectMethod"), connectMethod)
			+ core::sPair(TEXT("NetworkInfo"), networkInfo)
			+ core::sPair(TEXT("ModuleCount"), moduleCount)
			+ core::sPair(TEXT("ServiceList"), serviceList)
			;
	}
};

struct ST_NEW_FILE_INFO : public core::IFormatterObject
{
	std::tstring buffer;
	int length;

	ST_NEW_FILE_INFO(void)
	{}
	ST_NEW_FILE_INFO(std::tstring _buffer, int _length)
		: buffer(_buffer), length(_length)
	{}

	void OnSync(core::IFormatter& formatter)
	{
		formatter
			+ core::sPair(TEXT("buffer"), buffer)
			+ core::sPair(TEXT("length"), length)
			;
	}
};

struct ST_NEW_POLICY_INFO : public core::IFormatterObject
{
	std::tstring timestamp;
	std::tstring policyName;
	std::tstring policyID;
	std::tstring policyDescription;
	bool isFile;
	ST_NEW_FILE_INFO fileData;

	ST_NEW_POLICY_INFO(void)
	{}
	ST_NEW_POLICY_INFO(std::tstring _timestamp, std::tstring _policyName, std::tstring _policyID, std::tstring _policyDescription)
		: timestamp(_timestamp), policyName(_policyName), policyID(_policyID), policyDescription(_policyDescription)
	{}

	ST_NEW_POLICY_INFO& operator= (const ST_NEW_POLICY_INFO& t)
	{
		this->policyName = t.policyName;
		this->policyID = t.policyID;
		this->policyDescription = t.policyDescription;
		this->isFile = t.isFile;

		return *this;
	}

	void OnSync(core::IFormatter& formatter)
	{
		formatter
			+ core::sPair(TEXT("Timestamp"), timestamp)
			+ core::sPair(TEXT("PolicyName"), policyName)
			+ core::sPair(TEXT("PolicyID"), policyID)
			+ core::sPair(TEXT("PolicyDescription"), policyDescription)
			+ core::sPair(TEXT("IsFile"), isFile)
			+ core::sPair(TEXT("FileData"), fileData)
			;
	}
};

struct ST_NEW_PROCESS_INFO : public core::IFormatterObject
{
	int pid;
	int ppid;
	std::tstring name;
	std::tstring state;
	std::tstring cmdline;
	std::tstring startTime;

	ST_NEW_PROCESS_INFO(void)
	{}
	ST_NEW_PROCESS_INFO(int _pid, int _ppid, std::tstring _name, std::tstring _state, std::tstring _cmdline, std::tstring _startTime)
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

struct ST_NEW_FD_INFO : public core::IFormatterObject
{
	int pid;
	std::string fdName;
	std::string realPath;

	ST_NEW_FD_INFO(void)
	{}
	ST_NEW_FD_INFO(int _pid, std::string _fdName, std::string _realPath)
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

struct ST_NEW_MONITOR_TARGET : public core::IFormatterObject
{
	std::string processName;
	std::string logPath;

	ST_NEW_MONITOR_TARGET(void)
	{}
	ST_NEW_MONITOR_TARGET(std::string _processName, std::string _logPath)
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

struct ST_NEW_MONITOR_RESULT : public core::IFormatterObject
{
	std::string processName;
	std::tstring logPath;
	bool result;

	ST_NEW_MONITOR_RESULT(void)
	{}
	ST_NEW_MONITOR_RESULT(std::string _processName, std::tstring _logPath, bool _result)
		: processName(_processName), logPath(_logPath), result(_result)
	{}

	void OnSync(core::IFormatter& formatter)
	{
		formatter
			+ core::sPair(TEXT("ProcessName"), processName)
			+ core::sPair(TEXT("LogPath"), logPath)
			+ core::sPair(TEXT("Result"), result)
			;
	}
};

struct ST_NEW_MONITOR_INFO : public core::IFormatterObject
{
	std::string processName;
	std::string logPath;
	std::string changeData;

	ST_NEW_MONITOR_INFO(void)
	{}
	ST_NEW_MONITOR_INFO(std::string _processName, std::string _logPath, std::string _changeData)
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