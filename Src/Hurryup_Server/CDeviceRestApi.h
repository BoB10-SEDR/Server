 #pragma once
#include "stdafx.h"
#include "CApi.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

class CDeviceRestApi : public CApi<Pistache::Rest::Router>
{
public:
    void Routing(Pistache::Rest::Router& router);
private:
	void GetDeviceLists(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceInfo(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void PostDeviceInfo(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void PutDeviceInfo(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void DeleteDeviceInfo(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceCategoryLists(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceCategoryInfo(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void PostDeviceCategoryInfo(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void PutDeviceCategoryInfo(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void DeleteDeviceCategoryInfo(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceCount(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceUnregistedLists(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceHaveModules(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetAllDeviceStatistics(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceStatistics(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetAllDeviceLogsAttack(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceLogsAttack(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceHavePolicy(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceActivatePolicy(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceInactivatePolicy(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceRecommandPolicy(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetAllDeviceLogs(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceLogs(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceRecommandInspection(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void GetDeviceLive(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
};