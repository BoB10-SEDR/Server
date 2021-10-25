#include "Function.h"
#include "CMessage.h"

void func::GetProcessList(std::string agentInfo)
{
	MessageManager()->PushSendMessage(agentInfo, REQUEST, PROCESS_LIST, "");
}

void func::SaveProcessList(std::string agentInfo, std::string data)
{
	ST_PROCESS_LIST* processList = new ST_PROCESS_LIST();
	core::ReadJsonFromString(processList, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo, data));
}

void func::GetFileDescriptorList(std::string agentInfo)
{
	MessageManager()->PushSendMessage(agentInfo, REQUEST, FD_LIST, "");
}

void func::SaveFileDescriptorList(std::string agentInfo, std::string data)
{
	ST_FD_LIST* fdLIST = new ST_FD_LIST();
	core::ReadJsonFromString(fdLIST, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo, data));
}

void func::StartMonitoring(std::string agentInfo, std::vector<std::string> logLists)
{
	ST_MONITOR_LIST* monitorList = new ST_MONITOR_LIST(logLists);
	std::tstring jsMonitorList;
	core::WriteJsonToString(monitorList, jsMonitorList);

	//Save DataBase
	MessageManager()->PushSendMessage(agentInfo, REQUEST, MONITOR_ACTIVATE, jsMonitorList);
}

void func::StopMonitoring(std::string agentInfo, std::vector<std::string> logLists)
{
	ST_MONITOR_LIST* monitorList = new ST_MONITOR_LIST(logLists);
	std::tstring jsMonitorList;
	core::WriteJsonToString(monitorList, jsMonitorList);

	//Save DataBase
	MessageManager()->PushSendMessage(agentInfo, REQUEST, MONITOR_INACTIVATE, jsMonitorList);
}

void func::SaveMonitoringInfo(std::string agentInfo, std::string data)
{
	ST_MONITOR_INFO* monitorInfo = new ST_MONITOR_INFO();
	core::ReadJsonFromString(monitorInfo, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo, data));
}

void func::GetDeviceInfo(std::string agentInfo)
{
	MessageManager()->PushSendMessage(agentInfo, REQUEST, DEVICE_INFO, "");
}

void func::SaveDeviceInfo(std::string agentInfo, std::string data)
{
	ST_DEVICE_INFO* deviceInfo = new ST_DEVICE_INFO();
	core::ReadJsonFromString(deviceInfo, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo, data));
}

void func::GetModuleInfo(std::string agentInfo)
{
	MessageManager()->PushSendMessage(agentInfo, REQUEST, MODULE_INFO, "");
}

void func::SaveModuleInfo(std::string agentInfo, std::string data)
{
	ST_MODULE_INFO* moduleInfo = new ST_MODULE_INFO();
	core::ReadJsonFromString(moduleInfo, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo, data));
}

void func::ActivatePolicy(std::string agentInfo, int idx, std::string name, std::string version)
{
	ST_POLICY_INFO* policyInfo = new ST_POLICY_INFO(idx, name, version);
	std::tstring jsPolicyInfo;
	core::WriteJsonToString(policyInfo, jsPolicyInfo);
	MessageManager()->PushSendMessage(agentInfo, REQUEST, POLICY_ACTIVATE, jsPolicyInfo);
}

void func::InactivatePolicy(std::string agentInfo, int idx, std::string name, std::string version)
{
	ST_POLICY_INFO* policyInfo = new ST_POLICY_INFO(idx, name, version);
	std::tstring jsPolicyInfo;
	core::WriteJsonToString(policyInfo, jsPolicyInfo);
	MessageManager()->PushSendMessage(agentInfo, REQUEST, POLICY_INACTIVATE, jsPolicyInfo);
}

void func::SavePolicyStatus(std::string agentInfo, std::string data)
{
	ST_POLICY_STATE* policyState = new ST_POLICY_STATE();
	core::ReadJsonFromString(policyState, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo, data));
}