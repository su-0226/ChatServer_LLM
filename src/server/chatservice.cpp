#include"chatservice.hpp"
#include"public.hpp"
#include<muduo/base/Logging.h>
#include<vector>
#include<string>
using namespace muduo;



ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    my_handelMap.insert({LOGIN_MSG,bind(&ChatService::login,this,_1,_2,_3)});
    my_handelMap.insert({REG_MSG,bind(&ChatService::reg,this,_1,_2,_3)});
    my_handelMap.insert({ONE_CHAT_MSG,bind(&ChatService::oneChat,this,_1,_2,_3)});
    my_handelMap.insert({ADD_FRIEND_MSG,bind(&ChatService::add_friend,this,_1,_2,_3)});
    my_handelMap.insert({CREATE_GROUP_MSG,bind(&ChatService::createGroup,this,_1,_2,_3)});
    my_handelMap.insert({ADD_GROUP_MSG,bind(&ChatService::addGroup,this,_1,_2,_3)});
    my_handelMap.insert({GROUP_CHAT_MSG,bind(&ChatService::chatGroup,this,_1,_2,_3)});
    my_handelMap.insert({LOGOUT_MSG,bind(&ChatService::logout,this,_1,_2,_3)});

    if(my_redis.connect())
    {
        my_redis.init_notify_handler(bind(&ChatService::handlerRedisSubscribeMessage,this,_1,_2));
    }

    Init_Chatbot();

}

MsgHandler ChatService::getHandler(int msgid)
{
    auto it=my_handelMap.find(msgid);
    if(it==my_handelMap.end())
    {
        //LOG_ERROR<<"msgid:"<<msgid<<"do not have handler.";
        return [=](const TcpConnectionPtr& conn,json& js,Timestamp time){
            LOG_ERROR<<"msgid:"<<msgid<<"do not have handler.";
        };
    }
    else
    {
        return my_handelMap[msgid];
    }
}
//redis消息队列中的消息，channel（某客户端A的id） message （其他客户端B发送来的消息）,B不一定在当前server上登录
//                      客户端A被nginx分配到当前server上登陆了
void ChatService::handlerRedisSubscribeMessage(int channel,string message)
{
    {
        lock_guard<mutex> lock(mutex);
        auto it=my_UserConnectionMap.find(channel);
        if(it!=my_UserConnectionMap.end())
        {
            it->second->send(message);
            return;
        }
    }
    my_offM.insertMsg(channel,message);
}

