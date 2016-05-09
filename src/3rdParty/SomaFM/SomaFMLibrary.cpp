#include "SomaFMLibrary.h"
#include <QRegExp>

#include "Helper/Logger/Logger.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Parser/PlaylistParser.h"

SomaFMLibrary::SomaFMLibrary(QObject* parent) :
	QObject(parent)
{
}


void SomaFMLibrary::init_stations(){

	AsyncWebAccess* awa = new AsyncWebAccess(this);

	connect(awa, &AsyncWebAccess::sig_finished, this, &SomaFMLibrary::soma_website_fetched);

	awa->run("https://somafm.com/listen/");
}

void SomaFMLibrary::request_station(const QString& name)
{
	_requested_station = name;
	SomaFMStation station = _station_map[name];
	if(!station.is_valid()){
		return;
	}

	AsyncWebAccess* awa = new AsyncWebAccess(this);

	connect(awa, &AsyncWebAccess::sig_finished, this, &SomaFMLibrary::soma_playlist_content_fetched);

	awa->run(station.get_urls()[0]);
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
	Helper::File::write_file(data, filename);
	int n_tracks = PlaylistParser::parse_playlist(filename, v_md);
	SomaFMStation station = _station_map[_requested_station];

	for(MetaData& md : v_md){
		md.title = station.get_station_name();
		md.artist = "SomaFM";
		md.album = station.get_station_name();
		md.cover_download_url = station.get_cover_location().search_url;
	}

	if(n_tracks > 0){
		station.set_metadata(v_md);
		emit sig_station_loaded(station);
	}

}


SomaFMStation::SomaFMStation()
{
	_cover = CoverLocation::getInvalidLocation();
}

SomaFMStation::SomaFMStation(const QString& content) :
	SomaFMStation()
{
	_content = content;

	sp_log(Log::Debug) << "---------------------------------------------------------------";
	sp_log(Log::Debug) << "";
	sp_log(Log::Debug) << content;
	sp_log(Log::Debug) << "";
	sp_log(Log::Debug) << "---------------------------------------------------------------";

	parse_description();
	parse_station_name();
	parse_image();
	parse_urls();

	_cover.cover_path = Helper::get_sayonara_path() +
			"/covers/" +
			_station_name + "." + Helper::File::get_file_extension(_cover.search_url);
}

QString SomaFMStation::get_station_name() const
{
	return _station_name;
}

QStringList SomaFMStation::get_urls() const
{
	return _urls;
}

QString SomaFMStation::get_description() const
{
	return _description;
}

CoverLocation SomaFMStation::get_cover_location() const
{
	return _cover;
}

bool SomaFMStation::is_valid() const
{
	return (!_station_name.isEmpty() &&
			!_urls.isEmpty() &&
			!_description.isEmpty() &&
			_cover.valid);
}

MetaDataList SomaFMStation::get_metadata() const
{
	return _v_md;
}

void SomaFMStation::set_metadata(const MetaDataList& v_md)
{
	_v_md = v_md;
}


void SomaFMStation::parse_station_name()
{
	QString pattern("<h3>(.*)</h3>");
	QRegExp re(pattern);
	re.setMinimal(true);

	int idx = re.indexIn(_content);
	if(idx > 0){
		_station_name = re.cap(1);
	}
}

void SomaFMStation::parse_urls()
{
	QString pattern("<nobr>\\s*MP3:\\s*<a\\s+href=\"(.*)\"");
	QRegExp re(pattern);
	re.setMinimal(true);

	int idx = re.indexIn(_content);
	if(idx > 0){
		_urls << re.cap(1);
	}
}

void SomaFMStation::parse_description()
{
	QString pattern("<p\\s*class=\"descr\">(.*)</p>");
	QRegExp re(pattern);
	re.setMinimal(true);

	int idx = re.indexIn(_content);
	if(idx > 0){
		_description = re.cap(1);
	}
}

void SomaFMStation::parse_image()
{
	QString pattern("<img\\s*src=\\s*\"(.*)\"");
	QRegExp re(pattern);

	re.setMinimal(true);

	int idx = re.indexIn(_content);
	if(idx > 0){
		_cover.cover_path = "";
		_cover.search_url = QString("https://somafm.com/") + re.cap(1);
		_cover.valid = true;
	}
}
