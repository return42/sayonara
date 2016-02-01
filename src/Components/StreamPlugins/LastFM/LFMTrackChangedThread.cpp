/* LFMTrackChangedThread.cpp

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Jul 18, 2012 
 *
 */

#include "LFMTrackChangedThread.h"
#include "LFMWebAccess.h"
#include "LFMGlobals.h"

#include "Database/DatabaseHandler.h"

#include "Helper/Helper.h"

#include <QMap>
#include <QStringList>
#include <QUrl>
#include <QDir>
#include <QFile>


LFMTrackChangedThread::LFMTrackChangedThread(const QString& username, const QString& session_key, QObject* parent) :
	QObject(parent),
	SayonaraClass()
{

	_username = username;
	_session_key = session_key;

	ArtistList artists;
	DatabaseConnector::getInstance()->getAllArtists(artists);

	_smart_comparison = new SmartCompare(artists);
}


LFMTrackChangedThread::~LFMTrackChangedThread() {
	delete _smart_comparison;
}

void LFMTrackChangedThread::set_session_key(const QString& session_key) {
	_session_key = session_key;
}

void LFMTrackChangedThread::set_username(const QString& username) {
	_username = username;
}



void LFMTrackChangedThread::update_now_playing(const MetaData& md) {

	_md = md;

	LFMWebAccess* lfm_wa = new LFMWebAccess();

	connect(lfm_wa, LFMWebAccess_sig_response_str, this, &LFMTrackChangedThread::response_update);
	connect(lfm_wa, &LFMWebAccess::sig_error, this, &LFMTrackChangedThread::error_update);

	QString artist = _md.artist;
	QString title = _md.title;

    if(artist.trimmed().size() == 0) artist = "Unknown";
    artist.replace("&", "&amp;");

	UrlParams sig_data;
		sig_data["api_key"] = LFM_API_KEY;
		sig_data["artist"] = artist.toLocal8Bit();
		sig_data["duration"] = QString::number(_md.length_ms / 1000).toLocal8Bit();
		sig_data["method"] = QString("track.updatenowplaying").toLocal8Bit();
		sig_data["sk"] = _session_key.toLocal8Bit();
		sig_data["track"] =  title.toLocal8Bit();

		sig_data.append_signature();


	QByteArray post_data;
	QString url = lfm_wa->create_std_url_post(
				QString("http://ws.audioscrobbler.com/2.0/"),
				sig_data,
				post_data);

	lfm_wa->call_post_url(url, post_data);
}


void LFMTrackChangedThread::response_update(const QString& response){
	Q_UNUSED(response)
	LFMWebAccess* lfm_wa = static_cast<LFMWebAccess*>(sender());
	lfm_wa->deleteLater();
}


void LFMTrackChangedThread::error_update(const QString& error){
	LFMWebAccess* lfm_wa = static_cast<LFMWebAccess*>(sender());

	sp_log(Log::Warning) << "Last.fm: Cannot update track";
	sp_log(Log::Warning) << "Last.fm: " << error;

	lfm_wa->deleteLater();
}


ArtistMatch LFMTrackChangedThread::parse_similar_artists(const QDomDocument& doc){

	ArtistMatch artist_match;
	artist_match.artist = _md.artist;

	QDomElement docElement = doc.documentElement();
	QDomNode similarartists = docElement.firstChild();			// similarartists

	QString dir_name = Helper::get_sayonara_path() + "/similar_artists";
	QString file_name = dir_name + "/" +  _md.artist.toLower();

	if(similarartists.hasChildNodes()) {

		QDir dir;
		QFile file;
		dir.mkpath(dir_name);
		file.setFileName(file_name);

		if(!file.exists()){
			file.open(QIODevice::WriteOnly);
			file.write(doc.toByteArray());
			file.close();
		}


		QString artist_name = "";
		double match = -1.0;

		for(int idx_artist=0; idx_artist < similarartists.childNodes().size(); idx_artist++) {
			QDomNode artist = similarartists.childNodes().item(idx_artist);

			if(artist.nodeName().toLower().compare("artist") != 0) continue;

			if(!artist.hasChildNodes()) continue;

			for(int idx_content = 0; idx_content <artist.childNodes().size(); idx_content++) {
				QDomNode content = artist.childNodes().item(idx_content);
				if(content.nodeName().toLower().contains("name")) {
					QDomElement e = content.toElement();
					if(!e.isNull()) {
						artist_name = e.text();
					}
				}

				if(content.nodeName().toLower().contains("match")) {
					QDomElement e = content.toElement();
					if(!e.isNull()) {
						match = e.text().toDouble();
					}
				}

				if(artist_name.size() > 0 && match > 0) {
					artist_match.add(artist_name, match);
					artist_name = "";
					match = -1.0;
					break;
				}
			}
		}
	}

	else{
		sp_log(Log::Warning) << "LastFM Similar artists: empty xml document";
		return ArtistMatch();
	}

	_sim_artists_cache[_md.artist] = artist_match;
	return artist_match;
}




