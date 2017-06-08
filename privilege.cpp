#include "privilege.h"
#include "ui_privilege.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlError>

Privilege::Privilege(QWidget *parent) : QDialog(parent), ui(new Ui::Privilege) {
  ui->setupUi(this);
  model = new QSqlTableModel(this);
  ui->tableView->setModel(model);
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  ui->tableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  ui->tableView->horizontalHeader()->setSortIndicatorShown(true);
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &Privilege::view_sort);
  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  ui->splitter->setStretchFactor(0,1);
}

void Privilege::setDatabaseConnect(QSqlDatabase db) { this->db = db; }

void Privilege::fillTable() {
  model->setTable("users");
  model->select();
  model->sort(0, Qt::AscendingOrder);
  for (int i = 0; i < model->rowCount(); i++) {
    ui->comboBox->addItem(model->record(i).value(1).toString());
  }
  replaceHeader();
}

Privilege::~Privilege() { delete ui; }

void Privilege::view_sort(int column) {
  if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    model->sort(column, Qt::DescendingOrder);
  else
    model->sort(column, Qt::AscendingOrder);
}

void Privilege::replaceHeader() {
  model->setHeaderData(0, Qt::Horizontal, QStringLiteral("用户编号 Uid"));
  model->setHeaderData(1, Qt::Horizontal, QStringLiteral("用户名 Username"));
  model->setHeaderData(2, Qt::Horizontal, QStringLiteral("权限等级 Privilege"));
}

void Privilege::on_comboBox_currentIndexChanged(const QString &arg1) {
  if (arg1.size()) {
    if (arg1 == "payroller")
      ui->label_5->setText(arg1);
    else {
      QSqlQuery q(db);
      q.exec(QString("select Sname from baseinfo where Sid = '%1'").arg(arg1.mid(1)));
      q.last();
      ui->label_5->setText(q.record().value(0).toString());
    }
  }
}

void Privilege::on_pushButton_3_clicked() {
  bool OK;
  QString pswd = QInputDialog::getText(0,
                                       QStringLiteral("修改密码"),
                                       QStringLiteral("请输入新密码："),
                                       QLineEdit::PasswordEchoOnEdit, QString(), &OK);
  if (!OK) return;
  if (pswd != QInputDialog::getText(0,
                                    QStringLiteral("修改密码"),
                                    QStringLiteral("请确认新密码："),
                                    QLineEdit::Password, QString(), &OK)) {
    QMessageBox::critical(0, QStringLiteral("错误!"), QStringLiteral("密码输入不一致！"), QMessageBox::Ok);
    return;
  }
  if (!OK) return;
  QSqlQuery q(db);
  if (q.exec(QString("set password for '%1'@'localhost' =  '%2'")
                 .arg(ui->comboBox->currentText())
                 .arg(pswd)))
    QMessageBox::information(0, QStringLiteral("提示!"), QStringLiteral("密码已成功修改!"));
  else
    QMessageBox::critical(0, QStringLiteral("错误!"),
                             QString("Error: %1").arg(q.lastError().text()),
                             QMessageBox::Ok);
}

void Privilege::on_pushButton_clicked()
{
  int ret =
      QMessageBox::question(0, QStringLiteral("确认?"),
                               QStringLiteral("确定将修改操作同步到数据库吗?"),
                               QMessageBox::Yes, QMessageBox::No);
  if (ret != QMessageBox::Yes)
    return;
  if (model->submitAll()) {
    // qDebug() << model->database().commit();
    QMessageBox::information(0, QStringLiteral("提示!"),
                                QStringLiteral("数据表已更新！"));
  } else {
    //  model->database().rollback();
    QMessageBox::critical(0, QStringLiteral("错误!"),
                             QString("Error: %1").arg(model->lastError().text()),
                             QMessageBox::Ok);
  }
}

void Privilege::on_pushButton_2_clicked()
{
  int ret = QMessageBox::question(0, QStringLiteral("确认?"),
                                     QStringLiteral("确定撤销修改吗?"),
                                     QMessageBox::Yes, QMessageBox::No);
  if (ret != QMessageBox::Yes)
    return;
  model->revertAll();
}
