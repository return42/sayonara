#include "AlbumCoverModel.h"
#include "Components/Covers/CoverLocation.h"
#include "Helper/MetaData/Album.h"
#include "Helper/Logger/Logger.h"

#include <QPixmap>


struct AlbumCoverModel::Private
{
	QList<CoverLocation> cover_locations;
	AlbumList albums;
};

AlbumCoverModel::AlbumCoverModel(QObject* parent) :
	LibraryItemModel()
{
	_m = Pimpl::make<Private>();
}

AlbumCoverModel::~AlbumCoverModel() {}


int AlbumCoverModel::rowCount(const QModelIndex& parent) const
{
	return 2;
}

int AlbumCoverModel::columnCount(const QModelIndex& parent) const
{
	return _m->cover_locations.size();
}

QVariant AlbumCoverModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid()){
		return QVariant();
	}

	int row = index.row();
	int col = index.column();
	const CoverLocation& cl = _m->cover_locations[col];
	const Album& album = _m->albums[col];
	//sp_log(Log::Debug) << "Album name" << album.name;

	if(row == 0){
		switch(role){
			case Qt::DecorationRole:
				return QPixmap(cl.preferred_path()).scaled(100, 100, Qt::KeepAspectRatio);
			case Qt::SizeHintRole:
				return QSize(100, 100);
			default:
				return QVariant();
		}
	}

	else if(row == 1){
		switch(role){
			case Qt::DisplayRole:
				return album.name;
			default:
				return QVariant();
		}
	}


	else{
		sp_log(Log::Debug) << "Unknown row";
	}
	return QVariant();
}


void AlbumCoverModel::set_data(const AlbumList& albums, const QList<CoverLocation>& cover_locations)
{
	beginRemoveRows(QModelIndex(), 0, rowCount());
	endRemoveRows();
	beginInsertRows(QModelIndex(), 0, 2);
	endInsertRows();
	beginRemoveColumns(QModelIndex(), 0, columnCount());
	endRemoveColumns();
	beginInsertColumns(QModelIndex(), 0, albums.size());
	endInsertColumns();

	_m->cover_locations = cover_locations;
	_m->albums = albums;

	int rows = rowCount();
	int cols = columnCount();

	emit dataChanged(index(0, 0),
					 index(rows, cols)
	);
}


QModelIndex AlbumCoverModel::getFirstRowIndexOf(const QString& substr)
{
	return QModelIndex();
}

QModelIndex AlbumCoverModel::getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	return QModelIndex();
}

QModelIndex AlbumCoverModel::getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	return QModelIndex();
}

QMap<QChar, QString> AlbumCoverModel::getExtraTriggers()
{
	return QMap<QChar, QString>();
}


int AlbumCoverModel::get_searchable_column() const
{
	return 0;
}

QString AlbumCoverModel::get_string(int row) const
{
	return QString();
}

int AlbumCoverModel::get_id_by_row(int row)
{
	return -1;
}

CoverLocation AlbumCoverModel::get_cover(const SP::Set<int>& indexes) const
{
	return CoverLocation::getInvalidLocation();
}
