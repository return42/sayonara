/* ReloadThread.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * ReloadThread.h
 *
 *  Created on: Jun 19, 2011
 *      Author: luke
 */

#ifndef RELOADTHREAD_H_
#define RELOADTHREAD_H_


#include <QThread>
#include <QMap>

#include "Helper/Tagging/Tagging.h"
#include "Helper/SayonaraClass.h"

class DatabaseConnector;
class ReloadThread :
		public QThread,
		protected SayonaraClass
{

Q_OBJECT

signals:
	void sig_reloading_library(const QString& message, int progress);
    void sig_new_block_saved();


public:

	SINGLETON_QOBJECT(ReloadThread)

	void set_lib_path(const QString& library_path);

    void pause();
    void goon();
	bool is_running() const;


protected:
	virtual void run() override;


private:
	DatabaseConnector*		_db=nullptr;
	QString					_library_path;
	MetaDataList			_v_md;
	Tagging::Quality		_quality;
	bool					_paused, _running;


private:
    int get_and_save_all_files(const QMap<QString, MetaData>& v_md_map);
	void get_files_recursive (QDir base_dir, MetaDataList& v_md, int* n_files);
	void process_sub_files(const QDir& dir, const QStringList& sub_files);

};

#endif /* RELOADTHREAD_H_ */
