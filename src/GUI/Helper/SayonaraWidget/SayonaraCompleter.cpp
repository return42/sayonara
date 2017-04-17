/* SayonaraCompleter.cpp */

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



#include "SayonaraCompleter.h"
#include "GUI/Helper/Style/Style.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"

#include <QStringList>
#include <QAbstractItemView>

SayonaraCompleter::SayonaraCompleter(const QStringList& lst, QObject* parent) :
	QCompleter(lst, parent)
{
	setCaseSensitivity(Qt::CaseInsensitive);
	setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	popup()->setItemDelegate(new ComboBoxDelegate(this));
	popup()->setStyleSheet(Style::get_current_style());
}

SayonaraCompleter::~SayonaraCompleter() {}

void SayonaraCompleter::set_stringlist(const QStringList& lst)
{
	QAbstractItemModel* model = this->model();
	if(!model){
		return;
	}

	model->removeRows(0, this->model()->rowCount());
	model->insertRows(0, lst.size());

	int idx=0;
	for(const QString& str : lst){
		model->setData(model->index(idx++, 0), str);
	}
}

