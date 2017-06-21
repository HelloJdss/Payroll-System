#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>

/*该类是为了继承实现自定义代理，使得采用该代理的表格列仅仅可以选择指定的内容*/

class ComboBoxDelegate :public QStyledItemDelegate
{
  Q_OBJECT
public:
  ComboBoxDelegate(QStringList &Texts, QObject *parent = 0);
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
private:
  QStringList Texts;
};

#endif // COMBOBOXDELEGATE_H
