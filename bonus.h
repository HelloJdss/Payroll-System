#ifndef BONUS_H
#define BONUS_H

#include "noeditdelegate.h"
#include "addbonus.h"
#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
  class bonus;
}

class bonus : public QDialog
{
  Q_OBJECT

public:
  explicit bonus(QWidget *parent = 0);
  void setSid(int Sid);
  ~bonus();
public slots:
  void view_sort(int column);
private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_9_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_4_clicked();

private:
  bool isStaffQuery; //员工查看个人信息模式
  int Sid;
  QSqlTableModel* model;
  Ui::bonus *ui;
};

#endif // BONUS_H
