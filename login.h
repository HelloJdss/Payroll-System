#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

namespace Ui {
class Login;
}

class Login : public QDialog {
  Q_OBJECT

public:
  explicit Login(QWidget *parent = 0);
  void setLoginMode();
  void setSettingMode();
  void clearPswd();
  ~Login();
  QString dbname;
  qint32 dbport;
  QString dbuser;
  QString dbpswd;
  QString dbhost;

private slots:
  void on_pushButton_clicked();

private:
  bool isSettingMode;
  Ui::Login *ui;
};

#endif // LOGIN_H
