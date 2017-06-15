#ifndef STYLEDITEMDELEGATE_H
#define STYLEDITEMDELEGATE_H

#include <QStyledItemDelegate>

class StyledItemDelegate : public QStyledItemDelegate
{
public:
	using QStyledItemDelegate::QStyledItemDelegate;

	QSize sizeHint(const QStyleOptionViewItem &option,
	               const QModelIndex &index) const override;
};

#endif // STYLEDITEMDELEGATE_H
