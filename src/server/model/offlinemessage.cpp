#include"offlinemessage.hpp"
#include"database.h"
#include<muduo/base/Logging.h>

void offlineMsgModel::insertMsg(int userid,string msg)
{
    char sql[1024]={0};
    sprintf(sql,"insert into OfflineMessage values(%d,'%s')",userid,msg.c_str()); 
                //sql语句，insert into 表名(字段1,字段2) values(值1,值2)";|表插入语句
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}

void offlineMsgModel::clearMsg(int userid)
{
    char sql[1024]={0};
    sprintf(sql,"delete from OfflineMessage where userid=%d",userid); 
                //sql语句，"delete from 表名 where 主键=xx";|表按键删除语句
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}

vector<string> offlineMsgModel::getMsg(int userid)
{
    char sql[1024]={0};
    sprintf(sql,"select message from OfflineMessage where userid=%d",userid); 
                //sql语句，"select 要查询字段 from 表名 where 查询键=xx";|表按键查询字段信息语句
    vector<string> vec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* res=mysql.query(sql);//此处的res-->   是很多行的message字段信息，每一行只有一个字段，即message
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                vec.push_back(row[0]);//只有row[0]合法，其它都越界
            }
        }
        mysql_free_result(res);
        return vec;
    }
    return vec;
}












