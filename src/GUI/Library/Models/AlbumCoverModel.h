#ifndef ALBUMCOVERMODEL_H
#define ALBUMCOVERMODEL_H

#include "GUI/Library/Models/LibraryItemModel.h"
#include "Helper/Pimpl.h"
#include <QThread>

class Album;
class AlbumList;
class CoverLocation;
class CoverLookup;
class AlbumCoverModel :
		public LibraryItemModel
{
	Q_OBJECT
	PIMPL(AlbumCoverModel)

private:

public:
	explicit AlbumCoverModel(QObject* parent=nullptr);
	virtual ~AlbumCoverModel();

	void set_data(const AlbumList& albums);

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent=QModelIndex()) const override;
	int columnCount(const QModelIndex& paren=QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role) const override;

	QModelIndex getFirstRowIndexOf(const QString& substr) override;
	QModelIndex getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
	QModelIndex getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
	QMap<QChar, QString> getExtraTriggers() override;

	int get_searchable_column() const override;
	QString get_string(int row) const override;
	int get_id_by_row(int row) override;
	CoverLocation get_cover(const SP::Set<int>& indexes) const override;

	void set_max_columns(int columns);

public slots:
	void set_zoom(int zoom);

private slots:
	void next_hash(const QString& hash, const CoverLocation& cl);

};

#endif // ALBUMCOVERMODEL_H
