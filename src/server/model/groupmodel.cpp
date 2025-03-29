#include"groupmodel.hpp"
#include"database.h"



bool GroupModel::createGroup(Group& group)
{
    char sql[1024]={0};
    sprintf(sql,"insert into ALLGroup(groupname,groupdesc) values('%s','%s')",group.getName().c_str(),group.getDesc().c_str());

    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            group.setID(mysql_insert_id(mysql.getconnection()));
            return true;
        }
    }
    return false;
}

bool GroupModel::addGroup(int userid,int groupid,string&& role)
{
    char sql[1024]={0};
    if(role!="creator"&&role!="normal")
    {
        role="normal";
    }
    sprintf(sql,"insert into GroupUser(groupid,userid,grouprole) values(%d,%d,'%s')",groupid,userid,role.c_str());

    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

vector<Group> GroupModel::getGroup_fromUid(int userid)
{
    char sql[1024]={0};
    sprintf(sql,"select g.id,g.groupname,g.groupdesc from ALLGroup g inner join GroupUser u on g.id=u.groupid where u.userid=%d",userid);
    
    vector<Group> uid_in_groups;//用户id为userid的用户所属于的群组
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                Group group;
                group.setID(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                uid_in_groups.push_back(group);
            }
        }
        mysql_free_result(res);
    }
    //给每个group增加该group中的所有成员，储存在group.users中，该成员的类型为vector<GroupUser>
    for(auto& group:uid_in_groups)
    {
        sprintf(sql,"select u.id,u.name,u.state,g.grouprole from User u inner join GroupUser g on g.userid=u.id where g.groupid=%d",group.getID());

        MYSQL_RES* res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                GroupUser groupuser;
                groupuser.setId(atoi(row[0]));
                groupuser.setName(row[1]);
                groupuser.setState(row[2]);
                groupuser.setRole(row[3]);
                group.getUsers().push_back(groupuser);
            }
        }
        mysql_free_result(res);
    }
    return uid_in_groups;
}

vector<pair<int,string>> GroupModel::getGroup_allUid(int userid,int groupid)
{
    char sql[1024]={0};
    sprintf(sql,"select userid,grouprole from GroupUser where groupid=%d and userid!=%d",groupid,userid);

    MySQL mysql;
    vector<pair<int,string>> user_In_group;
    if(mysql.connect())
    {
        MYSQL_RES* res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                user_In_group.push_back({atoi(row[0]),row[1]});
            }
        }
        mysql_free_result(res);
    }
    return user_In_group;
}













