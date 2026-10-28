#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include "QStyle"
#include <QRegularExpression>
#include <QMessageBox>
#include <QWidget>
#include <memory>
#include <mutex>
#include <iostream>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDir>
//用来刷新QSS的
extern std::function<void(QWidget*)> repolish;
enum reqid
{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
};
enum errorcodes{
    SUCCESS = 0,
    ERR_JSON = 1, //Json解析失败
    ERR_NETWORK = 2,//网络错误
};
enum modules{
    REGISTERMOD = 0,
};
extern QString gate_url_prefix;

#endif // GLOBAL_H