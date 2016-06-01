#ifndef SOMAFMLIBRARY_H
#define SOMAFMLIBRARY_H

#include <QObject>
#include <QStringList>
#include <QImage>
#include <QList>
#include <QMap>
#include <QSettings>

#include "SomaFMStation.h"

class SomaFMLibrary : public QObject
{

	Q_OBJECT

signals:
	void sig_stations_loaded(const QList<SomaFMStation>& stations);
	void sig_station_changed(const SomaFMStation& station);


public:
	SomaFMLibrary(QObject* parent=nullptr);

	SomaFMStation get_station(const QString& name);
	void create_playlist_from_playlist(int idx);
	void search_stations();
	void set_station_loved(const QString& station_name, bool loved);


private slots:
	void soma_website_fetched(bool success);
	void soma_playlist_content_fetched(bool success);


private:
	QMap<QString, SomaFMStation> 	_station_map;
	QString 						_requested_station;
	QSettings*						_qsettings=nullptr;
};



#endif // SOMAFMLIBRARY_H
