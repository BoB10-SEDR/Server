#pragma once
#include "stdafx.h"
#include "CApi.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

class CMonitoringRestApi : public CApi<Pistache::Rest::Router>
{
public:
	void Routing(Pistache::Rest::Router& router);
private:
	void GetProcessLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetFileDescriptorLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);	
	void PostProcessLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostFileDescriptorLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostMonitoringActivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostMonitoringInactivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
};

