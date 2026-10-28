#ifndef USERINFOPAGE_H
#define USERINFOPAGE_H

#include <QWidget>
#include "global.h"

namespace Ui {
class UserInfoPage;
}

class UserInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit UserInfoPage(QWidget *parent = nullptr);
    ~UserInfoPage();

private slots:
    void on_up_btn_clicked();
    void on_submit_btn_clicked();
    void slot_userinfo_mod_finish(ReqId id, QString res, ErrorCodes err);

private:
    Ui::UserInfoPage *ui;
};

#endif // USERINFOPAGE_H
