#include "SomaFMLibrary.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Logger/Logger.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/Parser/StreamParser.h"
#include "Components/Playlist/PlaylistHandler.h"

SomaFMLibrary::SomaFMLibrary(QObject* parent) :
	QObject(parent)
{
	QString path = Helper::File::clean_filename(Helper::get_sayonara_path() + "/somafm.ini");

	_qsettings = new QSettings(path, QSettings::IniFormat, this);

}


void SomaFMLibrary::search_stations()
{
	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished, this, &SomaFMLibrary::soma_website_fetched);

	awa->run("https://somafm.com/listen/");
}
	

SomaFMStation SomaFMLibrary::get_station(const QString& name)
{
	_requested_station = name;
	SomaFMStation station = _station_map[name];
	return station;
}


void SomaFMLibrary::soma_website_fetched(bool success)
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());

	if(!success){
		awa->deleteLater();
		return;
	}

	QString content = QString::fromUtf8(awa->get_data());
	QStringList station_contents = content.split("<li");
	QList<SomaFMStation> stations;

	for(const QString& station_content : station_contents){

		SomaFMStation station(station_content);
		if(!station.is_valid()){
			continue;
		}

		QString station_name = station.get_name();

		bool loved = _qsettings->value(station_name, false).toBool();
			
		station.set_loved( loved );

		_station_map[station_name] = station;
		stations << station;
	}

	emit sig_stations_loaded(stations);

	awa->deleteLater();
}


void SomaFMLibrary::create_playlist_from_playlist(int idx)
{
	SomaFMStation station = _station_map[_requested_station];
	QStringList urls = station.get_urls();

	if( !between(idx, 0, urls.size())) {
		return;		
	}

	QString url = urls[idx];
	StreamParser* stream_parser = new StreamParser(station.get_name(), this);
	connect(stream_parser, &StreamParser::sig_finished, this, &SomaFMLibrary::soma_playlist_content_fetched);

	stream_parser->parse_stream(url);
}


void SomaFMLibrary::soma_playlist_content_fetched(bool success)
{
	StreamParser* parser = static_cast<StreamParser*>(sender());

	if(!success){
		parser->deleteLater();
		return;
	}

	MetaDataList v_md = parser->get_metadata();

	SomaFMStation station = _station_map[_requested_station];
	QString cover_url = station.get_cover_location().search_url;

	for(MetaData& md : v_md){
		md.cover_download_url = cover_url;
	}

	station.set_metadata(v_md);

	_station_map[_requested_station] = station;

	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->create_playlist(v_md,
						 station.get_name(),
						 true,
						 Playlist::Type::Stream);

	parser->deleteLater();
}


void SomaFMLibrary::set_station_loved(const QString& station_name, bool loved){

	_station_map[station_name].set_loved(loved);
	_qsettings->setValue(station_name, loved);

	emit sig_station_changed(_station_map[station_name]);
}
