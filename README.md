# ChatServer_LLM_Docker
ChatServer_LLM with Docker

ChatServer_LLM_Docker是基于c++的高性能网络聊天室，支持多人同时相互通信/群组通信，同时支持大语言模型API调用

项目使用nginx反向代理以实现服务器端的负载均衡、使用mysql存储用户数据、不同服务器之间的用户通过redis消息队列实现通信，
基于muduo网络库实现高并发通信、基于curl实现HTTP访问LLM-API、使用nlohmann::json进行服务端-客户端消息之间的序列化及反序列化
为了方便使用者使用，该项目配置了Docker，支持一键构建项目

#自用时需更改以下代码
main/src/server/chatservice :427行
void ChatService::Init_Chatbot()
{
    my_chatbot.setURL("your_model_url");//更改为自己想使用的modelURL
    my_chatbot.setAPIKEY("your_api_key");//更改为自己申请到的API_KEY
    my_chatbot.setModel("your_model_name");//更改为想使用的LLM_model
}


#linux环境下需要准备以下
muduo网络库
curl库
nginx(需支持TCP转发、修改nignx/nginx.conf)
redis
mysql

#编译命令
cd ./build
rm -rf *
cmake ..
make

#启动命令
cd ./bin
./ChatServer Serverip port
./ChatClient Nginxip port
