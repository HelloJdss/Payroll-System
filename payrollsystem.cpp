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
    : QMainWindow(parent), ui(new Ui::PayrollSystem) {
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

PayrollSystem::~PayrollSystem() {
  delete ui;
  delete localtime;
}

bool PayrollSystem::ConnectDatabase() {
  db.setDatabaseName(login_w.dbname);
  db.setHostName(login_w.dbhost);
  db.setPort(login_w.dbport);
  db.setUserName(login_w.dbuser);
  db.setPassword(login_w.dbpswd);
  if (db.open()) {
    qDebug() << "Connected";
    qDebug() << db.databaseName();
    QSqlQuery query;
    qDebug() << query.exec("use " + login_w.dbname + ";");
    this->currentUser->setText(db.userName());
    return true;
  } else
    qDebug() << "Disconnected";
  return false;
}

void PayrollSystem::on_action_L_triggered() { //用户登录
  if (isLogin) {
    QMessageBox::warning(0, QStringLiteral("警告!"), QStringLiteral("用户已登录，请先注销!"), QMessageBox::Ok);
    return;
  }
  if (QDialog::Accepted == login_w.exec()) {
    isLogin = ConnectDatabase();
    if (!isLogin)
      QMessageBox::critical(0, QStringLiteral("错误!"), QStringLiteral("登录失败！请检查用户名或密码！"), QMessageBox::Ok);
    else {
      QMessageBox::information(0, QStringLiteral("登录成功!"), QString::fromLocal8Bit("登录成功!\n当前用户: ") + login_w.dbuser);
      QSqlQuery query(db);
      query.exec(QString("select Privilege from users where Username = '%1'").arg(db.userName()));
      query.last();
      bool OK;
      currentUserPrivilege = query.value(0).toInt(&OK);
      // 10级为最高权限
      if (!OK)
        currentUserPrivilege = 0;
    }
  }
}

void PayrollSystem::timerupdate() {
  QDateTime time = QDateTime::currentDateTime();
  QString text = QString::fromLocal8Bit("本地时间: ");
  text += time.toString("yyyy-MM-dd hh:mm:ss");
  this->localtime->setText(text);
}

bool PayrollSystem::isCertificated(int require_privilege) {
  if (!isLogin) {
    QMessageBox::warning(0, QStringLiteral("警告!"), QStringLiteral("请先登录帐号！"), QMessageBox::Ok);
    return false;
  }
  if (require_privilege > currentUserPrivilege) {
    QMessageBox::critical(0, QStringLiteral("错误!"), QStringLiteral("当前用户特权等级不足！"), QMessageBox::Ok);
    return false;
  }
  return true;
}

void PayrollSystem::create_Addcraftinfo_Window() { //创建员工信息 --5
  if (!isCertificated(5))
    return;
  AddCraftInfo a(this);
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
      QMessageBox::information(0, QStringLiteral("提示!"), QStringLiteral("添加信息成功！"), QMessageBox::Ok);
      emit updateTable();
    }

    else {
      QMessageBox::critical(0, QStringLiteral("错误!"), QString("Error: %1").arg(query.lastError().text()), QMessageBox::Ok);
    }
  }
}

void PayrollSystem::on_action_I_triggered() //工种信息查询 --5
{
  if (!isCertificated(5))
    return;
  craft c(this);
  connect(&c, &craft::call_addcraftinfo_window, this, &PayrollSystem::create_Addcraftinfo_Window);
  connect(this, &PayrollSystem::updateTable, &c, &craft::updateTable);
  c.setDatabaseConnect(this->db);
  c.exec();
}

void PayrollSystem::on_action_A_triggered() //添加工种信息
{
  create_Addcraftinfo_Window();
}

void PayrollSystem::on_action_Q_triggered() { this->close(); }

void PayrollSystem::on_action_A_2_triggered() //添加员工信息 --5
{
  if (!isCertificated(5))
    return;
  AddStaffInfo c(this);
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
      emit updateTable();
      query.exec("select LAST_INSERT_ID()");
      query.last();
      if (!query.exec(QString("create user '%1'@'localhost' identified by '%1'").arg("U" + query.record().value(0).toString())))
        QMessageBox::critical(0, QStringLiteral("错误!"), QString("Error: %1").arg(query.lastError().text()), QMessageBox::Ok);
      if (!query.exec(QString("grant select,update,insert on payroll.* to '%1'@'localhost'").arg("U" + query.record().value(0).toString())))
        QMessageBox::critical(0, QStringLiteral("错误!"), QString("Error: %1").arg(query.lastError().text()), QMessageBox::Ok);
      if (!query.exec(QString("insert into Users(Username,Privilege) values('%1',1);").arg("U" + query.record().value(0).toString())))
        QMessageBox::critical(0, QStringLiteral("错误!"), QString("Error: %1").arg(query.lastError().text()), QMessageBox::Ok);
      QMessageBox::information(0, QStringLiteral("提示!"), QStringLiteral("添加信息成功！"), QMessageBox::Ok);
    } else {
      QMessageBox::critical(0, QStringLiteral("错误!"), QString("Error: %1").arg(query.lastError().text()), QMessageBox::Ok);
    }
  }
}

