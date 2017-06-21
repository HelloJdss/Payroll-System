#include "noeditdelegate.h"

NoEditDelegate::NoEditDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{

}

NoEditDelegate::~NoEditDelegate()
{

}

QWidget *NoEditDelegate::createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
{
  return NULL;
}
