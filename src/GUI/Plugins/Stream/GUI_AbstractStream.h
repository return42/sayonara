/* GUI_AbstractStream.h */

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

#ifndef GUI_ABSTRACT_STREAM_H_
#define GUI_ABSTRACT_STREAM_H_

#include "Interfaces/PlayerPlugin/PlayerPlugin.h"
#include "Helper/Pimpl.h"

class QComboBox;
class QPushButton;
class QLineEdit;
class QLabel;
class MenuToolButton;
class AbstractStreamHandler;

class GUI_AbstractStream :
		public PlayerPluginInterface
{
	Q_OBJECT

public:
	explicit GUI_AbstractStream(AbstractStreamHandler* stream_handler, QWidget* parent=nullptr);
	virtual ~GUI_AbstractStream();

protected:
	virtual void		language_changed() override;
	virtual void		play(QString url, QString station_name);

	virtual QString		get_title_fallback_name() const=0;
	void				setup_stations(const QMap<QString, QString>&);
	bool				has_loading_bar() const override;

	template<typename T, typename UiType>
	void setup_parent(T* subclass, UiType** uiptr)
	{
		PlayerPluginInterface::setup_parent(subclass, uiptr);

		UiType* ui = *uiptr;
		set_le_url( ui->le_url );
		set_combo_stream( ui->combo_stream );
		set_btn_play( ui->btn_play );
		set_btn_tool( ui->btn_tool );
		set_lab_listen( ui->lab_listen );

		GUI_AbstractStream::init_ui();
	}

protected slots:
	void listen_clicked();
	void combo_idx_changed(int idx);
	void delete_clicked();
	void save_clicked();
	void new_clicked();
	void text_changed(const QString& str);
	void too_many_urls_found(int n_urls, int n_max_urls);

	void stopped();
	void error();
	void data_available();
	void _sl_skin_changed();

private:
	PIMPL(GUI_AbstractStream)

	void init_connections();
	void init_streams();

	void set_le_url(QLineEdit* le_url);
	void set_combo_stream(QComboBox* le_combo_stream);
	void set_btn_play(QPushButton* btn_play);
	void set_btn_tool(MenuToolButton* btn_tool);
	void set_lab_listen(QLabel* lab_listen);

	void set_searching(bool searching);

	virtual void init_ui() override;
};

#endif // GUI_ABSTRACT_STREAM_H_
