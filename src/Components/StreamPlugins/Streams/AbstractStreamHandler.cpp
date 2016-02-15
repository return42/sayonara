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

AbstractStreamHandler::AbstractStreamHandler(QObject *parent) :
	QObject(parent)
{
	_blocked = false;
	_awa = new AsyncWebAccess(this);
	_db = DatabaseConnector::getInstance();
	_playlist = PlaylistHandler::getInstance();

	connect(_awa, &AsyncWebAccess::sig_finished, this, &AbstractStreamHandler::awa_finished);
}

void AbstractStreamHandler::clear(){
	_station_contents.clear();
}


bool AbstractStreamHandler::parse_station(const QString& url, const QString& station_name){

	if(_blocked) return false;

	_blocked = true;

	_url = url;
	_station_name = station_name;
	_awa->run(url, 2000);

	return true;
}


void AbstractStreamHandler::awa_finished(bool success){

	if(!success){
		emit sig_error();
		_blocked = false;
		return;
	}

	MetaDataList v_md;
	QByteArray content = _awa->get_data();

	bool check_md = false;
	QString url = _awa->get_url();

	if(!content.isEmpty()){
		PodcastParser::parse_podcast_xml_file_content(content, v_md);

		if(v_md.isEmpty()){

			QString extension = FileHelper::get_file_extension(_awa->get_url());

			QString filename = Helper::get_sayonara_path() + QDir::separator() + "tmp_playlist";
			if(!extension.isEmpty()){
				filename += "." + extension;
			}

			FileHelper::write_file(content, filename);

			PlaylistParser::parse_playlist(filename, v_md);

			QFile::remove(filename);
		}

		if(!v_md.isEmpty()){
			check_md = true;
		}

		else{
			QRegExp re("href=\"([^<]*\\.pls)\"");
			re.setMinimal(true);
			QString utf8_data = QString::fromUtf8(content);
			int idx = re.indexIn(utf8_data);
			if(idx > 0){
				QString playlist = re.cap(1);
				if(!playlist.startsWith("http")){
					playlist =  _awa->get_url_wo_file() + "/" + playlist;
				}

				sp_log(Log::Debug) << playlist;

				_blocked = false;
				parse_station(playlist, _station_name);
				return;
			}
		}
	}

	else {
		MetaData md;

		if(_station_name.isEmpty()){
			md.title = tr("Radio");
			md.album = url;
		}

		else{
			md.title = _station_name;
			md.album = _station_name;
		}

		md.artist = url;
		md.set_filepath(url);

		v_md << md;
	}

	if(check_md){

		for(MetaData& md : v_md) {

			if(_station_name.isEmpty()){
				md.album = _url;
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
				md.artist = _url;
			}
		}
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


