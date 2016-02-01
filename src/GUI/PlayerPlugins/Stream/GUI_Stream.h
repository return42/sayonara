/* GUI_Stream.h */

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


#ifndef GUI_STREAM_H_
#define GUI_STREAM_H_


#include "GUI/PlayerPlugins/Stream/ui_GUI_Stream.h"
#include "Components/StreamPlugins/Streams/StreamHandlerStreams.h"

#include "GUI_AbstractStream.h"


class GUI_Stream : public GUI_AbstractStream, public Ui::GUI_Stream {

	Q_OBJECT

protected:
	virtual void language_changed();

public:
	GUI_Stream(QWidget *parent=nullptr);
	virtual ~GUI_Stream();

	QString get_name() const override;
	QString get_display_name() const override;
	QLabel* get_title_label() const override;
	QPushButton* get_close_button() const override;
};

#endif /* GUI_STREAM_H_ */
