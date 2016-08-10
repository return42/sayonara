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

#include "GUI/InfoDialog/GUI_InfoDialog.h"
#include "GUI/TagEdit/GUI_TagEdit.h"
#include "GUI/AlternativeCovers/GUI_AlternativeCovers.h"

#include "Components/CoverLookup/CoverLookup.h"
#include "Components/LyricLookup/LyricLookup.h"
#include "Components/TagEdit/TagEdit.h"

#include "Helper/MetaDataInfo/MetaDataInfo.h"
#include "Helper/MetaDataInfo/AlbumInfo.h"
#include "Helper/MetaDataInfo/ArtistInfo.h"
#include "Helper/FileHelper.h"
#include "GUI/Helper/IconLoader/IconLoader.h"

#include <QPixmap>
#include <QCloseEvent>
#include <QPainter>
#include <QDateTime>
#include <QTabBar>


GUI_InfoDialog::GUI_InfoDialog(QWidget* parent) :
	SayonaraDialog(parent),
	Ui::InfoDialog()
{
	_is_initialized = false;
	_cur_mode = Mode::Invalid;
}

GUI_InfoDialog::~GUI_InfoDialog() {
}

void GUI_InfoDialog::language_changed() {

	if(!_is_initialized){
		return;
	}

	retranslateUi(this);

	prepare_info(_cur_mode);
}

void GUI_InfoDialog::skin_changed(){
	if(!_is_initialized){
		return;
	}

	QTabBar* tab_bar = tab_widget->tabBar();
	if(tab_bar){
		IconLoader* icon_loader = IconLoader::getInstance();
		tab_bar->setTabIcon(0, icon_loader->get_icon("dialog-info", "info"));
		tab_bar->setTabIcon(1, icon_loader->get_icon("document-properties", "lyrics"));
		tab_bar->setTabIcon(2, icon_loader->get_icon("accessories-text-editor", "edit"));
	}
}


void GUI_InfoDialog::lyric_server_changed(int idx) {
	Q_UNUSED(idx)
	prepare_lyrics();
}


void GUI_InfoDialog::prepare_lyrics() {

	if(!_is_initialized){
		return;
	}

	if(_v_md.size() != 1){
		return;
	}

	if(!_lyric_thread){
		_lyric_thread = new LyricLookupThread(this);

		QStringList lyric_server_list = _lyric_thread->get_servers();
		for(const QString& server : lyric_server_list) {
			combo_servers->addItem(server);
		}

		connect(_lyric_thread, &LyricLookupThread::sig_finished, this, &GUI_InfoDialog::lyrics_fetched);
	}

	te_lyrics->setText("");
	pb_loading->setVisible(true);

	_lyric_thread->run(_v_md[0].artist, _v_md[0].title, combo_servers->currentIndex());
}


void GUI_InfoDialog::lyrics_fetched() {

	if(!_is_initialized){
		return;
	}

	pb_loading->setVisible(false);

	QString lyrics = _lyric_thread->get_lyric_data();
	lyrics = lyrics.trimmed();

	int height, width;
	height = te_lyrics->height();
	width = tab_2->size().width();
	te_lyrics->resize(width, height);
	te_lyrics->setAcceptRichText(true);
	te_lyrics->setText(lyrics);
	te_lyrics->setLineWrapColumnOrWidth(te_lyrics->width());
	te_lyrics->setLineWrapMode(QTextEdit::FixedPixelWidth);
	te_lyrics->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	te_lyrics->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}


void GUI_InfoDialog::prepare_info(GUI_InfoDialog::Mode mode) {

	if(!_is_initialized){
		return;
	}

	MetaDataInfo* info;

	switch (mode){
		case GUI_InfoDialog::Mode::Artists:
			info = new ArtistInfo(_v_md);
			break;
		case GUI_InfoDialog::Mode::Albums:
			info = new AlbumInfo(_v_md);
			break;

		case GUI_InfoDialog::Mode::Tracks:
			info = new MetaDataInfo(_v_md);
			break;

		default:
			return;

	}

	QString info_text = info->get_info_as_string() + CAR_RET + CAR_RET +
			info->get_additional_info_as_string();

	lab_title->setText(info->get_header());
	lab_subheader->setText(info->get_subheader());

	lab_info->setText(info_text);
	lab_paths->setOpenExternalLinks(true);
	lab_paths->setText(info->get_paths_as_string());

	_cl = info->get_cover_location();

	if(_cl.valid){
		_cover_artist = info->get_cover_artist();
		_cover_album = info->get_cover_album();
	}

	else {
		_cover_artist.clear();
		_cover_album.clear();
	}

	delete info;
}


void GUI_InfoDialog::alternative_cover_fetched(const CoverLocation& cl){
	cover_fetched(cl);
}

void GUI_InfoDialog::cover_fetched(const CoverLocation& cl) {

	if(!_is_initialized){
		return;
	}

	QIcon icon(cl.cover_path);
	if(icon.isNull()) return;

	btn_image->setIcon(icon);
	btn_image->update();
	btn_image->repaint();
}


void GUI_InfoDialog::set_metadata(const MetaDataList& v_md, GUI_InfoDialog::Mode mode) {

	_cur_mode = mode;
	_v_md = v_md;

	prepare_info(mode);
}

