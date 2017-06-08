#ifndef CRAFT_H
#define CRAFT_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRelationalTableModel>

class QSqlTableModel;
namespace Ui {
class craft;
}

class craft : public QDialog {
  Q_OBJECT

public:
  explicit craft(QWidget *parent = 0);
  void setDatabaseConnect(QSqlDatabase db);
  ~craft();
public slots:
  void updateTable();
  void view_sort(int column);
private slots:
  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

  void on_comboBox_currentIndexChanged(int index);

  void on_pushButton_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_7_clicked();

  void on_pushButton_8_clicked();

  void on_pushButton_9_clicked();

private:
  Ui::craft *ui;
  QSqlDatabase db;
  QSqlRelationalTableModel *model;
  int currentIndex;
  void replaceHeader();
signals:
  void call_addcraftinfo_window();
};

#endif // CRAFT_H
