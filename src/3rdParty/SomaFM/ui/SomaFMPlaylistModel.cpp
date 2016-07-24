#include "SomaFMPlaylistModel.h"
#include "GUI/Helper/CustomMimeData.h"

SomaFMPlaylistModel::SomaFMPlaylistModel(QObject* parent) :
    QStringListModel(parent)
{

}


void SomaFMPlaylistModel::setStation(const SomaFMStation& station)
{
    _station = station;

    QStringList urls = station.get_urls();
    QStringList entries;

    for(QString& url : urls){
	    SomaFMStation::UrlType type = station.get_url_type(url);
	    if(type == SomaFMStation::UrlType::MP3){
		    entries << station.get_name() + " (mp3)";
	    }

	    else if(type == SomaFMStation::UrlType::AAC){
		    entries << station.get_name() + " (aac)";
	    }

	    else{
		    entries << url;
	    }
    }

    this->setStringList(entries);
}

QMimeData* SomaFMPlaylistModel::mimeData(const QModelIndexList& indexes) const
{
    if(indexes.isEmpty()){
	return nullptr;
    }

    int row = indexes[0].row();

    QStringList urls = _station.get_urls();
    if(!between(row, 0, urls.size())){
	return nullptr;
    }

    QUrl url( urls[row] );

    QMimeData* mime_data = new QMimeData();
	CoverLocation location = _station.get_cover_location();

    mime_data->setUrls({url});
	mime_data->setText(location.search_url);

    return mime_data;
}
