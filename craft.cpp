#include "craft.h"
#include "ui_craft.h"
#include "noeditdelegate.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>

craft::craft(QWidget *parent) : QDialog(parent), ui(new Ui::craft) {
  ui->setupUi(this);
  model = new QSqlRelationalTableModel(this);
  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  ui->tableView->setModel(model);
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  ui->tableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  ui->tableView->horizontalHeader()->setSortIndicatorShown(true);
  ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
  ui->tableView->setItemDelegateForColumn(0, new NoEditDelegate(this)); //禁止修改
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &craft::view_sort);
  ui->comboBox->addItems(QStringList() << QStringLiteral("工种编号")
                                       << QStringLiteral("工种名称")
                                       << QStringLiteral("工种等级")
                                       << QStringLiteral("基本工资")
                                       << QStringLiteral("所属部门"));
}

void craft::setDatabaseConnect(QSqlDatabase db)
{
  this->db = db;
  model->setTable("craft");
  model->setFilter(QString("Cid > 10000"));
  model->setRelation(4, QSqlRelation("Dept", "Did", "Dname"));
  model->select();
  replaceHeader();
}

craft::~craft() { delete ui; }

void craft::updateTable() { on_pushButton_2_clicked(); }

void craft::on_pushButton_3_clicked() //添加记录
{
  emit call_addcraftinfo_window();
}

void craft::on_pushButton_2_clicked() //全表查询
{
  //  QSqlQuery query(this->db);
  //  query.exec("select * from craft where Cid > 10000");
  //  model->setQuery(query);
  model->setFilter(QString("Cid > 10000"));
  model->select();
  replaceHeader();
}

void craft::replaceHeader() {
  model->setHeaderData(0, Qt::Horizontal, QStringLiteral("工种编号 Cid"));
  model->setHeaderData(1, Qt::Horizontal, QStringLiteral("工种名称 Cname"));
  model->setHeaderData(2, Qt::Horizontal, QStringLiteral("工种等级 Clevel"));
  model->setHeaderData(3, Qt::Horizontal, QStringLiteral("基本工资 Cbase"));
  model->setHeaderData(4, Qt::Horizontal, QStringLiteral("所属部门 Cdept"));
}

void craft::view_sort(int column) {
  if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    model->sort(column, Qt::DescendingOrder);
  else
    model->sort(column, Qt::AscendingOrder);
}

void craft::on_comboBox_currentIndexChanged(int index) {
  if (index < 0)
    return;
  currentIndex = index;
  ui->lineEdit->clear();
  if ((index == 3) || (index == 0))
    ui->lineEdit->setValidator(new QIntValidator(10001, 99999, this));
  else
    ui->lineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(".*"), this));
}

void craft::on_pushButton_clicked() //条件查询
{
  QString t = ui->lineEdit->text();
  QSqlQuery query(db);
  switch (this->currentIndex) {
  case 0:
    model->setFilter(QString("Cid = '%1'").arg(t.toInt()));
    break;
  case 1:
    model->setFilter(
        QString("Cname like '%1' and Cid > 10000").arg("%" + t + "%"));
    break;
  case 2:
    model->setFilter(
        QString("Clevel like '%1' and Cid > 10000").arg("%" + t + "%"));
    break;
  case 3:
    model->setFilter(QString("Cbase = '%1' and Cid > 10000").arg(t.toInt()));
    break;
  case 4:
    query.exec(QString("select Did from Dept where Dname like '%1'").arg("%" + t + "%"));
    query.last();
    model->setFilter(QString("Cdept = '%1' and Cid > 10000").arg(query.record().value(0).toInt()));
    break;
  default:
    break;
  }
  model->select();
  replaceHeader();
}

void craft::on_pushButton_6_clicked() //高级查询
{
  bool OK;
  QString s = QInputDialog::getText(this, QStringLiteral("高级查询(SQL)"),
                                          QStringLiteral("select * from craft where "),
                                          QLineEdit::Normal, "Cid > 10000", &OK);
  if (OK) {
    model->setFilter(s + " and Cid > 10000");
    model->select();
    replaceHeader();
  }
}

void craft::on_pushButton_4_clicked() //提交修改
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

void craft::on_pushButton_5_clicked() //撤销修改
{
  int ret = QMessageBox::question(this, QStringLiteral("确认?"),
                                  QStringLiteral("确定撤销修改吗?"),
                                  QMessageBox::Yes, QMessageBox::No);
  if (ret != QMessageBox::Yes)
    return;
  model->revertAll();
}

void craft::on_pushButton_7_clicked() {
  model->setSort(3, Qt::AscendingOrder);
  model->select();
  replaceHeader();
}

void craft::on_pushButton_8_clicked() {
  model->setSort(3, Qt::DescendingOrder);
  model->select();
  replaceHeader();
}

void craft::on_pushButton_9_clicked() //删除记录
{
  model->removeRow(ui->tableView->currentIndex().row());
}
