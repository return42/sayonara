/* AbstractStreamHandler.h */

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

#ifndef AbstractStreamHandler_H
#define AbstractStreamHandler_H

#include <QMap>
#include <QObject>

#include "Utils/Pimpl.h"

using StreamMap=QMap<QString, QString>;

/**
 * @brief Used to interprete website data as streams. Some methods have to be overridden,
 * to map their functions to their specific database functions.
 * The track list is held in a map, which is accessible through its station name. It can be
 * accessed via the get_tracks() method.
 * @ingroup Streams
 */
class AbstractStreamHandler : public QObject
{
	Q_OBJECT
	PIMPL(AbstractStreamHandler)

	public:
		explicit AbstractStreamHandler(QObject *parent=nullptr);
		virtual ~AbstractStreamHandler();

	signals:
		void sig_stopped();
		void sig_error();
		void sig_data_available();
		void sig_too_many_urls_found(int n_urls, int max_n_urls);

	public:
		/**
		 * @brief Retrieves data from the station and tries to interprete it via the parse_content() method.
		 * @param url url to retrieve the data from
		 * @param station_name the station name
		 * @return true, if no other station is parsed atm, false else
		 */
		bool parse_station(const QString& url, const QString& station_name);

		/**
		 * @brief Saves the station. Calls the add_stream() method.
		 * @param station_name The station name.
		 * @param url the station url.
		 */
		void save(const QString& station_name, const QString& url);

		/**
		 * @brief This method should return all stations in database
		 * @param streams target StreamMap
		 * @return true if successful, false else
		 */
		virtual bool get_all_streams(StreamMap& streams)=0;

		/**
		 * @brief This method should add a new station to database. If the station
		 * already exists, there should be a corresponding error handling.
		 * @param station_name station name
		 * @param url url
		 * @return true if successful, false else
		 */
		virtual bool add_stream(const QString& station_name, const QString& url)=0;

		/**
		 * @brief Delete a station from the database.
		 * @param station_name the station to be deleted
		 * @return true if successful, false else
		 */
		virtual bool delete_stream(const QString& station_name)=0;

		/**
		 * @brief Update the url of a station
		 * @param station_name the station to be updated
		 * @param url the new url
		 * @return true if successful, false else
		 */
		virtual bool update_url(const QString& station_name, const QString& url)=0;

		/**
		 * @brief Clears all station content
		 */
		void clear();

		void stop();

	private slots:
		void stream_parser_finished(bool success);
		void stopped();
};

#endif // AbstractStreamHandler_H
