/* EqSlider.h */

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



#ifndef EQSLIDER_H
#define EQSLIDER_H

#include "GUI/Helper/SayonaraWidget/SayonaraSlider.h"
#include <QKeyEvent>

class EqSlider : 
	public SayonaraSlider 
{
	Q_OBJECT

	signals:
		void sig_value_changed(int idx, int val);

	public:
		EqSlider(QWidget* parent);
		virtual ~EqSlider() {}

		void setData(int idx, QLabel* label);
		QLabel* getLabel() const;

		int getIndex() const;
		void setIndex(int idx);

		double get_eq_value() const;
		void set_eq_value(double val);

	


	protected:
		void sliderChange(SliderChange change) override;
		void keyPressEvent(QKeyEvent* e) override;

	private:
		QLabel* _label=nullptr;
		int		_idx;
};


#endif // EQSLIDER_H
