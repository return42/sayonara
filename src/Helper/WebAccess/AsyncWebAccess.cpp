/* AsyncWebAccess.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
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
#include <QRegExp>

AsyncWebAccess::AsyncWebAccess(QObject* parent, const QByteArray& header) :
	QObject(parent)
{
	Q_UNUSED(header)
	_nam = new QNetworkAccessManager(this);
	_timer = new QTimer();

	connect(_timer, &QTimer::timeout, this, &AsyncWebAccess::timeout);
	connect(_nam, &QNetworkAccessManager::finished, this, &AsyncWebAccess::finished);
}

AsyncWebAccess::~AsyncWebAccess() {
}


void AsyncWebAccess::set_raw_header(const QMap<QByteArray, QByteArray>& header){

	_header = header;
}

void AsyncWebAccess::run(const QString& url, int timeout){
	_header.clear();
	_img = QImage();
	_data.clear();
	_url = url;
	_nam->clearAccessCache();
	QRegExp re("(itpc|feed)://");
	if(re.indexIn(url) >= 0){
		_url.replace(re, "http://");
	}

	QNetworkRequest request;
	request.setUrl(_url);

	_reply = _nam->get(request);
	_timer->start(timeout);
}

void AsyncWebAccess::run_post(const QString &url, const QByteArray &post_data, int timeout){
	_header.clear();
	_img = QImage();
	_data.clear();
	_url = url;
	_nam->clearAccessCache();

	QUrl my_url(url);
	QNetworkRequest request(my_url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/x-www-form-urlencoded"));

	if(!_header.isEmpty()){
		for(const QByteArray& key : _header.keys()){
			request.setRawHeader(key, _header[key]);

		}
	}

	_reply = _nam->post(request, post_data);
	_timer->start(timeout);
}


void AsyncWebAccess::finished(QNetworkReply *reply){

	QNetworkReply::NetworkError error = reply->error();
	bool success = (error == QNetworkReply::NoError);
	QString redirect_url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();

	if(!redirect_url.isEmpty() && redirect_url != _url){

		QUrl url(_url);

		if(redirect_url.startsWith("/")){
			redirect_url.prepend(url.scheme() + "://" + url.host());
		}

		_url = redirect_url;
		run(redirect_url);
		return;
	}

	if(success){
		_data = reply->readAll();
	}

	else {
		_data = reply->readAll();
		sp_log(Log::Warning) << reply->errorString();
	}

	emit sig_finished(success);
}

void AsyncWebAccess::timeout(){

	if(_reply->bytesAvailable() > 0){
		emit sig_finished( true );
	}

	else{
		_reply->abort();
	}

	_timer->stop();

}


QByteArray AsyncWebAccess::get_data() const
{
    return _data;
}

QImage AsyncWebAccess::get_image() const
{
	QImage img;
	img.loadFromData(_data);
	return img;
}

QString AsyncWebAccess::get_url() const
{
	return _url;
}

QString AsyncWebAccess::get_url_filename() const
{
	QUrl url(_url);
	return url.fileName();
}

QString AsyncWebAccess::get_url_hostname() const
{
	QUrl url(_url);
	return url.host();
}

QString AsyncWebAccess::get_url_protocol() const
{
	QString url_str = _url;
	int idx = url_str.indexOf(get_url_hostname());
	if(idx < 0){
		return "";
	}

	return _url.left(idx);

}

QString AsyncWebAccess::get_url_wo_file() const{
	QString url_str = _url;
	int idx = url_str.lastIndexOf(get_url_filename());
	if(idx < 0){
		return "";
	}

	return _url.left(idx + 1);
}

