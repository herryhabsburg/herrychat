#ifndef GROUPMEMBERDIALOG_H
#define GROUPMEMBERDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include "userdata.h"

//群管理弹窗：群主可改群名/群头像/拉人/移除成员/设置管理员；
//普通成员可修改自己的群昵称、本群备注、退出群聊
class GroupMemberDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GroupMemberDialog(int thread_id, int owner_id, QWidget *parent = nullptr);
    ~GroupMemberDialog() = default;

signals:
    void sig_group_name_changed(int thread_id, QString name);
    void sig_group_member_removed(int thread_id, int target_uid);
    void sig_group_member_added(int thread_id, QList<int> member_ids);
    void sig_group_members_loaded(int thread_id);
    void sig_group_admin_changed(int thread_id, int target_uid, int role);
    void sig_group_nick_changed(int thread_id, QString nick);
    void sig_group_remark_changed(int thread_id, QString remark);
    void sig_group_icon_changed(int thread_id, QString icon);
    void sig_quit_group(int thread_id);

private slots:
    void on_add_member();
    void on_change_name();
    void on_remove_member(int uid);
    void on_set_admin(int uid, int role);
    void on_change_nick();
    void on_change_remark();
    void on_change_icon();
    void on_quit_group();

private:
    void load_members();
    void refresh_member_list();
    QString display_name(const std::shared_ptr<GroupMemberInfo>& member);

    int _thread_id;
    int _owner_id;
    bool _is_owner;
    int _self_uid;
    QListWidget* _member_list;
    QLineEdit* _name_edit;
    QLineEdit* _nick_edit;
    QLineEdit* _remark_edit;
    QLabel* _group_icon_lb;
};

#endif // GROUPMEMBERDIALOG_H
