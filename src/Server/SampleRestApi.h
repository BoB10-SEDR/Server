#pragma once
#include "stdafx.h"
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
    std::mutex sampleInfosLock;
    std::vector<struct ST_SAMPLE_INFO> sampleInfos;

    void GetSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void PostSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void PutSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void DeleteSampleRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
};

