#ifndef NOEDITDELEGATE_H
#define NOEDITDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>

/*该类是为了继承实现自定义代理，使得采用该代理的表格列不可编辑*/

class NoEditDelegate :public QStyledItemDelegate
{
public:
  NoEditDelegate(QObject *parent = 0);
  ~NoEditDelegate();
  QWidget *createEditor(QWidget *, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const;
};

#endif // NOEDITDELEGATE_H
