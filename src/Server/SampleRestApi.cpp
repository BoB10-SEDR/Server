#include "SampleRestApi.h"

CSampleRestApi::CSampleRestApi()
{

}

CSampleRestApi::~CSampleRestApi()
{

}

void CSampleRestApi::Routing(Pistache::Rest::Router& router)
{
    printf("Routing Complete\n");
    Pistache::Rest::Routes::Get(router, "/sample/:name", Pistache::Rest::Routes::bind(&CSampleRestApi::GetSampleRestApi, this));
    Pistache::Rest::Routes::Post(router, "/sample/:name/:value?", Pistache::Rest::Routes::bind(&CSampleRestApi::PostSampleRestApi, this));
    Pistache::Rest::Routes::Put(router, "/sample/:name/:value?", Pistache::Rest::Routes::bind(&CSampleRestApi::PutSampleRestApi, this));
    Pistache::Rest::Routes::Delete(router, "/sample/:name", Pistache::Rest::Routes::bind(&CSampleRestApi::DeleteSampleRestApi, this));
}

void CSampleRestApi::GetSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    printf("Start GetSampleRestApi\n");

    auto name = request.param(":name").as<std::string>();

    std::lock_guard<std::mutex> guard(sampleInfosLock);
    auto it = std::find_if(sampleInfos.begin(), sampleInfos.end(), [&](const struct ST_SAMPLE_INFO& sampleInfo) {
        return sampleInfo.name == name;
        });

    if (it == std::end(sampleInfos))
    {
        response.send(Pistache::Http::Code::Not_Found, "Data does not exist\n");
    }
    else
    {
        const auto& stSampleSend = *it;
        std::tstring jsSend;
        core::WriteJsonToString(&stSampleSend, jsSend);
        response.send(Pistache::Http::Code::Ok, jsSend);
    }
}

void CSampleRestApi::PostSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    printf("Start PostSampleRestApi\n");
    auto name = request.param(":name").as<std::string>();

    std::lock_guard<std::mutex> guard(sampleInfosLock);
    auto it = std::find_if(sampleInfos.begin(), sampleInfos.end(), [&](const struct ST_SAMPLE_INFO& sampleInfo) {
        return sampleInfo.name == name;
        });

    int val = 1;
    if (request.hasParam(":value"))
    {
        auto value = request.param(":value");
        val = value.as<int>();
    }

    if (it == std::end(sampleInfos))
    {
        sampleInfos.emplace_back(name, val);
        struct ST_SAMPLE_INFO stSampleSend(name, val);
        std::tstring jsSend;
        core::WriteJsonToString(&stSampleSend, jsSend);
        response.send(Pistache::Http::Code::Created, jsSend);
    }
    else
    {
        response.send(Pistache::Http::Code::Ok, "Data does already exist\n");
    }
}

void CSampleRestApi::PutSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    printf("Start PutSampleRestApi\n");
    auto name = request.param(":name").as<std::string>();

    std::lock_guard<std::mutex> guard(sampleInfosLock);
    auto it = std::find_if(sampleInfos.begin(), sampleInfos.end(), [&](const struct ST_SAMPLE_INFO& sampleInfo) {
        return sampleInfo.name == name;
        });

    int val = 1;
    if (request.hasParam(":value"))
    {
        auto value = request.param(":value");
        val = value.as<int>();
    }

    if (it == std::end(sampleInfos))
    {
        response.send(Pistache::Http::Code::Not_Found, "Metric does not exist\n");
    }
    else
    {
        auto& stSampleSend = *it;
        stSampleSend.value = val;
        std::tstring jsSend;
        core::WriteJsonToString(&stSampleSend, jsSend);
        response.send(Pistache::Http::Code::Ok, jsSend);
    }
}

void CSampleRestApi::DeleteSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    printf("Start DeleteSampleRestApi\n");
    auto name = request.param(":name").as<std::string>();

    std::lock_guard<std::mutex> guard(sampleInfosLock);
    auto it = std::find_if(sampleInfos.begin(), sampleInfos.end(), [&](const struct ST_SAMPLE_INFO& sampleInfo) {
        return sampleInfo.name == name;
        });

    int val = 1;
    if (request.hasParam(":value"))
    {
        auto value = request.param(":value");
        val = value.as<int>();
    }

    if (it == std::end(sampleInfos))
    {
        response.send(Pistache::Http::Code::Not_Found, "Data does not exist\n");
    }
    else
    {
        sampleInfos.erase(it);
        response.send(Pistache::Http::Code::Ok, "Data Deleted\n");
    }
}