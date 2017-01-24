#include "AlbumCoverModel.h"
#include "Components/Covers/CoverLocation.h"
#include "Helper/MetaData/Album.h"

#include <QPixmap>
#include <QVector>


struct AlbumCoverModel::Private
{
	QList<CoverLocation> cover_locations;
	AlbumList albums;
	QHash<QString, QPixmap> pixmaps;

	int size;
	int columns;

	Private()
	{
		size = 100;
		columns = 10;
	}
};

static QString get_hash(const Album& album)
{
	return album.name + "-" + album.id;
}

AlbumCoverModel::AlbumCoverModel(QObject* parent) :
	LibraryItemModel()
{
	_m = Pimpl::make<Private>();
}

AlbumCoverModel::~AlbumCoverModel() {}


int AlbumCoverModel::rowCount(const QModelIndex& parent) const
{
	return (_m->albums.size() + columnCount() - 1 )/ columnCount();
}

int AlbumCoverModel::columnCount(const QModelIndex& parent) const
{
	return _m->columns;
}

void AlbumCoverModel::set_max_columns(int columns)
{
	_m->columns = columns;
}

QVariant AlbumCoverModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid()) {
		return QVariant();
	}

	int row = index.row();
	int col = index.column();

	int n_columns = columnCount();

	int idx = (row * n_columns) + col;
	if(idx >= _m->cover_locations.size()){
		return QVariant();
	}

	switch(role)
	{
		case Qt::DisplayRole:
			return _m->albums[idx].name;

		case Qt::DecorationRole:
			{
				QPixmap p;
				QString hash = get_hash(_m->albums[idx]);

				if( !_m->pixmaps.contains(hash) )
				{
					const CoverLocation& cl = _m->cover_locations[idx];
					QString preferred = cl.preferred_path();
					p = QPixmap(preferred);
					if(cl.valid() && !CoverLocation::isInvalidLocation(cl.preferred_path())){
						_m->pixmaps[hash] = p;
					}
				}

				else {
					p = _m->pixmaps[hash];
				}

				return p.scaled(_m->size, _m->size, Qt::KeepAspectRatio);;
			}

		case Qt::SizeHintRole:
			return QSize(_m->size + 100, _m->size + 10);

		default:
			return QVariant();
	}

	return QVariant();
}


void AlbumCoverModel::set_data(const AlbumList& albums, const QList<CoverLocation>& cover_locations)
{
	beginRemoveRows(QModelIndex(), 0, rowCount());
	endRemoveRows();

	beginRemoveColumns(QModelIndex(), 0, columnCount());
	endRemoveColumns();

	_m->cover_locations = cover_locations;
	_m->albums = albums;

	beginInsertRows(QModelIndex(), 0, rowCount());
	endInsertRows();

	beginInsertColumns(QModelIndex(), 0, columnCount());
	endInsertColumns();

	emit dataChanged(index(0, 0),
					 index(rowCount(), columnCount())
					 );
}

void AlbumCoverModel::set_zoom(int zoom)
{
	_m->size = zoom;
	emit dataChanged(index(0, 0),
					 index(rowCount(), columnCount())
					 );
}


QModelIndex AlbumCoverModel::getFirstRowIndexOf(const QString& substr)
{
	return getNextRowIndexOf(substr, 0);
}

QModelIndex AlbumCoverModel::getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	for(int i=0; i<_m->albums.size(); i++){
		int idx = (i + cur_row) % _m->albums.size();
		QString title = get_string(idx);
		title = Library::convert_search_string(title, search_mode());

		if(title.contains(substr))
		{
			return this->index(idx / columnCount(), idx % columnCount());
		}
	}

	return QModelIndex();
}

QModelIndex AlbumCoverModel::getPrevRowIndexOf(const QString& substr, int row, const QModelIndex& parent)
{
	int len = _m->albums.size();
	for(int i=0; i<_m->albums.size(); i++){

		if(row - i < 0){
			row = len - 1;
		}

		int row_idx = (row - i) % len;

		QString title = get_string(row_idx);
		title = Library::convert_search_string(title, search_mode());
		if(title.contains(substr))
		{
			return this->index(row_idx / columnCount(), row_idx % columnCount());
		}
	}

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
	return _m->albums[row].name;
}

int AlbumCoverModel::get_id_by_row(int row)
{
	return -1;
}

CoverLocation AlbumCoverModel::get_cover(const SP::Set<int>& indexes) const
{
	return CoverLocation::getInvalidLocation();
}
