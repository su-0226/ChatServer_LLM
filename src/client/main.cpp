#include<iostream>
#include<thread>
#include<string>
#include<vector>
#include<chrono>
#include<functional>
#include<ctime>
#include<unistd.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include"json.hpp"
#include"group.hpp"
#include"user.hpp"
#include"public.hpp"

using namespace std;
using json=nlohmann::json;

//当前登录用户的User
User g_currentUser;
//当前登录用户的Friend信息，里面的password字段都是"""，空字符串
vector<User> g_FriendList;
//当前登录用户所属群组信息
vector<Group> g_GroupList;

bool isMainMenuRunning=false;

void show_CurrentUserData();
void readTaskHandler(int clientfd);
string getCurrentTime();
void mainMenu(int clientfd);


int main(int argc,char** argv)
{
    if(argc<3)
    {
        cerr<<"command invalid! example: ./ChatClient 127.0.0.1 6000"<<endl;
        exit(-1);
    }

    char* server_ip=argv[1];
    uint16_t port=atoi(argv[2]);

    int clientfd=socket(AF_INET,SOCK_STREAM,0);
    if(clientfd==-1)
    {
        cerr<<"client socket create error."<<endl;
        exit(-1);
    }

    sockaddr_in server;
    memset(&server,0,sizeof(sockaddr_in));
    server.sin_family=AF_INET;
    server.sin_port=htons(port);
    server.sin_addr.s_addr=inet_addr(server_ip);

    if(connect(clientfd,(sockaddr*)&server,sizeof(sockaddr_in))==-1)
    {
        cerr<<"client socket connect error."<<endl;
        close(clientfd);
        exit(-1);
    }

    for(;;)
    {
        cout<<"===================================================="<<endl;
        cout<<"===================== 1. login ====================="<<endl;
        cout<<"===================== 2. register =================="<<endl;
        cout<<"===================== 3. quit ======================"<<endl;
        cout<<"===================================================="<<endl;
        int choice=0;
        cin>>choice;
        cin.get();//清空输入缓冲区中的回车

        switch(choice)
        {
            case 1:
            {
                int id=0;
                char pwd[50]={0};
                cout<<"please input userid:";
                cin>>id;
                cin.get();
                cout<<"please input password:";
                cin.getline(pwd,50);
                //cin.get();

                json js;
                js["msgid"]=LOGIN_MSG;
                js["id"]=id;
                js["password"]=pwd;
                string request=js.dump();

                int len=send(clientfd,request.c_str(),strlen(request.c_str())+1,0);
                if(len==-1)
                {
                    cerr<<"send login msg error:"<<request<<endl;
                }
                else//发送登录信息成功
                {
                    char buffer[1024*10]={0};
                    len=recv(clientfd,buffer,1024*10,0);
                    if(len==-1)
                    {
                        cerr<<"recv login response error."<<endl;
                    }
                    else//从服务器接受信息成功
                    {
                        //cout<<"从服务器接受信息成功"<<endl;
                        //cout<<"buffer:"<<buffer<<endl;
                        json responsejs=json::parse(buffer);
                        if((responsejs["errorno"].get<int>())!=0)
                        {
                            cerr<<responsejs["errormsg"]<<endl;
                        }
                        else//登录服务器成功
                        {
                            //cout<<"登录服务器成功"<<endl;
                            g_currentUser.setId(responsejs["id"].get<int>());
                            g_currentUser.setName(responsejs["name"]);

                            if(responsejs.contains("friends"))
                            {
                                g_FriendList.clear();
                                vector<string> fri_vec=responsejs["friends"];
                                for(string& str:fri_vec)
                                {
                                    json js=json::parse(str);
                                    User fri;
                                    fri.setId(js["id"].get<int>());
                                    fri.setName(js["name"]);
                                    fri.setState(js["state"]);
                                    g_FriendList.push_back(fri);
                                }
                            }

                            if(responsejs.contains("groups"))
                            {
                                g_GroupList.clear();
                                vector<string> gro_vec=responsejs["groups"];
                                for(string& group:gro_vec)
                                {
                                    json js=json::parse(group);
                                    Group gro;
                                    gro.setID(js["groupid"].get<int>());
                                    gro.setName(js["groupname"]);
                                    gro.setDesc(js["groupdesc"]);
                                    vector<string> GroupUser_vec=js["groupuser"];
                                    for(string& groupuser:GroupUser_vec)
                                    {
                                        json js=json::parse(groupuser);
                                        GroupUser gu;
                                        gu.setId(js["id"].get<int>());
                                        gu.setName(js["name"]);
                                        gu.setRole(js["role"]);
                                        gu.setState(js["state"]);
                                        gro.getUsers().push_back(gu);
                                    }
                                    g_GroupList.push_back(gro); 
                                }
                            }
                            show_CurrentUserData();
                            if(responsejs.contains("offlinemsg"))
                            {
                                vector<string> offlinemsg=responsejs["offlinemsg"];
                                for(string& msg:offlinemsg)
                                {
                                    json js=json::parse(msg);
                                    if(js.contains("groupid"))
                                    {
                                        cout<<"Group: "<<js["groupid"]<<"--"<<js["time"]<<" ["<<js["from"].get<int>()
                                            <<"] "<<js["name"]<<": "<<js["msg"]<<endl;
                                    }
                                    else
                                    {
                                        cout<<js["time"]<<" ["<<js["from"].get<int>()<<"] "<<js["name"]<<": "<<js["msg"]<<endl;
                                    }
                                    
                                }
                            }
                            static int read_threadnum=0;
                            if(read_threadnum==0)
                            {
                                thread readTask(readTaskHandler,clientfd);
                                readTask.detach();
                            }
                            isMainMenuRunning=true;
                            mainMenu(clientfd);
                        }
                    }
                }
            }
            system("clear");
            break;
            case 2:
            {
                char name[50]={0};
                char pwd[50]={0};
                cout<<"please input username:";
                cin.getline(name,50);
                cout<<endl;
                cout<<"please input password:";
                cin.getline(pwd,50);

                json js;
                js["msgid"]=REG_MSG;
                js["name"]=name;
                js["password"]=pwd;
                string request=js.dump();

                int len=send(clientfd,request.c_str(),strlen(request.c_str())+1,0);
                if(len==-1)
                {
                    cerr<<"send reg msg error:"<<request<<endl;
                }
                else
                {
                    char buffer[1024]={0};
                    len=recv(clientfd,buffer,1024,0);
                    if(len==-1)
                    {
                        cerr<<"recv reg response error."<<endl;
                    }
                    else
                    {
                        json responsejs=json::parse(buffer);
                        if((responsejs["errorno"].get<int>())!=0)
                        {
                            cerr<<"register error, probably the name is alread exist."<<endl;
                        }
                        else
                        {
                            cout<<name<<" register success, userid is "<<responsejs["id"]<<" . Do not forget the id."<<endl;
                        }

                    }
                }
            }
            system("clear");
            break;
            case 3:
                close(clientfd);
                exit(0);
            default:
                system("clear");
                cerr<<"invalid input."<<endl;
                cin.clear();
                cin.ignore();
                break;
        }
    }
    return 0;
}

