#include "CPolicyRestApi.h"

void CPolicyRestApi::Routing(Pistache::Rest::Router& router)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("Routing Complete"));

    Pistache::Rest::Routes::Get(router, "/v1/policies", Pistache::Rest::Routes::bind(&CPolicyRestApi::GetPolicyLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/policies/:idx", Pistache::Rest::Routes::bind(&CPolicyRestApi::GetPolicyInfo, this));
    Pistache::Rest::Routes::Post(router, "/v1/policies", Pistache::Rest::Routes::bind(&CPolicyRestApi::PostPolicyInfo, this));
    Pistache::Rest::Routes::Put(router, "/v1/policies/:idx", Pistache::Rest::Routes::bind(&CPolicyRestApi::PutPolicyInfo, this));
    Pistache::Rest::Routes::Delete(router, "/v1/policies/:idx", Pistache::Rest::Routes::bind(&CPolicyRestApi::DeletePolicyInfo, this));
    Pistache::Rest::Routes::Post(router, "/v1/policies/:idx/activate/:device_idx", Pistache::Rest::Routes::bind(&CPolicyRestApi::PostPolicyActivate, this));
    Pistache::Rest::Routes::Post(router, "/v1/policies/:idx/inactivate/:device_idx", Pistache::Rest::Routes::bind(&CPolicyRestApi::PostPolicyInactivate, this));
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
        if (std::regex_match(message, regexNumber))
            page = atoi(message.c_str());
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({{"Parameter Errors", "page must be number."}});
            error = true;
        }
    }

    if (request.query().has("limit")) {
        std::string message = request.query().get("limit").value();
        if (std::regex_match(message, regexLimit))
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

    std::vector<MYSQL_ROW> rows = CDatabase::GetRowList(res);

    for (auto i : rows) {
        jsonMessage["outputs"].push_back(nlohmann::json::parse(i[0]));
    }

    jsonMessage["message"] = "Success";
    response.send(Pistache::Http::Code::Ok, jsonMessage.dump(),Pistache::Http::Mime::MediaType::fromString("application/json"));
}

void CPolicyRestApi::GetPolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("GetPolicyInfo"));

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
        TEXT("SELECT JSON_OBJECT('idx', p.idx, 'main', main, 'sub', sub, 'classify', classify, 'name',NAME, 'description', description, 'isfile', IF(isfile = 1, true, false), 'apply_content', apply_content, 'release_content', release_content) FROM policy p JOIN security_category s ON p.security_category_idx = s.idx WHERE p.idx = %d;"),
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

