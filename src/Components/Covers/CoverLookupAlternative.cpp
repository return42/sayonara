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

#include "CoverLookupAlternative.h"
#include "CoverLocation.h"
#include "Database/DatabaseHandler.h"

struct CoverLookupAlternative::Private
{
	CoverLookupPtr      cl;
	CoverLocation		cover_location;

	int					n_covers;
	bool				run;

	~Private(){
		cl->stop();
	}
};

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, int n_covers) :
    CoverLookupInterface(parent)
{
	_m = Pimpl::make<CoverLookupAlternative::Private>();
	_m->run = true;
	_m->n_covers = n_covers;
}

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const CoverLocation& cl, int n_covers) : 
	CoverLookupAlternative(parent, n_covers)
{
	_m->cover_location = cl;
}

CoverLookupAlternative::~CoverLookupAlternative() {}

void CoverLookupAlternative::stop() {
	_m->cl->stop();
}

void CoverLookupAlternative::start() {

	_m->run = true;

	_m->cl = CoverLookupPtr(new CoverLookup(this, _m->n_covers));

	connect(_m->cl.get(), &CoverLookup::sig_cover_found, this, &CoverLookupAlternative::cover_found);
	connect(_m->cl.get(), &CoverLookup::sig_finished, this, &CoverLookupAlternative::finished);

	_m->cl->fetch_cover(_m->cover_location);
}

void CoverLookupAlternative::cover_found(const QString& cover_path)
{
    emit sig_cover_found(cover_path);
}

void CoverLookupAlternative::finished(bool success)
{
    emit sig_finished(success);
}


