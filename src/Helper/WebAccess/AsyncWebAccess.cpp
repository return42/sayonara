/* AsyncWebAccess.cpp */

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


#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Helper.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QImage>
#include <QTimer>
#include <QRegExp>

struct AsyncWebAccess::Private
{
	QNetworkAccessManager*	nam=nullptr;
	QTimer*					timer=nullptr;
	QNetworkReply*			reply=nullptr;

	QString					url;
	QByteArray				data;
	AsyncWebAccess::Behavior behavior;
	QMap<QByteArray, QByteArray> header;

	void abort_request()
	{
		if(reply && reply->isRunning() ){
			reply->abort();
			reply->close();

			sp_log(Log::Warning) << "Request was stopped: " << url;
		}

		if(timer){
			timer->stop();
		}
	}
};

AsyncWebAccess::AsyncWebAccess(QObject* parent, const QByteArray& header, AsyncWebAccess::Behavior behavior) :
	QObject(parent)
{
	Q_UNUSED(header)

	_m = Pimpl::make<Private>();
	_m->nam = new QNetworkAccessManager(this);
	_m->timer = new QTimer();
	_m->behavior = behavior;

	connect(_m->timer, &QTimer::timeout, this, &AsyncWebAccess::timeout);
	connect(_m->nam, &QNetworkAccessManager::finished, this, &AsyncWebAccess::finished);
}

AsyncWebAccess::~AsyncWebAccess() {}

void AsyncWebAccess::run(const QString& url, int timeout)
{
	_m->header.clear();
	_m->data.clear();
	_m->url = url;
	_m->nam->clearAccessCache();

	QRegExp re("(itpc|feed)://");
	if(re.indexIn(url) >= 0){
		_m->url.replace(re, "http://");
	}

	QNetworkRequest request;
	request.setUrl(_m->url);
	QString user_agent;

	switch(_m->behavior){
		case AsyncWebAccess::Behavior::AsSayonara:
			user_agent = "sayonara";
			break;

		case AsyncWebAccess::Behavior::AsBrowser:
			user_agent = "Mozilla/5.0 (Linux; rv:35.0) Gecko/20100101 Firefox/35.0";
			break;

		case AsyncWebAccess::Behavior::Random:
			user_agent = Helper::get_random_string(Helper::get_random_number(8, 16));
			break;

		default:
			break;
	}

	request.setHeader(QNetworkRequest::UserAgentHeader, user_agent);

	_m->reply = _m->nam->get(request);
	if(timeout > 0){
		_m->timer->start(timeout);
	}
}

void AsyncWebAccess::run_post(const QString &url, const QByteArray &post_data, int timeout)
{
	_m->header.clear();
	_m->data.clear();
	_m->url = url;
	_m->nam->clearAccessCache();

	QUrl my_url(url);
	QNetworkRequest request(my_url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/x-www-form-urlencoded"));

	if(!_m->header.isEmpty()){
		for(const QByteArray& key : _m->header.keys()){
			request.setRawHeader(key, _m->header[key]);
		}
	}

	_m->reply = _m->nam->post(request, post_data);
	_m->timer->start(timeout);
}


void AsyncWebAccess::finished(QNetworkReply *reply)
{
	QNetworkReply::NetworkError error = reply->error();
	sp_log(Log::Debug, this) << "Finished " << _m->url << " after " << _m->timer->interval() - _m->timer->remainingTime();

	bool success = (error == QNetworkReply::NoError);
	if(!success){
		sp_log(Log::Warning, this) << "Cannot open " << _m->url << ": " << reply->errorString();
		return;
	}

	QString redirect_url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();

	if(!redirect_url.isEmpty() && redirect_url != _m->url) {


		QUrl url(_m->url);

		if(redirect_url.startsWith("/")){
			redirect_url.prepend(url.scheme() + "://" + url.host());
		}

		_m->abort_request();
		_m->url = redirect_url;

		sp_log(Log::Debug, this) << "Redirect: " << _m->url;
		run(redirect_url);

		return;
	}

	bool is_readable = reply->isReadable();
	qint64 bytes_available = reply->bytesAvailable();

	if( success &&
		is_readable &&
		bytes_available > 0)
	{
		_m->data = reply->readAll();
	}

	else if(is_readable)
	{
		success = true;
		_m->data.clear();
	}

	else {
		success = false;
		_m->data.clear();
	}

	_m->abort_request();
	_m->timer->stop();

	emit sig_finished(success);
}

void AsyncWebAccess::timeout()
{
	if(!_m->reply->isOpen()){
		_m->abort_request();
		return;
	}

	if(_m->reply->bytesAvailable() <= 0) {
		_m->abort_request();
		emit sig_finished(false);
	}

	else{
		//emit sig_finished( true );
	}

	_m->timer->stop();
}


QByteArray AsyncWebAccess::get_data() const
{
	return _m->data;
}

QImage AsyncWebAccess::get_image() const
{
	QImage img;
	img.loadFromData(_m->data);
	return img;
}

QString AsyncWebAccess::get_url() const
{
	return _m->url;
}

void AsyncWebAccess::set_behavior(AsyncWebAccess::Behavior behavior)
{
	_m->behavior = behavior;
}

void AsyncWebAccess::set_raw_header(const QMap<QByteArray, QByteArray>& header){
	_m->header = header;
}

