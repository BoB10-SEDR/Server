#include "CInspectionApi.h"

void CInspectionApi::Routing(Pistache::Rest::Router& router)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("Routing Complete"));

    Pistache::Rest::Routes::Get(router, "/v1/inspections/workbooks", Pistache::Rest::Routes::bind(&CInspectionApi::GetInspectionWorkbookLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/inspections/workbooks/:idx", Pistache::Rest::Routes::bind(&CInspectionApi::GetInspectionWorkbookInfo, this));
    Pistache::Rest::Routes::Post(router, "/v1/inspections/workbooks", Pistache::Rest::Routes::bind(&CInspectionApi::PostInspectionWorkbookInfo, this));
    Pistache::Rest::Routes::Put(router, "/v1/inspections/workbooks/:idx", Pistache::Rest::Routes::bind(&CInspectionApi::PutInspectionWorkbookInfo, this));
    Pistache::Rest::Routes::Delete(router, "/v1/inspections/workbooks/:idx", Pistache::Rest::Routes::bind(&CInspectionApi::DeleteInspectionWorkbookInfo, this));
    Pistache::Rest::Routes::Get(router, "/v1/inspections", Pistache::Rest::Routes::bind(&CInspectionApi::GetInspectionLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/inspections/:idx", Pistache::Rest::Routes::bind(&CInspectionApi::GetInspectionInfo, this));
    Pistache::Rest::Routes::Post(router, "/v1/inspections", Pistache::Rest::Routes::bind(&CInspectionApi::PostInspectionInfo, this));
    Pistache::Rest::Routes::Put(router, "/v1/inspections/:idx", Pistache::Rest::Routes::bind(&CInspectionApi::PutInspectionInfo, this));
    Pistache::Rest::Routes::Delete(router, "/v1/inspections/:idx", Pistache::Rest::Routes::bind(&CInspectionApi::DeleteInspectionInfo, this));
    Pistache::Rest::Routes::Post(router, "/v1/inspections/:idx/activate/:device_idx", Pistache::Rest::Routes::bind(&CInspectionApi::PostInspectionActivate, this));
    Pistache::Rest::Routes::Get(router, "/v1/inspections/:idx/download", Pistache::Rest::Routes::bind(&CInspectionApi::GetInspectionDownload, this));
    Pistache::Rest::Routes::Get(router, "/v1/inspections/:idx/devices", Pistache::Rest::Routes::bind(&CInspectionApi::GetInspectionAvailableDeviceLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/inspections/:idx/polices", Pistache::Rest::Routes::bind(&CInspectionApi::GetInspectionAvailablePolicyLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/inspections/logs", Pistache::Rest::Routes::bind(&CInspectionApi::GetInspectionLogLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/inspections/logs/:idx", Pistache::Rest::Routes::bind(&CInspectionApi::GetInspectionLogDetail, this));
}

void CInspectionApi::GetInspectionWorkbookLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("GetInspectionWorkbookLists"));

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
        TEXT("SELECT JSON_OBJECT('idx', a.idx, 'name', a.name, 'description', a.description, 'update_time', a.update_time, 'content', JSON_EXTRACT(b.content, '$'))\
            FROM inspection a\
            JOIN(\
                SELECT i.idx, CONCAT('[', GROUP_CONCAT(JSON_OBJECT('idx', ist.idx, 'name', ist.name, 'description', ist.description, 'classify', ist.classify)), ']') AS content\
                from inspection i\
                join inspection_step ist\
                ON json_contains(JSON_EXTRACT(i.content, '$[*]'), ist.idx)\
                GROUP BY i.idx\
            )b ON b.idx = a.idx\
            ORDER BY a.idx ASC LIMIT %d OFFSET %d;\
            "),
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

void CInspectionApi::GetInspectionWorkbookInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("GetInspectionWorkbookInfo"));
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
        TEXT("SELECT JSON_OBJECT('idx', a.idx, 'name', a.name, 'description', a.description, 'update_time', a.update_time, 'content', JSON_EXTRACT(b.content, '$'))\
            FROM inspection a\
            JOIN(\
                SELECT i.idx, CONCAT('[', GROUP_CONCAT(JSON_OBJECT('idx', ist.idx, 'name', ist.name, 'description', ist.description, 'classify', ist.classify, 'main', s.main, 'sub', s.sub)), ']') AS content\
                from inspection i\
                join inspection_step ist\
                ON json_contains(JSON_EXTRACT(i.content, '$[*]'), ist.idx)\
                JOIN security_category s\
                ON s.idx = ist.security_category_idx\
                GROUP BY i.idx\
            )b ON b.idx = a.idx\
            WHERE a.idx = %d;\
            "),
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

void CInspectionApi::PostInspectionWorkbookInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("PostInspectionWorkbookInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        std::vector<std::string> column = { "name", "description", "content" };

        for (auto i : column)
        {
            if (!request_body.count(i))
            {
                error = true;
                jsonMessage["errors"].push_back({ {"Parameter Not exisit.", i} });
            }
        }

        if (!request_body.at("content").is_array()) {
            error = true;
            jsonMessage["errors"].push_back({ {"Parameter Type Error.", "content type must be array"} });
        }

        for (auto i : request_body.at("content").get_ref<const nlohmann::json::array_t&>()) {
            if (!i.is_number_integer()) {
                error = true;
                jsonMessage["errors"].push_back({ {"Parameter Type Error.", "Device contents type must be Number"} });
            }
        }

        if (error)
        {
            response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        auto content = request_body.at("content").get_ref<const nlohmann::json::array_t&>();

        for (auto i : content) {
            MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM inspection_step where idx=%d;"), i.get_ref<const nlohmann::json::number_integer_t&>());

            if (res == NULL) {
                jsonMessage["message"] = "Error";
                jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
                response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
                return;
            }

            if (CDatabase::GetRowList(res).size() != 1) {
                error = true;
                jsonMessage["errors"].push_back({ {"Inspection Not Exisit", i} });
            }
        }

        if (error)
        {
            response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        std::ostringstream strContent;
        strContent << "[";
        if (!content.empty())
        {
            std::copy(content.begin(), content.end() - 1, std::ostream_iterator<int>(strContent, ","));
            strContent << content.back();
        }
        strContent << "]";


        int idx = -1;
        idx = dbcon.InsertQuery(TEXT("INSERT INTO inspection(name, description, content) values('%s','%s','%s');"),
            TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(strContent.str().c_str())
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

void CInspectionApi::PutInspectionWorkbookInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("PutInspectionWorkbookInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        int idx = -1;
        std::vector<std::string> column = { "name", "description", "content" };

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

        if (!request_body.at("content").is_array()) {
            error = true;
            jsonMessage["errors"].push_back({ {"Parameter Type Error.", "content type must be array"} });
        }

        for (auto i : request_body.at("content").get_ref<const nlohmann::json::array_t&>()) {
            if (!i.is_number_integer()) {
                error = true;
                jsonMessage["errors"].push_back({ {"Parameter Type Error.", "Device contents type must be Number"} });
            }
        }

        if (error)
        {
            response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM inspection WHERE idx = %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        if (CDatabase::GetRowList(res).size() != 1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "inspection Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        auto content = request_body.at("content").get_ref<const nlohmann::json::array_t&>();

        for (auto i : content) {
            MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM inspection_step where idx=%d;"), i.get_ref<const nlohmann::json::number_integer_t&>());

            if (res == NULL) {
                jsonMessage["message"] = "Error";
                jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
                response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
                return;
            }

            if (CDatabase::GetRowList(res).size() != 1) {
                error = true;
                jsonMessage["errors"].push_back({ {"Inspection Not Exisit", i} });
            }
        }

        if (error)
        {
            response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        std::ostringstream strContent;
        strContent << "[";
        if (!content.empty())
        {
            std::copy(content.begin(), content.end() - 1, std::ostream_iterator<int>(strContent, ","));
            strContent << content.back();
        }
        strContent << "]";

        bool result = false;

        result = dbcon.UpdateQuery(TEXT("UPDATE inspection set name = '%s' , description = '%s', content = '%s' where idx = %d;"),
            TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(strContent.str().c_str()),
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

void CInspectionApi::DeleteInspectionWorkbookInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("DeletePolicyInfo"));
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

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM inspection WHERE idx = %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        if (CDatabase::GetRowList(res).size() != 1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "inspection Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        bool result = true;

        result = dbcon.DeleteQuery(TEXT("DELETE FROM inspection where idx = %d;"), idx);

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

void CInspectionApi::GetInspectionLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("GetInspectionLists"));

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
        TEXT("SELECT JSON_OBJECT('idx', p.idx, 'main', main, 'sub', sub, 'classify', classify, 'name',NAME, 'description', description) FROM inspection_step p LEFT JOIN security_category s ON p.security_category_idx = s.idx ORDER BY p.idx ASC LIMIT %d OFFSET %d;"),
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

void CInspectionApi::GetInspectionInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("GetInspectionInfo"));
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
        TEXT("SELECT JSON_OBJECT('idx', p.idx, 'main', main, 'sub', sub, 'classify', classify, 'name',NAME, 'description', description, 'isfile', IF(isfile = 1, true, false), 'content', content) FROM inspection_step p LEFT JOIN security_category s ON p.security_category_idx = s.idx WHERE p.idx = %d;"),
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

void CInspectionApi::PostInspectionInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("PostInspectionInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        std::vector<std::string> column = { "main", "sub", "classify", "name", "description", "isfile", "content"};

        for (auto i : column)
        {
            if (!request_body.count(i))
            {
                error = true;
                jsonMessage["errors"].push_back({ {"Parameter Not exisit.", i} });
            }
        }

        if (!request_body.at("isfile").is_boolean()) {
            error = true;
            jsonMessage["errors"].push_back({ {"Parameter Type Error.", "isfile type must be boolean"} });
        }

        if (error)
        {
            response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM security_category where main='%s' and sub='%s';"),
            TEXT(request_body.at("main").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("sub").get_ref<const nlohmann::json::string_t&>().c_str()));

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        int security_category_idx = -1;
        int idx = -1;

        std::vector<MYSQL_ROW> row = CDatabase::GetRowList(res);
        if (row.size() == 1)
            security_category_idx = std::stoi(row[0][0]);

        if (security_category_idx == -1) {
            idx = dbcon.InsertQuery(TEXT("INSERT INTO inspection_step(classify, name, description, isfile, content, security_category_idx) values('%s','%s','%s',%d, '%s', NULL);"),
                TEXT(request_body.at("classify").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
                request_body.at("isfile").get_ref<const nlohmann::json::boolean_t&>() ? 1 : 0,
                TEXT(request_body.at("content").get_ref<const nlohmann::json::string_t&>().c_str())
            );
        }
        else
        {
            idx = dbcon.InsertQuery(TEXT("INSERT INTO inspection_step(classify, name, description, isfile, content, security_category_idx) values('%s','%s','%s',%d,'%s',%d);"),
                TEXT(request_body.at("classify").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
                request_body.at("isfile").get_ref<const nlohmann::json::boolean_t&>() ? 1 : 0,
                TEXT(request_body.at("content").get_ref<const nlohmann::json::string_t&>().c_str()),
                security_category_idx
            );
        }

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

void CInspectionApi::PutInspectionInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("PutPolicyInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        int idx = -1;

        std::vector<std::string> column = { "main", "sub", "classify", "name", "description", "isfile", "content"};

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

        if (!request_body.at("isfile").is_boolean()) {
            error = true;
            jsonMessage["errors"].push_back({ {"Parameter Type Error.", "isfile type must be boolean"} });
        }

        if (error)
        {
            response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM inspection_step WHERE idx = %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        if (CDatabase::GetRowList(res).size() != 1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "inspection_step Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        res = dbcon.SelectQuery(TEXT("SELECT idx FROM security_category where main='%s' and sub='%s';"),
            TEXT(request_body.at("main").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("sub").get_ref<const nlohmann::json::string_t&>().c_str()));

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        int security_category_idx = -1;
        bool result = true;

        std::vector<MYSQL_ROW> row = CDatabase::GetRowList(res);
        if (row.size() == 1)
            security_category_idx = std::stoi(row[0][0]);

        if (security_category_idx == -1) {
            result = dbcon.UpdateQuery(TEXT("UPDATE inspection_step set classify = '%s', name = '%s' , description = '%s', isfile = %d, content = '%s', security_category_idx = NULL where idx = %d;"),
                TEXT(request_body.at("classify").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
                request_body.at("isfile").get_ref<const nlohmann::json::boolean_t&>() ? 1 : 0,
                TEXT(request_body.at("content").get_ref<const nlohmann::json::string_t&>().c_str()),
                idx
            );
        }
        else
        {
            result = dbcon.UpdateQuery(TEXT("UPDATE inspection_step set classify = '%s', name = '%s' , description = '%s', isfile = %d, content = '%s', security_category_idx = %d where idx = %d;"),
                TEXT(request_body.at("classify").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
                request_body.at("isfile").get_ref<const nlohmann::json::boolean_t&>() ? 1 : 0,
                TEXT(request_body.at("content").get_ref<const nlohmann::json::string_t&>().c_str()),
                security_category_idx,
                idx
            );
        }

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

void CInspectionApi::DeleteInspectionInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("DeleteInspectionInfo"));
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

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM inspection_step WHERE idx = %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        if (CDatabase::GetRowList(res).size() != 1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "inspection_step Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        bool result = true;

        result = dbcon.DeleteQuery(TEXT("DELETE FROM inspection_step where idx = %d;"), idx);

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

void CInspectionApi::PostInspectionActivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("PostInspectionActivate"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        int idx = -1;
        int device_idx = -1;

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

        if (request.hasParam(":device_idx")) {
            std::string message = request.param(":device_idx").as<std::string>();
            if (std::regex_match(message, regexNumber))
                device_idx = atoi(message.c_str());
            else {
                jsonMessage["message"] = "Error";
                jsonMessage["errors"].push_back({ {"Parameter Errors", "device idx must be number."} });
                error = true;
            }
        }

        if (error)
        {
            response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT socket FROM device WHERE idx = %d;"), device_idx);
        int agentSocket = 0;

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        std::vector<MYSQL_ROW> row = CDatabase::GetRowList(res);

        if (row.size() == 0) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "Device Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }
        else if (row.size() == 1) {
            agentSocket = std::stoi(row[0][0]);
        }

        if (agentSocket == 0) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Device_Error", "Device Not Connected"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        res = dbcon.SelectQuery(TEXT("SELECT JSON_OBJECT('idx', a.idx, 'name', a.name, 'description', a.description, 'update_time', a.update_time, 'content', JSON_EXTRACT(b.content, '$'))\
            FROM inspection a\
            JOIN(\
                SELECT i.idx, CONCAT('[', GROUP_CONCAT(JSON_OBJECT('idx', ist.idx, 'name', ist.name, 'description', ist.description, 'classify', ist.classify, 'main', s.main, 'sub', s.sub)), ']') AS content\
                from inspection i\
                join inspection_step ist\
                ON json_contains(JSON_EXTRACT(i.content, '$[*]'), ist.idx)\
                JOIN security_category s\
                ON s.idx = ist.security_category_idx\
                GROUP BY i.idx\
            )b ON b.idx = a.idx\
            WHERE a.idx = %d;\
            "), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        row = CDatabase::GetRowList(res);
        if (row.size() == 0) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "Policy Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        std::cout << row[0][0] << std::endl;
        //ST_POLICY_INFO policyInfo(atoi(row[0][0]), row[0][1], row[0][3] == NULL ? "" : row[0][3]);
        //std::tstring jsPacketSend;
        //core::WriteJsonToString(&policyInfo, jsPacketSend);

        //MessageManager()->PushSendMessage(agentSocket, INSPECTION_ACTIVATE, row[0][0]);

        jsonMessage["message"] = "Success";
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

void CInspectionApi::GetInspectionDownload(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
}

void CInspectionApi::GetInspectionAvailableDeviceLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("GetInspectionAvailableDeviceLists"));
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

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT JSON_OBJECT('idx', d.idx, 'name', d.name) FROM device_recommand dr JOIN inspection_step p ON p.security_category_idx = dr.security_category_idx join device d ON d.device_category_idx = dr.device_category_idx  WHERE p.idx = %d;"), idx);

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
    catch (nlohmann::json::type_error& ex)
    {
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", ex.what()} });
        response.send(Pistache::Http::Code::Internal_Server_Error, ex.what());
    }
}

void CInspectionApi::GetInspectionAvailablePolicyLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("GetInspectionAvailablePolicyLists"));
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

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT JSON_OBJECT('idx', p.idx, 'classify', p.classify, 'name', p.name, 'description', p.description) FROM inspection_step i\
            JOIN policy p ON i.security_category_idx = p.security_category_idx\
            WHERE i.idx = %d"), idx);

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
    catch (nlohmann::json::type_error& ex)
    {
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", ex.what()} });
        response.send(Pistache::Http::Code::Internal_Server_Error, ex.what());
    }
}

void CInspectionApi::GetInspectionLogLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("GetInspectionLogLists"));

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
        TEXT("SELECT JSON_OBJECT('idx', il.idx, 'device_name', d.name, 'device_environment', d.environment, 'inspection_name', i.name, 'success', IF(il.success = 1, true, false), 'create_time', il.create_time) \
            FROM inspection_log il\
            JOIN device d ON d.idx = il.device_idx\
            JOIN inspection i ON i.idx = il.inspection_idx\
            ORDER BY il.idx ASC LIMIT %d OFFSET %d;"),
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

void CInspectionApi::GetInspectionLogDetail(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CInspectionApi.cpp - [%s]"), TEXT("GetInspectionInfo"));
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
        TEXT("SELECT JSON_OBJECT('idx', il.idx, 'device_name', d.name, 'device_environment', d.environment, 'inspection_name', i.name, 'result', il.result, 'success', IF(il.success = 1, true, false) , 'create_time', il.create_time)\
            FROM inspection_log il\
            JOIN device d ON d.idx = il.device_idx\
            JOIN inspection i ON i.idx = il.inspection_idx\
            WHERE il.idx = %d;"), idx);

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
