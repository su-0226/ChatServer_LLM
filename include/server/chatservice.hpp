#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include<functional>
#include<muduo/net/TcpConnection.h>
#include<muduo/net/TcpServer.h>
#include<unordered_map>
#include"json.hpp"
#include"usermodel.hpp"
#include"offlinemessage.hpp"
#include"redis.hpp"
#include"friendmodel.hpp"
#include"groupmodel.hpp"
#include"chatbot.hpp"
#include<mutex>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using json=nlohmann::json;

using MsgHandler=function<void(const TcpConnectionPtr& conn,json& js,Timestamp time)>;


class ChatService{
public:
    //构造单例模式
    static ChatService* instance();
    void Init_Chatbot();
    void login(const TcpConnectionPtr& ,json& ,Timestamp );
    void reg(const TcpConnectionPtr& ,json& ,Timestamp );
    void oneChat(const TcpConnectionPtr& ,json& ,Timestamp);
    void add_friend(const TcpConnectionPtr&,json&,Timestamp);
    void chatTObot(int userid,string msg,const TcpConnectionPtr& conn);
    void createGroup(const TcpConnectionPtr&,json&,Timestamp);
    void addGroup(const TcpConnectionPtr&,json&,Timestamp);
    void chatGroup(const TcpConnectionPtr&,json&,Timestamp);
    
    void logout(const TcpConnectionPtr&,json&,Timestamp);

    void handlerRedisSubscribeMessage(int channel,string message);

    MsgHandler getHandler(int msgid);
    void clientCloseException(const TcpConnectionPtr&);
    void reset();

private:
    ChatService();
    unordered_map<int,MsgHandler> my_handelMap;
    UserModel my_userModel;//service中用于操作、查询User表的类
    unordered_map<int,TcpConnectionPtr> my_UserConnectionMap;//存储所有在线用户的连接,该资源会被所有用户访问，需要注意线程安全
    mutex my_ConnectionMutex;//对my_UserConnectionMap的访问修改需要上锁
    offlineMsgModel my_offM;//service 中用于操作、查询OfflineMessage表的类
    FriendModel my_friM;
    GroupModel my_groM;
    Redis my_redis;
    ChatBot my_chatbot;
};





#endif