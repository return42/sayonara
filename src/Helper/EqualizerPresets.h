/* Equalizer_presets.h */

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


#ifndef _EQUALIZER_PRESETS_
#define _EQUALIZER_PRESETS_


#include <QList>
#include <QString>

struct EQ_Setting{
		
	QList<int>		values;
	QString			name;

	EQ_Setting(QString n="");
	EQ_Setting(const EQ_Setting& s);
	virtual ~EQ_Setting();
	
	bool operator==(const EQ_Setting& s);

	static QList<EQ_Setting> get_defaults();

	static EQ_Setting fromString(const QString& str);
	QString toString() const;

};



#endif
