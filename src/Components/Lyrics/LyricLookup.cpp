/* LyricLookupThread.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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

/*
 * LyricLookupThread.cpp
 *
 *  Created on: May 21, 2011
 *      Author: Lucio Carreras
 */

#include "LyricLookup.h"
#include "LyricServer.h"

#include "Helper/WebAccess/AsyncWebAccess.h"

#include <QStringList>
#include <QRegExp>
#include <QMap>

#include <algorithm>


struct LyricLookupThread::Private
{
	QString					artist;
	QString					title;
	int						cur_server;
	QList<ServerTemplate>	server_list;
	QString					final_wp;
	QMap<QString, QString>  regex_conversions;
};



LyricLookupThread::LyricLookupThread(QObject* parent) :
	QObject(parent)
{
	_m = Pimpl::make<LyricLookupThread::Private>();

	init_server_list();

	_m->cur_server = 0;
	_m->final_wp.clear();

	_m->regex_conversions.insert("$", "\\$");
	_m->regex_conversions.insert("*", "\\*");
	_m->regex_conversions.insert("+", "\\+");
	_m->regex_conversions.insert("?", "\\?");
	_m->regex_conversions.insert("[", "\\[");
	_m->regex_conversions.insert("]", "\\]");
	_m->regex_conversions.insert("(", "\\(");
	_m->regex_conversions.insert(")", "\\)");
	_m->regex_conversions.insert("{", "\\{");
	_m->regex_conversions.insert("}", "\\}");
	_m->regex_conversions.insert("^", "\\^");
	_m->regex_conversions.insert("|", "\\|");
	_m->regex_conversions.insert(".", "\\.");
}

LyricLookupThread::~LyricLookupThread() {}

QString LyricLookupThread::convert_to_regex(const QString& str) const
{
	QString ret = str;

	for(QString key : _m->regex_conversions.keys()){
		ret.replace(key, _m->regex_conversions.value(key));
	}

	ret.replace(" ", "\\s+");

	return ret;
}

QString LyricLookupThread::calc_server_url(QString artist, QString song)
{
	if(_m->cur_server < 0 || _m->cur_server >= _m->server_list.size()){
		return "";
	}

	QMap<QString, QString> replacements = _m->server_list[_m->cur_server].replacements;

	for(QString key : replacements.keys()) {
		while(artist.indexOf(key) >= 0){
			artist.replace(key, replacements.value(key));
		}

		while(song.indexOf(key) >= 0){
			song.replace(key, replacements.value(key));
		}
	}

	QString url = _m->server_list[_m->cur_server].call_policy;
	url.replace("<SERVER>", _m->server_list[_m->cur_server].server_address);
	url.replace("<FIRST_ARTIST_LETTER>", QString(artist[0]).trimmed());
	url.replace("<ARTIST>", artist.trimmed());
	url.replace("<TITLE>", song.trimmed());

	if(_m->server_list[_m->cur_server].to_lower){
		return url.toLower();
	}

	return url;
}


void LyricLookupThread::run(const QString& artist, const QString& title, int server_idx)
{
	_m->artist = artist;
	_m->title = title;

	_m->cur_server = std::max(0, server_idx);
	_m->cur_server = std::min(server_idx, _m->server_list.size() - 1);

	if(_m->artist.isEmpty() && _m->title.isEmpty()) {
		_m->final_wp = "No track selected";
		return;
	}

	_m->final_wp.clear();

	QString url = this->calc_server_url(_m->artist, _m->title);

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished, this, &LyricLookupThread::content_fetched);
	awa->run(url);
}


void LyricLookupThread::content_fetched()
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	QString url = awa->get_url();

	if(!awa->has_data())
	{
		_m->final_wp = tr("Sorry, could not fetch lyrics from %1").arg(awa->get_url());
		emit sig_finished();
		return;
	}

	_m->final_wp = parse_webpage(awa->get_data(), _m->server_list[_m->cur_server]);

	if ( _m->final_wp.isEmpty() )
	{
		_m->final_wp = tr("Sorry, no lyrics found") + "<br />" + url;
		emit sig_finished();
		return;
	}

	_m->final_wp.push_front(_m->server_list[_m->cur_server].display_str + "<br /><br />");
	_m->final_wp.push_front(awa->get_url() + "<br /><br />");
	_m->final_wp.push_front(
						//"<font size=\"5\" color=\"#F3841A\">"
						"<b>" +
						 _m->artist + " - " +  _m->title +
						 "</b>"
						 //"</font><br /><br />"
				);

	emit sig_finished();
}

