
#ifndef ALBUMCOVERDELEGATE_H
#define ALBUMCOVERDELEGATE_H

#include <QStyledItemDelegate>

class AlbumCoverDelegate : public QStyledItemDelegate
{
public:
	AlbumCoverDelegate(QObject* parent=nullptr);
	virtual ~AlbumCoverDelegate();

	// QAbstractItemDelegate interface
public:
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

};

#endif // ALBUMCOVERDELEGATE_H
