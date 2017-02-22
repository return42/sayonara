#ifndef ALBUMCOVERDELEGATE_H
#define ALBUMCOVERDELEGATE_H

#include <QStyledItemDelegate>
#include <QItemDelegate>

class AlbumCoverDelegate :
		public QItemDelegate
{
	Q_OBJECT

public:
	AlbumCoverDelegate(QObject* parent=nullptr);
	~AlbumCoverDelegate();

	// QAbstractItemDelegate interface
public:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // ALBUMCOVERDELEGATE_H