void LyricLookupThread::init_server_list()
{
	/*parse_xml();
	return;*/

	// motörhead
	// crosby, stills & nash
	// guns 'n' roses
	// AC/DC
	// the doors
	// the rolling stones
	// petr nalitch
	// eric burdon and the animals
	// Don't speak


	ServerTemplate wikia;
	wikia.display_str = "Wikia.com";
	wikia.server_address = QString("http://lyrics.wikia.com");
	wikia.addReplacement(" ", "_");
	wikia.addReplacement("&", "%26");
	wikia.call_policy = QString("<SERVER>/wiki/<ARTIST>:<TITLE>");
	wikia.start_end_tag.insert("<div class='lyricbox'>", "<!--");
	wikia.include_start_tag = false;
	wikia.include_end_tag = false;
	wikia.is_numeric = true;
	wikia.to_lower = false;
	wikia.error = "this page needs content";


	ServerTemplate oldieLyrics;
	oldieLyrics.display_str = "OldieLyrics.com";
	oldieLyrics.server_address = QString("http://www.oldielyrics.com/lyrics");
	oldieLyrics.addReplacement(" ", "_");
	oldieLyrics.addReplacement("(", "_");
	oldieLyrics.addReplacement(")", "_");
	oldieLyrics.addReplacement(".", "_");
	oldieLyrics.addReplacement("&", "_");
	oldieLyrics.addReplacement("'", "");
	oldieLyrics.addReplacement("__", "_");
	oldieLyrics.call_policy = QString("<SERVER>/<ARTIST>/<TITLE>.html");
	oldieLyrics.start_end_tag.insert("&#", "</div>");
	oldieLyrics.include_start_tag = true;
	oldieLyrics.include_end_tag = false;
	oldieLyrics.is_numeric = true;
	oldieLyrics.to_lower = true;
	oldieLyrics.error = QString("error 404");


	ServerTemplate lyricskeeper;
	lyricskeeper.display_str = "lyrics keeper";
	lyricskeeper.server_address = QString("http://lyrics-keeper.com");
	lyricskeeper.addReplacement("&", "");
	lyricskeeper.addReplacement(" ", "-");
	lyricskeeper.addReplacement("'", "-");
	lyricskeeper.addReplacement("--", "-");
	lyricskeeper.call_policy = QString("<SERVER>/en/<ARTIST>/<TITLE>.html");
	lyricskeeper.start_end_tag.insert("<div id=\"lyrics\">", "</div>");
	lyricskeeper.include_start_tag = false;
	lyricskeeper.include_end_tag = false;
	lyricskeeper.is_numeric = false;
	lyricskeeper.to_lower = true;
	lyricskeeper.error = QString("page cannot be found");


	ServerTemplate metrolyrics;
	metrolyrics.display_str = "MetroLyrics.com";
	metrolyrics.server_address = QString("http://www.metrolyrics.com");
	metrolyrics.addReplacement("&", "and");
	metrolyrics.addReplacement(" ", "-");
	metrolyrics.call_policy = QString("<SERVER>/<TITLE>-lyrics-<ARTIST>.html");
	//metrolyrics.start_end_tag.insert("<span class='line line-s' id='line_1'>", "</p>");
	metrolyrics.start_end_tag.insert ("<div id=\"lyrics-body-text\" class=\"js-lyric-text\">", "</div>");
	metrolyrics.include_start_tag = false;
	metrolyrics.include_end_tag = false;
	metrolyrics.is_numeric = false;
	metrolyrics.to_lower = true;
	metrolyrics.error = QString("404 page not found");


	ServerTemplate elyrics;
	elyrics.display_str = "eLyrics";
	elyrics.server_address= QString("http://www.elyrics.net/read");
	elyrics.addReplacement(" ", "-");
	elyrics.addReplacement("the ", "");
	elyrics.addReplacement("The ", "");
	elyrics.addReplacement("'", "_");
	elyrics.call_policy = QString("<SERVER>/<FIRST_ARTIST_LETTER>/<ARTIST>-lyrics/<TITLE>-lyrics.html");

	elyrics.start_end_tag.insert("lyrics</strong><br>", "</div>");
	elyrics.start_end_tag.insert("<div id='inlyr' style='font-size:14px;'>", "</div>");

	elyrics.include_start_tag = false;
	elyrics.include_end_tag = false;
	elyrics.is_numeric = false;
	elyrics.to_lower = true;
	elyrics.error = QString("Error 404");


	ServerTemplate golyr;
	golyr.display_str = "Golyr";
	golyr.server_address = "http://www.golyr.de";
	golyr.call_policy = "<SERVER>/<ARTIST>/songtext-<TITLE>";
	golyr.addReplacement("'", "-");
	golyr.addReplacement(" ", "-");
	golyr.addReplacement("(", "");
	golyr.addReplacement(")", "");
	golyr.start_end_tag.insert("<div id=\"lyrics\">", "</div> <div class=\"fads\"");
	golyr.include_end_tag = false;
	golyr.is_numeric = false;
	golyr.to_lower = false;
	golyr.error = "404 Not Found";

	ServerTemplate musixmatch;
	musixmatch.display_str = "Musixmatch";
	musixmatch.server_address = "https://www.musixmatch.com/";
	musixmatch.call_policy = "<SERVER>/lyrics/<ARTIST>/<TITLE>";
	musixmatch.addReplacement(" ", "-");
	musixmatch.addReplacement("'", "");
	musixmatch.addReplacement("/", "-");
	musixmatch.addReplacement(".", "-");
	musixmatch.addReplacement("&", "-");
	musixmatch.addReplacement("--", "-");
	musixmatch.start_end_tag.insert("<div id=\"selectable-lyrics\"", "</span><span data-reactid");
	musixmatch.start_end_tag.insert("<p class=.*content", "</p>");
	musixmatch.start_end_tag.insert("\"body\":\"", "\",\"");

	musixmatch.include_end_tag = false;
	musixmatch.is_numeric = false;
	musixmatch.to_lower = false;
	musixmatch.error = "404 Not Found";

	_m->server_list.push_back(wikia);
	_m->server_list.push_back(musixmatch);
	_m->server_list.push_back(metrolyrics);
	_m->server_list.push_back(oldieLyrics);
	_m->server_list.push_back(lyricskeeper);
	_m->server_list.push_back(elyrics);
	_m->server_list.push_back(golyr);


	/*sp_log(Log::Info) << "Servers: [";
	for(const ServerTemplate& t : _m->server_list){
		t.print_json();
		sp_log(Log::Info) << ",";
	}
	sp_log(Log::Info) << "]";*/

}

