#include "addstaffinfo.h"
#include "ui_addstaffinfo.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QValidator>
AddStaffInfo::AddStaffInfo(QWidget *parent)
    : QDialog(parent), ui(new Ui::addstaffinfo) {
  ui->setupUi(this);
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
  q.exec(QString("select Cid from craft where Cname = '%1' and Clevel = '%2'")
             .arg(ui->comboBox->currentText())
             .arg(ui->comboBox_2->currentText()));
  q.next();
  return q.record().value(0).toInt();
}

QString AddStaffInfo::getDept() { return ui->comboBox_3->currentText(); }

QString AddStaffInfo::getHiredate() { return ui->dateEdit->text(); }

QString AddStaffInfo::getTel() { return ui->lineEdit_2->text(); }

QString AddStaffInfo::getEmail() { return ui->lineEdit_3->text(); }

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
