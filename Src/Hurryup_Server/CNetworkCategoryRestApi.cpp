#include "CNetworkCategoryRestApi.h"

void CNetworkCategoryRestApi::Routing(Pistache::Rest::Router& router)
{
    core::Log_Debug(TEXT("CNetworkCategoryRestApiApi.cpp - [%s]"), TEXT("Routing Complete"));

    Pistache::Rest::Routes::Get(router, "/v1/networks/categories", Pistache::Rest::Routes::bind(&CNetworkCategoryRestApi::GetNetworkCategoryLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/networks/categories/:idx", Pistache::Rest::Routes::bind(&CNetworkCategoryRestApi::GetNetworkCategoryInfo, this));
    Pistache::Rest::Routes::Post(router, "/v1/networks/categories", Pistache::Rest::Routes::bind(&CNetworkCategoryRestApi::PostNetworkCategoryInfo, this));
    Pistache::Rest::Routes::Put(router, "/v1/networks/categories/:idx", Pistache::Rest::Routes::bind(&CNetworkCategoryRestApi::PutNetworkCategoryInfo, this));
    Pistache::Rest::Routes::Delete(router, "/v1/networks/categories/:idx", Pistache::Rest::Routes::bind(&CNetworkCategoryRestApi::DeleteNetworkCategoryInfo, this));
}

void CNetworkCategoryRestApi::GetNetworkCategoryLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CNetworkCategoryRestApi.cpp - [%s]"), TEXT("GetNetworkCategoryLists"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    bool error = false;
    CDatabase db("192.168.181.134", "bob", "bob10-sedr12!@", "3306", "hurryup_sedr");

    nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

    MYSQL_RES* res = db.SelectQuery(TEXT("SELECT JSON_OBJECT('idx', idx, 'name', name) FROM `network_category` ORDER BY `idx` ASC;"));

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

void CNetworkCategoryRestApi::GetNetworkCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CNetworkCategoryRestApi.cpp - [%s]"), TEXT("GetNetworkCategoryInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    bool error = false;
    int idx = -1;

    nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

    if (request.hasParam(":idx")) {
        std::string message = request.param(":idx").as<std::string>();
        if (std::regex_match(message, regexNumber))
            idx = atoi(message.c_str());
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "idx must be number."} });
            error = true;
        }
    }

    if (error) {
        response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    MYSQL_RES* res = dbcon.SelectQuery(
        TEXT("SELECT JSON_OBJECT('idx', idx, 'name', name) FROM network_category WHERE idx = %d;"),
        idx);

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

void CNetworkCategoryRestApi::PostNetworkCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CNetworkCategoryRestApi.cpp - [%s]"), TEXT("PostNetworkCategoryInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        std::vector<std::string> column = { "name" };

        for (auto i : column)
        {
            if (!request_body.count(i))
            {
                error = true;
                jsonMessage["errors"].push_back({ {"Parameter Not exisit.", i} });
            }
        }

        if (error)
        {
            response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        int idx = dbcon.InsertQuery(TEXT("INSERT INTO network_category(name) values('%s');"),
                TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str())
            );

        if (idx == -1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        jsonMessage["message"] = "Success";
        jsonMessage["outputs"].push_back({ {"idx", idx} });
        response.send(Pistache::Http::Code::Created, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
    }
    catch (nlohmann::json::type_error& ex)
    {
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", ex.what()} });
        response.send(Pistache::Http::Code::Internal_Server_Error, ex.what());
    }
}

void CNetworkCategoryRestApi::PutNetworkCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CNetworkCategoryRestApi.cpp - [%s]"), TEXT("PutNetworkCategoryInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        int idx = -1;

        std::vector<std::string> column = { "name" };

        if (request.hasParam(":idx")) {
            std::string message = request.param(":idx").as<std::string>();
            if (std::regex_match(message, regexNumber))
                idx = atoi(message.c_str());
            else {
                jsonMessage["message"] = "Error";
                jsonMessage["errors"].push_back({ {"Parameter Errors", "idx must be number."} });
                error = true;
            }
        }

        for (auto i : column)
        {
            if (!request_body.count(i))
            {
                error = true;
                jsonMessage["errors"].push_back({ {"Parameter Not exisit.", i} });
            }
        }

        if (error)
        {
            response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM network_category WHERE idx = %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        if (CDatabase::GetRowList(res).size() != 1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "Network Category Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        int result = false;

        result = dbcon.UpdateQuery(TEXT("UPDATE network_category set name = '%s' where idx = %d;"),
            TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
            idx
        );

        if (!result) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        jsonMessage["message"] = "Success";
        jsonMessage["outputs"].push_back({ {"idx", idx} });
        response.send(Pistache::Http::Code::Ok, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
    }
    catch (nlohmann::json::type_error& ex)
    {
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", ex.what()} });
        response.send(Pistache::Http::Code::Internal_Server_Error, ex.what());
    }
}

void CNetworkCategoryRestApi::DeleteNetworkCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CNetworkCategoryRestApi.cpp - [%s]"), TEXT("DeleteNetworkCategoryInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        int idx = -1;

        if (request.hasParam(":idx")) {
            std::string message = request.param(":idx").as<std::string>();
            if (std::regex_match(message, regexNumber))
                idx = atoi(message.c_str());
            else {
                jsonMessage["message"] = "Error";
                jsonMessage["errors"].push_back({ {"Parameter Errors", "idx must be number."} });
                error = true;
            }
        }

        if (error)
        {
            response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM network_category WHERE idx = %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        if (CDatabase::GetRowList(res).size() != 1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "Network Category Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        bool result = true;

        result = dbcon.DeleteQuery(TEXT("DELETE FROM network_category where idx = %d;"), idx);

        if (!result) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        jsonMessage["message"] = "Success";
        response.send(Pistache::Http::Code::No_Content, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
    }
    catch (nlohmann::json::type_error& ex)
    {
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", ex.what()} });
        response.send(Pistache::Http::Code::Internal_Server_Error, ex.what());
    }
}
