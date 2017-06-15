#include "StyledItemDelegate.h"
#include <QSize>

QSize StyledItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index)

	QFontMetrics fm = option.fontMetrics;
	return QSize(1, std::max(fm.height() + 4, 20));
}
