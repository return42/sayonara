/* CoverLookupAlternative.cpp */

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

#include "CoverLookup.h"
#include "CoverLookupAlternative.h"
#include "CoverLocation.h"
#include "Utils/Logger/Logger.h"

#include <QStringList>

using Cover::AlternativeLookup;
using Cover::Location;
using Cover::Lookup;

struct AlternativeLookup::Private
{
	Lookup*		cl=nullptr;
	Location	cover_location;
	QString		cover_location_string;

	int			n_covers;
	bool		run;

	~Private()
	{
		cl->stop();
	}
};

AlternativeLookup::AlternativeLookup(QObject* parent, int n_covers) :
	LookupBase(parent)
{
	m = Pimpl::make<AlternativeLookup::Private>();
	m->run = true;
	m->n_covers = n_covers;
}

AlternativeLookup::AlternativeLookup(QObject* parent, const Location& cl, int n_covers) :
	AlternativeLookup(parent, n_covers)
{
	m->cover_location = cl;
	m->cover_location_string = cl.to_string();

	sp_log(Log::Debug, this) << cl.search_urls();
}

AlternativeLookup::~AlternativeLookup() {}

void AlternativeLookup::stop()
{
	m->cl->stop();
}

void AlternativeLookup::start()
{
	m->run = true;

	m->cl = new Lookup(this, m->n_covers);

	connect(m->cl, &Lookup::sig_cover_found, this, &AlternativeLookup::cover_found);
	connect(m->cl, &Lookup::sig_finished, this, &AlternativeLookup::finished);

	bool can_fetch = m->cl->fetch_cover(m->cover_location, true);
	if(!can_fetch)
	{
		emit sig_finished(false);
	}
}

void AlternativeLookup::start(const QString& cover_fetcher_identifier)
{
	QString search_term = m->cover_location.search_term();
	m->cover_location.set_search_term(search_term, cover_fetcher_identifier);

	start();
}

Location AlternativeLookup::cover_location() const
{
	return m->cover_location;
}

void AlternativeLookup::set_cover_location(const Location& location)
{
	m->cover_location = location;
	m->cover_location_string = location.to_string();
}

void AlternativeLookup::cover_found(const QString& cover_path)
{
	emit sig_cover_found(cover_path);
}

void AlternativeLookup::finished(bool success)
{
	emit sig_finished(success);
}


