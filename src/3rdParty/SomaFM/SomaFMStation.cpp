#include "SomaFMStation.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"

SomaFMStation::SomaFMStation()
{
	_cover = CoverLocation::getInvalidLocation();
	_loved = false;
}

SomaFMStation::SomaFMStation(const QString& content) :
	SomaFMStation()
{
	_content = content;

	parse_description();
	parse_station_name();
	parse_image();
	parse_urls();

	_cover.cover_path = Helper::get_sayonara_path() +
			"/covers/" +
			_station_name + "." + Helper::File::get_file_extension(_cover.search_url);
}

QString SomaFMStation::get_name() const
{
	return _station_name;
}

QStringList SomaFMStation::get_urls() const
{
	return _urls.keys();
}

SomaFMStation::UrlType SomaFMStation::get_url_type(const QString& url) const
{
	return _urls[url];
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
		_station_name = Helper::cvt_str_to_first_upper(re.cap(1));
	}
}

void SomaFMStation::parse_urls()
{
	QString mp3_pattern("<nobr>\\s*MP3:\\s*<a\\s+href=\"(.*)\"");
	QString aac_pattern("<nobr>\\s*AAC:\\s*<a\\s+href=\"(.*)\"");
	QRegExp re_mp3(mp3_pattern);
	QRegExp re_aac(aac_pattern);

	re_mp3.setMinimal(true);
	re_aac.setMinimal(true);

	int idx=-1;
	do{
		idx = re_mp3.indexIn(_content, idx+1);
		if(idx > 0){
			QString url = re_mp3.cap(1);
			_urls[url] = SomaFMStation::UrlType::MP3;
		}
	} while(idx > 0);

	idx=-1;
	do{
		idx = re_aac.indexIn(_content, idx+1);
		if(idx > 0){
			QString url = re_aac.cap(1);
			_urls[url] = SomaFMStation::UrlType::AAC;
		}

	} while(idx > 0);
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

void SomaFMStation::set_loved(bool loved){
	_loved = loved;
}

bool SomaFMStation::is_loved() const
{
	return _loved;
}
