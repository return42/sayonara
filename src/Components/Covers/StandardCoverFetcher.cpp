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
