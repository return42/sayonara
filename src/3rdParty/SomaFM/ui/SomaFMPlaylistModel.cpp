/* SomaFMPlaylistModel.cpp */

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

/* SomaFMPlaylistModel.cpp */

#include "SomaFMPlaylistModel.h"
#include "Components/Covers/CoverLocation.h"
#include "GUI/Helper/CustomMimeData.h"
#include "Helper/globals.h"

#include <QUrl>

SomaFM::PlaylistModel::PlaylistModel(QObject* parent) :
    QStringListModel(parent) {}

SomaFM::PlaylistModel::~PlaylistModel() {}


void SomaFM::PlaylistModel::setStation(const SomaFM::Station& station)
{
    _station = station;

	QStringList urls = station.urls();
    QStringList entries;

    for(QString& url : urls){
		SomaFM::Station::UrlType type = station.url_type(url);
		if(type == SomaFM::Station::UrlType::MP3){
			entries << station.name() + " (mp3)";
	    }

		else if(type == SomaFM::Station::UrlType::AAC){
			entries << station.name() + " (aac)";
	    }

	    else{
		    entries << url;
	    }
    }

    this->setStringList(entries);
}

QMimeData* SomaFM::PlaylistModel::mimeData(const QModelIndexList& indexes) const
{
    if(indexes.isEmpty()){
	return nullptr;
    }

	int row = indexes.first().row();

	QStringList urls = _station.urls();
	if(!between(row, urls)){
		return nullptr;
    }

    QUrl url( urls[row] );

    QMimeData* mime_data = new QMimeData();
	CoverLocation location = _station.cover_location();

    mime_data->setUrls({url});
	if(!location.search_urls().isEmpty()){
		mime_data->setText(location.search_urls().first());
	}

	else{
		mime_data->setText("");
	}

    return mime_data;
}
