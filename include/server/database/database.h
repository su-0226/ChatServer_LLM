#ifndef DATABASE_H
#define DATABASE_H

#include<mysql/mysql.h>
#include<string>
using namespace std;

class MySQL{
public:
    MySQL();
    ~MySQL();
    bool connect();
    bool update(string sql);
    MYSQL_RES* query(string sql);
    MYSQL* getconnection();
private:
    MYSQL* my_conn;
};








#endif