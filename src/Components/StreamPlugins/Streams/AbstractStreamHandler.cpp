/* AbstractStreamHandler.cpp */

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



#include "AbstractStreamHandler.h"

#include "Components/Playlist/PlaylistHandler.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Database/DatabaseConnector.h"

#include "Helper/Parser/PodcastParser.h"
#include "Helper/Parser/PlaylistParser.h"
#include "Helper/UrlHelper.h"

AbstractStreamHandler::AbstractStreamHandler(QObject *parent) :
	QObject(parent)
{
	_blocked = false;
	_db = DatabaseConnector::getInstance();
	_playlist = PlaylistHandler::getInstance();
}

void AbstractStreamHandler::clear(){
	_station_contents.clear();
}


bool AbstractStreamHandler::parse_station(const QString& url, const QString& station_name){

	if(_blocked) {
		return false;
	}

	sp_log(Log::Debug) << "Parse station: " << url;

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished, this, &AbstractStreamHandler::awa_finished);

	_blocked = true;

	_url = url;
	_station_name = station_name;

	awa->run(url, 2000);

	return true;
}


void AbstractStreamHandler::awa_finished(bool success){

	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());

	if(!success){
		sp_log(Log::Debug) << "Stream error: "<< awa->get_url();

		_blocked = false;
		awa->deleteLater();

		if(!_stream_buffer.isEmpty()){
			QString new_station = _stream_buffer.takeFirst();
			sp_log(Log::Debug) << "Try out another one: "<< new_station;
			parse_station(new_station, _station_name);
		}

		else {
			emit sig_error();
		}

		awa->deleteLater();
		return;
	}

	_stream_buffer.clear();

	MetaDataList v_md;

	QString url = awa->get_url();
	QByteArray data = awa->get_data();

	_url = url;

	awa->deleteLater();

	if(!data.isEmpty()){

		v_md = parse_content(data);
		if(v_md.isEmpty()){
			_blocked = false;
			return;
		}
	}

	else {
		MetaData md;
		v_md << md;
	}

	for(MetaData& md : v_md){
		finalize_metadata(md, url);
	}

	_station_contents[_station_name] = v_md;

	emit sig_data_available();

	_playlist->create_playlist(v_md, _station_name, true, Playlist::Type::Stream);

	_blocked = false;

}


MetaDataList AbstractStreamHandler::get_tracks(const QString& station_name){
	return _station_contents[station_name];
}


void AbstractStreamHandler::save(const QString& station_name, const QString& url)
{
	add_stream(station_name, url);
}

MetaDataList AbstractStreamHandler::parse_content(const QByteArray& data){

	MetaDataList v_md;

	/** 1. try if podcast file **/
	PodcastParser::parse_podcast_xml_file_content(data, v_md);

	/** 2. try if playlist file **/
	if(v_md.isEmpty()){
		QString filename = write_playlist_file(data);
		PlaylistParser::parse_playlist(filename, v_md);
		QFile::remove(filename);
	}

	/** 3. search for a playlist file on website **/
	if(v_md.isEmpty()){

		_stream_buffer = search_for_playlist_files(data);
		if(_stream_buffer.isEmpty()){
			return MetaDataList();
		}

		_blocked = false;

		QString playlist_file = _stream_buffer.takeFirst();
		sp_log(Log::Debug) << "try out " << playlist_file;
		parse_station(playlist_file, _station_name);

		return MetaDataList();
	}

	return v_md;
}

void AbstractStreamHandler::finalize_metadata(MetaData &md, const QString& stream_url){

	if(_station_name.isEmpty()){
		md.album = stream_url;
		if(md.title.isEmpty()){
			md.title = tr("Radio");
		}
	}

	else{
		md.album = _station_name;
		if(md.title.isEmpty()){
			md.title = _station_name;
		}
	}

	if(md.artist.isEmpty()){
		md.artist = stream_url;
	}

	if(md.filepath().isEmpty()){
		md.set_filepath(stream_url);
	}
}


QString AbstractStreamHandler::write_playlist_file(const QByteArray& data)
{
	QString filename, extension;

	extension = Helper::File::get_file_extension(_url);
	filename = Helper::get_sayonara_path() + QDir::separator() + "tmp_playlist";

	if(!extension.isEmpty()){
		filename += "." + extension;
	}

	Helper::File::write_file(data, filename);
	return filename;
}


QStringList AbstractStreamHandler::search_for_playlist_files(const QByteArray& data){

	QStringList playlist_strings;
	QString base_url = Helper::Url::get_base_url(_url);

	QRegExp re("href=\"([^<]+\\.(pls|m3u|asx))\"");
	re.setMinimal(true);

	QString utf8_data = QString::fromUtf8(data);
	int idx = re.indexIn(utf8_data);

	while(idx > 0){

		QString playlist = re.cap(1);
		if(!playlist.startsWith("http")){
			playlist = base_url + "/" + playlist;
		}

		sp_log(Log::Debug) << "Found a playlist on website: " << playlist;
		playlist_strings << playlist;

		idx = re.indexIn(utf8_data, idx+1);
	}

	sp_log(Log::Debug) << "Found " << playlist_strings.size() << " playlists on website: ";
	return playlist_strings;
}
