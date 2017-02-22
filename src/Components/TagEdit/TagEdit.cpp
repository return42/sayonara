/* TagEdit.cpp */

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

#include "TagEdit.h"
#include "MetaDataChangeNotifier.h"
#include "Helper/Tagging/Tagging.h"
#include "Helper/MetaData/Genre.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/globals.h"
#include "Helper/Logger/Logger.h"
#include "Database/LibraryDatabase.h"
#include "Database/DatabaseConnector.h"

#include <QHash>
#include <algorithm>


struct TagEdit::Private
{
	MetaDataList			v_md;			// the current metadata
	MetaDataList			v_md_orig;		// the original metadata

	MetaDataList			v_md_before_change;
	MetaDataList			v_md_after_change;
	QList<bool>				changed_md;	// indicates if metadata at idx was changed
	QMap<int, QImage>		cover_map;

	QHash<QString, ArtistID> artist_map;
	QHash<QString, AlbumID> album_map;

	LibraryDatabase*		ldb=nullptr;	// database of LocalLibrary
	bool					notify;

	ArtistID get_artist_id(const QString& artist_name)
	{
		if(artist_map.contains(artist_name)){
			return artist_map[artist_name];
		} else {
			int id = ldb->getArtistID(artist_name);
			if(id < 0){
				id = ldb->insertArtistIntoDatabase(artist_name);
			}
			artist_map[artist_name] = id;
			return id;
		}
	}

	AlbumID get_album_id(const QString& album_name)
	{
		if(album_map.contains(album_name)){
			return album_map[album_name];
		} else {
			int id = ldb->getAlbumID(album_name);
			if(id < 0){
				id = ldb->insertAlbumIntoDatabase(album_name);
			}
			album_map[album_name] = id;
			return id;
		}
	}
};

TagEdit::TagEdit(QObject *parent) :
	QThread(parent)
{
	_m = Pimpl::make<TagEdit::Private>();
	_m->ldb = DB::getInstance()->get_std();
	_m->notify = true;

	connect(this, &QThread::finished, this, &TagEdit::thread_finished);
}

TagEdit::TagEdit(const MetaDataList& v_md, QObject* parent) :
	TagEdit(parent)
{
	set_metadata(v_md);
}

TagEdit::~TagEdit() {}


void TagEdit::update_track(int idx, const MetaData& md)
{
	_m->changed_md[idx] = !( md.is_equal_deep( _m->v_md_orig[idx]) );
	_m->v_md[idx] = md;
}

void TagEdit::undo(int idx)
{
	_m->v_md[idx] = _m->v_md_orig[idx];
}


void TagEdit::undo_all()
{
	_m->v_md = _m->v_md_orig;
}

const MetaData& TagEdit::get_metadata(int idx) const
{
	return _m->v_md[idx];
}

const MetaDataList& TagEdit::get_all_metadata() const
{
	return _m->v_md;
}


int TagEdit::get_n_tracks() const
{
	return _m->v_md.size();
}

void TagEdit::add_genre(int idx, const QString &genre)
{
	if(!between(idx, _m->v_md)){
		return;
	}

	MetaData& md = _m->v_md[idx];

	if(md.add_genre(Genre(genre))){
		_m->changed_md[idx] = true;
	}
}

void TagEdit::delete_genre(int idx, const QString& genre)
{
	if(!between(idx, _m->v_md)){
		return;
	}

	MetaData& md = _m->v_md[idx];
	if(md.remove_genre(Genre(genre))){
		_m->changed_md[idx] = true;
	}
}

void TagEdit::rename_genre(int idx, const QString& genre, const QString& new_name)
{
	if(!between(idx, _m->v_md)){
		return;
	}

	MetaData& md = _m->v_md[idx];
	if(md.remove_genre(Genre(genre))){
		_m->changed_md[idx] = true;
	}

	if(md.add_genre(Genre(new_name))){
		_m->changed_md[idx] = true;
	}
}


void TagEdit::set_metadata(const MetaDataList& v_md)
{
	_m->v_md = v_md;
	_m->v_md_orig = v_md;

	_m->cover_map.clear();
	_m->changed_md.clear();

	if(v_md.size() > 0){
		_m->ldb = DB::getInstance()->get(v_md.first());
	}

	for(int i=0; i<v_md.size(); i++){
		_m->changed_md << false;
	}

	emit sig_metadata_received(_m->v_md);
}

bool TagEdit::is_cover_supported(int idx) const
{
	return Tagging::is_cover_supported( _m->v_md[idx].filepath() );
}



void TagEdit::apply_artists_and_albums_to_md()
{
	for(int i=0; i<_m->v_md.size(); i++) {

		if( _m->changed_md[i] == false ) {
			continue;
		}

		MetaData& md = _m->v_md[i];

		ArtistID artist_id = _m->get_artist_id(md.artist);
		AlbumID album_id = _m->get_album_id(md.album);
		ArtistID album_artist_id = _m->get_artist_id(md.album_artist());

		md.album_id = album_id;
		md.artist_id = artist_id;
		md.set_album_artist_id(album_artist_id);
	}
}


void TagEdit::update_cover(int idx, const QImage& cover)
{
	if(cover.isNull()){
		return;
	}

	if(!between(idx, _m->v_md) ){
		return;
	}

	bool cover_supported = is_cover_supported(idx);
	if(!cover_supported) {
		return;
	}

	_m->cover_map[idx] = cover;
}


bool TagEdit::has_cover_replacement(int idx) const
{
	return _m->cover_map.contains(idx);
}

void TagEdit::run()
{
	MetaDataList v_md;
	MetaDataList v_md_orig;
	DatabaseConnector* db;

	sp_log(Log::Debug) << "Apply albums and artists";
	apply_artists_and_albums_to_md();

	sp_log(Log::Debug) << "Have to change" <<
						  std::count(_m->changed_md.begin(), _m->changed_md.end(), true)
					   << " tracks";

	int i=0;
	int n_operations = _m->v_md.size() + _m->cover_map.size();
	for(i=0; i<_m->v_md.size(); i++)
	{
		MetaData md = _m->v_md[i];
		emit sig_progress( (i * 100) / n_operations);

		if( _m->changed_md[i] == false ) {
			continue;
		}

		bool success = Tagging::setMetaDataOfFile(md);
		if( !success ) {
			continue;
		}

		if( !md.is_extern && md.id >= 0 ){
			success = _m->ldb->updateTrack(md);
		}

		if(success){
			v_md << std::move(md);
			v_md_orig.push_back(_m->v_md_orig[i]);
		}
	}

	for(int idx : _m->cover_map.keys()){
		
		Tagging::write_cover(_m->v_md[idx], _m->cover_map[idx]);
		emit sig_progress( (i++ * 100) / n_operations);
	}

	_m->ldb->createIndexes();

	db = DatabaseConnector::getInstance();
	db->clean_up();

	_m->v_md_after_change = v_md;
	_m->v_md_before_change = v_md_orig;
	_m->v_md_orig = _m->v_md;

	emit sig_progress(-1);
}

void TagEdit::thread_finished()
{
	if(_m->notify){
		MetaDataChangeNotifier::getInstance()->change_metadata(_m->v_md_before_change, _m->v_md_after_change);
	}
}

void TagEdit::commit()
{
	this->start();
}


