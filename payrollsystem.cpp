#include "payrollsystem.h"
#include "ui_payrollsystem.h"
#include <QDateTime>
#include <QDebug>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>
#include <QTimer>

PayrollSystem::PayrollSystem(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PayrollSystem)
{
  ui->setupUi(this);
  db = QSqlDatabase::addDatabase("QMYSQL");
  localtime = new QLabel;
  QTimer *localtimer = new QTimer(this);
  connect(localtimer, &QTimer::timeout, this, &PayrollSystem::timerupdate);
  ui->statusBar->addPermanentWidget(this->localtime);
  l1 = new QLabel;
  currentUser = new QLabel;
  l1->setText(QString::fromLocal8Bit("当前用户："));
  ui->statusBar->addWidget(l1);
  currentUser->setText(QString::fromLocal8Bit("未登录"));
  ui->statusBar->addWidget(this->currentUser);
  localtimer->start(1000);
  isLogin = false;
  currentUserPrivilege = 0;
}

PayrollSystem::~PayrollSystem()
{
  delete ui;
  delete localtime;
}

bool PayrollSystem::ConnectDatabase()  //连接数据库
{
  db.setDatabaseName(login_w.dbname);
  db.setHostName(login_w.dbhost);
  db.setPort(login_w.dbport);
  db.setUserName(login_w.dbuser);
  db.setPassword(login_w.dbpswd);
  if (db.open()) {
    qDebug() << "Connected";
    qDebug() << db.databaseName();
    QSqlQuery query;
    query.exec("use " + login_w.dbname + ";");
    return true;
  } else
    qDebug() << "Disconnected";
  return false;
}

void PayrollSystem::on_action_L_triggered() //用户登录
{
  if (isLogin) {
    QMessageBox::warning(this, QStringLiteral("警告!"), QStringLiteral("用户已登录，请先注销!"), QMessageBox::Ok);
    return;
  }
  login_w.setLoginMode();
  if (QDialog::Accepted == login_w.exec()) {
    isLogin = ConnectDatabase();
    if (!isLogin){
        QMessageBox::critical(this, QStringLiteral("错误!"), QStringLiteral("登录失败！请检查用户名或密码！"), QMessageBox::Ok);
        qDebug()<<db.databaseName()<<" "<<db.userName()<<" "<<db.hostName()<<" "<<db.password()<<" "<<db.port();
      }
    else {
      QMessageBox::information(this, QStringLiteral("登录成功!"), QString::fromLocal8Bit("登录成功!\n当前用户: ") + login_w.dbuser);
      QSqlQuery query(db);
      query.exec(QString("select Privilege from users where Username = '%1'").arg(db.userName()));
      query.last();
      bool OK;
      currentUserPrivilege = query.value(0).toInt(&OK);
      // 10级为最高权限
      if (!OK)
        currentUserPrivilege = 0;
      QString p;
      p.append(QString::fromLocal8Bit(" 特权等级: "));
      p.append(QString("%1").arg(this->currentUserPrivilege));
      this->currentUser->setText(db.userName()+p);
    }
  }
}

void PayrollSystem::timerupdate() //本地时间更新
{
  QDateTime time = QDateTime::currentDateTime();
  QString text = QString::fromLocal8Bit("本地时间: ");
  text += time.toString("yyyy-MM-dd hh:mm:ss");
  this->localtime->setText(text);
}

bool PayrollSystem::isCertificated(int require_privilege) //权限检测
{
  if (!isLogin) {
    QMessageBox::warning(this, QStringLiteral("警告!"), QStringLiteral("请先登录帐号！"), QMessageBox::Ok);
    return false;
  }
  if (require_privilege > currentUserPrivilege) {
    QMessageBox::critical(this, QStringLiteral("错误!"), QStringLiteral("当前用户特权等级不足！"), QMessageBox::Ok);
    return false;
  }
  return true;
}

