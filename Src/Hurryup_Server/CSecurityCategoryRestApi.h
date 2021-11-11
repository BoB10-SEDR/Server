#pragma once
#include "stdafx.h"
#include "CApi.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

class CSecurityCategoryRestApi : public CApi<Pistache::Rest::Router>
{
public:
	void Routing(Pistache::Rest::Router& router);
private:
	void GetSecurityCategoryLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetSecurityCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostSecurityCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PutSecurityCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void DeleteSecurityCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
};