void ChatService::login(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    int id=js["id"];
    string password=js["password"];
    User user=my_userModel.query(id);
    if(user.getId()==id&&user.getPassword()==password)
    {
        if(user.getState()=="online")
        {
            json response;
            response["msgid"]=LOGIN_MSG_ACK;
            response["errorno"]=2;
            response["errormsg"]=user.getName()+" already online, please change a id.";
            conn->send(response.dump());
        }
        //{"msgid":1,"id":1,"password":"123456"}   {"msgid":5,"id":2,"from":"li si","to":1,"msg":"nihao,wo shi li si."}
        else//登录成功
        {
            //LOG_INFO<<"登录成功";
            if((my_chatbot.getChatbotUser().getId())==-1)
            {
                User u=my_userModel.query_byName("chatbot");
                if(u.getId()==-1)
                {
                    my_chatbot.getChatbotUser().setName("chatbot");
                    my_chatbot.getChatbotUser().setState("online");
                    my_userModel.insert(my_chatbot.getChatbotUser());
                }
                else
                {
                    my_chatbot.getChatbotUser().setId(u.getId());
                    my_chatbot.getChatbotUser().setName(u.getName());
                    my_chatbot.getChatbotUser().setState("online");
                }
                my_userModel.updateStute(ref(my_chatbot.getChatbotUser()));
                my_friM.insert_fri(user.getId(),my_chatbot.getChatbotUser().getId());
            }
            else
            {
                my_chatbot.getChatbotUser().setState("online");
                my_userModel.updateStute(ref(my_chatbot.getChatbotUser()));
                my_friM.insert_fri(user.getId(),my_chatbot.getChatbotUser().getId());
            }
            //LOG_INFO<<"加机器人好友成功";
            {
            lock_guard<mutex> lock(my_ConnectionMutex);
            my_UserConnectionMap.insert({id,conn});//因为login传入的conn是const修饰的，所以my_UserConnectionMap的连接类型不能是引用
            }

            //客户端id到来后，服务器向redis订阅该id
            my_redis.subscribe(id);

            user.setState("online");
            my_userModel.updateStute(user);
            json response;
            response["msgid"]=LOGIN_MSG_ACK;
            response["errorno"]=0;
            response["id"]=user.getId();
            response["name"]=user.getName();
            //将离线时offlinemessage表中的信息加载出来返回
            vector<string> offline_msg=my_offM.getMsg(user.getId());
            if(!offline_msg.empty())
            {
                response["offlinemsg"]=offline_msg;
                my_offM.clearMsg(user.getId());
            }
            //返回登录ID的好友信息表
            vector<User> friends=my_friM.query_friList(user.getId());
            if(!friends.empty())
            {
                vector<string> friends_js;
                for(auto& user:friends)
                {
                    json fri_js;
                    fri_js["id"]=user.getId();
                    fri_js["name"]=user.getName();
                    fri_js["state"]=user.getState();
                    friends_js.push_back(fri_js.dump());
                }
                response["friends"]=friends_js;        
            }
            vector<Group> groupList=my_groM.getGroup_fromUid(user.getId());//调用getGroup_fromUid方法后，返回的Group中以及包含了vector<GroupUser>
            if(!groupList.empty())
            {
                vector<string> groupList_js;
                for(Group& group:groupList)//初始化群组中用户列表
                {
                    json group_js;
                    
                    group_js["groupid"]=group.getID();
                    group_js["groupname"]=group.getName();
                    group_js["groupdesc"]=group.getDesc();
                    vector<string> groupusers_str;
                    vector<GroupUser> groupusers=group.getUsers();
                    for(GroupUser& gu:groupusers)
                    {
                        json js;
                        js["id"]=gu.getId();
                        js["name"]=gu.getName();
                        js["state"]=gu.getState();
                        js["role"]=gu.getRole();
                        string str=js.dump();
                        groupusers_str.push_back(str);
                    }
                    group_js["groupuser"]=groupusers_str;
                    groupList_js.push_back(group_js.dump());
                }
                response["groups"]=groupList_js;
            }
            conn->send(response.dump());
        }  
    }
    else{
        json response;
        response["msgid"]=LOGIN_MSG_ACK;
        response["errorno"]=1;
        if(user.getId()==-1)
        {
            response["errormsg"]=" id have not register yet, login fail.";
        }
        else
        {
            response["errormsg"]=user.getName()+" password error, login fail.";
        }
        conn->send(response.dump());
    }

}
//注册时只需要提供name、password，id在User表中自增，state初始是“offline”
void ChatService::reg(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    string name=js["name"];
    if(name=="chatbot")
    {
        json response;
        response["msgid"]=REG_MSG_ACK;
        response["errorno"]=1;
        response["errormsg"]="name can not be chatbot.";
        conn->send(response.dump());
    }
    string password=js["password"];
    User user;
    user.setName(name);
    user.setPassword(password);
    bool is_inserted=my_userModel.insert(user);
    if(is_inserted)
    {
        LOG_INFO<<"user:"<<name<<"register success.";//注册成功，将信息返回给连接对端，使用TCPConnectionPtr& conn
        json response;
        response["msgid"]=REG_MSG_ACK;
        response["errorno"]=0;//errorno表示错误位，为0表示业务成功，此时无需恢复errormsg字段，若非0，则需要增加具体errormsg字段
        response["id"]=user.getId();
        conn->send(response.dump());//string str=json.dump()表示将json反序列化为string，json js=json::parse(string str)表示将string序列化为json
    }
    else
    {
        LOG_INFO<<"user:"<<name<<"register fail.";
        json response;
        response["msgid"]=REG_MSG_ACK;
        response["errorno"]=1;
        response["errormsg"]="insert to database:User fail.";
        conn->send(response.dump());
    }
}

void ChatService::chatTObot(int userid,string msg,const TcpConnectionPtr& conn)
{
    //LOG_INFO<<"chatTobot start, userid="<<userid<<", msg:"<<msg;
    
    string ans=my_chatbot.answer_question(userid,msg);
    LOG_INFO<<"chatbot.answer_question()->ans:"<<ans;
    json js_ans;
    js_ans["msgid"]=ONE_CHAT_MSG;
    js_ans["time"]=Timestamp::now().toFormattedString();
    js_ans["to"]=userid;
    js_ans["from"]=my_chatbot.getChatbotUser().getId();
    js_ans["msg"]=ans;
    {
        lock_guard<mutex> lock(my_ConnectionMutex);
        auto it=my_UserConnectionMap.find(userid);
        if(it!=my_UserConnectionMap.end())
        {
            it->second->send(js_ans.dump());
            LOG_INFO<<"send back to client from bot.";
            return;
        }
    }
    my_offM.insertMsg(userid,js_ans.dump());
}

