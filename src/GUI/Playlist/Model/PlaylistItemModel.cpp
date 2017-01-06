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

#include "PlaylistItemModel.h"
#include "Components/Playlist/AbstractPlaylist.h"

#include "GUI/Helper/CustomMimeData.h"

#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Library/SearchMode.h"
#include "Helper/FileHelper.h"
#include "Helper/Set.h"
#include "Helper/globals.h"
#include "Helper/Language.h"


PlaylistItemModel::PlaylistItemModel(PlaylistPtr pl, QObject* parent) :
	AbstractSearchListModel(parent),
	_pl(pl)
{
	connect(_pl.get(), &AbstractPlaylist::sig_data_changed, this, &PlaylistItemModel::playlist_changed);
}

PlaylistItemModel::~PlaylistItemModel() {}

int PlaylistItemModel::rowCount(const QModelIndex &parent) const{
	Q_UNUSED(parent);
	return _pl->get_count();
}


QVariant PlaylistItemModel::data(const QModelIndex &index, int role) const{

	if (!index.isValid()) {
		return QVariant();
	}

	if ( !between(index.row(), _pl->get_count())) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		return MetaData::toVariant( _pl->at_const_ref(index.row()) );
	}

	else{
		return QVariant();
	}
}

const MetaData& PlaylistItemModel::get_md(int row) const
{
	return _pl->at_const_ref(row);
}


