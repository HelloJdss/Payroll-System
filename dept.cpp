#include "dept.h"
#include "noeditdelegate.h"
#include "ui_dept.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>

Dept::Dept(QWidget *parent) : QDialog(parent), ui(new Ui::Dept) {
  ui->setupUi(this);
  model = new QSqlTableModel(this);
  ui->tableView->setModel(model);
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  ui->tableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  ui->tableView->horizontalHeader()->setSortIndicatorShown(true);
  ui->tableView->setItemDelegateForColumn(0, new NoEditDelegate(this)); //禁止修改
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &Dept::view_sort);
  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

void Dept::setDatabaseConnect(QSqlDatabase db) { this->db = db; }

void Dept::execQuery() {
  model->setTable("dept");
  model->select();
  model->sort(0, Qt::AscendingOrder);
  replaceHeader();
}

Dept::~Dept() { delete ui; }

void Dept::view_sort(int column) {
  if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    model->sort(column, Qt::DescendingOrder);
  else
    model->sort(column, Qt::AscendingOrder);
}

void Dept::replaceHeader() {
  model->setHeaderData(0, Qt::Horizontal, QStringLiteral("部门编号 Did"));
  model->setHeaderData(1, Qt::Horizontal, QStringLiteral("部门名称 Dname"));
}

void Dept::on_lineEdit_textChanged(const QString &arg1) {
  model->setFilter(QString("Dname like '%1'").arg("%" + arg1 + "%"));
  model->select();
}

void Dept::on_pushButton_3_clicked() {
  model->removeRow(ui->tableView->currentIndex().row());
}

void Dept::on_pushButton_2_clicked() {
  int ret = QMessageBox::question(this, QStringLiteral("确认?"),
                                     QStringLiteral("确定撤销修改吗?"),
                                     QMessageBox::Yes, QMessageBox::No);
  if (ret != QMessageBox::Yes)
    return;
  model->revertAll();
}

void Dept::on_pushButton_clicked() {
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
