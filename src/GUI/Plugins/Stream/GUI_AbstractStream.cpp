/* GUI_AbstractStream.cpp */

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

#include "GUI_AbstractStream.h"

#include "GUI/Helper/Delegates/ComboBoxDelegate.h"
#include "GUI/Helper/IconLoader/IconLoader.h"
#include "GUI/Helper/MenuTool/MenuTool.h"
#include "GUI/Helper/Style/Style.h"

#include "Helper/Parser/PlaylistParser.h"
#include "Helper/Parser/PodcastParser.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Language.h"
#include "Helper/Message/Message.h"
#include "Helper/Settings/Settings.h"

#include "Components/StreamPlugins/Streams/AbstractStreamHandler.h"

#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QAbstractItemView>

struct GUI_AbstractStream::Private
{
	QComboBox*				combo_stream=nullptr;
	QPushButton*			btn_play=nullptr;
	QLineEdit*				le_url=nullptr;
	QLabel*					lab_listen=nullptr;
	MenuToolButton*			btn_tool=nullptr;
	AbstractStreamHandler*	stream_handler=nullptr;
	QMap<QString, QString>	stations;
};

GUI_AbstractStream::GUI_AbstractStream(AbstractStreamHandler* stream_handler, QWidget* parent) :
	PlayerPluginInterface(parent)
{
	_m = Pimpl::make<Private>();
	_m->stream_handler = stream_handler;
}


GUI_AbstractStream::~GUI_AbstractStream() {}

void GUI_AbstractStream::init_connections()
{
	_m->combo_stream->setAutoCompletion(false);

	_m->combo_stream->setFocusPolicy(Qt::StrongFocus);
	_m->le_url->setFocusPolicy(Qt::StrongFocus);
	_m->btn_play->setFocusPolicy(Qt::StrongFocus);

	setTabOrder(_m->combo_stream, _m->le_url);
	setTabOrder(_m->le_url, _m->btn_play);

	_m->btn_tool->show_action(ContextMenu::EntryNew, true);

	connect(_m->btn_play, &QPushButton::clicked, this, &GUI_AbstractStream::listen_clicked);
	connect(_m->btn_tool, &MenuToolButton::sig_save, this, &GUI_AbstractStream::save_clicked);
	connect(_m->btn_tool, &MenuToolButton::sig_delete, this, &GUI_AbstractStream::delete_clicked);
	connect(_m->btn_tool, &MenuToolButton::sig_new, this, &GUI_AbstractStream::new_clicked);

	connect(_m->combo_stream, combo_activated_int, this, &GUI_AbstractStream::combo_idx_changed);
	connect(_m->combo_stream, &QComboBox::editTextChanged, this, &GUI_AbstractStream::text_changed);

	connect(_m->le_url, &QLineEdit::textChanged, this, &GUI_AbstractStream::text_changed);

	connect(_m->stream_handler, &AbstractStreamHandler::sig_error, this, &GUI_AbstractStream::error);
	connect(_m->stream_handler, &AbstractStreamHandler::sig_data_available, this, &GUI_AbstractStream::data_available);
}

void GUI_AbstractStream::init_streams()
{
	StreamMap data;

	if( _m->stream_handler->get_all_streams(data) ){
		setup_stations(data);
	}
}


void GUI_AbstractStream::init_ui()
{
	_m->btn_play->setMinimumSize(QSize(24,24));
	_m->btn_play->setMaximumSize(QSize(24,24));
	_m->btn_tool->setToolTip(Lang::get(Lang::Menu));
	_m->btn_tool->setText(Lang::get(Lang::Menu));

	_m->le_url->setPlaceholderText("Enter URL...");
	_m->combo_stream->lineEdit()->setPlaceholderText("Enter name...");
	_m->combo_stream->setItemDelegate(new ComboBoxDelegate(this));

	init_connections();
	init_streams();
	skin_changed();

	REGISTER_LISTENER(Set::Player_Style, _sl_skin_changed);
}


void GUI_AbstractStream::language_changed() {}


void GUI_AbstractStream::error()
{
	_m->btn_play->setDisabled(false);
	_m->lab_listen->setText(Lang::get(Lang::Listen));

	sp_log(Log::Warning, this) << "Stream Handler error";
	GlobalMessage::Answer answer =
			Message::question_yn(
				tr("Cannot open stream") + "\n" +
				_m->le_url->text() + "\n\n" +
				Lang::get(Lang::Retry).question());

	if(answer == GlobalMessage::Answer::Yes){
		listen_clicked();
	}
}

void GUI_AbstractStream::data_available()
{
	_m->btn_play->setDisabled(false);
	_m->lab_listen->setText(Lang::get(Lang::Listen));
}

void GUI_AbstractStream::_sl_skin_changed()
{
	if(!is_ui_initialized()){
		return;
	}

	_m->btn_play->setIcon( IconLoader::getInstance()->get_icon("media-playback-start", "play"));

	QAbstractItemView* view = _m->combo_stream->view();

	view->parentWidget()->setStyleSheet("margin: 0px; padding: -4px -1px; border: 1px solid #282828; background: none;");
	view->setStyleSheet(Style::get_current_style());
	view->setMinimumHeight(20 * view->model()->rowCount());
}


