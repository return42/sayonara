/* ImportCache.h */

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

#ifndef IMPORTCACHE_H
#define IMPORTCACHE_H

#include "Utils/Pimpl.h"

class QString;
class QStringList;
class MetaData;
class MetaDataList;

/**
 * @brief The ImportCache class
 * @ingroup Library
 */
class ImportCache
{
private:
	PIMPL(ImportCache)

public:
	ImportCache(const QString& library_path);
	virtual ~ImportCache();

	ImportCache(const ImportCache& other);
	ImportCache& operator=(const ImportCache& other);

	void			clear();

	void			add_soundfile(const MetaData& md);
	void			add_standard_file(const QString& filename);
	void			add_standard_file(const QString& filename, const QString& parent_dir);

	QStringList		files() const;
	MetaDataList	soundfiles() const;
	QString			target_filename(const QString& src_filename, const QString& target_directory) const;
	MetaData		metadata(const QString& filename) const;
	void			change_metadata(const MetaDataList& v_md_old, const MetaDataList& v_md_new);
};

using ImportCachePtr=std::shared_ptr<ImportCache>;

#endif // IMPORTCACHE_H
