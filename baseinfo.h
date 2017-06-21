#ifndef BASEINFO_H
#define BASEINFO_H

#include "addstaffinfo.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QSqlQuery>

namespace Ui {
  class Baseinfo;
}

class Baseinfo : public QDialog
{
  Q_OBJECT

public:
  explicit Baseinfo(QWidget *parent = 0);
  void setDatabaseConnect(QSqlDatabase db);
  void execQuery();
  ~Baseinfo();
public slots:
  void updateTable();
  void view_sort(int column);

private slots:
  void on_pushButton_3_clicked();

  void on_pushButton_9_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_7_clicked();

  void on_pushButton_clicked();

private:
  QSqlDatabase db;
  QSqlRelationalTableModel *model;
  void replaceHeader();
  Ui::Baseinfo *ui;
signals:
  void call_addstaffinfo_window();
};

#endif // BASEINFO_H
