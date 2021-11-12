#pragma once
#include "stdafx.h"
#include "CApi.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

class CDashBoardRestApi : public CApi<Pistache::Rest::Router>
{
public:
	void Routing(Pistache::Rest::Router& router);
private:
	void GetDashBoardStatisticsRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetDashBoardLogTimeRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetDashBoardLogAttackRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetDashBoardLogAttackTimeRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetDashBoardLogGroupRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetDashBoardPolicyRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	std::string GetTimeStamp();
};

