#include "CTestRestApi.h"

void CTestRestApi::Routing(Pistache::Rest::Router& router)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("Routing Complete"));

    Pistache::Rest::Routes::Post(router, "/request/list/process", Pistache::Rest::Routes::bind(&CTestRestApi::PostProcessListRestApi, this));
    Pistache::Rest::Routes::Post(router, "/request/list/fd", Pistache::Rest::Routes::bind(&CTestRestApi::PostFileDescriptorListRestApi, this));
    Pistache::Rest::Routes::Post(router, "/request/monitoring/activate", Pistache::Rest::Routes::bind(&CTestRestApi::PostStartMonitoringRestApi, this));
    Pistache::Rest::Routes::Post(router, "/request/monitoring/inactivate", Pistache::Rest::Routes::bind(&CTestRestApi::PostStopMonitoringRestApi, this));
    Pistache::Rest::Routes::Post(router, "/request/device/info", Pistache::Rest::Routes::bind(&CTestRestApi::PostDeviceInfoRestApi, this));
    Pistache::Rest::Routes::Post(router, "/request/module/info", Pistache::Rest::Routes::bind(&CTestRestApi::PostModuleInfo, this));
}

void CTestRestApi::PostProcessListRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("SampleRestApi.cpp - [%s]"), TEXT("PostProcessListRestApi"));

    try
    {
        nlohmann::json request_body = nlohmann::json::parse(request.body());

        int idx = request_body.at("idx").get_ref<const nlohmann::json::number_integer_t&>();
        int socket_key = -1;

        MYSQL_RES *res = dbcon.SelectQuery(TEXT("SELECT socket_key FROM bob10_sedr.device where idx=%d"), idx);

        std::vector<MYSQL_ROW> row = CDatabase::GetRowList(res);
        if (row.size() == 1)
            socket_key = std::stoi(row[0][0]);

        if (socket_key == -1) {
            response.send(Pistache::Http::Code::Ok, "Not Device");
        }
        else {
            MessageManager()->PushSendMessage(socket_key, REQUEST, "/request/list/process", "");
            response.send(Pistache::Http::Code::Ok, "Complete");
        }
    }
    catch (nlohmann::json::type_error& ex)
    {
        response.send(Pistache::Http::Code::Ok, ex.what());
    }
}

void CTestRestApi::PostFileDescriptorListRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("SampleRestApi.cpp - [%s]"), TEXT("PostSampleRestApi"));

    auto name = request.param(":name").as<std::string>();

    response.send(Pistache::Http::Code::Ok, "PostSampleRestApi\n");
}

void CTestRestApi::PostStartMonitoringRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("SampleRestApi.cpp - [%s]"), TEXT("PostSampleRestApi"));

    auto name = request.param(":name").as<std::string>();

    response.send(Pistache::Http::Code::Ok, "PostSampleRestApi\n");
}

void CTestRestApi::PostStopMonitoringRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("SampleRestApi.cpp - [%s]"), TEXT("PostSampleRestApi"));

    auto name = request.param(":name").as<std::string>();

    response.send(Pistache::Http::Code::Ok, "PostSampleRestApi\n");
}

void CTestRestApi::PostDeviceInfoRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("SampleRestApi.cpp - [%s]"), TEXT("PostSampleRestApi"));

    auto name = request.param(":name").as<std::string>();

    response.send(Pistache::Http::Code::Ok, "PostSampleRestApi\n");
}

void CTestRestApi::PostModuleInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("SampleRestApi.cpp - [%s]"), TEXT("PostSampleRestApi"));

    auto name = request.param(":name").as<std::string>();

    response.send(Pistache::Http::Code::Ok, "PostSampleRestApi\n");
}