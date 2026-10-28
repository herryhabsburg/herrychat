#ifndef CREATEGROUPDIALOG_H
#define CREATEGROUPDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>

//创建群聊弹窗：输入群名 + 勾选好友
class CreateGroupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CreateGroupDialog(QWidget *parent = nullptr);
    ~CreateGroupDialog() = default;

signals:
    void sig_create_group(QString name, QList<int> member_ids);

private slots:
    void on_confirm();
    void on_cancel();

private:
    QLineEdit* _name_edit;
    QListWidget* _friend_list;
};

#endif // CREATEGROUPDIALOG_H
