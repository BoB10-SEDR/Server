#include "CEnvironmentRestApi.h"

void CEnvironmentRestApi::Routing(Pistache::Rest::Router& router)
{
    core::Log_Debug(TEXT("CEnvironmentRestApi.cpp - [%s]"), TEXT("Routing Complete"));

    Pistache::Rest::Routes::Get(router, "/v1/environments/categories", Pistache::Rest::Routes::bind(&CEnvironmentRestApi::GetEnvironmentLists, this));
}

void CEnvironmentRestApi::GetEnvironmentLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CEnvironmentRestApi.cpp - [%s]"), TEXT("GetEnvironmentLists"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    bool error = false;
    CDatabase db("192.168.181.134", "bob", "bob10-sedr12!@", "3306", "hurryup_sedr");

    nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

    MYSQL_RES* res = db.SelectQuery(TEXT("SELECT JSON_OBJECT('idx', idx, 'name', name) FROM `environment` ORDER BY `idx` ASC;"));

    if (res == NULL) {
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
        response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    std::vector<MYSQL_ROW> rows = CDatabase::GetRowList(res);

    for (auto i : rows) {
        jsonMessage["outputs"].push_back(nlohmann::json::parse(i[0]));
    }

    jsonMessage["message"] = "Success";
    response.send(Pistache::Http::Code::Ok, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
}
