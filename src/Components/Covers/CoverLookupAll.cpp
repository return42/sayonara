/* CoverLookupAll.cpp */

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

#include "CoverLookupAll.h"
#include "CoverLocation.h"
#include "Helper/MetaData/Album.h"
#include "Helper/Helper.h"

struct CoverLookupAll::Private
{
	CoverLookup*	cl=nullptr;
	AlbumList		album_list;
	bool			run;

	~Private()
	{
		if(cl){
			cl->stop();
		}
	}
};


CoverLookupAll::CoverLookupAll(QObject* parent, const AlbumList& album_list) :
	CoverLookupInterface(parent)
{
	_m = Pimpl::make<CoverLookupAll::Private>();

	_m->album_list = album_list;
	_m->run = true;
	_m->cl = new CoverLookup(this);

	connect(_m->cl, &CoverLookup::sig_cover_found, this, &CoverLookupAll::cover_found);
	connect(_m->cl, &CoverLookup::sig_finished, this, &CoverLookupAll::finished);
}


CoverLookupAll::~CoverLookupAll() {}

void CoverLookupAll::start()
{
	Album album = _m->album_list.back();
	_m->cl->fetch_album_cover(album);
}


void CoverLookupAll::stop()
{
	_m->run = false;
	_m->cl->stop();
}


void CoverLookupAll::cover_found(const QString& cover_path) {
	_m->album_list.pop_back();
	emit sig_cover_found(cover_path);

	if(!_m->run) {
		return;
	}

    // Google and other webservices block, if looking too fast
	Helper::sleep_ms(1000);

	Album album = _m->album_list.back();
	_m->cl->fetch_album_cover(album);
}

void CoverLookupAll::finished(bool success) {
    emit sig_finished(success);
}
