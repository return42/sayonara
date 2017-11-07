/* PlaylistItemModel.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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


/*
* PlaylistItemModel.cpp
 *
 *  Created on: Apr 8, 2011
 *      Author: Lucio Carreras
 */

#include "Model.h"
#include "Components/Playlist/AbstractPlaylist.h"

#include "GUI/Utils/CustomMimeData.h"

#include "Utils/MetaData/MetaDataList.h"
#include "Utils/Library/SearchMode.h"
#include "Utils/FileUtils.h"
#include "Utils/Set.h"
#include "Utils/globals.h"
#include "Utils/Language.h"
#include "Utils/Logger/Logger.h"

#include <QUrl>
#include <QColor>

struct PlaylistItemModel::Private
{
	PlaylistPtr pl=nullptr;

	Private(PlaylistPtr pl) :
		pl(pl)
	{}
};

PlaylistItemModel::PlaylistItemModel(PlaylistPtr pl, QObject* parent) :
	AbstractSearchListModel(parent)
{
	m = Pimpl::make<Private>(pl);

	connect(m->pl.get(), &Playlist::Base::sig_data_changed, this, &PlaylistItemModel::playlist_changed);
}

PlaylistItemModel::~PlaylistItemModel() {}


int PlaylistItemModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m->pl->count();
}


QVariant PlaylistItemModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	if ( !between(index.row(), m->pl->count())) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		return MetaData::toVariant( m->pl->metadata(index.row()) );
	}

	return QVariant();
}


Qt::ItemFlags PlaylistItemModel::flags(const QModelIndex &index = QModelIndex()) const
{
	int row = index.row();
	if (!index.isValid()){
		return Qt::ItemIsEnabled;
	}

	if( row >= 0 && row < m->pl->count())
	{
		const MetaData& md = metadata(row);
		if(md.is_disabled){
			return Qt::NoItemFlags;
		}
	}

	return QAbstractItemModel::flags(index);
}


void PlaylistItemModel::clear()
{
	m->pl->clear();

	playlist_changed(0);
}


void PlaylistItemModel::remove_rows(const IndexSet& indexes)
{
	m->pl->delete_tracks(indexes);

	playlist_changed(0);
}


void PlaylistItemModel::move_rows(const IndexSet& indexes, int target_index)
{
	m->pl->move_tracks(indexes, target_index);

	playlist_changed(0);
}


IndexSet PlaylistItemModel::move_rows_up(const IndexSet& indexes)
{
	int min_row = *(std::min_element(indexes.begin(), indexes.end()));
	if(min_row <= 0){
		return IndexSet();
	}

	move_rows(indexes, min_row - 1);

	IndexSet new_indexes;
	for(int i=0; i<indexes.count(); i++)
	{
		new_indexes.insert(i + min_row - 1);
	}

	return new_indexes;
}


IndexSet PlaylistItemModel::move_rows_down(const IndexSet& indexes)
{
	auto min_max = std::minmax_element(indexes.begin(), indexes.end());
	int min_row = *(min_max.first);
	int max_row = *(min_max.second);

	if(max_row >= rowCount() - 1){
		return IndexSet();
	}

	IndexSet new_indexes;
	move_rows(indexes, max_row + 2);

	for(int i=0; i<indexes.count(); i++)
	{
		new_indexes.insert(i + min_row + 1);
	}

	return new_indexes;
}

void PlaylistItemModel::copy_rows(const IndexSet& indexes, int target_index)
{
	m->pl->copy_tracks(indexes, target_index);
	playlist_changed(0);
}


int PlaylistItemModel::current_track() const
{
	return m->pl->current_track_index();
}


void PlaylistItemModel::set_current_track(int row)
{
	m->pl->change_track(row);
}

const MetaData& PlaylistItemModel::metadata(int row) const
{
	return m->pl->metadata(row);
}


MetaDataList PlaylistItemModel::metadata(const IndexSet &rows) const
{
	MetaDataList v_md;
	v_md.reserve(rows.size());

	for(int row : rows)
	{
		v_md << m->pl->metadata(row);
	}

	return v_md;
}

const static QChar album_search_prefix('%');
const static QChar artist_search_prefix=('$');
const static QChar jump_prefix=(':');


bool PlaylistItemModel::has_items() const
{
	return (rowCount() > 0);
}

