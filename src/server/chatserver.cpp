#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"
#include<functional>
#include<string>
#include<iostream>
using namespace placeholders;
using namespace std;
using json=nlohmann::json;

ChatServer::ChatServer(EventLoop* loop,const InetAddress& listen_addr,const string& server_name)
                        :my_loop(loop),my_server(TcpServer(loop,listen_addr,server_name))
{
    my_server.setConnectionCallback(bind(&ChatServer::OnConnection,this,_1));
    my_server.setMessageCallback(bind(&ChatServer::OnMessage,this,_1,_2,_3));
    my_server.setThreadNum(4);
}

void ChatServer::start()
{
    my_server.start();
}

void ChatServer::OnConnection(const TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::OnMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp time)
{
    string mes=buf->retrieveAllAsString();
    //cout<<"mes:"<<mes<<endl;
    json js=json::parse(mes);
    //cout<<"after"<<endl;
    ChatService* instance=ChatService::instance();
    MsgHandler task=instance->getHandler(js["msgid"].get<int>());
    task(conn,js,time);
}




