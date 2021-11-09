#pragma once
#include "stdafx.h"
#include <stdarg.h>
#include <mysql/mysql.h>

#define MAX_BUF_SIZE 4096

class CDatabase
{
private:
    std::string m_address;
    std::string m_user;
    std::string m_password;
    std::string m_port;
    std::string m_database;
    MYSQL* m_conn;
public:
    CDatabase(const std::tstring& address, const std::tstring& user, const std::tstring& pass, const std::tstring& port, const std::tstring& database);
    ~CDatabase();

    void Init(const std::tstring& address, const std::tstring& user, const std::tstring& pass, const std::tstring& port, const std::tstring& database);
    int InsertQuery(std::tstring sql, ...);
    bool UpdateQuery(std::tstring sql, ...);
    bool DeleteQuery(std::tstring sql, ...);
    MYSQL_RES * SelectQuery(std::tstring sql, ...);
    MYSQL_RES * UserDefineQuery(std::string sql_query, ...);
    const char* GetLastError();

    static std::vector<MYSQL_FIELD*> GetFieldList(MYSQL_RES * res);
    static std::vector<MYSQL_ROW> GetRowList(MYSQL_RES * res);
};
