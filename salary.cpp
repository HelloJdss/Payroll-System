#include "salary.h"
#include "noeditdelegate.h"
#include "ui_salary.h"
#include <QAxObject>
#include <QDate>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>
#include <QTableWidget>
#include <QTextEdit>
#include <QTextStream>

Salary::Salary(QWidget *parent) : QDialog(parent), ui(new Ui::Salary) {
  ui->setupUi(this);
  model = new QSqlTableModel(this);
  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  ui->tableView->setModel(model);
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  ui->tableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  ui->tableView->horizontalHeader()->setSortIndicatorShown(true);
  ui->tableView->setItemDelegate(new NoEditDelegate(this)); //禁止修改
  model->setTable("salary");
  model->setFilter("Sid > 10000");
  model->select();
  model->setHeaderData(0, Qt::Horizontal, QStringLiteral("员工编号 Sid"));
  model->setHeaderData(1, Qt::Horizontal, QStringLiteral("年度 Syear"));
  model->setHeaderData(2, Qt::Horizontal, QStringLiteral("月度 Smonth"));
  model->setHeaderData(3, Qt::Horizontal, QStringLiteral("基本 Cbase"));
  model->setHeaderData(4, Qt::Horizontal, QStringLiteral("津贴 BSalary"));
  model->setHeaderData(5, Qt::Horizontal, QStringLiteral("缺勤 SubSalary"));
  model->setHeaderData(6, Qt::Horizontal, QStringLiteral("其它 OtherSalary"));
  model->setHeaderData(7, Qt::Horizontal, QStringLiteral("实发 ActualSalary"));
  model->setHeaderData(8, Qt::Horizontal, QStringLiteral("其它说明 OtherDecb"));
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &Salary::view_sort);
  ui->comboBox->addItems(QStringList() << QStringLiteral("员工编号")
                                       << QStringLiteral("日期范围")
                                       << QStringLiteral("所属部门"));
  ui->dateEdit->setDate(QDate::currentDate().addMonths(-1));
  ui->dateEdit_2->setDate(QDate::currentDate());
  isStaffMode = false;
}

void Salary::staffQuery(int Sid) //员工自助查询
{
  isStaffMode = true;
  this->Sid = Sid;
  ui->pushButton_3->setDisabled(true);
  ui->pushButton_9->setDisabled(true);
  ui->pushButton_5->setDisabled(true);
  ui->pushButton_4->setDisabled(true);
  ui->comboBox->setCurrentIndex(1);
  ui->comboBox->setDisabled(true);
  ui->spinBox->setValue(Sid); //固定工号
  this->setWindowTitle(QStringLiteral("个人月工资查询"));
  model->setFilter(QString("Sid = '%1'").arg(Sid));
  model->select();
}

