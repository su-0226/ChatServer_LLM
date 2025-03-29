#include"friendmodel.hpp"
#include"database.h"

void FriendModel::insert_fri(int userid,int friendid)
{
    
    char sql1[1024]={0};
    sprintf(sql1,"insert into Friend values(%d,%d)",//sql语句，insert into 表名（字段1，字段2） values（值1，值2）;|插入语句
                    userid,friendid);
    char sql2[1024]={0};
    sprintf(sql2,"insert into Friend values(%d,%d)",//sql语句，insert into 表名（字段1，字段2） values（值1，值2）;|插入语句
                    friendid,userid);
    MySQL mysql;//此处给句柄MYSQL* conn初始值，尚未连接数据库
    if(mysql.connect())//connect（）函数使得mysql句柄代表与数据库的连接
    {
        mysql.update(sql1);//使用mysql_query（MYSQL* mysql,char* query_santence）函数进行数据库操作
        mysql.update(sql2);
    }
}


vector<User> FriendModel::query_friList(int userid)
{
    char sql[1024]={0};
    //"select u.id,u.name,u.state from Friend f join User u on f.friendid=u.id where f.userid=%d"
    sprintf(sql,"select u.id,u.name,u.state from Friend f inner join User u on f.friendid=u.id where f.userid=%d",//sql语句
                    userid);
    MySQL mysql;//此处给句柄MYSQL* conn初始值，尚未连接数据库
    vector<User> friends;
    if(mysql.connect())//connect（）函数使得mysql句柄代表与数据库的连接
    {
        MYSQL_RES* res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                friends.push_back(user);
            }
        }
        mysql_free_result(res);
    }
    return friends;
}

bool FriendModel::is_friend(int userid,int friendid)
{
    char sql[1024]={0};
    sprintf(sql,"select * from Friend where userid=%d and friendid=%d",userid,friendid);//sql语句
    MySQL mysql;//mysql代表连接，其中有MYSQL* conn，使用conn进行query（）
    if(mysql.connect())//connect（）函数使得mysql句柄代表与数据库的连接
    {
        MYSQL_RES* res=mysql.query(sql);
        int rows=0;
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                ++rows;
            }
        }
        mysql_free_result(res);
        return rows!=0;
    }
    return false;
}










