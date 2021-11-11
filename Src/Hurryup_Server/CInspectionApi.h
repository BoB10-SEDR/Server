#pragma once
#include "stdafx.h"
#include "CApi.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

class CInspectionApi : public CApi<Pistache::Rest::Router>
{
public:
	void Routing(Pistache::Rest::Router& router);
private:
	void GetInspectionWorkbookLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetInspectionWorkbookInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostInspectionWorkbookInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PutInspectionWorkbookInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void DeleteInspectionWorkbookInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetInspectionLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetInspectionInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostInspectionInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PutInspectionInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void DeleteInspectionInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void PostInspectionActivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetInspectionDownload(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetInspectionAvailableDeviceLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetInspectionAvailablePolicyLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetInspectionLogLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void GetInspectionLogDetail(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
};

