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

    std::lock_guard<std::mutex> guard(metricsLock);
    auto it = std::find_if(metrics.begin(), metrics.end(), [&](const Metric& metric) {
        return metric.name() == name;
        });

    if (it == std::end(metrics))
    {
        response.send(Pistache::Http::Code::Not_Found, "Metric does not exist\n");
    }
    else
    {
        const auto& metric = *it;
        response.send(Pistache::Http::Code::Ok, std::to_string(metric.value()));
    }
}

void CSampleRestApi::PostSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    printf("Start PostSampleRestApi\n");
    auto name = request.param(":name").as<std::string>();

    std::lock_guard<std::mutex> guard(metricsLock);
    auto it = std::find_if(metrics.begin(), metrics.end(), [&](const Metric& metric) {
        return metric.name() == name;
        });

    int val = 1;
    if (request.hasParam(":value"))
    {
        auto value = request.param(":value");
        val = value.as<int>();
    }

    if (it == std::end(metrics))
    {
        metrics.emplace_back(std::move(name), val);
        response.send(Pistache::Http::Code::Created, std::to_string(val));
    }
    else
    {
        response.send(Pistache::Http::Code::Ok, "Metric does already exist\n");
    }
}

void CSampleRestApi::PutSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    printf("Start PutSampleRestApi\n");
    auto name = request.param(":name").as<std::string>();

    std::lock_guard<std::mutex> guard(metricsLock);
    auto it = std::find_if(metrics.begin(), metrics.end(), [&](const Metric& metric) {
        return metric.name() == name;
        });

    int val = 1;
    if (request.hasParam(":value"))
    {
        auto value = request.param(":value");
        val = value.as<int>();
    }

    if (it == std::end(metrics))
    {
        response.send(Pistache::Http::Code::Not_Found, "Metric does not exist\n");
    }
    else
    {
        auto& metric = *it;
        metric.setvalue(val);
        printf("%s %d\n", metric.name().c_str(), metric.value());
        response.send(Pistache::Http::Code::Ok, std::to_string(metric.value()));
    }
}

void CSampleRestApi::DeleteSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    printf("Start DeleteSampleRestApi\n");
    auto name = request.param(":name").as<std::string>();

    std::lock_guard<std::mutex> guard(metricsLock);
    auto it = std::find_if(metrics.begin(), metrics.end(), [&](const Metric& metric) {
        return metric.name() == name;
        });

    int val = 1;
    if (request.hasParam(":value"))
    {
        auto value = request.param(":value");
        val = value.as<int>();
    }

    if (it == std::end(metrics))
    {
        response.send(Pistache::Http::Code::Not_Found, "Metric does not exist\n");
    }
    else
    {
        metrics.erase(it);
        response.send(Pistache::Http::Code::Ok, "Metric Deleted\n");
    }
}