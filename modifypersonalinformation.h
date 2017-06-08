#ifndef MODIFYPERSONALINFORMATION_H
#define MODIFYPERSONALINFORMATION_H

#include <QDialog>
#include <QSqlDatabase>


namespace Ui {
  class ModifyPersonalInformation;
}

class ModifyPersonalInformation : public QDialog
{
  Q_OBJECT

public:
  explicit ModifyPersonalInformation(QWidget *parent = 0);
  void setDatabaseConnect(QSqlDatabase db);
  bool setSid(int Sid);
  void saveInfo();
  ~ModifyPersonalInformation();

private:
  int Sid;
  QSqlDatabase db;
  Ui::ModifyPersonalInformation *ui;
};

#endif // MODIFYPERSONALINFORMATION_H
