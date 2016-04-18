/* SoundcloudJsonParser.h */

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



#ifndef SOUNDCLOUDJSONPARSER_H
#define SOUNDCLOUDJSONPARSER_H



#include "SoundcloudGlobal.h"
#include "Helper/Logger/Logger.h"

#include <QList>
#include <QString>
#include <QByteArray>
#include <QObject>

class MetaData;
class MetaDataList;
class Artist;
class Album;
class ArtistList;
class AlbumList;

struct JsonItem {
	enum class Type : quint8 {
		Array=0,
		Number,
		String,
		Block
	} type;

	QList<JsonItem> values;
	QString key;
	QByteArray pure_value;

	void print(const QString& parent_key="") const {


		QString my_key = parent_key + "." + key;
		int i=0;
		for(const JsonItem& item : values){
			item.print(my_key);
			i++;
		}

		if(values.size() == 0){
			sp_log(Log::Debug) << my_key << " = " << pure_value;
		}
	}

};


class SoundcloudJsonParser : public QObject
{

	Q_OBJECT

private:
	QByteArray			_content;

	JsonItem			parse(QString key, const QByteArray& content);

	JsonItem			parse_array(QString key, QByteArray content);
	JsonItem			parse_block(QString key, QByteArray content);
	JsonItem			parse_standard(QString key, QByteArray content);

	int					find_block_end(const QByteArray& content, int start_at=0);
	int					find_array_end(const QByteArray& content, int start_at=0);
	int					find_value_end(const QByteArray& content, int start_at=0);

	bool				extract_track(const JsonItem& item, MetaData& md);
	bool				extract_artist(const JsonItem& item, Artist& artist);
	bool				extract_playlist(const JsonItem& item, Album& album, MetaDataList& v_md);


public:
	SoundcloudJsonParser(const QByteArray& content);
	~SoundcloudJsonParser();

	bool				parse_artists(ArtistList& artists);
	bool				parse_tracks(MetaDataList& v_md);
	bool				parse_playlists(AlbumList& albums, MetaDataList& v_md);

};

#endif // SOUNDCLOUDJSONPARSER_H
