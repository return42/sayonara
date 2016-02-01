/* LFMTrackChangedThread.h

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

#ifndef LFMTRACKCHANGEDTHREAD_H_
#define LFMTRACKCHANGEDTHREAD_H_

#include <QThread>
#include <QList>
#include <QMap>
#include <QDomDocument>

#include "LFMGlobals.h"
#include "Helper/SmartCompare/SmartCompare.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/SayonaraClass.h"

#define LFM_THREAD_TASK_UPDATE_TRACK 		1<<0
#define LFM_THREAD_TASK_SIM_ARTISTS 		1<<1


enum class Quality : quint8 {
	Poor = 0,
	Well = 1,
	Very_Good = 2
};

struct ArtistMatch{

	QMap<QString, double> very_good;
	QMap<QString, double> well;
	QMap<QString, double> poor;

	QString artist;

	bool is_valid() const {
		return  (very_good.size() > 0 ||
				well.size() > 0  ||
				poor.size() > 0);
	}

	bool operator ==(ArtistMatch am) const {
		return (artist == am.artist);
	}

	void add(QString artist, double match) {
		if(match > 0.15) very_good[artist] = match;
		else if(match > 0.05) well[artist] = match;
		else poor[artist] = match;
	}

	QMap<QString, double> get(Quality q) const {
		switch(q) {
			case Quality::Poor:
				return poor;
			case Quality::Well:
				return well;
			case Quality::Very_Good:
				return very_good;
		}

		return very_good;
	}
};

class LFMTrackChangedThread : public QObject, protected SayonaraClass {

	Q_OBJECT


signals:

	void sig_similar_artists_available(const IDList& artist_ids);


public:
	LFMTrackChangedThread(const QString& username=QString(), const QString& session_key=QString(), QObject* parent=nullptr);
	~LFMTrackChangedThread();


	void set_session_key(const QString& session_key);
	void set_username(const QString& username);


	void search_similar_artists(const MetaData& md);
	void update_now_playing(const MetaData& md);


private:

	SmartCompare* _smart_comparison;

	MetaData 	_md;

	/* update track */
	QString		_username;
	QString 	_session_key;



	/* similar artists */
	IDList	_chosen_ids;


	ArtistMatch parse_similar_artists(const QDomDocument& doc);
	void evaluate_artist_match(const ArtistMatch& artist_match);

	QMap<QString, int> filter_available_artists(const QMap<QString, double>& artists);


	QMap<QString, ArtistMatch>   _sim_artists_cache;


private slots:
	void response_sim_artists(const QDomDocument& doc);
	void error_sim_artists(const QString& error);

	void response_update(const QString& response);
	void error_update(const QString& error);
};

#endif /* LFMTRACKCHANGEDTHREAD_H_ */