void GUI_InfoDialog::tab_index_changed_int(int idx){
	GUI_InfoDialog::TabIndex tab;
	if( idx != GUI_InfoDialog::TabIndex::TabInfo &&
		idx != GUI_InfoDialog::TabIndex::TabLyrics &&
		idx != GUI_InfoDialog::TabIndex::TabEdit)
	{
		tab = GUI_InfoDialog::TabIndex::TabInfo;
	}

	else{
		tab = (GUI_InfoDialog::TabIndex) (idx);
	}

	tab_index_changed(tab);
}

void GUI_InfoDialog::tab_index_changed(GUI_InfoDialog::TabIndex idx){

	if(!_is_initialized){
		return;
	}

	ui_info_widget->hide();
	ui_lyric_widget->hide();
	_ui_tag_edit->hide();

	switch(idx){

		case GUI_InfoDialog::TabEdit:

			tab_widget->setCurrentWidget(_ui_tag_edit);
			{

				MetaDataList local_md = _v_md.extract_tracks( [](const MetaData& md){
					return !Helper::File::is_www(md.filepath());
				});

				if(local_md.size() > 0){
					_ui_tag_edit->get_tag_edit()->set_metadata(local_md);
				}
			}

			_ui_tag_edit->show();
			break;

		case GUI_InfoDialog::TabLyrics:


			tab_widget->setCurrentWidget(ui_lyric_widget);
			ui_lyric_widget->show();
			prepare_lyrics();
			break;

		default:
			tab_widget->setCurrentWidget(ui_info_widget);
			ui_info_widget->show();
			prepare_cover(_cl);
			break;
	}
}

void GUI_InfoDialog::show(GUI_InfoDialog::TabIndex tab) {

	if(!_is_initialized){
		init();
	}

	cover_fetched(CoverLocation::getInvalidLocation());

	bool tag_edit_enabled = std::any_of(_v_md.begin(), _v_md.end(), [](const MetaData& md){
		return (!Helper::File::is_www(md.filepath()));
	});

	tab_widget->setTabEnabled(GUI_InfoDialog::TabEdit, tag_edit_enabled);

	bool lyric_enabled = (_v_md.size() == 1);
	tab_widget->setTabEnabled(GUI_InfoDialog::TabLyrics, lyric_enabled);

	if( !tab_widget->isTabEnabled(tab) )
	{
		tab = GUI_InfoDialog::TabInfo;
	}

	if(tab == TabEdit){

		MetaDataList local_md = _v_md.extract_tracks([](const MetaData& md){
			return (!Helper::File::is_www(md.filepath()));
		});

		if(local_md.size() > 0){
			_ui_tag_edit->get_tag_edit()->set_metadata(local_md);
		}
	}

	tab_widget->setCurrentIndex(tab);
	tab_index_changed(tab);

	_is_initialized = true;

	SayonaraDialog::show();
}


void GUI_InfoDialog::prepare_cover(const CoverLocation& cover_location) {

	if(!cover_location.valid) {
		return;
	}

	_cover_lookup->fetch_cover(cover_location);
}


void GUI_InfoDialog::cover_clicked() {

	setFocus();

	if(!_ui_alternative_covers){
		_ui_alternative_covers = new GUI_AlternativeCovers(this);
		connect(_ui_alternative_covers, &GUI_AlternativeCovers::sig_cover_changed, this, &GUI_InfoDialog::alternative_cover_fetched);
	}

	if(_cover_artist.size() > 0 && _cover_album.size() > 0){
		_ui_alternative_covers->start(_cover_album, _cover_artist, _cl);
	}

	else if(_cover_artist.size() > 0){
		_ui_alternative_covers->start(_cover_artist, _cl);
	}

	else if(_cover_album.size() > 0){
		_ui_alternative_covers->start(_cover_album, "Various artists", _cl);
	}
}

void GUI_InfoDialog::init() {

	setupUi(this);

	QLayout* tab3_layout;

	_cover_lookup = new CoverLookup(this);

	tab3_layout = tab_3->layout();
	if(tab3_layout){
		_ui_tag_edit = new GUI_TagEdit(tab_3);
		tab3_layout->addWidget(_ui_tag_edit);
	}

	combo_servers->setCurrentIndex(0);

	connect(_cover_lookup, &CoverLookup::sig_cover_found, this, &GUI_InfoDialog::cover_fetched);
	connect(tab_widget, &QTabWidget::currentChanged, this, &GUI_InfoDialog::tab_index_changed_int);
	connect(_ui_tag_edit, &GUI_TagEdit::sig_cancelled, this, &GUI_InfoDialog::close);
	connect(combo_servers, combo_current_index_changed_int, this, &GUI_InfoDialog::lyric_server_changed);

	connect(btn_image, &QPushButton::clicked, this, &GUI_InfoDialog::cover_clicked);

	btn_image->setStyleSheet("QPushButton:hover {background-color: transparent;}");

	tab_widget->setCurrentIndex(GUI_InfoDialog::TabInfo);
	tab_widget->setFocusPolicy(Qt::NoFocus);

	_is_initialized = true;

	language_changed();
	skin_changed();

	prepare_info(_cur_mode);
}


void GUI_InfoDialog::closeEvent(QCloseEvent* e) {
	SayonaraDialog::closeEvent(e);
}

void GUI_InfoDialog::showEvent(QShowEvent *e)
{
	if(!_is_initialized){
		init();
	}

	SayonaraDialog::showEvent(e);
}
