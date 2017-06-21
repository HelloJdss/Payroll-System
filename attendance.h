#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include "noeditdelegate.h"
#include <QDialog>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

namespace Ui {
  class Attendance;
}

class Attendance : public QDialog
{
  Q_OBJECT

public:
  explicit Attendance(QWidget *parent = 0);

  ~Attendance();
public slots:
  void view_sort(int column);
private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_7_clicked();

  void on_pushButton_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_8_clicked();

private:
  void selectAll();
  void replaceHeader();
  QSqlTableModel *model;
  Ui::Attendance *ui;
};

#endif // ATTENDANCE_H
