#ifndef DEPT_H
#define DEPT_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlTableModel>

namespace Ui {
class Dept;
}

class Dept : public QDialog {
  Q_OBJECT

public:
  explicit Dept(QWidget *parent = 0);
  void setDatabaseConnect(QSqlDatabase db);
  void fillTable();
  ~Dept();
public slots:
  void view_sort(int column);

private slots:
  void on_lineEdit_textChanged(const QString &arg1);

  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

private:
  void replaceHeader();
  QSqlDatabase db;
  QSqlTableModel *model;
  Ui::Dept *ui;
};

#endif // DEPT_H
