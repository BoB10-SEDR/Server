#pragma once

#include "stdafx.h"
#include "CServer.h"
#include "CApi.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

class CRestApiServer : public::CServer
{
private:
	std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint;
	explicit CRestApiServer(Pistache::Address addr) : httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(addr))
	{ }
	std::vector<CApi<Pistache::Rest::Router>*> apiList;
	Pistache::Rest::Router router;
	CRestApiServer();
	~CRestApiServer();
public:
	static CRestApiServer* GetInstance(void);
	virtual void Init();
	virtual void Start();
	virtual void End();
	virtual void SetupRoutes();
};

inline CRestApiServer* RestApiServerManager()
{
	return CRestApiServer::GetInstance();
}


