#include "userinfopage.h"
#include "ui_userinfopage.h"
#include "usermgr.h"
#include <QDebug>
#include <QMessageBox>
#include "imagecropperdialog.h"
#include "imagecropperlabel.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include "httpmgr.h"
#include <QJsonDocument>

UserInfoPage::UserInfoPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserInfoPage)
{
    ui->setupUi(this);
    auto icon = UserMgr::GetInstance()->GetIcon();
    qDebug() << "icon is " << icon ;
    QPixmap pixmap(icon);
    QPixmap scaledPixmap = pixmap.scaled( ui->head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); // 将图片缩放到label的大小
    ui->head_lb->setPixmap(scaledPixmap); // 将缩放后的图片设置到QLabel上
    ui->head_lb->setScaledContents(true); // 设置QLabel自动缩放图片内容以适应大小
    //获取nick
    auto nick = UserMgr::GetInstance()->GetNick();
    //获取name
    auto name = UserMgr::GetInstance()->GetName();
    //描述
    auto desc = UserMgr::GetInstance()->GetDesc();
    ui->nick_ed->setText(nick);
    ui->name_ed->setText(name);
    ui->desc_ed->setText(desc);
    //连接更新用户信息接口返回
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_userinfo_mod_finish, this,
            &UserInfoPage::slot_userinfo_mod_finish);
}

UserInfoPage::~UserInfoPage()
{
    delete ui;
}

//上传头像
void UserInfoPage::on_up_btn_clicked()
{
    // 1. 让对话框也能选 *.webp
    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("选择图片"),
        QString(),
        tr("图片文件 (*.png *.jpg *.jpeg *.bmp *.webp)")
    );
    if (filename.isEmpty())
        return;

    // 2. 直接用 QPixmap::load() 加载，无需手动区分格式
    QPixmap inputImage;
    if (!inputImage.load(filename)) {
        QMessageBox::critical(
            this,
            tr("错误"),
            tr("加载图片失败！请确认已部署 WebP 插件。"),
            QMessageBox::Ok
        );
        return;
    }

    QPixmap image = ImageCropperDialog::getCroppedImage(filename, 600, 400, CropperShape::CIRCLE);
    if (image.isNull())
        return;

    QPixmap scaledPixmap = image.scaled( ui->head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); // 将图片缩放到label的大小
    ui->head_lb->setPixmap(scaledPixmap); // 将缩放后的图片设置到QLabel上
    ui->head_lb->setScaledContents(true); // 设置QLabel自动缩放图片内容以适应大小

    // 保存到头像目录：可执行文件目录/avatars/head_<uid>.png
    QString storageDir = QCoreApplication::applicationDirPath();
    QDir dir(storageDir);
    if (!dir.exists("avatars")) {
        if (!dir.mkpath("avatars")) {
            qWarning() << "无法创建 avatars 目录：" << dir.filePath("avatars");
            QMessageBox::warning(
                this,
                tr("错误"),
                tr("无法创建存储目录，请检查权限或磁盘空间。")
            );
            return;
        }
    }
    // 3. 拼接最终的文件名 head_<uid>.png
    int uid = UserMgr::GetInstance()->GetUid();
    QString filePath = dir.filePath(QString("avatars/head_%1.png").arg(uid));

    // 4. 保存 scaledPixmap 为 PNG（无损、最高质量）
    if (!scaledPixmap.save(filePath, "PNG")) {
        QMessageBox::warning(
            this,
            tr("保存失败"),
            tr("头像保存失败，请检查权限或磁盘空间。")
        );
        return;
    }

    qDebug() << "头像已保存到：" << filePath;

    // 5. 上传到服务器，更新数据库头像字段
    QJsonObject json_obj;
    json_obj["uid"] = uid;
    json_obj["icon"] = QDir::toNativeSeparators(filePath);
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/update_icon"),
                                        json_obj, ReqId::ID_UPDATE_USER_REQ, Modules::USERINFOMOD);

    // 6. 更新本地用户信息，本窗口立即生效
    UserMgr::GetInstance()->SetIcon(QDir::toNativeSeparators(filePath));
}

//提交修改名字/昵称/描述
void UserInfoPage::on_submit_btn_clicked()
{
    auto user_info = UserMgr::GetInstance()->GetUserInfo();
    if (user_info == nullptr) {
        return;
    }

    //昵称不能为空
    if (ui->nick_ed->text().isEmpty()) {
        QMessageBox::information(this, "提示", "昵称不能为空");
        return;
    }

    QJsonObject json_obj;
    json_obj["uid"] = user_info->_uid;
    json_obj["name"] = ui->name_ed->text();
    json_obj["nick"] = ui->nick_ed->text();
    json_obj["desc"] = ui->desc_ed->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/update_name"),
                                        json_obj, ReqId::ID_UPDATE_USER_REQ, Modules::USERINFOMOD);
}

void UserInfoPage::slot_userinfo_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if (err != ErrorCodes::SUCCESS) {
        QMessageBox::information(this, "提示", "网络请求失败，请重试");
        return;
    }

    //解析json
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        QMessageBox::information(this, "提示", "响应解析失败");
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    int error = jsonObj["error"].toInt();
    if (error != 0) {
        QMessageBox::information(this, "提示", "保存失败，请重试");
        return;
    }

    //更新本地用户信息，主界面立即生效
    UserMgr::GetInstance()->SetName(ui->name_ed->text());
    UserMgr::GetInstance()->SetNick(ui->nick_ed->text());
    UserMgr::GetInstance()->SetDesc(ui->desc_ed->text());
    QMessageBox::information(this, "提示", "保存成功！重新登录后好友即可看到更新");
}
