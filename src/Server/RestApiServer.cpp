#include "RestApiServer.h"

void CRestApiServer::Init(size_t thr)
{
	auto opts = Pistache::Http::Endpoint::options().threads(static_cast<int>(thr));
	httpEndpoint->init(opts);
	SetupRoutes();
}

void CRestApiServer::Start()
{
	httpEndpoint->setHandler(router.handler());
	httpEndpoint->serve();
}

void CRestApiServer::SetupRoutes()
{
	sampleRestApi.Routing(router);
}