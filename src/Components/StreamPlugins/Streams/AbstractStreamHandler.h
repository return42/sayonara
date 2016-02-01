/* AbstractStreamHandler.h */

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


#ifndef AbstractStreamHandler_H
#define AbstractStreamHandler_H

#include <QMap>
#include <QString>
#include "Helper/MetaData/MetaData.h"

typedef QMap<QString, QString> StreamMap;

class PlaylistHandler;
class AsyncWebAccess;
class DatabaseConnector;

class AbstractStreamHandler : public QObject
{
	Q_OBJECT
public:
	explicit AbstractStreamHandler(QObject *parent=nullptr);

signals:
	void sig_error();
	void sig_data_available();

public:
	bool parse_station(const QString& url, const QString& station_name);
	void clear();

	MetaDataList get_tracks(const QString& station_name);

	void save(const QString& station_name, const QString& url);

	virtual bool get_all_streams(StreamMap& streams)=0;
	virtual bool add_stream(const QString& station_name, const QString& url)=0;
	virtual bool delete_stream(const QString& station_name)=0;
	virtual bool update_url(const QString& station_name, const QString& url)=0;
	virtual bool rename_stream(const QString& station_name, const QString& url)=0;


protected:
	DatabaseConnector*				_db=nullptr;
	PlaylistHandler*				_playlist=nullptr;
	AsyncWebAccess*					_awa=nullptr;

	QMap<QString, MetaDataList>		_station_contents;
	QString							_url;
	QString							_station_name;
	bool							_blocked;



private slots:
	void awa_finished(bool success);

};

#endif // AbstractStreamHandler_H
