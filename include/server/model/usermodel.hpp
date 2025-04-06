#ifndef USERMODEL_H
#define USERMODEL_H

#include"user.hpp"

class UserModel{
public:
    bool insert(User& user);
    User query(int id);
    bool updateStute(User& user);
    void resetStute();
    User query_byName(string name);
private:



};








#endif