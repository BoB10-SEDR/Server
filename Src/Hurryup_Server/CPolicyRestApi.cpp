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
    Pistache::Rest::Routes::Get(router, "/v1/policies/:idx/activate", Pistache::Rest::Routes::bind(&CPolicyRestApi::GetPolicyActivateDeviceLists, this));
}

void CPolicyRestApi::GetPolicyLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("GetPolicyLists"));
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
            jsonMessage["errors"].push_back({ {"Parameter Errors", "limit must be 2 digits number."} });
            error = true;
        }
    }

    if (error) {
        response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    MYSQL_RES* res = dbcon.SelectQuery(
        TEXT("SELECT JSON_OBJECT('idx', p.idx, 'main', main, 'sub', sub, 'classify', classify, 'name',NAME, 'description', description) FROM policy p LEFT JOIN security_category s ON p.security_category_idx = s.idx ORDER BY p.idx ASC LIMIT %d OFFSET %d;"),
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
        TEXT("SELECT JSON_OBJECT('idx', p.idx, 'main', main, 'sub', sub, 'classify', classify, 'name',NAME, 'description', description, 'isfile', IF(isfile = 1, true, false), 'apply_content', apply_content, 'release_content', release_content, 'argument', JSON_EXTRACT(argument, '$'), 'command', command) FROM policy p LEFT JOIN security_category s ON p.security_category_idx = s.idx WHERE p.idx = %d;"),
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
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        //TODO :: argument 자료형 검증 필요

        bool error = false;
        std::vector<std::string> column = { "main", "sub", "classify", "name", "description", "isfile", "apply_content", "release_content", "argument", "command"};
        
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

        //TODO :: 배열을 문자열로 만드는 공통 메소드 구현 필요

        auto argument = request_body.at("argument").get_ref<const nlohmann::json::array_t&>();
        std::ostringstream strArgument;
        strArgument << "[";
        if (!argument.empty())
        {
            std::copy(argument.begin(), argument.end() - 1, std::ostream_iterator<const nlohmann::json::object_t>(strArgument, ","));
            strArgument << argument.back();
        }
        strArgument << "]";

        int idx = dbcon.InsertQuery(TEXT("INSERT INTO policy(classify, name, description, isfile, apply_content, release_content, security_category_idx, argument, command)\
                                    values('%s','%s','%s',%d,'%s','%s', (SELECT idx FROM security_category where main = '%s' and sub = '%s'), '%s', '%s');"),
            TEXT(request_body.at("classify").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
            request_body.at("isfile").get_ref<const nlohmann::json::boolean_t&>() ? 1 : 0,
            TEXT(request_body.at("apply_content").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("release_content").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("main").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("sub").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(strArgument.str().c_str()),
            TEXT(request_body.at("command").get_ref<const nlohmann::json::string_t&>().c_str())
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

void CPolicyRestApi::PutPolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("PutPolicyInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        int idx = -1;

        std::vector<std::string> column = { "main", "sub", "classify", "name", "description", "isfile", "apply_content", "release_content", "argument", "command" };

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

        auto argument = request_body.at("argument").get_ref<const nlohmann::json::array_t&>();
        std::ostringstream strArgument;
        strArgument << "[";
        if (!argument.empty())
        {
            std::copy(argument.begin(), argument.end() - 1, std::ostream_iterator<const nlohmann::json::object_t>(strArgument, ","));
            strArgument << argument.back();
        }
        strArgument << "]";

        bool result = dbcon.UpdateQuery(TEXT("UPDATE policy set classify = '%s', name = '%s' , description = '%s', isfile = %d, apply_content = '%s', release_content = '%s', security_category_idx = (SELECT idx FROM security_category where main='%s' and sub='%s'), argument = '%s', command = '%s' where idx = %d;"),
                TEXT(request_body.at("classify").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("description").get_ref<const nlohmann::json::string_t&>().c_str()),
                request_body.at("isfile").get_ref<const nlohmann::json::boolean_t&>() ? 1 : 0,
                TEXT(request_body.at("apply_content").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("release_content").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("main").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(request_body.at("sub").get_ref<const nlohmann::json::string_t&>().c_str()),
                TEXT(strArgument.str().c_str()),
                TEXT(request_body.at("command").get_ref<const nlohmann::json::string_t&>().c_str()),
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

void CPolicyRestApi::DeletePolicyInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
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
        response.send(Pistache::Http::Code::Internal_Server_Error, ex.what());
    }
}

void CPolicyRestApi::PostPolicyActivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("PostPolicyActivate"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        int idx = -1;
        int device_idx = -1;

        std::vector<std::string> column = { "argument", "command" };

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

        res = dbcon.SelectQuery(TEXT("SELECT idx, name, isfile, apply_content, argument, command FROM policy where idx= %d;"), idx);

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

        std::tstring argument = row[0][4];
        std::tstring command = row[0][5];

        if (request_body.count("argument")) {
            auto tmp = request_body.at("argument").get_ref<const nlohmann::json::array_t&>();
            std::ostringstream strArgument;
            strArgument << "[";
            if (!tmp.empty())
            {
                std::copy(tmp.begin(), tmp.end() - 1, std::ostream_iterator<const nlohmann::json::object_t>(strArgument, ","));
                strArgument << tmp.back();
            }
            strArgument << "]";

            argument = strArgument.str();
        }

        if (request_body.count("command"))
            command = request_body.at("command").get_ref<const nlohmann::json::string_t&>().c_str();

        std::cout << argument << std::endl;
        std::cout << command << std::endl;

        ST_POLICY_INFO policyInfo(atoi(row[0][0]), row[0][1], row[0][3] == NULL ? "" : row[0][3]);
        std::tstring jsPacketSend;
        core::WriteJsonToString(&policyInfo, jsPacketSend);

        MessageManager()->PushSendMessage(agentSocket, POLICY_INACTIVATE, jsPacketSend);

        std::cout << jsPacketSend << std::endl;

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

void CPolicyRestApi::PostPolicyInactivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("PostPolicyInactivate"));
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

        res = dbcon.SelectQuery(TEXT("SELECT idx, name, isfile, release_content FROM policy where idx= %d;"), idx);

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

        ST_POLICY_INFO policyInfo(atoi(row[0][0]), row[0][1], row[0][3] == NULL ? "" : row[0][3]);
        std::tstring jsPacketSend;
        core::WriteJsonToString(&policyInfo, jsPacketSend);

        MessageManager()->PushSendMessage(agentSocket, POLICY_ACTIVATE, jsPacketSend);

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

void CPolicyRestApi::GetPolicyDownload(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    
}

void CPolicyRestApi::GetPolicyAvailableDeviceLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("GetPolicyAvailableDeviceLists"));
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

        MYSQL_RES * res = dbcon.SelectQuery(TEXT("SELECT JSON_OBJECT('idx', d.idx, 'name', d.name) FROM device_recommand dr JOIN policy p ON p.security_category_idx = dr.security_category_idx join device d ON d.device_category_idx = dr.device_category_idx  WHERE p.idx = %d;"), idx);

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

void CPolicyRestApi::GetPolicyActivateDeviceLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("GetPolicyActivateDeviceLists"));
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

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT JSON_OBJECT('idx', d.idx, 'name', d.name, 'model_number', d.model_number, 'serial_number', d.serial_number)\
            FROM policy p\
            JOIN device_policy dp ON dp.policy_idx = p.idx\
            JOIN device d ON dp.device_idx = d.idx\
            WHERE p.idx = %d"), idx);

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
