
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

#include "Utils/FileUtils.h"

#include "SearchableFileTreeModel.h"
#include "GUI/Utils/SearchableWidget/MiniSearcher.h"

#include "Utils/Settings/Settings.h"
#include "Utils/Library/SearchMode.h"
#include "Utils/Library/LibraryInfo.h"
#include "Utils/Library/Filter.h"
#include "Utils/MetaData/MetaDataList.h"

#include "Components/Library/LibraryManager.h"

#include "Database/LibraryDatabase.h"
#include "Database/DatabaseConnector.h"

#include <QDirIterator>
#include <algorithm>

struct SearchableFileTreeModel::Private
{
	QStringList	found_strings;
	int			cur_idx;
	bool		search_only_dirs;

	Private()
	{
		search_only_dirs = false;
		cur_idx = -1;
	}
};

SearchableFileTreeModel::SearchableFileTreeModel(QObject* parent) :
	SearchableModel<QFileSystemModel>(parent)
{
	m = Pimpl::make<Private>();
}

SearchableFileTreeModel::~SearchableFileTreeModel() {}

void SearchableFileTreeModel::search_only_dirs(bool b)
{
	if(b != m->search_only_dirs){
		m->cur_idx = 0;
	}

	m->search_only_dirs = b;
}

QModelIndex SearchableFileTreeModel::getFirstRowIndexOf(const QString& substr)
{
	Library::Filter filter;
	filter.set_filtertext("%" + substr + "%");
	filter.set_mode(Library::Filter::Mode::Filename);

	m->found_strings.clear();
	m->cur_idx = -1;

	DB::Connector* db = DB::Connector::instance();
	DB::LibraryDatabases library_dbs = db->library_dbs();

	for(DB::LibraryDatabase* lib_db : library_dbs)
	{
		LibraryId lib_id = lib_db->library_id();
		if(lib_id < 0){
			continue;
		}

		Library::Info info = Library::Manager::instance()->library_info(lib_id);
		QString info_path = info.path();
		QString symlink_path = info.symlink_path();

		MetaDataList v_md;
		lib_db->getAllTracksBySearchString(filter, v_md);

		for(const MetaData& md : v_md)
		{
			QString filepath = md.filepath();
			QString parent = ::Library::Util::convert_search_string(Util::File::get_parent_directory(filepath), search_mode());

			if(m->search_only_dirs)
			{
				if(!parent.contains(substr)){
					continue;
				}
			}

			filepath.replace(info_path, symlink_path);

			m->found_strings << Util::File::get_parent_directory(filepath);
		}
	}

	QString str;
	if(m->found_strings.size() > 0)
	{
		std::sort(m->found_strings.begin(), m->found_strings.end(), [](const QString& str1 , const QString& str2){
			return (str1 < str2);
		});

		m->found_strings.removeDuplicates();

		str = m->found_strings.first();
		m->cur_idx = 0;
	}

	QModelIndex idx = index(str);
	if(canFetchMore(idx)){
		fetchMore(idx);
	}

	return idx;
}


QModelIndex SearchableFileTreeModel::getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(substr)
	Q_UNUSED(cur_row)
	Q_UNUSED(parent)

	QString str;

	if(m->cur_idx < 0 || m->found_strings.isEmpty() ){
		return QModelIndex();
	}

	m->cur_idx = (m->cur_idx + 1) % m->found_strings.size();

	str = m->found_strings[m->cur_idx];

	return index(str);
}


QModelIndex SearchableFileTreeModel::getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(substr)
	Q_UNUSED(cur_row)
	Q_UNUSED(parent)

	QString str;

	if(m->cur_idx < 0 ){
		return QModelIndex();
	}

	if(m->cur_idx == 0){
		str = m->found_strings[m->cur_idx];

		return index(str);
	}

	m->cur_idx--;
	str = m->found_strings[m->cur_idx];

	return index(str);
}

QMap<QChar, QString> SearchableFileTreeModel::getExtraTriggers()
{
	return QMap<QChar, QString>();
}
