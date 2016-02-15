#ifndef PLSPARSER_H
#define PLSPARSER_H

#include <QString>
#include "AbstractPlaylistParser.h"
#include <tuple>

typedef std::tuple<QString, QString, int> PLSTuple;

class PLSParser : public AbstractPlaylistParser
{

public:
	PLSParser(const QString& filename);

private:
	virtual void parse() override;

	int get_number_of_entries();
	bool split_line(const QString& line, QString& key, QString& val, int& idx);


};

#endif // PLSPARSER_H
