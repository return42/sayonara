/* GUI_AbstractStream.h */

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


#ifndef GUI_AbstractStream_H
#define GUI_AbstractStream_H

#include "GUI/Helper/MenuTool/MenuTool.h"
#include "Interfaces/PlayerPlugin/PlayerPlugin.h"
#include "Components/StreamPlugins/Streams/AbstractStreamHandler.h"
#include "Helper/Message/GlobalMessage.h"

#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

class AbstractStreamHandler;
class DatabaseConnector;

class GUI_AbstractStream : public PlayerPluginInterface
{

	Q_OBJECT

private:
	QComboBox*				_combo_stream=nullptr;
	QPushButton*			_btn_play=nullptr;
	QLineEdit*				_le_url=nullptr;
	QLabel*					_lab_listen=nullptr;
	MenuToolButton*			_btn_tool=nullptr;

	virtual void			init_connections();
	virtual void			init_streams();


signals:
	void sig_close_event();


public:
	explicit GUI_AbstractStream(AbstractStreamHandler* stream_handler, QWidget* parent=nullptr);
	virtual ~GUI_AbstractStream();


protected:
	DatabaseConnector*		_db=nullptr;	// used to fetch entries from DB
	AbstractStreamHandler*	_stream_handler=nullptr;

	// url, name
	QMap<QString, QString>	_stations;


	virtual void						language_changed() override;
	virtual void						init_ui() override;

	virtual void						play(QString url, QString station_name);
	virtual GlobalMessage::Answer		show_delete_confirm_dialog();

	virtual QString						get_title_fallback_name() const=0;
	virtual void						setup_stations(const QMap<QString, QString>&);

	template<typename T, typename UiType>
	void setup_parent(T* subclass, UiType** ui)
	{
		PlayerPluginInterface::setup_parent(subclass, ui);

		_le_url =			(*ui)->le_url;
		_combo_stream =		(*ui)->combo_stream;
		_btn_play =			(*ui)->btn_play;
		_btn_tool =			(*ui)->btn_tool;
		_lab_listen =		(*ui)->lab_listen;

		init_connections();
		init_streams();
	}


protected slots:
	virtual void listen_clicked();
	virtual void combo_idx_changed(int idx);
	virtual void delete_clicked();
	virtual void save_clicked();
	virtual void new_clicked();
	virtual void text_changed(const QString& str);

	void error();
	void data_available();
	void _sl_skin_changed();
};

#endif // GUI_AbstractStream_H