void CPolicyRestApi::PostPolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("PostPolicyInfo"));

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        std::vector<std::string> column = { "main", "sub", "classify", "name", "description", "isfile", "apply_content", "release_content" };
        
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
            jsonMessage["errors"].push_back({ {"Parameter Type Error.", "isfile type must be boolean"}});
        }

        if (error)
        {
            response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }     

        MYSQL_RES * res = dbcon.SelectQuery(TEXT("SELECT idx FROM security_category where main='%s' and sub='%s';"),
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
            idx = dbcon.InsertQuery(TEXT("INSERT INTO policy(classify, name, description, isfile, apply_content, release_content, security_category_idx) values('%s','%s','%s',%d,'%s','%s', NULL);"),
                TEXT(request_body.at("classify").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
                request_body.at("isfile").get_ref<const nlohmann::json::boolean_t&>() ? 1 : 0,
                TEXT(request_body.at("apply_content").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("release_content").get_ref<const nlohmann::json::string_t&>().c_str())
            );
        }
        else 
        {
            idx = dbcon.InsertQuery(TEXT("INSERT INTO policy(classify, name, description, isfile, apply_content, release_content, security_category_idx) values('%s','%s','%s',%d,'%s','%s',%d);"),
                TEXT(request_body.at("classify").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
                request_body.at("isfile").get_ref<const nlohmann::json::boolean_t&>() ? 1 : 0,
                TEXT(request_body.at("apply_content").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("release_content").get_ref<const nlohmann::json::string_t&>().c_str()),
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
        response.send(Pistache::Http::Code::Ok, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json")); 
    }
    catch (nlohmann::json::type_error& ex)
    {
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", ex.what()} });
        response.send(Pistache::Http::Code::Ok, ex.what());
    }
}

void CPolicyRestApi::PutPolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("PutPolicyInfo"));

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        int idx = -1;

        std::vector<std::string> column = { "main", "sub", "classify", "name", "description", "isfile", "apply_content", "release_content" };

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

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM policy WHERE idx = %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        if (CDatabase::GetRowList(res).size() != 1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "Policy Is Not Exisit"} });
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
            result = dbcon.UpdateQuery(TEXT("UPDATE policy set classify = '%s', name = '%s' , description = '%s', isfile = %d, apply_content = '%s', release_content = '%s', security_category_idx = NULL where idx = %d;"),
                TEXT(request_body.at("classify").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
                request_body.at("isfile").get_ref<const nlohmann::json::boolean_t&>() ? 1 : 0,
                TEXT(request_body.at("apply_content").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("release_content").get_ref<const nlohmann::json::string_t&>().c_str()),
                idx
            );
        }
        else
        {
            result = dbcon.UpdateQuery(TEXT("UPDATE policy set classify = '%s', name = '%s' , description = '%s', isfile = %d, apply_content = '%s', release_content = '%s', security_category_idx = %d where idx = %d;"),
                TEXT(request_body.at("classify").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
                request_body.at("isfile").get_ref<const nlohmann::json::boolean_t&>() ? 1 : 0,
                TEXT(request_body.at("apply_content").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("release_content").get_ref<const nlohmann::json::string_t&>().c_str()),
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
        response.send(Pistache::Http::Code::Ok, ex.what());
    }
}

void CPolicyRestApi::DeletePolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("DeletePolicyInfo"));

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

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM policy WHERE idx = %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        if (CDatabase::GetRowList(res).size() != 1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "Policy Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        bool result = true;

        result = dbcon.DeleteQuery(TEXT("DELETE FROM policy where idx = %d;"), idx);

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
        response.send(Pistache::Http::Code::Ok, ex.what());
    }
}

void CPolicyRestApi::PostPolicyActivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("PostPolicyActivate"));

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

        res = dbcon.SelectQuery(TEXT("SELECT idx, name, isfile, apply_content FROM policy where idx= %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        bool result = true;

        row = CDatabase::GetRowList(res);
        if (row.size() == 0) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "Policy Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        ST_POLICY_INFO policyInfo(atoi(row[0][0]), row[0][1], row[0][3] == NULL ? "" : row[0][3]);
        std::tstring jsPacketSend;
        core::WriteJsonToString(&policyInfo, jsPacketSend);

        MessageManager()->PushSendMessage(agentSocket, REQUEST, "/monitoring/activate", jsPacketSend);

        std::cout << jsPacketSend << std::endl;

        jsonMessage["message"] = "Success";
        response.send(Pistache::Http::Code::Ok, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
    }
    catch (nlohmann::json::type_error& ex)
    {
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", ex.what()} });
        response.send(Pistache::Http::Code::Ok, ex.what());
    }
}

void CPolicyRestApi::PostPolicyInactivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("PostPolicyInactivate"));

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

        res = dbcon.SelectQuery(TEXT("SELECT idx, name, isfile, release_content FROM policy where idx= %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        bool result = true;

        row = CDatabase::GetRowList(res);
        if (row.size() == 0) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "Policy Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        ST_POLICY_INFO policyInfo(atoi(row[0][0]), row[0][1], row[0][3] == NULL ? "" : row[0][3]);
        std::tstring jsPacketSend;
        core::WriteJsonToString(&policyInfo, jsPacketSend);

        MessageManager()->PushSendMessage(agentSocket, REQUEST, "/monitoring/inactivate", jsPacketSend);

        std::cout << jsPacketSend << std::endl;

        jsonMessage["message"] = "Success";
        response.send(Pistache::Http::Code::Ok, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
    }
    catch (nlohmann::json::type_error& ex)
    {
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", ex.what()} });
        response.send(Pistache::Http::Code::Ok, ex.what());
    }
}

void CPolicyRestApi::GetPolicyDownload(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{

}

void CPolicyRestApi::GetPolicyAvailableDeviceLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
}

