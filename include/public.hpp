#ifndef PUBLIC_H
#define PUBLIC_H


//约定客户端、服务器之间收发信息格式如下：
//  json格式{"msgid":xx,""}
//MsgType指的是msgid
enum MsgType
{
    LOGIN_MSG=1,//登录信息  对应json中应该至少有{"msgid":1, "id":xx, "password":"xxxxx"};{"msgid":1, "id":4, "password":"321"}
    LOGIN_MSG_ACK,//登录响应信息   
    REG_MSG,//注册信息   对应json中应该至少有{"msgid":3, "name":xx, "password":"xxxxx"};{"msgid":3, "name":"bubu", "password":"888888"}
    REG_MSG_ACK,//注册响应信息
    ONE_CHAT_MSG,//一对一聊天发送信息   对应json中应该至少有{"msgid":5, "from":fromID, "to":toID,"msg":"xxxxxxxxxxxxxxxxxxx(msg)"};
    ADD_FRIEND_MSG,//加好友信息   对应json中应该至少有{"msgid":6, "id":xx, "friendid":xx"};{"msgid":6, "id":4, "friendid":1}

    CREATE_GROUP_MSG,//创建群聊信息   对应json中应该至少有{"msgid":7, "id":xx, "groupname":"xxx","groupdesc":"xxxxxxxxxxxxxxxxxxx"};
    ADD_GROUP_MSG,//加入群聊信息   对应json中应该至少有{"msgid":8, "id":xx, "groupid":xxx,"role":"可选"};{"msgid":8, "id":1, "groupid":1}
    GROUP_CHAT_MSG,//群聊发送消息信息   对应json中应该至少有{"msgid":9, "from":xx, "groupid":xxx,"msg":"xxxxxxxxxxxxxxxxxxx(msg)"};

    LOGOUT_MSG    //退出登录信息   对应json中应该至少有{"msgid":10, "id":xx};

    //{"msgid":7, "id":3, "groupname":"lovehome","groupdesc":"lovehome family"}
    //{"msgid":9, "from":1, "groupid":1,"msg":"hello everyone i am zhang san."}
    //{"msgid":5, "from":1, "to":4,"msg":"hello zihao."}

};








#endif