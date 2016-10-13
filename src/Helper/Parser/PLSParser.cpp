/* PLSParser.cpp */

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

#include "PLSParser.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaData.h"

#include <QRegExp>

#include <algorithm>


static int split_line(const QString& line, QString& key, QString& val)
{
	int idx;
	int pos_idx;
	QRegExp re_idx("(\\S+)([0-9]+)");
	QStringList splitted = line.split("=");

	if(splitted.size() < 2){
		return -1;
	}

	pos_idx = re_idx.indexIn(splitted[0]);
	if(pos_idx < 0){
		return -1;
	}

	key = re_idx.cap(1).toLower();
	val = splitted[1];
	idx = re_idx.cap(2).toInt();

	return idx;
}




PLSParser::PLSParser(const QString& filename) :
	AbstractPlaylistParser(filename)
{
}

PLSParser::~PLSParser() {}

void PLSParser::parse(){

	QStringList lines = _file_content.split("\n");

	MetaData md;
	int cur_track_idx = -1;

	for(QString line : lines) {

		QString key, val;
		int track_idx;

		line = line.trimmed();
		if(line.isEmpty() || line.startsWith("#")){
			continue;
		}

		track_idx = split_line(line, key, val);

		if(track_idx < 0){
			continue;
		}


		if(track_idx != cur_track_idx){

			if(cur_track_idx > 0){
				_v_md << md;
			}

			md = MetaData();
			cur_track_idx = track_idx;
		}


		md.track_num = track_idx;

		if(key.startsWith("file")) {
			QString filepath = get_absolute_filename(val);
			md.set_filepath(filepath);
			md.artist = filepath;
		}

		else if(key.startsWith("title")){
			md.title = val;
		}

		else if(key.startsWith("length")) {

			int len = val.toInt();

			len = std::max(0, len);
			md.length_ms = len * 1000;
		}
	}

	if(!md.filepath().isEmpty()){
		_v_md << md;
	}
}

