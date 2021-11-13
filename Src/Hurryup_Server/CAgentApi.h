#pragma once
#include "stdafx.h"
#include "CApi.h"

class CAgentApi : public::CApi<std::map<OPCODE, void(CAgentApi::*)(int, std::tstring)>>
{
public:
	void Routing(std::map<OPCODE, void(CAgentApi::*)(int, std::tstring)> & router);
	void ResponseProcessList(int agentFd, std::tstring data);
	void ResponseFileDescriptorList(int agentFd, std::tstring data);
	void ResponseMonitoringResult(int agentFd, std::tstring data);
	void ResponseMonitoringLog(int agentFd, std::tstring data);
	void ResponseDeviceInfo(int agentFd, std::tstring data);
	void ResponseDeviceDead(int agentFd, std::tstring data);
	void ResponseModuleInfo(int agentFd, std::tstring data);
	void ResponsePolicyResult(int agentFd, std::tstring data);
	void ResponseCheckResult(int agentFd, std::tstring data);
	std::string GetTimeStamp();
	int GetDeviceId(int agentFd);
private:

};

#ifdef _DEBUG
inline void TestResponseProcessList()
{
	core::Log_Info(TEXT("CAgentApi.h - [%s]"), TEXT("Test Response Process List"));
	CAgentApi agentApi;

	ST_PROCESS_LIST processList;
	processList.processLists.push_back(ST_PROCESS_INFO(2, 1, "process1", "sleep", "/process1", agentApi.GetTimeStamp().c_str()));
	processList.processLists.push_back(ST_PROCESS_INFO(3, 1, "process2", "zombee", "/process2", agentApi.GetTimeStamp().c_str()));

	std::tstring jsTestData;
	core::WriteJsonToString(&processList, jsTestData);

	agentApi.ResponseProcessList(1, jsTestData);
}

inline void TestResponseFileDescriptorList()
{
	core::Log_Info(TEXT("CAgentApi.h - [%s]"), TEXT("Test Response FileDescriptor List"));
	CAgentApi agentApi;

	ST_FD_LIST fdList;
	fdList.fdLists.push_back(ST_FD_INFO(1, "/proc/process1", "/dev/null"));
	fdList.fdLists.push_back(ST_FD_INFO(1, "/proc/process2", "/dev/1"));

	std::tstring jsTestData;
	core::WriteJsonToString(&fdList, jsTestData);

	agentApi.ResponseFileDescriptorList(1, jsTestData);
}

inline void TestResponseMonitoringResult()
{
	core::Log_Info(TEXT("CAgentApi.h - [%s]"), TEXT("Test Response FileDescriptor List"));
	CAgentApi agentApi;

	ST_MONITOR_RESULT monitorResult("test process", "/logPath/test", true);

	std::tstring jsTestData;
	core::WriteJsonToString(&monitorResult, jsTestData);

	agentApi.ResponseMonitoringResult(1, jsTestData);
}

inline void TestResponseMonitoringLog()
{
	core::Log_Info(TEXT("CAgentApi.h - [%s]"), TEXT("Test Response FileDescriptor List"));
	CAgentApi agentApi;

	ST_MONITOR_INFO monitorInfo("test process", "/logPath/test", "change Content");

	std::tstring jsTestData;
	core::WriteJsonToString(&monitorInfo, jsTestData);

	agentApi.ResponseMonitoringLog(1, jsTestData);
}

inline void TestResponseDeviceInfo()
{
	core::Log_Info(TEXT("CAgentApi.h - [%s]"), TEXT("Test Response FileDescriptor List"));
	CAgentApi agentApi;

	ST_DEVICE_INFO deviceInfo("device", "00-00-00", "11-11-11", "129.123.123.123", "aa:bb:cc:dd:ee:ff", "x86", "win");

	std::tstring jsTestData;
	core::WriteJsonToString(&deviceInfo, jsTestData);

	agentApi.ResponseDeviceInfo(1, jsTestData);
}

inline void TestResponseDeviceDead()
{
	core::Log_Info(TEXT("CAgentApi.h - [%s]"), TEXT("Test Response FileDescriptor List"));
	CAgentApi agentApi;

	agentApi.ResponseDeviceDead(1, "");
}
inline void TestResponseModuleInfo()
{
	core::Log_Info(TEXT("CAgentApi.h - [%s]"), TEXT("Test Response FileDescriptor List"));
	CAgentApi agentApi;

	ST_MODULE_INFO moduleInfo("11-11-11", "aa:bb:cc:dd:ee:ff", "module", "00-11-22", "22-22-22", "aa:bb:cc:dd:ee:ff");

	std::tstring jsTestData;
	core::WriteJsonToString(&moduleInfo, jsTestData);

	agentApi.ResponseModuleInfo(1, jsTestData);
}
inline void TestResponsePolicyResult()
{

}
inline void TestResponseCheckResult()
{
	
}
#endif