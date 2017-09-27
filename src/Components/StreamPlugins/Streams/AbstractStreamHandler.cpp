/* AbstractStreamHandler.cpp */

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

#include "AbstractStreamHandler.h"

#include "Database/DatabaseConnector.h"
#include "Components/Playlist/PlaylistHandler.h"
#include "Components/PlayManager/PlayManager.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/Parser/StreamParser.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Message/GlobalMessage.h"

struct AbstractStreamHandler::Private
{
	StreamParser*					stream_parser=nullptr;
	PlaylistHandler*				playlist=nullptr;
	QMap<QString, MetaDataList>		station_contents;
	QString							station_name;
	bool							blocked;

	Private()
	{
		playlist = PlaylistHandler::getInstance();
		blocked = false;
	}
};


AbstractStreamHandler::AbstractStreamHandler(QObject *parent) :
	QObject(parent)
{
	m = Pimpl::make<AbstractStreamHandler::Private>();
	_db = DatabaseConnector::getInstance();
}

AbstractStreamHandler::~AbstractStreamHandler() {}

void AbstractStreamHandler::clear()
{
	m->station_contents.clear();
}

void AbstractStreamHandler::stop()
{
	if(m->stream_parser && m->blocked) {
		m->stream_parser->stop();
	}
}

void AbstractStreamHandler::stopped()
{
	m->blocked = false;
	emit sig_stopped();

	sender()->deleteLater();
	m->stream_parser = nullptr;
}


bool AbstractStreamHandler::parse_station(const QString& url, const QString& station_name)
{
	if(m->blocked) {
		return false;
	}

	m->blocked = true;

	m->stream_parser = new StreamParser(station_name, this);
	connect(m->stream_parser, &StreamParser::sig_finished, this, &AbstractStreamHandler::stream_parser_finished);
	connect(m->stream_parser, &StreamParser::sig_too_many_urls_found, this, &AbstractStreamHandler::sig_too_many_urls_found);
	connect(m->stream_parser, &StreamParser::sig_stopped, this, &AbstractStreamHandler::stopped);

	m->stream_parser->parse_stream(url);

	return true;
}


void AbstractStreamHandler::stream_parser_finished(bool success)
{
	StreamParser* stream_parser = static_cast<StreamParser*>(sender());

	if(!success) {
		sp_log(Log::Debug, this) << "Stream parser finished with error";
		stream_parser->deleteLater(); m->stream_parser = nullptr;

		m->blocked = false;
		emit sig_error();
	}

	else {

		MetaDataList v_md = stream_parser->get_metadata();
		m->station_contents[m->station_name] = v_md;

		if(!v_md.isEmpty()){
			int idx = m->playlist->create_playlist(v_md, m->station_name, true, Playlist::Type::Stream);
			m->playlist->change_track(0, idx);
		}

		m->blocked = false;
		emit sig_data_available();
	}

	stream_parser->deleteLater(); m->stream_parser = nullptr;
}


void AbstractStreamHandler::save(const QString& station_name, const QString& url)
{
	add_stream(station_name, url);
}

