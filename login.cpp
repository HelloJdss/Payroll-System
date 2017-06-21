#include "login.h"
#include "ui_login.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QDir>

Login::Login(QWidget *parent) : QDialog(parent), ui(new Ui::Login) {
  ui->setupUi(this);
  dbname = "payroll";
  dbport = 3306;
  dbhost = "localhost";
  isSettingMode = false;
  QFile file(QDir::toNativeSeparators("settings/connect_settings.ini")); //如果已有信息，则自动读取
  QFileInfo info(file);
  QRegularExpression rxName("DataBase Name:\\s*(.*)\\s*;"),
                     rxPort("DataBase Port:\\s*(.*)\\s*;"),
                     rxHost("DataBase Host:\\s*(.*)\\s*;");
  QRegularExpressionMatch match;
  if (info.exists()) { //判断文件是否存在
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QString s = file.readAll();
      match = rxName.match(s);
      if(match.hasMatch()){
          ui->lineEdit_4->setText(match.captured(1));
          dbhost = match.captured(1);
        }
      match = rxPort.match(s);
      if(match.hasMatch()){
          qint32 port;
          bool OK;
          port = match.captured(1).toInt(&OK);
          if(OK){
              ui->spinBox->setValue(port);
              dbport = ui->spinBox->value();
            }
        }
      match = rxHost.match(s);
      if(match.hasMatch()){
          ui->lineEdit_3->setText(match.captured(1));
          dbname = match.captured(1);
        }
      file.close();
    }
  }
  else
    qDebug() << file.fileName() << " Read Failed!";
}

void Login::setLoginMode()
{
  ui->label_3->setText(QStringLiteral("用户登陆"));
  ui->label_2->show();
  ui->label->show();
  ui->lineEdit->show();
  ui->lineEdit_2->show();
  ui->label_4->hide();
  ui->label_5->hide();
  ui->label_6->hide();
  ui->lineEdit_3->hide();
  ui->lineEdit_4->hide();
  ui->spinBox->hide();
  ui->pushButton->setText(QStringLiteral("登录"));
  this->setWindowTitle(QStringLiteral("用户登录"));
  isSettingMode = false;
}

void Login::setSettingMode()
{
  ui->label_3->setText(QStringLiteral("连接设置"));
  ui->label_2->hide();
  ui->label->hide();
  ui->lineEdit->hide();
  ui->lineEdit_2->hide();
  ui->label_4->show();
  ui->label_5->show();
  ui->label_6->show();
  ui->lineEdit_3->show();
  ui->lineEdit_4->show();
  ui->spinBox->show();
  ui->pushButton->setText(QStringLiteral("保存"));
  this->setWindowTitle(QStringLiteral("连接设置"));
  isSettingMode = true;
}

void Login::clearPswd() { ui->lineEdit_2->clear(); }

Login::~Login() { delete ui; }

void Login::on_pushButton_clicked() {
  dbuser = ui->lineEdit->text();
  dbpswd = ui->lineEdit_2->text();
  dbport = ui->spinBox->value();
  dbhost = ui->lineEdit_3->text();
  dbname = ui->lineEdit_4->text();
  if(isSettingMode){
      QDir d;
      d.mkpath("./settings/");
      QFile file(QDir::toNativeSeparators("settings/connect_settings.ini"));
      if(file.open(QFile::WriteOnly | QFile::Text)){
          QTextStream out(&file);
          out << "DataBase Name: " << dbname << ";\n";
          out << "DataBase Port: " << QString("%1").arg(dbport) << ";\n";
          out << "DataBase Host: " << dbhost << ";\n";
          file.close();
        }
    }
  accept();
}
