#include "CDashBoardRestApi.h"

std::string CDashBoardRestApi::GetTimeStamp()
{
    time_t curTime = time(NULL);
    char timestamp[20];
    struct tm* a = localtime(&curTime);

    sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + a->tm_year, a->tm_mon + 1, a->tm_mday, a->tm_hour, a->tm_min, a->tm_sec);
    return timestamp;
}

void CDashBoardRestApi::Routing(Pistache::Rest::Router& router)
{
    core::Log_Debug(TEXT("CDashBoardRestApi.cpp - [%s]"), TEXT("Routing Complete")); 

    Pistache::Rest::Routes::Get(router, "/v1/dashboard/statistics", Pistache::Rest::Routes::bind(&CDashBoardRestApi::GetDashBoardStatisticsRestApi, this));
    Pistache::Rest::Routes::Get(router, "/v1/dashboard/logs", Pistache::Rest::Routes::bind(&CDashBoardRestApi::GetDashBoardLogTimeRestApi, this));
    Pistache::Rest::Routes::Get(router, "/v1/dashboard/logs/attack", Pistache::Rest::Routes::bind(&CDashBoardRestApi::GetDashBoardLogAttackRestApi, this));
    Pistache::Rest::Routes::Get(router, "/v1/dashboard/logs/attack/detail", Pistache::Rest::Routes::bind(&CDashBoardRestApi::GetDashBoardLogAttackTimeRestApi, this));
    Pistache::Rest::Routes::Get(router, "/v1/dashboard/logs/group", Pistache::Rest::Routes::bind(&CDashBoardRestApi::GetDashBoardLogGroupRestApi, this));
    Pistache::Rest::Routes::Get(router, "/v1/dashboard/policies", Pistache::Rest::Routes::bind(&CDashBoardRestApi::GetDashBoardPolicyRestApi, this));
}

