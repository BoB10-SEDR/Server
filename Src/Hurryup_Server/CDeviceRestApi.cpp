#include "CDeviceRestApi.h"

void CDeviceRestApi::Routing(Pistache::Rest::Router &router)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("Routing Complete"));

    Pistache::Rest::Routes::Get(router, "/sample/:name", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetSampleRestApi, this));
    Pistache::Rest::Routes::Post(router, "/sample/:name/:value?", Pistache::Rest::Routes::bind(&CDeviceRestApi::PostSampleRestApi, this));
    Pistache::Rest::Routes::Put(router, "/sample/:name/:value?", Pistache::Rest::Routes::bind(&CDeviceRestApi::PutSampleRestApi, this));
    Pistache::Rest::Routes::Delete(router, "/sample/:name", Pistache::Rest::Routes::bind(&CDeviceRestApi::DeleteSampleRestApi, this));
}

void CDeviceRestApi::GetSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("SampleRestApi.cpp - [%s]"), TEXT("GetSampleRestApi"));

    auto name = request.param(":name").as<std::string>();

    response.send(Pistache::Http::Code::Not_Found, "GetSampleRestApi\n");
}

void CDeviceRestApi::PostSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("SampleRestApi.cpp - [%s]"), TEXT("PostSampleRestApi"));

    auto name = request.param(":name").as<std::string>();

    response.send(Pistache::Http::Code::Ok, "PostSampleRestApi\n");
}

void CDeviceRestApi::PutSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("SampleRestApi.cpp - [%s]"), TEXT("PutSampleRestApi"));

    auto name = request.param(":name").as<std::string>();


    response.send(Pistache::Http::Code::Not_Found, "PutSampleRestApi\n");
}

void CDeviceRestApi::DeleteSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("SampleRestApi.cpp - [%s]"), TEXT("DeleteSampleRestApi"));

    auto name = request.param(":name").as<std::string>();

    response.send(Pistache::Http::Code::Ok, "DeleteSampleRestApi\n");
}