#include"usermodel.hpp"
#include"database.h"
#include<muduo/base/Logging.h>
#include<iostream>
#include<stdlib.h>
using namespace std;



bool UserModel::insert(User& user)
{
    char sql[1024]={0};
    sprintf(sql,"insert into User(name,password,state) values('%s','%s','%s')",//sql语句，insert into 表名（字段1，字段2） values（值1，值2）;|插入语句
                    user.getName().c_str(),user.getPassword().c_str(),user.getState().c_str());
    MySQL mysql;//此处给句柄初始值，尚未连接数据库
    if(mysql.connect())//connect（）函数使得mysql句柄代表与数据库的连接
    {
        if(mysql.update(sql))//使用mysql_query（MYSQL* mysql,char* query_santence）函数进行数据库操作
        {
            user.setId(mysql_insert_id(mysql.getconnection()));//返回上一次插入记录的自增长ID
            return true;
        }
    }
    return false;
}

//根据主键id查表
User UserModel::query(int id)
{
    char sql[1024]={0};
    sprintf(sql,"select * from User where id=%d",id);//sql语句，select * from 表 where 主键=xx;|主键查找语句
    MySQL mysql;
    if(mysql.connect())//connect（）函数使得mysql句柄代表与数据库的连接
    {
        MYSQL_RES* res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            if((row=mysql_fetch_row(res))!=nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
            
    }
    return User();

}

User UserModel::query_byName(string name)
{
    char sql[1024]={0};
    sprintf(sql,"select * from User where name='%s'",name.c_str());//sql语句，select * from 表 where 主键=xx;|主键查找语句
    MySQL mysql;
    if(mysql.connect())//connect（）函数使得mysql句柄代表与数据库的连接
    {
        MYSQL_RES* res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            if((row=mysql_fetch_row(res))!=nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
            
    }
    return User();

}


bool UserModel::updateStute(User& user)
{
    char sql[1024]={0};
    sprintf(sql,"update User set state='%s' where id=%d",
                        user.getState().c_str(),user.getId());//sql语句，update 表 set 字段='value' where 主键=xx;|按主键更新字段值
    MySQL mysql;//mysql代表连接，其中有MYSQL* conn，使用conn进行query（）
    if(mysql.connect())//connect（）函数使得mysql句柄代表与数据库的连接
    {
        return mysql.update(sql);
    }
    return false;
}


void UserModel::resetStute()
{
    string sql="update User set state='offline' where state='online'";//服务器断开连接，将User表中所有用户状态设置为offline
    MySQL mysql;//mysql代表连接，其中有MYSQL* conn，使用conn进行query（）
    if(mysql.connect())//connect（）函数使得mysql句柄代表与数据库的连接
    {
        mysql.update(sql);
    }
}




