#include "AlbumCoverModel.h"
#include "Components/Covers/CoverLocation.h"
#include "Components/Covers/CoverLookup.h"
#include "Helper/MetaData/Album.h"
#include "Helper/Helper.h"

#include <QPixmap>
#include <QVector>


struct AlbumCoverModel::Private
{	
	bool blocked;
	AlbumList albums;
	QHash<QString, QPixmap> pixmaps;
	QHash<QString, int> hash_album_map;
	QMap<CoverLookup*, Album> clu_buffer;

	int size;
	int columns;
	int n_threads_running;

	Private()
	{
		blocked = false;
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
	_m = Pimpl::make<Private>();
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
				if(!_m->pixmaps.contains(hash)){

					p = QPixmap(CoverLocation().preferred_path());
					CoverLookup* clu = new CoverLookup(nullptr, 1);
					clu->set_identifier(hash);
					connect(clu, &CoverLookup::sig_cover_found, this, &AlbumCoverModel::cover_found);
					connect(clu, &CoverLookup::sig_finished, this, &AlbumCoverModel::clu_finished);

					if(_m->n_threads_running < 20){
						clu->fetch_album_cover(album);
						_m->n_threads_running++;
					}

					else{
						_m->clu_buffer.insert(clu, album);
					}
				}

				else{
					p = _m->pixmaps[hash];
				}

				return p.scaled(_m->size, _m->size, Qt::KeepAspectRatio);;
			}

		case Qt::SizeHintRole:
			return QSize(_m->size + 50, _m->size + 50);

		default:
			return QVariant();
	}

	return QVariant();
}


void AlbumCoverModel::set_data(const AlbumList& albums)
{
	beginRemoveRows(QModelIndex(), 0, rowCount());
	endRemoveRows();

	beginRemoveColumns(QModelIndex(), 0, columnCount());
	endRemoveColumns();

	_m->albums = albums;
	for(int i=0; i<albums.size(); i++){
		QString hash = get_hash(_m->albums[i]);
		_m->hash_album_map[hash] = i;
	}

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
	Q_UNUSED(parent)

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
	Q_UNUSED(parent)

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
	// TODO: Implement me. Maybe get id by index?
	Q_UNUSED(row)
	return -1;
}

CoverLocation AlbumCoverModel::get_cover(const SP::Set<int>& indexes) const
{
	// TODO: Implement me
	Q_UNUSED(indexes)
	return CoverLocation::getInvalidLocation();
}


void AlbumCoverModel::cover_found(const QString& filepath)
{
	CoverLookup* clu = dynamic_cast<CoverLookup*>(sender());
	QString hash = clu->identifier();
	int idx = _m->hash_album_map[hash];
	int row = idx / columnCount();
	int col = idx % columnCount();
	QModelIndex midx = index(row, col);

	QPixmap p(filepath);
	_m->pixmaps[hash] = p;

	emit dataChanged(midx, midx);
}

void AlbumCoverModel::clu_finished(bool b)
{
	CoverLookup* clu = dynamic_cast<CoverLookup*>(sender());
	while(_m->blocked){
		Helper::sleep_ms(10);
	}

	_m->blocked = true;
	_m->n_threads_running--;

	if(!_m->clu_buffer.isEmpty()){
		CoverLookup* clu_key = _m->clu_buffer.firstKey();
		Album album = _m->clu_buffer.take(clu_key);
		clu_key->fetch_album_cover(album);
		_m->n_threads_running++;
	}

	_m->blocked = false;

	clu->deleteLater();
}
