#include "AlbumCoverFetchThread.h"
#include "Helper/MetaData/Album.h"
#include "Components/Covers/CoverLookup.h"
#include "Components/Covers/CoverLocation.h"
#include "Helper/Helper.h"
#include "Helper/Logger/Logger.h"

#include <atomic>
#include <mutex>


struct AlbumCoverFetchThread::Private
{
	QStringList hashes;
	QList<CoverLocation> cover_locations;
	bool may_run;
	std::atomic<bool> goon;
	std::mutex mutex;

	QString current_hash;
	CoverLocation current_cl;

	Private()
	{
		may_run = true;
		goon = true;
	}
};


AlbumCoverFetchThread::AlbumCoverFetchThread(QObject* parent) :
	QThread(parent)
{
	_m = Pimpl::make<Private>();
}

AlbumCoverFetchThread::~AlbumCoverFetchThread() {}


void AlbumCoverFetchThread::run()
{
	while(_m->may_run) {

		while(_m->hashes.isEmpty() || !_m->goon) {
			Helper::sleep_ms(100);
		}

		_m->goon = false;

		if(!_m->may_run){
			break;
		}

		std::lock_guard<std::mutex> guard(_m->mutex);

		_m->current_hash = _m->hashes.takeFirst();
		_m->current_cl = _m->cover_locations.takeFirst();

		emit sig_next(_m->current_hash, _m->current_cl);
	}
}


void AlbumCoverFetchThread::add_data(const QString& hash, const CoverLocation& cl)
{
	if(!_m->hashes.contains(hash)) {

		std::lock_guard<std::mutex> guard(_m->mutex);

		_m->hashes.push_front(hash);
		_m->cover_locations.push_front(cl);
	}
}

void AlbumCoverFetchThread::done(bool success)
{
	Q_UNUSED(success)
	/*if(!success) {

		std::lock_guard<std::mutex> guard(_m->mutex);

		_m->current_cl.remove_first_search_url();

		if( _m->current_cl.has_search_urls()) 
		{
			_m->hashes.push_front(_m->current_hash);
			_m->cover_locations.push_front(_m->current_cl);
		}
	}*/

	_m->goon = true;
}

