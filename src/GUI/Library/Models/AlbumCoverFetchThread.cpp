#include "AlbumCoverFetchThread.h"
#include "Helper/MetaData/Album.h"
#include "Components/Covers/CoverLookup.h"
#include "Components/Covers/CoverLocation.h"
#include "Helper/Helper.h"
#include "Helper/Logger/Logger.h"

#include <atomic>


struct AlbumCoverFetchThread::Private
{
    QStringList hashes;
    QList<CoverLocation> cover_locations;
    bool may_run;
    std::atomic<bool> goon;

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
    while(_m->may_run){

	while(_m->hashes.isEmpty() || !_m->goon){
	    Helper::sleep_ms(100);
	}

	_m->goon = false;

	if(!_m->may_run){
	    break;
	}

	emit sig_next(_m->hashes.first(), _m->cover_locations.first());
    }
}


void AlbumCoverFetchThread::add_data(const QString& hash, const CoverLocation& cl)
{
    if(!_m->hashes.contains(hash)){
	_m->hashes.push_front(hash);
	_m->cover_locations.push_front(cl);
    }
}

void AlbumCoverFetchThread::done(bool success)
{
    if(!success){
	QList<CoverLocation> cls;

	CoverLocation& cl = _m->cover_locations.first();
	cl.remove_first_search_url();

	if(!cl.has_search_urls()){
	    _m->hashes.removeFirst();
	    _m->cover_locations.removeFirst();
	}
    }

    else if(!_m->hashes.isEmpty()){
	_m->hashes.removeFirst();
	_m->cover_locations.removeFirst();
    }

    _m->goon = true;
}

