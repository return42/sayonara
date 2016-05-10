#ifndef LISTDELEGATE_H_
#define LISTDELEGATE_H_

#include <QStyledItemDelegate>
#include <QSize>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QListView>

#include "Helper/Settings/SayonaraClass.h"

class ListDelegate : 
	public QStyledItemDelegate,
	protected SayonaraClass
{
	Q_OBJECT

protected:
	QListView*		_parent=nullptr;

public:
	ListDelegate(QListView* parent=nullptr);
	virtual ~ListDelegate();

	virtual QSize sizeHint(const QStyleOptionViewItem &option,
			       const QModelIndex &index) const override;

	virtual void paint( QPainter *painter,  
			const QStyleOptionViewItem &option, 
			const QModelIndex &index) const override;

};

#endif
