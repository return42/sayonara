#include "PLSParser.h"
#include "Helper/FileHelper.h"
#include "Helper/Helper.h"

#include <QRegExp>
#include <QPair>

PLSParser::PLSParser(const QString& filename) :
	AbstractPlaylistParser(filename)
{
}


void PLSParser::parse(){

	QStringList lines = _file_content.split("\n");

	MetaData md;
	int cur_track_idx = -1;

	for(QString line : lines) {

		bool success;
		QString key, val;
		int track_idx;

		line = line.trimmed();
		if(line.isEmpty() || line.startsWith("#")){
			continue;
		}

		success = split_line(line, key, val, track_idx);

		if(!success){
			continue;
		}


		if(track_idx != cur_track_idx){

			if(cur_track_idx > 0){
				_v_md << md;
			}

			md = MetaData();
			cur_track_idx = track_idx;
		}


		md.track_num = track_idx;

		if(key.startsWith("file")) {
			QString filepath = get_absolute_filename(val);
			md.set_filepath(filepath);
			md.artist = filepath;
		}

		else if(key.startsWith("title")){
			md.title = val;
		}

		else if(key.startsWith("length")) {

			int len = val.toInt();

			len = std::max(0, len);
			md.length_ms = len * 1000;
		}
	}

	if(!md.filepath().isEmpty()){
		_v_md << md;
	}
}


int PLSParser::get_number_of_entries(){

	QRegExp re("^\\s*numberofentries\\s*=\\s*([0-9]+)");

	int idx = -1;
	idx = re.indexIn(_file_content.toLower());
	if(idx == -1){
		return -1;
	}

	return re.cap(1).toInt();
}

bool PLSParser::split_line(const QString& line, QString& key, QString& val, int& idx)
{
	int pos_idx;
	QRegExp re_idx("(\\S+)([0-9]+)");
	QStringList splitted = line.split("=");

	if(splitted.size() < 2){
		return false;
	}

	pos_idx = re_idx.indexIn(splitted[0]);
	if(pos_idx < 0){
		return false;
	}

	key = re_idx.cap(1).toLower();
	val = splitted[1];
	idx = re_idx.cap(2).toInt();

	return (idx >= 0);
}
