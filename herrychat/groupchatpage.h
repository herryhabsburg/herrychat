#ifndef GROUPCHATPAGE_H
#define GROUPCHATPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <memory>
#include "userdata.h"

//群聊列表页：显示我加入的所有群聊，并提供"创建群聊"入口
class GroupChatPage : public QWidget
{
    Q_OBJECT
public:
    explicit GroupChatPage(QWidget *parent = nullptr);
    ~GroupChatPage() = default;
    void AddGroup(int thread_id, QString name, int owner_id);
    void ClearGroups();
    void RefreshGroups();

signals:
    void sig_jump_group_chat(int thread_id);
    void sig_create_group();

private slots:
    void on_item_clicked(QListWidgetItem *item);

private:
    QListWidget* _group_list;
    QPushButton* _create_btn;
};

#endif // GROUPCHATPAGE_H
