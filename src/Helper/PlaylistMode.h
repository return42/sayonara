/* PlaylistMode.h */

/* Copyright (C) 2011  Lucio Carreras
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


#ifndef PLAYLISTMODE_H_
#define PLAYLISTMODE_H_


#include <iostream>
#include <QList>
#include <QVariant>
#include <QStringList>
#include <QDebug>

using namespace std;

struct PlaylistMode {

	bool				rep1;
	bool				repAll;
	bool				repNone;
	bool				append;
	bool				shuffle;
	bool				dynamic;
	bool				gapless;

	bool				ui_rep1;
	bool				ui_repAll;
	bool				ui_append;
	bool				ui_shuffle;
	bool				ui_dynamic;
	bool				ui_gapless;

	PlaylistMode(){
		rep1 = false;
		repAll = false;
		repNone = true;
		append = false;
		shuffle = false;
		gapless = false;
		dynamic = false;

		ui_rep1 = true;
		ui_repAll = true;
		ui_append = true;
		ui_shuffle = true;
		ui_dynamic = true;
		ui_gapless = true;
	}


	void print(){
		cout << "rep1 = " << rep1 << ", "
			<< "repAll = " << repAll << ", "
			<< "repNone = " << repNone << ", "
			<< "append = " << append <<", "
			<< "dynamic = " << dynamic << ","
			<< "gapless = " << gapless << endl;
			
	}

	QString toString() const {
		QString str;
		str += (append ? "1" : "0")  + QString(",");
		str += (repAll ? "1" : "0")  + QString(",");
		str += (rep1 ? "1" : "0")  + QString(",");
		str += (repNone ? "1" : "0")  + QString(",");
		str += (shuffle ? "1" : "0")  + QString(",");
		str += (dynamic ? "1" : "0") + QString(",");
		str += (gapless ? "1" : "0");

		return str;
	}

    static PlaylistMode fromString(QString str){

        PlaylistMode plm;
		QStringList list = str.split(',');

        if(list.size() < 6) return plm;

        plm.append = list[0].toInt() == 1;
        plm.repAll = list[1].toInt() == 1;
        plm.rep1 = list[2].toInt() == 1;
        plm.repNone = list[3].toInt() == 1;
        plm.shuffle = list[4].toInt() == 1;
        plm.dynamic = list[5].toInt() == 1;

		if(list.size() > 6){
			plm.gapless = list[6].toInt() == 1;
		}

        return plm;
	}

	bool operator==(const PlaylistMode& pm) const {

		if(pm.append != append) return false;
		if(pm.repAll != repAll) return false;
		if(pm.rep1 != rep1) return false;
		if(pm.shuffle != shuffle) return false;
		if(pm.dynamic != dynamic) return false;
		if(pm.gapless != gapless) return false;

		return true;
	}
};

#endif
