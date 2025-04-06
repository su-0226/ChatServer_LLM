#include"database.h"
#include<muduo/base/Logging.h>

//数据库就在本机，所以使用本机环回地址127.0.0.1访问   使用docker时server应该改为mysql               而不是ip地址，static string server="192.168.46.128";
//static string server="127.0.0.1";
static string server="mysql";
static string user="root";
static string password="123456";
static string dbname="chat";


MySQL::MySQL()
{
    my_conn=mysql_init(nullptr);//获取操作数据库的句柄、my_conn
}


MySQL::~MySQL()
{
    if(my_conn!=nullptr)
    {
        mysql_close(my_conn);//释放句柄，关闭数据库连接
    }
}

//连接数据库
bool MySQL::connect()
{
    MYSQL* p=mysql_real_connect(my_conn,server.c_str(),user.c_str(),password.c_str(),dbname.c_str(),3306,nullptr,0);
    if(p!=nullptr)
    {
        mysql_query(my_conn,"set names gbk");
        LOG_INFO<<"connect mysql success.";
    }
    else{
        LOG_INFO<<"connect mysql fail.";
        if(my_conn==nullptr)
        {
            LOG_INFO<<"error:my_conn is nullptr";
        }
        LOG_INFO<<"error"<<mysql_error(my_conn);
    }
    return p;
}


bool MySQL::update(string sql)
{
    if(mysql_query(my_conn,sql.c_str()))
    {
        //LOG_INFO<<__FILE__<<":"<<__LINE__<<":"
                        //<<sql<<"更新失败"<<"失败号："<<mysql_errno(my_conn);
        return false;
    }
    return true;
}

//查询数据
MYSQL_RES* MySQL::query(string sql)
{   
    if(mysql_query(my_conn,sql.c_str()))
    {
        //LOG_INFO<<__FILE__<<":"<<__LINE__<<":"
                        //<<sql<<"查询失败";
        return nullptr;
    }
    return mysql_use_result(my_conn);
}

MYSQL* MySQL::getconnection()
{
    return my_conn;
}