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

#include <QCloseEvent>
#include <QDateTime>
#include <QTabBar>
#include <QWidget>

class GUI_TagEdit;
class LyricLookupThread;
class InfoDialogContainer;
class MetaDataList;
class CoverLocation;


namespace Ui
{
	class InfoDialog;
}

/**
 * @brief The GUI_InfoDialog class
 * @ingroup GUI
 */
class GUI_InfoDialog :
		public SayonaraDialog
{

	Q_OBJECT

public:

	enum class Tab : quint8 {
		Info=0,
		Lyrics=1,
		Edit=2
	};

	GUI_InfoDialog(InfoDialogContainer* container, QWidget* parent=nullptr);
	virtual ~GUI_InfoDialog();

	void set_metadata(const MetaDataList& vd, MetaDataList::Interpretation interpretation);
	bool has_metadata() const;
	void show(GUI_InfoDialog::Tab tab);


private slots:
	void lyrics_fetched();
	void lyric_server_changed(int idx);

	void tab_index_changed_int(int idx);
	void tab_index_changed(GUI_InfoDialog::Tab idx);

	void skin_changed() override;
	void language_changed() override;

private:

	void init();

	void prepare_cover(const CoverLocation& cover_path);
	void prepare_lyrics();
	void prepare_info(MetaDataList::Interpretation mode);

	void closeEvent(QCloseEvent *e) override;
	void showEvent(QShowEvent *e) override;


private:

	struct GUI_InfoDialogMembersPrivate;
	GUI_InfoDialogMembersPrivate*	_m=nullptr;
	Ui::InfoDialog*			ui=nullptr;

};

#endif /* GUI_INFODIALOG_H_ */
