#include "comboboxdelegate.h"
#include <QComboBox>

ComboBoxDelegate::ComboBoxDelegate(QStringList &Texts, QObject *parent)
  : QStyledItemDelegate(parent)
{
  this->Texts = Texts;
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
  QComboBox *editor = new QComboBox(parent);
  return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QComboBox *combobox = static_cast <QComboBox*> (editor);
  combobox->addItems(Texts);
  bool find = false;
  QString currentText = index.model()->data(index, Qt::EditRole).toString();
  for(int i = 0; i < Texts.size(); i++){ //搜索是否存在正在显示的信息
      if(currentText == Texts.at(i)) {
          find = true;
          combobox->setCurrentIndex(i);
        }
    }
  if(!find){ //若不存在，则添加
      combobox->addItem(currentText);
      combobox->setCurrentIndex(Texts.size()); //设为正在显示的信息
    }
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QComboBox *combobox = static_cast <QComboBox*> (editor);
  model->setData(index, combobox->currentText() ,Qt::EditRole);
}
