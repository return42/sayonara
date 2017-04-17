/* StandardCoverFetcher.cpp */

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



#include "StandardCoverFetcher.h"
#include <QStringList>

bool StandardCoverFetcher::can_fetch_cover_directly() const
{
    return true;
}

QStringList StandardCoverFetcher::calc_addresses_from_website(const QByteArray& website) const
{
	Q_UNUSED(website)
    return QStringList();
}


QString StandardCoverFetcher::get_artist_address(const QString& artist) const
{
	Q_UNUSED(artist)
	return QString();
}

QString StandardCoverFetcher::get_album_address(const QString& artist, const QString& album) const
{
	Q_UNUSED(artist)
	Q_UNUSED(album)
	return QString();
}

QString StandardCoverFetcher::get_search_address(const QString& str) const
{
	Q_UNUSED(str)
	return QString();
}

bool StandardCoverFetcher::is_search_supported() const
{
	return false;
}

bool StandardCoverFetcher::is_album_supported() const
{
	return false;
}

bool StandardCoverFetcher::is_artist_supported() const
{
	return false;
}


int StandardCoverFetcher::get_estimated_size() const
{
	return -1;
}

QString StandardCoverFetcher::get_keyword() const
{
	return QString();
}
