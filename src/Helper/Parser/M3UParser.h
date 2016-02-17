#ifndef M3UPARSER_H
#define M3UPARSER_H

#include "AbstractPlaylistParser.h"

/**
 * @brief The M3UParser class
 * @ingroup PlaylistParser
 */
class M3UParser : public AbstractPlaylistParser
{

public:
	M3UParser(const QString& filename);

private:
	virtual void parse() override;

	bool parse_first_line(const QString& line, MetaData& md);
	void parse_local_file(const QString& line, MetaData& md);
	void parse_www_file(const QString& line, MetaData& md);

};


#endif // M3UPARSER_H
