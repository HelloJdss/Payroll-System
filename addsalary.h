#ifndef ADDSALARY_H
#define ADDSALARY_H

#include <QDialog>

namespace Ui {
  class AddSalary;
}

class AddSalary : public QDialog
{
  Q_OBJECT

public:
  explicit AddSalary(QWidget *parent = 0);
  void saveInfo();
  ~AddSalary();

private slots:
  int caculateAttendancePunishment(int Sid);
  void updateInfo(int Sid);
  void on_spinBox_9_valueChanged(int arg1);

  void on_spinBox_valueChanged(int);

  void on_spinBox_2_valueChanged(int);
  
  void on_spinBox_5_valueChanged(int);
  
  void on_spinBox_8_valueChanged(int);

private:
  Ui::AddSalary *ui;
};

#endif // ADDSALARY_H
