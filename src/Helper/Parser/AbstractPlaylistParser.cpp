#include "AbstractPlaylistParser.h"
#include "Helper/FileHelper.h"
#include "Helper/Helper.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "PlaylistParser.h"


AbstractPlaylistParser::AbstractPlaylistParser(const QString& filename)
{
	QString pure_file;

	Helper::File::split_filename(filename, _directory, pure_file);
	Helper::File::read_file_into_str(filename, _file_content);
	_parsed = false;
}

AbstractPlaylistParser::~AbstractPlaylistParser(){

}

MetaDataList AbstractPlaylistParser::get_md(bool force_parse) {

	if(force_parse){
		_parsed = false;
	}

	if(!_parsed){
		_v_md.clear();
		parse();
		sp_log(Log::Debug) << "Found " << _v_md.size() << " entries";
	}

	_parsed = true;


	return _v_md;
}


QString AbstractPlaylistParser::get_absolute_filename(const QString& filename) const
{
	QString ret;

	if(filename.isEmpty()){
		return "";
	}

	if(Helper::File::is_www(filename)){
		return filename;
	}

	if(!Helper::File::is_absolute(filename)){
		ret = _directory + QDir::separator() + filename;
	}
	else{
		ret = filename;
	}

	if(!QFile::exists(ret)){
		ret = "";
	}

	return ret;
}
