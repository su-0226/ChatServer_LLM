#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include"group.hpp"
#include<string>
#include<vector>
#include<unordered_map>

class GroupModel{
public:
    bool createGroup(Group& group);
    bool addGroup(int userid,int groupid,string&& role);
    vector<Group> getGroup_fromUid(int userid);
    vector<pair<int,string>> getGroup_allUid(int userid,int groupid);

private:
    
};

#endif