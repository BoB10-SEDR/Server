#pragma once
#include "stdafx.h"
#include "CApi.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

class CTestRestApi : public CApi<Pistache::Rest::Router>
{
public:
	void Routing(Pistache::Rest::Router& router);
private:
	void PostProcessListRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostFileDescriptorListRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostStartMonitoringRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostStopMonitoringRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostDeviceInfoRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostModuleInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
};

