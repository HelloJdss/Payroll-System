#include "baseinfo.h"
#include "ui_baseinfo.h"
#include "noeditdelegate.h"
#include "comboboxdelegate.h"
#include <QMessageBox>
#include <QSqlError>
#include <QInputDialog>

Baseinfo::Baseinfo(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Baseinfo)
{
  ui->setupUi(this);
  model = new QSqlRelationalTableModel(this);
  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  ui->tableView->setModel(model);
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  ui->tableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  ui->tableView->horizontalHeader()->setSortIndicatorShown(true);
  ui->tableView->setItemDelegateForColumn(0, new NoEditDelegate(this)); //禁止修改编号
  ui->tableView->setItemDelegateForColumn(2, new ComboBoxDelegate(QStringList() << QString::fromLocal8Bit("男")
                                                                                << QString::fromLocal8Bit("女"), this)); //为性别设定代理
  ui->tableView->setItemDelegateForColumn(4, new NoEditDelegate(this)); //禁止修改工种
  ui->tableView->setItemDelegateForColumn(5, new NoEditDelegate(this)); //禁止修改部门
 // ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &Baseinfo::view_sort);
  ui->comboBox->addItems(QStringList() << QStringLiteral("员工编号")
                                       << QStringLiteral("员工姓名")
                                       << QStringLiteral("员工性别")
                                       << QStringLiteral("所属部门"));
}

void Baseinfo::setDatabaseConnect(QSqlDatabase db)
{
  this->db = db;
}

void Baseinfo::execQuery()
{
  model->setTable("baseinfo");
  model->setFilter("Sid > 10000");
  model->select();
  model->sort(0, Qt::AscendingOrder);
  replaceHeader();
}

Baseinfo::~Baseinfo()
{
  delete ui;
}

void Baseinfo::updateTable()
{
  model->select();
  replaceHeader();
}

void Baseinfo::view_sort(int column)
{
  if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    model->sort(column, Qt::DescendingOrder);
  else
    model->sort(column, Qt::AscendingOrder);
}

void Baseinfo::replaceHeader()
{
  model->setHeaderData(0, Qt::Horizontal, QStringLiteral("编号 Sid"));
  model->setHeaderData(1, Qt::Horizontal, QStringLiteral("姓名 Sname"));
  model->setHeaderData(2, Qt::Horizontal, QStringLiteral("性别 Ssex"));
  model->setHeaderData(3, Qt::Horizontal, QStringLiteral("生辰 Sbirthday"));
  model->setHeaderData(4, Qt::Horizontal, QStringLiteral("工种 Cid"));
  model->setHeaderData(5, Qt::Horizontal, QStringLiteral("部门 Department"));
  model->setHeaderData(6, Qt::Horizontal, QStringLiteral("入职 Hiredate"));
  model->setHeaderData(7, Qt::Horizontal, QStringLiteral("电话 Tel"));
  model->setHeaderData(8, Qt::Horizontal, QStringLiteral("邮箱 Email"));
}

void Baseinfo::on_pushButton_3_clicked() //添加记录
{
  emit call_addstaffinfo_window();
}

void Baseinfo::on_pushButton_9_clicked() //删除记录
{
  model->removeRow(ui->tableView->currentIndex().row());
}

void Baseinfo::on_pushButton_2_clicked() //全表查询
{
  execQuery();
}

void Baseinfo::on_pushButton_5_clicked() //撤销修改
{
  int ret = QMessageBox::question(this, QStringLiteral("确认?"),
                                  QStringLiteral("确定撤销修改吗?"),
                                  QMessageBox::Yes, QMessageBox::No);
  if (ret != QMessageBox::Yes)
    return;
  model->revertAll();
}

void Baseinfo::on_pushButton_4_clicked() //提交修改
{
  // model->database().transaction(); //启动事务
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

void Baseinfo::on_pushButton_6_clicked() //高级查询
{
  bool OK;
  QString s = QInputDialog::getText(this, QStringLiteral("高级查询(SQL)"),
                                          QStringLiteral("select * from baseinfo where "),
                                          QLineEdit::Normal, "Sid > 10000", &OK);
  if (OK) {
    model->setFilter(s + " and Sid > 10000");
    model->select();
    replaceHeader();
  }
}

void Baseinfo::on_pushButton_7_clicked() //部门修改
{
  AddStaffInfo a;
  QModelIndex index = model->index(ui->tableView->currentIndex().row(),0);
  a.setSid(index.data().toInt());
  if(QDialog::Accepted == a.exec()) a.updateInfo();
}

void Baseinfo::on_pushButton_clicked() //条件查询
{
  QString t = ui->lineEdit->text();
  switch (ui->comboBox->currentIndex()) {
  case 0:
    model->setFilter(QString("Sid = '%1' and Sid > 10000").arg(t.toInt()));
    break;
  case 1:
    model->setFilter(
        QString("Sname like '%1' and Sid > 10000").arg("%" + t + "%"));
    break;
  case 2:
    model->setFilter(
        QString("Ssex = '%1' and Sid > 10000").arg(t));
    break;
  case 3:
    model->setFilter(QString("Department like '%1' and Sid > 10000").arg("%" + t + "%"));
    break;
  default:
    break;
  }
  model->select();
}
