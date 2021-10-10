#pragma once
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

class CSampleRestApi
{
public:
    CSampleRestApi();
    ~CSampleRestApi();
    void Routing(Pistache::Rest::Router& router);
private:
    class Metric
    {
    public:
        explicit Metric(std::string name, int initialValue = 1)
            : name_(std::move(name))
            , value_(initialValue)
        { }

        void setvalue(int _value = 1)
        {
            value_ = _value;
        }

        int value() const
        {
            return value_;
        }

        const std::string& name() const
        {
            return name_;
        }

    private:
        std::string name_;
        int value_;
    };

    std::mutex metricsLock;
    std::vector<Metric> metrics;

    void GetSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void PostSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void PutSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void DeleteSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
};

