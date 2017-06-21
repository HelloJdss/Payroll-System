#include "addstaffinfo.h"
#include "ui_addstaffinfo.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QValidator>
#include <QMessageBox>
#include <QSqlError>

AddStaffInfo::AddStaffInfo(QWidget *parent)
    : QDialog(parent), ui(new Ui::addstaffinfo) {
  ui->setupUi(this);
  isModifyState = false;
  model = new QSqlQueryModel(this);
  ui->lineEdit_2->setValidator(new QRegularExpressionValidator(QRegularExpression("\\+?\\d*-?\\d*"), this)); //设置电话格式
  ui->lineEdit_3->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+@[a-zA-Z0-9]+(\\.[a-zA-Z0-9]+)+"),this)); //设置邮箱格式
  setCraftDept();
}

void AddStaffInfo::setDatabaseConnect(QSqlDatabase db) { this->db = db; }

QString AddStaffInfo::getName() { return ui->lineEdit->text(); }

QString AddStaffInfo::getSex() {
  return ui->radioButton->isChecked() ? QString::fromLocal8Bit("男")
                                      : QString::fromLocal8Bit("女");
}

QString AddStaffInfo::getBirth() { return ui->dateEdit_2->text(); }

int AddStaffInfo::getCid() {
  QSqlQuery q(this->db);
  bool ok;
  ok = q.exec(QString("select Cid from craft where Cname = '%1' and Clevel = '%2'")
               .arg(ui->comboBox->currentText())
               .arg(ui->comboBox_2->currentText()));
  q.last();
  qDebug()<<q.record().value(0).toInt();
  return q.record().value(0).toInt();
}

QString AddStaffInfo::getDept() { return ui->comboBox_3->currentText(); }

QString AddStaffInfo::getHiredate() { return ui->dateEdit->text(); }

QString AddStaffInfo::getTel() { return ui->lineEdit_2->text(); }

QString AddStaffInfo::getEmail() { return ui->lineEdit_3->text(); }

bool AddStaffInfo::setSid(int Sid)
{

  QSqlQuery q(db);
  q.exec(QString("select * from baseinfo where Sid = '%1'").arg(Sid));
  if(!q.last()){
      QMessageBox::warning(this,QStringLiteral("警告!"),QStringLiteral("不存在员工信息!"),QMessageBox::Ok);
      return false;
    }
  this->Sid = Sid;
  ui->lineEdit->setText(q.record().value(1).toString());
  q.record().value(2).toString()==QString::fromLocal8Bit("男")
        ? ui->radioButton->setChecked(true)
        : ui->radioButton_2->setChecked(true);
  ui->dateEdit_2->setDate(QDate::fromString(q.record().value(3).toString(),"yyyy-MM-dd"));
  for(int i = 0; i < DidList.size(); i++){
      ui->comboBox_3->setCurrentIndex(i);
      if(ui->comboBox_3->currentText() == q.record().value(5).toString()) //设定为当前部门
        break;
    }
  QSqlQuery q1(db);
  q1.exec(QString("select Cname,Clevel from craft where Cid = '%1'").arg(q.record().value(4).toString()));
  q1.last();
  for(int i = 0; i < ui->comboBox->model()->rowCount(); i++){
      ui->comboBox->setCurrentIndex(i);
      if(ui->comboBox->currentText() == q1.record().value(0).toString()) break;
    }
  for(int i = 0; i < ui->comboBox_2->model()->rowCount(); i++){
      ui->comboBox_2->setCurrentIndex(i);
      if(ui->comboBox_2->currentText() == q1.record().value(1).toString()) break;
    }
  ui->dateEdit->setDate(QDate::fromString(q.record().value(6).toString(),"yyyy-MM-dd"));
  ui->lineEdit_2->setText(q.record().value(7).toString());
  ui->lineEdit_3->setText(q.record().value(8).toString());
  isModifyState = true;
  ui->pushButton->setText(QStringLiteral("更新"));
  this->setWindowTitle(QStringLiteral("更新员工信息"));
  return true;
}

void AddStaffInfo::updateInfo()
{
  QSqlQuery query(db);
  bool ret = false;
  ret = query.exec(QString("update baseinfo set Sname = '%1',"
                           "                    Ssex = '%2',"
                           "                    Sbirthday = '%3',"
                           "                    Cid = '%4',"
                           "                    Department = '%5',"
                           "                    Hiredate = '%6',"
                           "                    Tel = '%7',"
                           "                    Email = '%8' where Sid = '%9'")
                   .arg(getName()).arg(getSex()).arg(getBirth()).arg(getCid())
                   .arg(getDept()).arg(getHiredate()).arg(getTel()).arg(getEmail())
                   .arg(this->Sid));
  if(ret) QMessageBox::information(this,QStringLiteral("成功!"),QStringLiteral("更新成功"));
  else QMessageBox::critical(this,QStringLiteral("错误!"),QString(query.lastError().text()),QMessageBox::Ok);
}

AddStaffInfo::~AddStaffInfo() {
  delete ui;
  delete model;
}

void AddStaffInfo::setCraftDept() {
  QSqlQuery q(db);
  q.exec("select Did,Dname from dept");
  isInitialized = false;
  while (q.next()) {
    ui->comboBox_3->addItem(q.record().value(1).toString());
    DidList << q.record().value(0).toInt();
  }
  setCraftinfo(ui->comboBox_3->currentIndex());
  isInitialized = true;
}

void AddStaffInfo::setCraftinfo(int DeptIndex) {
  isInitialized = false;
  model->setQuery(QString("select Cname from craft where Cid > 10000 and Cdept "
                          "= '%1' group by Cname")
                      .arg(DidList.at(DeptIndex)));
  ui->comboBox->clear();
  int i;
  for (i = 0; i < model->rowCount(); i++) {
    ui->comboBox->addItem(model->record(i).value(0).toString());
  }
  isInitialized = true;
  ui->comboBox->setCurrentIndex(-1);
  if (i)
    ui->comboBox->setCurrentIndex(0); //触发更新comboBox_2函数
}

//更新comboBox_2
void AddStaffInfo::on_comboBox_currentIndexChanged(const QString &arg1) {

  if (!isInitialized) //初始化comboBox过程中不予执行
    return;
  model->setQuery(
      QString("select Clevel from craft where Cname = '%1'").arg(arg1));
  ui->comboBox_2->clear();
  for (int i = 0; i < model->rowCount(); i++) {
    ui->comboBox_2->addItem(model->record(i).value(0).toString());
  }
}

void AddStaffInfo::on_comboBox_3_currentIndexChanged(int index) {
  if (isInitialized)
    setCraftinfo(index);
}
