/* CoverLookup.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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


/*
 * CoverLookup.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: Lucio Carreras
 */

#include "CoverLookup.h"
#include "CoverFetchThread.h"
#include "CoverLocation.h"

#include "Database/DatabaseConnector.h"

#include "Utils/MetaData/MetaData.h"
#include "Utils/MetaData/Album.h"

#include <QFile>
#include <QImage>

using Cover::Lookup;
using Cover::FetchThread;

struct Lookup::Private
{
    int             n_covers;
    FetchThread*    cft=nullptr;

	Private(int n_covers) :
		n_covers(n_covers)
	{}
};

Lookup::Lookup(QObject* parent, int n_covers) :
	LookupBase(parent)
{
	m = Pimpl::make<Private>(n_covers);
}

Lookup::~Lookup() {}

bool Lookup::start_new_thread(const Cover::Location& cl )
{
	if(!cl.has_search_urls()){
		return false;
	}

	FetchThread* cft = new FetchThread(this, cl, m->n_covers);

	connect(cft, &FetchThread::sig_cover_found, this, &Lookup::cover_found);
	connect(cft, &FetchThread::sig_finished, this, &Lookup::finished);

	cft->start();

	m->cft = cft;

	return true;
}


bool Lookup::fetch_cover(const Location& cl, bool also_www)
{
	// Look, if cover exists in .Sayonara/covers
	if( QFile::exists(cl.cover_path()) && m->n_covers == 1 )
	{
		emit sig_cover_found(cl.cover_path());
		emit sig_finished(true);
		return true;
	}

	// For one cover, we also can use the local cover path
	if(!cl.local_paths().isEmpty() && m->n_covers == 1)
	{	
		emit sig_cover_found(cl.local_path(0));
		emit sig_finished(true);
		return true;
	}

	// we have to fetch the cover from the internet
	if(also_www){
		if(!start_new_thread( cl )){
			return false;
		}
	}

	else{
		return false;
	}

	return true;
}


bool Lookup::fetch_album_cover(const Album& album, bool also_www)
{
	Location cl = Location::get_cover_location(album);
	return fetch_cover(cl, also_www);
}


void Lookup::finished(bool success)
{
	m->cft = nullptr;
    emit sig_finished(success);
}


void Lookup::cover_found(const QString& file_path)
{
	FetchThread* cft = static_cast<FetchThread*>(sender());
    emit sig_cover_found(file_path);

	if(!cft->more()){
		emit sig_finished(true);
	}
}

void Lookup::stop()
{
	if(m->cft){
		m->cft->stop();
		emit sig_finished(true);
	}
}
