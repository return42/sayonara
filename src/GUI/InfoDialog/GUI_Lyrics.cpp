/* GUI_Lyrics.cpp */

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



#include "GUI_Lyrics.h"

#include "GUI/InfoDialog/ui_GUI_Lyrics.h"
#include "Helper/Logger/Logger.h"
#include "GUI/Helper/SayonaraWidget/SayonaraCompleter.h"
#include "Components/Lyrics/LyricLookup.h"
#include "Helper/MetaData/MetaData.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidgetTemplate.h"
#include "Helper/Language.h"
#include "Helper/Settings/Settings.h"

#include <QWheelEvent>
#include <QShowEvent>

#include <cmath>

struct GUI_Lyrics::Private
{
	MetaData md;
	qreal font_size;
	qreal initial_font_size;
};

GUI_Lyrics::GUI_Lyrics(QWidget *parent) :
	SayonaraWidget(parent)
{
	_m = Pimpl::make<Private>();
}

GUI_Lyrics::~GUI_Lyrics()
{
	if(ui){
		delete ui;
	}
	ui = nullptr;
}


void GUI_Lyrics::init()
{
	if(ui){
		return;
	}

	ui = new Ui::GUI_Lyrics();
	ui->setupUi(this);

	ui->te_lyrics->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->te_lyrics->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->te_lyrics->setReadOnly(true);

	ui->pb_progress->hide();

	QString server = _settings->get(Set::Lyrics_Server);
	LyricLookupThread* lyric_thread = new LyricLookupThread();
	QStringList servers = lyric_thread->get_servers();
	ui->combo_servers->addItems(servers);
	int idx = ui->combo_servers->findText(server);
	if(idx < 0){
		idx = 0;
	}

	ui->combo_servers->setCurrentIndex(idx);

	delete lyric_thread; lyric_thread = nullptr;

	_m->font_size = QApplication::font().pointSizeF();
	_m->initial_font_size = QApplication::font().pointSizeF();
	int percent = _settings->get(Set::Lyrics_Zoom);
	ui->sb_zoom->setValue(percent);
	zoom( (percent * _m->initial_font_size) / 100.0 );

	connect(ui->combo_servers, combo_current_index_changed_int, this, &GUI_Lyrics::lyric_server_changed);
	connect(ui->btn_search, &QPushButton::clicked, this, &GUI_Lyrics::lyric_search_button_pressed);
	connect(ui->btn_close, &QPushButton::clicked, this, &GUI_Lyrics::sig_closed);
	connect(ui->btn_close, &QPushButton::clicked, this, &GUI_Lyrics::close);
	connect(ui->btn_switch, &QPushButton::clicked, this, &GUI_Lyrics::switch_pressed);
	connect(ui->sb_zoom, spinbox_value_changed_int, this, [=](int percent){
		zoom( (percent * _m->initial_font_size) / 100.0 );
	});

	set_metadata(_m->md);
	language_changed();
}


void GUI_Lyrics::lyric_server_changed(int idx)
{
	Q_UNUSED(idx)

	_settings->set(Set::Lyrics_Server, ui->combo_servers->currentText());
	prepare_lyrics();
}

void GUI_Lyrics::lyric_search_button_pressed()
{
	prepare_lyrics();
}

void GUI_Lyrics::prepare_lyrics()
{
	if(!ui){
		return;
	}

	ui->te_lyrics->clear();

	QString artist, title;
	artist = ui->le_artist->text();
	title = ui->le_title->text();

	if(artist.isEmpty() || title.isEmpty()){
		return;
	}

	int cur_idx = ui->combo_servers->currentIndex();

	LyricLookupThread* lyric_thread = new LyricLookupThread(this);
	connect(lyric_thread, &LyricLookupThread::sig_finished, this, &GUI_Lyrics::lyrics_fetched);

	if(ui->combo_servers->count() == 0){
		QStringList lyric_server_list = lyric_thread->get_servers();
		for(const QString& server : lyric_server_list) {
			ui->combo_servers->addItem(server);
		}

		cur_idx = 0;
	}

	ui->pb_progress->setVisible(true);
	ui->btn_search->setEnabled(false);
	ui->combo_servers->setEnabled(false);

	lyric_thread->run(artist, title, cur_idx);
}

