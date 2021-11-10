#include "CPolicyRestApi.h"

void CPolicyRestApi::Routing(Pistache::Rest::Router& router)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("Routing Complete"));

    Pistache::Rest::Routes::Get(router, "/v1/policies", Pistache::Rest::Routes::bind(&CPolicyRestApi::GetPolicyLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/policies/:idx", Pistache::Rest::Routes::bind(&CPolicyRestApi::GetPolicyInfo, this));
    Pistache::Rest::Routes::Post(router, "/v1/policies", Pistache::Rest::Routes::bind(&CPolicyRestApi::PostPolicyInfo, this));
    Pistache::Rest::Routes::Put(router, "/v1/policies/:idx", Pistache::Rest::Routes::bind(&CPolicyRestApi::PutPolicyInfo, this));
    Pistache::Rest::Routes::Delete(router, "/v1/policies/:idx", Pistache::Rest::Routes::bind(&CPolicyRestApi::DeletePolicyInfo, this));
    Pistache::Rest::Routes::Post(router, "/v1/policies/:idx/activate", Pistache::Rest::Routes::bind(&CPolicyRestApi::PostPolicyActivate, this));
    Pistache::Rest::Routes::Post(router, "/v1/policies/:idx/inactivate", Pistache::Rest::Routes::bind(&CPolicyRestApi::PostPolicyInactivate, this));
    Pistache::Rest::Routes::Get(router, "/v1/policies/:idx/download", Pistache::Rest::Routes::bind(&CPolicyRestApi::GetPolicyDownload, this));
    Pistache::Rest::Routes::Get(router, "/v1/policies/:idx/devices", Pistache::Rest::Routes::bind(&CPolicyRestApi::GetPolicyAvailableDeviceLists, this));
}

void CPolicyRestApi::GetPolicyLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("GetPolicyLists"));

    bool error = false;
    int page = 1;
    int limit = 20;

    nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

    if (request.query().has("page")) {
        std::string message = request.query().get("page").value();
        if (std::regex_match(message, regexPage))
            page = atoi(message.c_str());
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({{"Parameter Errors", "page must be number."}});
            error = true;
        }
    }

    if (request.query().has("limit")) {
        std::string message = request.query().get("limit").value();
        if (std::regex_match(message, regexPage))
            limit = atoi(message.c_str());
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "limit must be number."} });
            error = true;
        }
    }

    if (error) {
        response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    MYSQL_RES* res = dbcon.SelectQuery(
        TEXT("SELECT JSON_OBJECT('idx', p.idx, 'main', main, 'sub', sub, 'classify', classify, 'name',NAME, 'description', description) FROM policy p JOIN security_category s ON p.security_category_idx = s.idx ORDER BY p.idx ASC LIMIT %d OFFSET %d;"),
        limit, limit*(page -1));
    
    if (res == NULL) {
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
        response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;    
    }

    for (auto i : CDatabase::GetRowList(res)) {
        jsonMessage["outputs"].push_back(nlohmann::json::parse(i[0]));
    }

    jsonMessage["message"] = "Success";
    response.send(Pistache::Http::Code::Ok, jsonMessage.dump(),Pistache::Http::Mime::MediaType::fromString("application/json"));
}

void CPolicyRestApi::GetPolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
}

void CPolicyRestApi::PostPolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
}

void CPolicyRestApi::PutPolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
}

void CPolicyRestApi::DeletePolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
}

void CPolicyRestApi::PostPolicyActivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
}

void CPolicyRestApi::PostPolicyInactivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
}

void CPolicyRestApi::GetPolicyDownload(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
}

void CPolicyRestApi::GetPolicyAvailableDeviceLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
}

