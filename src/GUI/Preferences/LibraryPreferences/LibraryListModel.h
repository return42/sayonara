#ifndef LIBRARYLISTMODEL_H
#define LIBRARYLISTMODEL_H

#include <QAbstractListModel>
#include "Helper/Pimpl.h"

typedef QString LibName;
typedef QString LibPath;

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

	void append_row(const LibName& name, const LibPath& path);
	void rename_row(int row, const LibName& new_name);
	void move_row(int row_idx, int new_idx);
	void remove_row(int row_idx);

	QStringList get_all_names() const;
	QStringList get_all_paths() const;

	void reset();
	void commit();
};

#endif // LIBRARYLISTMODEL_H
