#include "annualbonus.h"
#include "ui_annualbonus.h"
#include <QDate>
#include <QIntValidator>
#include <QMessageBox>

AnnualBonus::AnnualBonus(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AnnualBonus)
{
  ui->setupUi(this);
  model = new QSqlQueryModel(this);
  ui->tableView->setModel(model);
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  ui->tableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  ui->tableView->horizontalHeader()->setSortIndicatorShown(true);
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &AnnualBonus::view_sort);
  ui->dateEdit->setDate(QDate::currentDate());
  ui->lineEdit->setValidator(new QIntValidator(10001,99999,this));
}

void AnnualBonus::setSid(int Sid) //个人工资查询
{
  ui->lineEdit->setText(QString("%1").arg(Sid));
  ui->lineEdit->setDisabled(true);
}

AnnualBonus::~AnnualBonus()
{
  delete ui;
}

void AnnualBonus::view_sort(int column)
{
  if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    model->sort(column, Qt::DescendingOrder);
  else
    model->sort(column, Qt::AscendingOrder);
}

void AnnualBonus::Query()
{
  if(ui->lineEdit->text().size() == 0)
    model->setQuery(QString("select Sid,sum(ActualSalary),sum(BSalary),"
                            "(sum(ActualSalary) +  sum(BSalary))/12 from salary where Syear = '%1' group by Sid")
                    .arg(ui->dateEdit->date().year()));
  else
    model->setQuery(QString("select Sid,sum(ActualSalary),sum(BSalary),"
                            "(sum(ActualSalary) +  sum(BSalary))/12 from salary where Syear = '%1' and Sid = '%2'")
                    .arg(ui->dateEdit->date().year())
                    .arg(ui->lineEdit->text()));
  model->setHeaderData(0, Qt::Horizontal, QStringLiteral("员工编号 Sid"));
  model->setHeaderData(1, Qt::Horizontal, QStringLiteral("年度实发"));
  model->setHeaderData(2, Qt::Horizontal, QStringLiteral("津贴合计"));
  model->setHeaderData(3, Qt::Horizontal, QStringLiteral("年终奖金"));
  int RowNum = model->rowCount();
  int BSalary = 0,ActualSalry = 0;
  float AnnualSalary = 0;
  for(int i = 0; i < RowNum; i++){
      BSalary += model->data(model->index(i, 2)).toInt();
      AnnualSalary += model->data(model->index(i, 3)).toFloat();
      ActualSalry += model->data(model->index(i,1)).toInt();
    }
  ui->label_5->setText(QString("%1").arg(ActualSalry));
  ui->label_7->setText(QString("%1").arg(BSalary));
  ui->label_9->setText(QString("%1").arg(AnnualSalary));
}

void AnnualBonus::on_dateEdit_dateChanged(const QDate &)
{
  Query();
}

void AnnualBonus::on_lineEdit_textChanged(const QString &)
{
  Query();
}

void AnnualBonus::on_pushButton_clicked()
{
  QMessageBox *a = new QMessageBox(this);
  a->setStandardButtons(0);
  a->setWindowTitle(QStringLiteral("报表中..."));
  a->setText(QStringLiteral("正在报表..."));
  a->show();
  Salary::TableViewToExcel(ui->tableView);
  delete a;
}
