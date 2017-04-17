/* AlbumCoverFetchThread.cpp */

/* Copyright (C) 2011-2017  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



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
		init();
	}

	void init()
	{
		may_run = true;
		goon = true;
		hashes.clear();
		cover_locations.clear();
		current_hash = QString();
		current_cl = CoverLocation();
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
	_m->init();
	const int PauseBetweenRequests = 300;
	const int NumParallelRequests = 10;

	while(_m->may_run) {

		while(_m->hashes.isEmpty() || !_m->goon) {
			Helper::sleep_ms(PauseBetweenRequests);
		}

		_m->goon = false;

		if(!_m->may_run){
			break;
		}

		for(int i=0; i<NumParallelRequests; i++){
			if(_m->hashes.isEmpty()) {
				break;
			}

			if(i > 0) {
				Helper::sleep_ms(PauseBetweenRequests);
			}

			try{
				std::lock_guard<std::mutex> guard(_m->mutex);
				_m->current_hash = _m->hashes.takeFirst();
				_m->current_cl = _m->cover_locations.takeFirst();

				emit sig_next();

			} catch(std::exception* e) {
				sp_log(Log::Warning, this) << "1 Exception" << e->what();
				Helper::sleep_ms(PauseBetweenRequests);
			}
		}
	}
}


void AlbumCoverFetchThread::add_data(const QString& hash, const CoverLocation& cl)
{
	if(!_m->hashes.contains(hash) && (_m->current_hash.compare(hash) != 0))
	{
		bool done = false;
		while(!done) {
			try {
				std::lock_guard<std::mutex> guard(_m->mutex);
				_m->hashes.push_front(hash);
				_m->cover_locations.push_front(cl);
				done = true;

			} catch(std::exception* e) {
				sp_log(Log::Warning, this) << "2 Exception" << e->what();
				Helper::sleep_ms(10);
			}
		}
	}
}


QString AlbumCoverFetchThread::current_hash() const
{
	return _m->current_hash;
}

CoverLocation AlbumCoverFetchThread::current_cover_location() const
{
	return _m->current_cl;
}


void AlbumCoverFetchThread::done(bool success)
{
	Q_UNUSED(success)

	_m->goon = true;
}

void AlbumCoverFetchThread::stop()
{
	_m->may_run = false;
}

