#include "CAgentApi.h"
#include <time.h>

int CAgentApi::GetDeviceId(int agentFd)
{
	int deviceId = -1;

	MYSQL_RES* res = this->dbcon.SelectQuery(TEXT("SELECT idx FROM bob10_sedr.device WHERE socket=%d;"), agentFd);

	if (dbcon.GetLastError() == NULL)
	{
		core::Log_Error(TEXT("CAgentApi.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
		return -1;
	}

	std::vector<MYSQL_ROW> rowList = CDatabase::GetRowList(res);
	if (rowList.size() == 1)
		deviceId = std::stoi(rowList[0][0]);

	return deviceId;
}

void CAgentApi::Routing(std::map<OPCODE, void(CAgentApi::*)(int, std::tstring)> & router)
{
	router.insert({PROCESS_LIST, &CAgentApi::ResponseProcessList});
	router.insert({FD_LIST, &CAgentApi::ResponseFileDescriptorList });
	router.insert({MONITORING_RESULT, &CAgentApi::ResponseMonitoringResult });
	router.insert({MONITORING_LOG, &CAgentApi::ResponseMonitoringLog });
	router.insert({DEVICE, &CAgentApi::ResponseDeviceInfo });
	router.insert({DEVICE_DEAD, &CAgentApi::ResponseDeviceDead });
	router.insert({MODULE, &CAgentApi::ResponseModuleInfo });
	router.insert({POLICY_RESULT, &CAgentApi::ResponsePolicyResult });
	router.insert({INSPECTION_RESULT, &CAgentApi::ResponseCheckResult });
}

void CAgentApi::ResponseProcessList(int agentFd, std::tstring data)
{
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Response Process List"), agentFd);

	ST_PROCESS_LIST processList;

	core::ReadJsonFromString(&processList, data);

	int deviceId = GetDeviceId(agentFd);

	if (deviceId == -1)
	{
		core::Log_Warn(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Device Not Exisit"), agentFd);
		return;
	}

	dbcon.DeleteQuery(TEXT("DELETE FROM bob10_sedr.process WHERE device_idx = %d;"), deviceId);
	
	if (dbcon.GetLastError() == NULL)
	{
		core::Log_Error(TEXT("CAgentApi.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
		return;
	}

	for (auto i : processList.processLists)
	{
		dbcon.InsertQuery(TEXT("Insert into bob10_sedr.process(pid, ppid, name, state, command, start_time, update_time, device_idx) values (%d, %d,'%s','%s','%s','%s', '%s', %d);"), i.pid, i.ppid, TEXT(i.name.c_str()), TEXT(i.state.c_str()), TEXT(i.cmdline.c_str()), TEXT(i.startTime.c_str()), TEXT(Cutils::GetTimeStamp().c_str()), deviceId);
		if (dbcon.GetLastError() == NULL)
		{
			core::Log_Error(TEXT("Function.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
			return;
		}
		core::Log_Debug(TEXT("CAgentApi.cpp - [PID] : %d"), i.pid);
		core::Log_Debug(TEXT("CAgentApi.cpp - [PPID] : %d"), i.ppid);
		core::Log_Debug(TEXT("CAgentApi.cpp - [Name] : %s"), TEXT(i.name.c_str()));
		core::Log_Debug(TEXT("CAgentApi.cpp - [State] : %s"), TEXT(i.state.c_str()));
		core::Log_Debug(TEXT("CAgentApi.cpp - [Cmdline] : %s"), TEXT(i.cmdline.c_str()));
		core::Log_Debug(TEXT("CAgentApi.cpp - [STime] : %s"), TEXT(i.startTime.c_str()));
	}

	core::Log_Debug(TEXT("CAgentApi.cpp - [%s-%d] : %s"), TEXT("Save DataBase"), deviceId, TEXT(data.c_str()));
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Response Process List Complete"), deviceId);

}
void CAgentApi::ResponseFileDescriptorList(int agentFd, std::tstring data)
{
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Response Process File Descriptor List"), agentFd);

	ST_FD_LIST fdLIST;
	core::ReadJsonFromString(&fdLIST, data);

	int deviceId = GetDeviceId(agentFd);

	if (deviceId == -1)
	{
		core::Log_Warn(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Device Not Exisit"), agentFd);
		return;
	}

	dbcon.DeleteQuery(TEXT("DELETE FROM bob10_sedr.file_descriptor WHERE device_idx = %d and pid = %d;"), deviceId, fdLIST.pid);

	if (dbcon.GetLastError() == NULL)
	{
		core::Log_Error(TEXT("CAgentApi.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
		return;
	}

	core::Log_Debug(TEXT("CAgentApi.cpp - [PID] : %d"), fdLIST.pid);
	for (auto i : fdLIST.fdLists)
	{
		dbcon.InsertQuery(TEXT("Insert into bob10_sedr.file_descriptor(pid, name, path, device_idx, update_time) values (%d,'%s','%s', %d, '%s');"), i.pid, TEXT(i.fdName.c_str()), TEXT(i.realPath.c_str()), deviceId, TEXT(Cutils::GetTimeStamp().c_str()));

		if (dbcon.GetLastError() == NULL)
		{
			core::Log_Error(TEXT("Function.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
			return;
		}
		core::Log_Debug(TEXT("CAgentApi.cpp - [PID] : %d"), i.pid);
		core::Log_Debug(TEXT("CAgentApi.cpp - [FdName] : %s"), TEXT(i.fdName.c_str()));
		core::Log_Debug(TEXT("CAgentApi.cpp - [RealPath] : %s"), TEXT(i.realPath.c_str()));
	}

	//Save DataBase
	core::Log_Debug(TEXT("CAgentApi.cpp - [%s-%d] : %s"), TEXT("Save DataBase"), agentFd, TEXT(data.c_str()));
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Response Process File Descriptor Complete"), agentFd);
}
void CAgentApi::ResponseMonitoringResult(int agentFd, std::tstring data)
{
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d] : %s"), TEXT("Response Monitoring Result"), agentFd, TEXT(data.c_str()));

	ST_MONITOR_RESULT monitorResult;
	core::ReadJsonFromString(&monitorResult, data);

	int deviceId = GetDeviceId(agentFd);

	if (deviceId == -1)
	{
		core::Log_Warn(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Device Not Exisit"), agentFd);
		return;
	}

	dbcon.InsertQuery(TEXT("INSERT INTO monitoring (process_name, log_path, activate, device_idx, update_time) VALUES('%s', '%s', %d, %d, '%s') ON DUPLICATE KEY UPDATE activate = %d, update_time = '%s';"),
		TEXT(monitorResult.processName.c_str()), TEXT(monitorResult.logPath.c_str()), monitorResult.result, deviceId, TEXT(Cutils::GetTimeStamp().c_str()), monitorResult.result, TEXT(Cutils::GetTimeStamp().c_str()));

	if (dbcon.GetLastError() == NULL)
	{
		core::Log_Error(TEXT("CAgentApi.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
		return;
	}

	//Save DataBase
	core::Log_Debug(TEXT("CAgentApi.cpp - [%s-%d] : %s"), TEXT("Save DataBase"), agentFd, TEXT(data.c_str()));
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Response Monitoring Result Complete"), agentFd);
}
void CAgentApi::ResponseMonitoringLog(int agentFd, std::tstring data)
{
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d] : %s"), TEXT("Request Monitoring Info"), agentFd, TEXT(data.c_str()));

	int deviceId = GetDeviceId(agentFd);

	if (deviceId == -1)
	{
		core::Log_Warn(TEXT("Function.cpp - [%s-%d]"), TEXT("Device Not Exisit"), agentFd);
		return;
	}

	dbcon.InsertQuery(TEXT("INSERT INTO bob10_sedr.log(event_code, description, device_idx, original_log, create_time, environment, status, layer)\
						VALUES ('%s', '%s', %d, '%s', '%s', '%s', '%s, '%s);"),
						TEXT("E-Montoring"), TEXT("모니터링 로그 데이터 입니다."), deviceId, TEXT(data.c_str()), TEXT(Cutils::GetTimeStamp().c_str()), TEXT("Agent"), TEXT("INFO"), TEXT("Device"));

	if (dbcon.GetLastError() == NULL)
	{
		core::Log_Error(TEXT("CAgentApi.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
		return;
	}

	//Save DataBase
	core::Log_Debug(TEXT("CAgentApi.cpp - [%s-%d] : %s"), TEXT("Save DataBase"), agentFd, TEXT(data.c_str()));
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Request Monitoring Info Complete"), agentFd);
}
void CAgentApi::ResponseDeviceInfo(int agentFd, std::tstring data)
{
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d] : %s"), TEXT("Response Device Info"), agentFd, TEXT(data.c_str()));

	ST_NEW_INFO<ST_NEW_DEVICE_INFO> info;
	core::ReadJsonFromString(&info, data);

	std::tstring networkInfo;
	std::tstring osInfo;
	std::tstring servceList;
	std::tstring connectMethod = "[]";

	ST_NEW_VECTOR_DATABASE<ST_NEW_NETWORK_INTERFACE_INFO> stNetworkInfo("network_info", info.metaInfo.networkInfo);
	ST_NEW_VECTOR_DATABASE<ST_NEW_SERVICE_INFO> stServiceInfo("service_info", info.metaInfo.serviceList);

	core::WriteJsonToString(&stNetworkInfo, networkInfo);
	core::WriteJsonToString(&info.metaInfo.osInfo, osInfo);
	core::WriteJsonToString(&stServiceInfo, servceList);


	dbcon.InsertQuery(TEXT("INSERT INTO `device` (`name`, `model_number`, `serial_number`, `device_category_idx`, `network_info`, `os_info`, `service_list`, `connect_method`, `live`, `update_time`, `socket`)\
		VALUES('%s', '%s', '%s', (SELECT device_model_category.device_category_idx\
			FROM device_model_category\
			WHERE device_model_category.model_number = '%s'), '%s', '%s', '%s', '%s', 1, '%s', %d)\
		ON DUPLICATE KEY UPDATE `update_time` = '%s', `socket`= %d, `live`=1"),
		TEXT(info.metaInfo.name.c_str()),
		TEXT(info.metaInfo.modelNumber.c_str()),
		TEXT(info.serialNumber.c_str()),
		TEXT(info.metaInfo.modelNumber.c_str()),
		TEXT(networkInfo.c_str()),
		TEXT(osInfo.c_str()),
		TEXT(servceList.c_str()),
		TEXT(connectMethod.c_str()),
		TEXT(info.timestamp.c_str()),
		agentFd,
		TEXT(info.timestamp.c_str()),
		agentFd
	);

	if (dbcon.GetLastError() == NULL)
	{
		core::Log_Error(TEXT("Function.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
		return;
	}

	//Save DataBase
	core::Log_Debug(TEXT("Function.cpp - [%s-%d] : %s"), TEXT("Save DataBase"), agentFd, TEXT(data.c_str()));
	core::Log_Info(TEXT("Function.cpp - [%s-%d]"), TEXT("Response Device Info Complete"), agentFd);
}

void CAgentApi::ResponseDeviceDead(int agentFd, std::tstring data)
{
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Response UpdateDeviceLive"), agentFd);

	dbcon.UpdateQuery(TEXT("UPDATE device SET update_time='%s', socket=0, live=0 WHERE socket=%d;"),
		TEXT(Cutils::GetTimeStamp().c_str()),
		agentFd
	);

	if (dbcon.GetLastError() == NULL)
	{
		core::Log_Error(TEXT("CAgentApi.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
		return;
	}

	//Save DataBase
	core::Log_Debug(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Save DataBase"), agentFd);
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d]"), TEXT("Response UpdateDeviceLive Complete"), agentFd);
}
void CAgentApi::ResponseModuleInfo(int agentFd, std::tstring data)
{
	
//	core::Log_Info(TEXT("Function.cpp - [%s-%d] : %s"), TEXT("Response Module Info"), agentFd, TEXT(data.c_str()));
//
//	ST_MODULE_INFO moduleInfo;
//	core::ReadJsonFromString(&moduleInfo, data);
//
//	//이렇게 찾거나 또는 mac과 시리얼 정보로 찾는다.
//	int deviceId = GetDeviceId(agentFd);
//
//	if (deviceId == -1)
//	{
//		core::Log_Warn(TEXT("Function.cpp - [%s-%d]"), TEXT("Device Not Exisit"), agentFd);
//		return;
//	}
//
//	int category = -1;
//
//	MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT module_category_idx FROM bob10_sedr.module_model_match WHERE model_number='%s';"),
//		TEXT(moduleInfo.modelNumber.c_str())
//	);
//
//	if (dbcon.GetLastError() == NULL)
//	{
//		core::Log_Error(TEXT("Function.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
//		return;
//	}
//
//	std::vector<MYSQL_ROW> row = CDatabase::GetRowList(res);
//	if (row.size() == 1)
//		category = std::stoi(row[0][0]);
//
//	if (category == -1) {
//		dbcon.InsertQuery(TEXT("INSERT INTO module(device_idx, model_number, serial_number, mac, update_time) VALUES(%d, '%s', '%s', '%s', '%s') ON DUPLICATE KEY UPDATE update_time = '%s'"),
//			deviceId,
//			TEXT(moduleInfo.modelNumber.c_str()),
//			TEXT(moduleInfo.serialNumber.c_str()),
//			TEXT(moduleInfo.mac.c_str()),
//			TEXT(CAgentApi::GetTimeStamp().c_str()),
//			TEXT(CAgentApi::GetTimeStamp().c_str())
//		);
//
//		if (dbcon.GetLastError() == NULL)
//		{
//			core::Log_Error(TEXT("Function.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
//			return;
//		}
//	}
//	else {
//		dbcon.InsertQuery(TEXT("INSERT INTO module(module_category_idx, device_idx, model_number, serial_number, mac, update_time) VALUES(%d, %d, '%s', '%s', '%s', '%s') ON DUPLICATE KEY UPDATE update_time = '%s'"),
//			category,
//			deviceId,
//			TEXT(moduleInfo.modelNumber.c_str()),
//			TEXT(moduleInfo.serialNumber.c_str()),
//			TEXT(moduleInfo.mac.c_str()),
//			TEXT(CAgentApi::GetTimeStamp().c_str()),
//			TEXT(CAgentApi::GetTimeStamp().c_str())
//		);
//
//		if (dbcon.GetLastError() == NULL)
//		{
//			core::Log_Error(TEXT("Function.cpp - [%s] %s"), TEXT("Database Error"), TEXT(dbcon.GetLastError()));
//			return;
//		}
	//}

	////Save DataBase
	//core::Log_Debug(TEXT("Function.cpp - [%s-%d] : %s"), TEXT("Save DataBase"), agentFd, TEXT(data.c_str()));
	//core::Log_Info(TEXT("Function.cpp - [%s-%d]"), TEXT("Response Module Info Complete"), agentFd);
}
void CAgentApi::ResponsePolicyResult(int agentFd, std::tstring data)
{
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%d] : %s"), TEXT("Response Policy Check"), agentFd, TEXT(data.c_str()));

	ST_POLICY_RESULT policyState;
	core::ReadJsonFromString(&policyState, data);

	//Save DataBase
	core::Log_Debug(TEXT("CAgentApi.cpp - [%s-%d] : %s"), TEXT("Save DataBase"), agentFd, TEXT(data.c_str()));
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%s]"), TEXT("Response Policy Check Complete"), agentFd);
}
void CAgentApi::ResponseCheckResult(int agentFd, std::tstring data)
{
	core::Log_Info(TEXT("Function.cpp - [%s]"), TEXT("Response Checklist Status"));

	ST_CHECK_RESULT checkResult;
	core::ReadJsonFromString(&checkResult, data);

	//Save DataBase
	core::Log_Debug(TEXT("CAgentApi.cpp - [%s-%d] : %s"), TEXT("Save DataBase"), agentFd, TEXT(data.c_str()));
	core::Log_Info(TEXT("CAgentApi.cpp - [%s-%s]"), TEXT("Response Checklist Status Complete"), agentFd);
}