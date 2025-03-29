#ifndef REDIS_H
#define REDIS_H

#include<hiredis/hiredis.h>
#include<thread>
#include<string>
#include<functional>
using namespace std;


class Redis{
public:
    Redis();
    ~Redis();

    bool connect();

    bool publish(int channel,string message);

    bool subscribe(int channel);

    bool unsubscribe(int channel);

    //在独立线程中接收订阅通道的消息
    void observer_channel_message();

    void init_notify_handler(function<void(int,string)> fn);

private:
    //hiredis 同步上下文对象，负责publish信息
    redisContext* publish_context;
    //hiredis 同步上下文对象，负责subscribe信息
    redisContext* subscribe_context;
    //收到订阅信息时给server上报信息的回调函数
    function<void(int,string)> notify_message_handler;
};

#endif