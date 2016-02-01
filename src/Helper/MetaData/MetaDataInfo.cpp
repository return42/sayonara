/* MetaDataInfo.cpp */

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



#include "Helper/MetaData/MetaDataInfo.h"
#include "Helper/FileHelper.h"


MetaDataInfo::MetaDataInfo(const MetaDataList* v_md) :
	QObject(nullptr),
	SayonaraClass()
{

	if(v_md->size() == 0 ) return;

	quint64 length = 0;
	quint64 filesize = 0;
	quint16 year_min = std::numeric_limits<quint16>::max();
	quint16 year_max = 0;
	quint32 bitrate_min = std::numeric_limits<quint32>::max();
	quint32 bitrate_max = 0;
	quint16 tracknum = 0;
	bool calc_track_num = (v_md->size() == 1);

	QStringList genres;


	_db = DB::getInstance(v_md->at(0));

	for(const MetaData& md : (*v_md) ){

		if(!_artists.contains(md.artist)){
			_artists << md.artist;
		}

		if(!_albums.contains(md.album)){
			_albums << md.album;
		}

		if(!_album_ids.contains(md.album_id)){
			_album_ids << md.album_id;
		}

		if(!_artist_ids.contains(md.artist_id)){
			_artist_ids << md.artist_id;
		}

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
		const QList<CustomField>& custom_fields = md.get_custom_fields();

		for(const CustomField& field : custom_fields){

			QString name = field.get_display_name();
			QString value = field.get_value();
			if(value.isEmpty()){
				continue;
			}

			_additional_info[name] += field.get_value();
		}

		// genre
		for(const QString& genre : md.genres){

			QString first_upper = Helper::cvt_str_to_first_upper(genre).trimmed();

			if(first_upper.size() == 0) continue;
			if(genres.contains( first_upper ) ) continue;

			genres.append(first_upper.trimmed());
		}

		// paths
		if(!FileHelper::is_www(md.filepath())){
			QString filename, dir;
			FileHelper::split_filename(md.filepath(), dir, filename);
			if( !_paths.contains(dir)){
				_paths << dir;
			}
		}

		else{
			_paths << md.filepath();
		}
	}


	if(bitrate_max > 0){
		insert_interval(InfoStrings::Bitrate, bitrate_min / 1000, bitrate_max / 1000);
	}

	if(year_max > 0){
		insert_interval(InfoStrings::Year, year_min, year_max);
	}

	insert_number(InfoStrings::nTracks, v_md->size());
	insert_filesize(filesize);
	insert_playing_time(length);
	insert_genre(genres);

	set_header(*v_md);
	set_subheader(tracknum);
	set_cover_location(*v_md);
}


MetaDataInfo::~MetaDataInfo(){

}


void MetaDataInfo::set_header(){}
void MetaDataInfo::set_header(const MetaDataList& lst){

	if(lst.size() == 1){
        const MetaData& md = lst[0];
		_header = md.title;
	}

	else{
		_header = tr("Various tracks");
	}
}


void MetaDataInfo::set_subheader(){}
void MetaDataInfo::set_subheader(quint16 tracknum){

	_subheader = calc_artist_str();

	if(tracknum){
		_subheader += CAR_RET + calc_tracknum_str(tracknum) + " " + tr("track on") + " ";
	}

	else{
		_subheader += CAR_RET + tr("on") + " ";
	}

	_subheader += calc_album_str();
}

void MetaDataInfo::set_cover_location(){}
void MetaDataInfo::set_cover_location(const MetaDataList& lst){

	if(lst.size() == 1){
        const MetaData& md = lst[0];
		_cover_location = CoverLocation::get_cover_location(md);
	}

	else if(_album_ids.size() == 1){
		_cover_location = CoverLocation::get_cover_location(_album_ids[0], _db->get_id());
	}

	else if(_albums.size() == 1 && _artists.size() == 1){
		QString album = _albums[0];
		QString artist = _artists[0];
		_cover_location = CoverLocation::get_cover_location(album, artist);
	}

	else if(_albums.size() == 1){
		QString album = _albums[0];
		_cover_location = CoverLocation::get_cover_location(album, _artists);
	}

	else {
		_cover_location = CoverLocation::getInvalidLocation();
	}
}


QString MetaDataInfo::calc_artist_str(){

	QString str;

	if( _artists.size() == 1 ){
		str = _artists[0];
	}

	else{
		str = tr("%1 various artists").arg(_artists.size());
	}

	return str;
}


QString MetaDataInfo::calc_album_str(){

	QString str;

	if( _albums.size() == 1){
		str = _albums[0];
	}

	else{
		str = tr("%1 various albums").arg(_albums.size());
	}

	return str;
}

QString MetaDataInfo::calc_tracknum_str( quint16 tracknum ){

	QString str;
	switch (tracknum) {
		case 1:
			str = tr("1st");

			break;
		case 2:
			str = tr("2nd");
			break;
		case 3:
			str = tr("3rd");
			break;
		default:
			str = QString::number(tracknum) + tr("th");
		break;
	}

	return str;
}


void MetaDataInfo::insert_playing_time(quint64 ms){
	QString str = Helper::cvt_ms_to_string(ms);
	_info.insert(InfoStrings::PlayingTime, str);
}

void MetaDataInfo::insert_genre(const QStringList& lst){
	QString str = lst.join(", ");
	_info.insert(InfoStrings::Genre, str);
}

void MetaDataInfo::insert_filesize(quint64 filesize){
	QString str = FileHelper::calc_filesize_str(filesize);
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
	switch(idx){

		case InfoStrings::nTracks:
			return tr("#Tracks") + ": ";
		case InfoStrings::nAlbums:
			return tr("#Albums") + ": ";
		case InfoStrings::nArtists:
			return tr("#Artists") + ": ";
		case InfoStrings::Filesize:
			return tr("Filesize") + ": ";
		case InfoStrings::PlayingTime:
			return tr("Playing time") + ": ";
		case InfoStrings::Year:
			return tr("Year") + ": ";
		case InfoStrings::Sampler:
			return tr("Sampler? ");
		case InfoStrings::Bitrate:
			return tr("Bitrate") + ": ";
		case InfoStrings::Genre:
			return tr("Genre") + ": ";
		default: break;

	}

	return "";
}

QMap<InfoStrings, QString> MetaDataInfo::get_info() const
{
	return _info;
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

QStringList MetaDataInfo::get_paths() const
{
	return _paths;
}

QString MetaDataInfo::get_paths_as_string() const
{

	QString str;
	QString lib_path = _settings->get(Set::Lib_Path);

	for(const QString& path : _paths){

		QString name = path;
		name.replace(lib_path, "...");

		QString link = Helper::create_link(name, path, false);
		str += link + CAR_RET;
	}

	return str;
}

CoverLocation MetaDataInfo::get_cover_location() const
{
	return _cover_location;
}

QString MetaDataInfo::get_cover_artist() const
{

	if(_artists.size() == 0) {
		return "";
	}

	else if(_artists.size() > 1) {
		return tr("Various artists");
	}

	return 	_artists[0];

}

QString MetaDataInfo::get_cover_album() const
{
	if(_albums.size() == 0) {
		return "";
	}

	return _albums[0];
}






