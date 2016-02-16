#ifndef ABSTRACTPLAYLISTPARSER_H
#define ABSTRACTPLAYLISTPARSER_H


#include <QString>
#include "Helper/MetaData/MetaData.h"

/**
 * @brief The AbstractPlaylistParser class
 * @ingroup PlaylistParser
 */
class AbstractPlaylistParser
{

public:
	AbstractPlaylistParser(const QString& filepath);
	virtual ~AbstractPlaylistParser();

	/**
	 * @brief parse playlist and return found metadata
	 * @param force_parse once if parsed, this function won't parse again and just return the metadata. \n
	 * Set to true if you want to force parsing again
	 * @return list of MetaData
	 */
	virtual MetaDataList get_md(bool force_parse=false) final;


protected:
	MetaDataList		_v_md;
	QString				_file_content;
	QString				_directory;
	QString				_download_target;
	bool				_parsed;


protected:

	/**
	 * @brief here the parsing is done\n
	 * Called by MetaDataList get_md(bool force_parse=false)
	 */
	virtual void parse()=0;

	/**
	 * @brief calculates the absolute filename for a track depending on the path of the playlist file
	 * @param filename as seen in the playlist
	 * @return absolute filename if local file. filename else
	 */
	QString get_absolute_filename(const QString& filename) const;
};

#endif // ABSTRACTPLAYLISTPARSER_H
