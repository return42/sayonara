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
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/FileHelper.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/WebAccess/IcyWebAccess.h"
#include "Helper/Parser/PlaylistParser.h"
#include "Helper/Parser/PodcastParser.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Language.h"

#include <QFile>
#include <QDir>
#include <QUrl>

struct StreamParser::Private
{
	// If an url leads me to some website content and I have to parse it
	// and this Url is found again during parsing, it cannot be a stream
	// and so, it cannot be a metadata object
	QStringList		forbidden_urls;
	QString			last_url;
	QString			station_name;
	QString			cover_url;
	MetaDataList	v_md;
	QStringList 	urls;
	AsyncWebAccess* active_awa=nullptr;
	IcyWebAccess*	active_icy=nullptr;
	const int		MaxSizeUrls=1000;
	bool			stopped;

	bool is_url_forbidden(const QUrl& url) const
	{
		for(const QString& forbidden_url_str : forbidden_urls)
		{
			QUrl forbidden_url(forbidden_url_str);
			QString forbidden_host = forbidden_url.host();
			if ((forbidden_host.compare(url.host(), Qt::CaseInsensitive) == 0) &&
			   (forbidden_url.port(80) == url.port(80)) &&
				(forbidden_url.path().compare(url.path()) == 0) &&
				(forbidden_url.fileName().compare(url.path()) == 0))
			{
				return true;
			}
		}

		return false;
	}
};

StreamParser::StreamParser(const QString& station_name, QObject* parent) : 
	QObject(parent)
{
	_m = Pimpl::make<Private>();
	_m->station_name = station_name;
	_m->stopped = false;
}

StreamParser::~StreamParser() {}

void StreamParser::parse_stream(const QString& url)
{
	parse_streams( {url} );
}

void StreamParser::parse_streams(const QStringList& urls)
{
	_m->stopped = false;
	_m->v_md.clear();
	_m->urls = urls;
	_m->urls.removeDuplicates();

	if(_m->urls.size() > _m->MaxSizeUrls){
		emit sig_too_many_urls_found(_m->urls.size(), _m->MaxSizeUrls);
	}

	else {
		parse_next_url();
	}
}


bool StreamParser::parse_next_url()
{
	if(_m->stopped){
		emit sig_stopped();
		return false;
	}

	if(_m->urls.isEmpty()) {
		sp_log(Log::Debug, this) << "No more urls to parse";
		emit sig_finished( _m->v_md.size() > 0);
		return false;
	}

	QString url = _m->urls.takeFirst();
	AsyncWebAccess* awa = new AsyncWebAccess(this);
	awa->set_behavior(AsyncWebAccess::Behavior::AsSayonara);
	connect(awa, &AsyncWebAccess::sig_finished, this, &StreamParser::awa_finished);
	awa->run(url, 5000);
	_m->active_awa = awa;

	return true;
}


void StreamParser::awa_finished()
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	AsyncWebAccess::Status status = awa->status();
	_m->last_url = awa->url();
	_m->active_awa = nullptr;

	if(_m->stopped){
		awa->deleteLater();
		emit sig_stopped();
		return;
	}

	switch(status)
	{
		case AsyncWebAccess::Status::GotData:
		{
			_m->forbidden_urls << _m->last_url;
			sp_log(Log::Warning, this) << "Got data. Try to parse content";

			QPair<MetaDataList, PlaylistFiles> result = parse_content(awa->data());

			_m->v_md << result.first;
			_m->urls << result.second;

			_m->v_md.remove_duplicates();
			_m->urls.removeDuplicates();

			for(MetaData& md : _m->v_md) {
				tag_metadata(md, _m->last_url, _m->cover_url);
			}
		} break;

		case AsyncWebAccess::Status::NoHttp:
		{
			sp_log(Log::Warning, this) << "No correct http was found. Maybe Icy?";
			IcyWebAccess* iwa = new IcyWebAccess(this);
			_m->active_icy = iwa;
			connect(iwa, &IcyWebAccess::sig_finished, this, &StreamParser::icy_finished);
			iwa->check(QUrl(_m->last_url));

			awa->deleteLater();
		} return;

		case AsyncWebAccess::Status::AudioStream:
		{
			sp_log(Log::Warning, this) << "Found audio stream";
			MetaData md;
			tag_metadata(md, _m->last_url, _m->cover_url);

			_m->v_md << md;
			_m->v_md.remove_duplicates();
		} break;

		default:
			sp_log(Log::Warning, this) << "Web Access finished: " << (int) status;
	}

	awa->deleteLater();

	if(_m->urls.size() > _m->MaxSizeUrls){
		emit sig_too_many_urls_found(_m->urls.size(), _m->MaxSizeUrls);
	}

	else {
		parse_next_url();
	}
}


