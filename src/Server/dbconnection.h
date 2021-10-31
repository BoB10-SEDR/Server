#pragma once
#include <iostream>
#include <mysql/mysql.h>
#include <vector>
#include <map>

class ujinSQL
{
private:
    std::string m_address;
    std::string m_user;
    std::string m_pass;
    std::string m_database;
    MYSQL* m_conn;
    MYSQL_RES* m_res;

public:
    ujinSQL(const std::string& address, const std::string& user, const std::string& pass, const std::string& database); 
    ~ujinSQL();

    MYSQL_RES* ExcuteQuery(const std::string& sql_query); 
    std::vector<std::vector<std::string>> GetQueryRow(); 
    void PrintRecords(const std::vector<std::vector<std::string>>& records); 
};