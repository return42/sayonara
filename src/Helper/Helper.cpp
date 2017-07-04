/* Helper.cpp */

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
 * Helper.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: Lucio Carreras
 */

#include <QtGlobal>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QString>
#include <QCryptographicHash>
#include <QDir>
#include <QRegExp>
#include <QDateTime>

#include <thread>
#include <chrono>

#ifdef Q_OS_LINUX
#ifndef LIB_INSTALL_DIR
#define LIB_INSTALL_DIR "/usr/lib"
#endif
#endif

#ifdef Q_OS_WIN
	#include <windows.h>
#endif

#include "Helper/globals.h"
#include "Helper/Helper.h"
#include "Helper/Random/RandomGenerator.h"
#include "Helper/Macros.h"
#include "Helper/Language.h"
#include "Helper/FileHelper.h"

template<typename T>
QString cvtNum2String(T num, int digits) {
	QString str = QString::number(num);
	while(str.size() < digits) {
		str.prepend("0");
	}

	return str;
}

uint64_t Helper::date_to_int(const QDateTime& date_time)
{
	QString str = date_time.toUTC().toString("yyMMddHHmmss");
	return str.toULongLong();
}

uint64_t Helper::current_date_to_int()
{
	QString str = QDateTime::currentDateTimeUtc().toString("yyMMddHHmmss");
	return str.toULongLong();
}


QString Helper::cvt_str_to_first_upper(const QString& str) 
{
	QStringList lst = str.split(" ");
	QStringList tgt_lst;

	for(QString word : lst) {
		tgt_lst << cvt_str_to_very_first_upper(word);
	}

	return tgt_lst.join(" ");
}

QString Helper::cvt_str_to_very_first_upper(const QString& str) 
{
	if(str.isEmpty()){
		return str;
	}
	QString ret_str = str.toLower();;
	QChar c = str.at(0).toUpper();

	ret_str.remove(0, 1);
	ret_str.prepend(c);
	return ret_str;
}


QString Helper::cvt_ms_to_string(uint64_t msec, bool empty_zero, bool colon, bool show_days) 
{
	if(msec == 0 && empty_zero){
		return "";
	}

	bool show_hrs = false;

	uint64_t sec = msec / 1000;
	uint64_t min = sec / 60;

	uint64_t secs = sec % 60;
	uint64_t hrs = min / 60;
	uint64_t days = hrs / 24;

	QString final_str;

	if(days > 0 && show_days) {
		final_str += QString::number(days) + Lang::get(Lang::DaysShort) + " ";
		hrs = hrs % 24;
		show_hrs = true;
	}

	if(!show_days) {
		hrs += (days * 24);
	}

	if(hrs > 0 || show_hrs) {
		final_str += QString::number(hrs) + Lang::get(Lang::HoursShort) + " ";
		min = min % 60;
	}

	if(colon){
		final_str +=  cvtNum2String(min, 2) + ":" + cvtNum2String(secs, 2);
	}

	else{
		final_str +=  cvtNum2String(min, 2) + Lang::get(Lang::MinutesShort) + " " + cvtNum2String(secs, 2);
	}

	return final_str;
}


QString Helper::sayonara_path(const QString& append_path)
{
	return Helper::File::clean_filename(
			QDir::homePath() + "/.Sayonara/" + append_path
	);
}


QString Helper::share_path(const QString& append_path)
{
	QString base_path;

#ifdef Q_OS_WIN
	base_path = "./share/";
#else
	base_path = SAYONARA_INSTALL_SHARE_PATH;
#endif

	return Helper::File::clean_filename(base_path + "/" + append_path);
}

QString Helper::lib_path(const QString& append_path)
{
	QString base_path;

#ifdef Q_OS_WIN
	base_path = "./lib/";
#else
	base_path = SAYONARA_INSTALL_LIB_PATH;
#endif

	return Helper::File::clean_filename(base_path + "/" + append_path);
}