void StreamParser::icy_finished()
{
	IcyWebAccess* iwa = static_cast<IcyWebAccess*>(sender());
	IcyWebAccess::Status status = iwa->status();
	_m->active_icy = nullptr;

	if(_m->stopped){
		iwa->deleteLater();
		emit sig_stopped();
		return;
	}

	if(status == IcyWebAccess::Status::Success) {
		sp_log(Log::Debug, this) << "Stream is icy stream";
		MetaData md;
		tag_metadata(md, _m->last_url, _m->cover_url);

		_m->v_md << md;
		_m->v_md.remove_duplicates();
	} else {
		sp_log(Log::Warning) << "Stream is no icy stream";
	}

	iwa->deleteLater();

	parse_next_url();
}


QPair<MetaDataList, PlaylistFiles>  StreamParser::parse_content(const QByteArray& data) const
{
	QPair<MetaDataList, PlaylistFiles> result;

	sp_log(Log::Debug, this) << QString::fromUtf8(data);

	/** 1. try if podcast file **/
	result.first = PodcastParser::parse_podcast_xml_file_content(data);

	/** 2. try if playlist file **/
	if(result.first.isEmpty()) {
		QString filename = write_playlist_file(data);
		result.first = PlaylistParser::parse_playlist(filename);
		QFile::remove(filename);
	}

	if(result.first.isEmpty()){
		result = parse_website(data);
	}

	return result;
}

QPair<MetaDataList, PlaylistFiles> StreamParser::parse_website(const QByteArray& arr) const
{
	MetaDataList v_md;
	QStringList playlist_files;

	QStringList valid_extensions;
	valid_extensions << Helper::get_soundfile_extensions(false);
	valid_extensions << Helper::get_playlistfile_extensions(false);

	QStringList found_strings;
	QString re_prefix = "(http[s]*://|\"/|'/)";
	QString re_path = "\\S+\\.(" + valid_extensions.join("|") + ")";
	QString re_string = "(" + re_prefix + re_path + ")";

	QRegExp reg_exp(re_string);
	QUrl parent_url(_m->last_url);

	QString website = QString::fromUtf8(arr);
	int idx = reg_exp.indexIn(website);
	while(idx >= 0)
	{
		QStringList found_urls = reg_exp.capturedTexts();
		for(QString str : found_urls) {
			QUrl found_url(str);
			if( (str.size() > 7) &&
				(!_m->is_url_forbidden(QUrl(found_url))) )
			{
				if(str.startsWith("\"") || str.startsWith("'")){
					str.remove(0, 1);
				}

				found_strings << str;
			}
		}

		idx = reg_exp.indexIn(website, idx + 1);
	}

	found_strings.removeDuplicates();

	for(const QString& found_str : found_strings) {
		QString child_url;

		if(!found_str.startsWith("http")) {
			child_url =	parent_url.scheme() + "://" + parent_url.host();
			if(!found_str.startsWith("/")) {
				child_url += "/";
			}
		}

		child_url += found_str;

		if(Helper::File::is_soundfile(found_str)){
			MetaData md;
			tag_metadata(md, child_url);
			v_md << md;
		}

		else if(Helper::File::is_playlistfile(found_str)){
			playlist_files << child_url;
		}
	}

	sp_log(Log::Debug, this) << "Found " << _m->urls.size() << " playlists and " << v_md.size() << " streams";

	return QPair<MetaDataList, PlaylistFiles>(v_md, playlist_files);
}

void StreamParser::tag_metadata(MetaData &md, const QString& stream_url, const QString& cover_url) const
{
	if(_m->station_name.isEmpty()) {
		md.album = stream_url;
		if(md.title.isEmpty()){
			md.title = Lang::get(Lang::Radio);
		}
	}

	else {
		md.album = _m->station_name;
		if(md.title.isEmpty()){
			md.title = _m->station_name;
		}
	}

	if(md.artist.isEmpty()) {
		md.artist = stream_url;
	}

	if(md.filepath().isEmpty()) {
		md.set_filepath(stream_url);
	}

	if(!cover_url.isEmpty()) {
		md.cover_download_url = cover_url;
	}
}


QString StreamParser::write_playlist_file(const QByteArray& data) const
{
	QString extension = Helper::File::get_file_extension(_m->last_url);
	QString filename = Helper::get_sayonara_path("tmp_playlist");

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

	for(MetaData& md : _m->v_md){
		md.cover_download_url = url;
	}
}

void StreamParser::stop()
{
	_m->stopped = true;

	if(_m->active_awa){
		AsyncWebAccess* awa = _m->active_awa;
		_m->active_awa = nullptr;
		awa->stop();
	}

	if(_m->active_icy){
		IcyWebAccess* icy = _m->active_icy;
		_m->active_icy = nullptr;
		icy->stop();

	}
}