void LFMTrackChangedThread::search_similar_artists(const MetaData& md) {

	if(md.db_id != 0) return;

	_md = md;

	ArtistMatch artist_match;
	artist_match.artist = _md.artist;

	if(_md.artist.trimmed().size() == 0){
		return;
	}

    // check if already in cache
	if(_sim_artists_cache.keys().contains(_md.artist)) {
		artist_match = _sim_artists_cache.value(_md.artist);
		evaluate_artist_match(artist_match);
		return;
    }


	LFMWebAccess* lfm_wa = new LFMWebAccess();

	connect(lfm_wa, LFMWebAccess_sig_response_doc, this, &LFMTrackChangedThread::response_sim_artists);
	connect(lfm_wa, &LFMWebAccess::sig_error, this, &LFMTrackChangedThread::error_sim_artists);

	QString url = 	QString("http://ws.audioscrobbler.com/2.0/?");
	QString encoded = QUrl::toPercentEncoding( _md.artist );
	url += QString("method=artist.getsimilar&");
	url += QString("artist=") + encoded + QString("&");
	url += QString("api_key=") + LFM_API_KEY;

	lfm_wa->call_url_xml(url);
	return;
}


void LFMTrackChangedThread::evaluate_artist_match(const ArtistMatch& artist_match){

	if(!artist_match.is_valid()){
		return;
	}



	// if we always take the best, it's boring
	Quality quality, quality_org;
	RandomGenerator rnd;
	int rnd_number = rnd.get_number(1, 999);

	if(rnd_number > 350) {
		quality = Quality::Very_Good;			// [250-999]
	}

	else if(rnd_number > 75){
		quality = Quality::Well;		// [50-250]
	}

	else {
		quality = Quality::Poor;
	}

	quality_org = quality;
	QMap<QString, int> possible_artists;

	while(possible_artists.size() == 0) {

		QMap<QString, double> quality_map = artist_match.get(quality);
		possible_artists = filter_available_artists(quality_map);

		switch(quality){
			case Quality::Poor:
				quality = Quality::Very_Good;
				break;
			case Quality::Well:
				quality = Quality::Poor;
				break;
			case Quality::Very_Good:
				quality = Quality::Well;
				break;
			default: // will never be executed
				quality = quality_org;
				break;
		}

		if(quality == quality_org) {
			break;
		}
	}

	if(possible_artists.size() == 0){
		return;
	}

	_chosen_ids.clear();
	for(auto it = possible_artists.begin(); it != possible_artists.end(); it++) {
		_chosen_ids.push_back(it.value());
	}

	if(!_chosen_ids.isEmpty()){
		emit sig_similar_artists_available(_chosen_ids);
	}
}


QMap<QString, int> LFMTrackChangedThread::filter_available_artists(const QMap<QString, double>& artist_match) {

		DatabaseConnector* db = DatabaseConnector::getInstance();
        QMap<QString, int> possible_artists;

		for(const QString& key : artist_match.keys()) {

#if SMART_COMP

            QMap<QString, float> sc_map = _smart_comparison->get_similar_strings(key);
			for(const QString& sc_key : sc_map.keys() ){
				int artist_id = db->getArtistID(sc_key);
				if(artist_id >= 0 && sc_map[sc_key] > 5.0f){

					possible_artists[sc_key] = artist_id;
				}

			}

#else
                int artist_id = db->getArtistID(key);
                if(artist_id >= 0 ){

                    possible_artists[key] = artist_id;
                }

#endif
        }

        return possible_artists;
}

void LFMTrackChangedThread::response_sim_artists(const QDomDocument& doc){

	ArtistMatch artist_match = parse_similar_artists(doc);
	evaluate_artist_match(artist_match);
}

void LFMTrackChangedThread::error_sim_artists(const QString& error){
	sp_log(Log::Warning) << "Last.fm: Search similar artists" << error;
}
