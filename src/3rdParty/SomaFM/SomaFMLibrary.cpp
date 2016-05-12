#include "SomaFMLibrary.h"
#include <QRegExp>

#include "Helper/Logger/Logger.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Parser/PlaylistParser.h"

#include "Components/Playlist/PlaylistHandler.h"

SomaFMLibrary::SomaFMLibrary(QObject* parent) :
	QObject(parent)
{
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

	for(const QString& station_content : station_contents){
		SomaFMStation station(station_content);
		if(station.is_valid()){
			_station_map[station.get_station_name()] = station;
		}
	}

	emit sig_stations_loaded(_station_map.keys());

	awa->deleteLater();
}


void SomaFMLibrary::create_playlist_from_playlist(int idx)
{
	SomaFMStation station = _station_map[_requested_station];
	QStringList urls = station.get_urls();

	if( !between(idx, 0, urls.size())) {
		return;		
	}

	AsyncWebAccess* awa = new AsyncWebAccess(this);

	connect(awa, &AsyncWebAccess::sig_finished, this, &SomaFMLibrary::soma_playlist_content_fetched);

	awa->run(urls[idx]);
}

void SomaFMLibrary::soma_playlist_content_fetched(bool success)
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());

	if(!success){
		awa->deleteLater();
		return;
	}

	MetaDataList v_md;
	QByteArray data = awa->get_data();
	QString extension = Helper::File::get_file_extension(awa->get_url());
	QString filename = Helper::get_sayonara_path() + "/soma_playlist." + extension;

	success = Helper::File::write_file(data, filename);
	if(!success){
		awa->deleteLater();
		return;
	}

	SomaFMStation station = _station_map[_requested_station];
	QString station_name = station.get_station_name();
	QString cover_url = station.get_cover_location().search_url;
	PlaylistParser::parse_playlist(filename, v_md);

	for(MetaData& md : v_md){
		md.title = station_name;
		md.artist = "SomaFM";
		md.album = station_name;
		md.cover_download_url = cover_url;
	}

	station.set_metadata(v_md);

	_station_map[_requested_station] = station;

	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->create_playlist(v_md,
						 station.get_station_name(),
						 true,
						 Playlist::Type::Stream);

	QFile::remove(filename);
	awa->deleteLater();
}


