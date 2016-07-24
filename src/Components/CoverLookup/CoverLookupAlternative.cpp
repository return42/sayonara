/* CoverLookupAlternative.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
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

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, int n_covers) :
    CoverLookupInterface(parent)
{
	_run = true;
    _n_covers = n_covers;
}

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const QString& album_name, const QString& artist_name, int n_covers) :
    CoverLookupAlternative(parent, n_covers)
{
    _album_name = album_name;
    _artist_name = artist_name;
	_search_type = SearchType::Standard;
}

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const QString& album_name, const QStringList& artists_name, int n_covers) :
    CoverLookupAlternative(parent, n_covers)
{
	_album_name = album_name;
    _artists_name = artists_name;
	_search_type = SearchType::Sampler;
}

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const Album& album, int n_covers) :
	CoverLookupAlternative(parent, n_covers)
{
	_album = album;
	_search_type = SearchType::ByAlbum;
}


CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const QString& artist_name, int n_covers)  :
    CoverLookupAlternative(parent, n_covers)
{
    _artist_name = artist_name;
	_search_type = SearchType::ByArtistName;
}


CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const Artist& artist, int n_covers) :
    CoverLookupAlternative(parent, n_covers)
{
    _artist = artist;
	_search_type = SearchType::ByArtist;
}


CoverLookupAlternative::~CoverLookupAlternative() {
	_cl->stop();
}

void CoverLookupAlternative::stop() {
    _cl->stop();
}

void CoverLookupAlternative::start() {

    _run = true;

	_cl = CoverLookupPtr(new CoverLookup(this, _n_covers));

	connect(_cl.get(), &CoverLookup::sig_cover_found, this, &CoverLookupAlternative::cover_found);
	connect(_cl.get(), &CoverLookup::sig_finished, this, &CoverLookupAlternative::finished);

    switch(_search_type) {

		case SearchType::Standard:
            _cl->fetch_album_cover_standard(_artist_name, _album_name);
            break;

		case SearchType::Sampler:
            _cl->fetch_album_cover_sampler(_artists_name, _album_name);
            break;

		case SearchType::ByAlbum:
            _cl->fetch_album_cover(_album);
            break;

		case SearchType::ByArtistName:
            _cl->fetch_artist_cover_standard(_artist_name);

            break;

		case SearchType::ByArtist:
            _cl->fetch_artist_cover(_artist);
            break;

		default: // will never be reached
            break;
    }
}

void CoverLookupAlternative::cover_found(const CoverLocation& cover_path)
{
    emit sig_cover_found(cover_path);
}

void CoverLookupAlternative::finished(bool success)
{
    emit sig_finished(success);
}


