#include"chatserver.hpp"
#include"chatservice.hpp"
#include<iostream>
#include<signal.h>
using namespace std;

void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}


int main(int argc,char** argv)
{
    if(argc<3)
    {
        cerr<<"command invalid! example: ./ChatServer 127.0.0.1 6000"<<endl;
        exit(-1);
    }

    char* server_ip=argv[1];
    uint16_t port=atoi(argv[2]);
    
    signal(SIGINT,resetHandler);//SIGINT信号到来表示程序即将终止，通常由crtl C导致，此时执行resetHandler函数
    EventLoop m_loop;
    InetAddress listen_addr(server_ip,port);
    ChatServer main_server(&m_loop,listen_addr,"ChatServer");
    main_server.start();
    m_loop.loop();
}