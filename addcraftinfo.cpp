#include "addcraftinfo.h"
#include "ui_addcraftinfo.h"
#include <QIntValidator>

AddCraftInfo::AddCraftInfo(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddCraftInfo) {
  ui->setupUi(this);
  ui->lineEdit_3->setValidator(new QIntValidator(this));
}

AddCraftInfo::~AddCraftInfo() { delete ui; }

QString AddCraftInfo::getCraftname() { return ui->lineEdit->text(); }

QString AddCraftInfo::getCraftlevel() { return ui->lineEdit_2->text(); }

int AddCraftInfo::getCraftbase() { return ui->lineEdit_3->text().toInt(); }

QString AddCraftInfo::getCtaftDept() { return ui->comboBox->currentText(); }

void AddCraftInfo::fillComboBox(QStringList t) { ui->comboBox->addItems(t); }
