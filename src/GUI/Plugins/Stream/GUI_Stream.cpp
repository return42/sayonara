/* GUI_Stream.cpp */

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

#include "GUI_Stream.h"
#include "GUI/Plugins/Stream/ui_GUI_Stream.h"
#include "Components/StreamPlugins/Streams/StreamHandlerStreams.h"
#include "Helper/Language.h"

GUI_Stream::GUI_Stream(QWidget *parent) :
	GUI_AbstractStream(new StreamHandlerStreams(), parent) {}

GUI_Stream::~GUI_Stream()
{
	if(ui)
	{
		delete ui; ui=nullptr;
	}
}

QString GUI_Stream::get_name() const
{
	return "Webstreams";
}

QString GUI_Stream::get_display_name() const
{
	return tr("Webstreams");
}

void GUI_Stream::language_changed()
{
	if(ui){
		ui->retranslateUi(this);
	}
}

void GUI_Stream::init_ui()
{
	setup_parent(this, &ui);
}


QString GUI_Stream::get_title_fallback_name() const
{
	return Lang::get(Lang::Radio);
}
