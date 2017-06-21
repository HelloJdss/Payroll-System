#include "addsalary.h"
#include "ui_addsalary.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDate>
#include <QDebug>

AddSalary::AddSalary(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AddSalary)
{
  ui->setupUi(this);
  ui->spinBox->setValue(QDate::currentDate().addMonths(-1).year()); //自动补全上一个月
  ui->spinBox_2->setValue(QDate::currentDate().addMonths(-1).month());
  updateInfo(ui->spinBox_9->value());
}

void AddSalary::saveInfo()
{
  QSqlQuery q;
  bool OK;
  q.exec(QString("select count(*) from salary where (Sid = '%1') and (Syear = '%2') and (Smonth = '%3')")
         .arg(ui->spinBox_9->value())
         .arg(ui->spinBox->value())
         .arg(ui->spinBox_2->value()));
  q.last();
  if(q.record().value(0).toInt()){
      QMessageBox::critical(this,QStringLiteral("错误"),QStringLiteral("本月已有月工资信息!"));
      return;
    }
  OK = q.exec(QString("insert into salary(Sid, Syear, Smonth, "
                      "                   Cbase, BSalary, SubSalary, "
                      "                   OtherSalary, ActualSalary, OtherDecb)"
                      "                   values('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9')")
              .arg(ui->spinBox_9->value())
              .arg(ui->spinBox->value())
              .arg(ui->spinBox_2->value())
              .arg(ui->spinBox_3->value())
              .arg(ui->spinBox_4->value())
              .arg(ui->spinBox_6->value())
              .arg(ui->spinBox_8->value())
              .arg(ui->spinBox_7->value())
              .arg(ui->lineEdit_2->text()));
  if(OK) QMessageBox::information(this,QStringLiteral("成功!"),QStringLiteral("录入成功"));
  else QMessageBox::critical(this,QStringLiteral("错误!"),QString(q.lastError().text()),QMessageBox::Ok);

}

AddSalary::~AddSalary()
{
  delete ui;
}

int AddSalary::caculateAttendancePunishment(int Sid) //计算缺勤惩罚
{
  QSqlQuery q;
  bool ret;
  ret = q.exec(QString("select count(*) from attendance where Sid = '%1' and date_format(ADate, '%Y-%c') = '%2'")
               .arg(Sid)
               .arg(QString(ui->spinBox->text() + "-" + ui->spinBox_2->text())));
  if(!ret)
    return 0;
  int DayCount = 0;
  int DayRequire = ui->spinBox_5->value();
  float rate;
  if(DayRequire == 0)
    return 0;
  q.last();
  DayCount = q.record().value(0).toInt();
  qDebug() << "DayCount: " << DayCount << "DayRequire: " << DayRequire;
  if(DayCount > DayRequire)
    return 0;
  rate = (float(DayRequire) - float(DayCount)) / float(DayRequire); //采用缺勤比例计算
  qDebug() << rate;
  return rate * ui->spinBox_3->value();
}

void AddSalary::updateInfo(int Sid)
{
  QSqlQuery q;
  bool ret;
  ret = q.exec(QString("select Cbase, sum(Bonus) from craft, bonus "
                       "where craft.Cid in (select Cid from baseinfo where Sid = '%1') "
                       "  and bonus.Sid = '%1'"
                       "  and date_format(bonus.Bdate, '%Y-%c') = '%2'")
                       .arg(Sid)
                       .arg(ui->spinBox->text() + "-" + ui->spinBox_2->text()));
  if(!ret)
    return;
  if(!q.last())
    return;
  ui->spinBox_3->setValue(q.record().value(0).toInt());
  ui->spinBox_4->setValue(q.record().value(1).toInt());
  ui->spinBox_6->setValue(caculateAttendancePunishment(Sid));
  int sum = 0;
  sum = ui->spinBox_3->value() + ui->spinBox_4->value() - ui->spinBox_6->value() + ui->spinBox_8->value();
  ui->spinBox_7->setValue(sum);
}



void AddSalary::on_spinBox_9_valueChanged(int arg1) //Sid changed
{
  updateInfo(arg1);
  //ui->spinBox_6->setValue();
}

void AddSalary::on_spinBox_valueChanged(int) // year Changed
{
  updateInfo(ui->spinBox_9->value());
}

void AddSalary::on_spinBox_2_valueChanged(int) //month Changed
{
  updateInfo(ui->spinBox_9->value());
}

void AddSalary::on_spinBox_5_valueChanged(int) //attendance Changed
{
  updateInfo(ui->spinBox_9->value());
}

void AddSalary::on_spinBox_8_valueChanged(int) //othersalary Changed
{
  updateInfo(ui->spinBox_9->value());
}