void readTaskHandler(int clientfd)
{
    for(;;)
    {
        char buffer[1024]={0};
        int len=recv(clientfd,buffer,1024,0);
        if(len==-1||len==0)
        {
            close(clientfd);
            exit(-1);
        }
        json js=json::parse(buffer);
        int msgid=js["msgid"].get<int>();
        if(msgid==ONE_CHAT_MSG)
        {
            cout<<js["time"]<<" ["<<js["from"].get<int>()<<"] "<<js["name"]<<": "<<js["msg"]<<endl;
            continue;
        }
        else if(msgid==GROUP_CHAT_MSG)
        {
            cout<<js["time"]<<" Group:"<<js["groupid"]<<" "<<" ["<<js["from"].get<int>()<<"] "<<js["name"]<<": "<<js["msg"]<<endl;
            continue;
        }





    }
}

void show_CurrentUserData()
{
    cout<<"================================login user================================"<<endl;
    cout<<"current login user => id:"<<g_currentUser.getId()<<", name:"<<g_currentUser.getName()<<endl;
    cout<<"===============================friend list================================"<<endl;
    if(!g_FriendList.empty())
    {
        for(User& user:g_FriendList)
        {
            cout<<"name:"<<user.getName()<<", id:"<<user.getId()<<", statue:"<<user.getState()<<endl;
        }
    }
    cout<<"================================group list================================"<<endl;
    if(!g_GroupList.empty())
    {
        for(Group& group:g_GroupList)
        {
            cout<<"group name:"<<group.getName()<<", group id:"<<group.getID()<<", group desc:"<<group.getDesc()<<endl;
            for(GroupUser g_user:group.getUsers())
            {
                cout<<"Member name:"<<g_user.getName()<<", id:"<<g_user.getId()<<", role:"<<g_user.getRole()<<", statue:"<<g_user.getState()<<endl;
            }
        }
    }
    cout<<"=========================================================================="<<endl;
}

