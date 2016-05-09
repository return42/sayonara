#ifndef SOMAFMLIBRARY_H
#define SOMAFMLIBRARY_H

#include <QObject>
#include <QStringList>
#include <QImage>
#include <QList>
#include <QMap>

#include "Helper/MetaData/MetaDataList.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Components/CoverLookup/CoverLocation.h"


class SomaFMStation
{
public:
	SomaFMStation();
	SomaFMStation(const QString& content);

	QString get_station_name() const;
	QStringList get_urls() const;
	QString get_description() const;
	CoverLocation get_cover_location() const;
	bool is_valid() const;
	MetaDataList get_metadata() const;
	void set_metadata(const MetaDataList& v_md);


private:

	QString			_content;

	QString			_station_name;
	QStringList		_urls;
	QString			_description;
	CoverLocation	_cover;
	MetaDataList	_v_md;

private:
	void parse_station_name();
	void parse_urls();
	void parse_description();
	void parse_image();
};




class SomaFMLibrary : public QObject
{

	Q_OBJECT

signals:
	void sig_stations_loaded(const QStringList& station_names);
	void sig_station_loaded(const SomaFMStation& station);


public:
	SomaFMLibrary(QObject* parent=nullptr);

	void init_stations();
	void request_station(const QString& name);


private slots:
	void soma_website_fetched(bool success);
	void soma_playlist_content_fetched(bool success);



private:
	QMap<QString, SomaFMStation> _station_map;
	QString _requested_station;


};



#endif // SOMAFMLIBRARY_H
