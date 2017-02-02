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
#include "CoverHelper.h"
#include "CoverFetchThread.h"
#include "CoverLocation.h"
#include "GoogleCoverFetcher.h"
#include "StandardCoverFetcher.h"
#include "LFMCoverFetcher.h"

#include "Database/DatabaseConnector.h"

#include "Helper/MetaData/MetaData.h"
#include "Helper/MetaData/Album.h"

#include <QFile>
#include <QImage>

CoverLookupInterface::CoverLookupInterface(QObject* parent):
	QObject(parent) {}

CoverLookupInterface::~CoverLookupInterface() {}

CoverLookup::CoverLookup(QObject* parent, int n_covers) :
	CoverLookupInterface(parent)
{
	_n_covers = n_covers;
}

CoverLookup::~CoverLookup() {}

void CoverLookup::start_new_thread(const CoverLocation& cl )
{
	// TODO:
	if(!cl.has_search_urls()){
		return;
	}

	QString url = cl.search_urls().first();
	CoverFetchThread* cft;
	if(url.contains("google", Qt::CaseInsensitive)){
		cft = new GoogleCoverFetcher(this, cl, _n_covers);
	}

	else if(url.contains("audioscrobbler")){
		cft = new LFMCoverFetcher(this, cl, _n_covers);
	}

	else{
		cft = new StandardCoverFetcher(this, cl, _n_covers);
	}

	connect(cft, &CoverFetchThread::sig_cover_found, this, &CoverLookup::cover_found);
	connect(cft, &CoverFetchThread::sig_finished, this, &CoverLookup::finished);

	cft->start();
}


bool CoverLookup::fetch_cover(const CoverLocation& cl)
{
	// Look, if cover exists in .Sayonara/covers
	if( QFile::exists(cl.cover_path()) && _n_covers == 1 )
	{
		emit sig_cover_found(cl.cover_path());
		return true;
	}

	// For one cover, we also can use the local cover path
	if(!cl.local_paths().isEmpty() && _n_covers == 1)
	{
		emit sig_cover_found(cl.local_path(0));
		return true;
	}

	// we have to fetch the cover from the internet
	start_new_thread( cl );

	return true;
}


bool CoverLookup::fetch_album_cover(const Album& album)
{
	CoverLocation cl = CoverLocation::get_cover_location(album);
	return fetch_cover(cl);
}


void CoverLookup::finished(bool success)
{
    emit sig_finished(success);
}


void CoverLookup::cover_found(const QString& file_path)
{
	CoverFetchThread* cft = static_cast<CoverFetchThread*>(sender());
    emit sig_cover_found(file_path);

	if(!cft->more()){
		emit sig_finished(true);
	}
}

void CoverLookup::stop() {}


