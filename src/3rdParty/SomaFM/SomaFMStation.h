#ifndef SOMAFMSTATION_H
#define SOMAFMSTATION_H

#include "Helper/MetaData/MetaDataList.h"
#include "Components/CoverLookup/CoverLocation.h"

class SomaFMStation
{
public:

	enum class UrlType : quint8
	{
		AAC=0,
		MP3,
		Undefined
	};

	SomaFMStation();
	SomaFMStation(const QString& content);

	QString get_station_name() const;
	QStringList get_urls() const;
	QString get_description() const;
	UrlType get_url_type(const QString& url) const;
	CoverLocation get_cover_location() const;
	bool is_valid() const;
	MetaDataList get_metadata() const;
	void set_metadata(const MetaDataList& v_md);

	void set_loved(bool loved);
	bool is_loved() const;


private:

	QString			_content;

	QString			_station_name;
	QMap<QString, UrlType> _urls;
	QString			_description;
	CoverLocation	_cover;
	MetaDataList	_v_md;
	bool			_loved;

private:
	void parse_station_name();
	void parse_urls();
	void parse_description();
	void parse_image();
};

#endif
