#include "Function.h"
#include "CMessage.h"

void func::GetProcessList(std::string agentInfo)
{
	LoggerManager()->Info("Request GetProcessList");
	MessageManager()->PushSendMessage(agentInfo, REQUEST, PROCESS_LIST, "");
}

void func::SaveProcessList(std::string agentInfo, std::string data)
{
	ST_PROCESS_LIST* processList = new ST_PROCESS_LIST();
	core::ReadJsonFromString(processList, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo.c_str(), data.c_str()));
}

void func::GetFileDescriptorList(std::string agentInfo)
{
	LoggerManager()->Info("Request GetFileDescriptorList");
	MessageManager()->PushSendMessage(agentInfo, REQUEST, FD_LIST, "");
}

void func::SaveFileDescriptorList(std::string agentInfo, std::string data)
{
	ST_FD_LIST* fdLIST = new ST_FD_LIST();
	core::ReadJsonFromString(fdLIST, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo.c_str(), data.c_str()));
}

void func::StartMonitoring(std::string agentInfo, std::vector<std::string> logLists)
{
	LoggerManager()->Info("Request StartMonitoring");
	ST_MONITOR_LIST* monitorList = new ST_MONITOR_LIST(logLists);
	std::tstring jsMonitorList;
	core::WriteJsonToString(monitorList, jsMonitorList);

	MessageManager()->PushSendMessage(agentInfo, REQUEST, MONITOR_ACTIVATE, jsMonitorList);
}

void func::StopMonitoring(std::string agentInfo, std::vector<std::string> logLists)
{
	LoggerManager()->Info("Request StopMonitoring");
	ST_MONITOR_LIST* monitorList = new ST_MONITOR_LIST(logLists);
	std::tstring jsMonitorList;
	core::WriteJsonToString(monitorList, jsMonitorList);

	MessageManager()->PushSendMessage(agentInfo, REQUEST, MONITOR_INACTIVATE, jsMonitorList);
}

void func::SaveMonitoringInfo(std::string agentInfo, std::string data)
{
	ST_MONITOR_INFO* monitorInfo = new ST_MONITOR_INFO();
	core::ReadJsonFromString(monitorInfo, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo.c_str(), data.c_str()));
}

void func::GetDeviceInfo(std::string agentInfo)
{
	LoggerManager()->Info("Request GetDeviceInfo");
	MessageManager()->PushSendMessage(agentInfo, REQUEST, DEVICE_INFO, "");
}

void func::SaveDeviceInfo(std::string agentInfo, std::string data)
{
	ST_DEVICE_INFO* deviceInfo = new ST_DEVICE_INFO();
	core::ReadJsonFromString(deviceInfo, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo.c_str(), data.c_str()));
}

void func::GetModuleInfo(std::string agentInfo)
{
	LoggerManager()->Info("Request GetModuleInfo");
	MessageManager()->PushSendMessage(agentInfo, REQUEST, MODULE_INFO, "");
}

void func::SaveModuleInfo(std::string agentInfo, std::string data)
{
	ST_MODULE_INFO* moduleInfo = new ST_MODULE_INFO();
	core::ReadJsonFromString(moduleInfo, data);

	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo.c_str(), data.c_str()));
}

void func::ActivatePolicy(std::string agentInfo, int idx, std::string name, std::string version)
{
	LoggerManager()->Info("Request ActivatePolicy");
	ST_POLICY_INFO* policyInfo = new ST_POLICY_INFO(idx, name, version);
	std::tstring jsPolicyInfo;
	core::WriteJsonToString(policyInfo, jsPolicyInfo);
	MessageManager()->PushSendMessage(agentInfo, REQUEST, POLICY_ACTIVATE, jsPolicyInfo);
}

void func::InactivatePolicy(std::string agentInfo, int idx, std::string name, std::string version)
{
	LoggerManager()->Info("Request InactivatePolicy");
	ST_POLICY_INFO* policyInfo = new ST_POLICY_INFO(idx, name, version);
	std::tstring jsPolicyInfo;
	core::WriteJsonToString(policyInfo, jsPolicyInfo);
	MessageManager()->PushSendMessage(agentInfo, REQUEST, POLICY_INACTIVATE, jsPolicyInfo);
}

void func::SavePolicyStatus(std::string agentInfo, std::string data)
{
	ST_POLICY_RESULT* policyState = new ST_POLICY_RESULT();
	core::ReadJsonFromString(policyState, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo.c_str(), data.c_str()));
}

void func::ActivateCheck(std::string agentInfo, int idx, std::string name)
{
	LoggerManager()->Info("Request ActivateCheck");
	int logID;	//Create Database Data Result idx
	ST_CHECK_INFO* checkInfo = new ST_CHECK_INFO(idx, name, logID);

	std::tstring jsCheckInfo;
	core::WriteJsonToString(checkInfo, jsCheckInfo);
	MessageManager()->PushSendMessage(agentInfo, REQUEST, CHECK_ACTIVATE, jsCheckInfo);
}

void func::SaveCheckStatus(std::string agentInfo, std::string data)
{
	ST_CHECK_RESULT* checkResult = new ST_CHECK_RESULT();
	core::ReadJsonFromString(checkResult, data);

	//Save DataBase
	LoggerManager()->Info(StringFormatter("Save DataBase [%s] : %s", agentInfo.c_str(), data.c_str()));
}

void func::SaveMessage(std::string agentInfo, std::string data)
{
	ST_MESSAGE* message = new ST_MESSAGE();
	core::ReadJsonFromString(message, data);

	if (!message->status) {
		LoggerManager()->Warn(StringFormatter("Error Command [%s] : %s", agentInfo.c_str(), message->data.c_str()));
	}
	else {
		LoggerManager()->Info(StringFormatter("Success Command [%s] : %s", agentInfo.c_str(), message->data.c_str()));
	}
}