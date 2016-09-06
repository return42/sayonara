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
		
	QList<int>		_values;
	QString			_name;

	EQ_Setting(const QString& name="");
	EQ_Setting(const EQ_Setting& s);
	virtual ~EQ_Setting();
	
	bool operator==(const EQ_Setting& s) const;



	QString name() const;
	void set_name(const QString& name);

	QList<int> values() const;
	int value(int idx) const;

	void set_value(int idx, int val);
	void set_values(const QList<int> values);
	void append_value(int val);

	bool is_default() const;
	bool is_default_name() const;

	static QList<EQ_Setting> get_defaults();
	static QList<int> get_default_values(const QString& name);
	static bool is_default_name(const QString& name);

	static EQ_Setting fromString(const QString& str);
	QString toString() const;

};




#endif
