#ifndef SEARCHINFORMATION_H
#define SEARCHINFORMATION_H

#include "Helper/Pimpl.h"
#include "Helper/typedefs.h"
#include "Helper/SetFwd.h"

class QString;

namespace SC
{
	class SearchInformation
	{
		PIMPL(SearchInformation)

		public:
			SearchInformation(int artist_id, int album_id, int track_id, const QString& search_string);
			~SearchInformation();

			QString search_string() const;
			int artist_id() const;
			int album_id() const;
			int track_id() const;
	};

	class SearchInformationList
	{
		PIMPL(SearchInformationList)

		public:
			SearchInformationList();
			~SearchInformationList();

            IntSet artist_ids(const QString& search_string) const;
            IntSet album_ids(const QString& search_string) const;
            IntSet track_ids(const QString& search_string) const;

			SearchInformationList& operator<<(const SearchInformation& search_information);
			bool is_empty() const;
			void clear();
	};
}

#endif // SEARCHINFORMATION_H
