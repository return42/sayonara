/* LFMWebAccess.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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
 * LFMWebAccess.h
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */

#ifndef LFMWEBACCESS_H_
#define LFMWEBACCESS_H_

#include "LFMGlobals.h"

#include "Helper/Helper.h"
#include "Helper/Logger/Logger.h"

#include <QDomDocument>


class UrlParams :  public QMap<QByteArray, QByteArray> {

public:
	UrlParams() :
		QMap<QByteArray, QByteArray>()
	{

	}

	void append_signature(){
		QByteArray signature;

		for(const QByteArray& key : this->keys()) {

			signature += key;
			signature += this->value(key);
		}

		signature += LFM_API_SECRET;

		QByteArray hash = Helper::calc_hash(signature);

		this->insert("api_sig", hash);
	}

};

#define LFMWebAccess_sig_response_doc static_cast<void (LFMWebAccess::*)(const QDomDocument&)>(&LFMWebAccess::sig_response)
#define LFMWebAccess_sig_response_str static_cast<void (LFMWebAccess::*)(const QString&)>(&LFMWebAccess::sig_response)
class LFMWebAccess : public QObject {

	Q_OBJECT

signals:
	void sig_response(const QDomDocument& doc);
	void sig_response(const QString& response);
	void sig_error(const QString& error);

public:
	void call_url(const QString& url);
	void call_url_xml(const QString& url);
	void call_post_url(const QString& url, const QByteArray& post_data);
	void call_post_url_https(const QString& url, const QByteArray& post_data);

private slots:
	void awa_finished(bool success);
	void awa_finished_xml(bool success);

private:
	QString parse_error_message(const QString& response);
	bool check_error(const QByteArray& data, bool success);

public:
	static QString parse_session_answer(const QString& content);
	static QString parse_token_answer(const QString& content);

	static QString create_std_url(const QString& base_url, const UrlParams& data);
	static QString create_std_url_post(const QString& base_url, const UrlParams& data, QByteArray& post_data);
};


#endif /* LFMWEBACCESS_H_ */