void PayrollSystem::on_action_A_3_triggered() //添加部门 --5
{
  if (!isCertificated(5))
    return;
  QString deptname = QInputDialog::getText(0, QStringLiteral("添加部门"), QStringLiteral("请输入部门名称："), QLineEdit::Normal);
  if (!deptname.size())
    return;
  QSqlQuery q(db);
  if (q.exec(QString("insert into dept(Dname) values('%1')").arg(deptname.left(20))))
    QMessageBox::information(0, QStringLiteral("提示!"), QStringLiteral("添加信息成功！"), QMessageBox::Ok);
  else
    QMessageBox::critical(0, QStringLiteral("错误!"), QString("Error: %1").arg(q.lastError().text()), QMessageBox::Ok);
}

void PayrollSystem::on_action_D_triggered() { //解散部门 --5
  if (!isCertificated(5))
    return;
  QString deptname = QInputDialog::getText(0,
                                           QStringLiteral("解散部门"),
                                           QStringLiteral("请输入部门名称：(注：请保证该部门不再有员工以及工种信息)"),
                                           QLineEdit::Normal);
  if (!deptname.size())
    return;
  QSqlQuery q(db);
  if (q.exec(QString("delete from dept where Dname = '%1'").arg(deptname.left(20))))
    QMessageBox::information(0, QStringLiteral("提示!"), QStringLiteral("删除信息成功！"), QMessageBox::Ok);
  else
    QMessageBox::critical(0, QStringLiteral("错误!"), QString("Error: %1").arg(q.lastError().text()), QMessageBox::Ok);
}

void PayrollSystem::on_action_L_2_triggered() { //查询部门 --5
  if (!isCertificated(5))
    return;
  Dept a;
  a.setDatabaseConnect(db);
  a.fillTable();
  a.exec();
}

void PayrollSystem::on_action_Q_2_triggered() //注销登录
{
  db.close();
  currentUser->setText(QString::fromLocal8Bit("未登录"));
  QMessageBox::information(0, QStringLiteral("提示!"), QStringLiteral("用户已注销!"));
  isLogin = false;
  login_w.clearPswd();
}

void PayrollSystem::on_action_P_triggered() //修改密码 -- 1
{
  if (!isCertificated(1))
    return;
  bool OK;
  QString pswd = QInputDialog::getText(0, QStringLiteral("修改密码"), QStringLiteral("请输入新密码："), QLineEdit::PasswordEchoOnEdit, QString(), &OK);
  if (!OK)
    return;
  if (pswd != QInputDialog::getText(0, QStringLiteral("修改密码"), QStringLiteral("请确认新密码："), QLineEdit::Password, QString(), &OK)) {
    QMessageBox::critical(0, QStringLiteral("错误!"), QStringLiteral("密码输入不一致！"), QMessageBox::Ok);
    return;
  }
  if (!OK)
    return;
  QSqlQuery q(db);
  if (q.exec(QString("set password = '%1'").arg(pswd)))
    QMessageBox::information(0, QStringLiteral("提示!"), QStringLiteral("密码已成功修改!"));
  else
    QMessageBox::critical(0, QStringLiteral("错误!"), QString("Error: %1").arg(q.lastError().text()), QMessageBox::Ok);
}


void PayrollSystem::on_action_M_triggered()//高级权限 -- 10
{
  if (!isCertificated(10))
    return;
  Privilege p;
  p.setDatabaseConnect(db);
  p.fillTable();
  p.exec();
}

void PayrollSystem::on_action_C_triggered() //修改个人信息 --1
{
  if (!isCertificated(1))
    return;
  if(db.userName()=="payroller") {
    QMessageBox::warning(0,QStringLiteral("警告!"),QStringLiteral("当前为数据库管理员用户!"),QMessageBox::Ok);
    return;
  }
  int Sid;
  bool OK;
  Sid=db.userName().mid(1).toInt(&OK);
  if(!OK) {
      QMessageBox::critical(0,QStringLiteral("错误!"),QStringLiteral("用户名格式错误!"),QMessageBox::Ok);
      return;
    }
  ModifyPersonalInformation m;
  m.setDatabaseConnect(db);
  if(!m.setSid(Sid))
    return;
  if(m.exec()==QDialog::Accepted) m.saveInfo();
}
