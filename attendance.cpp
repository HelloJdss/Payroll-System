#include "attendance.h"
#include "ui_attendance.h"
#include <QMessageBox>
#include <QDate>
#include <QInputDialog>
#include <QDebug>

Attendance::Attendance(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Attendance)
{
  ui->setupUi(this);
  model = new QSqlTableModel(this);
  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  ui->tableView->setModel(model);
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  ui->tableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  ui->tableView->horizontalHeader()->setSortIndicatorShown(true);
  ui->tableView->setItemDelegateForColumn(0, new NoEditDelegate(this)); //禁止修改编号
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &Attendance::view_sort);
  model->setTable("attendance");
  selectAll();
  ui->dateEdit->setDate(QDate::currentDate());
}

Attendance::~Attendance()
{
  delete ui;
}

void Attendance::view_sort(int column)
{
  if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    model->sort(column, Qt::DescendingOrder);
  else
    model->sort(column, Qt::AscendingOrder);
}

void Attendance::selectAll()
{
  model->setFilter(QString("Sid > 10000"));
  model->select();
  replaceHeader();
}

void Attendance::replaceHeader()
{
  model->setHeaderData(0, Qt::Horizontal, QStringLiteral("编号 Sid"));
  model->setHeaderData(1, Qt::Horizontal, QStringLiteral("签到日期 ADate"));
}

void Attendance::on_pushButton_2_clicked() //删除记录
{
  model->removeRow(ui->tableView->currentIndex().row());
}

void Attendance::on_pushButton_3_clicked() //撤销修改
{
  int ret = QMessageBox::question(this, QStringLiteral("确认?"),
                                  QStringLiteral("确定撤销修改吗?"),
                                  QMessageBox::Yes, QMessageBox::No);
  if (ret != QMessageBox::Yes)
    return;
  model->revertAll();
}

void Attendance::on_pushButton_4_clicked() //提交修改
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

void Attendance::on_pushButton_7_clicked() //全表查询
{
  selectAll();
}

void Attendance::on_pushButton_clicked() //添加记录
{
  QSqlQuery q;
  bool OK;
  QString Sid = ui->lineEdit->text();
  QString currentDate = ui->dateEdit->date().toString("yyyy-MM-dd");
  q.exec(QString("select * from attendance where Sid = '%1' and ADate = '%2'").arg(Sid).arg(currentDate));
  if(q.next()) {
      QMessageBox::critical(this,QStringLiteral("错误!"),QStringLiteral("请勿重复签到!"),QMessageBox::Ok);
      return;
    }
  OK = q.exec(QString("insert into attendance(Sid, ADate) values ('%1', '%2')").arg(Sid).arg(currentDate));
  if(OK)
    QMessageBox::information(this, QStringLiteral("提示!"), QStringLiteral("今日已签到!"));
  else
    QMessageBox::critical(this, QStringLiteral("错误!"), QString("Error: %1").arg(q.lastError().text()), QMessageBox::Ok);
}

void Attendance::on_pushButton_6_clicked() //员工筛选
{
  bool OK;
  QString Sid = QInputDialog::getText(this, QStringLiteral("员工筛选"),
                                           QStringLiteral("请输入员工编号（注：不用加\"U\"）："),
                                           QLineEdit::Normal, QString("10001"), &OK);
  if (!OK)
    return;
  model->setFilter(QString("Sid = '%1'").arg(Sid));
  model->select();
}

void Attendance::on_pushButton_5_clicked() //日期筛选
{
  bool OK;
  QString Date = QInputDialog::getText(this, QStringLiteral("日期筛选"),
                                          QStringLiteral("请输入查询日期（注：格式为YYYY-mm，如2017-06）："),
                                          QLineEdit::Normal, QString("2017-06"), &OK);
  if (!OK)
    return;
  model->setFilter(QString("date_format(ADate,'%Y-%m') = '%1'").arg(Date));
  model->select();
}

void Attendance::on_pushButton_8_clicked()
{
  bool OK;
  QString Text = QInputDialog::getText(this, QStringLiteral("高级查询(SQL)"),
                                          QStringLiteral("select * from attendance where "),
                                          QLineEdit::Normal, QString("Sid = 10001 and date_format(ADate,'%Y-%m') = \'2017-06\'"), &OK);
  if (!OK)
    return;
  model->setFilter(Text);
  model->select();
}
