#include "MimeDataHelper.h"

#include "GUI/Helper/CustomMimeData.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"

#include <QStringList>
#include <QUrl>

#include <algorithm>


MetaDataList GUI::MimeData::get_metadata(const QMimeData *data)
{
    if(!data){
        return MetaDataList();
    }

	const CustomMimeData* cmd = cast(data);
	if(cmd)
	{
		return cmd->metadata();
	}

	if(data->hasUrls())
	{
        MetaDataList v_md;
		DirectoryReader reader;
		reader.set_filter(Helper::soundfile_extensions());

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

QStringList GUI::MimeData::get_playlists(const QMimeData *data)
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
		if(Helper::File::is_playlistfile(url.toString())){
			www_playlists << url.toString();
		}
	}

	return www_playlists;
}

const CustomMimeData* GUI::MimeData::cast(const QMimeData *data)
{
	return dynamic_cast<const CustomMimeData*>(data);
}

void GUI::MimeData::set_inner_drag_drop(QMimeData* data)
{
    if(!data){
        return;
    }

    data->setProperty("inner", QString("true"));
}

bool GUI::MimeData::is_inner_drag_drop(const QMimeData *data)
{
    if(!data){
        return false;
    }

    QString src = data->property("inner").toString();
    return (src.size() > 0);
}

QString GUI::MimeData::cover_url(const QMimeData *data)
{
    if(!data){
        return QString();
    }

    return data->property("cover_url").toString();
}

void GUI::MimeData::set_cover_url(QMimeData *data, const QString &url)
{
    if(!data){
        return;
    }

    data->setProperty("cover_url", url);
}
