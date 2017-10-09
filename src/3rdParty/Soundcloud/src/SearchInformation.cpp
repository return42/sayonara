#include "SearchInformation.h"
#include "Utils/Set.h"
#include "Utils/Logger/Logger.h"

#include <QString>
#include <QHash>


struct SC::SearchInformation::Private
{
	int artist_id;
	int album_id;
	int track_id;

	QString search_string;
};

struct SC::SearchInformationList::Private
{
    QHash<QString, IntSet> artist_id_map;
    QHash<QString, IntSet> album_id_map;
    QHash<QString, IntSet> track_id_map;
};


SC::SearchInformation::SearchInformation(int artist_id, int album_id, int track_id, const QString& search_string)
{
	m = Pimpl::make<Private>();
	m->artist_id = artist_id;
	m->album_id = album_id;
	m->track_id = track_id;
	m->search_string = search_string;
}

SC::SearchInformation::~SearchInformation() {}

QString SC::SearchInformation::search_string() const
{
	return m->search_string;
}

int SC::SearchInformation::artist_id() const
{
	return m->artist_id;
}

int SC::SearchInformation::album_id() const
{
	return m->album_id;
}

int SC::SearchInformation::track_id() const
{
	return m->track_id;
}



SC::SearchInformationList::SearchInformationList()
{
	m = Pimpl::make<Private>();
}

SC::SearchInformationList::~SearchInformationList(){}


static IntSet ids(const QString& search_string, const QHash<QString, IntSet>& id_map)
{
    IntSet ids;
	QHash<int, int> results;
	int iterations=0;

	for(int idx = 0; idx<search_string.size() - 3; idx++)
	{
		QString part = search_string.mid(idx, 3);
        const IntSet& part_ids = id_map[part];

		if(part_ids.isEmpty()){
			break;
		}

		for(int part_id : part_ids)
		{
			if(results.contains(part_id)){
				results[part_id] = results[part_id] + 1;
			}

			else {
				if(iterations == 0){
					results[part_id] = 1;
				}
			}
		}

		iterations++;
	}

	for(int result : results.keys()) {
		if(results[result] == iterations){
			ids.insert(result);
		}
	}

	return ids;
}

IntSet SC::SearchInformationList::artist_ids(const QString& search_string) const
{
	return ids(search_string, m->artist_id_map);
}

IntSet SC::SearchInformationList::album_ids(const QString& search_string) const
{
	return ids(search_string, m->album_id_map);
}

IntSet SC::SearchInformationList::track_ids(const QString& search_string) const
{
	return ids(search_string, m->track_id_map);
}

SC::SearchInformationList& SC::SearchInformationList::operator<<(const SearchInformation& search_information)
{
	QString search_string = search_information.search_string();
	for(int idx=0; idx<search_string.size() - 5; idx++)
	{
		QString part = search_string.mid(idx, 3).toLower();

		m->album_id_map[part].insert(search_information.album_id());
		m->artist_id_map[part].insert(search_information.artist_id());
		m->track_id_map[part].insert(search_information.track_id());
	}

	return *this;
}

bool SC::SearchInformationList::is_empty() const
{
	return m->album_id_map.isEmpty();
}

void SC::SearchInformationList::clear()
{
	m->album_id_map.clear();
	m->artist_id_map.clear();
	m->track_id_map.clear();
}

