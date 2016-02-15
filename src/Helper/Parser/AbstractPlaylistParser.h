#ifndef ABSTRACTPLAYLISTPARSER_H
#define ABSTRACTPLAYLISTPARSER_H


#include <QString>
#include "Helper/MetaData/MetaData.h"

class AbstractPlaylistParser
{

public:
	AbstractPlaylistParser(const QString& filepath);
	virtual ~AbstractPlaylistParser();

	virtual MetaDataList get_md(bool force_parse=false) final;


protected:
	MetaDataList		_v_md;
	QString				_file_content;
	QString				_directory;
	QString				_download_target;
	bool				_parsed;


protected:

	virtual void parse()=0;

	QString get_absolute_filename(const QString& filename) const;
};

#endif // ABSTRACTPLAYLISTPARSER_H
