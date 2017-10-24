
/* GUI_Bookmarks.cpp */

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

#include "GUI_Bookmarks.h"

#include "GUI/Plugins/Bookmarks/ui_GUI_Bookmarks.h"

#include "Utils/Utils.h"
#include "Utils/MetaData/MetaData.h"
#include "Utils/Message/Message.h"
#include "Utils/Language.h"

#include "Components/Bookmarks/Bookmark.h"
#include "Components/Bookmarks/Bookmarks.h"

static const QString NoBookmarkText("--:--");

GUI_Bookmarks::GUI_Bookmarks(QWidget *parent) :
	PlayerPluginInterface(parent) {}

GUI_Bookmarks::~GUI_Bookmarks()
{
	if(ui)
	{
		delete ui; ui=nullptr;
	}
}

QString GUI_Bookmarks::get_name() const
{
	return "Bookmarks";
}

QString GUI_Bookmarks::get_display_name() const
{
	return Lang::get(Lang::Bookmarks);
}


void GUI_Bookmarks::retranslate_ui()
{
	ui->retranslateUi(this);
}


void GUI_Bookmarks::init_ui()
{
	if(is_ui_initialized()){
		return;
	}

	_bookmarks = new Bookmarks(this);

	setup_parent(this, &ui);

	connect(_bookmarks, &Bookmarks::sig_bookmarks_changed, this, &GUI_Bookmarks::bookmarks_changed);
	connect(_bookmarks, &Bookmarks::sig_next_changed, this, &GUI_Bookmarks::next_changed);
	connect(_bookmarks, &Bookmarks::sig_prev_changed, this, &GUI_Bookmarks::prev_changed);

	connect(ui->btn_tool, &MenuToolButton::sig_new, this, &GUI_Bookmarks::new_clicked);
	connect(ui->btn_tool, &MenuToolButton::sig_delete, this, &GUI_Bookmarks::del_clicked);
	connect(ui->btn_bw, &QPushButton::clicked, this, &GUI_Bookmarks::prev_clicked);
	connect(ui->btn_fw, &QPushButton::clicked, this, &GUI_Bookmarks::next_clicked);
	connect(ui->cb_loop, &QCheckBox::clicked, this, &GUI_Bookmarks::loop_clicked);
	connect(ui->cb_bookmarks, combo_current_index_changed_int, this, &GUI_Bookmarks::combo_changed);

	ui->btn_tool->show_action(ContextMenu::EntryNew, false);
	ui->btn_tool->show_action(ContextMenu::EntryDelete, false);

	disable_prev();
	disable_next();

	bookmarks_changed();
}


void GUI_Bookmarks::bookmarks_changed()
{
	if(!is_ui_initialized()){
		return;
	}

	QList<Bookmark> bookmarks = _bookmarks->get_all_bookmarks();

	disconnect(ui->cb_bookmarks, combo_current_index_changed_int, this, &GUI_Bookmarks::combo_changed);

	ui->cb_bookmarks->clear();
	for(const Bookmark& bookmark : bookmarks){
		ui->cb_bookmarks->addItem(bookmark.get_name());
	}

	MetaData md = _bookmarks->get_cur_track();

	ui->btn_tool->show_action(ContextMenu::EntryNew, (md.id >= 0) );
	ui->btn_tool->show_action(ContextMenu::EntryDelete, !bookmarks.isEmpty() );

	if(md.id >= 0 && bookmarks.size() > 0){
		ui->stackedWidget->setCurrentIndex(0);
	}

	else{
		ui->stackedWidget->setCurrentIndex(1);
	}

	connect(ui->cb_bookmarks, combo_current_index_changed_int, this, &GUI_Bookmarks::combo_changed);
}


void GUI_Bookmarks::disable_prev()
{
	if(!is_ui_initialized()){
		return;
	}

	ui->btn_bw->setEnabled( false );
	ui->lab_last->setText( NoBookmarkText );
}


void GUI_Bookmarks::disable_next()
{
	if(!is_ui_initialized()){
		return;
	}

	ui->btn_fw->setEnabled(false);
	ui->lab_next->setText( NoBookmarkText );
}


void GUI_Bookmarks::prev_changed(const Bookmark& bookmark)
{
	if(!is_ui_initialized()){
		return;
	}

	ui->btn_bw->setEnabled( bookmark.is_valid() );
	ui->cb_loop->setEnabled( ui->btn_fw->isEnabled() );

	if( !bookmark.is_valid() ){
		disable_prev();
		return;
	}

	ui->lab_last->setText(Util::cvt_ms_to_string(bookmark.get_time() * 1000, true, true, false));
}


void GUI_Bookmarks::next_changed(const Bookmark& bookmark)
{
	if(!is_ui_initialized()){
		return;
	}

	ui->btn_fw->setEnabled( bookmark.is_valid() );
	ui->cb_loop->setEnabled( ui->btn_fw->isEnabled() );

	if( !bookmark.is_valid() ){
		disable_next();
		return;
	}

	ui->lab_next->setText(Util::cvt_ms_to_string(bookmark.get_time() * 1000, true, true, false));
}


void GUI_Bookmarks::combo_changed(int cur_idx)
{
	ui->btn_tool->show_action(ContextMenu::EntryDelete, (cur_idx >= 0));

	if(cur_idx >= 0){
		_bookmarks->jump_to(cur_idx);
	}
}


void GUI_Bookmarks::next_clicked()
{
	_bookmarks->jump_next();
}


void GUI_Bookmarks::prev_clicked()
{
	_bookmarks->jump_prev();
}


void GUI_Bookmarks::loop_clicked(bool b)
{
	bool success = _bookmarks->set_loop(b);
	if(!success){
		ui->cb_loop->setChecked(success);
	}
}


void GUI_Bookmarks::new_clicked()
{
	Bookmarks::CreationStatus status = _bookmarks->create();
	if( status == Bookmarks::CreationStatus::NoDBTrack ){
		Message::warning(tr("Sorry, bookmarks can only be set for library tracks at the moment."),
						Lang::get(Lang::Bookmarks));
	}
}


void GUI_Bookmarks::del_clicked()
{
	int idx = ui->cb_bookmarks->currentIndex();
	_bookmarks->remove(idx);
}
