/* MetaDataInfo.cpp */

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

#include "MetaDataInfo.h"
#include "Helper/Helper.h"
#include "Helper/globals.h"
#include "Helper/FileHelper.h"
#include "Helper/Language.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Settings/Settings.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Genre.h"
#include "Components/Library/LibraryManager.h"
#include "Helper/Library/LibraryInfo.h"

#include "Components/Covers/CoverLocation.h"
#include "Database/DatabaseHandler.h"

#include <limits>

MetaDataInfo::MetaDataInfo(const MetaDataList& v_md) :
	QObject(nullptr),
	SayonaraClass()
{
	if(v_md.isEmpty()) {
		return;
	}

	uint64_t length = 0;
	uint64_t filesize = 0;
	uint16_t year_min = std::numeric_limits<uint16_t>::max();
	uint16_t year_max = 0;
	uint32_t bitrate_min = std::numeric_limits<uint32_t>::max();
	uint32_t bitrate_max = 0;
	uint16_t tracknum = 0;
	bool calc_track_num = (v_md.size() == 1);

	QStringList genres;
	QMap<QString, QStringList> values;

	for(const MetaData& md : v_md )
	{
		_artists.insert(md.artist());
		_albums.insert(md.album());
		_album_artists.insert(md.album_artist());

		_album_ids.insert(md.album_id);
		_artist_ids.insert(md.artist_id);
		_album_artist_ids.insert(md.album_artist_id());

		length += md.length_ms;
		filesize += md.filesize;

		if(calc_track_num){
			tracknum = md.track_num;
		}

		// bitrate
		if(md.bitrate != 0){
			bitrate_min = std::min(md.bitrate, bitrate_min);
		}

		bitrate_max = std::max(md.bitrate, bitrate_max);

		// year
		if(md.year != 0) {
			year_min = std::min(year_min, md.year);
		}

		year_max = std::max(year_max, md.year);

		// custom fields
        const CustomFieldList& custom_fields = md.get_custom_fields();

        for(const CustomField& field : custom_fields)
        {
			QString name = field.get_display_name();
			QString value = field.get_value();
			if(value.isEmpty()){
				continue;
			}

			if(!values[name].contains(value)){
				values[name] << value;
			}
		}

		// genre
		genres = md.genres_to_list();

		// paths
		if(!Util::File::is_www(md.filepath())){
			QString filename, dir;
			Util::File::split_filename(md.filepath(), dir, filename);
			if( !_paths.contains(dir)){
				_paths << dir;
			}
		}

		else{
			_paths << md.filepath();
		}
	}

	for(const QString& name : values.keys()){
		_additional_info[name] = values[name].join("<br />");
	}

	if(bitrate_max > 0){
		insert_interval(InfoStrings::Bitrate, bitrate_min / 1000, bitrate_max / 1000);
	}

	if(year_max > 0){
		insert_interval(InfoStrings::Year, year_min, year_max);
	}

	insert_number(InfoStrings::nTracks, v_md.size());
	insert_filesize(filesize);
	insert_playing_time(length);
	insert_genre(genres);

	set_header(v_md);
	set_subheader(tracknum);
	set_cover_location(v_md);
}

MetaDataInfo::~MetaDataInfo() {}

void MetaDataInfo::set_header() {}
void MetaDataInfo::set_header(const MetaDataList& lst)
{
	if(lst.size() == 1){
        const MetaData& md = lst[0];
		_header = md.title;
	}

	else{
		_header = Lang::get(Lang::VariousTracks);
	}
}


void MetaDataInfo::set_subheader() {}
void MetaDataInfo::set_subheader(uint16_t tracknum)
{
	_subheader = calc_artist_str();

	if(tracknum){
		_subheader += CAR_RET + calc_tracknum_str(tracknum) + " " +
				Lang::get(Lang::TrackOn) + " ";
	}

	else{
		_subheader += CAR_RET + Lang::get(Lang::On) + " ";
	}

	_subheader += calc_album_str();
}

void MetaDataInfo::set_cover_location() {}
void MetaDataInfo::set_cover_location(const MetaDataList& lst)
{
	if(lst.size() == 1){
        const MetaData& md = lst[0];
		_cover_location = CoverLocation::get_cover_location(md);
	}

	else if(_album_ids.size() == 1){
		Album album;

		album.id = _album_ids.first();
		album.set_name(_albums.first());
		album.set_artists(_artists.toList());
		album.set_db_id(lst[0].db_id());

		_cover_location = CoverLocation::get_cover_location(album);
	}

	else if(_albums.size() == 1 && _artists.size() == 1){
		QString album = _albums.first();
		QString artist = _artists.first();
		_cover_location = CoverLocation::get_cover_location(album, artist);
	}

	else if(_albums.size() == 1){
		QString album = _albums.first();
		_cover_location = CoverLocation::get_cover_location(album, _artists.toList());
	}

	else {
		_cover_location = CoverLocation::getInvalidLocation();
	}
}