void CDashBoardRestApi::GetDashBoardStatisticsRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDashBoardRestApi.cpp - [%s]"), TEXT("GetDashBoardStatisticsRestApi"));

    std::string cur = CDashBoardRestApi::GetTimeStamp();
    std::string start = "1970-01-01";

    int time = 5;
    bool error = false; 

    nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

    if (request.query().has("start")) {
        std::string message = request.query().get("start").value();
        if (std::regex_match(message, regexDate))
            start = message;
        else{
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "start must be Date Type."} });
            error = true;
        }
    }

    if (request.query().has("time")) {
        std::string message = request.query().get("time").value();
        if (std::regex_match(message, regexNumber)) 
            time = atoi(message.c_str());
        else{
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "time must be Number"} });
            error = true;
        }
    }

    if (error) {
        response.send(Pistache::Http::Code::Bad_Request, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    std::map<std::tstring, std::tstring> status = {{"INFO", "info"},{"THREAT", "threat"}, {"FAIL", "fail"}};

    // 현재시간 까지 로그 통계 구하기
    MYSQL_RES* res = dbcon.SelectQuery(
            TEXT("SELECT status, Count(*) AS count\
                FROM log\
                WHERE TIMESTAMP('%s') <= TIMESTAMP(create_time) AND TIMESTAMP(create_time) <= TIMESTAMPADD(MINUTE, %d, '%s')\
                GROUP BY status\
                ORDER BY STATUS ASC;"),
            start.c_str(), 0, cur.c_str());

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
        result[value+"_rate"] = result[value].get_ref<const nlohmann::json::number_integer_t&>() * 100 / total;
    }

    // time전 로그 통계 구하기
    res = dbcon.SelectQuery(
        TEXT("SELECT status, Count(*) AS count\
                FROM log\
                WHERE TIMESTAMP('%s') <= TIMESTAMP(create_time) AND TIMESTAMP(create_time) <= TIMESTAMPADD(MINUTE, %d, '%s')\
                GROUP BY status\
                ORDER BY STATUS ASC;"),
        start.c_str(), -time, cur.c_str());

    if (res == NULL) {
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
        response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    rows = CDatabase::GetRowList(res);

    int total_change = 0;

    for (auto i : rows) {
        result[status[i[0]]+"_change"] = result[status[i[0]]].get_ref<const nlohmann::json::number_integer_t&>() - atoi(i[1]);
        total_change += atoi(i[1]);
    }

    result["total_change"] = result["total"].get_ref<const nlohmann::json::number_integer_t&>() - total_change;
    for (const auto& [key, value] : status) {
        result[value + "_change_rate"] = result[value+"_change"].get_ref<const nlohmann::json::number_integer_t&>() * 100 / result[value].get_ref<const nlohmann::json::number_integer_t&>();
    }

    // 공격로그 통계
    res = dbcon.SelectQuery(
    TEXT("SELECT COUNT(*) AS type, SUM(attack) AS total_attack\
        FROM(SELECT security_category_idx, COUNT(*) AS attack\
            FROM log\
            WHERE security_category_idx IS NOT NULL AND TIMESTAMP('%s') <= TIMESTAMP(create_time) AND TIMESTAMP(create_time) <= TIMESTAMPADD(MINUTE, %d, '%s')\
            GROUP BY security_category_idx\
        )a; "),
        start.c_str(), 0, cur.c_str());

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
            WHERE security_category_idx IS NOT NULL AND TIMESTAMP('%s') <= TIMESTAMP(create_time) AND TIMESTAMP(create_time) <= TIMESTAMPADD(MINUTE, %d, '%s')\
            GROUP BY security_category_idx\
        )a; "),
        start.c_str(), -time, cur.c_str());

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

    //장비 카테고리 수, 장비 수
    res = dbcon.SelectQuery(
        TEXT("SELECT COUNT(*) AS type, SUM(a.count) AS devices\
            FROM(\
                SELECT COUNT(*) AS count, device_category_idx FROM device\
                GROUP BY device_category_idx\
            ) a"));

    if (res == NULL) {
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
        response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    rows = CDatabase::GetRowList(res);

    for (auto i : rows) {
        result["device_type"] = atoi(i[0]);
        result["device"] = atoi(i[1]);
    }

    //모듈 카테고리 수, 모듈 수
    res = dbcon.SelectQuery(
        TEXT("SELECT COUNT(*) AS type, SUM(a.count) AS modules\
            FROM(\
                SELECT COUNT(*) AS count, module_category_idx  FROM module\
                GROUP BY module_category_idx\
            ) a"));

    if (res == NULL) {
        jsonMessage["message"] = "Error";
        jsonMessage["errors"].push_back({ {"Internal Server Errors", "Database Errors"} });
        response.send(Pistache::Http::Code::Internal_Server_Error, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
        return;
    }

    rows = CDatabase::GetRowList(res);

    for (auto i : rows) {
        result["module_type"] = atoi(i[0]);
        result["module"] = atoi(i[1]);
    }

    jsonMessage["outputs"].push_back(result);

    jsonMessage["message"] = "Success";
    response.send(Pistache::Http::Code::Ok, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
}

void CDashBoardRestApi::GetDashBoardLogTimeRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDashBoardRestApi.cpp - [%s]"), TEXT("GetDashBoardStatisticsRestApi"));

    std::string cur = CDashBoardRestApi::GetTimeStamp();
    std::string start = "1970-01-01";
    std::string end = cur;

    int time = 5;
    bool error = false;

    nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

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

    if (request.query().has("end")) {
        std::string message = request.query().get("end").value();
        if (std::regex_match(message, regexDate))
            end = message;
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "end must be Date Type."} });
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
        TEXT("SELECT a.basis_date, CONCAT('[', GROUP_CONCAT(JSON_OBJECT(a.status, a.avg_col)),']'), SUM(a.avg_col) AS SUM\
            FROM(\
                SELECT TIMESTAMPADD(MINUTE, FLOOR(TIMESTAMPDIFF(MINUTE, '%s', create_time) / %d) * %d, '%s')  AS basis_date\
                , Count(*) AS avg_col, status\
                FROM log\
                WHERE DATE('%s') <= DATE(create_time) AND DATE(create_time) <= DATE('%s')\
                GROUP BY TIMESTAMPADD(MINUTE, FLOOR(TIMESTAMPDIFF(MINUTE, '%s', create_time) / %d) * %d, '%s'), STATUS\
            ) a\
            GROUP BY a.basis_date"),
        start.c_str(), time, time, start.c_str(), start.c_str(), end.c_str(), start.c_str(), time, time, start.c_str());

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
        nlohmann::json tmp = nlohmann::json::parse(i[1]);
        for (auto j : tmp) {
            for (auto& [key, value] : j.items()) {
                result[status[key]] = value;
                result[status[key]+"_rate"] = value.get_ref<const nlohmann::json::number_integer_t&>()*100 / atoi(i[2]);
            }
        }
        result["date"] = i[0];
        result["total"] = atoi(i[2]);
        jsonMessage["outputs"].push_back(result);
    }

    jsonMessage["message"] = "Success";
    response.send(Pistache::Http::Code::Ok, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
}

