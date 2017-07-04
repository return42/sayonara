/* Genre.cpp */

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



#include "Genre.h"
#include "Helper/Helper.h"
#include <QHash>

struct Genre::Private
{
	uint32_t id;
	QString name;

	static uint32_t calc_id(const QString& name)
	{
		return static_cast<uint32_t> (qHash(name.trimmed().toLower().toLocal8Bit()));
	}
};

Genre::Genre(const QString& name)
{
	_m = Pimpl::make<Private>();
	_m->name = name;
	_m->id = _m->calc_id(name);
}

Genre::~Genre() {}

uint32_t Genre::calc_id(const QString& name)
{
	return Genre::Private::calc_id(name);
}

Genre::Genre(const Genre& other)
{
	_m = Pimpl::make<Private>();
	_m->name = other.name();
	_m->id = other.id();
}

Genre& Genre::operator =(const Genre& other)
{
	_m->name = (other.name());
	_m->id = (other.id());
	return *this;
}


uint32_t Genre::id() const
{
	return _m->id;
}

QString Genre::name() const
{
	return Helper::cvt_str_to_first_upper(_m->name);
}

void Genre::set_name(const QString& name)
{
	_m->name = name;
	_m->id = Genre::Private::calc_id(name);
}

bool Genre::is_equal(const Genre& other) const
{
	return (_m->id == other.id());
}

bool Genre::operator ==(const Genre& other) const
{
	return is_equal(other);
}

bool Genre::operator <(const Genre& other) const
{
	return (_m->id < other.id());
}

bool Genre::operator >(const Genre& other) const
{
	return (_m->id > other.id());
}