void GUI_AbstractStream::play(QString url, QString station_name)
{
	bool success = _m->stream_handler->parse_station(url, station_name);
	if(!success){
		sp_log(Log::Warning, this) << "Stream Handler busy";
		_m->btn_play->setEnabled(true);
		_m->lab_listen->setEnabled(true);
		_m->lab_listen->setText(Lang::get(Lang::Play));
	}
}


void GUI_AbstractStream::listen_clicked()
{
	QString name, url;

	if( _m->combo_stream->currentIndex() <= 0) {
		url = _m->le_url->text();
		name = get_title_fallback_name();
	}

	else{
		url = _m->le_url->text();
		name = _m->combo_stream->currentText();
	}

	url = url.trimmed();
	if(url.size() > 5) {
		_m->btn_play->setDisabled(true);
		_m->lab_listen->setText(tr("Busy..."));

		play(url, name);
	}
}

void GUI_AbstractStream::combo_idx_changed(int idx)
{
	QString cur_station_name = _m->combo_stream->currentText();
	QString address = _m->stations[cur_station_name];
	bool listen_enabled;

	if(address.size() > 0) {
		_m->le_url->setText(address);
	}

	if(idx == 0) {
		_m->le_url->setText("");
	}

	listen_enabled = (_m->le_url->text().size() > 5);

	_m->btn_tool->show_action(ContextMenu::EntryDelete, idx > 0);
	_m->btn_play->setEnabled(listen_enabled);
	_m->lab_listen->setEnabled(listen_enabled);
	_m->combo_stream->setToolTip(address);
}

void GUI_AbstractStream::new_clicked()
{
	if(_m->combo_stream->count() > 0){
		_m->combo_stream->setCurrentIndex(0);
		_m->combo_stream->setItemText(0, "");
	}

	else{
		_m->combo_stream->addItem("");
		_m->combo_stream->setCurrentIndex(0);
	}

	_m->le_url->setText("");
}

void GUI_AbstractStream::text_changed(const QString& str)
{
	Q_UNUSED(str)

	bool listen_enabled = (!_m->le_url->text().isEmpty());

	_m->btn_play->setEnabled(listen_enabled);
	_m->lab_listen->setEnabled(listen_enabled);

	_m->btn_tool->show_action(ContextMenu::EntrySave,  listen_enabled &&
						  !_m->combo_stream->currentText().isEmpty());
}

void GUI_AbstractStream::delete_clicked()
{
	if(_m->combo_stream->currentIndex() <= 0) return;

	QString cur_station_name = _m->combo_stream->currentText();

	GlobalMessage::Answer ret = Message::question_yn(tr("Do you really want to delete %1").arg(cur_station_name));

	if(ret == GlobalMessage::Answer::Yes) {
		if( _m->stream_handler->delete_stream(cur_station_name) ) {
			StreamMap map;
			sp_log(Log::Info, this) << cur_station_name << "successfully deleted";

			if( _m->stream_handler->get_all_streams(map) ) {
				setup_stations(map);
			}
		}
	}
}

void GUI_AbstractStream::save_clicked()
{
	QString name = _m->combo_stream->currentText();
	QString url = _m->le_url->text();
	GlobalMessage::Answer answer;

	if(name.isEmpty() || url.isEmpty()){
		return;
	}

	StreamMap map;


	for(int i=0; i<_m->combo_stream->count(); i++)
	{
		QString text = _m->combo_stream->itemText(i);
		if(text == name){
			answer = Message::question_yn(tr("Overwrite?") + "\n" + name + "\n" + url);
			if(answer == GlobalMessage::Answer::Yes){
				_m->stream_handler->update_url(name, url);
				_m->stream_handler->get_all_streams(map);
				setup_stations(map);
				return;
			}

			else{
				return;
			}
		}
	}

	_m->stream_handler->save(name, url);
	_m->stream_handler->get_all_streams(map);
	setup_stations(map);
}

void GUI_AbstractStream::setup_stations(const StreamMap& stations)
{
	QString old_name = _m->combo_stream->currentText();
	QString old_url = _m->le_url->text();

	_m->combo_stream->clear();
	_m->le_url->clear();

	_m->stations = stations;
	_m->stations[""] = "";

	if(_m->stations.size() == 1){
		_m->combo_stream->setCurrentIndex(0);
	}

	for(auto it = _m->stations.begin(); it != _m->stations.end(); it++) {
		_m->combo_stream->addItem(it.key(), it.value());
	}

	_m->combo_stream->view()->setItemDelegate(new ComboBoxDelegate(this));
	_m->btn_play->setEnabled(false);
	_m->lab_listen->setEnabled(false);
	_m->btn_tool->show_action(ContextMenu::EntrySave, false);
	_m->btn_tool->show_action(ContextMenu::EntryDelete, false);
	_m->combo_stream->setCurrentText(old_name);
	_m->le_url->setText(old_url);
}


void GUI_AbstractStream::set_le_url(QLineEdit* le_url)
{
	_m->le_url = le_url;
}

void GUI_AbstractStream::set_combo_stream(QComboBox* combo_stream)
{
	_m->combo_stream = combo_stream;
}

void GUI_AbstractStream::set_btn_play(QPushButton* btn_play)
{
	_m->btn_play = btn_play;
}

void GUI_AbstractStream::set_btn_tool(MenuToolButton* btn_tool)
{
	_m->btn_tool = btn_tool;
}

void GUI_AbstractStream::set_lab_listen(QLabel* lab_listen)
{
	_m->lab_listen = lab_listen;
}
