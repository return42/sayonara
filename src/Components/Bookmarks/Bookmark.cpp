/* Bookmark.cpp */

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

#include "Bookmark.h"
#include <QString>

struct Bookmark::Private
{
	uint32_t		time;
	QString		name;
	bool		valid;
};

Bookmark::Bookmark(uint32_t time)
{
	_m = Pimpl::make<Private>();
	_m->time = time;
}

Bookmark::Bookmark(uint32_t time, const QString& name, bool valid) :
	Bookmark(time)
{
	_m->name = name;
	_m->valid = valid;
}

Bookmark::~Bookmark(){}


Bookmark::Bookmark(const Bookmark& other) :
	Bookmark(other._m->time, other._m->name, other._m->valid)
{

}

Bookmark& Bookmark::operator=(const Bookmark& other)
{
	_m->time = other._m->time;
	_m->name = other._m->name;
	_m->valid = other._m->valid;

	return *this;
}

uint32_t Bookmark::get_time() const
{
	return _m->time;
}

QString Bookmark::get_name() const
{
	return _m->name;
}

bool Bookmark::is_valid() const
{
	return _m->valid;
}
