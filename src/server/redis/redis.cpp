#include"redis.hpp"
#include<iostream>
using namespace std;

Redis::Redis()
    :publish_context(nullptr),subscribe_context(nullptr)
{

}

Redis::~Redis()
{
    if(publish_context!=nullptr)
    {
        redisFree(publish_context);
    }
    if(subscribe_context!=nullptr)
    {
        redisFree(subscribe_context);
    }
}

bool Redis::connect()
{
    publish_context=redisConnect("127.0.0.1",6379);
    if(publish_context==nullptr)
    {
        cerr<<"publish redis connect failed."<<endl;
        return false;
    }
    subscribe_context=redisConnect("127.0.0.1",6379);
    if(subscribe_context==nullptr)
    {
        cerr<<"subscribe redis connect failed."<<endl;
        return false;
    }
    //子线程监听通道上的事件，有消息就给业务层上报信息
    thread t(
        [&](){
            observer_channel_message();
        }
    );
    t.detach();
    cout<<"connect redis-server success."<<endl;
    return true;
}

bool Redis::publish(int channel,string message)
{
    redisReply* reply=(redisReply*) redisCommand(publish_context,"Publish %d %s",channel,message.c_str());
    if(reply==nullptr)
    {
        cerr<<"publish command failed."<<endl;
        return false;
    }   

    freeReplyObject(reply);
    return true;
}

bool Redis::subscribe(int channel)
{
    if(REDIS_ERR==redisAppendCommand(subscribe_context,"Subscribe %d",channel))
    {
        cerr<<"subscribe command failed."<<endl;
        return false;
    }
    int done=0;
    while(!done)
    {
        if(REDIS_ERR==redisBufferWrite(subscribe_context,&done))
        {
            cerr<<"subscribe command failed."<<endl;
            return false;
        }
    }
    
    return true;
}

bool Redis::unsubscribe(int channel)
{
    if(REDIS_ERR==redisAppendCommand(subscribe_context,"Unsubscribe %d",channel))
    {
        cerr<<"unsubscribe command failed."<<endl;
        return false;
    }
    int done=0;
    while(!done)
    {
        if(REDIS_ERR==redisBufferWrite(subscribe_context,&done))
        {
            cerr<<"unsubscribe command failed."<<endl;
            return false;
        }
    }
    return true;
}

void Redis::observer_channel_message()
{
    //此函数在独立子线程中运行
    redisReply* reply=nullptr;
    while(REDIS_OK==redisGetReply(subscribe_context,(void**) &reply))
    {   //element[1]是有订阅号消息发生时的订阅通道号，element[2]是该通道号返回的消息message
        if(reply!=nullptr&&reply->element[2]!=nullptr&&reply->element[2]->str!=nullptr)
        {
            notify_message_handler(atoi(reply->element[1]->str),reply->element[2]->str);
        }
        freeReplyObject(reply);
    }
    cerr<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>observer_channel_message quit<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
}

void Redis::init_notify_handler(function<void(int,string)> fn)
{
    notify_message_handler=fn;
}








