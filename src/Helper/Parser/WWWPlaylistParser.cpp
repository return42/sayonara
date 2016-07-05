#include "WWWPlaylistParser.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/FileHelper.h"
#include "Helper/Helper.h"
#include "PlaylistParser.h"


WWWPlaylistParser::WWWPlaylistParser(QObject *parent) :
	QObject(parent)
{
}

void WWWPlaylistParser::start(const QString& playlist_url)
{
	QStringList urls;
	urls << playlist_url;
	start(urls);
}

void WWWPlaylistParser::start(const QStringList &playlist_urls)
{
	_v_md.clear();
	_urls = playlist_urls;

	fetch_next_url();
}

void WWWPlaylistParser::content_fetched(bool success){

	AsyncWebAccess* awa = dynamic_cast<AsyncWebAccess*>(sender());

	if(!success){
		awa->deleteLater();
		if(_urls.isEmpty()){
			emit sig_playlist_parsed(false);
		}
		else{
			fetch_next_url();
		}
		return;
	}

	QString filename = Helper::get_sayonara_path() + QDir::separator() + "tmpPlaylist";
	QByteArray arr = awa->get_data();
	awa->deleteLater();

	Helper::File::delete_files( {filename} );
	success = Helper::File::write_file(arr, filename);

	if(!success){

		if(_urls.isEmpty()){
			emit sig_playlist_parsed(false);
		}

		else{
			fetch_next_url();
		}
		return;
	}

	MetaDataList v_md;
	PlaylistParser::parse_playlist(filename, v_md);
	_v_md << v_md;
	if(_urls.isEmpty()){
		emit sig_playlist_parsed(true);
	}

	else{
		fetch_next_url();
	}
}

MetaDataList WWWPlaylistParser::get_metadata() const
{
	return _v_md;
}

void WWWPlaylistParser::fetch_next_url()
{
	QString playlist_url = _urls.takeFirst();
	AsyncWebAccess* next_awa = new AsyncWebAccess(this);
	connect(next_awa, &AsyncWebAccess::sig_finished, this, &WWWPlaylistParser::content_fetched);
	next_awa->run(playlist_url);
}
