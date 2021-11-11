#pragma once
#include "stdafx.h"
#include "CApi.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

class CNetworkCategoryRestApi : public CApi<Pistache::Rest::Router>
{
public:
	void Routing(Pistache::Rest::Router& router);
private:
	void GetNetworkCategoryLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetNetworkCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostNetworkCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PutNetworkCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void DeleteNetworkCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
};

