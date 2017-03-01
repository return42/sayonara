#include "AlbumCoverModel.h"
#include "AlbumCoverFetchThread.h"

#include "Components/Covers/CoverLocation.h"
#include "Components/Covers/CoverLookup.h"
#include "Helper/MetaData/Album.h"
#include "Helper/Helper.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Set.h"

#include <QStringList>
#include <QPixmap>
#include <QVector>
#include <QThread>


struct AlbumCoverModel::Private
{
	AlbumCoverFetchThread* cover_thread=nullptr;
	AlbumList albums;
	QHash<QString, QPixmap> pixmaps;
	QHash<QString, CoverLocation> cover_locations;
	QHash<QString, QModelIndex> indexes;

	int size;
	int columns;
	int n_threads_running;

	Private(QObject* parent)
	{
		cover_thread = new AlbumCoverFetchThread(parent);

		size = 100;
		columns = 10;
		n_threads_running = 0;
	}
};

static QString get_hash(const Album& album)
{
	return album.name + "-" + album.id;
}

AlbumCoverModel::AlbumCoverModel(QObject* parent) :
	LibraryItemModel()
{
	// TODO: Not good, parent should be delegated through LibraryItemModel
	Q_UNUSED(parent);
	_m = Pimpl::make<Private>(this);

	connect(_m->cover_thread, &AlbumCoverFetchThread::sig_next, this, &AlbumCoverModel::next_hash);
}

AlbumCoverModel::~AlbumCoverModel() {}


int AlbumCoverModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return (_m->albums.size() + columnCount() - 1 )/ columnCount();
}

int AlbumCoverModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return _m->columns;
}

void AlbumCoverModel::set_max_columns(int columns)
{
	if(columns == 0){
		return;
	}

	if(columns == _m->columns){
		return;
	}

	int old_rows = rowCount();
	int rows = (_m->albums.size() / columns) + 1;

	int diff = columns - _m->columns;
	int first, last;
	if(diff > 0) {
		first = _m->columns;
		last = first + diff;

		_m->columns = columns;

		diff = old_rows - rows;
		beginRemoveRows(QModelIndex(), rows - diff, rows);
		endRemoveRows();

		beginInsertColumns(QModelIndex(), first, last);
		endInsertColumns();
	}

	else {
		diff = -diff;
		first = _m->columns - diff;
		last = _m->columns;

		beginRemoveColumns(QModelIndex(), first, last);
		_m->columns = columns;
		endRemoveColumns();

		diff = rows - old_rows;
		beginInsertRows(QModelIndex(), rows, rows + diff);
		endInsertRows();
	}

	emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

QVariant AlbumCoverModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid()) {
		return QVariant();
	}

	int row = index.row();
	int col = index.column();

	int n_columns = columnCount();
	int lin_idx = (row * n_columns) + col;
	if(lin_idx >= _m->albums.size()){
		return QVariant();
	}

	switch(role)
	{
		case Qt::DisplayRole:
			return _m->albums[lin_idx].name;

		case Qt::TextAlignmentRole:
			return Qt::AlignHCenter;

		case Qt::DecorationRole:
			{
				const Album& album = _m->albums[lin_idx];
				QString hash = get_hash(album);
				QPixmap p;
				if(!_m->pixmaps.contains(hash)) {
					CoverLocation cl;
					if(!_m->cover_locations.contains(hash)){
						cl = CoverLocation::get_cover_location(album);
						_m->cover_locations[hash] = cl;
					}

					else{
						cl = _m->cover_locations[hash];
					}

					p = QPixmap(cl.preferred_path());
					if(!CoverLocation::isInvalidLocation(cl.preferred_path())){
						_m->pixmaps[hash] = p;
					}

					else {
						if(!_m->cover_thread->isRunning()){
							_m->cover_thread->start();
						}

						_m->indexes[hash] = index;
						_m->cover_thread->add_data(hash, cl);
					}
				}

				else{
					p = _m->pixmaps[hash];
				}

				return p.scaled(_m->size, _m->size, Qt::KeepAspectRatio);;
			}

		case Qt::SizeHintRole:
			return get_item_size();

		default:
			return QVariant();
	}

	return QVariant();
}

QSize AlbumCoverModel::get_item_size() const
{
	return QSize(_m->size + 50, _m->size + 50);
}


void AlbumCoverModel::set_data(const AlbumList& albums)
{
	beginRemoveRows(QModelIndex(), 0, rowCount());
	endRemoveRows();

	beginRemoveColumns(QModelIndex(), 0, columnCount());
	endRemoveColumns();

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


void AlbumCoverModel::next_hash(const QString& hash, const CoverLocation& cl)
{
	AlbumCoverFetchThread* acft = dynamic_cast<AlbumCoverFetchThread*>(sender());
	QModelIndex idx = _m->indexes[hash];

	CoverLookup* clu = new CoverLookup(this, 1);
	connect(clu, &CoverLookup::sig_finished, this, [=](bool success){

		if(success){
			emit dataChanged(idx, idx);
		}

		acft->done(success);
		clu->deleteLater();
	});

	clu->fetch_cover(cl);
}


QModelIndex AlbumCoverModel::getFirstRowIndexOf(const QString& substr)
{
	return getNextRowIndexOf(substr, 0);
}

QModelIndex AlbumCoverModel::getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

	for(int i=0; i<_m->albums.size(); i++){
		int idx = (i + cur_row) % _m->albums.size();
		QString title = get_string(idx);
		title = Library::convert_search_string(title, search_mode());

		if(title.contains(substr))
		{
			return this->index(idx / columnCount(), idx % columnCount());
		}

		for(const QString& artist : _m->albums[idx].artists) {
			QString cvt_artist = Library::convert_search_string(artist, search_mode());

			if(cvt_artist.contains(substr)){
				return this->index(idx / columnCount(), idx % columnCount());
			}
		}
	}

	return QModelIndex();
}

QModelIndex AlbumCoverModel::getPrevRowIndexOf(const QString& substr, int row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

	int len = _m->albums.size();
	for(int i=0; i<_m->albums.size(); i++){

		if(row - i < 0){
			row = len - 1;
		}

		int idx = (row - i) % len;

		QString title = get_string(idx);
		title = Library::convert_search_string(title, search_mode());
		if(title.contains(substr))
		{
			return this->index(idx / columnCount(), idx % columnCount());
		}

		for(const QString& artist : _m->albums[idx].artists) {
			QString cvt_artist = Library::convert_search_string(artist, search_mode());

			if(cvt_artist.contains(substr)){
				return this->index(idx / columnCount(), idx % columnCount());
			}
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

QString AlbumCoverModel::get_string(int idx) const
{
	if(idx < 0 || idx >= _m->albums.size()){
		return QString();
	}

	return _m->albums[idx].name;
}

int AlbumCoverModel::get_id_by_row(int idx)
{
	if(idx < 0 || idx >= _m->albums.size()){
		return -1;
	}

	return _m->albums[idx].id;
}

CoverLocation AlbumCoverModel::get_cover(const SP::Set<int>& indexes) const
{
	if(indexes.size() != 1){
		return CoverLocation::getInvalidLocation();
	}

	int idx = indexes.first();
	if(idx < 0 || idx >= _m->albums.size() ){
		return CoverLocation::getInvalidLocation();
	}

	QString hash = get_hash( _m->albums[idx] );
	if(!_m->cover_locations.contains(hash)){
		return CoverLocation::getInvalidLocation();
	}

	return _m->cover_locations[hash];
}


