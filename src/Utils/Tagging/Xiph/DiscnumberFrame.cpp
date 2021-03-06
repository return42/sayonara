/* DiscnumberFrame.cpp */

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



#include "DiscnumberFrame.h"
#include <QStringList>

Xiph::DiscnumberFrame::DiscnumberFrame(TagLib::Tag* tag) :
    Xiph::XiphFrame<Models::Discnumber>(tag, "DISCNUMBER")
{

}

Xiph::DiscnumberFrame::~DiscnumberFrame() {}

bool Xiph::DiscnumberFrame::map_tag_to_model(Models::Discnumber& model)
{
	TagLib::String str;
	bool success = value(str);
	if(!success){
		return false;
	}

	QString sval = cvt_string(str);
    QStringList lst = sval.split("/");
    if(lst.size() > 0){
		model.disc = lst[0].toInt();
    }

    if(lst.size() > 1){
		model.n_discs= lst[1].toInt();
    }

    return (lst.size() > 0);
}

bool Xiph::DiscnumberFrame::map_model_to_tag(const Models::Discnumber& model)
{
    QString str;
    str += QString::number(model.disc);
    str += "/";
    str += QString::number(model.n_discs);

	this->set_value(str);

    return true;
}