void PayrollSystem::create_Addcraftinfo_Window() //创建工种信息 --5
{
  if (!isCertificated(5))
    return;
  AddCraftInfo a;
  QSqlQuery q1(db);
  q1.exec("select Dname from dept");
  QStringList DnameList;
  while (q1.next()) {
    DnameList << q1.record().value(0).toString();
  }
  a.fillComboBox(DnameList);
  while (QDialog::Accepted == a.exec()) {
    QSqlQuery query(db);
    query.prepare("insert into craft(Cname,Clevel,Cbase,Cdept) "
                  "values(:name,:level,:base,:dept)");
    // while (query.next())
    QString name, level;
    int base, dept;
    name = a.getCraftname();
    level = a.getCraftlevel();
    base = a.getCraftbase();
    q1.exec(QString("select Did from dept where Dname = '%1'")
                .arg(a.getCtaftDept()));
    q1.last();
    dept = q1.record().value(0).toInt();
    query.bindValue(":name", name);
    query.bindValue(":level", level);
    query.bindValue(":base", base);
    query.bindValue(":dept", dept);
    if (query.exec()) {
      QMessageBox::information(&a, QStringLiteral("提示!"), QStringLiteral("添加信息成功！"), QMessageBox::Ok);
      emit updateCraftTable();
    }

    else {
      QMessageBox::critical(&a, QStringLiteral("错误!"), QString("Error: %1").arg(query.lastError().text()), QMessageBox::Ok);
    }
  }
}

void PayrollSystem::create_Addstaffinfo_Window() //添加员工信息 --5
{
  if (!isCertificated(5))
    return;
  AddStaffInfo c;
  while (QDialog::Accepted == c.exec()) { //循环添加
    QSqlQuery query(db);
    query.prepare("insert into "
                  "baseinfo(Sname,Ssex,Sbirthday,Cid,Department,Hiredate,Tel,"
                  "Email) "
                  "values(:Sname,:Ssex,:Sbirthday,:Cid,:Department,:Hiredate,:"
                  "Tel,:Email)");
    // while (query.next())
    QString name, birthday, tel, email, department, hiredate, sex;
    int Cid;
    name = c.getName();
    birthday = c.getBirth();
    tel = c.getTel();
    email = c.getEmail();
    department = c.getDept();
    hiredate = c.getHiredate();
    sex = c.getSex();
    Cid = c.getCid();
    query.bindValue(":Sname", name);
    query.bindValue(":Ssex", sex);
    query.bindValue(":Sbirthday", birthday);
    query.bindValue(":Cid", Cid);
    query.bindValue(":Department", department);
    query.bindValue(":Hiredate", hiredate);
    query.bindValue(":Tel", tel);
    query.bindValue(":Email", email);
    if (query.exec()) {
      emit updateStaffTable();
      query.exec("select LAST_INSERT_ID()");
      query.last();
      QString username = query.record().value(0).toString();
      if (!query.exec(QString("create user '%1'@'localhost' identified by '%1';"
                              "grant select,update,insert on payroll.* to '%1'@'localhost'")
                              .arg("U" + username)))
        QMessageBox::critical(&c, QStringLiteral("错误!"), QString("Error: %1").arg(query.lastError().text()), QMessageBox::Ok);
      if (!query.exec(QString("insert into Users(Username,Privilege) values('%1',1);").arg("U" + username)))
        QMessageBox::critical(&c, QStringLiteral("错误!"), QString("Error: %1").arg(query.lastError().text()), QMessageBox::Ok);
      QMessageBox::information(&c, QStringLiteral("提示!"), QStringLiteral("添加信息成功！"), QMessageBox::Ok);
    } else {
      QMessageBox::critical(&c, QStringLiteral("错误!"), QString("Error: %1").arg(query.lastError().text()), QMessageBox::Ok);
    }
  }
}

