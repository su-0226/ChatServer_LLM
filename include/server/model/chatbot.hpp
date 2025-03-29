#ifndef CHATBOT_H
#define CHATBOT_H
#include<string>
#include"json.hpp"
#include"user.hpp"
#include<unordered_map>
#include<list>
#include<curl/curl.h>
using namespace std;
using json=nlohmann::json;

class ChatBot{
public:
    ChatBot(string url="",string api_key="",double temperature=0.7,int max_tokens=150,string model="qwen-turbo");

    inline void setURL(string url){URL=url;};
    inline void setAPIKEY(string ak){API_KEY=ak;};
    inline void setTemperature(double t){temperature=t;};
    inline void setMaxTokens(int mt){max_tokens=mt;};
    inline void setModel(string m){MODEL=m;};
    void removeID_Buf(int userid);

    User& getChatbotUser(){return ref(me_bot);};

    string answer_question(int userid,string message);
    void get_messages(int userid,string message);
    static size_t MessageComeCallback(void* contents,size_t size,size_t nmemb,string* output);

private:
    User me_bot;
    string MODEL;
    string API_KEY;
    string URL;
    double temperature;
    int max_tokens;
    unordered_map<int,list<json>> online_user_msgBuffer;
};
#endif