#include "GoogleCoverFetcher.h"
#include "Helper/Logger/Logger.h"

#include <QStringList>
#include <QRegExp>
#include <QString>
#include <QUrl>

bool GoogleCoverFetcher::can_fetch_cover_directly() const
{
	return false;
}

QStringList GoogleCoverFetcher::calc_addresses_from_website(const QByteArray& website) const
{
	QString regex = "(https://encrypted-tbn.+)\"";
	QStringList addresses;

	if (website.isEmpty()) {
		sp_log(Log::Error, this) << "Website empty";
		return addresses;
	}

	int idx=500;

	QString website_str = QString::fromLocal8Bit(website);

	while(true)
	{
		QRegExp re(regex);
		re.setMinimal(true);
		idx = re.indexIn(website_str, idx);

		if(idx == -1) {
			break;
		}

		QString str = re.cap(0);

		idx += str.length();
		str.remove("\"");
		addresses << str;
	}

	sp_log(Log::Debug, this) << "Got " << addresses.size() << " Addresses";

	return addresses;
}


QString GoogleCoverFetcher::get_artist_address(const QString& artist) const
{
	return get_search_address(QUrl::toPercentEncoding(artist));
}

QString GoogleCoverFetcher::get_album_address(const QString& artist, const QString& album) const
{
	QString new_album, searchstring;
	QRegExp regex;

	searchstring = QUrl::toPercentEncoding(artist);
	new_album = album;

	regex = QRegExp(QString("(\\s)?-?(\\s)?((cd)|(CD)|((d|D)((is)|(IS))(c|C|k|K)))(\\d|(\\s\\d))"));

	new_album = new_album.toLower();
	new_album = new_album.remove(regex);
	new_album = new_album.replace("()", "");
	new_album = new_album.replace("( )", "");
	new_album = new_album.trimmed();
	new_album = QUrl::toPercentEncoding(album);

	if(searchstring.size() > 0) {
		searchstring += "+";
	}

	searchstring += new_album;

	return get_search_address(searchstring);
}

QString GoogleCoverFetcher::get_search_address(const QString& str) const
{
	QString searchstring = str;
	searchstring.replace(" ", "%20");
	searchstring.replace("/", "%2F");
	searchstring.replace("&", "%26");
	searchstring.replace("$", "%24");

	QString url = QString("https://www.google.de/search?num=20&hl=de&site=imghp&tbm=isch&source=hp");

	url += QString("&q=") + searchstring;
	url += QString("&oq=") + searchstring;

	return url;
}

bool GoogleCoverFetcher::is_search_supported() const
{
	return true;
}

bool GoogleCoverFetcher::is_album_supported() const
{
	return true;
}

bool GoogleCoverFetcher::is_artist_supported() const
{
	return true;
}


int GoogleCoverFetcher::get_estimated_size() const
{
	return 150;
}


QString GoogleCoverFetcher::get_keyword() const
{
	return "google";
}