QModelIndex PlaylistItemModel::getPrevRowIndexOf(const QString& substr, int row, const QModelIndex &parent)
{
	Q_UNUSED(parent)

	QString converted_string = substr;

	int len = m->pl->count();
	if(len < row) row = len - 1;

	// ALBUM
	if(converted_string.startsWith(album_search_prefix))
	{
		converted_string.remove(album_search_prefix);
		converted_string = converted_string.trimmed();

		for(int i=0; i<len; i++)
		{
			if(row - i < 0) row = len - 1;
			int row_idx = (row - i) % len;

			QString album = m->pl->metadata(row_idx).album();
			album = Library::Util::convert_search_string(album, search_mode());

			if(album.contains(converted_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	//ARTIST
	else if(converted_string.startsWith(artist_search_prefix))
	{
		converted_string.remove(artist_search_prefix);
		converted_string = converted_string.trimmed();

		for(int i=0; i<len; i++)
		{
			if(row - i < 0) row = len - 1;
			int row_idx = (row - i) % len;

			QString artist = m->pl->metadata(row_idx).artist();
			artist = Library::Util::convert_search_string(artist, search_mode());

			if(artist.contains(converted_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	// JUMP
	else if(converted_string.startsWith(jump_prefix))
	{
		converted_string.remove(jump_prefix);
		converted_string = converted_string.trimmed();
		bool ok;
		int line = converted_string.toInt(&ok);
		if(ok && len > line) {
			return this->index(line, 0);
		}
	}

	// TITLE
	else
	{
		for(int i=0; i<len; i++)
		{
			if(row - i < 0) row = len - 1;
			int row_idx = (row - i) % len;
			QString title = m->pl->metadata(row_idx).title;
			title = Library::Util::convert_search_string(title, search_mode());

			if(title.contains(converted_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	return this->index(-1, -1);
}

QModelIndex PlaylistItemModel::getNextRowIndexOf(const QString& substr, int row, const QModelIndex &parent)
{
	Q_UNUSED(parent)

	QString converted_string = substr;

	int len = m->pl->count();
	if(len < row) {
		row = len - 1;
	}

	// ALBUM
	if(converted_string.startsWith(album_search_prefix))
	{
		converted_string.remove(album_search_prefix);
		converted_string = converted_string.trimmed();

		for(int i=0; i< len; i++)
		{
			int row_idx = (i + row) % len;

			QString album = m->pl->metadata(row_idx).album();
			album = Library::Util::convert_search_string(album, search_mode());

			if(album.contains(converted_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	//ARTIST
	else if(converted_string.startsWith(artist_search_prefix))
	{
		converted_string.remove(artist_search_prefix);
		converted_string = converted_string.trimmed();

		for(int i=0; i< len; i++)
		{
			int row_idx = (i + row) % len;

			QString artist = m->pl->metadata(row_idx).artist();

			artist = Library::Util::convert_search_string(artist, search_mode());

			if(artist.contains(converted_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	// JUMP
	else if(converted_string.startsWith(jump_prefix))
	{
		converted_string.remove(jump_prefix);
		converted_string = converted_string.trimmed();

		bool ok;
		int line = converted_string.toInt(&ok);
		if(ok && (m->pl->count() > line) ){
			return this->index(line, 0);
		}

		else return this->index(-1, -1);
	}

	// TITLE
	else
	{
		for(int i=0; i< len; i++)
		{
			int row_idx = (i + row) % len;

			QString title = m->pl->metadata(row_idx).title;
			title = Library::Util::convert_search_string(title, search_mode());

			if(title.contains(converted_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	return this->index(-1, -1);
}


QMap<QChar, QString> PlaylistItemModel::getExtraTriggers()
{
	QMap<QChar, QString> map;

	map.insert(artist_search_prefix, Lang::get(Lang::Artist));
	map.insert(album_search_prefix, Lang::get(Lang::Album));
	map.insert(jump_prefix, tr("Goto row"));

	return map;
}


CustomMimeData* PlaylistItemModel::custom_mimedata(const QModelIndexList& indexes) const
{
	CustomMimeData* mimedata = new CustomMimeData();
	QList<QUrl> urls;

	MetaDataList v_md;
	v_md.reserve(indexes.size());

	for(const QModelIndex& idx : indexes)
	{
		if(idx.row() >= m->pl->count()){
			continue;
		}

		v_md << m->pl->metadata(idx.row());
		QUrl url(QString("file://") + m->pl->metadata(idx.row()).filepath());
		urls << url;
	}

	if(v_md.empty()){
		return nullptr;
	}

	mimedata->set_inner_drag_drop();
	mimedata->set_metadata(v_md);
	mimedata->setText("tracks");
	mimedata->setUrls(urls);

	return mimedata;
}

QMimeData* PlaylistItemModel::mimeData(const QModelIndexList& indexes) const
{
	CustomMimeData* cmd =  custom_mimedata(indexes);
	return static_cast<QMimeData*> (cmd);
}

bool PlaylistItemModel::has_local_media(const IndexSet& rows) const
{
	const  MetaDataList& tracks = m->pl->playlist();

	for(int row : rows){
		if(!Util::File::is_www(tracks[row].filepath())){
			return true;
		}
	}

	return false;
}

void PlaylistItemModel::playlist_changed(int pl_idx)
{
	Q_UNUSED(pl_idx)
	emit dataChanged(this->index(0),
					 this->index(m->pl->count() - 1));
}

