/* GUI_InfoDialog.h

 * Copyright (C) 2012-2016 Lucio Carreras
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras,
 * Jul 19, 2012
 *
 */

#ifndef GUI_INFODIALOG_H_
#define GUI_INFODIALOG_H_

#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Components/CoverLookup/CoverLocation.h"

#include "GUI/InfoDialog/ui_GUI_InfoDialog.h"

#include <QCloseEvent>

class GUI_TagEdit;
class GUI_AlternativeCovers;
class CoverLookup;
class LyricLookupThread;

class GUI_InfoDialog :
		public SayonaraDialog,
		private Ui::InfoDialog
{

	Q_OBJECT

public:

	enum class Mode : quint8 {
		Tracks=0,
		Albums,
		Artists,
		Invalid

	};

	enum TabIndex {
		TabInfo=0,
		TabLyrics=1,
		TabEdit=2
	};

	GUI_InfoDialog(QWidget* parent=nullptr);
	virtual ~GUI_InfoDialog();

	void set_metadata(const MetaDataList& vd, GUI_InfoDialog::Mode mode);
	void show(GUI_InfoDialog::TabIndex tab);


private slots:
	void lyrics_fetched();
	void lyric_server_changed(int idx);

	void tab_index_changed_int(int idx);
	void tab_index_changed(GUI_InfoDialog::TabIndex idx);

	void cover_clicked();
	void cover_fetched(const CoverLocation&);

	void skin_changed() override;
	void language_changed() override;


private:

	GUI_TagEdit*			_ui_tag_edit=nullptr;
	GUI_AlternativeCovers*	_ui_alternative_covers=nullptr;

	CoverLookup* 			_cover_lookup=nullptr;

	LyricLookupThread*		_lyric_thread=nullptr;

	GUI_InfoDialog::Mode	_cur_mode;

	QString 				_cover_artist;
	QString					_cover_album;
	CoverLocation			_cl;

	MetaDataList			_v_md;
	bool					_is_initialized;


private:

	void init();

	void prepare_cover(const CoverLocation& cover_path);
	void prepare_lyrics();
	void prepare_info(GUI_InfoDialog::Mode mode);

	void closeEvent(QCloseEvent *e) override;
	void showEvent(QShowEvent *e) override;

};

#endif /* GUI_INFODIALOG_H_ */