void CDashBoardRestApi::GetDashBoardLogAttackRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDashBoardRestApi.cpp - [%s]"), TEXT("GetDashBoardStatisticsRestApi"));

    std::string cur = CDashBoardRestApi::GetTimeStamp();
    std::string start = "1970-01-01";
    std::string end = cur;

    int time = 5;
    bool error = false;

    nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

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

    if (request.query().has("end")) {
        std::string message = request.query().get("end").value();
        if (std::regex_match(message, regexDate))
            end = message;
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "end must be Date Type."} });
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
        TEXT("SELECT JSON_OBJECT('main', a.main, 'sub', a.sub , 'count', a.log_count , 'percent', a.log_percent * 100)\
            FROM\
            (SELECT sc.main, sc.sub, COUNT(*) AS log_count, COUNT(*) / (SELECT COUNT(*) FROM log WHERE security_category_idx IS NOT NULL) AS log_percent\
                FROM log l\
                JOIN security_category sc ON sc.idx = l.security_category_idx\
                WHERE DATE('%s') <= DATE(create_time) AND DATE(create_time) <= DATE('%s')\
                GROUP BY security_category_idx\
            )a"),
        start.c_str(), end.c_str());

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

void CDashBoardRestApi::GetDashBoardLogAttackTimeRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDashBoardRestApi.cpp - [%s]"), TEXT("GetDashBoardStatisticsRestApi"));

    std::string cur = CDashBoardRestApi::GetTimeStamp();
    std::string start = "1970-01-01";
    std::string end = cur;

    int time = 5;
    bool error = false;

    nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

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

    if (request.query().has("end")) {
        std::string message = request.query().get("end").value();
        if (std::regex_match(message, regexDate))
            end = message;
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "end must be Date Type."} });
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
        TEXT("SELECT a.basis_date, CONCAT('[', GROUP_CONCAT(JSON_OBJECT('main', s.main, 'sub', s.sub, 'count', a.col)) ,']'), SUM(a.col) AS SUM\
            FROM(\
                SELECT TIMESTAMPADD(MINUTE, FLOOR(TIMESTAMPDIFF(MINUTE, '%s', create_time) / %d) * %d, '%s')  AS basis_date\
                , Count(*) AS col, security_category_idx\
                FROM log\
                WHERE DATE('%s') <= DATE(create_time) AND DATE(create_time) <= DATE('%s')\
                GROUP BY TIMESTAMPADD(MINUTE, FLOOR(TIMESTAMPDIFF(MINUTE, '%s', create_time) / %d) * %d, '%s'), security_category_idx\
            ) a\
            JOIN security_category s\
            ON a.security_category_idx = s.idx\
            GROUP BY a.basis_date\
            "),
        start.c_str(), time, time, start.c_str(), start.c_str(), end.c_str(), start.c_str(), time, time, start.c_str());

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
        result["date"] = i[0];
        result["data"] = nlohmann::json::parse(i[1]);
        result["total"] = atoi(i[2]);
        jsonMessage["outputs"].push_back(result);
    }

    jsonMessage["message"] = "Success";
    response.send(Pistache::Http::Code::Ok, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
}

