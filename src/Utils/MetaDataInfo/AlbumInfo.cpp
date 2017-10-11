/* AlbumInfo.cpp */

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

#include "AlbumInfo.h"
#include "MetaDataInfo.h"
#include "Utils/Language.h"
#include "Utils/MetaData/Album.h"
#include "Utils/MetaData/MetaData.h"
#include "Utils/MetaData/MetaDataList.h"
#include "Database/LibraryDatabase.h"
#include "Database/DatabaseConnector.h"


AlbumInfo::AlbumInfo(const MetaDataList& v_md) :
	MetaDataInfo(v_md)
{
	_db_id = v_md.first().db_id();
	QString str_sampler;

	// clear, because it's from Metadata. We are not interested in
	// rather fetch albums' additional data, if there's only one album
	_additional_info.clear();

	if(_albums.size() > 1){
		insert_number(InfoStrings::nAlbums, _albums.size());
	}

	if(_artists.size() > 1){
		insert_number(InfoStrings::nArtists, _artists.size());
	}

	if(_albums.size() == 1){
		Album album;
		bool success;

		if(_artists.size() > 1){
			str_sampler = Lang::get(Lang::Yes).toLower();
			_info.insert(InfoStrings::Sampler, str_sampler);
		}
		if(_artists.size() == 1){
			str_sampler = Lang::get(Lang::No).toLower();
			_info.insert(InfoStrings::Sampler, str_sampler);
		}

		int album_id = _album_ids.first();

		DatabaseConnector* db = DatabaseConnector::instance();
		LibraryDatabase* lib_db = db->library_db(-1, _db_id);
        if(lib_db) {
            // BIG TODO FOR SOUNDCLOUD
            success = lib_db->getAlbumByID(album_id, album);
        }

        else{
            success = false;
        }

		if(success){
			_additional_info.clear();
			// custom fields
            const CustomFieldList& custom_fields = album.get_custom_fields();
            for(const CustomField& field : custom_fields)
            {
				QString name = field.get_display_name();
				QString value = field.get_value();
				if(value.isEmpty()){
					continue;
				}

				_additional_info[name] += field.get_value();
			}
		}
	}

	set_header();
	set_subheader();
	set_cover_location();
}

AlbumInfo::~AlbumInfo() {}

void AlbumInfo::set_header()
{
	_header = calc_album_str();
}

void AlbumInfo::set_subheader()
{
	_subheader = Lang::get(Lang::By).toLower() + " " + calc_artist_str();
}

void AlbumInfo::set_cover_location()
{
	if(_album_ids.size() == 1) {

		DatabaseConnector* db = DatabaseConnector::instance();
		LibraryDatabase* lib_db = db->library_db(-1, _db_id);

		Album album;
		album.id = _album_ids.first();
		album.set_name(_albums.first());
		album.set_artists(_artists.toList());
		album.set_db_id(lib_db->db_id());

        _cover_location = Cover::Location::get_cover_location(album);
	}

	else if( _albums.size() == 1) {
		QString album = _albums.first();

		if(!_album_artists.isEmpty()) {
            _cover_location = Cover::Location::get_cover_location(album, _album_artists.toList());
		}

		else{
            _cover_location = Cover::Location::get_cover_location(album, _artists.toList());
		}
	}

	else{
        _cover_location = Cover::Location::getInvalidLocation();
	}
}

