#include "groupmemberdialog.h"
#include "usermgr.h"
#include "creategroupdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include "tcpmgr.h"
#include <QDebug>

GroupMemberDialog::GroupMemberDialog(int thread_id, int owner_id, QWidget *parent)
    : QDialog(parent), _thread_id(thread_id), _owner_id(owner_id)
{
    auto self = UserMgr::GetInstance()->GetUserInfo();
    _self_uid = (self != nullptr) ? self->_uid : 0;
    _is_owner = (_self_uid == owner_id);

    setWindowTitle(tr("群聊信息"));
    setFixedSize(400, 620);

    auto* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(16, 16, 16, 16);
    main_layout->setSpacing(10);

    //======== 顶部群信息区：群头像 + 群名 ========
    auto* info_layout = new QHBoxLayout;
    info_layout->setSpacing(12);

    _group_icon_lb = new QLabel;
    _group_icon_lb->setFixedSize(64, 64);
    _group_icon_lb->setStyleSheet(
        "background:#F2F2F2;border-radius:10px;border:1px solid #E5E5E5;");

    auto* name_area = new QVBoxLayout;
    auto* name_lb = new QLabel(tr("群聊名称"));
    name_lb->setStyleSheet("font-size:12px; color:#999999;");
    _name_edit = new QLineEdit;
    _name_edit->setStyleSheet(
        "QLineEdit{border:1px solid #E5E5E5;border-radius:6px;padding:6px 8px;font-size:14px;}"
        "QLineEdit:focus{border:1px solid #07C160;}");
    _name_edit->setEnabled(_is_owner);

    name_area->addWidget(name_lb);
    name_area->addWidget(_name_edit);

    info_layout->addWidget(_group_icon_lb);
    info_layout->addLayout(name_area, 1);

    if (_is_owner) {
        auto* btn_area = new QVBoxLayout;
        auto* icon_btn = new QPushButton(tr("更换头像"));
        auto* rename_btn = new QPushButton(tr("修改"));
        icon_btn->setStyleSheet(
            "QPushButton{background-color:#F2F2F2;color:#333333;border:none;border-radius:6px;padding:6px 8px;font-size:12px;}"
            "QPushButton:hover{background-color:#E5E5E5;}");
        rename_btn->setStyleSheet(
            "QPushButton{background-color:#07C160;color:white;border:none;border-radius:6px;padding:6px 12px;font-size:12px;}"
            "QPushButton:hover{background-color:#06AD56;}");
        btn_area->addWidget(icon_btn);
        btn_area->addWidget(rename_btn);
        info_layout->addLayout(btn_area);
        connect(icon_btn, &QPushButton::clicked, this, &GroupMemberDialog::on_change_icon);
        connect(rename_btn, &QPushButton::clicked, this, &GroupMemberDialog::on_change_name);
    }

    //======== 我的群设置区：群昵称 + 备注 ========
    auto* nick_layout = new QHBoxLayout;
    auto* nick_lb = new QLabel(tr("我的群昵称"));
    nick_lb->setStyleSheet("font-size:13px; color:#333333;");
    _nick_edit = new QLineEdit;
    _nick_edit->setPlaceholderText(tr("在群内显示的昵称"));
    _nick_edit->setStyleSheet(
        "QLineEdit{border:1px solid #E5E5E5;border-radius:6px;padding:6px 8px;font-size:13px;}"
        "QLineEdit:focus{border:1px solid #07C160;}");
    auto* nick_btn = new QPushButton(tr("修改"));
    nick_btn->setStyleSheet(
        "QPushButton{background-color:#F2F2F2;color:#333333;border:none;border-radius:6px;padding:6px 10px;font-size:12px;}"
        "QPushButton:hover{background-color:#E5E5E5;}");
    nick_layout->addWidget(nick_lb);
    nick_layout->addWidget(_nick_edit, 1);
    nick_layout->addWidget(nick_btn);
    connect(nick_btn, &QPushButton::clicked, this, &GroupMemberDialog::on_change_nick);

    auto* remark_layout = new QHBoxLayout;
    auto* remark_lb = new QLabel(tr("本群备注"));
    remark_lb->setStyleSheet("font-size:13px; color:#333333;");
    _remark_edit = new QLineEdit;
    _remark_edit->setPlaceholderText(tr("备注显示在会话列表"));
    _remark_edit->setStyleSheet(
        "QLineEdit{border:1px solid #E5E5E5;border-radius:6px;padding:6px 8px;font-size:13px;}"
        "QLineEdit:focus{border:1px solid #07C160;}");
    auto* remark_btn = new QPushButton(tr("修改"));
    remark_btn->setStyleSheet(
        "QPushButton{background-color:#F2F2F2;color:#333333;border:none;border-radius:6px;padding:6px 10px;font-size:12px;}"
        "QPushButton:hover{background-color:#E5E5E5;}");
    remark_layout->addWidget(remark_lb);
    remark_layout->addWidget(_remark_edit, 1);
    remark_layout->addWidget(remark_btn);
    connect(remark_btn, &QPushButton::clicked, this, &GroupMemberDialog::on_change_remark);

    //======== 群成员区 ========
    auto* member_title_layout = new QHBoxLayout;
    auto* member_lb = new QLabel(tr("群成员"));
    member_lb->setStyleSheet("font-size:13px; font-weight:bold; color:#333333;");
    member_title_layout->addWidget(member_lb);
    member_title_layout->addStretch();
    if (_is_owner) {
        auto* add_btn = new QPushButton(tr("+ 拉人"));
        add_btn->setStyleSheet(
            "QPushButton{background-color:#07C160;color:white;border:none;border-radius:6px;padding:6px 12px;font-size:12px;}"
            "QPushButton:hover{background-color:#06AD56;}");
        member_title_layout->addWidget(add_btn);
        connect(add_btn, &QPushButton::clicked, this, &GroupMemberDialog::on_add_member);
    }

    _member_list = new QListWidget;
    _member_list->setStyleSheet(
        "QListWidget{border:1px solid #E5E5E5;border-radius:6px;background:white;}"
        "QListWidget::item{height:52px;border-bottom:1px solid #F2F2F2;}");

    main_layout->addLayout(info_layout);
    main_layout->addLayout(nick_layout);
    main_layout->addLayout(remark_layout);
    main_layout->addLayout(member_title_layout);
    main_layout->addWidget(_member_list, 1);

    //退出群聊（群主不显示）
    if (!_is_owner) {
        auto* quit_btn = new QPushButton(tr("退出群聊"));
        quit_btn->setStyleSheet(
            "QPushButton{background-color:#F5222D;color:white;border:none;border-radius:6px;padding:8px 0;font-size:13px;}"
            "QPushButton:hover{background-color:#D9121E;}");
        main_layout->addWidget(quit_btn);
        connect(quit_btn, &QPushButton::clicked, this, &GroupMemberDialog::on_quit_group);
    }

    //读取当前群名/群头像
    auto group = UserMgr::GetInstance()->GetGroupById(_thread_id);
    if (group != nullptr) {
        _name_edit->setText(group->_name);
        if (!group->_icon.isEmpty()) {
            QPixmap pix(group->_icon);
            _group_icon_lb->setPixmap(pix.scaled(_group_icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            _group_icon_lb->setPixmap(QPixmap(":/res/group_icon.png").scaled(_group_icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    //我的群昵称/备注回显
    auto members = UserMgr::GetInstance()->GetGroupMembers(_thread_id);
    for (auto& m : members) {
        if (m->_uid == _self_uid) {
            _nick_edit->setText(m->_group_nick);
            _remark_edit->setText(m->_remark);
            break;
        }
    }

    connect(this, &GroupMemberDialog::sig_group_members_loaded, this, &GroupMemberDialog::refresh_member_list);

    //服务器回包后再刷新列表
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_load_group_members, this,
            [this](int thread_id, QJsonArray) {
        if (thread_id == _thread_id) {
            //回显我的群昵称/备注
            auto members = UserMgr::GetInstance()->GetGroupMembers(_thread_id);
            for (auto& m : members) {
                if (m->_uid == _self_uid) {
                    _nick_edit->setText(m->_group_nick);
                    _remark_edit->setText(m->_remark);
                    break;
                }
            }
            refresh_member_list();
        }
    });

    //群头像修改回包后刷新预览
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_group_icon_rsp, this,
            [this](int thread_id, QString icon) {
        if (thread_id == _thread_id && !icon.isEmpty()) {
            QPixmap pix(icon);
            _group_icon_lb->setPixmap(pix.scaled(_group_icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    });

    //设置/取消管理员回包后刷新成员列表（角色标签、按钮变化）
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_group_admin_rsp, this,
            [this](int thread_id, int, int) {
        if (thread_id == _thread_id) {
            refresh_member_list();
        }
    });

    load_members();
}

void GroupMemberDialog::load_members()
{
    //请求服务器加载群成员
    QJsonObject jsonObj;
    jsonObj["thread_id"] = _thread_id;
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_LOAD_GROUP_MEMBER_REQ, jsonData);
}

QString GroupMemberDialog::display_name(const std::shared_ptr<GroupMemberInfo>& member)
{
    if (!member->_group_nick.isEmpty()) {
        return member->_group_nick;
    }
    if (!member->_nick.isEmpty()) {
        return member->_nick;
    }
    return member->_name;
}

void GroupMemberDialog::refresh_member_list()
{
    _member_list->clear();
    auto members = UserMgr::GetInstance()->GetGroupMembers(_thread_id);
    if (members.empty()) {
        return;
    }

    for (auto& member : members) {
        auto* item = new QListWidgetItem;
        item->setSizeHint(QSize(0, 52));

        auto* wid = new QWidget;
        auto* layout = new QHBoxLayout(wid);
        layout->setContentsMargins(12, 5, 12, 5);

        //头像
        auto* icon_lb = new QLabel;
        icon_lb->setFixedSize(38, 38);
        QPixmap icon(member->_icon);
        if (icon.isNull()) {
            icon.load(":/res/head_1.jpg");
        }
        icon_lb->setPixmap(icon.scaled(icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        icon_lb->setScaledContents(true);

        //名字（群昵称优先）
        auto* name_lb = new QLabel(display_name(member));
        name_lb->setStyleSheet("font-size:14px; color:#333333;");

        //角色标签
        auto* role_lb = new QLabel;
        if (member->_role == 2) {
            role_lb->setText(tr("群主"));
            role_lb->setStyleSheet("font-size:11px; color:#07C160;");
        } else if (member->_role == 1) {
            role_lb->setText(tr("管理员"));
            role_lb->setStyleSheet("font-size:11px; color:#FF9900;");
        }

        layout->addWidget(icon_lb);
        layout->addSpacing(8);
        layout->addWidget(name_lb);
        layout->addStretch();
        layout->addWidget(role_lb);

        //群主管理按钮：设置/取消管理员 + 移除
        if (_is_owner && member->_uid != _owner_id) {
            int uid = member->_uid;
            if (member->_role == 1) {
                auto* admin_btn = new QPushButton(tr("取消管理员"));
                admin_btn->setStyleSheet(
                    "QPushButton{background-color:#FF9900;color:white;border:none;border-radius:6px;padding:4px 8px;font-size:11px;}"
                    "QPushButton:hover{background-color:#E68A00;}");
                connect(admin_btn, &QPushButton::clicked, this, [this, uid]() { on_set_admin(uid, 0); });
                layout->addWidget(admin_btn);
            } else if (member->_role == 0) {
                auto* admin_btn = new QPushButton(tr("设管理员"));
                admin_btn->setStyleSheet(
                    "QPushButton{background-color:#FF9900;color:white;border:none;border-radius:6px;padding:4px 8px;font-size:11px;}"
                    "QPushButton:hover{background-color:#E68A00;}");
                connect(admin_btn, &QPushButton::clicked, this, [this, uid]() { on_set_admin(uid, 1); });
                layout->addWidget(admin_btn);
            }
            auto* remove_btn = new QPushButton(tr("移除"));
            remove_btn->setStyleSheet(
                "QPushButton{background-color:#F5222D;color:white;border:none;border-radius:6px;padding:4px 8px;font-size:11px;}"
                "QPushButton:hover{background-color:#D9121E;}");
            connect(remove_btn, &QPushButton::clicked, this, [this, uid]() { on_remove_member(uid); });
            layout->addWidget(remove_btn);
        }

        _member_list->addItem(item);
        _member_list->setItemWidget(item, wid);
    }
}

void GroupMemberDialog::on_add_member()
{
    if (!_is_owner) {
        return;
    }

    //弹好友勾选窗口（复用创建群的好友选择）
    auto* dlg = new CreateGroupDialog(this);
    dlg->setWindowTitle(tr("选择成员"));
    connect(dlg, &CreateGroupDialog::sig_create_group, this, [this](QString, QList<int> member_ids) {
        emit sig_group_member_added(_thread_id, member_ids);
        load_members();
    });
    dlg->exec();
    delete dlg;
}

void GroupMemberDialog::on_change_name()
{
    QString name = _name_edit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("群聊名称不能为空"));
        return;
    }
    emit sig_group_name_changed(_thread_id, name);
}

void GroupMemberDialog::on_change_nick()
{
    QString nick = _nick_edit->text().trimmed();
    if (nick.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("群昵称不能为空"));
        return;
    }
    emit sig_group_nick_changed(_thread_id, nick);
}

void GroupMemberDialog::on_change_remark()
{
    QString remark = _remark_edit->text().trimmed();
    emit sig_group_remark_changed(_thread_id, remark);
}

void GroupMemberDialog::on_change_icon()
{
    //群主选择图片作为群头像
    QString file = QFileDialog::getOpenFileName(this, tr("选择群头像"),
        QString(), tr("图片文件 (*.png *.jpg *.jpeg *.bmp)"));
    if (file.isEmpty()) {
        return;
    }

    //复制到 avatars 目录：group_<thread_id>.png
    QString app_dir = QCoreApplication::applicationDirPath();
    QDir dir(app_dir);
    if (!dir.exists("avatars")) {
        dir.mkdir("avatars");
    }
    QString dest = app_dir + "/avatars/group_" + QString::number(_thread_id) + ".png";
    QFile::remove(dest);
    if (!QFile::copy(file, dest)) {
        QMessageBox::warning(this, tr("提示"), tr("头像保存失败"));
        return;
    }

    emit sig_group_icon_changed(_thread_id, "avatars/group_" + QString::number(_thread_id) + ".png");

    //本地立即预览
    QPixmap pix(dest);
    _group_icon_lb->setPixmap(pix.scaled(_group_icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void GroupMemberDialog::on_set_admin(int uid, int role)
{
    emit sig_group_admin_changed(_thread_id, uid, role);
    //等待回包后刷新
}

void GroupMemberDialog::on_remove_member(int uid)
{
    auto ret = QMessageBox::question(this, tr("确认"), tr("确定将该成员移出群聊？"));
    if (ret != QMessageBox::Yes) {
        return;
    }
    emit sig_group_member_removed(_thread_id, uid);
}

void GroupMemberDialog::on_quit_group()
{
    auto ret = QMessageBox::question(this, tr("确认"), tr("确定退出该群聊？"));
    if (ret != QMessageBox::Yes) {
        return;
    }
    emit sig_quit_group(_thread_id);
}
