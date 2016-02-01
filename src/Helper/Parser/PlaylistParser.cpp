/* PlaylistParser.cpp */

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


#include "Helper/Parser/PlaylistParser.h"
#include "Helper/Helper.h"

#include "Helper/Tagging/Tagging.h"
#include "Database/DatabaseConnector.h"


#include <QFile>
#include <QDir>
#include <QUrl>
#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>



static DatabaseConnector* db = DatabaseConnector::getInstance();

QString _correct_filepath(QString filepath, QString abs_path) {

	if(FileHelper::is_www(filepath)) {
		return filepath;
	}

	QString ret;

    bool is_absolute = QDir(filepath).isAbsolute();
    QString tmp_filepath;
    if(!is_absolute) {
        tmp_filepath = abs_path + filepath.trimmed();
        if(!QFile::exists(tmp_filepath)) {
            tmp_filepath = abs_path + filepath;
            if(!QFile::exists(tmp_filepath)) {
                return "";
            }
			else {
				return QDir(tmp_filepath).absolutePath();
			}
        }

		else {
			return QDir(tmp_filepath).absolutePath();
		}
    }

    else {
        tmp_filepath = filepath.trimmed();
        if(!QFile::exists(tmp_filepath)) {
            tmp_filepath = filepath;
            if(!QFile::exists(tmp_filepath)) {
                return "";
            }
			else {
				return QDir(tmp_filepath).absolutePath();
			}
        }

		else {
			return QDir(tmp_filepath).absolutePath();
		}
    }

	// will never be reached
	return "";
}


int PlaylistParser::parse_m3u(QString file_content, MetaDataList& v_md, QString abs_path) {
	QStringList list = file_content.split('\n');

    QString artist;
    QString title;
    quint64 len_ms = 0;

	for(const QString& _line : list) {

		//sp_log(Log::Debug) << "Line = " << line.trimmed();

		QString line = _line.trimmed();
        if(line.startsWith("#EXTINF:", Qt::CaseInsensitive)) {
            QStringList sl1;

            line.remove("#EXTINF:", Qt::CaseInsensitive);

            sl1 = line.split(",");

            if(sl1.size() > 0){
                qint32 len = sl1[0].toInt();
                if(len > 0){
                    len_ms = (quint64) (len * 1000);
                }

				else{
					len_ms = 0;
				}
            }

            if(sl1.size() > 1){
                QStringList sl2;
                sl2 = sl1[1].split("-");
                if(sl2.size() > 0){
                    artist = sl2[0];
                }

                if(sl2.size() > 1){
                    title = sl2[1];
                }
            }

            continue;
        }

        if(line.trimmed().startsWith('#')) {
            continue;
        }



		if(FileHelper::is_playlistfile(line)){
			return parse_playlist(line, v_md);
		}

		if( !FileHelper::is_www(line)) {
			MetaData md;
			QString cor_filepath = _correct_filepath(line, abs_path);

			md.set_filepath(cor_filepath);

			MetaData md_tmp = db->getTrackByPath(cor_filepath);

			if( md_tmp.id >= 0 ) {
				v_md << std::move(md_tmp);
            }

			else if( !md.filepath().isEmpty() &&
					 Tagging::getMetaDataOfFile(md) )
			{
				v_md << std::move(md);
			}
		}

		else {
			MetaData md;
			QString cor_filepath = _correct_filepath(line, abs_path);

            if(md.artist.size() == 0 && artist.isEmpty()){
                md.artist = line;
            }

            else{
                md.artist = artist;
            }

            md.title = title;
			md.album = "";
            md.length_ms = len_ms;

			md.set_filepath(cor_filepath);
			v_md << std::move(md);

            artist = "";
            title = "";
            len_ms = 0;
		}
	}


	return v_md.size();
}


int PlaylistParser::parse_asx(QString file_content, MetaDataList& v_md, QString abs_path) {


	v_md.clear();
	QDomDocument doc("AsxFile");
	doc.setContent(file_content.toLower());

	QDomElement docElement = doc.documentElement();
	QDomNode entry = docElement.firstChildElement("entry");

	if(!entry.hasChildNodes()) return 0;

	do{

		MetaData md;
		md.length_ms = 0;
		md.album = "";

		for(int entry_child=0; entry_child<entry.childNodes().size(); entry_child++)
		{
			md.bitrate = 128000;

			QDomNode content = entry.childNodes().at(entry_child);
			QString nodename = content.nodeName().toLower();
			QDomElement e = content.toElement();

			if(!nodename.compare("ref")) {
				QString path = e.attribute("href");
				QString cor_filepath = _correct_filepath(path, abs_path);

				// filepath, convert to absolute path if relative

				md.artist = path.trimmed();
				md.set_filepath(cor_filepath);
			}

			else if(!nodename.compare("title")) {
				md.title = e.text();
			}

            else if(!nodename.compare("album")) {
                md.album = e.text();
            }

			else if(!nodename.compare("author")) {
				md.artist = e.text();
			}
		}

		v_md << std::move(md);
		entry = entry.nextSibling();
	} while (!entry.isNull());

	return v_md.size();
}


