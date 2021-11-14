#pragma once
#include "stdafx.h"
#include "CApi.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

class CPolicyRestApi : public CApi<Pistache::Rest::Router>
{
public:
	void Routing(Pistache::Rest::Router& router);
private:
	void GetPolicyLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetPolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostPolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PutPolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void DeletePolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostPolicyActivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostPolicyInactivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetPolicyDownload(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetPolicyAvailableDeviceLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetPolicyActivateDeviceLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
};

