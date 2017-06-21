#include "bonus.h"
#include "ui_bonus.h"
#include <QDate>
#include <QMessageBox>
#include <QSqlError>
#include <QInputDialog>
#include <QDebug>

bonus::bonus(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::bonus)
{
  ui->setupUi(this);
  model = new QSqlTableModel(this);
  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  ui->tableView->setModel(model);
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  ui->tableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  ui->tableView->horizontalHeader()->setSortIndicatorShown(true);
  ui->tableView->setItemDelegateForColumn(0, new NoEditDelegate(this)); //禁止修改
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &bonus::view_sort);
  ui->dateEdit->setDate(QDate::currentDate().addMonths(-1));
  ui->dateEdit_2->setDate(QDate::currentDate());
  isStaffQuery = false;
  model->setTable("bonus");
  model->setFilter("Sid > 10000");
  model->select();
  model->setHeaderData(0, Qt::Horizontal, QStringLiteral("员工编号 Sid"));
  model->setHeaderData(1, Qt::Horizontal, QStringLiteral("加班日期 Bdate"));
  model->setHeaderData(2, Qt::Horizontal, QStringLiteral("加班类别 Btype"));
  model->setHeaderData(3, Qt::Horizontal, QStringLiteral("加班时长 Btime"));
  model->setHeaderData(4, Qt::Horizontal, QStringLiteral("奖励津贴 Bonus"));
}

void bonus::setSid(int Sid)
{
  isStaffQuery = true;
  this->Sid = Sid;
  this->setWindowTitle(QStringLiteral("个人津贴查询"));
  ui->pushButton_2->hide();
  ui->pushButton_3->hide();
  ui->pushButton_4->hide();
  ui->pushButton_5->hide();
  ui->pushButton_6->hide();
  ui->pushButton_9->hide();
  ui->tableView->setItemDelegate(new NoEditDelegate(this)); //禁止修改
  model->setFilter(QString("Sid = '%1'").arg(Sid));
  model->select();
}

bonus::~bonus()
{
  delete ui;
}

void bonus::view_sort(int column)
{
  if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    model->sort(column, Qt::DescendingOrder);
  else
    model->sort(column, Qt::AscendingOrder);
}

void bonus::on_pushButton_clicked() //日期查询
{
  QString s;
  if(isStaffQuery)
    s.append(QString("Sid = '%1'").arg(Sid));
  else
    s.append("Sid > 10000");
  s.append(QString(" and date_format(Bdate,'%Y-%m-%d') between '%1' and '%2'")
                  .arg(ui->dateEdit->date().toString("yyyy-MM-dd"))
                  .arg(ui->dateEdit_2->date().toString("yyyy-MM-dd")));
  qDebug()<<s;
  model->setFilter(s);
  model->select();
}

void bonus::on_pushButton_2_clicked() //全表查询
{
  model->setFilter("Sid > 10000");
  model->select();
}

void bonus::on_pushButton_3_clicked() //添加记录
{
  AddBonus a;
  if(QDialog::Accepted == a.exec())
    a.saveInfo();
}

void bonus::on_pushButton_9_clicked() //删除记录
{
  model->removeRow(ui->tableView->currentIndex().row());
}

void bonus::on_pushButton_6_clicked() //高级查询
{
  bool OK;
  QString Text = QInputDialog::getText(this, QStringLiteral("高级查询(SQL)"),
                                          QStringLiteral("select * from bonus where "),
                                          QLineEdit::Normal, QString("Sid > 10000 and date_format(BDate,'%Y-%m') = \'2017-06\'"), &OK);
  if (!OK)
    return;
  model->setFilter(Text);
  model->select();
}

void bonus::on_pushButton_5_clicked()
{
  int ret = QMessageBox::question(this, QStringLiteral("确认?"),
                                  QStringLiteral("确定撤销修改吗?"),
                                  QMessageBox::Yes, QMessageBox::No);
  if (ret != QMessageBox::Yes)
    return;
  model->revertAll();
}

void bonus::on_pushButton_4_clicked()
{
  int ret = QMessageBox::question(this, QStringLiteral("确认?"),
                                     QStringLiteral("确定将修改操作同步到数据库吗?"),
                                     QMessageBox::Yes, QMessageBox::No);
  if (ret != QMessageBox::Yes)
    return;
  if (model->submitAll()) {
    // qDebug() << model->database().commit();
    QMessageBox::information(this, QStringLiteral("提示!"),
                             QStringLiteral("数据表已更新！"));
  } else {
    //  model->database().rollback();
    QMessageBox::critical(this, QStringLiteral("错误!"),
                          QString("Error: %1").arg(model->lastError().text()),
                          QMessageBox::Ok);
    }
}
