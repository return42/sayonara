/* ImportFolderThread.h */

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


#ifndef IMPORTFOLDERTHREAD_H
#define IMPORTFOLDERTHREAD_H

#include <QThread>

class ImportCache;
class MetaDataList;

class CachingThread :
		public QThread
{
	Q_OBJECT

signals:
	void			sig_progress(int);


public:
	CachingThread(const QStringList& file_list, QObject *parent=nullptr);
	virtual ~CachingThread();

	void			cancel();
	bool			is_cancelled() const;
	ImportCache		get_cache() const;
	void			change_metadata(const MetaDataList& v_md_old, const MetaDataList& v_md_new);


private:

	struct Private;
	CachingThread::Private* _m=nullptr;

private:
	void run() override;
};

#endif // IMPORTFOLDERTHREAD_H
