#include "login.h"
#include "payrollsystem.h"
#include <QApplication>
#include <QDebug>
#include <QStyleFactory>
int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setStyle(QStyleFactory::create("fusion"));
  PayrollSystem w;
  w.show();
  return a.exec();
}
