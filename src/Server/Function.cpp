#include "Function.h"
#include "CMessage.h"

void func::GetProcessList(std::string agentInfo)
{
	//데이터베이스에서 에이전트 고유 정보 조회

	MessageManager()->PushSendMessage(agentInfo, REQUEST, PROCESS_LIST, "");
}

void func::SaveProcessList(std::string data)
{
	ST_PROCESS_LIST* processList = new ST_PROCESS_LIST();
	core::ReadJsonFromString(processList, data);

	LoggerManager()->Info(StringFormatter("Save DataBase : %s", data));
}

void func::GetFileDescriptorList(std::string agentInfo)
{
	MessageManager()->PushSendMessage(agentInfo, REQUEST, FD_LIST, "");
}

void func::SaveFileDescriptorList(std::string data)
{
	ST_FD_LIST* fdLIST = new ST_FD_LIST();
	core::ReadJsonFromString(fdLIST, data);

	LoggerManager()->Info(StringFormatter("Save DataBase : %s", data));
}

void func::StartMonitoring()
{

}

void func::StopMonitoring()
{

}

void func::SaveMonitoringResult()
{

}

void func::SaveMonitoringInfo()
{

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