void help(int fd=0,string str="");
void onechat(int fd,string str);
void addfriend(int fd,string str);
void creategroup(int fd,string str);
void addgroup(int fd,string str);
void groupchat(int fd,string str);
void quit(int fd,string str);

unordered_map<string,string> commandMap={
    {"help","显示所有支持的命令及它们的格式 help"},
    {"chat","一对一聊天，格式 chat:friendid:message"},
    {"addfriend","添加好友，格式 addfriend:friendid"},
    {"creategroup","创建群聊，格式 creategroup:groupname:groupdesc"},
    {"addgroup","加入群聊，格式 addgroup:groupid"},
    {"groupchat","群聊发送信息，格式 groupchat:groupid:message"},
    {"quit","退出登录, 返回login、register界面, 格式 quit"}
};

unordered_map<string,function<void(int,string)>> commandHandlerMap={
    {"help",help},
    {"chat",onechat},
    {"addfriend",addfriend},
    {"creategroup",creategroup},
    {"addgroup",addgroup},
    {"groupchat",groupchat},
    {"quit",quit}
};

void mainMenu(int cfd)
{
    help();
    char buffer[1024]={0};
    while(isMainMenuRunning)
    {
        cin.getline(buffer,1024);
        string commandbuf(buffer);
        string command;
        int idx=commandbuf.find(":");
        if(idx==-1)
        {
            command=commandbuf;
        }
        else
        {
            command=commandbuf.substr(0,idx);
        }
        auto it=commandHandlerMap.find(command);
        if(it==commandHandlerMap.end())
        {
            cerr<<"invalid input command."<<endl;
            continue;
        }
        else
        {
            it->second(cfd,commandbuf.substr(idx+1,commandbuf.size()-idx));
        }

    }
}

void help(int fd,string str) //help
{
    cout<<"show command list and format:"<<endl;
    for(auto& it:commandMap)
    {
        cout<<it.first<<" >>> "<<it.second<<endl;
    }
    cout<<endl;
}

void onechat(int fd,string str)// str格式 friendid:message
{
    //onechat 的json格式 {"msgid":5, "from":fromID, "to":toID,"msg":"xxxxxxxxxxxxxxxxxxx(msg)"};
    int idx=str.find(":");
    if(idx==-1)
    {
        cerr<<"invalid onechat command."<<endl;
        return;
    }
    int toID=atoi((str.substr(0,idx)).c_str());
    string msg=str.substr(idx+1,str.size()-idx);
    json js;
    js["msgid"]=ONE_CHAT_MSG;
    js["from"]=g_currentUser.getId();
    js["to"]=toID;
    js["msg"]=msg;
    string sendstr=js.dump();
    int len=send(fd,sendstr.c_str(),strlen(sendstr.c_str())+1,0);
    if(len==-1)
    {
        cerr<<"send onechat command error."<<" msg:"<<sendstr<<endl;
    }
}

