/* GUI_AlternativeCovers.h */

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
 * GUI_AlternativeCovers.h
 *
 *  Created on: Jul 1, 2011
 *      Author: Lucio Carreras
 */

#ifndef GUI_ALTERNATE_COVERS_H_
#define GUI_ALTERNATE_COVERS_H_

#include "GUI/AlternativeCovers/ui_GUI_AlternativeCovers.h"
#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"
#include "Components/CoverLookup/CoverLocation.h"

#include <QPixmap>
#include <QList>
#include <QModelIndex>

class Album;
class Artist;
class CoverLookupAlternative;
class AlternateCoverItemDelegate;
class AlternateCoverItemModel;

class GUI_AlternativeCovers :
		public SayonaraDialog,
		private Ui::AlternativeCovers
{

	Q_OBJECT
public:
	GUI_AlternativeCovers(QWidget* parent=nullptr);
	virtual ~GUI_AlternativeCovers();

signals:
	void sig_cover_changed(const CoverLocation&);

public slots:
	void start(const Album& album, const CoverLocation& cl = CoverLocation());
	void start(int album_id, quint8 db_id, const CoverLocation& cl = CoverLocation());
	void start(QString album_name, QString artist_name, const CoverLocation& cl = CoverLocation());
	void start(const Artist& artist, const CoverLocation& cl = CoverLocation());
	void start(QString artist_name, const CoverLocation& cl = CoverLocation());

private slots:
	void save_button_pressed();
	void cancel_button_pressed();
	void search_button_pressed();
	void cover_pressed(const QModelIndex& idx);
	void open_file_dialog();
	void cl_new_cover(const CoverLocation& path);
	void cl_finished(bool);

private:

	int						_cur_idx;
	QString					_last_path;
	CoverLocation			_cover_location;
	QList<CoverLocation>	_filelist;
	bool					_is_searching;

	AlternateCoverItemDelegate*		_delegate=nullptr;
	AlternateCoverItemModel*		_model=nullptr;

	CoverLookupAlternative*			_cl_alternative=nullptr;

	void reset_model();
	void connect_and_start();
	void delete_all_files();

protected:
	void closeEvent(QCloseEvent* e) override;
	void language_changed() override;
};

#endif /* GUI_ALTERNATE_COVERS_H_ */