int PlaylistParser::parse_pls(QString file_content, MetaDataList& v_md, QString abs_path) {

	// abs_path = "", if file is not local

	v_md.clear();

	QStringList lines = file_content.split("\n");

	int n_titles = 0;

	// get number of titles
	int idx = file_content.toLower().indexOf("numberofentries=") +
			QString("numberofentries=").size();

	if( !between(idx, 0, file_content.size())) {
		return 0;
	}

	QString n_titles_str = "";
	while( file_content.at(idx).isDigit() ) {
		n_titles_str += file_content.at(idx++);
	}


	n_titles = n_titles_str.toInt();
	if(n_titles == 0) {
		return 0;
	}

	MetaData md;
	md.album = "";
	v_md.resize(n_titles);

	for(const QString& _line : lines) {

		if(_line.trimmed().size() == 0 ||
			_line.trimmed().startsWith("#")){

			continue;
		}

		QString line = _line;

		// remove comments
		int comment_idx=line.indexOf('#');
		if(comment_idx >= 0) {
			line = line.mid(comment_idx, line.size() - comment_idx);
		}

		// split key, value
		QString key, val, tmp_key;
		QStringList line_splitted = line.split('=');
		if( line_splitted.size() <= 1 ) continue;

		tmp_key = line_splitted[0].trimmed();
        val = line_splitted[1];


		int track_idx = -1;
		int f_track_idx = 0;
		QString track_idx_str = "";

		for(int i=0; i<tmp_key.size(); i++) {
			QChar c = tmp_key[i];
			if(c.isDigit()) {
				if(f_track_idx == 0) f_track_idx = i;
				track_idx_str += c;
			}
		}

		track_idx = track_idx_str.toInt();
		if( track_idx <= 0 ||
			track_idx_str.size() == 0 ||
            (track_idx - 1 ) >= v_md.size() ) continue;

		key = tmp_key.left(f_track_idx);

		if(key.toLower().startsWith("file")) {

			QString cor_filepath = _correct_filepath(val, abs_path);

			v_md[track_idx - 1].artist = val;

			// calc absolute filepath
			v_md[track_idx - 1].set_filepath(cor_filepath);
		}

        else if(line.toLower().startsWith("title")) {
			v_md[track_idx - 1].title = val;
		}

		else if(line.toLower().startsWith("length")) {
			int len = val.toInt();
			if(len > 0){
				v_md[track_idx - 1].length_ms = len * 1000;
			}

			else{
				len = 0;
			}
		}
	}

	return v_md.size();

}

int PlaylistParser::parse_playlist(const QString &playlist_file, MetaDataList& v_md){

	if(!FileHelper::is_file(playlist_file)) {
		return -1;
	}

	if(FileHelper::is_www(playlist_file)) {
		return -1;
	}

	QByteArray content;
	bool success = Helper::read_file_into_byte_arr(playlist_file, content);
	if(!success){
		return -1;
	}

	return parse_playlist_content(content, v_md, playlist_file);

}

int PlaylistParser::parse_playlist_content(const QByteArray& content, MetaDataList& v_md, const QString& filename) {

	// is only changed, if container file is local
	QString abs_path = "";

	MetaDataList v_md_tmp;
	MetaDataList v_md_to_delete;

	if(!filename.isEmpty()) {
		int last_slash = filename.lastIndexOf(QDir::separator());
		abs_path = filename.left(last_slash+1);
	}

	if(filename.toLower().endsWith("m3u")) {

		parse_m3u(content, v_md_tmp, abs_path);
	}

	else if(filename.toLower().endsWith("pls")) {
		parse_pls(content, v_md_tmp, abs_path);
	}

	else if(filename.toLower().endsWith("ram")) {
		parse_m3u(content, v_md_tmp, abs_path);
	}

	else if(filename.toLower().endsWith("asx")) {
		parse_asx(content, v_md_tmp, abs_path);
	}

	else{
		parse_m3u(content, v_md_tmp);

		if(v_md_tmp.isEmpty()){
			parse_pls(content, v_md_tmp);
		}

		if(v_md_tmp.isEmpty()){
			parse_asx(content, v_md_tmp);
		}
	}

	for(const MetaData& md : v_md_tmp) {

        if( Helper::check_track(md) ){
			v_md << std::move(md);
        }

        else{
			v_md_to_delete << std::move(md);
        }
	}

	if(!v_md_to_delete.isEmpty()){
		DatabaseConnector::getInstance()->deleteTracks(v_md_to_delete);
	}

	return v_md.size();
}


void PlaylistParser::save_playlist(QString filename, const MetaDataList& v_md, bool relative) {

	if(!filename.endsWith("m3u", Qt::CaseInsensitive)) {
		filename.append(".m3u");
	}

	bool success;
    QString dir_str = filename.left(filename.lastIndexOf(QDir::separator()));
    QDir dir(dir_str);
    dir.cd(dir_str);

	QFile file(filename);
	success = file.open(QIODevice::WriteOnly);
	if(!success){
		return;
	}

	file.write( QByteArray("#EXTM3U\n") );

    qint64 lines = 0;
	for(const MetaData& md : v_md) {

        QString str;
        if(relative) {
			str = dir.relativeFilePath(md.filepath());
        }

        else{
			str = md.filepath();
        }

        QString ext_data = "#EXTINF: " + QString::number(md.length_ms / 1000)  + ", " + md.artist + " - " + md.title + "\n";
		lines += file.write(ext_data.toLocal8Bit());
		lines += file.write(str.toLocal8Bit());
		lines += file.write( QByteArray("\n") );
    }

	file.close();
}




