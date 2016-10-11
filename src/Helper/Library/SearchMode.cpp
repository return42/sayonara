/* SearchMode.cpp */

/* Copyright (C) 2011-2015  Lucio Carreras
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



#include "SearchMode.h"
#include <QRegExp>
#include <QStringList>

QStringList no_diacritic_chars;
QString diacritic_chars;

QString Library::convert_search_string(const QString& str, Library::SearchModeMask mode)
{
    QString ret = str;
	if(mode & Library::CaseInsensitve)
    {
		ret = str.toLower();
    }

	if(mode & Library::NoSpecialChars)
    {
		QString special_chars =
				QString::fromUtf8("\\.|'|\"|&|!|\\$|\\+|\\*|\\s|/|\\(|\\)|\\=|-|_|;|:|,|\\?|<|>|\\[|\\]|\\{|\\}|@|€");

		ret.remove(QRegExp(special_chars));
    }

	if(mode & Library::NoDiacriticChars)
    {
		if (diacritic_chars.isEmpty()) {
			diacritic_chars = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
			no_diacritic_chars << "S"<<"OE"<<"Z"<<"s"<<"oe"<<"z"<<"Y"<<"Y"<<"u"<<"A"<<"A"<<"A"<<"A"<<"A"<<"A"<<"AE"<<"C"<<"E"<<"E"<<"E"<<"E"<<"I"<<"I"<<"I"<<"I"<<"D"<<"N"<<"O"<<"O"<<"O"<<"O"<<"O"<<"O"<<"U"<<"U"<<"U"<<"U"<<"Y"<<"s"<<"a"<<"a"<<"a"<<"a"<<"a"<<"a"<<"ae"<<"c"<<"e"<<"e"<<"e"<<"e"<<"i"<<"i"<<"i"<<"i"<<"o"<<"n"<<"o"<<"o"<<"o"<<"o"<<"o"<<"o"<<"u"<<"u"<<"u"<<"u"<<"y"<<"y";
	}

	QString output;
	for (int i = 0; i < ret.length(); i++) {
	    QChar c = ret[i];
	    int found_index = diacritic_chars.indexOf(c);

	    if (found_index < 0) {
		output.append(c);
	    }
	    else {
		QString replacement = no_diacritic_chars[found_index];
		if(mode & Library::CaseInsensitve)
		{
		    replacement = replacement.toLower();
		}

		output.append(replacement);
	    }
	}

	ret = output;
    }

    return ret;
}

