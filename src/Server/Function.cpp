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

void func::GetDeviceInfo()
{

}

void func::SaveDeviceInfo()
{

}

void func::GetModuleInfo()
{

}

void func::SaveModuleInfo()
{

}

void func::ActivatePolicy()
{

}

void func::InactivatePolicy()
{

}