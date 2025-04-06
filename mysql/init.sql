USE chat;

CREATE TABLE IF NOT EXISTS User(
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '用户ID',
    name VARCHAR(50) NOT NULL UNIQUE COMMENT '用户名',
    password VARCHAR(50) NOT NULL COMMENT '用户密码',
    state ENUM('online','offline') DEFAULT 'offline' COMMENT '用户登陆状态'
);

CREATE TABLE IF NOT EXISTS Friend(
    userid INT NOT NULL COMMENT '用户ID',
    friendid INT NOT NULL COMMENT '朋友ID',
    PRIMARY KEY(userid,friendid)
);

CREATE TABLE IF NOT EXISTS ALLGroup(
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '组ID',
    groupname VARCHAR(50) NOT NULL COMMENT '组名',
    groupdesc VARCHAR(200) DEFAULT '' COMMENT '组描述'
);

CREATE TABLE IF NOT EXISTS GroupUser(
    groupid INT PRIMARY KEY COMMENT '组ID',
    userid INT NOT NULL COMMENT '组员ID',
    grouprole ENUM('creator','normal') DEFAULT 'normal' COMMENT '组描述'
);

CREATE TABLE IF NOT EXISTS OfflineMessage(
    userid INT COMMENT '用户ID',
    message VARCHAR(5000) NOT NULL COMMENT '离线消息'
);