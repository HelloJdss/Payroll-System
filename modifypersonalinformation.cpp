#include "modifypersonalinformation.h"
#include "ui_modifypersonalinformation.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QValidator>
#include <QMessageBox>
#include <QDate>
#include <QDebug>
#include <QSqlError>
ModifyPersonalInformation::ModifyPersonalInformation(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ModifyPersonalInformation)
{
  ui->setupUi(this);
  ui->lineEdit_2->setValidator(new QRegularExpressionValidator(QRegularExpression("\\+?\\d*-?\\d*"), this)); //设置电话格式
  ui->lineEdit_3->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+@[a-zA-Z0-9]+(\\.[a-zA-Z0-9]+)+"),this)); //设置邮箱格式
}

void ModifyPersonalInformation::setDatabaseConnect(QSqlDatabase db)
{
  this->db = db;
}

bool ModifyPersonalInformation::setSid(int Sid)
{
  QSqlQuery q(db);
  q.exec(QString("select Sname,Ssex,Sbirthday,Tel,Email from baseinfo where Sid = '%1'").arg(Sid));
  if(!q.last()){
      QMessageBox::warning(0,QStringLiteral("警告!"),QStringLiteral("不存在员工信息!"),QMessageBox::Ok);
      return false;
    }
  this->Sid=Sid;
  ui->lineEdit->setText(q.record().value(0).toString());
  q.record().value(1).toString()==QString::fromLocal8Bit("男")
        ? ui->radioButton->setChecked(true)
        : ui->radioButton_2->setChecked(true);
  ui->dateEdit_2->setDate(QDate::fromString(q.record().value(2).toString(),"yyyy-MM-dd"));
  ui->lineEdit_2->setText(q.record().value(3).toString());
  ui->lineEdit_3->setText(q.record().value(4).toString());
  return true;
}

void ModifyPersonalInformation::saveInfo()
{
  QSqlQuery q(db);
  bool OK;
  OK = q.exec(QString("update baseinfo set Sname = '%1',"
                       "                    Ssex = '%2',"
                       "                    Sbirthday = '%3',"
                       "                    Tel = '%4',"
                       "                    Email = '%5' where Sid = '%6'")
                                           .arg(ui->lineEdit->text())
                                           .arg(ui->radioButton->isChecked()
                                                ? QString::fromLocal8Bit("男")
                                                : QString::fromLocal8Bit("女"))
                                           .arg(ui->dateEdit_2->text())
                                           .arg(ui->lineEdit_2->text())
                                           .arg(ui->lineEdit_3->text())
                                           .arg(Sid)
                                           );
  if(OK) QMessageBox::information(0,QStringLiteral("提示!"),QStringLiteral("数据已更新!"));
  else   QMessageBox::critical(0, QStringLiteral("错误!"),
                                  QString("Error: %1").arg(q.lastError().text()),
                                  QMessageBox::Ok
                                  );
}

ModifyPersonalInformation::~ModifyPersonalInformation()
{
  delete ui;
}
