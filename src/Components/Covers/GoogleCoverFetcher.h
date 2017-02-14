#ifndef GOOGLECOVERFETCHER_H
#define GOOGLECOVERFETCHER_H


#include "CoverFetchThread.h"

class GoogleCoverFetcher :
	public CoverFetchThread
{

public:
	using CoverFetchThread::CoverFetchThread;


// CoverFetchThread interface
public:
	bool can_fetch_cover_directly() const override;
	QStringList calc_addresses_from_website(const QByteArray& website) const override;

	QString get_artist_address(const QString& artist) const override;
	QString get_album_address(const QString& artist, const QString& album) const override;
	QString get_search_address(const QString& str) const override;
	bool is_search_supported() const override;
	bool is_album_supported() const override;
	bool is_artist_supported() const override;

	int get_estimated_size() const override;
	QString get_unique_url_identifier() const override;
};

#endif // GOOGLECOVERFETCHER_H