QString Helper::create_link(const QString& name, bool dark, const QString& target, bool underline) 
{
	QString new_target;
	QString content;
	QString style;
	QString ret;

	if(target.size() == 0){
		new_target = name;
	}

	else {
		new_target = target;
	}

	if(!underline) style = " style: \"text-decoration=none;\" ";

	if(dark) {
		content = LIGHT_BLUE(name);
	}
	else {
		content = DARK_BLUE(name);
	}
	
	if(new_target.contains("://") || new_target.contains("mailto:")){
		ret = QString("<a href=\"") + new_target + "\"" + style + ">" + content + "</a>";
	}

	else {
		ret = QString("<a href=\"file://") + new_target + "\"" + style + ">" + content + "</a>";
	}

	return ret;
}


QStringList Helper::soundfile_extensions(bool with_asterisk)
{
	QStringList filters;
	filters << "mp3"
			<< "ogg"
			<< "opus"
			<< "oga"
			<< "m4a"
			<< "wav"
			<< "flac"
			<< "aac"
			<< "wma"
			<< "mpc"
			<< "aiff";

	QStringList upper_filters;
	for(QString& filter : filters) {
		if(with_asterisk) {
			filter.prepend("*.");
		}

		upper_filters << filter.toUpper();
	}

	filters.append(upper_filters);


	return filters;
}


QStringList Helper::playlist_extensions(bool with_asterisk)
{
	QStringList filters;

	filters << "pls"
			<< "m3u"
			<< "ram"
			<< "asx";

	QStringList upper_filters;
	for(QString& filter : filters) {
		if(with_asterisk) {
			filter.prepend("*.");
		}
		upper_filters << filter.toUpper();
	}

	filters.append(upper_filters);

	return filters;
}


QStringList Helper::podcast_extensions(bool with_asterisk)
{
	QStringList filters;

	filters << "xml"
			<< "rss";

	QStringList upper_filters;
	for(QString& filter : filters) {
		if(with_asterisk) {
			filter.prepend("*.");
		}
		upper_filters << filter.toUpper();
	}

	filters.append(upper_filters);

	return filters;
}


QString Helper::easy_tag_finder(const QString& tag, const QString& xml_doc) 
{
	int p = tag.indexOf('.');
	QString ret = tag;
	QString new_tag = tag;
	QString t_rev;
	QString new_xml_doc = xml_doc;

	while(p > 0) {
		ret = new_tag.left(p);
		t_rev = tag.right(new_tag.length() - p -1);

		new_xml_doc = easy_tag_finder(ret, new_xml_doc);
		p = t_rev.indexOf('.');
		new_tag = t_rev;
	}

	ret = new_tag;

	QString str2search_start = QString("<") + ret + QString(".*>");
	QString str2search_end = QString("</") + ret + QString(">");
	QString str2search = str2search_start + "(.+)" + str2search_end;
	QRegExp rx(str2search);
	rx.setMinimal(true);


	int pos = 0;
	if(rx.indexIn(new_xml_doc, pos) != -1) {
		return rx.cap(1);
	}

	return "";
}


QByteArray Helper::calc_hash(const QByteArray& data) 
{
	return QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
}


void Helper::sleep_ms(uint64_t ms)
{
#ifdef Q_OS_WIN
	Sleep(ms);
#else
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#endif
}

int Helper::random_number(int min, int max)
{
	return RandomGenerator().get_number(min, max);
}

QStringList Helper::ip_addresses()
{
	QStringList ret;
	QList<QHostAddress> host_list;
	host_list = QNetworkInterface::allAddresses();
	for(const QHostAddress& host : host_list){
		QString address = host.toString();
		if(!address.startsWith("127") && 
			host.protocol() == QAbstractSocket::IPv4Protocol)
		{
			ret << host.toString();
		}
	}

	return ret;
}

#include <cstdlib>

void Helper::set_environment(const QString& key, const QString& value)
{
#ifdef Q_OS_WIN
	QString str = key + "=" + value;
	_putenv(str.toLocal8Bit().constData());
	sp_log(Log::Info) << "Windows: Set environment variable " << str;
#else
	setenv(key.toLocal8Bit().constData(), value.toLocal8Bit().constData(), 1);
#endif
}

QString Helper::random_string(int max_chars)
{
	QString ret;
	for(int i=0; i<max_chars; i++){
		char c = random_number(97, 123);
		ret.append(QChar(c));
	}

	return ret;
}
