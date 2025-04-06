#ifndef GROUPUSER_H
#define GROUPUSER_H

#include"user.hpp"

class GroupUser:public User{
public:
    void setRole(string role)
    {
        if(role=="creator"||role=="normal")
        {
            my_role=role;
            return;
        }
        else
        {
            my_role="normal";
        }
    }

    inline string getRole(){return my_role;} 


private:
    string my_role;

};
#endif