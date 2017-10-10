/* IndexDirectoriesThread.h */

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

#ifndef INDEXDIRECTORIESTHREAD_H
#define INDEXDIRECTORIESTHREAD_H

#include <QStringList>
#include <QThread>
#include "Utils/Pimpl.h"

class MetaDataList;
class IndexDirectoriesThread :
		public QThread
{
	Q_OBJECT
	PIMPL(IndexDirectoriesThread)

public:
	explicit IndexDirectoriesThread(const MetaDataList& v_md);
	virtual ~IndexDirectoriesThread();

	QStringList get_directories() const;

protected:
	void run() override;
};

#endif // INDEXDIRECTORIESTHREAD_H
