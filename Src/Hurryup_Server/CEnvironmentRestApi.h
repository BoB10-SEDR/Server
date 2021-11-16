#pragma once
#include "stdafx.h"
#include "CApi.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

class CEnvironmentRestApi : public CApi<Pistache::Rest::Router>
{
public:
	void Routing(Pistache::Rest::Router& router);
private:
	void GetEnvironmentLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
};

