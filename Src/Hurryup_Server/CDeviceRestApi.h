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
	void GetSampleRestApi(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void PostSampleRestApi(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void PutSampleRestApi(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
	void DeleteSampleRestApi(const Pistache::Rest::Request & request, Pistache::Http::ResponseWriter response);
};