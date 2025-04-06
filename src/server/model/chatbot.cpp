#include"chatbot.hpp"
#include<functional>
#include<iostream>
#include<vector>
using namespace placeholders;
using namespace std;

ChatBot::ChatBot(string url,string api_key,double temp,int mt,string model)
                :URL(url),API_KEY(api_key),temperature(temp),max_tokens(mt),MODEL(model)
{

}

void ChatBot::get_messages(int userid,string message)
{
    //cout<<"inner get_messages"<<endl;
    auto it=online_user_msgBuffer.find(userid);
    if(it==online_user_msgBuffer.end())
    {
        cout<<"first ask qwen."<<endl;
        online_user_msgBuffer[userid].push_back({{"role", "system"}, {"content", "You are a helpful assistant."}});
        online_user_msgBuffer[userid].push_back({{"role", "user"}, {"content", message}});
        return;
    }
    //cout<<"not first ask."<<endl;
    if(it->second.size()>=20)
    {
        it->second.pop_front();
        it->second.pop_front();
        it->second.push_back(message);
        return;
    }
    it->second.push_back({{"role", "user"}, {"content", message}});
    return;
}

string ChatBot::answer_question(int userid,string msg)
{
    CURL* curl;
    CURLcode res;
    string readBuffer;
    get_messages(userid,msg);
    auto it=online_user_msgBuffer.find(userid);
    list<json>& messages=it->second;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl=curl_easy_init();
    if(curl)
    {
        cout<<"curl true."<<endl;
        curl_easy_setopt(curl,CURLOPT_URL,URL.c_str());
        struct curl_slist* header=nullptr;
        header=curl_slist_append(header,"Content-Type: application/json");
        header=curl_slist_append(header,("Authorization: Bearer"+API_KEY).c_str());
        curl_easy_setopt(curl,CURLOPT_HTTPHEADER,header);
        json question_json={
            {"model",MODEL},
            {"messages",messages},
            {"temperature",temperature},
            {"max_tokens",max_tokens}
        };
        string question_str=question_json.dump(); 
        cout<<"question_str:"<<question_str<<endl;
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,question_str.c_str());
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,MessageComeCallback);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,&readBuffer);
        res=curl_easy_perform(curl);
        if(res!=CURLE_OK)
        {
            cout<<"res!=CURL_OK"<<endl;
            it->second.pop_back();
            json err=json::parse(readBuffer);
            return err.dump();
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(header);
    
    }
    curl_global_cleanup();
    cout<<"readBuffer from ai:"<<readBuffer<<endl;
    json answer=json::parse(readBuffer);
    return answer["choices"][0]["message"]["content"];
}

void ChatBot::removeID_Buf(int userid)
{
    auto it=online_user_msgBuffer.find(userid);
    if(it==online_user_msgBuffer.end())
    {
        return;
    }
    online_user_msgBuffer.erase(it);
}

size_t ChatBot::MessageComeCallback(void* contents,size_t size,size_t nmemb,string* output)
{
    output->append((char*)contents,size*nmemb);
    //cout<<"inner MessageComeCallback, size* nmemb="<<size*nmemb<<endl;
    return size*nmemb;
}


