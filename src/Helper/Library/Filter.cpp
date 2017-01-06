/* Filter.cpp */

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

#include "Filter.h"
#include "DateFilter.h"

struct Library::Filter::Private
{
    Library::DateFilter	    date_filter;
    QString		    filtertext;
    Library::Filter::Mode   mode;

};

Library::Filter::Filter()
{
    _m = Pimpl::make<Library::Filter::Private>();
    clear();
}


Library::Filter::Filter(const Library::Filter& other)
{
    _m = Pimpl::make<Library::Filter::Private>();
    *(_m) = *(other._m);

}

void Library::Filter::operator=(const Filter& other)
{
    *(_m) = *(other._m);
}

Library::Filter::~Filter(){}


Library::DateFilter Library::Filter::date_filter() const
{
    return _m->date_filter;
}

void Library::Filter::set_date_filter(const Library::DateFilter& filter)
{
    _m->date_filter = filter;
}

QString Library::Filter::filtertext() const
{
    return _m->filtertext;
}

void Library::Filter::set_filtertext(const QString& str)
{
    _m->filtertext = str;
}

Library::Filter::Mode Library::Filter::mode() const
{
    return _m->mode;
}

void Library::Filter::set_mode(Library::Filter::Mode mode)
{
    _m->mode = mode;
}

bool Library::Filter::cleared() const
{
    return !(_m->date_filter.valid() ||
	    _m->filtertext.size() > 0);
}

void Library::Filter::clear()
{
    _m->date_filter = DateFilter("");
    _m->filtertext = QString();
    _m->mode = Mode::Fulltext;
}


