#ifndef ADDCRAFTINFO_H
#define ADDCRAFTINFO_H

#include <QDialog>

namespace Ui {
class AddCraftInfo;
}

class AddCraftInfo : public QDialog {
  Q_OBJECT

public:
  explicit AddCraftInfo(QWidget *parent = 0);
  ~AddCraftInfo();
  QString getCraftname();
  QString getCraftlevel();
  int getCraftbase();
  QString getCtaftDept();
  void fillComboBox(QStringList);

private:
  Ui::AddCraftInfo *ui;
};

#endif // ADDCRAFTINFO_H
