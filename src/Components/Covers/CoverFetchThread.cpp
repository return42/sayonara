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
#include "CoverFetchManager.h"
#include "CoverFetcherInterface.h"

#include "Helper/Logger/Logger.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/FileHelper.h"

#include <QImage>
#include <QStringList>

struct CoverFetchThread::Private
{
	AsyncWebAccess*		single_img_fetcher=nullptr;
	AsyncWebAccess*		multi_img_fetcher=nullptr;
	AsyncWebAccess*		content_fetcher=nullptr;

	CoverLocation		cl;
	CoverFetcherInterface* acf=nullptr;

	QString				url;
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
	_m->cl = cl;
}

CoverFetchThread::~CoverFetchThread() {}

bool CoverFetchThread::start()
{
	if(_m->cl.has_search_urls()){
		_m->url = _m->cl.search_urls().first();
		_m->cl.remove_first_search_url();
	}

	else {
		return false;
	}

	CoverFetchManager* cfm = CoverFetchManager::getInstance();
	_m->acf = cfm->get_coverfetcher(_m->url);

	if(!_m->acf){
		return false;
	}

	if( _m->acf->can_fetch_cover_directly() )
	{
		_m->addresses.clear();
		_m->addresses << _m->url;

		more();
	}

	else
	{
		AsyncWebAccess* awa = new AsyncWebAccess(this);
		awa->setObjectName(_m->acf->get_keyword());
		awa->set_behavior(AsyncWebAccess::Behavior::AsSayonara);
		connect(awa, &AsyncWebAccess::sig_finished, this, &CoverFetchThread::content_fetched);
		awa->run(_m->url, 5000);
	}

	return true;
}


bool CoverFetchThread::more()
{
	// we have all our covers
	if(_m->n_covers == _m->n_covers_found){
		emit sig_finished(true);
		return true;
	}

	// we have no more addresses and not all our covers
	if(_m->addresses.isEmpty()) {
		bool success = start();

		if(!success){
			emit sig_finished(false);
		}

		return false;
	}

	QString address = _m->addresses.takeFirst();
	AsyncWebAccess* awa = new AsyncWebAccess(this);
	awa->set_behavior(AsyncWebAccess::Behavior::AsSayonara);

	if(_m->n_covers == 1) {
		connect(awa, &AsyncWebAccess::sig_finished, this, &CoverFetchThread::single_image_fetched);
	}

	else {
		connect(awa, &AsyncWebAccess::sig_finished, this, &CoverFetchThread::multi_image_fetched);
	}

	awa->run(address, 5000);

	return true;
}


void CoverFetchThread::content_fetched(bool success)
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());

	if(awa->objectName() == _m->acf->get_keyword()){
		if(success) {
			QByteArray website = awa->get_data();
			_m->addresses = _m->acf->calc_addresses_from_website(website);
		}

		else {
			sp_log(Log::Warning, this) << "Could not fetch content from " << _m->acf->get_keyword();
		}
	}

	awa->deleteLater();
	more();
}

void CoverFetchThread::single_image_fetched(bool success)
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());

	if(success) {
		QImage img  = awa->get_image();

		if(!img.isNull()) {
			QString target_file = _m->cl.cover_path();
			_m->n_covers_found++;
			save_and_emit_image(target_file, img);
		}
	}

	else {
		sp_log(Log::Warning, this) << "Could not fetch cover from " << _m->acf->get_keyword();
	}

	awa->deleteLater();
}


void
CoverFetchThread::multi_image_fetched(bool success)
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());

	if(success){

		QImage img  = awa->get_image();

		if(!img.isNull()){

			QString filename, dir, cover_path;
			QString target_file = _m->cl.cover_path();
			Helper::File::split_filename(target_file, dir, filename);

			cover_path = dir + "/" + QString::number(_m->n_covers_found) + "_" + filename;
			save_and_emit_image(cover_path, img);

			_m->n_covers_found++;
		}
	}

	else {
		sp_log(Log::Warning, this) << "Could not fetch multi cover " << _m->acf->get_keyword();
	}

	awa->deleteLater();
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
		sp_log(Log::Warning, this) << "Cannot save image to " << filename;
	}

	else{
		emit sig_cover_found(filename);
	}
}
