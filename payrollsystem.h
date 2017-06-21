#ifndef PAYROLLSYSTEM_H
#define PAYROLLSYSTEM_H

#include "addbonus.h"
#include "addcraftinfo.h"
#include "addstaffinfo.h"
#include "addsalary.h"
#include "annualbonus.h"
#include "attendance.h"
#include "privilege.h"
#include "modifypersonalinformation.h"
#include "bonus.h"
#include "baseinfo.h"
#include "craft.h"
#include "dept.h"
#include "login.h"
#include "salary.h"
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

  void on_action_M_2_triggered();

  void on_action_M_3_triggered();

  void on_action_S_triggered();

  void on_action_A_4_triggered();

  void on_action_M_4_triggered();

  void on_action_Q_3_triggered();

  void on_action_A_5_triggered();

  void on_action_M_5_triggered();

  void on_action_Q_4_triggered();

  void on_action_E_triggered();

  void on_action_H_triggered();

  void on_action_S_2_triggered();

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
  void updateCraftTable();
  void updateStaffTable();
public slots:
  void create_Addcraftinfo_Window();
  void create_Addstaffinfo_Window();
};

#endif // PAYROLLSYSTEM_H
