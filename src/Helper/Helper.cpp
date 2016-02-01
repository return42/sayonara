/* Helper.cpp */

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
 * Helper.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#include <QtGlobal>
#include <thread>
#include <chrono>
#ifdef Q_OS_LINUX
#ifndef LIB_INSTALL_DIR
#define LIB_INSTALL_DIR "/usr/lib"
#endif
#endif


#include "Helper/Helper.h"
#include "Helper/globals.h"
#include "Database/DatabaseHandler.h"
#include "Helper/Settings/Settings.h"

#include "Components/StreamPlugins/LastFM/LFMGlobals.h"

template<typename T>
QString cvtNum2String(T num, int digits) {
	QString str = QString::number(num);
	while(str.size() < digits) {
		str.prepend("0");
	}

	return str;
}


QString Helper::cvt_str_to_first_upper(const QString& str) {

	QString ret_str = "";
	QStringList lst = str.split(" ");

	for(QString& word : lst) {
		QString first = word.left(1);
		word.remove(0,1);
		word = first.toUpper() + word + " ";

		ret_str += word;
	}

	return ret_str.left(ret_str.size() - 1);
}


QString Helper::cvt_ms_to_string(quint64 msec, bool empty_zero, bool colon, bool show_days) {

	if(msec == 0 && empty_zero){
		return "";
	}

	StringDummy str(nullptr);

	bool show_hrs = false;

	quint64 sec = msec / 1000;
	quint64 min = sec / 60;

	quint64 secs = sec % 60;
	quint64 hrs = min / 60;
	quint64 days = hrs / 24;

	QString final_str;

	if(days > 0 && show_days) {
		final_str += QString::number(days) + str.days() + " ";
		hrs = hrs % 24;
		show_hrs = true;
	}

	if(!show_days) {
		hrs += (days * 24);
	}

	if(hrs > 0 || show_hrs) {
		final_str += QString::number(hrs) + str.hours() + " ";
		min = min % 60;
	}

	if(colon){
		final_str +=  cvtNum2String(min, 2) + ":" + cvtNum2String(secs, 2);
	}

	else{
		final_str +=  cvtNum2String(min, 2) + str.minutes() + " " + cvtNum2String(secs, 2);
	}

	return final_str;
}




QIcon Helper::get_icon(const QString& icon_name){

	QString path;

	if(icon_name.endsWith(".png")){
		path = icon_name;
		// alles paletti
	}

	else if(!icon_name.endsWith(".svg.png")){

		path = icon_name + ".svg.png";
	}

	path.prepend(":/Icons/");


	QIcon icon = QIcon(path);
	if(icon.isNull()){
		sp_log(Log::Warning) << "Icon " << path << " does not exist";
	}

	return icon;
}

QPixmap Helper::get_pixmap(const QString& icon_name, QSize sz, bool keep_aspect){

	QString path = QString(":/Icons/") + icon_name;
	if(path.endsWith(".png")){
		// alles paletti
	}

	else if(!path.endsWith(".svg.png")){
		path += ".svg.png";
	}

	QPixmap pixmap(path);

	if(pixmap.isNull()){
		sp_log(Log::Warning) << "Pixmap " << path << " does not exist";
	}

	if(sz.width() == 0){
		return pixmap;
	}

	else{
		if(keep_aspect){
			return pixmap.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}

		else{
			return pixmap.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		}
	}

	return pixmap;
}




QString Helper::get_error_file(){
	return get_sayonara_path() + "error_log";
}

QString Helper::get_sayonara_path() {
	return QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator();
}


QString Helper::get_share_path() {

#ifdef Q_OS_WIN
	return QString("./share");
#endif

	return SAYONARA_INSTALL_SHARE_PATH;
}

QString Helper::get_lib_path() {

#ifdef Q_OS_WIN
	return QString("./lib");
#endif

	return SAYONARA_INSTALL_LIB_PATH;
}




