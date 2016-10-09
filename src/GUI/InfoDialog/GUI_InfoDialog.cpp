/* GUI_InfoDialog.cpp

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

#include "GUI_InfoDialog.h"
#include "GUI/InfoDialog/ui_GUI_InfoDialog.h"

#include "InfoDialogContainer.h"

#include "GUI/TagEdit/GUI_TagEdit.h"
#include "GUI/Helper/IconLoader/IconLoader.h"

#include "Components/Covers/CoverLocation.h"
#include "Components/LyricLookup/LyricLookup.h"
#include "Components/TagEdit/TagEdit.h"

#include "Helper/MetaDataInfo/MetaDataInfo.h"
#include "Helper/MetaDataInfo/AlbumInfo.h"
#include "Helper/MetaDataInfo/ArtistInfo.h"
#include "Helper/FileHelper.h"
#include "Helper/globals.h"

#include <algorithm>

struct GUI_InfoDialog::Private
{
	InfoDialogContainer*	info_dialog_container=nullptr;
	GUI_TagEdit*			ui_tag_edit=nullptr;
	CoverLocation			cl;
	MetaDataList			v_md;
	MetaDataList::Interpretation	md_interpretation;
};


GUI_InfoDialog::GUI_InfoDialog(InfoDialogContainer* container, QWidget* parent) :
	SayonaraDialog(parent)
{
	ui = nullptr;
	_m = new GUI_InfoDialog::Private();

	_m->info_dialog_container = container;
	_m->md_interpretation = MetaDataList::Interpretation::None;
}

GUI_InfoDialog::~GUI_InfoDialog()
{
	delete _m; _m = nullptr;
	if(ui){
		delete ui;
		ui = nullptr;
	}
}

void GUI_InfoDialog::language_changed()
{
	if(!ui){
		return;
	}

	ui->retranslateUi(this);

	prepare_info(_m->md_interpretation);
}

void GUI_InfoDialog::skin_changed()
{
	if(!ui){
		return;
	}

	QTabBar* tab_bar = ui->tab_widget->tabBar();
	if(tab_bar)
	{
		IconLoader* icon_loader = IconLoader::getInstance();
		tab_bar->setTabIcon(0, icon_loader->get_icon("dialog-info", "info"));
		tab_bar->setTabIcon(1, icon_loader->get_icon("document-properties", "lyrics"));
		tab_bar->setTabIcon(2, icon_loader->get_icon("accessories-text-editor", "edit"));
	}
}


void GUI_InfoDialog::lyric_server_changed(int idx)
{
	Q_UNUSED(idx)
	prepare_lyrics();
}


void GUI_InfoDialog::prepare_lyrics()
{
	if(!ui){
		return;
	}

	if(_m->v_md.size() != 1){
		return;
	}

	int cur_idx = ui->combo_servers->currentIndex();

	LyricLookupThread* lyric_thread = new LyricLookupThread(this);
	connect(lyric_thread, &LyricLookupThread::sig_finished, this, &GUI_InfoDialog::lyrics_fetched);

	if(ui->combo_servers->count() == 0){
		QStringList lyric_server_list = lyric_thread->get_servers();
		for(const QString& server : lyric_server_list) {
			ui->combo_servers->addItem(server);
		}
		cur_idx = 0;
	}

	ui->te_lyrics->setText("");
	ui->pb_loading->setVisible(true);

	lyric_thread->run(_m->v_md.first().artist, _m->v_md.first().title, cur_idx);
}


void GUI_InfoDialog::lyrics_fetched()
{
	LyricLookupThread* lyric_thread = static_cast<LyricLookupThread*>(sender());

	if(!ui){
		lyric_thread->deleteLater();
		return;
	}

	ui->pb_loading->setVisible(false);

	QString lyrics = lyric_thread->get_lyric_data();
	lyrics = lyrics.trimmed();

	int height = ui->te_lyrics->height();
	int width = ui->tab_2->size().width();
	ui->te_lyrics->resize(width, height);
	ui->te_lyrics->setAcceptRichText(true);
	ui->te_lyrics->setText(lyrics);
	ui->te_lyrics->setLineWrapColumnOrWidth(ui->te_lyrics->width());
	ui->te_lyrics->setLineWrapMode(QTextEdit::FixedPixelWidth);
	ui->te_lyrics->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->te_lyrics->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	sender()->deleteLater();
}


void GUI_InfoDialog::prepare_info(MetaDataList::Interpretation md_interpretation)
{
	if(!ui){
		return;
	}

	MetaDataInfo* info;

	switch (md_interpretation)
	{
		case MetaDataList::Interpretation::Artists:
			info = new ArtistInfo(_m->v_md);
			break;
		case MetaDataList::Interpretation::Albums:
			info = new AlbumInfo(_m->v_md);
			break;

		case MetaDataList::Interpretation::Tracks:
			info = new MetaDataInfo(_m->v_md);
			break;

		default:
			return;
	}

	QString info_text = info->get_info_as_string() + CAR_RET + CAR_RET +
			info->get_additional_info_as_string();

	ui->lab_title->setText(info->get_header());
	ui->lab_subheader->setText(info->get_subheader());
	ui->lab_info->setText(info_text);
	ui->lab_paths->setOpenExternalLinks(true);
	ui->lab_paths->setText(info->get_paths_as_string());

	_m->cl = info->get_cover_location();

	prepare_cover(_m->cl);

	delete info;
}


void GUI_InfoDialog::set_metadata(const MetaDataList& v_md, MetaDataList::Interpretation md_interpretation)
{
	_m->md_interpretation = md_interpretation;
	_m->v_md = v_md;

	prepare_info(md_interpretation);
}

bool GUI_InfoDialog::has_metadata() const
{
	return (_m->v_md.size() > 0);
}

void GUI_InfoDialog::tab_index_changed_int(int idx)
{
	idx = std::min( (int) GUI_InfoDialog::Tab::Edit, idx);
	idx = std::max( (int) GUI_InfoDialog::Tab::Info, idx);

	tab_index_changed( (GUI_InfoDialog::Tab) idx );
}

void GUI_InfoDialog::tab_index_changed(GUI_InfoDialog::Tab idx)
{
	if(!ui){
		return;
	}

	ui->ui_info_widget->hide();
	ui->ui_lyric_widget->hide();
	_m->ui_tag_edit->hide();

	switch(idx)
	{
		case GUI_InfoDialog::Tab::Edit:

			ui->tab_widget->setCurrentWidget(_m->ui_tag_edit);
			{
				MetaDataList local_md;
				for(const MetaData& md : _m->v_md){
					if(!Helper::File::is_www(md.filepath())){
						local_md << md;
					}
				}

				if(local_md.size() > 0){
					_m->ui_tag_edit->get_tag_edit()->set_metadata(local_md);
				}
			}

			_m->ui_tag_edit->show();
			break;

		case GUI_InfoDialog::Tab::Lyrics:

			ui->tab_widget->setCurrentWidget(ui->ui_lyric_widget);
			ui->ui_lyric_widget->show();
			prepare_lyrics();
			break;

		default:
			ui->tab_widget->setCurrentWidget(ui->ui_info_widget);
			ui->ui_info_widget->show();
			prepare_cover(_m->cl);
			break;
	}
}

void GUI_InfoDialog::show(GUI_InfoDialog::Tab tab)
{
	if(!ui){
		init();
	}

	QTabWidget* tab_widget = ui->tab_widget;

	prepare_cover(CoverLocation::getInvalidLocation());

	bool lyric_enabled = (_m->v_md.size() == 1);
	bool tag_edit_enabled = std::any_of(_m->v_md.begin(), _m->v_md.end(), [](const MetaData& md){
		return (!Helper::File::is_www(md.filepath()));
	});

	tab_widget->setTabEnabled((int) GUI_InfoDialog::Tab::Edit, tag_edit_enabled);
	tab_widget->setTabEnabled((int) GUI_InfoDialog::Tab::Lyrics, lyric_enabled);

	if( !tab_widget->isTabEnabled((int) tab) )
	{
		tab = GUI_InfoDialog::Tab::Info;
	}

	if(tab == GUI_InfoDialog::Tab::Edit)
	{
		MetaDataList local_md;
		for(const MetaData& md : _m->v_md){
			if(!Helper::File::is_www(md.filepath())){
				local_md << md;
			}
		}

		if(local_md.size() > 0){
			_m->ui_tag_edit->get_tag_edit()->set_metadata(local_md);
		}
	}

	tab_widget->setCurrentIndex((int) tab);
	tab_index_changed(tab);

	SayonaraDialog::show();
}


void GUI_InfoDialog::prepare_cover(const CoverLocation& cl)
{
	ui->btn_image->set_cover_location(cl);
}


void GUI_InfoDialog::init()
{
	ui = new Ui::InfoDialog();
	ui->setupUi(this);

	QLayout* tab3_layout = ui->tab_3->layout();
	QTabWidget* tab_widget = ui->tab_widget;

	if(tab3_layout){
		_m->ui_tag_edit = new GUI_TagEdit(ui->tab_3);
		tab3_layout->addWidget(_m->ui_tag_edit);
	}

	ui->combo_servers->setCurrentIndex(0);

	connect(tab_widget, &QTabWidget::currentChanged, this, &GUI_InfoDialog::tab_index_changed_int);
	connect(_m->ui_tag_edit, &GUI_TagEdit::sig_cancelled, this, &GUI_InfoDialog::close);
	connect(ui->combo_servers, combo_current_index_changed_int, this, &GUI_InfoDialog::lyric_server_changed);

	ui->btn_image->setStyleSheet("QPushButton:hover {background-color: transparent;}");

	tab_widget->setCurrentIndex((int) GUI_InfoDialog::Tab::Info);
	tab_widget->setFocusPolicy(Qt::NoFocus);

	language_changed();
	skin_changed();

	prepare_info(_m->md_interpretation);
}


void GUI_InfoDialog::closeEvent(QCloseEvent* e)
{
	SayonaraDialog::closeEvent(e);

	_m->info_dialog_container->info_dialog_closed();
}

void GUI_InfoDialog::showEvent(QShowEvent *e)
{
	if(!ui){
		init();
	}

	SayonaraDialog::showEvent(e);
}
