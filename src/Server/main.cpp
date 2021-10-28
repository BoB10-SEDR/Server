#include "dbconnection.h"

int main(int argc, char* argv[])
{
    auto conn = new ujinSQL("12.52.135.143", "root", "autoset", "mysql"); // 생성자에서 mysql db 연결

    MYSQL_RES* res = conn->ExcuteQuery("show tables"); // 쿼리 실행하기
    std::cout << "MySQL Tables in mysql database:" << std::endl;
    std::vector<std::vector<std::string>> records = conn->GetQueryRow();
    conn->PrintRecords(records);

    for (int i = 1; i < 3; i++) // 0은 칼럼명이라 1부터 시작
    {
        std::string query = "select * from ";
        query.append(records[i][0]); // show tables의 결과에서 0번째 인덱스가 table명이므로
        MYSQL_RES* res = conn->ExcuteQuery(query);
        std::vector<std::vector<std::string>> records2 = conn->GetQueryRow();
        conn->PrintRecords(records2);
    }

    return 0;
}