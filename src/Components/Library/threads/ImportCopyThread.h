/* ImportCopyThread.h */

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



#ifndef IMPORT_COPY_THREAD_H
#define IMPORT_COPY_THREAD_H

#include <QThread>
#include <QString>
#include <QMap>
#include <QString>
#include <QStringList>

#include "Helper/MetaData/MetaData.h"
#include "Helper/SayonaraClass.h"


class ImportCopyThread : public QThread, protected SayonaraClass
{
    Q_OBJECT

public:

	enum class Mode : quint8 {
		Copy=0,
		Rollback
	};

    explicit ImportCopyThread(QObject *parent=nullptr);


	void set_vars(	const QString& target_dir,
					const QStringList& files,
					const QMap<QString, MetaData>& md_map,
					const QMap<QString, QString>& pd_map
				  );

	int get_n_files() const;
	int get_copied_files() const;

	MetaDataList get_metadata() const;

	bool get_cancelled() const;
    void set_cancelled();

	void set_mode(ImportCopyThread::Mode mode);
	ImportCopyThread::Mode  get_mode() const;


private:
	QString        _lib_dir;
	QString        _chosen_dir;
	QStringList    _files;

	QMap<QString, MetaData> _md_map;
	QMap<QString, QString> _pd_map;

	MetaDataList	_v_md;
	QStringList		_lst_copied_files;
	QStringList		_created_dirs;
	int				_n_files;
	int				_copied_files;
	Mode			_mode;
	int				_percent;
	bool			_cancelled;


private:

	void run();
	void copy();
	void rollback();
    void emit_percent(int i, int n);


signals:
    void sig_progress(int);


};


#endif
