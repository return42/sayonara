/* AlbumInfo.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
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
#include "Helper/MetaData/MetaDataInfo.h"

AlbumInfo::AlbumInfo(const MetaDataList* v_md) :
	MetaDataInfo(v_md){

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
			str_sampler = tr("yes");
			_info.insert(InfoStrings::Sampler, str_sampler);
		}
		if(_artists.size() == 1){
			str_sampler = tr("no");
			_info.insert(InfoStrings::Sampler, str_sampler);
		}

		success = _db->getAlbumByID(_album_ids[0], album);

		if(success){
			_additional_info.clear();
			// custom fields
			const QList<CustomField>& custom_fields = album.get_custom_fields();
			for(const CustomField& field : custom_fields){

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

AlbumInfo::~AlbumInfo(){}

void AlbumInfo::set_header(){
	_header = calc_album_str();
}

void AlbumInfo::set_subheader(){
	_subheader = tr("by") + " " + calc_artist_str();
}

void AlbumInfo::set_cover_location(){


	if(_album_ids.size() == 1){
		_cover_location = CoverLocation::get_cover_location(_album_ids[0], _db->get_id());
	}

	else if( _albums.size() == 1){
		_cover_location = CoverLocation::get_cover_location(_albums[0], _artists);
	}

	else{
		_cover_location = CoverLocation::getInvalidLocation();
	}
}

