#ifndef USERMGR_H
#define USERMGR_H
#include <QObject>
#include <memory>
#include <singleton.h>
#include "userdata.h"
#include <vector>
class UserMgr:public QObject,public Singleton<UserMgr>,
        public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~ UserMgr();
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    void SetToken(QString token);
    int GetUid();
    QString GetName();
    QString GetNick();
    QString GetIcon();
    QString GetDesc();
    void SetName(QString name);
    void SetNick(QString nick);
    void SetDesc(QString desc);
    void SetIcon(QString icon);
     std::shared_ptr<UserInfo> GetUserInfo();
    void AppendApplyList(QJsonArray array);
    void AppendFriendList(QJsonArray array);
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    void AddApplyList(std::shared_ptr<ApplyInfo> app);
    bool AlreadyApply(int uid);
    std::vector<std::shared_ptr<UserInfo>> GetChatListPerPage();
    bool IsLoadChatFin();
    void UpdateChatLoadedCount();
    std::vector<std::shared_ptr<UserInfo>> GetConListPerPage();
    //返回全部好友（用于创建群聊等需要全量好友的场景）
    std::vector<std::shared_ptr<UserInfo>> GetAllFriends();
    void UpdateContactLoadedCount();
    bool IsLoadConFin();
    bool CheckFriendById(int uid);
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);
    std::shared_ptr<UserInfo> GetFriendById(int uid);
    int GetLastChatThreadId();
    void SetLastChatThreadId(int id);
    void AddChatThreadData(std::shared_ptr<ChatThreadData> chat_thread_data, int other_uid);
    int GetThreadIdByUid(int uid);
    std::shared_ptr<ChatThreadData> GetChatThreadByThreadId(int thread_id);
    std::shared_ptr<ChatThreadData> GetChatThreadByUid(int uid);

    //获取当前正在加载的聊天数据。
    std::shared_ptr<ChatThreadData> GetCurLoadData();
    std::shared_ptr<ChatThreadData> GetNextLoadData();

    // ===== 群聊 =====
    void AppendGroupList(QJsonArray array);
    void AddGroupInfo(std::shared_ptr<GroupInfo> group);
    std::vector<std::shared_ptr<GroupInfo>> GetGroupList();
    std::shared_ptr<GroupInfo> GetGroupById(int thread_id);
    void UpdateGroupName(int thread_id, QString name);
    void UpdateGroupIcon(int thread_id, QString icon);
    void AppendGroupMembers(int thread_id, QJsonArray array);
    std::vector<std::shared_ptr<GroupMemberInfo>> GetGroupMembers(int thread_id);
    //群管理本地缓存更新
    void UpdateGroupMemberRole(int thread_id, int uid, int role);
    void UpdateGroupMemberNick(int thread_id, int uid, QString group_nick);
    void UpdateGroupMemberRemark(int thread_id, int uid, QString remark);
    void RemoveGroupMemberLocal(int thread_id, int uid);
    void RemoveGroupLocal(int thread_id);
private:
    UserMgr();
    std::shared_ptr<UserInfo> _user_info;
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    std::vector<std::shared_ptr<UserInfo>> _friend_list;
    QMap<int, std::shared_ptr<UserInfo>> _friend_map;
    QString _token;
    int _chat_loaded;
    int _contact_loaded;
    //建立会话id到数据的映射关系
    QMap<int, std::shared_ptr<ChatThreadData>> _chat_map;
    //聊天会话id列表
    std::vector<int> _chat_thread_ids;
    //记录已经加载聊天列表的会话索引
    int _cur_load_chat_index;
    //上次会话的id
    int _last_chat_thread_id;
    //缓存其他用户uid和聊天的thread_id的映射关系。
    QMap<int, int> _uid_to_thread_id;
    // ===== 群聊 =====
    QMap<int, std::shared_ptr<GroupInfo>> _group_map;
    QMap<int, std::vector<std::shared_ptr<GroupMemberInfo>>> _group_members_map;

public slots:
    void SlotAddFriendRsp(std::shared_ptr<AuthRsp> rsp);
    void SlotAddFriendAuth(std::shared_ptr<AuthInfo> auth);
};

#endif // USERMGR_H
