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
不更改此处代码将无法正常使用与机器人的对话功能

#使用Docker
1、使用者在设备中下载好Docker，并且配置好镜像的国内加速源，不然无法构建mysql、redis、nginx镜像
2、确保bash当前工作目录是docker-compose.yml的上级目录
3、sudo docker-compose up --build   #构建项目，该命令会自动配置nginx、redis、mysql等，在mysql中创建项目所需要的表
4、一切正常的话无需管当前页面，新开bash进行下述操作

#Docker构建成功后，使用项目
sudo docker ps -a    #查看docker容器启动状态，需要server1、server2、server3、client、mysql、redis、nginx都处于启动状态
sudo docker exec -it server1 /bin/bash    #进入服务器容器中，准备启动ChatServer，可以选择启动多台服务器，
进入服务器容器后，./ChatServer 0.0.0.0 6000    #server1->6000    server2->6002    server3->6004    都已经在nginx/nginx.conf中配置好服务器负载均衡
sudo docker exec -it server1 /bin/bash    #进入客户端容器，可以同时启动多个服务端进程
进入客户端容器后，./ChatClient nginx 8000    #此时可以看到交互界面，按照提示即可使用各种功能

#有问题可联系1714417026@qq.com


