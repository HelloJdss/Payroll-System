#ifndef ANNUALBONUS_H
#define ANNUALBONUS_H

#include <QDialog>
#include <QSqlQueryModel>
#include "salary.h"

namespace Ui {
  class AnnualBonus;
}

class AnnualBonus : public QDialog
{
  Q_OBJECT

public:
  explicit AnnualBonus(QWidget *parent = 0);
  void setSid(int Sid);
  ~AnnualBonus();
public slots:
  void view_sort(int column);
private slots:
  void on_dateEdit_dateChanged(const QDate &);

  void on_lineEdit_textChanged(const QString &);

  void on_pushButton_clicked();

private:
  void Query();
  QSqlQueryModel *model;
  Ui::AnnualBonus *ui;
};

#endif // ANNUALBONUS_H
