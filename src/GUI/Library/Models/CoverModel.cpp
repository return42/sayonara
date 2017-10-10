/* AlbumCoverModel.cpp */

/* Copyright (C) 2011-2017  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CoverModel.h"
#include "AlbumCoverFetchThread.h"

#include "Components/Library/AbstractLibrary.h"
#include "Components/Covers/CoverLocation.h"
#include "Components/Covers/CoverLookup.h"

#include "Utils/MetaData/Album.h"
#include "Utils/Utils.h"
#include "Utils/Logger/Logger.h"
#include "Utils/Set.h"

#include <QStringList>
#include <QPixmap>
#include <QThread>

using Cover::Location;
using Cover::Lookup;
using Library::CoverModel;

struct CoverModel::Private
{
	AlbumCoverFetchThread* cover_thread=nullptr;
	AlbumList albums;
	QHash<QString, QPixmap> pixmaps;
	QHash<QString, Location> cover_locations;
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
	return album.name() + "-" + album.id;
}

CoverModel::CoverModel(QObject* parent, AbstractLibrary* library) :
    ItemModel(parent, library)
{
	Q_UNUSED(parent);
	m = Pimpl::make<Private>(this);

	connect(m->cover_thread, &AlbumCoverFetchThread::sig_next, this, &CoverModel::next_hash);
}

CoverModel::~CoverModel()
{
	if(m->cover_thread){
		m->cover_thread->stop();
	}
}


int CoverModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return (m->albums.size() / columnCount()) + 1;
}

int CoverModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return m->columns;
}

void CoverModel::set_max_columns(int columns)
{
	if(columns == 0){
		return;
	}

	int old_columns = columnCount();
	int old_rows = rowCount();

	m->columns = columns;
	int rows = rowCount();

	if(old_columns == columns && old_rows == rows){
		return;
	}

	if(columns > old_columns) {
		beginInsertColumns(QModelIndex(), 0, columns - old_columns - 1);
		endInsertColumns();
	}

	else if(columns < old_columns) {
		beginRemoveColumns(QModelIndex(), 0, old_columns - columns - 1);
		endRemoveColumns();
	}

	if(rows > old_rows)	{
		beginInsertRows(QModelIndex(), old_rows, rows - 1);
		endInsertRows();
	}

	else if(rows < old_rows) {
		beginRemoveRows(QModelIndex(), rows, old_rows - 1);
		endRemoveRows();
	}

	emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1), {Qt::DisplayRole, Qt::SizeHintRole});
}

int CoverModel::zoom() const
{
    return m->size;
}


QVariant CoverModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid()) {
		return QVariant();
	}

	int row = index.row();
	int col = index.column();

	int n_columns = columnCount();
	int lin_idx = (row * n_columns) + col;
    if(lin_idx >= m->albums.count()){
		return QVariant();
	}

	switch(role)
	{
		case Qt::DisplayRole:
			return m->albums[lin_idx].name();

		case Qt::TextAlignmentRole:
			return Qt::AlignHCenter;

		case Qt::DecorationRole:
			{
				const Album& album = m->albums[lin_idx];
				QString hash = get_hash(album);
				QPixmap p;
				if(!m->pixmaps.contains(hash))
				{
					Location cl;
					if(!m->cover_locations.contains(hash)){
                        QString str = album.to_string();
						cl = Location::get_cover_location(album);
						m->cover_locations[hash] = cl;
					}

					else{
						cl = m->cover_locations[hash];
					}

					p = QPixmap(cl.preferred_path());
					if(!Location::isInvalidLocation(cl.preferred_path())){
						m->pixmaps[hash] = p.scaled(m->size, m->size, Qt::KeepAspectRatio);
					}

					else {
						if(!m->cover_thread->isRunning()){
							m->cover_thread->start();
						}

						m->indexes[hash] = index;
						/*sp_log(Log::Debug, this) << "";
						sp_log(Log::Debug, this) << " **** " << "Fetch cover for " << album.name;
						sp_log(Log::Debug, this) << "";*/
						m->cover_thread->add_data(hash, cl);
					}
				}

				else{
					p = m->pixmaps[hash];
					if(p.size().width() < m->size - 20)
					{
						m->pixmaps.remove(hash);
					}
				}

				return p.scaled(m->size, m->size, Qt::KeepAspectRatio);
			}

		case Qt::SizeHintRole:
			return item_size();

		default:
			return QVariant();
	}

	return QVariant();
}

