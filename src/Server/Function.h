#pragma once
#include "stdafx.h"

namespace func
{
	void RequestProcessList(std::tstring agentInfo);
	void ResponseProcessList(std::tstring agentInfo, std::tstring data);
	void RequestFileDescriptorList(std::tstring agentInfo, std::tstring pid);
	void ResponseFileDescriptorList(std::tstring agentInfo, std::tstring data);
	void RequestStartMonitoring(std::tstring agentInfo, std::vector<ST_MONITOR_TARGET> targetLists);
	void RequestStopMonitoring(std::tstring agentInfo, std::vector<ST_MONITOR_TARGET> targetLists);
	void ResponseMonitoringResult(std::tstring agentInfo, std::tstring data);
	void ResponseMonitoringLog(std::tstring agentInfo, std::tstring data);
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
