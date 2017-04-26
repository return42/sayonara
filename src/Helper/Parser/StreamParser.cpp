/* StreamParser.cpp */

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

#include "StreamParser.h"
#include "Helper/Helper.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/FileHelper.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Parser/PlaylistParser.h"
#include "Helper/Parser/PodcastParser.h"
#include "Helper/UrlHelper.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Language.h"
#include "Helper/WebAccess/IcyWebAccess.h"

#include <QFile>
#include <QDir>
#include <QUrl>

struct StreamParser::Private
{
	QStringList 	urls;
	QString			last_url;
	QString			station_name;
	QString			cover_url;
	MetaDataList	v_md;
};

StreamParser::StreamParser(const QString& station_name, QObject* parent) : 
	QObject(parent)
{
	_m = new StreamParser::Private();
	_m->station_name = station_name;
}

StreamParser::~StreamParser()
{
	delete _m; _m=nullptr;
}

void StreamParser::parse_streams(const QStringList& urls)
{
	_m->v_md.clear();
	_m->urls = urls;
	parse_next();
}

void StreamParser::parse_stream(const QString& url)
{
	parse_streams( {url} );
}

bool StreamParser::parse_next()
{
	if(_m->urls.isEmpty()){
		emit sig_finished( _m->v_md.size() > 0);
		return false;
	}

	QString url = _m->urls.takeFirst();
	AsyncWebAccess* awa = new AsyncWebAccess(this);
	awa->set_behavior(AsyncWebAccess::Behavior::AsSayonara);
	connect(awa, &AsyncWebAccess::sig_finished, this, &StreamParser::awa_finished);
	awa->run(url, 5000);

	return true;
}


void StreamParser::awa_finished()
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	AsyncWebAccess::Status status = awa->status();
	_m->last_url = awa->get_url();

	MetaDataList v_md;
	awa->deleteLater();

	// Maybe it's an Icy Stream?
	if( status == AsyncWebAccess::Status::Error) {
		IcyWebAccess* iwa = new IcyWebAccess(this);
		connect(iwa, &IcyWebAccess::sig_finished, this, &StreamParser::icy_finished);
		iwa->check(QUrl(_m->last_url));
		return;
	}

	else if( status == AsyncWebAccess::Status::Timeout ||
			status == AsyncWebAccess::Status::NoData)
	{
		// nothing
	}

	else if(status == AsyncWebAccess::Status::GotData)
	{
		QByteArray data = awa->get_data();

		v_md = parse_content(data);

		for(MetaData& md : v_md) {
			tag_metadata(md, _m->last_url);
			if(!_m->cover_url.isEmpty()) {
				md.cover_download_url = _m->cover_url;
			}
		}

		_m->v_md << v_md;
	}

	else if(status == AsyncWebAccess::Status::Stream)
	{
		MetaData md;
		tag_metadata(md, _m->last_url);
		if(!_m->cover_url.isEmpty()) {
			md.cover_download_url = _m->cover_url;
		}

		_m->v_md << md;
	}

	parse_next();
}


void StreamParser::icy_finished()
{
	IcyWebAccess* iwa = static_cast<IcyWebAccess*>(sender());
	IcyWebAccess::Status status = iwa->status();

	if(status == IcyWebAccess::Status::Success) {
		sp_log(Log::Debug) << "Stream is icy stream";
		MetaData md;
		tag_metadata(md, _m->last_url);
		if(!_m->cover_url.isEmpty()) {
			md.cover_download_url = _m->cover_url;
		}

		_m->v_md << md;
	} else {
		sp_log(Log::Warning) << "Stream is no icy stream";
	}

	iwa->deleteLater();

	parse_next();
}

MetaDataList StreamParser::parse_content(const QByteArray& data)
{
	MetaDataList v_md;

	/** 1. try if podcast file **/
	PodcastParser::parse_podcast_xml_file_content(data, v_md);

	/** 2. try if playlist file **/
	if(v_md.isEmpty()) {
		QString filename = write_playlist_file(data);
		PlaylistParser::parse_playlist(filename, v_md);
		QFile::remove(filename);
	}

	return v_md;
}

void StreamParser::tag_metadata(MetaData &md, const QString& stream_url) const
{
	if(_m->station_name.isEmpty()){
		md.album = stream_url;
		if(md.title.isEmpty()){
			md.title = Lang::get(Lang::Radio);
		}
	}

	else{
		md.album = _m->station_name;
		if(md.title.isEmpty()){
			md.title = _m->station_name;
		}
	}

	if(md.artist.isEmpty()){
		md.artist = stream_url;
	}

	if(md.filepath().isEmpty()){
		md.set_filepath(stream_url);
	}
}


QString StreamParser::write_playlist_file(const QByteArray& data) const
{
	QString filename, extension;

	extension = Helper::File::get_file_extension(_m->last_url);
	filename = Helper::get_sayonara_path("tmp_playlist");

	if(!extension.isEmpty()){
		filename += "." + extension;
	}

	Helper::File::write_file(data, filename);
	return filename;
}

MetaDataList StreamParser::get_metadata() const
{
	return _m->v_md;
}

void StreamParser::set_cover_url(const QString& url)
{
	_m->cover_url = url;

	if(!_m->v_md.isEmpty()){
	
		for(MetaData& md : _m->v_md){
			md.cover_download_url = url;
		}
	}
}