void Salary::TableViewToExcel(QTableView *table)
{
  bool OK;
  QString title = QInputDialog::getText(table, QStringLiteral("表格名"), QStringLiteral("请输入表格标题"),QLineEdit::Normal,QString(),&OK);
  if(!OK)
    return;
  QString fileName = QFileDialog::getSaveFileName(table, QStringLiteral("保存"),
                                                  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                  QStringLiteral("Excel 文件(*.xlsx)"));
  if (fileName != "") {
    QAxObject *excel = new QAxObject;
    if (excel->setControl("Excel.Application")) //连接Excel控件
    {
      excel->dynamicCall("SetVisible (bool Visible)", "false"); //不显示窗体
      excel->setProperty("DisplayAlerts", false); //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
      QAxObject *workbooks = excel->querySubObject("WorkBooks"); //获取工作簿集合
      workbooks->dynamicCall("Add");          //新建一个工作簿
      QAxObject *workbook = excel->querySubObject("ActiveWorkBook"); //获取当前工作簿
      QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);
      int i, j;
      //               //QTablewidget 获取数据的列数
      //               int colcount=table->columnCount();
      //                //QTablewidget 获取数据的行数
      //               int rowscount=table->rowCount();

      // QTableView 获取列数
      int colcount = table->model()->columnCount();
      // QTableView 获取行数
      int rowcount = table->model()->rowCount();
      QAxObject *cell, *col;
      //标题行
      cell = worksheet->querySubObject("Cells(int,int)", 1, 1);
      cell->dynamicCall("SetValue(const QString&)", title);
      cell->querySubObject("Font")->setProperty("Size", 18);
      //调整行高
      worksheet->querySubObject("Range(const QString&)", "1:1")->setProperty("RowHeight", 30);
      //合并标题行
      QString cellTitle;
      cellTitle.append("A1:");
      cellTitle.append(QChar(colcount - 1 + 'A'));
      cellTitle.append(QString::number(1));
      QAxObject *range = worksheet->querySubObject("Range(const QString&)", cellTitle);
      range->setProperty("WrapText", true);
      range->setProperty("MergeCells", true);
      range->setProperty("HorizontalAlignment", -4108); // xlCenter
      range->setProperty("VerticalAlignment", -4108);   // xlCenter
      //列标题
      for (i = 0; i < colcount; i++) {
        QString columnName;
        columnName.append(QChar(i + 'A'));
        columnName.append(":");
        columnName.append(QChar(i + 'A'));
        col = worksheet->querySubObject("Columns(const QString&)", columnName);
        col->setProperty("ColumnWidth", table->columnWidth(i) / 6);
        cell = worksheet->querySubObject("Cells(int,int)", 2, i + 1);
        // QTableWidget 获取表格头部文字信息
        // columnName=table->horizontalHeaderItem(i)->text();
        // QTableView 获取表格头部文字信息
        columnName = table->model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        cell->dynamicCall("SetValue(const QString&)", columnName);
        cell->querySubObject("Font")->setProperty("Bold", true);
        cell->querySubObject("Interior")
            ->setProperty("Color", QColor(191, 191, 191));
        cell->setProperty("HorizontalAlignment", -4108); // xlCenter
        cell->setProperty("VerticalAlignment", -4108);   // xlCenter
      }
      //数据区
      // QTableWidget 获取表格数据部分
      //            for(i=0;i<rowcount;i++){
      //              for (j=0;j<colcount;j++)
      //                {
      //                  worksheet->querySubObject("Cells(int,int)", i+3,
      //                  j+1)->dynamicCall("SetValue(const QString&)",
      //                  table->item(i,j)?table->item(i,j)->text():"");
      //                }
      //            }

      // QTableView 获取表格数据部分
      for (i = 0; i < rowcount; i++) //行数
      {
        for (j = 0; j < colcount; j++) //列数
        {
          QModelIndex index = table->model()->index(i, j);
          QString strdata = table->model()->data(index).toString();
          worksheet->querySubObject("Cells(int,int)", i + 3, j + 1)->dynamicCall("SetValue(const QString&)", strdata);
        }
      }

      //画框线
      QString lrange;
      lrange.append("A2:");
      lrange.append(colcount - 1 + 'A');
      lrange.append(QString::number(table->model()->rowCount() + 2));
      range = worksheet->querySubObject("Range(const QString&)", lrange);
      range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
      range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));
      //调整数据区行高
      QString rowsName;
      rowsName.append("2:");
      rowsName.append(QString::number(table->model()->rowCount() + 2));
      range = worksheet->querySubObject("Range(const QString&)", rowsName);
      range->setProperty("RowHeight", 20);
      workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileName)); //保存至fileName
      workbook->dynamicCall("Close()");        //关闭工作簿
      excel->dynamicCall("Quit()");            //关闭excel
      delete excel;
      excel = NULL;
      if (QMessageBox::question(table, QStringLiteral("完成"),
                                QStringLiteral("文件已经导出，是否现在打开？"),
                                QMessageBox::Yes | QMessageBox::No) ==
          QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
      }
    } else {
      QMessageBox::warning(table, QStringLiteral("错误"), QStringLiteral("未能创建 Excel 对象，请安装 Microsoft Excel。"), QMessageBox::Apply);
    }
  }
}

Salary::~Salary() { delete ui; }

void Salary::view_sort(int column) {
  if (ui->tableView->horizontalHeader()->sortIndicatorOrder())
    model->sort(column, Qt::DescendingOrder);
  else
    model->sort(column, Qt::AscendingOrder);
}

void Salary::on_pushButton_3_clicked() //添加
{
  AddSalary s;
  if (s.exec() == QDialog::Accepted)
    s.saveInfo();
  model->select();
}

void Salary::on_pushButton_9_clicked() //删除
{
  model->removeRow(ui->tableView->currentIndex().row());
}

void Salary::on_pushButton_2_clicked() //全表
{
  if (isStaffMode)
    model->setFilter(QString("Sid = '%1'").arg(Sid));
  else
    model->setFilter("Sid > 10000");
  model->select();
}

void Salary::on_pushButton_6_clicked() //高级
{
  bool OK;
  QString s = QInputDialog::getText(this, QStringLiteral("高级查询(SQL)"),
                                    QStringLiteral("select * from salary where "),
                                    QLineEdit::Normal, "Sid > 10000", &OK);
  if (OK) {
    if (isStaffMode)
      model->setFilter(
          s + QString(" and Sid = '%1'").arg(Sid)); //员工限定只能查询自己的
    else
      model->setFilter(s + " and Sid > 10000");
    model->select();
  }
}