QString Helper::create_link(const QString& name, const QString& target, bool underline) {
	
	bool dark = (Settings::getInstance()->get(Set::Player_Style) == 1);

	QString new_target;
	QString content;
	QString style = "";
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




QStringList Helper::get_soundfile_extensions() {

	QStringList filters;
	filters << "*.mp3"
			<< "*.ogg"
			<< "*.opus"
			<< "*.oga"
			<< "*.m4a"
			<< "*.wav"
			<< "*.flac"
			<< "*.aac"
			<< "*.wma";

	return filters;
}


QStringList Helper::get_playlistfile_extensions() {

	QStringList filters;

	filters << "*.pls"
			<< "*.m3u"
			<< "*.ram"
			<< "*.asx";


	for(const QString& filter : filters) {
		filters.push_back(filter.toUpper());
	}

	return filters;
}




QStringList Helper::get_podcast_extensions() {

	QStringList filters, filters_new;

	filters << "*.xml"
			<< "*.rss";

	for(const QString& filter : filters) {
		filters_new.push_back(filter.toUpper());
	}

	return filters_new;
}



bool Helper::check_track(const MetaData& md) {

	QString filepath = md.filepath();
	if( FileHelper::is_www(filepath) ) return true;

	return QFile::exists( filepath );
}

bool Helper::read_file_into_byte_arr(const QString& filename, QByteArray& content){
	QFile file(filename);
	content.clear();


	if(!file.open(QIODevice::ReadOnly)){
		return false;
	}

	while(!file.atEnd()){
		QByteArray arr = file.read(4096);
		content.append(arr);
	}

	file.close();

	return (content.size() > 0);
}


bool Helper::read_file_into_str(const QString& filename, QString& content) {

	QFile file(filename);
	content.clear();
	if(!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	while (!file.atEnd()) {
		content.append(file.readLine());
	}

	file.close();

	if(content.size() > 0 ) {
		return true;
	}

	return false;

}


QString Helper::easy_tag_finder(const QString& tag, const QString& xml_doc) {

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

QByteArray Helper::calc_hash(const QByteArray& data) {
	return QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
}



QString Helper::get_major_artist(const ArtistList& artists) {

	QStringList lst;

	for(const Artist& artist : artists) {
		lst << artist.name;
	}

	return get_major_artist(lst);
}

QString Helper::get_major_artist(const QStringList& artists) {

	QMap<QString, int> map;
	if(artists.size() == 0) return "";
	if(artists.size() == 1) return artists[0].toLower().trimmed();

	int n_artists = artists.size();

	for(const QString& artist : artists) {

		QString alower = artist.toLower().trimmed();

		// count appearance of artist
		if( !map.keys().contains(alower) ) {
			map.insert(alower, 1);
		}
		else {
			map[alower] = map.value(alower) + 1;
		};
	}

	// n_appearances have to be at least 2/3 of all apperances
	for(const QString& artist : map.keys()) {

		int n_appearances = map.value(artist);
		if(n_appearances * 3 > n_artists * 2) return artist;

	}

	return StringDummy().various();
}

QString Helper::get_album_major_artist(int album_id) {

	if(album_id == -1) return "";

	QStringList artists;
	MetaDataList v_md(0);
	IDList ids;

	DatabaseConnector* db = DatabaseConnector::getInstance();

	ids << album_id;
	db->getAllTracksByAlbum(ids, v_md);

	if(v_md.size() == 0) return "";
	if(v_md.size() == 1) return v_md[0].artist;

	for(const MetaData& md : v_md) {
		artists << md.artist;
	}

	return get_major_artist(artists);
}


void Helper::sleep_ms(quint64 ms){
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}




QString Helper::StringDummy::various(){
	return tr("Various");
}

QString Helper::StringDummy::days(){
	return tr("d");
}

QString Helper::StringDummy::hours(){
	return tr("h");
}

QString Helper::StringDummy::minutes(){
	return tr("m");
}

QString Helper::StringDummy::seconds(){
	return tr("s");
}

QString Helper::StringDummy::disc(){
	return tr("Disc");
}

QString Helper::StringDummy::discs(){
	return tr("Discs");
}


int Helper::get_random_number(int min, int max)
{
	return RandomGenerator().get_number(min, max);
}
