#include "addbonus.h"
#include "ui_addbonus.h"
#include <QDate>
#include <QSqlQuery>
#include <QIntValidator>
#include <QMessageBox>
#include <QSqlError>

AddBonus::AddBonus(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AddBonus)
{
  ui->setupUi(this);
  ui->lineEdit->setValidator(new QIntValidator(10000, 99999, this));
  ui->dateEdit->setDate(QDate::currentDate());
}

AddBonus::~AddBonus()
{
  delete ui;
}

void AddBonus::saveInfo()
{
  QSqlQuery q;
  bool ret;
  ui->spinBox->interpretText();
  ui->spinBox_2->interpretText();
  ret = q.exec(QString("insert into bonus(Sid, Bdate, Btype, Btime, Bonus) values('%1', '%2', '%3', '%4', '%5')")
               .arg(ui->lineEdit->text())
               .arg(ui->dateEdit->date().toString("yyyy-MM-dd"))
               .arg(ui->lineEdit_2->text())
               .arg(ui->spinBox->value())
               .arg(ui->spinBox_2->value()));
  if(ret)
    QMessageBox::information(this, QStringLiteral("提示!"), QStringLiteral("录入成功!"));
  else
    QMessageBox::critical(this, QStringLiteral("错误!"), QString("Error: " + q.lastError().text()), QMessageBox::Ok);
}
