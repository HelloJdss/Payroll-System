#ifndef PRIVILEGE_H
#define PRIVILEGE_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlTableModel>

namespace Ui {
class Privilege;
}

class Privilege : public QDialog {
  Q_OBJECT

public:
  explicit Privilege(QWidget *parent = 0);
  void setDatabaseConnect(QSqlDatabase db);
  void execQuery();
  ~Privilege();
public slots:
  void view_sort(int column);

private slots:
  void on_comboBox_currentIndexChanged(const QString &arg1);

  void on_pushButton_3_clicked();

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

private:
  void replaceHeader();
  QSqlDatabase db;
  QSqlTableModel *model;
  Ui::Privilege *ui;
};

#endif // PRIVILEGE_H
