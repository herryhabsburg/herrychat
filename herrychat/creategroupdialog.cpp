#include "creategroupdialog.h"
#include "usermgr.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>
#include <QDebug>

CreateGroupDialog::CreateGroupDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("发起群聊"));
    setFixedSize(360, 480);

    auto* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(16, 16, 16, 16);
    main_layout->setSpacing(10);

    auto* name_lb = new QLabel(tr("群聊名称："));
    name_lb->setStyleSheet("font-size:13px; color:#333333;");
    _name_edit = new QLineEdit;
    _name_edit->setPlaceholderText(tr("请输入群聊名称"));
    _name_edit->setStyleSheet(
        "QLineEdit{border:1px solid #E5E5E5;border-radius:6px;padding:6px 8px;font-size:13px;}"
        "QLineEdit:focus{border:1px solid #07C160;}");

    auto* friend_lb = new QLabel(tr("选择成员（可多选）："));
    friend_lb->setStyleSheet("font-size:13px; color:#333333;");

    _friend_list = new QListWidget;
    _friend_list->setStyleSheet(
        "QListWidget{border:1px solid #E5E5E5;border-radius:6px;background:white;}"
        "QListWidget::item{height:44px;}");

    //从好友列表加载可勾选项
    //加载全部好友（避免分页计数影响，保证创建群时可选择所有好友）
    auto friends = UserMgr::GetInstance()->GetAllFriends();
    if (friends.empty()) {
        auto* empty_lb = new QLabel(tr("暂无好友，请先添加好友后再创建群聊"));
        empty_lb->setStyleSheet("font-size:13px; color:#999999;");
        empty_lb->setAlignment(Qt::AlignCenter);
        _friend_list->addItem(new QListWidgetItem);
        _friend_list->setItemWidget(_friend_list->item(0), empty_lb);
        _friend_list->item(0)->setSizeHint(QSize(0, 120));
    }
    for (auto& friend_info : friends) {
        auto* item = new QListWidgetItem;
        item->setSizeHint(QSize(0, 44));
        _friend_list->addItem(item);
        auto* check = new QCheckBox(friend_info->_name);
        check->setProperty("uid", friend_info->_uid);
        check->setStyleSheet("font-size:13px; color:#333333;");
        _friend_list->setItemWidget(item, check);
    }

    auto* btn_layout = new QHBoxLayout;
    auto* cancel_btn = new QPushButton(tr("取消"));
    auto* confirm_btn = new QPushButton(tr("创建"));
    cancel_btn->setStyleSheet(
        "QPushButton{background-color:#F2F2F2;color:#333333;border:none;border-radius:6px;padding:8px 0;font-size:13px;}"
        "QPushButton:hover{background-color:#E5E5E5;}");
    confirm_btn->setStyleSheet(
        "QPushButton{background-color:#07C160;color:white;border:none;border-radius:6px;padding:8px 0;font-size:13px;}"
        "QPushButton:hover{background-color:#06AD56;}");
    btn_layout->addWidget(cancel_btn);
    btn_layout->addWidget(confirm_btn);

    main_layout->addWidget(name_lb);
    main_layout->addWidget(_name_edit);
    main_layout->addWidget(friend_lb);
    main_layout->addWidget(_friend_list, 1);
    main_layout->addLayout(btn_layout);

    connect(cancel_btn, &QPushButton::clicked, this, &CreateGroupDialog::on_cancel);
    connect(confirm_btn, &QPushButton::clicked, this, &CreateGroupDialog::on_confirm);
}

void CreateGroupDialog::on_confirm()
{
    QString name = _name_edit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请输入群聊名称"));
        return;
    }

    QList<int> member_ids;
    for (int i = 0; i < _friend_list->count(); ++i) {
        auto* item = _friend_list->item(i);
        auto* wid = _friend_list->itemWidget(item);
        auto* check = qobject_cast<QCheckBox*>(wid);
        if (check && check->isChecked()) {
            member_ids.push_back(check->property("uid").toInt());
        }
    }

    if (member_ids.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请至少选择一名成员"));
        return;
    }

    emit sig_create_group(name, member_ids);
    accept();
}

void CreateGroupDialog::on_cancel()
{
    reject();
}
