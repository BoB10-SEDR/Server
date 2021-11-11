#pragma once
#include "stdafx.h"
#include <mysql/mysql.h>
#include <iostream>
#include <vector>
#include <map>

class CDatabase
{
private:
    std::string m_address;
    std::string m_user;
    std::string m_password;
    std::string m_database;
    MYSQL* m_conn;
    MYSQL_RES* m_res;
public:
    CDatabase(const std::tstring& address, const std::tstring& user, const std::tstring& pass, const std::tstring& database);
    ~CDatabase();

    MYSQL_RES* ExcuteQuery(const std::string& sql_query); 
    static std::vector<MYSQL_FIELD*> GetFieldList(MYSQL_RES* res); 
    static std::vector<MYSQL_ROW> GetRowList(MYSQL_RES* res); //결과
    int LastResponsePK(); //마지막 실행한 PK가져옴(FK로 사용)
    void ClearResponse(MYSQL_RES* res); //응답 clear

    std::vector<std::vector<std::string>> GetQueryRow(); 
    void PrintRecords(const std::vector<std::vector<std::string>>& records); 
};