QSize CoverModel::item_size() const
{
	return QSize(m->size + 50, m->size + 50);
}


void CoverModel::set_data(const AlbumList& albums)
{
	m->albums = albums;
	set_max_columns(m->columns);

	emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void CoverModel::set_zoom(int zoom, const QSize& view_size)
{
	m->size = zoom;

	int new_columns = (view_size.width() / this->item_size().width());
	set_max_columns(new_columns);
}


void CoverModel::next_hash()
{
	AlbumCoverFetchThread* acft = dynamic_cast<AlbumCoverFetchThread*>(sender());
	QString hash = acft->current_hash();
	Location cl = acft->current_cover_location();

	QModelIndex idx = m->indexes[hash];

	Lookup* clu = new Lookup(this, 1);
	connect(clu, &Lookup::sig_finished, [=](bool success){

		if(success){
			emit dataChanged(idx, idx);
		}

		acft->done(success);
		clu->deleteLater();
	});

	clu->fetch_cover(cl);
}

bool CoverModel::has_items() const
{
    return (m->albums.size() > 0);
}

QModelIndex CoverModel::getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

    for(int i=0; i<m->albums.count(); i++)
    {
		int idx = (i + cur_row) % m->albums.size();
		QString title = get_string(idx);
		title = Library::Util::convert_search_string(title, search_mode());

		if(title.contains(substr))
		{
			return this->index(idx / columnCount(), idx % columnCount());
		}

		for(const QString& artist : m->albums[idx].artists()) {
			QString cvt_artist = Library::Util::convert_search_string(artist, search_mode());

			if(cvt_artist.contains(substr)){
				return this->index(idx / columnCount(), idx % columnCount());
			}
		}
	}

	return QModelIndex();
}

QModelIndex CoverModel::getPrevRowIndexOf(const QString& substr, int row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

	int len = m->albums.size();
    for(int i=0; i<m->albums.count(); i++){

		if(row - i < 0){
			row = len - 1;
		}

		int idx = (row - i) % len;

		QString title = get_string(idx);
		title = Library::Util::convert_search_string(title, search_mode());
		if(title.contains(substr))
		{
			return this->index(idx / columnCount(), idx % columnCount());
		}

		for(const QString& artist : m->albums[idx].artists()) {
			QString cvt_artist = Library::Util::convert_search_string(artist, search_mode());

			if(cvt_artist.contains(substr)){
				return this->index(idx / columnCount(), idx % columnCount());
			}
		}
	}

	return QModelIndex();
}

QMap<QChar, QString> CoverModel::getExtraTriggers()
{
	return QMap<QChar, QString>();
}


int CoverModel::get_searchable_column() const
{
	return 0;
}

QString CoverModel::get_string(int idx) const
{
    if(idx < 0 || idx >= m->albums.count())
    {
		return QString();
	}

	return m->albums[idx].name();
}

int CoverModel::get_id_by_row(int idx)
{
    if(idx < 0 || idx >= m->albums.count())
    {
		return -1;
	}

	return m->albums[idx].id;
}

Location CoverModel::get_cover(const IndexSet& indexes) const
{
	if(indexes.size() != 1){
		return Location::getInvalidLocation();
	}

	int idx = indexes.first();
    if(idx < 0 || idx >= m->albums.count() ){
		return Location::getInvalidLocation();
	}

	QString hash = get_hash( m->albums[idx] );
	if(!m->cover_locations.contains(hash)){
		return Location::getInvalidLocation();
	}

	return m->cover_locations[hash];
}


const IndexSet& CoverModel::selections() const
{
    return library()->selected_albums();
}