QString MetaDataInfo::calc_artist_str() const
{
	QString str;

	if( _album_artists.size() == 1){
		str = _album_artists.first();
	}

	else if( _artists.size() == 1 ){
		str = _artists.first();
	}

	else{
		QString::number(_artists.size()) + " " + Lang::get(Lang::VariousArtists);
	}

	return str;
}


QString MetaDataInfo::calc_album_str()
{
	QString str;

	if( _albums.size() == 1){
		str = _albums.first();
	}

	else{
		QString::number(_artists.size()) + " " + Lang::get(Lang::VariousAlbums) ;
	}

	return str;
}

QString MetaDataInfo::calc_tracknum_str( uint16_t tracknum )
{
	QString str;
	switch (tracknum)
	{
		case 1:
			str = Lang::get(Lang::First);
			break;
		case 2:
			str = Lang::get(Lang::Second);
			break;
		case 3:
			str = Lang::get(Lang::Third);
			break;
		default:
			str = QString::number(tracknum) + Lang::get(Lang::Th);
		break;
	}

	return str;
}


void MetaDataInfo::insert_playing_time(uint64_t ms)
{
	QString str = Util::cvt_ms_to_string(ms);
	_info.insert(InfoStrings::PlayingTime, str);
}

void MetaDataInfo::insert_genre(const QStringList& lst)
{
	QString str = lst.join(", ");
	_info.insert(InfoStrings::Genre, str);
}

void MetaDataInfo::insert_filesize(uint64_t filesize)
{
	QString str = Util::File::calc_filesize_str(filesize);
	_info.insert(InfoStrings::Filesize, str);
}


QString MetaDataInfo::get_header() const
{
	return _header;
}

QString MetaDataInfo::get_subheader() const
{
	return _subheader;
}


QString MetaDataInfo::get_info_string(InfoStrings idx) const
{
	switch(idx)
	{
		case InfoStrings::nTracks:
			return QString("#") + Lang::get(Lang::Tracks) + ": ";
		case InfoStrings::nAlbums:
			return QString("#") + Lang::get(Lang::Albums) + ": ";
		case InfoStrings::nArtists:
			return QString("#") + Lang::get(Lang::Artists) + ": ";
		case InfoStrings::Filesize:
			return Lang::get(Lang::Filesize) + ": ";
		case InfoStrings::PlayingTime:
			return Lang::get(Lang::PlayingTime) + ": ";
		case InfoStrings::Year:
			return Lang::get(Lang::Year) + ": ";
		case InfoStrings::Sampler:
			return Lang::get(Lang::Sampler);
		case InfoStrings::Bitrate:
			return Lang::get(Lang::Bitrate) + ": ";
		case InfoStrings::Genre:
			return Lang::get(Lang::Genre) + ": ";
		default: break;
	}

	return "";
}


QString MetaDataInfo::get_info_as_string() const
{
	QString str;

	for( const InfoStrings& key : _info.keys() ){
		str += BOLD(get_info_string(key)) + _info.value(key) + CAR_RET;
	}

	return str;
}

QString MetaDataInfo::get_additional_info_as_string() const
{
	QString str;

	for(const QString& key : _additional_info.keys()){
		str += BOLD(key) + ": " + _additional_info[key] + CAR_RET;
	}

	return str;
}

QString MetaDataInfo::get_paths_as_string() const
{
	QString ret;
	QList<LibraryInfo> lib_infos = LibraryManager::instance()->all_libraries();
	QStringList lib_paths;
	for(const LibraryInfo& li : lib_infos){
		lib_paths << li.path();
	}

	std::sort(lib_paths.begin(), lib_paths.end(), [](const QString& lp1, const QString& lp2){
		return (lp1.length() > lp2.length());
	});


	bool dark = (_settings->get(Set::Player_Style) == 1);

	for(const QString& path : _paths){
		QString name = path;

		for(const QString& lp : lib_paths){
			if(name.contains(lp)){
				name.replace(lp, "...");
				break;
			}
		}

		QString link = Util::create_link(name, dark, path, false);
		ret += link + CAR_RET;
	}

	return ret;
}

CoverLocation MetaDataInfo::get_cover_location() const
{
	return _cover_location;
}


void MetaDataInfo::insert_interval(InfoStrings key, int min, int max)
{
	QString str;

	if(min == max){
		str = QString::number(min);
	}

	else {
		str = QString::number(min) + " - " + QString::number(max);
	}

	if(key == InfoStrings::Bitrate){
		str += " kBit/s";
	}

	_info.insert(key, str);
}


void MetaDataInfo::insert_number(InfoStrings key, int number)
{
	QString str = QString::number(number);
	_info.insert(key, str);
}
