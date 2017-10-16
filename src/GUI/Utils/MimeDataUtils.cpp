/* MimeDataUtils.cpp */

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



#include "MimeDataUtils.h"

#include "Components/DirectoryReader/DirectoryReader.h"

#include "GUI/Utils/CustomMimeData.h"

#include "Utils/MetaData/MetaDataList.h"
#include "Utils/Utils.h"
#include "Utils/FileUtils.h"

#include <QStringList>
#include <QUrl>

#include <algorithm>

using namespace Gui::Util;

MetaDataList MimeData::get_metadata(const QMimeData *data)
{
    if(!data){
        return MetaDataList();
    }

    const CustomMimeData* cmd = dynamic_cast<const CustomMimeData*>(data);
	if(cmd)
	{
		return cmd->metadata();
	}

	if(data->hasUrls())
	{
        MetaDataList v_md;
		DirectoryReader reader;
		reader.set_filter(::Util::soundfile_extensions());

        for(const QUrl& url : data->urls())
		{
			if(url.isLocalFile())
			{
				QStringList file_list;
				file_list << url.toLocalFile();
				MetaDataList v_md_tmp = reader.get_md_from_filelist(file_list);
				v_md << std::move(v_md_tmp);
			}
		}

		return v_md;
	}

	return MetaDataList();
}

QStringList MimeData::get_playlists(const QMimeData *data)
{
    if(!data){
        return QStringList();
    }

	if(!data->hasUrls()){
		return QStringList();
	}

	QStringList www_playlists;

	for(const QUrl& url : data->urls())
	{
		if(::Util::File::is_playlistfile(url.toString())){
			www_playlists << url.toString();
		}
	}

	return www_playlists;
}


void MimeData::set_inner_drag_drop(QMimeData* data)
{
    if(!data){
        return;
    }

    data->setProperty("inner", QString("true"));
}

bool MimeData::is_inner_drag_drop(const QMimeData *data)
{
    if(!data){
        return false;
    }

    QString src = data->property("inner").toString();
    return (src.size() > 0);
}

QString MimeData::cover_url(const QMimeData *data)
{
    if(!data){
        return QString();
    }

    return data->property("cover_url").toString();
}

void MimeData::set_cover_url(QMimeData *data, const QString &url)
{
    if(!data){
        return;
    }

    data->setProperty("cover_url", url);
}