void PayrollSystem::on_action_I_triggered() //工种信息查询 --5
{
  if (!isCertificated(5))
    return;
  craft c(this);
  connect(&c, &craft::call_addcraftinfo_window, this, &PayrollSystem::create_Addcraftinfo_Window);
  connect(this, &PayrollSystem::updateCraftTable, &c, &craft::updateTable);
  c.setDatabaseConnect(this->db);
  c.exec();
}

void PayrollSystem::on_action_A_triggered() //添加工种信息
{
  create_Addcraftinfo_Window();
}

void PayrollSystem::on_action_Q_triggered() { this->close(); }

void PayrollSystem::on_action_A_2_triggered() //添加员工信息
{
  create_Addstaffinfo_Window();
}

void PayrollSystem::on_action_A_3_triggered() //添加部门 --5
{
  if (!isCertificated(5))
    return;
  bool OK;
  QString deptname = QInputDialog::getText(this, QStringLiteral("添加部门"),
                                           QStringLiteral("请输入部门名称："),
                                           QLineEdit::Normal, QString(), &OK);
  if (!OK)
    return;
  QSqlQuery q(db);
  if (q.exec(QString("insert into dept(Dname) values('%1')").arg(deptname.left(20))))
    QMessageBox::information(this, QStringLiteral("提示!"), QStringLiteral("添加信息成功！"), QMessageBox::Ok);
  else
    QMessageBox::critical(this, QStringLiteral("错误!"), QString("Error: %1").arg(q.lastError().text()), QMessageBox::Ok);
}

void PayrollSystem::on_action_D_triggered()  //解散部门 --5
{
  if (!isCertificated(5))
    return;
  bool OK;
  QString deptname = QInputDialog::getText(this, QStringLiteral("解散部门"),
                                           QStringLiteral("请输入部门名称：(注：请保证该部门不再有员工以及工种信息)"),
                                           QLineEdit::Normal, QString(), &OK);
  if (!OK)
    return;
  QSqlQuery q(db);
  if (q.exec(QString("delete from dept where Dname = '%1'").arg(deptname.left(20))))
    QMessageBox::information(0, QStringLiteral("提示!"), QStringLiteral("删除信息成功！"), QMessageBox::Ok);
  else
    QMessageBox::critical(0, QStringLiteral("错误!"), QString("Error: %1").arg(q.lastError().text()), QMessageBox::Ok);
}

void PayrollSystem::on_action_L_2_triggered()  //查询部门 --5
{
  if (!isCertificated(5))
    return;
  Dept a;
  a.setDatabaseConnect(db);
  a.execQuery();
  a.exec();
}

void PayrollSystem::on_action_Q_2_triggered() //注销登录 --0
{
  if(!isCertificated(0))
    return;
  db.close();
  currentUser->setText(QString::fromLocal8Bit("未登录"));
  QMessageBox::information(this, QStringLiteral("提示!"), QStringLiteral("用户已注销!"));
  isLogin = false;
  login_w.clearPswd();
}

void PayrollSystem::on_action_P_triggered() //修改密码 -- 1
{
  if (!isCertificated(1))
    return;
  bool OK;
  QString pswd = QInputDialog::getText(this, QStringLiteral("修改密码"), QStringLiteral("请输入新密码："), QLineEdit::PasswordEchoOnEdit, QString(), &OK);
  if (!OK)
    return;
  if (pswd != QInputDialog::getText(this, QStringLiteral("修改密码"), QStringLiteral("请确认新密码："), QLineEdit::Password, QString(), &OK)) {
    QMessageBox::critical(this, QStringLiteral("错误!"), QStringLiteral("密码输入不一致！"), QMessageBox::Ok);
    return;
  }
  if (!OK)
    return;
  QSqlQuery q(db);
  if (q.exec(QString("set password = '%1'").arg(pswd)))
    QMessageBox::information(this, QStringLiteral("提示!"), QStringLiteral("密码已成功修改!"));
  else
    QMessageBox::critical(this, QStringLiteral("错误!"), QString("Error: %1").arg(q.lastError().text()), QMessageBox::Ok);
}

