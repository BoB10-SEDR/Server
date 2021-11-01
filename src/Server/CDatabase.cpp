#include "CDatabase.h"

CDatabase::CDatabase(const std::tstring& address, const std::tstring& user, const std::tstring& password, const std::tstring& database)
{
    this->m_address = address;
    this->m_user = user;
    this->m_password = password;
    this->m_database = database;

    this->m_conn = mysql_init(NULL);

    if (!mysql_real_connect(this->m_conn, this->m_address.c_str(), this->m_user.c_str(), this->m_password.c_str(), this->m_database.c_str(), 3306, NULL, 0))
    {
        core::Log_Error(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Connection Error"), TEXT(mysql_error(this->m_conn)));
        exit(1);
    }
}

CDatabase::~CDatabase()
{
    mysql_free_result(this->m_res);
    mysql_close(this->m_conn);
}

MYSQL_RES* CDatabase::ExcuteQuery(const std::string& sql_query)
{
    core::Log_Debug(TEXT("CDatabase.cpp - [%s] : %s"), TEXT("Mysql Query Excute"), TEXT(sql_query.c_str()));
    if (mysql_query(this->m_conn, sql_query.c_str()))
    {
        core::Log_Error(TEXT("CDatabase.cpp - [%s] : %s -> %s"), TEXT("Mysql Query Error"), TEXT(sql_query.c_str()), TEXT(mysql_error(this->m_conn)));
        return NULL;
    }

    return this->m_res = mysql_store_result(this->m_conn);
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

int CDatabase::LastResponsePK()
{
    return mysql_insert_id(this->m_conn);
}

void CDatabase::ClearResponse(MYSQL_RES* res)
{
    mysql_free_result(res);
    mysql_free_result(this->m_res);
}

const char* CDatabase::LastError()
{
    return mysql_error(this->m_conn);
}

std::vector<std::vector<std::string>> CDatabase::GetQueryRow() // 2차원 배열을 사용
{
    std::vector<std::vector<std::string>> records;

    MYSQL_FIELD* field;
    std::vector<std::string> fields;
    while ((field = mysql_fetch_field(this->m_res)) != NULL) // 필드 구해오기
    {
        fields.emplace_back(field->name);
    }
    records.emplace_back(fields);

    MYSQL_ROW row;
    int nfields = fields.size();
    while ((row = mysql_fetch_row(this->m_res)) != NULL) // 레코드 읽어서 넣기
    {
        std::vector<std::string> data;
        for (int i = 0; i < nfields; i++)
        {
            data.emplace_back(row[i]);
        }
        records.emplace_back(data);
    }
    return records;
}

void CDatabase::PrintRecords(const std::vector<std::vector<std::string>>& records)
{
    int ntotal = records.size();
    if (ntotal == 0)
    {
        return;
    }

    int nfield = records[0].size();
    std::cout << "+------------------------------------------------------------+" << std::endl;
    for (int i = 0; i < nfield; i++)
    {
        std::cout << records[0][i] << "\t";
    }
    std::cout << std::endl << "+------------------------------------------------------------+" << std::endl;
    for (int i = 1; i < ntotal; i++)
    {
        for (int j = 0; j < nfield; j++)
        {
            std::cout << records[i][j] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    return;
}