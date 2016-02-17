/* ArtistInfo.cpp */

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


#include "ArtistInfo.h"
#include "Database/DatabaseHandler.h"

ArtistInfo::ArtistInfo(const MetaDataList* v_md) :
	MetaDataInfo(v_md){

	insert_number(InfoStrings::nAlbums, _albums.size());

	// clear, because it's from Metadata. We are not interested in these
	// rather fetch artists' additional data, if there's only one artist
	_additional_info.clear();

	if(_artist_ids.size() == 1){
		Artist artist;
		bool success;

		success = _db->getArtistByID(_artist_ids[0], artist);

		if(success){
			_additional_info.clear();
			// custom fields
			const QList<CustomField>& custom_fields = artist.get_custom_fields();
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

	else if(_artists.size() > 1){
		insert_number(InfoStrings::nArtists, _artists.size());
	}



	set_header();
	set_subheader();
	set_cover_location();
}

ArtistInfo::~ArtistInfo(){}

void ArtistInfo::set_header(){
	_header = calc_artist_str();

}

void ArtistInfo::set_subheader(){
	_subheader = "";
}

void ArtistInfo::set_cover_location(){

	if( _artists.size() == 1){
		QString artist = _artists[0];
		_cover_location = CoverLocation::get_cover_location(artist);
	}

	else{
		_cover_location = CoverLocation::getInvalidLocation();
	}
}


QString ArtistInfo::get_cover_album() const
{
	return "";
}
