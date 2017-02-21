/* CoverFetchThread.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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


/*
 * CoverFetchThread.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: Lucio Carreras
 */

#include "CoverFetchThread.h"
#include "CoverLocation.h"

#include "Helper/Logger/Logger.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/FileHelper.h"

#include <QFile>
#include <QDir>
#include <QImage>
#include <QStringList>

struct CoverFetchThread::Private
{
	AsyncWebAccess*		single_img_fetcher=nullptr;
	AsyncWebAccess*		multi_img_fetcher=nullptr;
	AsyncWebAccess*		content_fetcher=nullptr;

	QString				url;
	QString				target_file;
	QStringList			addresses;
	int					n_covers;
	int					n_covers_found;

	Private()
	{
		n_covers_found = 0;
	}
};

CoverFetchThread::CoverFetchThread() {}

CoverFetchThread::CoverFetchThread(QObject* parent, const CoverLocation& cl, const int n_covers) :
	QObject(parent)
{
	_m = Pimpl::make<Private>();

	_m->n_covers = n_covers;
	_m->url = QString();

	if(cl.has_search_urls()){
		_m->url = cl.search_urls().first();
	}

	_m->target_file = cl.cover_path();
}


CoverFetchThread::~CoverFetchThread() {}

bool CoverFetchThread::start()
{
	_m->n_covers_found = 0;

	if( can_fetch_cover_directly() )
	{
		_m->addresses.clear();
		_m->addresses << _m->url;

		more();
	}

	else
	{
		AsyncWebAccess* awa = new AsyncWebAccess(this);
		awa->set_behavior(AsyncWebAccess::Behavior::AsSayonara);
		connect(awa, &AsyncWebAccess::sig_finished, this, &CoverFetchThread::content_fetched);

		sp_log(Log::Debug) << "Try to fetch cover from " << _m->url;
		awa->run(_m->url, 3000);
	}

	return true;
}


bool
CoverFetchThread::more()
{
	if(_m->n_covers == _m->n_covers_found || _m->addresses.isEmpty()){
		emit sig_finished(true);
		return false;
	}

	QString address = _m->addresses.takeFirst();
	AsyncWebAccess* awa = new AsyncWebAccess(this);
	awa->set_behavior(AsyncWebAccess::Behavior::AsSayonara);

	if(_m->n_covers == 1){
		connect(awa, &AsyncWebAccess::sig_finished, this, &CoverFetchThread::single_image_fetched);
	}

	else{
		connect(awa, &AsyncWebAccess::sig_finished, this, &CoverFetchThread::multi_image_fetched);
	}

	sp_log(Log::Debug) << "2 Try to fetch cover from " << address;
	awa->run(address, 10000);

	return true;
}


void
CoverFetchThread::content_fetched(bool success)
{
	if(!success){
		sp_log(Log::Warning) << "Could not fetch content";
		return;
	}

	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	QByteArray website = awa->get_data();

	_m->addresses = calc_addresses_from_website(website, 10);

	if(_m->addresses.isEmpty())
	{
		emit sig_finished(false);
		return;
	}

	more();
}


void
CoverFetchThread::single_image_fetched(bool success)
{
	if(!success){
		sp_log(Log::Warning) << "Could not fetch cover";
		return;
	}

	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	QImage img  = awa->get_image();

	if(!img.isNull()){
		_m->n_covers_found++;
		save_and_emit_image(_m->target_file, img);
	}

	awa->deleteLater();
}


void
CoverFetchThread::multi_image_fetched(bool success)
{
	if(!success){
		sp_log(Log::Warning) << "Could not fetch cover";
		return;
	}

	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	QImage img  = awa->get_image();
	awa->deleteLater();

	if(img.isNull()){
		return;
	}

	QString filename, dir;
	Helper::File::split_filename(_m->target_file, dir, filename);
	QString cover_path = dir + "/" + QString::number(_m->n_covers_found) + "_" + filename;
	save_and_emit_image(cover_path, img);

	_m->n_covers_found++;
}


void
CoverFetchThread::save_and_emit_image(const QString& filepath, const QImage& img)
{
	QString filename = filepath;
	QString ext = Helper::File::calc_file_extension(filepath);
	if(ext.compare("gif", Qt::CaseInsensitive) == 0){
		filename = filename.left(filename.size() - 3);
		filename += "png";
	}

	filename = Helper::File::get_absolute_filename(filename);

	bool success = img.save(filename);
	if(!success){
		sp_log(Log::Warning) << "Cannot save image to " << filename;
	}

	else{
		emit sig_cover_found(filename);
	}
}