void PayrollSystem::on_action_M_triggered()//高级权限 -- 10
{
  if (!isCertificated(10))
    return;
  Privilege p;
  p.setDatabaseConnect(db);
  p.execQuery();
  p.exec();
}

void PayrollSystem::on_action_C_triggered() //修改个人信息 --1
{
  if (!isCertificated(1))
    return;
  if(db.userName()=="payroller") {
    QMessageBox::warning(this,QStringLiteral("警告!"),QStringLiteral("当前为数据库管理员用户!"),QMessageBox::Ok);
    return;
  }
  int Sid;
  bool OK;
  Sid=db.userName().mid(1).toInt(&OK);
  if(!OK) {
      QMessageBox::critical(this,QStringLiteral("错误!"),QStringLiteral("用户名格式错误!"),QMessageBox::Ok);
      return;
    }
  ModifyPersonalInformation m;
  m.setDatabaseConnect(db);
  if(!m.setSid(Sid))
    return;
  if(m.exec()==QDialog::Accepted) m.saveInfo();
}

void PayrollSystem::on_action_M_2_triggered() //员工信息管理 --5
{
  if(!isCertificated(5))
    return;
  Baseinfo b;
  connect(&b, &Baseinfo::call_addstaffinfo_window, this, &PayrollSystem::create_Addstaffinfo_Window);
  connect(this, &PayrollSystem::updateStaffTable, &b, &Baseinfo::updateTable);
  b.setDatabaseConnect(db);
  b.execQuery();
  b.exec();
}

void PayrollSystem::on_action_M_3_triggered() //考勤管理
{
  if(!isCertificated(5))
    return;
  Attendance a;
  a.exec();
}

void PayrollSystem::on_action_S_triggered() //员工签到
{
  if(!isCertificated(1))
    return;
  QString currentDate;
  qDebug()<<(currentDate = QDate::currentDate().toString("yyyy-MM-dd"));
  if(db.userName()=="payroller") {
    QMessageBox::warning(this,QStringLiteral("警告!"),QStringLiteral("当前为数据库管理员用户!"),QMessageBox::Ok);
    return;
  }
  int Sid;
  bool OK;
  Sid=db.userName().mid(1).toInt(&OK);
  if(!OK) {
      QMessageBox::critical(this,QStringLiteral("错误!"),QStringLiteral("用户名格式错误!"),QMessageBox::Ok);
      return;
    }
  QSqlQuery q;
  q.exec(QString("select * from attendance where Sid = '%1' and ADate = '%2'").arg(Sid).arg(currentDate));
  if(q.next()) {
      QMessageBox::critical(this,QStringLiteral("错误!"),QStringLiteral("请勿重复签到!"),QMessageBox::Ok);
      return;
    }
  OK = q.exec(QString("insert into attendance(Sid, ADate) values ('%1', '%2')").arg(Sid).arg(currentDate));
  if(OK)
    QMessageBox::information(this, QStringLiteral("提示!"), QStringLiteral("今日已签到!"));
  else
    QMessageBox::critical(this, QStringLiteral("错误!"), QString("Error: %1").arg(q.lastError().text()), QMessageBox::Ok);
}

void PayrollSystem::on_action_A_4_triggered() //添加津贴 --5
{
  if(!isCertificated(5))
    return;
  AddBonus a;
  while(QDialog::Accepted == a.exec()){
      a.saveInfo();
    }
}

void PayrollSystem::on_action_M_4_triggered() //管理津贴 --5
{
  if(!isCertificated(5))
    return;
  bonus b;
  b.exec();
}

void PayrollSystem::on_action_Q_3_triggered() //个人津贴查询 --1
{
  if (!isCertificated(1))
    return;
  if(db.userName()=="payroller") {
    QMessageBox::warning(this,QStringLiteral("警告!"),QStringLiteral("当前为数据库管理员用户!"),QMessageBox::Ok);
    return;
  }
  int Sid;
  bool OK;
  Sid = db.userName().mid(1).toInt(&OK);
  if(!OK) {
      QMessageBox::critical(this,QStringLiteral("错误!"),QStringLiteral("用户名格式错误!"),QMessageBox::Ok);
      return;
    }
  bonus b;
  b.setSid(Sid);
  b.exec();
}

