#include "CSecurityCategoryRestApi.h"

void CSecurityCategoryRestApi::Routing(Pistache::Rest::Router& router)
{
    core::Log_Debug(TEXT("CSecurityCategoryRestApiApi.cpp - [%s]"), TEXT("Routing Complete"));

    Pistache::Rest::Routes::Get(router, "/v1/securities/categories", Pistache::Rest::Routes::bind(&CSecurityCategoryRestApi::GetSecurityCategoryLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/securities/categories/:idx", Pistache::Rest::Routes::bind(&CSecurityCategoryRestApi::GetSecurityCategoryInfo, this));
    Pistache::Rest::Routes::Post(router, "/v1/securities/categories", Pistache::Rest::Routes::bind(&CSecurityCategoryRestApi::PostSecurityCategoryInfo, this));
    Pistache::Rest::Routes::Put(router, "/v1/securities/categories/:idx", Pistache::Rest::Routes::bind(&CSecurityCategoryRestApi::PutSecurityCategoryInfo, this));
    Pistache::Rest::Routes::Delete(router, "/v1/securities/categories/:idx", Pistache::Rest::Routes::bind(&CSecurityCategoryRestApi::DeleteSecurityCategoryInfo, this));
}

void CSecurityCategoryRestApi::GetSecurityCategoryLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CSecurityCategoryRestApi.cpp - [%s]"), TEXT("GetSecurityCategoryLists"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    bool error = false;
    int page = 1;
    int limit = 20;

    nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

    if (request.query().has("page")) {
        std::string message = request.query().get("page").value();
        if (std::regex_match(message, regexNumber))
            page = atoi(message.c_str());
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "page must be number."} });
            error = true;
        }
    }

    if (request.query().has("limit")) {
        std::string message = request.query().get("limit").value();
        if (std::regex_match(message, regexLimit))
            limit = atoi(message.c_str());
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "limit must be 2 digits number."} });
            error = true;
        }
    }

    if (error) {
        response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    MYSQL_RES* res = dbcon.SelectQuery(
        TEXT("SELECT JSON_OBJECT('idx', idx, 'main', main, 'sub', sub) FROM security_category ORDER BY idx ASC LIMIT %d OFFSET %d;"),
        limit, limit * (page - 1));

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

void CSecurityCategoryRestApi::GetSecurityCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CSecurityCategoryRestApi.cpp - [%s]"), TEXT("GetSecurityCategoryInfo"));
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
        TEXT("SELECT JSON_OBJECT('idx', idx, 'main', main, 'sub', sub) FROM security_category WHERE idx = %d;"),
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

void CSecurityCategoryRestApi::PostSecurityCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CSecurityCategoryRestApi.cpp - [%s]"), TEXT("PostSecurityCategoryInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        std::vector<std::string> column = { "main", "sub"};

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

        int idx = dbcon.InsertQuery(TEXT("INSERT INTO security_category(main, sub) values('%s', '%s');"),
            TEXT(request_body.at("main").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("sub").get_ref<const nlohmann::json::string_t&>().c_str())
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

void CSecurityCategoryRestApi::PutSecurityCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CSecurityCategoryRestApi.cpp - [%s]"), TEXT("PutSecurityCategoryInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        int idx = -1;

        std::vector<std::string> column = { "main" ,"sub"};

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

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM security_category WHERE idx = %d;"), idx);

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

        result = dbcon.UpdateQuery(TEXT("UPDATE security_category set main = '%s', sub = '%s' where idx = %d;"),
            TEXT(request_body.at("main").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("sub").get_ref<const nlohmann::json::string_t&>().c_str()),
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

void CSecurityCategoryRestApi::DeleteSecurityCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CSecurityCategoryRestApi.cpp - [%s]"), TEXT("DeleteSecurityCategoryInfo"));
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

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM security_category WHERE idx = %d;"), idx);

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

        result = dbcon.DeleteQuery(TEXT("DELETE FROM security_category where idx = %d;"), idx);

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
