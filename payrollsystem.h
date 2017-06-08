#ifndef PAYROLLSYSTEM_H
#define PAYROLLSYSTEM_H

#include "addcraftinfo.h"
#include "addstaffinfo.h"
#include "privilege.h"
#include "modifypersonalinformation.h"
#include "craft.h"
#include "dept.h"
#include "login.h"
#include <QLabel>
#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>


class AddCraftInfo;
class AddStaffInfo;

namespace Ui {
class PayrollSystem;
}

class PayrollSystem : public QMainWindow {
  Q_OBJECT

public:
  explicit PayrollSystem(QWidget *parent = 0);
  ~PayrollSystem();
  bool ConnectDatabase();
private slots:
  void on_action_L_triggered();

  void on_action_I_triggered();

  void on_action_A_triggered();

  void on_action_Q_triggered();

  void on_action_A_2_triggered();

  void on_action_A_3_triggered();

  void on_action_D_triggered();

  void on_action_L_2_triggered();

  void on_action_Q_2_triggered();

  void on_action_P_triggered();


  void on_action_M_triggered();
  
  void on_action_C_triggered();

private:
  Ui::PayrollSystem *ui;
  QSqlDatabase db;
  Login login_w;
  QLabel *localtime, *currentUser, *l1;
  bool isLogin;
  int currentUserPrivilege;
  void timerupdate();
  bool isCertificated(int require_privilege); //判断当前用户拥有合法权限,传入参数为所需权限等级
signals:
  void updateTable();
public slots:
  void create_Addcraftinfo_Window();
};

#endif // PAYROLLSYSTEM_H
