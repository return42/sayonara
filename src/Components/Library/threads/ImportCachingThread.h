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
#include <QMap>
#include <QString>
#include <QStringList>
#include "Helper/MetaData/MetaData.h"

#define IMPORT_MODE_EXTRACT 0
#define IMPORT_MODE_COPY 1

class ImportCachingThread : public QThread
{
	Q_OBJECT


signals:
	void			sig_done();
	void			sig_progress(int);


public:
	ImportCachingThread(QObject *parent=nullptr);


	/**
	 * @brief Set files to be searched for MetaData.
	 * @param files files and/or directories
	 */
	void set_filelist(const QStringList& files);


	/**
	 * @brief Get all files, even images and so on
	 * @return Empty list, if thread was cancelled
	 */
	QStringList get_extracted_files() const;


	/**
	 * @brief get cached MetaDataList.
	 * @return Empty List, if thread was cancelled
	 */
	MetaDataList	get_metadata() const;

	/**
	 * @brief get number of tracks.
	 * @return 0, if thread was cancelled.
	 */
	int				get_n_tracks() const;

	/**
	 * @brief End thread, when work is done. \n
	 * This should prevent the thread from being deleted before data can be fetched
	 */
	void			set_may_terminate();

	/**
	 * @brief Stop work
	 */
	void			set_cancelled();

	/**
	 * @brief query, if thread was cancelled
	 * @return
	 */
	bool			was_cancelled() const;


	/**
	 * @brief get metadata by filepath \n
	 * key: filepath \n
	 * value: MetaData
	 * @return MetaData by filepath
	 */
	QMap<QString, MetaData> get_md_map() const;


	/**
	 * @brief  get source folder of file \n
	 * key: filepath \n
	 * value: src folder where the file has been extracted from \n
	 * This is important if we select multiple directories and import them \n
	 * for example dir a, b, c
	 * @return
	 */
	QMap<QString, QString> get_pd_map() const;

public slots:

	/**
	 * @brief called, when MetaData has been changed by ID3 Editor
	 * @param old_md Old MetaData
	 * @param new_md New MetaData
	 */
	void update_metadata(const MetaDataList& old_md, const MetaDataList& new_md);



private:
	QString                 _src_dir;
	QStringList             _files;
	QStringList             _filelist;
	bool                    _may_terminate;
	bool                    _cancelled;

	// key: filepath
	// val: Metadata of filepath
	QMap<QString, MetaData> _md_map;

	// key: filepath
	// val: src folder where the file has been extracted from
	QMap<QString, QString>	_pd_map;

	MetaDataList			_v_md;


private:
	void run();



};

#endif // IMPORTFOLDERTHREAD_H
