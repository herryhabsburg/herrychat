#pragma once
#include <string>
#include <vector>
#include <memory>
struct UserInfo {
	UserInfo():name(""), pwd(""),uid(0),email(""),nick(""),desc(""),sex(0), icon(""), back("") {}
	std::string name;
	std::string pwd;
	int uid;
	std::string email;
	std::string nick;
	std::string desc;
	int sex;
	std::string icon;
	std::string back;
};

struct ApplyInfo {
	ApplyInfo(int uid, std::string name, std::string desc,
		std::string icon, std::string nick, int sex, int status)
		:_uid(uid),_name(name),_desc(desc),
		_icon(icon),_nick(nick),_sex(sex),_status(status){}

	int _uid;
	std::string _name;
	std::string _desc;
	std::string _icon;
	std::string _nick;
	int _sex;
	int _status;
};

//聊天线程信息
struct ChatThreadInfo {
	int _thread_id;
	std::string _type;     // "private" or "group"
	int _user1_id;    // 私聊时对应 private_chat.user1_id；群聊时设为 0
	int _user2_id;    // 私聊时对应 private_chat.user2_id；群聊时设为 0
};

//聊天消息信息
struct ChatMessage {
	int message_id;
	int thread_id;
	int sender_id;
	int recv_id;
	std::string unique_id;
	std::string content;
	std::string chat_time;
	int status;
};

// 查询结果结构，增加next_cursor字段
struct PageResult {
	std::vector<ChatMessage> messages;
	bool load_more;
	int next_cursor;  // 本页最后一条message_id，用于下次查询
};

//群聊信息
struct GroupInfo {
	GroupInfo() : _thread_id(0), _name(""), _owner_id(0), _icon(""), _remark("") {}
	int _thread_id;
	std::string _name;
	int _owner_id;
	std::string _icon;   //群头像
	std::string _remark; //我(当前用户)的本群备注
};

//群成员信息（含角色）
struct GroupMember {
	std::shared_ptr<UserInfo> _info;
	int _role; //0=普通 1=管理员 2=创建者(群主)
	std::string _group_nick; //群昵称
	std::string _remark;     //本群备注
};
