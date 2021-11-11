#include "CRestApiServer.h"
#include "CDeviceRestApi.h"
#include "CTestRestApi.h"
#include "CPolicyRestApi.h"
#include "CInspectionApi.h"
#include "CNetworkCategoryRestApi.h"
#include "CSecurityCategoryRestApi.h"

extern ST_ENV env;

CRestApiServer::CRestApiServer()
{

}

CRestApiServer::~CRestApiServer()
{

}

void CRestApiServer::Init()
{

	//port 초기화
	Pistache::Port port(static_cast<uint16_t>(std::stol(env.apiPort)));

	int thr = std::stoi(env.thr);

	//ip 초기화
	Pistache::Address addr(Pistache::Ipv4::any(), port);

	std::cout << "Cores = " << Pistache::hardware_concurrency() << std::endl;
	std::cout << "Using " << thr << " threads" << std::endl;

	httpEndpoint = std::make_shared<Pistache::Http::Endpoint>(addr);

	//쓰레드 개수 초기화
	auto opts = Pistache::Http::Endpoint::options().threads(static_cast<int>(thr));
	httpEndpoint->init(opts);

	//api 리스트 초기화

	apiList.push_back(new CDeviceRestApi());
	apiList.push_back(new CTestRestApi());
	apiList.push_back(new CPolicyRestApi());
	apiList.push_back(new CInspectionApi());
	apiList.push_back(new CNetworkCategoryRestApi());
	apiList.push_back(new CSecurityCategoryRestApi());

	SetupRoutes();
}

void CRestApiServer::Start()
{
	core::Log_Debug(TEXT("RestApiServer.cpp - [%s]"), TEXT("Working RestAPI In Thread"));
	httpEndpoint->setHandler(router.handler());
	httpEndpoint->serve();
}

void CRestApiServer::End()
{
	for (auto i : apiList)
	{
		free(i);
	}
}

void CRestApiServer::SetupRoutes()
{
	for (auto i:apiList)
	{
		i->Routing(router);
	}
}

CRestApiServer* CRestApiServer::GetInstance()
{
	static CRestApiServer instance;
	return &instance;
}