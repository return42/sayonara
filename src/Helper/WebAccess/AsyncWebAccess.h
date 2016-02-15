/* AsyncWebAccess.h */

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



#ifndef ASYNCWEBACCESS_H_
#define ASYNCWEBACCESS_H_

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QByteArray>
#include <QImage>
#include <QTimer>

enum class AwaDataType : quint8
{
	String=0,
	Image
};

class AsyncWebAccess : public QObject{

	Q_OBJECT

signals:
	void sig_finished(bool success);

public:
	AsyncWebAccess(QObject* parent=nullptr, const QByteArray& header=QByteArray());
	virtual ~AsyncWebAccess();

	QByteArray get_data() const;
	QImage get_image() const;
	QString	get_url() const;
	QString get_url_filename() const;
	QString get_url_hostname() const;
	QString get_url_protocol() const;
	QString get_url_wo_file() const;

	void run(const QString& url, int timeout=4000);
	void run_post(const QString& url, const QByteArray& post_data, int timeout=4000);
	void set_raw_header(const QMap<QByteArray, QByteArray>& header);


private slots:
	void finished(QNetworkReply* reply);
	void timeout();

private:

	QNetworkAccessManager*	_nam=nullptr;
	QString					_url;
	QByteArray				_data;

	QImage					_img;
	int						_type;
	int						_id;

	QTimer*					_timer=nullptr;
	QNetworkReply*			_reply=nullptr;

	QMap<QByteArray, QByteArray> _header;

};


#endif
