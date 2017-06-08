#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) : QDialog(parent), ui(new Ui::Login) {
  ui->setupUi(this);
  dbname = "payroll";
  dbport = 3306;
  dbhost = "localhost";
}

void Login::clearPswd() { ui->lineEdit_2->clear(); }

Login::~Login() { delete ui; }

void Login::on_pushButton_clicked() {
  dbuser = ui->lineEdit->text();
  dbpswd = ui->lineEdit_2->text();
}