QStringList LyricLookupThread::get_servers() const
{
	QStringList lst;
	for(const ServerTemplate& t : _m->server_list) {
		lst << t.display_str;
	}

	return lst;
}

QString LyricLookupThread::get_lyric_data() const
{
	return _m->final_wp;
}



QString LyricLookupThread::parse_webpage(const QByteArray& raw, const ServerTemplate& t) const
{
	QString dst(raw);

	for(QString start_tag : t.start_end_tag.keys()) {
		QString content;
		QString end_tag;

		end_tag = t.start_end_tag.value(start_tag);

		start_tag = convert_to_regex(start_tag);
		if(start_tag.startsWith("<") && !start_tag.endsWith(">")){
			start_tag.append(".*>");
		}

		end_tag = convert_to_regex(end_tag);

		QRegExp regex;
		regex.setMinimal(true);
		regex.setPattern(start_tag + "(.+)" + end_tag);
		if(regex.indexIn(dst) != -1){
			content  = regex.cap(1);
		}

		if(content.isEmpty()){
			continue;
		}


		QRegExp re_script;
		re_script.setPattern("<script.+</script>");
		re_script.setMinimal(true);
		while(re_script.indexIn(content) != -1){
			content.replace(re_script, "");
		}

		QString word;
		if(t.is_numeric) {
			QRegExp rx("&#(\\d+);|<br />|</span>|</p>");

			QStringList tmplist;
			int pos = 0;
			while ((pos = rx.indexIn(content, pos)) != -1) {
				QString str = rx.cap(1);

				pos += rx.matchedLength();
				if(str.size() == 0) {
					tmplist.push_back(word);
					word = "";
					tmplist.push_back("<br />");
				}

				else{
					word.append(QChar(str.toInt()));
				}
			}

			dst = "";

			for(const QString& str : tmplist) {
				dst.append(str);
			}
		}

		else{
			dst = content;
		}

		dst.replace("\n", "<br />");
		dst.replace("\\n", "<br />");

		if(dst.size() > 100){
			break;
		}
	}

	return dst;
}
