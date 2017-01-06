/* FileSystemWatcher.h */

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

#ifndef FILESYSTEMWATCHER_H
#define FILESYSTEMWATCHER_H

#include <QThread>
#include <QStringList>

class FileSystemWatcher : public QThread
{
	Q_OBJECT

signals:
	void sig_changed();

public:
	FileSystemWatcher(const QString& library_path, QObject* parent);
	void refresh();
	void stop();

protected:
	void run() override;

private:
	QStringList			_indexed_files;
	QString				_library_path;
	bool				_may_run;
	bool				_refresh;
	bool				_waiting;

	QStringList index_files(const QString& root);
};

#endif // FILESYSTEMWATCHER_H
