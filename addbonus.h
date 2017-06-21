#ifndef ADDBONUS_H
#define ADDBONUS_H

#include <QDialog>

namespace Ui {
  class AddBonus;
}

class AddBonus : public QDialog
{
  Q_OBJECT

public:
  explicit AddBonus(QWidget *parent = 0);
  ~AddBonus();
  void saveInfo();
private:
  Ui::AddBonus *ui;
};

#endif // ADDBONUS_H