void Salary::on_pushButton_5_clicked() //撤销
{
  int ret = QMessageBox::question(this, QStringLiteral("确认?"),
                                  QStringLiteral("确定撤销修改吗?"),
                                  QMessageBox::Yes, QMessageBox::No);
  if (ret != QMessageBox::Yes)
    return;
  model->revertAll();
}

void Salary::on_pushButton_4_clicked() //提交
{
  int ret =
      QMessageBox::question(this, QStringLiteral("确认?"),
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

void Salary::on_comboBox_currentIndexChanged(int index) {
  if (index < 0)
    return;
  QSqlQuery q;
  q.exec("select * from dept");
  ui->comboBox_2->clear();
  while (q.next()) {
    ui->comboBox_2->addItem(q.record().value(1).toString());
  }
  switch (index) {
  case 0:
    ui->spinBox->show();
    ui->comboBox_2->hide();
    break;
  case 1:
    ui->spinBox->hide();
    ui->comboBox_2->hide();
    break;
  case 2:
    ui->spinBox->hide();
    ui->comboBox_2->show();
    break;
  default:
    break;
  }
}

void Salary::on_pushButton_clicked() //查询
{
  switch (ui->comboBox->currentIndex()) {
  case 0: //员工编号
    model->setFilter(QString("Sid = '%1' and (Syear between '%2' and '%3') and (Smonth "
                             "between '%4' and '%5')")
                     .arg(ui->spinBox->value())
                     .arg(ui->dateEdit->date().year())
                     .arg(ui->dateEdit_2->date().year())
                     .arg(ui->dateEdit->date().month())
                     .arg(ui->dateEdit_2->date().month()));
    model->select();
    break;
  case 1: //日期检索
    if (isStaffMode)
      model->setFilter(QString("(Syear between '%1' and '%2') and (Smonth "
                               "between '%3' and '%4') and Sid = '%5'")
                           .arg(ui->dateEdit->date().year())
                           .arg(ui->dateEdit_2->date().year())
                           .arg(ui->dateEdit->date().month())
                           .arg(ui->dateEdit_2->date().month())
                           .arg(Sid));
    else
      model->setFilter(QString("(Syear between '%1' and '%2') and (Smonth "
                               "between '%3' and '%4')")
                           .arg(ui->dateEdit->date().year())
                           .arg(ui->dateEdit_2->date().year())
                           .arg(ui->dateEdit->date().month())
                           .arg(ui->dateEdit_2->date().month()));
    model->select();
    break;
  case 2: //部门检索
    model->setFilter(
        QString("(Sid in (select Sid from baseinfo where Department = '%1')) and (Syear between '%2' and '%3') and (Smonth "
                "between '%4' and '%5')")
            .arg(ui->comboBox_2->currentText())
            .arg(ui->dateEdit->date().year())
            .arg(ui->dateEdit_2->date().year())
            .arg(ui->dateEdit->date().month())
            .arg(ui->dateEdit_2->date().month()));
    model->select();
    break;
  default:
    break;
  }
}

void Salary::on_pushButton_7_clicked() //工资报表
{
  QMessageBox *a = new QMessageBox(this);
  a->setStandardButtons(0);
  a->setWindowTitle(QStringLiteral("报表中..."));
  a->setText(QStringLiteral("正在报表..."));
  a->show();
  int RowNum = model->rowCount();
  int Cbase = 0,BSalary = 0,SubSalary = 0,OtherSalary = 0,ActualSalry = 0;
  model->insertRow(RowNum);
  model->setData(model->index(RowNum,0),QStringLiteral("统计：")); //临时添加报表统计信息
  for(int i = 0; i < RowNum; i++){
      Cbase += model->data(model->index(i, 3)).toInt();
      BSalary += model->data(model->index(i, 4)).toInt();
      SubSalary += model->data(model->index(i, 5)).toInt();
      OtherSalary += model->data(model->index(i, 6)).toInt();
      ActualSalry += model->data(model->index(i,7)).toInt();
    }
  model->setData(model->index(RowNum, 3), Cbase);
  model->setData(model->index(RowNum, 4), BSalary);
  model->setData(model->index(RowNum, 5), SubSalary);
  model->setData(model->index(RowNum, 6), OtherSalary);
  model->setData(model->index(RowNum, 7), ActualSalry);
  TableViewToExcel(ui->tableView);
  model->revertAll();
  delete a;
}
