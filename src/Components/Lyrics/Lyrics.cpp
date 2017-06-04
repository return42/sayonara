#include "Lyrics.h"
#include "LyricLookup.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/Tagging/Tagging.h"
#include "Helper/Logger/Logger.h"

#include <QStringList>

struct Lyrics::Private
{
	QStringList servers;
	MetaData md;
	QString artist;
	QString title;
	QString lyrics;
	QString lyric_header;
	QString lyric_tag_content;

	bool is_valid;

	Private()
	{
		is_valid = false;

		LyricLookupThread* lyric_thread = new LyricLookupThread();
		servers = lyric_thread->servers();
		delete lyric_thread;
	}

	void guess_artist_and_title();
};

Lyrics::Lyrics(QObject* parent) :
	QObject(parent)
{
	_m = Pimpl::make<Private>();
}

Lyrics::~Lyrics() {}

bool Lyrics::fetch_lyrics(const QString& artist, const QString& title, int server_index)
{
	if(artist.isEmpty() || title.isEmpty()) {
		return false;
	}

	if(server_index < 0 || server_index >= _m->servers.size()) {
		return false;
	}

	LyricLookupThread* lyric_thread = new LyricLookupThread(this);
	connect(lyric_thread, &LyricLookupThread::sig_finished, this, &Lyrics::lyrics_fetched);

	lyric_thread->run(artist, title, server_index);
	return true;
}

bool Lyrics::save_lyrics(const QString& plain_text)
{
	if(plain_text.isEmpty()){
		return false;
	}

	if(_m->md.filepath().isEmpty()){
		return false;
	}

	bool success = Tagging::write_lyrics(_m->md, plain_text);
	if(success){
		_m->is_valid = true;
		_m->lyric_tag_content = plain_text;
	}

	return success;
}

QStringList Lyrics::servers() const
{
	return _m->servers;
}

void Lyrics::set_metadata(const MetaData& md)
{
	_m->md = md;
	_m->guess_artist_and_title();

	Tagging::extract_lyrics(md, _m->lyric_tag_content);
}

QString Lyrics::artist() const
{
	return _m->artist;
}

QString Lyrics::title() const
{
	return _m->title;
}

QString Lyrics::lyric_header() const
{
	return _m->lyric_header;
}

QString Lyrics::local_lyric_header() const
{
	return "<b>" + artist() + " - " + title() + "</b>";
}

QString Lyrics::lyrics() const
{
	return _m->lyrics.trimmed();
}

QString Lyrics::local_lyrics() const
{
	if(is_lyric_tag_available()){
		return _m->lyric_tag_content.trimmed();
	}

	return QString();
}

bool Lyrics::is_lyric_valid() const
{
	return _m->is_valid;
}

bool Lyrics::is_lyric_tag_available() const
{
	return (!_m->lyric_tag_content.isEmpty());
}

bool Lyrics::is_lyric_tag_supported() const
{
	return Tagging::is_lyrics_supported(_m->md.filepath());
}

void Lyrics::lyrics_fetched()
{
	LyricLookupThread* lyric_thread = static_cast<LyricLookupThread*>(sender());

	_m->lyrics = lyric_thread->lyric_data();
	_m->lyric_header = lyric_thread->lyric_header();
	_m->is_valid = (!lyric_thread->has_error());

	lyric_thread->deleteLater();

	emit sig_lyrics_fetched();
}


void Lyrics::Private::guess_artist_and_title()
{
	bool guessed = false;

	if(	md.radio_mode() == RadioMode::Station &&
		md.artist.contains("://"))
	{
		if(md.title.contains("-")){
			QStringList lst = md.title.split("-");
			artist = lst.takeFirst().trimmed();
			title = lst.join("-").trimmed();
			guessed = true;
		}

		else if(md.title.contains(":")){
			QStringList lst = md.title.split(":");
			artist = lst.takeFirst().trimmed();
			title = lst.join(":").trimmed();
			guessed = true;
		}
	}

	if(guessed == false) {
		if(!md.artist.isEmpty()) {
			artist = md.artist;
			title = md.title;
		}

		else if(!md.album_artist().isEmpty()) {
			artist = md.album_artist();
			title = md.title;
		}

		else {
			artist = md.artist;
			title = md.title;
		}
	}
}

