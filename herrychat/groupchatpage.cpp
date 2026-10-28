#include "groupchatpage.h"
#include "usermgr.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QDebug>

GroupChatPage::GroupChatPage(QWidget *parent)
    : QWidget(parent)
{
    //顶部标题栏
    auto* title_layout = new QHBoxLayout;
    auto* title_lb = new QLabel(tr("群聊"));
    title_lb->setObjectName("group_chat_title_lb");
    title_lb->setStyleSheet("font-size: 16px; font-weight: bold; color: #333333;");
    _create_btn = new QPushButton(tr("+ 创建群聊"));
    _create_btn->setObjectName("create_group_btn");
    _create_btn->setStyleSheet(
        "QPushButton{background-color:#07C160;color:white;border:none;border-radius:6px;"
        "padding:6px 14px;font-size:13px;}"
        "QPushButton:hover{background-color:#06AD56;}");
    title_layout->addWidget(title_lb);
    title_layout->addStretch();
    title_layout->addWidget(_create_btn);

    //群列表
    _group_list = new QListWidget;
    _group_list->setObjectName("group_list");
    _group_list->setStyleSheet(
        "QListWidget{background-color:white;border:none;}"
        "QListWidget::item{height:64px;border-bottom:1px solid #F2F2F2;}"
        "QListWidget::item:hover{background-color:#F5F5F5;}"
        "QListWidget::item:selected{background-color:#E8F4EC;}");

    auto* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(10, 10, 10, 10);
    main_layout->addLayout(title_layout);
    main_layout->addWidget(_group_list);

    connect(_create_btn, &QPushButton::clicked, this, &GroupChatPage::sig_create_group);
    connect(_group_list, &QListWidget::itemClicked, this, &GroupChatPage::on_item_clicked);
}

void GroupChatPage::AddGroup(int thread_id, QString name, int owner_id)
{
    //避免重复添加
    for (int i = 0; i < _group_list->count(); ++i) {
        auto item = _group_list->item(i);
        if (item->data(Qt::UserRole).toInt() == thread_id) {
            return;
        }
    }

    auto* item = new QListWidgetItem;
    item->setData(Qt::UserRole, thread_id);

    //自绘条目：群图标 + 群名
    auto* wid = new QWidget;
    auto* layout = new QHBoxLayout(wid);
    layout->setContentsMargins(12, 8, 12, 8);

    auto* icon_lb = new QLabel;
    icon_lb->setFixedSize(40, 40);
    //群头像（群主设置过则用群头像，否则默认群图标）
    auto group = UserMgr::GetInstance()->GetGroupById(thread_id);
    QPixmap icon;
    if (group != nullptr && !group->_icon.isEmpty()) {
        icon.load(group->_icon);
    }
    if (icon.isNull()) {
        icon.load(":/res/group_icon.png");
    }
    icon_lb->setPixmap(icon.scaled(icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    icon_lb->setScaledContents(true);

    auto* name_lb = new QLabel(name);
    name_lb->setStyleSheet("font-size:14px; color:#333333;");

    auto* owner_lb = new QLabel;
    auto self = UserMgr::GetInstance()->GetUserInfo();
    if (self && self->_uid == owner_id) {
        owner_lb->setText(tr("群主"));
        owner_lb->setStyleSheet("font-size:11px; color:#07C160;");
    } else {
        owner_lb->setText(tr(""));
    }

    layout->addWidget(icon_lb);
    layout->addSpacing(10);
    layout->addWidget(name_lb);
    layout->addStretch();
    layout->addWidget(owner_lb);

    item->setSizeHint(QSize(0, 64));
    _group_list->addItem(item);
    _group_list->setItemWidget(item, wid);
}

void GroupChatPage::ClearGroups()
{
    _group_list->clear();
}

void GroupChatPage::RefreshGroups()
{
    _group_list->clear();
    auto groups = UserMgr::GetInstance()->GetGroupList();
    for (auto& group : groups) {
        //备注优先显示（仿微信"备注群名"）
        QString display = group->_remark.isEmpty() ? group->_name : group->_remark;
        AddGroup(group->_thread_id, display, group->_owner_id);
    }
}

void GroupChatPage::on_item_clicked(QListWidgetItem *item)
{
    int thread_id = item->data(Qt::UserRole).toInt();
    emit sig_jump_group_chat(thread_id);
}
