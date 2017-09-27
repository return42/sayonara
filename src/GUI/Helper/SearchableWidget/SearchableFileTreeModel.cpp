
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

#include "Helper/FileHelper.h"

#include "SearchableFileTreeModel.h"
#include "GUI/Helper/SearchableWidget/MiniSearcher.h"

#include "Helper/Settings/Settings.h"
#include "Helper/Library/SearchMode.h"
#include "Helper/Library/LibraryInfo.h"

#include <QDirIterator>
#include <algorithm>

struct SearchableFileTreeModel::Private
{
	QStringList	found_strings;
	int			cur_idx;

	Private()
	{
		cur_idx = -1;
	}
};

SearchableFileTreeModel::SearchableFileTreeModel(QObject* parent) :
	SearchModelInterface<QFileSystemModel>(parent)
{
	m = Pimpl::make<Private>();
}

SearchableFileTreeModel::~SearchableFileTreeModel() {}

QModelIndex SearchableFileTreeModel::getFirstRowIndexOf(const QString& substr)
{
	m->cur_idx = -1;
	m->found_strings.clear();

	Settings* settings = Settings::getInstance();
	Library::SearchModeMask mask = settings->get(Set::Lib_SearchMode);
	QString converted_substr = Library::convert_search_string(substr, mask);

	QDirIterator it(this->rootPath(), QDirIterator::Subdirectories);
	QString str;

	while (it.hasNext())
	{
		it.next();

		QString filename = it.fileName();
		filename = Library::convert_search_string(filename, mask);

		if (filename.contains(converted_substr))
		{
			str = it.filePath();

			if(it.fileInfo().isFile()){
				QString parent_folder = Helper::File::get_parent_directory(str);
				str = parent_folder;
			}

			m->found_strings << str;
		}
	}


	if(m->found_strings.size() > 0){
		std::sort(m->found_strings.begin(), m->found_strings.end(), [](const QString& str1 , const QString& str2){
			return (str1 < str2);
		});

		m->found_strings.removeDuplicates();

		str = m->found_strings.first();
		m->cur_idx = 0;
	}

	return index(str);
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