void addfriend(int fd,string str)//str格式 friendid 
{
    //加好友json格式为：{"msgid":6, "id":xx, "friendid":xx};
    int friendfd=atoi(str.c_str());
    json js;
    js["msgid"]=ADD_FRIEND_MSG;
    js["id"]=g_currentUser.getId();
    js["friendid"]=atoi(str.c_str());
    string sendstr=js.dump();
    int len=send(fd,sendstr.c_str(),strlen(sendstr.c_str())+1,0);
    if(len==-1)
    {
        cerr<<"send addfriend message error."<<" msg:"<<sendstr<<endl;
    }
}
void creategroup(int fd,string str)//str格式 groupname:groupdesc  idx=9  str.size=19    groupname=str.substr(0,9),groupdesc=str.substr(10,19-9)
{
    //创建群聊的json 为{"msgid":7, "id":xx, "groupname":"xxx","groupdesc":"xxxxxxxxxxxxxxxxxxx"};
    int idx=str.find(":");
    if(idx==-1)
    {
        cerr<<"invalid creategroup command."<<endl;
        return;
    }
    string groupname=str.substr(0,idx);
    string groupdesc=str.substr(idx+1,str.size()-idx);
    json js;
    js["msgid"]=CREATE_GROUP_MSG;
    js["id"]=g_currentUser.getId();
    js["groupname"]=groupname;
    js["groupdesc"]=groupdesc;
    string sendstr=js.dump();

    int len=send(fd,sendstr.c_str(),strlen(sendstr.c_str())+1,0);
    if(len==-1)
    {
        cerr<<"send creategroup error."<<" msg:"<<sendstr<<endl;
    }
}
void addgroup(int fd,string str)//str groupid
{
    //加入群聊的json格式   {"msgid":8, "id":xx, "groupid":xxx,"role":"可选"}
    int groupid=atoi(str.c_str());
    json js;
    js["msgid"]=ADD_GROUP_MSG;
    js["id"]=g_currentUser.getId();
    js["groupid"]=groupid;
    string sendstr=js.dump();

    int len=send(fd,sendstr.c_str(),strlen(sendstr.c_str())+1,0);
    if(len==-1)
    {
        cerr<<"send addgroup error."<<" msg:"<<sendstr<<endl;
    }
}
void groupchat(int fd,string str)//str   groupid:message
{
    //groupchat 的json格式 {"msgid":9, "from":xx, "groupid":xxx,"msg":"xxxxxxxxxxxxxxxxxxx(msg)"}
    int idx=str.find(":");
    if(idx==-1)
    {
        cerr<<"invalid groupchat command."<<endl;
        return;
    }
    int groupid=atoi((str.substr(0,idx)).c_str());
    string msg=str.substr(idx+1,str.size()-idx);
    json js;
    js["msgid"]=GROUP_CHAT_MSG;
    js["from"]=g_currentUser.getId();
    js["groupid"]=groupid;
    js["msg"]=msg;
    string sendstr=js.dump();
    int len=send(fd,sendstr.c_str(),strlen(sendstr.c_str())+1,0);
    if(len==-1)
    {
        cerr<<"send groupchat command error."<<" msg:"<<sendstr<<endl;
    }
}
void quit(int fd,string str)//格式 quit
{
    //用户登出的json格式   {"msgid":10, "id":xx}
    json js;
    js["msgid"]=LOGOUT_MSG;
    js["id"]=g_currentUser.getId();
    string sendstr=js.dump();

    int len=send(fd,sendstr.c_str(),strlen(sendstr.c_str())+1,0);
    if(len==-1)
    {
        cerr<<"send logout error."<<" msg:"<<sendstr<<endl;
    }
    isMainMenuRunning=false;
}


