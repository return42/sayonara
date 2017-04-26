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
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/Parser/StreamParser.h"
#include "Helper/MetaData/MetaDataList.h"

struct AbstractStreamHandler::Private
{
	PlaylistHandler*				playlist=nullptr;
	QMap<QString, MetaDataList>		station_contents;
	QString							station_name;
	bool							blocked;
};


AbstractStreamHandler::AbstractStreamHandler(QObject *parent) :
	QObject(parent)
{
	_m = Pimpl::make<AbstractStreamHandler::Private>();
	_m->playlist = PlaylistHandler::getInstance();
	_m->blocked = false;

	_db = DatabaseConnector::getInstance();
}

AbstractStreamHandler::~AbstractStreamHandler() {}

void AbstractStreamHandler::clear()
{
	_m->station_contents.clear();
}

bool AbstractStreamHandler::parse_station(const QString& url, const QString& station_name)
{
	if(_m->blocked) {
		return false;
	}

	_m->blocked = true;

	StreamParser* stream_parser = new StreamParser(station_name, this);
	connect(stream_parser, &StreamParser::sig_finished, this, &AbstractStreamHandler::stream_parser_finished);

	stream_parser->parse_stream(url);

	return true;
}

void AbstractStreamHandler::stream_parser_finished(bool success)
{
	if(!success){
		emit sig_error();
		_m->blocked = false;
		return;
	}

	StreamParser* stream_parser = static_cast<StreamParser*>(sender());
	MetaDataList v_md = stream_parser->get_metadata();

	_m->station_contents[_m->station_name] = v_md;

	emit sig_data_available();

	_m->playlist->create_playlist(v_md, _m->station_name, true, Playlist::Type::Stream);
	_m->blocked = false;
}

void AbstractStreamHandler::save(const QString& station_name, const QString& url)
{
	add_stream(station_name, url);
}

