#pragma once
#include "stdafx.h"

namespace func
{
	void GetProcessList(std::string agentInfo);
	void SaveProcessList(std::string agentInfo, std::string data);
	void GetFileDescriptorList(std::string agentInfo);
	void SaveFileDescriptorList(std::string agentInfo, std::string data);
	void StartMonitoring(std::string agentInfo, std::vector<std::string> logLists);
	void StopMonitoring(std::string agentInfo, std::vector<std::string> logLists);
	void SaveMonitoringInfo(std::string agentInfo, std::string data);
	void GetDeviceInfo(std::string agentInfo);
	void SaveDeviceInfo(std::string agentInfo, std::string data);
	void GetModuleInfo(std::string agentInfo);
	void SaveModuleInfo(std::string agentInfo, std::string data);
	void ActivatePolicy();
	void InactivatePolicy();
}
