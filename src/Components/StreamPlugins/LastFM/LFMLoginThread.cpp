/* LoginThread.cpp */

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

#include "LFMLoginThread.h"
#include "LFMGlobals.h"
#include "LFMWebAccess.h"
#include "Utils/Utils.h"
#include "Utils/Message/Message.h"
#include "Utils/Logger/Logger.h"

using namespace LastFM;

LoginThread::LoginThread(QObject *parent) :
	QObject(parent) {}

LoginThread::~LoginThread() {}

void LoginThread::login(const QString& username, const QString& password)
{
	WebAccess* lfm_wa = new WebAccess();
    connect(lfm_wa, &WebAccess::sig_response, this, &LoginThread::wa_response);
    connect(lfm_wa, &WebAccess::sig_error, this, &LoginThread::wa_error);

	_login_info.logged_in = false;
	_login_info.session_key = "";
	_login_info.subscriber = false;

    UrlParams signature_data;
        signature_data["api_key"] = LFM_API_KEY;
		signature_data["method"] = "auth.getMobileSession";
		signature_data["password"] = password.toLocal8Bit();
		signature_data["username"] = username.toLocal8Bit();

		signature_data.append_signature();

	QByteArray post_data;
	QString url = lfm_wa->create_std_url_post("https://ws.audioscrobbler.com/2.0/", signature_data, post_data);

	lfm_wa->call_post_url(url, post_data);
}


void LoginThread::wa_response(const QByteArray& data)
{
	WebAccess* lfm_wa = static_cast<WebAccess*>(sender());
	QString str = QString::fromUtf8(data);

	_login_info.logged_in = true;
	_login_info.session_key = Util::easy_tag_finder("lfm.session.key", str);
	_login_info.subscriber = (Util::easy_tag_finder("lfm.session.subscriber", str).toInt() == 1);
	_login_info.error = str;

	if(_login_info.session_key.size() >= 32){
		emit sig_logged_in(true);
	}

	else {
		emit sig_logged_in(false);
	}

	lfm_wa->deleteLater();
}


void LoginThread::wa_error(const QString& error)
{
	WebAccess* lfm_wa = static_cast<WebAccess*>(sender());

	sp_log(Log::Warning, this) << "LastFM: Cannot login";
	sp_log(Log::Warning, this) << error;

	emit sig_error(error);

	lfm_wa->deleteLater();
}


LoginStuff LoginThread::getLoginStuff()
{
	return _login_info;
}

