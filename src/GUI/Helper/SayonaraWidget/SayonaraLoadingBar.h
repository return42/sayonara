/* SayonaraLoadingBar.h */

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


/* SayonaraLoadingBar.h */

#ifndef SAYONARALOADINGBAR_H
#define SAYONARALOADINGBAR_H

#include <QProgressBar>

/**
 * @brief The SayonaraLoadingBar class
 * @ingroup Widgets
 */
class SayonaraLoadingBar :
		public QProgressBar
{

public:
	enum class Orientation
	{
		Top=0,
		Middle,
		Bottom
	};

private:
	QWidget*	_parent=nullptr;
	int			_fixed_height;
	Orientation _orientation;

public:
	explicit SayonaraLoadingBar(QWidget* parent);
	~SayonaraLoadingBar();

	void set_orientation(SayonaraLoadingBar::Orientation o);

protected:
	void showEvent(QShowEvent* e) override;
};



#endif // SAYONARALOADINGBAR_H
