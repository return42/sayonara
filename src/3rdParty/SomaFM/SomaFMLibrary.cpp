/* SomaFMLibrary.cpp */

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

/* SomaFMLibrary.cpp */

#include "SomaFMLibrary.h"
#include "SomaFMStation.h"

#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/Parser/StreamParser.h"
#include "Helper/globals.h"
#include "Helper/MetaData/MetaDataList.h"

#include "Components/Playlist/PlaylistHandler.h"
#include "Components/Covers/CoverLocation.h"

#include <QMap>
#include <QSettings>
#include <algorithm>

struct SomaFM::Library::Private
{
	QMap<QString, SomaFM::Station> 	station_map;
	QString 						requested_station;
	QSettings*						qsettings=nullptr;
};

SomaFM::Library::Library(QObject* parent) :
	QObject(parent)
{
	_m = Pimpl::make<Private>();
	QString path = Helper::sayonara_path("somafm.ini");

	_m->qsettings = new QSettings(path, QSettings::IniFormat, this);
}

SomaFM::Library::~Library()
{
	_m->qsettings->deleteLater();
}


void SomaFM::Library::search_stations()
{
	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished, this, &SomaFM::Library::soma_website_fetched);

	awa->run("https://somafm.com/listen/");
}
	

SomaFM::Station SomaFM::Library::station(const QString& name)
{
	_m->requested_station = name;
	return _m->station_map[name];
}


void SomaFM::Library::soma_website_fetched()
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	QList<SomaFM::Station> stations;

	if(awa->status() != AsyncWebAccess::Status::GotData){
		awa->deleteLater();
		emit sig_stations_loaded(stations);
		return;
	}

	QString content = QString::fromUtf8(awa->data());
	QStringList station_contents = content.split("<li");


	for(const QString& station_content : station_contents)
	{
		SomaFM::Station station(station_content);
		if(!station.is_valid()){
			continue;
		}

		QString station_name = station.name();

		bool loved = _m->qsettings->value(station_name, false).toBool();
			
		station.set_loved( loved );

		_m->station_map[station_name] = station;
		stations << station;
	}

	sort_stations(stations);
	emit sig_stations_loaded(stations);

	awa->deleteLater();
}

void SomaFM::Library::create_playlist_from_station(int row)
{
	Q_UNUSED(row)

	SomaFM::Station station = _m->station_map[_m->requested_station];
	StreamParser* parser = new StreamParser(station.name(), this);
	connect(parser, &StreamParser::sig_finished, this, &SomaFM::Library::soma_station_playlists_fetched);
	parser->parse_streams(station.urls());
}

void SomaFM::Library::soma_station_playlists_fetched(bool success)
{
	StreamParser* parser = dynamic_cast<StreamParser*>(sender());

	if(!success){
		parser->deleteLater();
		return;
	}

	MetaDataList v_md  = parser->get_metadata();
	SomaFM::Station station = _m->station_map[_m->requested_station];
	QString cover_url;
	CoverLocation cl = station.cover_location();
	if(cl.has_search_urls()){
		cover_url = cl.search_urls().first();
	}

	for(auto it = v_md.begin(); it != v_md.end(); it++){
		it->cover_download_url = cover_url;
	}

	station.set_metadata(v_md);

	_m->station_map[_m->requested_station] = station;

	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->create_playlist(v_md,
						 station.name(),
						 true,
						 Playlist::Type::Stream);

	parser->deleteLater();
}


void SomaFM::Library::create_playlist_from_playlist(int idx)
{
	SomaFM::Station station = _m->station_map[_m->requested_station];
	QStringList urls = station.urls();

	if( !between(idx, urls)) {
		return;		
	}

	QString url = urls[idx];
	StreamParser* stream_parser = new StreamParser(station.name(), this);
	connect(stream_parser, &StreamParser::sig_finished, this, &SomaFM::Library::soma_playlist_content_fetched);

	stream_parser->parse_stream(url);
}


void SomaFM::Library::soma_playlist_content_fetched(bool success)
{
	StreamParser* parser = static_cast<StreamParser*>(sender());

	if(!success){
		parser->deleteLater();
		return;
	}

	MetaDataList v_md = parser->get_metadata();

	SomaFM::Station station = _m->station_map[_m->requested_station];
	CoverLocation cl = station.cover_location();
	QString cover_url;
	if(cl.has_search_urls()){
		cover_url = cl.search_urls().first();
	}

	for(auto it = v_md.begin(); it != v_md.end(); it++){
		it->cover_download_url = cover_url;
	}

	station.set_metadata(v_md);

	_m->station_map[_m->requested_station] = station;

	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->create_playlist(v_md,
						 station.name(),
						 true,
						 Playlist::Type::Stream);

	parser->deleteLater();
}


void SomaFM::Library::set_station_loved(const QString& station_name, bool loved)
{
	_m->station_map[station_name].set_loved(loved);
	_m->qsettings->setValue(station_name, loved);

	QList<SomaFM::Station> stations;
	for(const QString& key : _m->station_map.keys()){
		if(key.isEmpty()){
			continue;
		}

		stations << _m->station_map[key];
	}

	sort_stations(stations);
	emit sig_stations_loaded(stations);
}


void SomaFM::Library::sort_stations(QList<SomaFM::Station>& stations)
{
	auto lambda = [](const SomaFM::Station& s1, const SomaFM::Station& s2){
		if(s1.is_loved() && !s2.is_loved()){
			return true;
		}

		else if(!s1.is_loved() && s2.is_loved()){
			return false;
		}

		return s1.name() < s2.name();
	};

	std::sort(stations.begin(), stations.end(), lambda);
}
