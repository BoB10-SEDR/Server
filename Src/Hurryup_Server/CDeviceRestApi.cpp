#include "CDeviceRestApi.h"
#include "Cutils.h"

void CDeviceRestApi::Routing(Pistache::Rest::Router &router)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("Routing Complete"));

    Pistache::Rest::Routes::Get(router, "/v1/devices", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/:idx", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceInfo, this));
    Pistache::Rest::Routes::Post(router, "/v1/devices", Pistache::Rest::Routes::bind(&CDeviceRestApi::PostDeviceInfo, this));
    Pistache::Rest::Routes::Put(router, "/v1/devices/:idx", Pistache::Rest::Routes::bind(&CDeviceRestApi::PutDeviceInfo, this));
    Pistache::Rest::Routes::Delete(router, "/v1/devices/:idx", Pistache::Rest::Routes::bind(&CDeviceRestApi::DeleteDeviceInfo, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/categories", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceCategoryLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/categories/:idx", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceCategoryInfo, this));
    Pistache::Rest::Routes::Post(router, "/v1/devices/categories", Pistache::Rest::Routes::bind(&CDeviceRestApi::PostDeviceCategoryInfo, this));
    Pistache::Rest::Routes::Put(router, "/v1/devices/categories/:idx", Pistache::Rest::Routes::bind(&CDeviceRestApi::PutDeviceCategoryInfo, this));
    Pistache::Rest::Routes::Delete(router, "/v1/devices/categories/:idx", Pistache::Rest::Routes::bind(&CDeviceRestApi::DeleteDeviceCategoryInfo, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/count", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceCount, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/unregistered", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceUnregistedLists, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/:idx/modules", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceHaveModules, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/:idx/statistics", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceStatistics, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/:idx/logs/attack", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceLogsAttack, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/:idx/policies", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceHavePolicy, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/:idx/policies/activate", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceActivatePolicy, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/:idx/policies/inactivate", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceInactivatePolicy, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/:idx/policies/recommand", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceRecommandPolicy, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/:idx/logs", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceLogs, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/:idx/inspection/recommand", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceRecommandInspection, this));
    Pistache::Rest::Routes::Get(router, "/v1/devices/:idx/live", Pistache::Rest::Routes::bind(&CDeviceRestApi::GetDeviceLive, this));
}

void CDeviceRestApi::GetDeviceLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceLists"));
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
        TEXT("SELECT JSON_OBJECT('idx', d.idx, 'category', dc.name, 'name', d.NAME, 'model_number', d.model_number, 'serial_number', serial_number, 'live', IF(d.live = 1,TRUE, FALSE), 'environment', d.environment, 'network_info', JSON_EXTRACT(network_info, '$.network_info'), 'os_info', JSON_EXTRACT(os_info, '$.os_info'), 'service_list', JSON_EXTRACT(service_list, '$.service_list'), 'connect_method', JSON_EXTRACT(connect_method, '$.connect_method') )\
            FROM device d\
            JOIN device_category dc\
            ON d.device_category_idx = dc.idx\
            ORDER BY d.idx ASC LIMIT %d OFFSET %d;\
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

void CDeviceRestApi::GetDeviceInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceInfo"));
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
        TEXT("SELECT JSON_OBJECT('idx', d.idx, 'category', dc.name, 'name', d.NAME, 'model_number', d.model_number, 'serial_number', serial_number, 'live', IF(d.live = 1,TRUE, FALSE), 'environment', d.environment, 'network_info', JSON_EXTRACT(network_info, '$.network_info'), 'os_info', JSON_EXTRACT(os_info, '$.os_info'), 'service_list', JSON_EXTRACT(service_list, '$.service_list'), 'connect_method', JSON_EXTRACT(connect_method, '$.connect_method') )\
            FROM device d\
            JOIN device_category dc\
            ON d.device_category_idx = dc.idx\
            WHERE d.idx = %d;\
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

void CDeviceRestApi::PostDeviceInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("PostDeviceInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        std::vector<std::string> column = { "name", "model_number", "serial_number", "environment", "network_info",  "os_info", "service_list", "connect_method"};

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

        nlohmann::json network_info = request_body.at("network_info");
        nlohmann::json os_info = request_body.at("os_info");
        nlohmann::json service_list = request_body.at("service_list");
        nlohmann::json connect_method = request_body.at("connect_method");

        int idx = -1;
        idx = dbcon.InsertQuery(TEXT("INSERT INTO `device` (`name`, `model_number`, `serial_number`, `device_category_idx`, `environment`, `network_info`, `os_info`, `service_list`, `connect_method`, `update_time`)\
		VALUES('%s', '%s', '%s', (SELECT device_model_category.device_category_idx\
			FROM device_model_category\
			WHERE device_model_category.model_number = '%s'), '%s', '%s', '%s', '%s', '%s', '%s')\
		ON DUPLICATE KEY UPDATE `update_time` = '%s'"),
            TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("model_number").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("serial_number").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("model_number").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("environment").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(network_info.dump().c_str()),
            TEXT(os_info.dump().c_str()),
            TEXT(service_list.dump().c_str()),
            TEXT(connect_method.dump().c_str()),
            TEXT(Cutils::GetTimeStamp().c_str()),
            TEXT(Cutils::GetTimeStamp().c_str())
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

void CDeviceRestApi::PutDeviceInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("PutDeviceInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        int idx = -1;
        std::vector<std::string> column = { "name", "model_number", "serial_number", "environment", "network_info",  "os_info", "service_list", "connect_method" };

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

        nlohmann::json network_info = request_body.at("network_info");
        nlohmann::json os_info = request_body.at("os_info");
        nlohmann::json service_list = request_body.at("service_list");
        nlohmann::json connect_method = request_body.at("connect_method");

        bool result = dbcon.UpdateQuery(TEXT("UPDATE device set name = '%s' , model_number = '%s', serial_number = '%s', device_category_idx = (SELECT device_model_category.device_category_idx\
			FROM device_model_category\
			WHERE device_model_category.model_number = '%s'), environment = '%s', network_info = '%s', os_info = '%s', service_list = '%s', connect_method = '%s', update_time = '%s'\
            where idx = %d;"),
            TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("model_number").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("serial_number").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("model_number").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("environment").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(network_info.dump().c_str()),
            TEXT(os_info.dump().c_str()),
            TEXT(service_list.dump().c_str()),
            TEXT(connect_method.dump().c_str()),
            TEXT(Cutils::GetTimeStamp().c_str()),
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

void CDeviceRestApi::DeleteDeviceInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("DeleteDeviceInfo"));
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

        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM device WHERE idx = %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        if (CDatabase::GetRowList(res).size() != 1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "device Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        bool result = true;

        result = dbcon.DeleteQuery(TEXT("DELETE FROM device where idx = %d;"), idx);

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

void CDeviceRestApi::GetDeviceCategoryLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceCategoryLists"));
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

    MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT JSON_OBJECT('idx', dc.idx, 'name', dc.name, 'model_number', dmc.model_number)\
            FROM device_category dc\
            JOIN device_model_category dmc\
            ON dc.idx = dmc.device_category_idx\
            ORDER BY idx ASC LIMIT %d OFFSET %d;"),
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

void CDeviceRestApi::GetDeviceCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
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

    MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT JSON_OBJECT('idx', dc.idx, 'name', dc.name, 'model_number', dmc.model_number)\
            FROM device_category dc\
            JOIN device_model_category dmc\
            ON dc.idx = dmc.device_category_idx\
            WHERE idx = %d;"), idx);

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

void CDeviceRestApi::PostDeviceCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("PostDeviceCategoryInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        std::vector<std::string> column = { "name", "model_number"};

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

        int idx = dbcon.InsertQuery(TEXT("INSERT device_category(NAME) VALUE('%s') ON DUPLICATE KEY UPDATE idx = idx"),
            TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str())
        );

        if (idx == -1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        idx = dbcon.InsertQuery(TEXT("INSERT device_model_category(device_category_idx, model_number)\
            VALUE((SELECT idx FROM device_category WHERE NAME = '%s'), '%s')\
            ON DUPLICATE KEY UPDATE device_category_idx = device_category_idx"),
            TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
            TEXT(request_body.at("model_number").get_ref<const nlohmann::json::string_t&>().c_str())
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

void CDeviceRestApi::PutDeviceCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("PutDeviceCategoryInfo"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    try {
        nlohmann::json request_body = nlohmann::json::parse(request.body());
        nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

        bool error = false;
        int idx = -1;
        std::vector<std::string> column = { "name", "model_number" };

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

        bool result = dbcon.InsertQuery(TEXT("UPDATE device_category SET NAME='%s' WHERE idx=%d"),
            TEXT(request_body.at("name").get_ref<const nlohmann::json::string_t&>().c_str()),
            idx
        );

        if (!result) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        //TODO 카테고리 모델 번호 기능 추가 필요, API 분활 필요한 듯

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

void CDeviceRestApi::DeleteDeviceCategoryInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("DeleteDeviceCategoryInfo"));
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

        //TODO : device_model_category 테이블에 제약성을 활용하여 제거하기
        MYSQL_RES* res = dbcon.SelectQuery(TEXT("SELECT idx FROM device_category WHERE idx = %d;"), idx);

        if (res == NULL) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
            response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        if (CDatabase::GetRowList(res).size() != 1) {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Not_Found", "device_category Is Not Exisit"} });
            response.send(Pistache::Http::Code::Not_Found, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
            return;
        }

        bool result = true;

        result = dbcon.DeleteQuery(TEXT("DELETE FROM device_category where idx = %d;"), idx);

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

void CDeviceRestApi::GetDeviceCount(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
}

void CDeviceRestApi::GetDeviceUnregistedLists(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceUnregistedLists"));
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
        TEXT("SELECT JSON_OBJECT('idx', d.idx, 'name', d.NAME, 'model_number', d.model_number, 'serial_number', serial_number, 'live', IF(d.live = 1,TRUE, FALSE), 'environment', d.environment, 'network_info', JSON_EXTRACT(network_info, '$.network_info'), 'os_info', JSON_EXTRACT(os_info, '$.os_info'), 'service_list', JSON_EXTRACT(service_list, '$.service_list'), 'connect_method', JSON_EXTRACT(connect_method, '$.connect_method'))\
            FROM device d\
            WHERE d.device_category_idx IS NULL\
            ORDER BY d.idx ASC LIMIT %d OFFSET %d;"),
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

void CDeviceRestApi::GetDeviceHaveModules(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceHaveModules"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    bool error = false;
    int page = 1;
    int limit = 20;
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
        TEXT("SELECT JSON_OBJECT('idx', m.idx, 'category', mc.name, 'model_number', m.model_number, 'serial_number', m.serial_number, 'mac', m.mac, 'update_time', m.update_time, 'network_category', nc.name)\
            FROM module m\
            JOIN module_category mc\
            ON m.module_category_idx = mc.idx\
            JOIN network_category nc\
            ON m.network_category_idx = nc.idx\
            WHERE device_idx = %d\
            ORDER BY m.idx ASC LIMIT %d OFFSET %d;"),
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

void CDeviceRestApi::GetDeviceStatistics(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDashBoardRestApi.cpp - [%s]"), TEXT("GetDeviceStatistics"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    std::string end = Cutils::GetTimeStamp();
    std::string start = "1970-01-01";

    int time = 5;
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

    if (request.query().has("start")) {
        std::string message = request.query().get("start").value();
        if (std::regex_match(message, regexDate))
            start = message;
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "start must be Date Type."} });
            error = true;
        }
    }

    if (request.query().has("time")) {
        std::string message = request.query().get("time").value();
        if (std::regex_match(message, regexNumber))
            time = atoi(message.c_str());
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "time must be Number"} });
            error = true;
        }
    }

    if (error) {
        response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    std::map<std::tstring, std::tstring> status = { {"INFO", "info"},{"THREAT", "threat"}, {"FAIL", "fail"} };

    // 현재시간 까지 로그 통계 구하기
    MYSQL_RES* res = dbcon.SelectQuery(
        TEXT("SELECT status, Count(*) AS count\
                FROM log\
                WHERE TIMESTAMP('%s') <= TIMESTAMP(create_time) AND TIMESTAMP(create_time) <= TIMESTAMPADD(MINUTE, %d, '%s') AND device_idx = %d\
                GROUP BY status\
                ORDER BY STATUS ASC;"),
        start.c_str(), 0, end.c_str(), idx);

    if (res == NULL) {
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
        response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    std::vector<MYSQL_ROW> rows = CDatabase::GetRowList(res);

    nlohmann::json result;
    int total = 0;

    for (auto i : rows) {
        result[status[i[0]]] = atoi(i[1]);
        total += atoi(i[1]);
    }

    result["total"] = total;

    for (const auto& [key, value] : status) {
        result[value + "_rate"] = result[value].get_ref<const nlohmann::json::number_integer_t&>() * 100 / total;
    }

    // time전 로그 통계 구하기
    res = dbcon.SelectQuery(
        TEXT("SELECT status, Count(*) AS count\
                FROM log\
                WHERE TIMESTAMP('%s') <= TIMESTAMP(create_time) AND TIMESTAMP(create_time) <= TIMESTAMPADD(MINUTE, %d, '%s') AND device_idx = %d\
                GROUP BY status\
                ORDER BY STATUS ASC;"),
        start.c_str(), -time, end.c_str(), idx);

    if (res == NULL) {
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
        response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    rows = CDatabase::GetRowList(res);

    int total_change = 0;

    for (auto i : rows) {
        result[status[i[0]] + "_change"] = result[status[i[0]]].get_ref<const nlohmann::json::number_integer_t&>() - atoi(i[1]);
        total_change += atoi(i[1]);
    }

    result["total_change"] = result["total"].get_ref<const nlohmann::json::number_integer_t&>() - total_change;
    for (const auto& [key, value] : status) {
        result[value + "_change_rate"] = result[value + "_change"].get_ref<const nlohmann::json::number_integer_t&>() * 100 / result[value].get_ref<const nlohmann::json::number_integer_t&>();
    }

    // 공격로그 통계
    res = dbcon.SelectQuery(
        TEXT("SELECT COUNT(*) AS type, SUM(attack) AS total_attack\
        FROM(SELECT security_category_idx, COUNT(*) AS attack\
            FROM log\
            WHERE security_category_idx IS NOT NULL AND TIMESTAMP('%s') <= TIMESTAMP(create_time) AND TIMESTAMP(create_time) <= TIMESTAMPADD(MINUTE, %d, '%s')  AND device_idx = %d\
            GROUP BY security_category_idx\
        )a; "),
        start.c_str(), 0, end.c_str(), idx);

    if (res == NULL) {
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
        response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    rows = CDatabase::GetRowList(res);

    for (auto i : rows) {
        result["attack_type"] = atoi(i[0]);
        result["attack"] = atoi(i[1]);
    }

    result["attack_rate"] = result["attack"].get_ref<const nlohmann::json::number_integer_t&>() * 100 / total;


    // 공격로그 time 시간 전 통계
    res = dbcon.SelectQuery(
        TEXT("SELECT COUNT(*) AS type, SUM(attack) AS total_attack\
        FROM(SELECT security_category_idx, COUNT(*) AS attack\
            FROM log\
            WHERE security_category_idx IS NOT NULL AND TIMESTAMP('%s') <= TIMESTAMP(create_time) AND TIMESTAMP(create_time) <= TIMESTAMPADD(MINUTE, %d, '%s') AND device_idx = %d\
            GROUP BY security_category_idx\
        )a; "),
        start.c_str(), -time, end.c_str(), idx);

    if (res == NULL) {
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
        response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    rows = CDatabase::GetRowList(res);

    for (auto i : rows) {
        result["attack_change"] = result["attack"].get_ref<const nlohmann::json::number_integer_t&>() - atoi(i[1]);
    }

    result["attack_change_rate"] = result["attack_change"].get_ref<const nlohmann::json::number_integer_t&>() * 100 / result["attack"].get_ref<const nlohmann::json::number_integer_t&>();

    jsonMessage["outputs"].push_back(result);

    jsonMessage["message"] = "Success";
    response.send(Pistache::Http::Code::Ok, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
}

void CDeviceRestApi::GetDeviceLogsAttack(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceLogsAttack"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    std::string start = "1970-01-01";
    std::string end = Cutils::GetTimeStamp();

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

    if (request.query().has("start")) {
        std::string message = request.query().get("start").value();
        if (std::regex_match(message, regexDate))
            start = message;
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "start must be Date Type."} });
            error = true;
        }
    }

    if (error) {
        response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    std::map<std::tstring, std::tstring> status = { {"INFO", "info"},{"THREAT", "threat"}, {"FAIL", "fail"} };

    // 현재시간 까지 로그 통계 구하기
    MYSQL_RES* res = dbcon.SelectQuery(
        TEXT("SELECT JSON_OBJECT('main', a.main, 'sub', a.sub , 'count', a.log_count , 'percent', a.log_percent * 100)\
            FROM\
            (SELECT sc.main, sc.sub, COUNT(*) AS log_count, COUNT(*) / (SELECT COUNT(*) FROM log WHERE security_category_idx IS NOT NULL) AS log_percent\
                FROM log l\
                JOIN security_category sc ON sc.idx = l.security_category_idx\
                WHERE DATE('%s') <= DATE(create_time) AND DATE(create_time) <= DATE('%s') AND l.device_idx = %d\
                GROUP BY security_category_idx\
            )a"),
        start.c_str(), end.c_str(), idx);

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

void CDeviceRestApi::GetDeviceHavePolicy(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceHavePolicy"));
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

    std::map<std::tstring, std::tstring> status = { {"INFO", "info"},{"THREAT", "threat"}, {"FAIL", "fail"} };

    // 현재시간 까지 로그 통계 구하기
    MYSQL_RES* res = dbcon.SelectQuery(
        TEXT("SELECT JSON_OBJECT('idx', p.idx, 'main', sc.main, 'sub', sc.sub, 'classify', p.classify, 'name', p.name, 'description', p.description)\
            FROM device_policy dp\
            JOIN policy p\
            ON dp.policy_idx = p.idx\
            JOIN security_category sc\
            ON sc.idx = p.security_category_idx\
            WHERE dp.device_idx = %d"), idx);

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

void CDeviceRestApi::GetDeviceActivatePolicy(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceActivatePolicy"));
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
        TEXT("SELECT JSON_OBJECT('idx', p.idx, 'main', sc.main, 'sub', sc.sub, 'classify', p.classify, 'name', p.name, 'description', p.description)\
            FROM device_policy dp\
            JOIN policy p\
            ON dp.policy_idx = p.idx\
            JOIN security_category sc\
            ON sc.idx = p.security_category_idx\
            WHERE dp.device_idx = %d AND dp.activate = 1"), idx);

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

void CDeviceRestApi::GetDeviceInactivatePolicy(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceInactivatePolicy"));
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
        TEXT("SELECT JSON_OBJECT('idx', p.idx, 'main', sc.main, 'sub', sc.sub, 'classify', p.classify, 'name', p.name, 'description', p.description)\
            FROM device_policy dp\
            JOIN policy p\
            ON dp.policy_idx = p.idx\
            JOIN security_category sc\
            ON sc.idx = p.security_category_idx\
            WHERE dp.device_idx = %d AND dp.activate = 0"), idx);

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

void CDeviceRestApi::GetDeviceRecommandPolicy(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceRecommandPolicy"));
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
        TEXT("SELECT  JSON_OBJECT('idx', p.idx, 'main', sc.main, 'sub', sc.sub, 'classify', p.classify, 'name', p.name, 'description', p.description)\
            FROM device_recommand dr\
            JOIN device d\
            ON d.device_category_idx = dr.device_category_idx\
            JOIN policy p\
            ON p.idx = dr.security_category_idx\
            JOIN security_category sc\
            ON p.security_category_idx = sc.idx\
            WHERE d.idx = %d"), idx);

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

void CDeviceRestApi::GetDeviceLogs(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceLogs"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    bool error = false;
    int page = 1;
    int limit = 20;
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
        TEXT("SELECT JSON_OBJECT('event_code', l.event_code, 'description', description, 'environment', l.environment , 'status', l.STATUS, 'main', sc.main, 'sub', sc.sub, 'original_log', JSON_EXTRACT(original_log, '$'),'layer', l.layer, 'create_time', l.create_time )\
            FROM log l\
            LEFT OUTER JOIN security_category sc\
            ON sc.idx = l.security_category_idx\
            WHERE device_idx = %d\
            ORDER BY l.idx ASC LIMIT %d OFFSET %d;\
            "),
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

void CDeviceRestApi::GetDeviceRecommandInspection(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDeviceRestApi.cpp - [%s]"), TEXT("GetDeviceRecommandInspection"));
    response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");

    bool error = false;
    int page = 1;
    int limit = 20;
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
        TEXT("SELECT  JSON_OBJECT('idx', p.idx, 'main', sc.main, 'sub', sc.sub, 'classify', p.classify, 'name', p.name, 'description', p.description)\
            FROM device_recommand dr\
            JOIN device d\
            ON d.device_category_idx = dr.device_category_idx\
            JOIN inspection_step p\
            ON p.idx = dr.security_category_idx\
            JOIN security_category sc\
            ON p.security_category_idx = sc.idx\
            WHERE d.idx = %d"), idx);

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

void CDeviceRestApi::GetDeviceLive(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{

}
