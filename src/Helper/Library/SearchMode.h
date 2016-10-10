#ifndef LIBRARYSEARCHMODE_H
#define LIBRARYSEARCHMODE_H

#include <QString>

namespace Library
{
    enum SearchMode
    {
		CaseInsensitve=(1<<0),
		NoSpecialChars=(1<<1),
		NoDiacriticChars=(1<<2)
    };

    typedef int SearchModeMask;

	QString convert_search_string(const QString& str, Library::SearchModeMask mode);
}

#endif // LIBRARYSEARCHMODE_H
