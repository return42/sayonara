#ifndef DISCOGSCOVERFETCHER_H
#define DISCOGSCOVERFETCHER_H

#include "CoverFetchThread.h"
class DiscogsCoverFetcher :
		public CoverFetchThread
{
	using CoverFetchThread::CoverFetchThread;



	// CoverFetchThread interface
public:
	virtual bool can_fetch_cover_directly() const override;
	virtual QStringList calc_addresses_from_website(const QByteArray& website, int n_covers) const override;
	virtual QString get_artist_address(const QString& artist) const override;
	virtual QString get_album_address(const QString& artist, const QString& album) const override;
	virtual QString get_search_address(const QString& str) const override;
	virtual bool is_search_supported() const override;
	virtual bool is_album_supported() const override;
	virtual bool is_artist_supported() const override;
	virtual int get_estimated_size() const override;
};

#endif // DISCOGSCOVERFETCHER_H
