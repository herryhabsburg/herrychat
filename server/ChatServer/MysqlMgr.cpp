#include "MysqlMgr.h"


MysqlMgr::~MysqlMgr() {

}

int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	return _dao.RegUser(name, email, pwd);
}

bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email) {
	return _dao.CheckEmail(name, email);
}

bool MysqlMgr::UpdatePwd(const std::string& name, const std::string& pwd) {
	return _dao.UpdatePwd(name, pwd);
}

MysqlMgr::MysqlMgr() {
}

bool MysqlMgr::CheckPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo) {
	return _dao.CheckPwd(name, pwd, userInfo);
}

bool MysqlMgr::AddFriendApply(const int& from, const int& to, const std::string& desc, const std::string& back_name)
{
	return _dao.AddFriendApply(from, to, desc, back_name);
}

bool MysqlMgr::AuthFriendApply(const int& from, const int& to) {
	return _dao.AuthFriendApply(from, to);
}

bool MysqlMgr::AddFriend(const int& from, const int& to, std::string back_name, 
	std::vector<std::shared_ptr<AddFriendMsg>>& msg_list) {
	return _dao.AddFriend(from, to, back_name, msg_list);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(int uid)
{
	return _dao.GetUser(uid);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(std::string name)
{
	return _dao.GetUser(name);
}

bool MysqlMgr::GetApplyList(int touid, 
	std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit) {

	return _dao.GetApplyList(touid, applyList, begin, limit);
}

bool MysqlMgr::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info) {
	return _dao.GetFriendList(self_id, user_info);
}

bool MysqlMgr::GetUserThreads(int64_t userId,
	int64_t lastId,
	int      pageSize,
	std::vector<std::shared_ptr<ChatThreadInfo>>& threads,
	bool& loadMore,
	int& nextLastId)
{
	return _dao.GetUserThreads(userId, lastId, pageSize, threads, loadMore, nextLastId);
}

bool MysqlMgr::CreatePrivateChat(int user1_id, int user2_id, int& thread_id)
{
	return _dao.CreatePrivateChat(user1_id, user2_id, thread_id);
}

std::shared_ptr<PageResult> MysqlMgr::LoadChatMsg(int threadId, int lastId, int pageSize)
{
	return _dao.LoadChatMsg(threadId, lastId, pageSize);
}

bool MysqlMgr::AddChatMsg(std::vector<std::shared_ptr<ChatMessage>>& chat_datas) {
	return _dao.AddChatMsg(chat_datas);
}

// ===== 群聊 =====
bool MysqlMgr::CreateGroupChat(int owner_id, const std::string& name, const std::vector<int>& member_ids, int& thread_id) {
	return _dao.CreateGroupChat(owner_id, name, member_ids, thread_id);
}

bool MysqlMgr::GetUserGroups(int uid, std::vector<std::shared_ptr<GroupInfo>>& groups) {
	return _dao.GetUserGroups(uid, groups);
}

bool MysqlMgr::GetGroupMembers(int thread_id, std::vector<GroupMember>& members) {
	return _dao.GetGroupMembers(thread_id, members);
}

bool MysqlMgr::AddGroupMember(int thread_id, int uid, int role) {
	return _dao.AddGroupMember(thread_id, uid, role);
}

bool MysqlMgr::RemoveGroupMember(int thread_id, int uid) {
	return _dao.RemoveGroupMember(thread_id, uid);
}

bool MysqlMgr::UpdateGroupName(int thread_id, const std::string& name) {
	return _dao.UpdateGroupName(thread_id, name);
}

bool MysqlMgr::GetGroupOwner(int thread_id, int& owner_id) {
	return _dao.GetGroupOwner(thread_id, owner_id);
}

bool MysqlMgr::SetGroupRole(int thread_id, int target_uid, int role) {
	return _dao.SetGroupRole(thread_id, target_uid, role);
}

bool MysqlMgr::UpdateGroupNick(int thread_id, int uid, const std::string& nick) {
	return _dao.UpdateGroupNick(thread_id, uid, nick);
}

bool MysqlMgr::UpdateGroupRemark(int thread_id, int uid, const std::string& remark) {
	return _dao.UpdateGroupRemark(thread_id, uid, remark);
}

bool MysqlMgr::UpdateGroupIcon(int thread_id, const std::string& icon) {
	return _dao.UpdateGroupIcon(thread_id, icon);
}

bool MysqlMgr::QuitGroup(int thread_id, int uid) {
	return _dao.QuitGroup(thread_id, uid);
}

