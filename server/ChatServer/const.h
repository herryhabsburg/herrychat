#pragma once
#include <functional>


enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,  //Json解析错误
	RPCFailed = 1002,  //RPC请求错误
	VarifyExpired = 1003, //验证码过期
	VarifyCodeErr = 1004, //验证码错误
	UserExist = 1005,       //用户已经存在
	PasswdErr = 1006,    //密码错误
	EmailNotMatch = 1007,  //邮箱不匹配
	PasswdUpFailed = 1008,  //更新密码失败
	PasswdInvalid = 1009,   //密码更新失败
	TokenInvalid = 1010,   //Token失效
	UidInvalid = 1011,  //uid无效
	CREATE_CHAT_FAILED = 1012, //创建聊天失败
	LOAD_CHAT_FAILED = 1013, //加载聊天失败
};


// Defer类
class Defer {
public:
	// 接受一个lambda表达式或者函数指针
	Defer(std::function<void()> func) : func_(func) {}

	// 析构函数中执行传入的函数
	~Defer() {
		func_();
	}

private:
	std::function<void()> func_;
};

#define MAX_LENGTH  1024*2
//头部总长度
#define HEAD_TOTAL_LEN 4
//头部id长度
#define HEAD_ID_LEN 2
//头部数据长度
#define HEAD_DATA_LEN 2
#define MAX_RECVQUE  10000
#define MAX_SENDQUE 1000


enum MSG_IDS {
	MSG_CHAT_LOGIN = 1005, //用户登陆
	MSG_CHAT_LOGIN_RSP = 1006, //用户登陆回包
	ID_SEARCH_USER_REQ = 1007, //用户搜索请求
	ID_SEARCH_USER_RSP = 1008, //搜索用户回包
	ID_ADD_FRIEND_REQ = 1009, //申请添加好友请求
	ID_ADD_FRIEND_RSP  = 1010, //申请添加好友回复
	ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友申请
	ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
	ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
	ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
	ID_TEXT_CHAT_MSG_REQ = 1017, //文本聊天信息请求
	ID_TEXT_CHAT_MSG_RSP = 1018, //文本聊天信息回复
	ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
	ID_NOTIFY_OFF_LINE_REQ = 1021, //通知用户下线
	ID_HEART_BEAT_REQ = 1023,      //心跳请求
	ID_HEARTBEAT_RSP = 1024,       //心跳回复
	ID_LOAD_CHAT_THREAD_REQ = 1025, //加载聊天线程请求
	ID_LOAD_CHAT_THREAD_RSP = 1026, //加载聊天线程回复
	ID_CREATE_PRIVATE_CHAT_REQ = 1027, //创建私聊请求
	ID_CREATE_PRIVATE_CHAT_RSP = 1028, //创建私聊回复

	ID_LOAD_CHAT_MSG_REQ = 1029,      //加载聊天消息
	ID_LOAD_CHAT_MSG_RSP = 1030,      //加载聊天消息

	ID_CREATE_GROUP_CHAT_REQ = 1032,   //创建群聊请求
	ID_CREATE_GROUP_CHAT_RSP = 1033,   //创建群聊回复
	ID_GROUP_CHAT_MSG_REQ = 1034,      //群聊文本消息请求
	ID_GROUP_CHAT_MSG_RSP = 1035,      //群聊文本消息回复
	ID_NOTIFY_GROUP_CHAT_MSG_REQ = 1036, //群聊消息通知
	ID_LOAD_GROUP_MEMBER_REQ = 1037,   //加载群成员
	ID_LOAD_GROUP_MEMBER_RSP = 1038,   //加载群成员回复
	ID_GROUP_ADD_MEMBER_REQ = 1039,    //拉人进群
	ID_GROUP_ADD_MEMBER_RSP = 1040,    //拉人进群回复
	ID_NOTIFY_GROUP_ADD_MEMBER = 1041, //通知被拉成员
	ID_GROUP_MEMBER_REMOVE_REQ = 1042, //移除群成员
	ID_GROUP_MEMBER_REMOVE_RSP = 1043, //移除群成员回复
	ID_NOTIFY_GROUP_MEMBER_REMOVE = 1044, //通知被移除成员
	ID_GROUP_UPDATE_NAME_REQ = 1045,   //群主改群名
	ID_GROUP_UPDATE_NAME_RSP = 1046,   //群主改群名回复
	ID_NOTIFY_GROUP_UPDATE_NAME = 1047, //通知群名变更

	ID_SET_GROUP_ADMIN_REQ = 1048,     //群主设置/取消管理员
	ID_SET_GROUP_ADMIN_RSP = 1049,     //设置管理员回复
	ID_NOTIFY_GROUP_ROLE_CHANGE = 1050, //通知成员角色变化
	ID_UPDATE_GROUP_ICON_REQ = 1051,   //群主改群头像
	ID_UPDATE_GROUP_ICON_RSP = 1052,   //改群头像回复
	ID_NOTIFY_GROUP_ICON = 1053,       //通知群头像变更
	ID_UPDATE_GROUP_NICK_REQ = 1054,   //修改我的群昵称
	ID_UPDATE_GROUP_NICK_RSP = 1055,   //修改群昵称回复
	ID_UPDATE_GROUP_REMARK_REQ = 1056, //修改我的本群备注
	ID_UPDATE_GROUP_REMARK_RSP = 1057, //修改备注回复
	ID_QUIT_GROUP_REQ = 1058,          //退出群聊
	ID_QUIT_GROUP_RSP = 1059,          //退出群聊回复
	ID_NOTIFY_GROUP_QUIT = 1060,       //通知群主成员退出
};

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define NAME_INFO  "nameinfo_"
#define LOCK_PREFIX "lock_"
#define USER_SESSION_PREFIX "usession_"
#define LOCK_COUNT "lockcount"

//分布式锁的持有时间
#define LOCK_TIME_OUT 10
//分布式锁的重试时间
#define ACQUIRE_TIME_OUT 5


