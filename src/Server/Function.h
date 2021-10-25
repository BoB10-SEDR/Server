#pragma once
#include "stdafx.h"

namespace func
{
	void GetProcessList(std::string agentInfo);
	void SaveProcessList(std::string data);
	void GetFileDescriptorList(std::string agentInfo);
	void SaveFileDescriptorList(std::string data);
	void StartMonitoring();
	void StopMonitoring();
	void SaveMonitoringResult();
	void SaveMonitoringInfo();
	void GetDeviceInfo();
	void SaveDeviceInfo();
	void GetModuleInfo();
	void SaveModuleInfo();
	void ActivatePolicy();
	void InactivatePolicy();
}
