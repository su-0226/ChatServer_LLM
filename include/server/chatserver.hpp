#ifndef CHATSERVER_H
#define CHATSERVER_H

#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;
using namespace std;

class ChatServer{
public:
    ChatServer()=delete;
    ChatServer(EventLoop* loop,const InetAddress& listen_addr,const string& server_name);

    void start();

private:
    void OnConnection(const TcpConnectionPtr&);
    void OnMessage(const TcpConnectionPtr&,Buffer*,Timestamp);
    TcpServer my_server;
    EventLoop* my_loop;

};













#endif