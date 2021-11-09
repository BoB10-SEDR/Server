#include "CDatabase.h"

CDatabase::CDatabase(const std::tstring& address, const std::tstring& user, const std::tstring& password, const std::tstring& port, const std::tstring& database)
{
    this->m_address = address;
    this->m_user = user;
    this->m_password = password;
    this->m_database = database;
    this->m_port = port;

    this->m_conn = mysql_init(NULL);

    if (!mysql_real_connect(this->m_conn, this->m_address.c_str(), this->m_user.c_str(), this->m_password.c_str(), this->m_database.c_str(), atoi(this->m_port.c_str()), NULL, 0))
    {
        core::Log_Error(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Connection Error"), TEXT(mysql_error(this->m_conn)));
        exit(1);
    }
}

CDatabase::~CDatabase()
{
    mysql_close(this->m_conn);
}

void CDatabase::Init(const std::tstring& address, const std::tstring& user, const std::tstring& password, const std::tstring& port, const std::tstring& database)
{
    this->m_address = address;
    this->m_user = user;
    this->m_password = password;
    this->m_database = database;
    this->m_port = port;

    this->m_conn = mysql_init(NULL);

    if (!mysql_real_connect(this->m_conn, this->m_address.c_str(), this->m_user.c_str(), this->m_password.c_str(), this->m_database.c_str(), atoi(this->m_port.c_str()), NULL, 0))
    {
        core::Log_Error(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Connection Error"), TEXT(mysql_error(this->m_conn)));
        exit(1);
    }
}

int CDatabase::InsertQuery(const std::tstring sql_query, ...)
{
    core::Log_Debug(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Mysql Query Excute"), TEXT(sql_query.c_str()));

    va_list ap;     
    char buf[MAX_BUF_SIZE];

    va_start(ap, sql_query);
    vsprintf(buf, sql_query.c_str(), ap);
    va_end(ap);

    core::Log_Debug(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Mysql Insert Query"), TEXT(buf));

    if (mysql_query(this->m_conn, buf))
    {
        core::Log_Error(TEXT("CDatabase.cpp - [%s] : %s -> %s"), TEXT("Mysql Query Error"), TEXT(buf), TEXT(mysql_error(this->m_conn)));
        return NULL;
    }

    MYSQL_RES* res = mysql_store_result(this->m_conn);
    int idx = mysql_insert_id(this->m_conn);
    return idx;
}

bool CDatabase::UpdateQuery(const std::tstring sql_query, ...)
{
    core::Log_Debug(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Mysql Query Excute"), TEXT(sql_query.c_str()));
    
    va_list ap;
    char buf[MAX_BUF_SIZE];

    va_start(ap, sql_query);
    vsprintf(buf, sql_query.c_str(), ap);
    va_end(ap);

    core::Log_Debug(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Mysql Update Query"), TEXT(buf));

    if (mysql_query(this->m_conn, buf))
    {
        core::Log_Error(TEXT("CDatabase.cpp - [%s] : %s -> %s"), TEXT("Mysql Query Error"), TEXT(buf), TEXT(mysql_error(this->m_conn)));
        return false;
    }

    return true;
}

bool CDatabase::DeleteQuery(const std::tstring sql_query, ...)
{
    core::Log_Debug(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Mysql Query Excute"), TEXT(sql_query.c_str()));

    va_list ap;
    char buf[MAX_BUF_SIZE];

    va_start(ap, sql_query);
    vsprintf(buf, sql_query.c_str(), ap);
    va_end(ap);

    core::Log_Debug(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Mysql Delete Query"), TEXT(buf));

    if (mysql_query(this->m_conn, buf))
    {
        core::Log_Error(TEXT("CDatabase.cpp - [%s] : %s -> %s"), TEXT("Mysql Query Error"), TEXT(buf), TEXT(mysql_error(this->m_conn)));
        return false;
    }

    return true;
}

MYSQL_RES* CDatabase::UserDefineQuery(std::string sql_query, ...)
{
    core::Log_Debug(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Mysql Query Excute"), TEXT(sql_query.c_str()));

    va_list ap;
    char buf[MAX_BUF_SIZE];

    va_start(ap, sql_query);
    vsprintf(buf, sql_query.c_str(), ap);
    va_end(ap);

    core::Log_Debug(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Mysql UserDefine Query"), TEXT(buf));

    if (mysql_query(this->m_conn, buf))
    {
        core::Log_Error(TEXT("CDatabase.cpp - [%s] : %s -> %s"), TEXT("Mysql Query Error"), TEXT(buf), TEXT(mysql_error(this->m_conn)));
        return NULL;
    }

    return mysql_store_result(this->m_conn);
}

MYSQL_RES* CDatabase::SelectQuery(std::tstring sql_query, ...)
{
    core::Log_Debug(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Mysql Query Excute"), TEXT(sql_query.c_str()));

    va_list ap;
    char buf[MAX_BUF_SIZE];

    va_start(ap, sql_query);
    vsprintf(buf, sql_query.c_str(), ap);
    va_end(ap);

    core::Log_Debug(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Mysql Select Query"), TEXT(buf));

    if (mysql_query(this->m_conn, buf))
    {
        core::Log_Error(TEXT("CDatabase.cpp - [%s] : %s -> %s"), TEXT("Mysql Query Error"), TEXT(buf), TEXT(mysql_error(this->m_conn)));
        return NULL;
    }

    return mysql_store_result(this->m_conn);
}

std::vector<MYSQL_FIELD*> CDatabase::GetFieldList(MYSQL_RES* res)
{
    std::vector<MYSQL_FIELD*> result;
    MYSQL_FIELD* field;

    while ((field = mysql_fetch_field(res)) != NULL)
    {
        result.emplace_back(field);
    }

    return result;
}

std::vector<MYSQL_ROW> CDatabase::GetRowList(MYSQL_RES* res)
{
    std::vector<MYSQL_ROW> result;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        result.emplace_back(row);
    }

    return result;
}

const char* CDatabase::GetLastError()
{
    return mysql_error(this->m_conn);
}