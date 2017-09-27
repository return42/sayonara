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
#include "Helper/Language.h"

struct Library::Filter::Private
{
    Library::DateFilter	    date_filter;
    QString		    filtertext;
    Library::Filter::Mode   mode;
};

Library::Filter::Filter()
{
    m = Pimpl::make<Library::Filter::Private>();
    clear();
}


Library::Filter::Filter(const Library::Filter& other)
{
    m = Pimpl::make<Library::Filter::Private>();
    *m = *(other.m);
}

void Library::Filter::operator=(const Filter& other)
{
    *m = *(other.m);
}

Library::Filter::~Filter() {}


Library::DateFilter Library::Filter::date_filter() const
{
    return m->date_filter;
}

void Library::Filter::set_date_filter(const Library::DateFilter& filter)
{
    m->date_filter = filter;
}

QString Library::Filter::filtertext() const
{
    return m->filtertext;
}

void Library::Filter::set_filtertext(const QString& str)
{
    m->filtertext = str;
}

Library::Filter::Mode Library::Filter::mode() const
{
    return m->mode;
}

void Library::Filter::set_mode(Library::Filter::Mode mode)
{
    m->mode = mode;
}

bool Library::Filter::cleared() const
{
    return !(m->date_filter.valid() ||
			 m->filtertext.size() > 0);
}

QString Library::Filter::get_text(Library::Filter::Mode mode)
{
	switch(mode)
	{
		case Library::Filter::Mode::Date:
			return Lang::get(Lang::Date);
		case Library::Filter::Mode::Filename:
			return Lang::get(Lang::Filename);
		case Library::Filter::Mode::Fulltext:
            return Lang::get(Lang::Fulltext);
		case Library::Filter::Mode::Genre:
			return Lang::get(Lang::Genre);
		default:
			return QString();
	}
}

void Library::Filter::clear()
{
    m->date_filter = DateFilter("");
    m->filtertext = QString();
    m->mode = Mode::Fulltext;
}


