#ifndef SALARY_H
#define SALARY_H

#include "addsalary.h"
#include <QDialog>
#include <QDialog>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QTableView>


namespace Ui {
  class Salary;
}

class Salary : public QDialog
{
  Q_OBJECT

public:
  explicit Salary(QWidget *parent = 0);
  void staffQuery(int Sid);
  static void TableViewToExcel(QTableView* table);
  ~Salary();
public slots:
  void view_sort(int column);

private slots:
  void on_pushButton_3_clicked();

  void on_pushButton_9_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_4_clicked();

  void on_comboBox_currentIndexChanged(int index);

  void on_pushButton_clicked();

  void on_pushButton_7_clicked();

private:
  bool isStaffMode;
  int Sid;
  QSqlTableModel *model;
  Ui::Salary *ui;
};

#endif // SALARY_H
