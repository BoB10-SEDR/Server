#include "Function.h"
#include "CMessage.h"

void func::GetProcessList(std::tstring agentInfo)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Process List"), TEXT(agentInfo.c_str()));
	MessageManager()->PushSendMessage(agentInfo, REQUEST, PROCESS_LIST, "");
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Process List Complete"), TEXT(agentInfo.c_str()));
}

void func::SaveProcessList(std::tstring agentInfo, std::tstring data)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Response Process List"), TEXT(agentInfo.c_str()));

	ST_PROCESS_LIST processList;

	core::ReadJsonFromString(&processList, data);
#ifdef _DEBUG
	for (auto i : processList.processLists)
	{
		core::Log_Debug(TEXT("Function.cpp - [PID] : %d"), i.pid);
		core::Log_Debug(TEXT("Function.cpp - [PPID] : %d"), i.ppid);
		core::Log_Debug(TEXT("Function.cpp - [Name] : %s"), TEXT(i.name.c_str()));
		core::Log_Debug(TEXT("Function.cpp - [State] : %s"), TEXT(i.state.c_str()));
		core::Log_Debug(TEXT("Function.cpp - [Cmdline] : %s"), TEXT(i.cmdline.c_str()));
		core::Log_Debug(TEXT("Function.cpp - [STime] : %s"), TEXT(i.startTime.c_str()));
	}
#endif
	//Save DataBase

	core::Log_Debug(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Save DataBase"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Response Process List Complete"), TEXT(agentInfo.c_str()));
}

void func::GetFileDescriptorList(std::tstring agentInfo, std::tstring pid)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Process File Descriptor List"), TEXT(agentInfo.c_str()));
	MessageManager()->PushSendMessage(agentInfo, REQUEST, FD_LIST, pid);
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Process File Descriptor List Complete"), TEXT(agentInfo.c_str()));
}

void func::SaveFileDescriptorList(std::tstring agentInfo, std::tstring data)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Response Process File Descriptor List"), TEXT(agentInfo.c_str()));

	ST_FD_LIST fdLIST;
	core::ReadJsonFromString(&fdLIST, data);

#ifdef _DEBUG
	core::Log_Debug(TEXT("Function.cpp - [PID] : %d"), fdLIST.pid);
	for (auto i : fdLIST.fdLists)
	{
		core::Log_Debug(TEXT("Function.cpp - [PID] : %d"), i.pid);
		core::Log_Debug(TEXT("Function.cpp - [Name] : %s"), TEXT(i.name.c_str()));
		core::Log_Debug(TEXT("Function.cpp - [Path] : %s"), TEXT(i.path.c_str()));
	}
#endif

	//Save DataBase
	core::Log_Debug(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Save DataBase"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Response Process List Complete"), TEXT(agentInfo.c_str()));
}

void func::StartMonitoring(std::tstring agentInfo, std::vector<std::tstring> logLists)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Monitoring Target Added"), TEXT(agentInfo.c_str()));

	ST_MONITOR_LIST monitorList;
	monitorList.pathLists = logLists;

	std::tstring jsMonitorList;
	core::WriteJsonToString(&monitorList, jsMonitorList);
	MessageManager()->PushSendMessage(agentInfo, REQUEST, MONITOR_ACTIVATE, jsMonitorList);

	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Monitoring Target Added Complete"), TEXT(agentInfo.c_str()));
}

void func::StopMonitoring(std::tstring agentInfo, std::vector<std::tstring> logLists)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Monitoring Target Removed"), TEXT(agentInfo.c_str()));

	ST_MONITOR_LIST monitorList;
	std::tstring jsMonitorList;
	core::WriteJsonToString(&monitorList, jsMonitorList);

	MessageManager()->PushSendMessage(agentInfo, REQUEST, MONITOR_INACTIVATE, jsMonitorList);
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Monitoring Target Removed Complete"), TEXT(agentInfo.c_str()));
}

void func::SaveMonitoringInfo(std::tstring agentInfo, std::tstring data)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Request Monitoring Info"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));

	ST_MONITOR_INFO monitorInfo;
	core::ReadJsonFromString(&monitorInfo, data);

	//Save DataBase
	core::Log_Debug(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Save DataBase"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Monitoring Info Complete"), TEXT(agentInfo.c_str()));
}

void func::GetDeviceInfo(std::tstring agentInfo)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Device Info"), TEXT(agentInfo.c_str()));
	MessageManager()->PushSendMessage(agentInfo, REQUEST, DEVICE_INFO, "");
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Device Info Complete"), TEXT(agentInfo.c_str()));
}

