#pragma once
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include "SampleRestApi.h"

class CRestApiServer
{
public:
    explicit CRestApiServer(Pistache::Address addr) : httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(addr))
    { }

    void Init(size_t thr = 2);
    void Start();

private:
    std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint;
    Pistache::Rest::Router router;
    CSampleRestApi sampleRestApi;

    void SetupRoutes();
};

