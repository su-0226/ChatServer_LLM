#ifndef GROUP_H
#define GROUP_H

#include<string>
#include<vector>
#include"groupuser.hpp"
using namespace std;

class Group{
public:
    Group(int id=-1,string name="",string desc="")
                    :group_id(id),group_name(name),group_desc(desc)
    {}
    inline void setID(int id){group_id=id;}
    inline void setName(string name){group_name=name;}
    inline void setDesc(string desc){group_desc=desc;}

    inline int getID(){return group_id;}
    inline string getName(){return group_name;}
    inline string getDesc(){return group_desc;}
    inline vector<GroupUser>& getUsers(){return users;}

private:
    int group_id;//组id
    string group_name;//组名
    string group_desc;//组描述
    vector<GroupUser> users;//该组中所有User，通过查表GroupUser获得,每个GroupUser都是一个带有role信息的user类。
};
#endif