void PayrollSystem::on_action_A_5_triggered() //录入月工资 --5
{
  if (!isCertificated(5))
    return;
  AddSalary a;
  while(QDialog::Accepted == a.exec()){
      a.saveInfo();
    }
}

void PayrollSystem::on_action_M_5_triggered() //月工资管理 --6
{
  if(!isCertificated(5))
    return;
  Salary s;
  s.exec();
}

void PayrollSystem::on_action_Q_4_triggered() //个人月工资查询 --1
{
  if(!isCertificated(1))
    return;
  if(db.userName()=="payroller") {
    QMessageBox::warning(this,QStringLiteral("警告!"),QStringLiteral("当前为数据库管理员用户!"),QMessageBox::Ok);
    return;
  }
  int Sid;
  bool OK;
  Sid = db.userName().mid(1).toInt(&OK);
  if(!OK) {
      QMessageBox::critical(this,QStringLiteral("错误!"),QStringLiteral("用户名格式错误!"),QMessageBox::Ok);
      return;
    }
  Salary s;
  s.staffQuery(Sid);
  s.exec();
}

void PayrollSystem::on_action_E_triggered() //年终奖查询 --1 or 5
{
  if(!isCertificated(1))
    return;
  AnnualBonus a;
  if(db.userName()=="payroller") {
  //  QMessageBox::warning(0,QStringLiteral("警告!"),QStringLiteral("当前为数据库管理员用户!"),QMessageBox::Ok);
    a.exec();
    return;
  }
  int Sid;
  bool OK;
  Sid = db.userName().mid(1).toInt(&OK);
  if(!OK) {
      QMessageBox::critical(this,QStringLiteral("错误!"),QStringLiteral("用户名格式错误!"),QMessageBox::Ok);
      return;
    }
  if(currentUserPrivilege >= 5) //可查询他人
    a.exec();
  else{  //仅可查询自己
      a.setSid(Sid);
      a.exec();
    }
}

void PayrollSystem::on_action_H_triggered()
{
  QMessageBox::information(this,QStringLiteral("帮助"),
                           QStringLiteral("本数据库管理系统由\n"
                                          "华中科技大学计算机科学与技术系CS1401班 覃宇骁 制作\n"
                                          ">> 本管理系统所依托的数据库软件为MySQL，使用前请保证数据库中存在满足依赖条件的数据库实例!\n"
                                          ">> 本管理系统所依托的数据库表项：\n>"
                                          "\t1. 员工工种信息表 craft\n"
                                          "\t2. 员工部门信息表 dept\n"
                                          "\t3. 员工基本信息表 baseinfo\n"
                                          "\t4. 用户登陆特权表 users\n"
                                          "\t5. 员工考勤信息表 attendance\n"
                                          "\t6. 员工津贴信息表 bonus\n"
                                          "\t7. 员工工资信息表 salary\n"
                                          ">> 本管理系统默认连接端口号3306，连接地址\'localhost\'，数据库名称\'payroll\'您可以通过数据库设置来修改它!\n"
                                          ">> 本管理系统管理员帐户为\'payroller\'，不可修改，请保证数据库中存在具有相应特权级的用户!\n"
                                          "要求权限：\n>"
                                          "\t1. 建立数据库连接\n"
                                          "\t2. 对\'payroller\'内所有表项增删查改\n"
                                          "\t3. 在数据库中创建用户并授予权限\n"
                                          ">> 本管理系统由于开发时间和作者个人能力所限，还存在诸多不合理和待完善之处，如有疑问，可联系邮箱382825415@qq.com"),
                           QMessageBox::Ok);
}

void PayrollSystem::on_action_S_2_triggered() //数据库设置
{
  login_w.setSettingMode();
  login_w.exec();
}
