#ifndef OFFLINEMESSAGE_H
#define OFFLINEMESSAGE_H

#include<string>
#include<vector>
using namespace std;

//操作数据库中的OfflineMessage表，
class offlineMsgModel{
public:
    void insertMsg(int userid,string msg);
    void clearMsg(int userid);
    vector<string> getMsg(int userid);
private:

};


#endif