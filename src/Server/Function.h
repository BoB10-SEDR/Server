#pragma once
#include "stdafx.h"

namespace func
{
	void GetProcessList(std::tstring agentInfo);
	void SaveProcessList(std::tstring agentInfo, std::tstring data);
	void GetFileDescriptorList(std::tstring agentInfo, std::tstring pid);
	void SaveFileDescriptorList(std::tstring agentInfo, std::tstring data);
	void StartMonitoring(std::tstring agentInfo, std::vector<std::tstring> logLists);
	void StopMonitoring(std::tstring agentInfo, std::vector<std::tstring> logLists);
	void SaveMonitoringInfo(std::tstring agentInfo, std::tstring data);
	void GetDeviceInfo(std::tstring agentInfo);
	void SaveDeviceInfo(std::tstring agentInfo, std::tstring data);
	void GetModuleInfo(std::tstring agentInfo);
	void SaveModuleInfo(std::tstring agentInfo, std::tstring data);
	void ActivatePolicy(std::tstring agentInfo, int idx, std::tstring name, std::tstring version);
	void InactivatePolicy(std::tstring agentInfo, int idx, std::tstring name, std::tstring version);
	void SavePolicyStatus(std::tstring agentInfo, std::tstring data);
	void ActivateCheck(std::tstring agentInfo, int idx, std::tstring name);
	void SaveCheckStatus(std::tstring agentInfo, std::tstring data);
	void SaveMessage(std::tstring agentInfo, std::tstring data);
}