void GUI_Lyrics::language_changed()
{
	if(!ui){
		return;
	}

	ui->lab_artist->setText(Lang::get(Lang::Artist));
	ui->lab_tit->setText(Lang::get(Lang::Title));
	ui->lab_zoom->setText(Lang::get(Lang::Zoom));
	ui->lab_server->setText(tr("Server"));
}


void GUI_Lyrics::lyrics_fetched()
{
	LyricLookupThread* lyric_thread = static_cast<LyricLookupThread*>(sender());

	if(!ui){
		lyric_thread->deleteLater();
		return;
	}

	QString lyrics = lyric_thread->get_lyric_data().trimmed();

	ui->te_lyrics->setHtml(lyrics);

	ui->pb_progress->setVisible(false);
	ui->btn_search->setEnabled(true);
	ui->combo_servers->setEnabled(true);

	sender()->deleteLater();
}

void GUI_Lyrics::set_metadata(const MetaData &md)
{
	_m->md = md;
	if(!ui){
		return;
	}

	guess_artist_and_title(md);

	QStringList completer_entries;
	completer_entries << md.artist << md.album_artist();
	completer_entries.removeDuplicates();

	if(ui->le_artist->completer() != nullptr){
		ui->le_artist->completer()->deleteLater();
	}

	ui->le_artist->setCompleter( new SayonaraCompleter(completer_entries, ui->le_artist) );

	if(this->isVisible()){
		prepare_lyrics();
	}
}

void GUI_Lyrics::guess_artist_and_title(const MetaData& md)
{
	bool guessed = false;

	if(md.radio_mode() == RadioMode::Station &&
			md.artist.contains("://"))
	{
		if(md.title.contains("-")){
			QStringList lst = md.title.split("-");
			ui->le_artist->setText( lst.takeFirst().trimmed() );
			ui->le_title->setText(lst.join("-").trimmed());
			guessed = true;
		}

		else if(md.title.contains(":")){
			QStringList lst = md.title.split(":");
			ui->le_artist->setText( lst.takeFirst().trimmed() );
			ui->le_title->setText(lst.join(":").trimmed());
			guessed = true;
		}
	}

	if(guessed == false) {
		if(!md.artist.isEmpty()) {
			ui->le_artist->setText( md.artist );
			ui->le_title->setText( md.title );
		}

		else if(!md.album_artist().isEmpty()) {
			ui->le_artist->setText( md.album_artist() );
			ui->le_title->setText( md.title );
		}

		else {
			ui->le_artist->setText(md.artist);
			ui->le_title->setText(md.title);
		}
	}

	ui->btn_switch->setVisible(guessed);
}

void GUI_Lyrics::switch_pressed()
{
	QString artist = ui->le_artist->text();
	QString title = ui->le_title->text();

	ui->le_artist->setText(title);
	ui->le_title->setText(artist);
}

void GUI_Lyrics::zoom(qreal font_size)
{
	_m->font_size = std::min(30.0, font_size);
	_m->font_size = std::max(5.0, font_size);

	ui->te_lyrics->setStyleSheet("font-size: " + QString::number(_m->font_size) + "pt;");
	_settings->set(Set::Lyrics_Zoom, ui->sb_zoom->value());
}


void GUI_Lyrics::zoom_in()
{
	zoom(_m->font_size + 1.0);
}

void GUI_Lyrics::zoom_out()
{
	zoom(_m->font_size - 1.0);
}


void GUI_Lyrics::showEvent(QShowEvent* e)
{
	init();

	QWidget::showEvent(e);

	prepare_lyrics();
}

void GUI_Lyrics::wheelEvent(QWheelEvent* e)
{
	e->accept();

	sp_log(Log::Debug, this) << (int) e->modifiers();
	if( (e->modifiers() & Qt::ShiftModifier) ||
		(e->modifiers() & Qt::ControlModifier))
	{
		int delta_zoom = 10;
		if(e->delta() < 0){
			delta_zoom = -10;
		}

		ui->sb_zoom->setValue( ui->sb_zoom->value() + delta_zoom);
	}
}

void GUI_Lyrics::keyPressEvent(QKeyEvent* e)
{
	e->ignore();

	int delta_zoom = 0;
	switch(e->key())
	{
		case Qt::Key_Plus:
			delta_zoom = 10;
			break;
		case Qt::Key_Minus:
			delta_zoom = -10;
			break;
		default:
			break;
	}

	ui->sb_zoom->setValue( ui->sb_zoom->value() + delta_zoom);
}
