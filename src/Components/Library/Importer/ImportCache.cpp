/* ImportCache.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
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


#include "ImportCache.h"
#include "Helper/Settings/Settings.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaDataList.h"

struct ImportCache::Private
{
	QString					library_path;
	MetaDataList			v_md;
	QMap<QString, MetaData> src_md_map;
	QMap<QString, QString>	src_dst_map;
	QStringList				files;
};

ImportCache::ImportCache()
{
	_m = Pimpl::make<ImportCache::Private>();
	_m->library_path = Settings::getInstance()->get(Set::Lib_Path);
}

ImportCache::~ImportCache() {}
ImportCache::ImportCache(const ImportCache& other)
{
	_m = Pimpl::make<ImportCache::Private>();
	ImportCache::Private data = *(other._m.get());
	(*_m) = data;
}

ImportCache& ImportCache::operator=(const ImportCache& other)
{
	ImportCache::Private data = *(other._m.get());
	(*_m) = data;

	return *this;
}

void ImportCache::clear()
{
	_m->files.clear();
	_m->v_md.clear();
	_m->src_dst_map.clear();
}

void ImportCache::add_soundfile(const MetaData& md)
{
	if(md.filepath().isEmpty()){
		return;
	}

	_m->v_md << md;
	_m->src_md_map[md.filepath()] = md;
}

void ImportCache::add_standard_file(const QString& filename, const QString& parent_dir)
{
	if(filename.isEmpty()){
		return;
	}

	_m->files << filename;

	QString pure_src_filename = Helper::File::get_filename_of_path(filename);
	QString target_subdir;

	if(!parent_dir.isEmpty()){

		QString file_dir = Helper::File::get_parent_directory(filename);
		QString sub_dir = file_dir.remove(Helper::File::get_absolute_filename(parent_dir));
		QString pure_srcdir = Helper::File::get_filename_of_path(parent_dir);

		target_subdir = pure_srcdir + "/" + sub_dir + "/";
	}

	_m->src_dst_map[filename] = target_subdir + pure_src_filename;
}

QStringList ImportCache::get_files() const
{
	return _m->files;
}

MetaDataList ImportCache::get_soundfiles() const
{
	return _m->v_md;
}

QString ImportCache::get_target_filename(const QString &src_filename, const QString& target_directory) const
{
	return _m->library_path + "/" + target_directory + "/" + _m->src_dst_map[src_filename];
}

MetaData ImportCache::get_metadata(const QString& filename) const
{
	return _m->src_md_map[filename];
}


void ImportCache::change_metadata(const MetaDataList& v_md_old, const MetaDataList& v_md_new)
{
	Q_UNUSED(v_md_old)

	_m->v_md = v_md_new;
	for(const MetaData& md : v_md_new){
		_m->src_md_map[md.filepath()] = md;
	}
}
