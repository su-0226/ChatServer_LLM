#include<iostream>
#include<string>
#include<curl/curl.h>
#include"../thirdparty/json.hpp"

using namespace std;
using json=nlohmann::json;

static size_t writeCallback(void* contents,size_t size,size_t nmemb,string* output)
{
    size_t allSize=size * nmemb;
    output->append((char*)contents,allSize);
    return allSize;
}

int main()
{
    string api_key="sk-2b7fada44ee84a02812eabf1c64641ab";
    string url="https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions";
    string model="qwen-turbo";

    json request_json={
        {"model",model},
        {"messages",{
            {{"role","user"},{"content","hello,who are you?"}}
        }}
    };
    
    string request_body=request_json.dump();

    CURL* curl=curl_easy_init();
    if(curl)
    {
        CURLcode res;
        string response_str;
        string header_str;
        struct curl_slist* headers=nullptr;
        headers=curl_slist_append(headers,("Authorization:Bearer"+api_key).c_str());
        headers=curl_slist_append(headers,"Content-Type:application/json");

        curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
        curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,request_body.c_str());
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,writeCallback);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response_str);

        res=curl_easy_perform(curl);
        if(res!=CURLE_OK)
        {
            cerr<<"curl_easy_perform fail:"<<curl_easy_strerror(res)<<endl;
        }
        else
        {
            auto response_json=json::parse(response_str);
            string reply=response_json["choices"][0]["message"]["content"];
            cout<<model<<":"<<reply<<endl;
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

    }

    return 0;
}
