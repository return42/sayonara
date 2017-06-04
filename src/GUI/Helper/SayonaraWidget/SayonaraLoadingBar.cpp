/* SayonaraLoadingBar.cpp */

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


/* SayonaraLoadingBar.cpp */

#include "SayonaraLoadingBar.h"

SayonaraLoadingBar::SayonaraLoadingBar(QWidget* parent) :
	QProgressBar(parent)
{
	_parent = parent;

	_position = SayonaraLoadingBar::Position::Bottom;
	_fixed_height = 5;

	this->setEnabled(false);
	this->setObjectName("loading_bar");

	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	this->setMaximumHeight(_fixed_height);
	this->setMinimum(0);
	this->setMaximum(0);
}

SayonaraLoadingBar::~SayonaraLoadingBar() {}

void SayonaraLoadingBar::set_position(SayonaraLoadingBar::Position o)
{
	_position = o;
}

void SayonaraLoadingBar::showEvent(QShowEvent* e)
{
	QProgressBar::showEvent(e);

	int y;
	switch(_position)
	{
		case SayonaraLoadingBar::Position::Top:
			 y = 2;
			break;
		case SayonaraLoadingBar::Position::Middle:
			y = (_parent->height() - _fixed_height) / 2;
			break;
		case SayonaraLoadingBar::Position::Bottom:
		default:
			 y = _parent->height() - _fixed_height - 2;
			break;
	}

	this->setGeometry(2,
					  y,
					  _parent->width() - 4,
					  _fixed_height);
}
