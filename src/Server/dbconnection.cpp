#include "dbconnection.h"


ujinSQL::ujinSQL(const std::string& _address, const std::string& _user, const std::string& _pass, const std::string& _database)
{
    this->m_address = _address;
    this->m_user = _user;
    this->m_pass = _pass;
    this->m_database = _database;

    this->m_conn = mysql_init(NULL);
    if (!mysql_real_connect(this->m_conn, this->m_address.c_str(), this->m_user.c_str(), this->m_pass.c_str(), this->m_database.c_str(), 0, NULL, 0))
    {
        std::cout << "Connection error: " << mysql_error(this->m_conn) << std::endl;
        exit(1);
    }
}

ujinSQL::~ujinSQL()
{
    mysql_free_result(this->m_res);
    mysql_close(this->m_conn);
}

MYSQL_RES* ujinSQL::ExcuteQuery(const std::string& _sql_query)
{
    if (mysql_query(this->m_conn, _sql_query.c_str()))
    {
        std::cout << "MYSQL query error: " << mysql_error(this->m_conn) << std::endl;
        exit(1);
    }
    return this->m_res = mysql_store_result(this->m_conn);
}

std::vector<std::vector<std::string>> ujinSQL::GetQueryRow() // 2차원 배열을 사용
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

void ujinSQL::PrintRecords(const std::vector<std::vector<std::string>>& records)
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