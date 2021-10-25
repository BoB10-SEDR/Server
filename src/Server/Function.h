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
	void GetDeviceInfo();
	void SaveDeviceInfo();
	void GetModuleInfo();
	void SaveModuleInfo();
	void ActivatePolicy();
	void InactivatePolicy();
}
