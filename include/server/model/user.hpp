#ifndef USER_H
#define USER_H

#include<string>
using namespace std;
class User{
public:
    User(int id=-1,string name="",string pwd="",string state="offline"):
                m_id(id),m_name(name),m_password(pwd),m_state(state)
    {}
    inline void setId(int id){m_id=id;}
    inline void setName(string name){m_name=name;}
    inline void setPassword(string pwd){m_password=pwd;}
    inline void setState(string state){m_state=state;}

    inline int getId(){return m_id;}
    inline string getName(){return m_name;}
    inline string getPassword(){return m_password;}
    inline string getState(){return m_state;}



private:
    int m_id;
    string m_name;
    string m_password;
    string m_state;
};
















#endif