/* LibraryItem.h */

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

#ifndef _LIBRARY_ITEM_H_
#define _LIBRARY_ITEM_H_

#include "Utils/Pimpl.h"
#include "Utils/typedefs.h"
#include <vector>
#include <QHash>

using HashValue=uint32_t;
class QString;


/**
 * @brief The CustomField class
 * a CustomField is some additional entry than can be set for MetaData, Albums and Artists
 * and will be displayed on the Info Dialog
 * These custom fields are intendend for Plugins
 * @ingroup MetaDataHelper
 */
class CustomField
{
	PIMPL(CustomField)

public:
	CustomField(const QString& id, const QString& display_name, const QString& value);
	CustomField(const CustomField& other);
	CustomField(CustomField&& other);

	CustomField& operator=(const CustomField& other);
	CustomField& operator=(CustomField&& other);

	~CustomField();

	QString get_id() const;
	QString get_display_name() const;
	QString get_value() const;
};

using CustomFieldList=std::vector<CustomField>;

/**
 * @brief The LibraryItem class
 * @ingroup MetaDataHelper
 */
class LibraryItem
{
	PIMPL(LibraryItem)

public:
	LibraryItem();
	LibraryItem(const LibraryItem& other);
	LibraryItem(LibraryItem&& other);

	LibraryItem& operator=(const LibraryItem& other);
	LibraryItem& operator=(LibraryItem&& other);

	virtual ~LibraryItem();

	void add_custom_field(const CustomField& field);
	void add_custom_field(const QString& id, const QString& display_name, const QString& value);

	const CustomFieldList& get_custom_fields() const;
	QString get_custom_field(const QString& id) const;
	QString get_custom_field(int idx) const;

	QString cover_download_url() const;
	void set_cover_download_url(const QString& url);

	DbId db_id() const;
	void set_db_id(DbId id);

	virtual void print() const;

protected:
	static QHash<HashValue, QString>& album_pool();
	static QHash<HashValue, QString>& artist_pool();
};



#endif

