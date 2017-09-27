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
#include "Database/DatabaseHandler.h"
#include "Helper/Logger/Logger.h"

#include <QStringList>


struct CoverLookupAlternative::Private
{
	CoverLookupPtr      cl;
	CoverLocation		cover_location;

	int					n_covers;
	bool				run;

	~Private()
	{
		cl->stop();
	}
};

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, int n_covers) :
	AbstractCoverLookup(parent)
{
	m = Pimpl::make<CoverLookupAlternative::Private>();
	m->run = true;
	m->n_covers = n_covers;
}

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const CoverLocation& cl, int n_covers) : 
	CoverLookupAlternative(parent, n_covers)
{
	m->cover_location = cl;

	sp_log(Log::Debug, this) << cl.search_urls();
}

CoverLookupAlternative::~CoverLookupAlternative() {}

void CoverLookupAlternative::stop()
{
	m->cl->stop();
}

void CoverLookupAlternative::start()
{
	m->run = true;

	m->cl = CoverLookupPtr(new CoverLookup(this, m->n_covers));

	connect(m->cl.get(), &CoverLookup::sig_cover_found, this, &CoverLookupAlternative::cover_found);
	connect(m->cl.get(), &CoverLookup::sig_finished, this, &CoverLookupAlternative::finished);

	bool can_fetch = m->cl->fetch_cover(m->cover_location, true);
	if(!can_fetch){
		emit sig_finished(false);
	}
}

void CoverLookupAlternative::cover_found(const QString& cover_path)
{
	emit sig_cover_found(cover_path);
}

void CoverLookupAlternative::finished(bool success)
{
	emit sig_finished(success);
}


