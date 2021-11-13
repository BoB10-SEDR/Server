#include "CMonitoringRestApi.h"
#include "Cutils.h"

void CMonitoringRestApi::Routing(Pistache::Rest::Router& router)
{
    core::Log_Debug(TEXT("CMonitoringRestApi.cpp - [%s]"), TEXT("Routing Complete"));

    Pistache::Rest::Routes::Get(router, "/v1/monitoring/:idx/process", Pistache::Rest::Routes::bind(&CMonitoringRestApi::GetProcessLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/monitoring/:idx/process/:process/filedescriptor", Pistache::Rest::Routes::bind(&CMonitoringRestApi::GetFileDescriptorLists, this));
    Pistache::Rest::Routes::Post(router, "/v1/monitoring/:idx/activate", Pistache::Rest::Routes::bind(&CMonitoringRestApi::PostMonitoringActivate, this));
    Pistache::Rest::Routes::Post(router, "/v1/monitoring/:idx/inactivate", Pistache::Rest::Routes::bind(&CMonitoringRestApi::PostMonitoringInactivate, this));
}

void CMonitoringRestApi::GetProcessLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CMonitoringRestApi.cpp - [%s]"), TEXT("GetProcessLists"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    bool error = false;
    int idx = -1;
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
        TEXT("SELECT JSON_OBJECT('idx', idx, 'pid', pid, 'ppid', ppid, 'state', state, 'command', command, 'start_time', start_time, 'update_time', update_time)\
            FROM process WHERE device_idx = %d\
            ORDER BY idx ASC LIMIT %d OFFSET %d;"),
        idx, limit, limit * (page - 1));

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

void CMonitoringRestApi::GetFileDescriptorLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CMonitoringRestApi.cpp - [%s]"), TEXT("GetProcessLists"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    bool error = false;
    int idx = -1;
    int process = -1;
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

    if (request.hasParam(":process")) {
        std::string message = request.param(":process").as<std::string>();
        if (std::regex_match(message, regexNumber))
            process = atoi(message.c_str());
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
        TEXT("SELECT JSON_OBJECT('idx', idx, 'pid', pid,  'name', name, 'path', path)\
            FROM file_descriptor WHERE device_idx = %d AND pid = %d\
            ORDER BY idx ASC LIMIT %d OFFSET %d;"),
            idx, process, limit, limit * (page - 1));

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

void CMonitoringRestApi::PostMonitoringActivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("PostPolicyActivate"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
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

        std::vector<std::string> column = { "path" ,"process_name"};

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

        std::tstring path = request_body.at("path").get_ref<const nlohmann::json::string_t&>();
        std::tstring processName = request_body.at("process_name").get_ref<const nlohmann::json::string_t&>();

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT socket FROM device WHERE idx = %d;"), idx);
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

        int monitoringIdx = dbcon.InsertQuery(TEXT("INSERT INTO monitoring(process_name, log_path, activate, device_idx, update_time) Values('%s', '%s', %d, %d, '%s') ON DUPLICATE KEY UPDATE activate = %d, update_time = '%s';"),
            TEXT(processName.c_str()), TEXT(path.c_str()), 1, idx, TEXT(Cutils::GetTimeStamp().c_str()), 1, TEXT(Cutils::GetTimeStamp().c_str()));

        if (monitoringIdx == -1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        //ST_POLICY_INFO policyInfo(atoi(row[0][0]), row[0][1], row[0][3] == NULL ? "" : row[0][3]);
        //std::tstring jsPacketSend;
        //core::WriteJsonToString(&policyInfo, jsPacketSend);

        //MessageManager()->PushSendMessage(agentSocket, REQUEST, "/monitoring/activate", jsPacketSend);

        //std::cout << jsPacketSend << std::endl;

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

void CMonitoringRestApi::PostMonitoringInactivate(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CPolicyRestApi.cpp - [%s]"), TEXT("PostPolicyActivate"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
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

        std::vector<std::string> column = { "path" ,"process_name" };

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

        std::tstring path = request_body.at("path").get_ref<const nlohmann::json::string_t&>();
        std::tstring processName = request_body.at("process_name").get_ref<const nlohmann::json::string_t&>();

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT socket FROM device WHERE idx = %d;"), idx);
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

        int monitoringIdx = dbcon.InsertQuery(TEXT("INSERT INTO monitoring(process_name, log_path, activate, device_idx, update_time) Values('%s', '%s', %d, %d, '%s') ON DUPLICATE KEY UPDATE activate = %d, update_time = '%s';"),
            TEXT(processName.c_str()), TEXT(path.c_str()), 0, idx, TEXT(Cutils::GetTimeStamp().c_str()), 0, TEXT(Cutils::GetTimeStamp().c_str()));

        if (monitoringIdx == -1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        //ST_POLICY_INFO policyInfo(atoi(row[0][0]), row[0][1], row[0][3] == NULL ? "" : row[0][3]);
        //std::tstring jsPacketSend;
        //core::WriteJsonToString(&policyInfo, jsPacketSend);

        //MessageManager()->PushSendMessage(agentSocket, REQUEST, "/monitoring/activate", jsPacketSend);

        //std::cout << jsPacketSend << std::endl;

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