void ChatService::oneChat(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    int toID=js["to"].get<int>();
    int fromID=js["from"].get<int>();
    if(toID==my_chatbot.getChatbotUser().getId())//跟ai聊天
    {
        LOG_INFO<<"CHAT TO BOT.";
        chatTObot(fromID,js["msg"],conn);
    }
    User from=my_userModel.query(fromID);
    js.contains("groupid")? js["msgid"]=GROUP_CHAT_MSG:js["msgid"]=ONE_CHAT_MSG;
    js["name"]=from.getName();
    js["time"]=time.toFormattedString();
    {
        lock_guard<mutex> lock(my_ConnectionMutex);
        auto it=my_UserConnectionMap.find(toID);
        if(it!=my_UserConnectionMap.end())
        {
            it->second->send(js.dump());
            return;
        }
    }
    User user=my_userModel.query(toID);
    if(user.getId()==-1)//要发送的人不存在
    {
        conn->send("the guy you want to chat not exist.");
        return;
    }
    if(user.getState()=="online")
    {
        my_redis.publish(toID,js.dump());
        return;
    }
    //要发送信息的用户离线
    my_offM.insertMsg(toID,js.dump());
}

//添加好友信息，json消息至少需要包含{"msgid":6,"friendid":xx,"id":xx}
void ChatService::add_friend(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    int userid=js["id"].get<int>();
    int friendid=js["friendid"].get<int>();
    User fri=my_userModel.query(friendid);
    if(fri.getId()==-1)
    {
        LOG_INFO<<"the friend you want to add not register yet.";
        return;
    }
    if(my_friM.is_friend(userid,friendid))
    {
        LOG_INFO<<"the id you want to add is already your friend.";
        return;
    }
    my_friM.insert_fri(userid,friendid);
}

void ChatService::createGroup(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    int userid=js["id"].get<int>();
    string groupname=js["groupname"];
    string groupdesc=js["groupdesc"];
    Group group;
    group.setName(groupname);
    group.setDesc(groupdesc);
    if(my_groM.createGroup(group))
    {
        my_groM.addGroup(userid,group.getID(),"creator");
        LOG_INFO<<"创建群组"<<groupname<<"成功.";
        return;
    }
    LOG_INFO<<"创建群组"<<groupname<<"失败. 原因极有可能为群组名称已存在.";
}
void ChatService::addGroup(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    int userid=js["id"].get<int>();
    int groupid=js["groupid"].get<int>();
    string role;
    if(js.contains("role"))
    {
        role=js["role"];
    }
    else
    {
        role="normal";
    }
    //string role=js["role"];
    if(my_groM.addGroup(userid,groupid,move(role)))
    {
        LOG_INFO<<"加入"<<groupid<<"号群组成功.";
        return;
    };
    LOG_INFO<<"加入"<<groupid<<"号群组失败.";
}

void ChatService::chatGroup(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    int groupid=js["groupid"].get<int>();
    int userid=js["from"].get<int>();
    string msg=js["msg"];
    vector<pair<int,string>> users=my_groM.getGroup_allUid(userid,groupid);
    for(auto& groupuser:users)
    {
        json js_one;
        js_one["from"]=userid;
        js_one["groupid"]=groupid;
        js_one["to"]=groupuser.first;
        js_one["msg"]=msg;
        oneChat(conn,js_one,time);
    }
}

void ChatService::logout(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    int id=js["id"];
    User user=my_userModel.query(id);
    if(user.getId()==-1)
    {
        return;
    }
    if(user.getState()=="offline")
    {
        return;
    }
    {
        lock_guard<mutex> lock(my_ConnectionMutex);
        auto it=my_UserConnectionMap.find(user.getId());
        if(it!=my_UserConnectionMap.end())
        {
            my_UserConnectionMap.erase(it);
        }
    }
    user.setState("offline");
    my_userModel.updateStute(user);
    my_redis.unsubscribe(user.getId());
    my_chatbot.removeID_Buf(user.getId());
}

void ChatService::clientCloseException(const TcpConnectionPtr& conn)
{
    User user;
    {
        lock_guard<mutex> lock(my_ConnectionMutex);
        for(auto it=my_UserConnectionMap.begin();it!=my_UserConnectionMap.end();++it)
        {
            if(it->second==conn)
            {
                user.setId(it->first);
                my_UserConnectionMap.erase(it);//从用户连接表中删除异常断开的用户
                break;
            }
        } 
    }
    if(user.getId()!=-1)
    {
        user.setState("offline");
        my_userModel.updateStute(user);
        my_redis.unsubscribe(user.getId());
        my_chatbot.removeID_Buf(user.getId());
    }
    
}

void ChatService::reset()
{
    my_userModel.resetStute();
}

void ChatService::Init_Chatbot()
{
    my_chatbot.setURL("https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions");
    my_chatbot.setAPIKEY("sk-2b7fada44ee84a02812eabf1c64641ab");
    my_chatbot.setModel("qwen-turbo");
    my_chatbot.setMaxTokens(150);
}