void CDashBoardRestApi::GetDashBoardLogGroupRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDashBoardRestApi.cpp - [%s]"), TEXT("GetDashBoardStatisticsRestApi"));

    std::string cur = CDashBoardRestApi::GetTimeStamp();
    std::string start = "1970-01-01";
    std::string end = cur;

    int time = 5;
    bool error = false;

    nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

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

    if (request.query().has("end")) {
        std::string message = request.query().get("end").value();
        if (std::regex_match(message, regexDate))
            end = message;
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "end must be Date Type."} });
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
        TEXT("SELECT environment, CONCAT('[', GROUP_CONCAT(JSON_OBJECT(STATUS, avg_col)) ,']')\
            FROM(\
                SELECT Count(*) AS avg_col, STATUS, d.environment\
                FROM log l\
                JOIN device d\
                ON l.device_idx = d.idx\
                WHERE DATE('%s') <= DATE(create_time) AND DATE(create_time) <= DATE('%s')\
                GROUP BY STATUS, d.environment\
            ) a\
            GROUP by environment\
            "),
        start.c_str(),  end.c_str());

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
        result["environment"] = i[0];
        result["data"] = nlohmann::json::parse(i[1]);
        jsonMessage["outputs"].push_back(result);
    }

    jsonMessage["message"] = "Success";
    response.send(Pistache::Http::Code::Ok, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
}

void CDashBoardRestApi::GetDashBoardPolicyRestApi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    core::Log_Debug(TEXT("CDashBoardRestApi.cpp - [%s]"), TEXT("GetDashBoardStatisticsRestApi"));

    std::string cur = CDashBoardRestApi::GetTimeStamp();
    std::string start = "1970-01-01";
    std::string end = cur;

    int time = 5;
    bool error = false;

    nlohmann::json jsonMessage = { {"message", ""}, {"errors", nlohmann::json::array()}, {"outputs", nlohmann::json::array()} };

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

    if (request.query().has("end")) {
        std::string message = request.query().get("end").value();
        if (std::regex_match(message, regexDate))
            end = message;
        else {
            jsonMessage["message"] = "Error";
            jsonMessage["errors"].push_back({ {"Parameter Errors", "end must be Date Type."} });
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
        TEXT("SELECT JSON_OBJECT('idx', a.idx, 'main', a.main, 'sub', a.sub, 'classify', a.classify, 'name', a.name, 'description', a.description, 'recommand', a.recommand, 'activate', a.activate, 'recommand_devices', JSON_EXTRACT(a.recommand_devices, '$'), 'activate_devices', JSON_EXTRACT(a.activate_devices, '$'))\
            FROM(\
                SELECT sc.main, sc.sub, p.idx, p.name, p.classify, p.description,\
                (SELECT COUNT(*) FROM device_policy WHERE policy_idx = p.idx) AS activate,\
                (SELECT COUNT(*) FROM device_recommand WHERE security_category_idx = sc.idx) AS recommand,\
                (SELECT CONCAT('[', (GROUP_CONCAT(JSON_OBJECT('idx', d.idx, 'name', d.name, 'environment', d.environment))), ']')\
                    FROM device_recommand dr\
                    JOIN device d\
                    ON dr.device_category_idx = d.device_category_idx\
                    WHERE dr.security_category_idx = sc.idx\
                    GROUP BY dr.security_category_idx) AS recommand_devices,\
                (SELECT CONCAT('[', (GROUP_CONCAT(JSON_OBJECT('idx', d.idx, 'name', d.name, 'environment', d.environment))), ']')\
                    FROM device_policy dp\
                    JOIN device d\
                    ON dp.device_idx = d.idx\
                    WHERE dp.policy_idx = p.idx AND dp.activate = 1\
                    GROUP BY dp.policy_idx) AS activate_devices\
                FROM policy p\
                JOIN security_category sc\
                ON p.security_category_idx = sc.idx\
                ORDER BY recommand DESC\
            ) a"));

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
        jsonMessage["outputs"].push_back(nlohmann::json::parse(i[0]));
    }

    jsonMessage["message"] = "Success";
    response.send(Pistache::Http::Code::Ok, jsonMessage.dump(), Pistache::Http::Mime::MediaType::fromString("application/json"));
}