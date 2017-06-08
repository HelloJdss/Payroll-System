#ifndef ADDSTAFFINFO_H
#define ADDSTAFFINFO_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQueryModel>

namespace Ui {
class addstaffinfo;
}

class AddStaffInfo : public QDialog {
  Q_OBJECT

public:
  explicit AddStaffInfo(QWidget *parent = 0);
  void setDatabaseConnect(QSqlDatabase db);
  QString getName();
  QString getSex();
  QString getBirth();
  int getCid();
  QString getDept();
  QString getHiredate();
  QString getTel();
  QString getEmail();
  ~AddStaffInfo();

private slots:
  void on_comboBox_currentIndexChanged(const QString &arg1);

  void on_comboBox_3_currentIndexChanged(int index);

private:
  Ui::addstaffinfo *ui;
  QSqlDatabase db;
  QSqlQueryModel *model;
  bool isInitialized;
  QList<int> DidList;
  void setCraftDept(); //根据craft表的信息填充comboBox
  void setCraftinfo(int DeptIndex);
};

#endif // ADDSTAFFINFO_H