Qt::ItemFlags PlaylistItemModel::flags(const QModelIndex &index = QModelIndex()) const{

	int row = index.row();
	if (!index.isValid()){
		return Qt::ItemIsEnabled;
	}

	if( row >= 0 && row < _pl->get_count()){
		const MetaData& md = get_md(row);
		if(md.is_disabled){
			return Qt::NoItemFlags;
		}
	}

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool PlaylistItemModel::setData(const QModelIndex&index, const QVariant&var, int role)
{
	Q_UNUSED(var)
	Q_UNUSED(role)
	if(!index.isValid()){
		return false;
	}
	return true;
}

void PlaylistItemModel::clear()
{
	_pl->clear();
}


void PlaylistItemModel::remove_rows(const SP::Set<int>& indexes){

	_pl->delete_tracks(indexes);
}


void PlaylistItemModel::move_rows(const SP::Set<int>& indexes, int target_index){
	_pl->move_tracks(indexes, target_index);
	playlist_changed(0);
}

void PlaylistItemModel::copy_rows(const SP::Set<int>& indexes, int target_index){
	_pl->copy_tracks(indexes, target_index);
	playlist_changed(0);
}


int PlaylistItemModel::get_current_track() const {
	return _pl->get_cur_track_idx();
}


void PlaylistItemModel::set_current_track(int row)
{
	_pl->change_track(row);
}


void PlaylistItemModel::get_metadata(const IdxList& rows, MetaDataList& v_md) 
{
	v_md.clear();
	for(int row : rows){
		v_md << _pl->at_const_ref(row);
	}
}

#define ALBUM_SEARCH '%'
#define ARTIST_SEARCH '$'
#define JUMP ':'

QModelIndex PlaylistItemModel::getFirstRowIndexOf(const QString& substr) 
{
	if(_pl->is_empty()) {
		return this->index(-1, -1);
	}

	return getNextRowIndexOf(substr, 0);

}

QModelIndex PlaylistItemModel::getPrevRowIndexOf(const QString& substr, int row, const QModelIndex &parent) 
{
	Q_UNUSED(parent)

	QString converted_string = substr;

	int len = _pl->get_count();
	if(len < row) row = len - 1;

	// ALBUM
	if(converted_string.startsWith(ALBUM_SEARCH)) 
	{
		converted_string.remove(ALBUM_SEARCH).trimmed();

		for(int i=0; i<len; i++) 
		{
			if(row - i < 0) row = len - 1;
			int row_idx = (row - i) % len;

			QString album = _pl->at_const_ref(row_idx).album;
			album = Library::convert_search_string(album, search_mode());

			if(album.contains(converted_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	//ARTIST
	else if(converted_string.startsWith(ARTIST_SEARCH)) 
	{
		converted_string.remove(ARTIST_SEARCH).trimmed();

		for(int i=0; i<len; i++) 
		{
			if(row - i < 0) row = len - 1;
			int row_idx = (row - i) % len;

			QString artist = _pl->at_const_ref(row_idx).artist;
			artist = Library::convert_search_string(artist, search_mode());

			if(artist.contains(converted_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	// JUMP
	else if(converted_string.startsWith(JUMP)) 
	{
		converted_string.remove(JUMP).trimmed();
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
			QString title = _pl->at_const_ref(row_idx).title;
			title = Library::convert_search_string(title, search_mode());

			if(title.contains(converted_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	return this->index(-1, -1);
}

QModelIndex PlaylistItemModel::getNextRowIndexOf(const QString& substr, int row, const QModelIndex &parent) {

	Q_UNUSED(parent)

	QString converted_string = substr;

	int len = _pl->get_count();
	if(len < row) row = len - 1;

	// ALBUM
	if(converted_string.startsWith(ALBUM_SEARCH)) {
		converted_string.remove(ALBUM_SEARCH).trimmed();

		for(int i=0; i< len; i++) {
			int row_idx = (i + row) % len;

			QString album = _pl->at_const_ref(row_idx).album;
			album = Library::convert_search_string(album, search_mode());

			if(album.contains(converted_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	//ARTIST
	else if(converted_string.startsWith(ARTIST_SEARCH)) 
	{
		converted_string.remove(ARTIST_SEARCH).trimmed();

		for(int i=0; i< len; i++) {
			int row_idx = (i + row) % len;

			QString artist = _pl->at_const_ref(row_idx).artist;
			artist = Library::convert_search_string(artist, search_mode());

			if(artist.contains(converted_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	// JUMP
	else if(converted_string.startsWith(JUMP)) 
	{
		converted_string.remove(JUMP).trimmed();

		bool ok;
		int line = converted_string.toInt(&ok);
		if(ok && (_pl->get_count() > line) ){
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

			QString title = _pl->at_const_ref(row_idx).title;
			title = Library::convert_search_string(title, search_mode());

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
	map.insert(ARTIST_SEARCH, Lang::get(Lang::Artist));
	map.insert(ALBUM_SEARCH, Lang::get(Lang::Album));
	map.insert(JUMP, tr("Goto row"));
	return map;
}


CustomMimeData* PlaylistItemModel::get_custom_mimedata(const QModelIndexList& indexes) const {

	CustomMimeData* mimedata = new CustomMimeData();
	MetaDataList v_md;
	QList<QUrl> urls;

	for(const QModelIndex& idx : indexes){
		if(idx.row() >= _pl->get_count()){
			continue;
		}

		v_md << _pl->at_const_ref(idx.row());
		QUrl url(QString("file://") + _pl->at_const_ref(idx.row()).filepath());
		urls << url;
	}

	if(v_md.isEmpty()){
		return nullptr;
	}

	mimedata->setMetaData(v_md);
	mimedata->setText("tracks");
	mimedata->setUrls(urls);

	return mimedata;
}

QMimeData* PlaylistItemModel::mimeData(const QModelIndexList& indexes) const {
	CustomMimeData* cmd =  get_custom_mimedata(indexes);
	return static_cast<QMimeData*> (cmd);
}

bool PlaylistItemModel::has_local_media(const IdxList& idxs) const
{
	const  MetaDataList& tracks = _pl->get_playlist();

	for(int idx : idxs){
		if(!Helper::File::is_www(tracks[idx].filepath())){
			return true;
		}
	}

	return false;
}


void PlaylistItemModel::playlist_changed(int pl_idx)
{
	Q_UNUSED(pl_idx)
	emit dataChanged(this->index(0),
					 this->index(_pl->get_count() - 1));
}

