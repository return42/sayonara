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
