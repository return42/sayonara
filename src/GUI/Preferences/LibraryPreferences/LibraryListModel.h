#ifndef LIBRARYLISTMODEL_H
#define LIBRARYLISTMODEL_H

#include <QAbstractListModel>
#include "Helper/Pimpl.h"

class LibraryListModel :
		public QAbstractListModel
{
	Q_OBJECT
	PIMPL(LibraryListModel)

public:
	LibraryListModel(QObject* parent=nullptr);
	~LibraryListModel();

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent=QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	void append_row(const QString& name, const QString& path);
	void move_row(int row_idx, int new_idx);
	void remove_row(int row_idx);
	void reset();
};

#endif // LIBRARYLISTMODEL_H
