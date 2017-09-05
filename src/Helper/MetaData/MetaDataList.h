	/* MetaDataList.h */

/* Copyright (C) 2011-2017  Lucio Carreras
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

#ifndef METADATALIST_H
#define METADATALIST_H

#include "MetaData.h"
#include "MetaDataFwd.h"
#include "Helper/typedefs.h"
#include "Helper/SetFwd.h"
#include "Helper/Pimpl.h"

/**
 * @brief The MetaDataList class
 * @ingroup MetaDataHelper
 */

class MetaDataList :
		public QList<MetaData>
{
	PIMPL(MetaDataList)

public:
	MetaDataList();
	MetaDataList(const MetaDataList&);

	~MetaDataList();

	MetaDataList& operator=(const MetaDataList& other);

	void set_current_track(int idx);
	int current_track() const;

	bool contains(const MetaData& md) const;
	MetaDataList& remove_track(int idx);
	MetaDataList& remove_tracks(const SP::Set<int>& rows);
	MetaDataList& remove_tracks(int first, int last);

	MetaDataList& move_tracks(const SP::Set<int>& indexes, int tgt_idx);
	MetaDataList& copy_tracks(const SP::Set<int>& indexes, int tgt_idx);
	MetaDataList& insert_tracks(const MetaDataList& v_md, int tgt_idx);

	IdxList findTracks(int id) const;
	IdxList findTracks(const QString&) const;

	QStringList toStringList() const;

	MetaDataList& operator <<(const MetaDataList& v_md);
	MetaDataList& operator <<(const MetaData& md);

	bool contains(int32_t id) const;
	void remove_duplicates();
};

#endif // METADATALIST_H
