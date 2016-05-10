#ifndef SOMAFMLIBRARY_H
#define SOMAFMLIBRARY_H

#include <QObject>
#include <QStringList>
#include <QImage>
#include <QList>
#include <QMap>

#include "SomaFMStation.h"
#include "Helper/WebAccess/AsyncWebAccess.h"

class SomaFMLibrary : public QObject
{

	Q_OBJECT

signals:
	void sig_stations_loaded(const QStringList& station_names);


public:
	SomaFMLibrary(QObject* parent=nullptr);

	SomaFMStation get_station(const QString& name);
	void create_playlist_from_playlist(int idx);
	void search_stations();


private slots:
	void soma_website_fetched(bool success);
	void soma_playlist_content_fetched(bool success);


private:
	QMap<QString, SomaFMStation> 	_station_map;
	QString 						_requested_station;
};



#endif // SOMAFMLIBRARY_H