void func::SaveDeviceInfo(std::tstring agentInfo, std::tstring data)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Response Device Info"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));

	ST_DEVICE_INFO deviceInfo;
	core::ReadJsonFromString(&deviceInfo, data);

	//Save DataBase
	core::Log_Debug(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Save DataBase"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Response Device Info Complete"), TEXT(agentInfo.c_str()));
}

void func::GetModuleInfo(std::tstring agentInfo)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Module Info"), TEXT(agentInfo.c_str()));
	MessageManager()->PushSendMessage(agentInfo, REQUEST, MODULE_INFO, "");
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Module Info Complete"), TEXT(agentInfo.c_str()));
}

void func::SaveModuleInfo(std::tstring agentInfo, std::tstring data)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Response Module Info"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));

	ST_MODULE_INFO moduleInfo;
	core::ReadJsonFromString(&moduleInfo, data);

	//Save DataBase
	core::Log_Debug(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Save DataBase"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Response Module Info Complete"), TEXT(agentInfo.c_str()));
}

void func::ActivatePolicy(std::tstring agentInfo, int idx, std::tstring name, std::tstring version)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Policy Activate"), TEXT(agentInfo.c_str()));

	ST_POLICY_INFO policyInfo;
	policyInfo.idx = idx;
	policyInfo.name = name;
	policyInfo.version = version;

	std::tstring jsPolicyInfo;
	core::WriteJsonToString(&policyInfo, jsPolicyInfo);
	MessageManager()->PushSendMessage(agentInfo, REQUEST, POLICY_ACTIVATE, jsPolicyInfo);

	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Policy Activate Complete"), TEXT(agentInfo.c_str()));
}

void func::InactivatePolicy(std::tstring agentInfo, int idx, std::tstring name, std::tstring version)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Policy InActivate"), TEXT(agentInfo.c_str()));

	ST_POLICY_INFO policyInfo;
	policyInfo.idx = idx;
	policyInfo.name = name;
	policyInfo.version = version;

	std::tstring jsPolicyInfo;
	core::WriteJsonToString(&policyInfo, jsPolicyInfo);
	MessageManager()->PushSendMessage(agentInfo, REQUEST, POLICY_INACTIVATE, jsPolicyInfo);

	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request Policy InActivate Complete"), TEXT(agentInfo.c_str()));
}

void func::SavePolicyStatus(std::tstring agentInfo, std::tstring data)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Response Policy Check"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));

	ST_POLICY_RESULT policyState;
	core::ReadJsonFromString(&policyState, data);

	//Save DataBase
	core::Log_Debug(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Save DataBase"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Response Policy Check Complete"), TEXT(agentInfo.c_str()));
}

void func::ActivateCheck(std::tstring agentInfo, int idx, std::tstring name)
{
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request CheckList Activate"), TEXT(agentInfo.c_str()));

	int logID;	//Create Database Data Result idx
	ST_CHECK_INFO checkInfo;
	checkInfo.idx = idx;
	checkInfo.name = name;
	checkInfo.logID = logID;

	std::tstring jsCheckInfo;
	core::WriteJsonToString(&checkInfo, jsCheckInfo);
	MessageManager()->PushSendMessage(agentInfo, REQUEST, CHECK_ACTIVATE, jsCheckInfo);

	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Request CheckList Activate Complete"), TEXT(agentInfo.c_str()));
}

void func::SaveCheckStatus(std::tstring agentInfo, std::tstring data)
{
	core::Log_Info(TEXT("Function.cpp - [%s]"), TEXT("Response Checklist Status"));

	ST_CHECK_RESULT checkResult;
	core::ReadJsonFromString(&checkResult, data);

	//Save DataBase
	core::Log_Debug(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Save DataBase"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Response Checklist Status Complete"), TEXT(agentInfo.c_str()));
}

void func::SaveMessage(std::tstring agentInfo, std::tstring data)
{
	core::Log_Info(TEXT("Function.cpp - [%s]"), TEXT("Response Message"));

	ST_MESSAGE message;
	core::ReadJsonFromString(&message, data);

	if (!message.status) {
		core::Log_Warn(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Error Command"), TEXT(agentInfo.c_str()), message.data.c_str());
	}
	else {
		core::Log_Debug(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Success Command"), TEXT(agentInfo.c_str()), message.data.c_str());
	}

	//Save DataBase
	core::Log_Debug(TEXT("Function.cpp - [%s-%s] : %s"), TEXT("Save DataBase"), TEXT(agentInfo.c_str()), TEXT(data.c_str()));
	core::Log_Info(TEXT("Function.cpp - [%s-%s]"), TEXT("Response Message Complete"), TEXT(agentInfo.c_str()));
}