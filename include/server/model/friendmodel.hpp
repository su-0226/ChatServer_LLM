#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include"user.hpp"
#include<vector>
using namespace std;

class FriendModel{
public:
    void insert_fri(int userid,int friendid);
    vector<User> query_friList(int userid);
    bool is_friend(int userid,int friendid);
private:

};


#endif