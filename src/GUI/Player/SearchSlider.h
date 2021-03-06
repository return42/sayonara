/* SearchSlider.h

 * Copyright (C) 2011-2017 Lucio Carreras
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras,
 * Sep 14, 2012
 *
 */

#ifndef SEARCHSLIDER_H_
#define SEARCHSLIDER_H_

#include "GUI/Utils/Widgets/Slider.h"

class SearchSlider:
		public Gui::Slider
{
	Q_OBJECT

signals:
	void sig_slider_moved(int);

public:
	explicit SearchSlider(QWidget* parent=nullptr);
	virtual ~SearchSlider();

	bool is_busy() const;

protected:
	void mousePressEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void mouseMoveEvent(QMouseEvent* e) override;
	bool event(QEvent *event) override;

private:
	void emit_new_val(int value);
};

#endif /* SEARCHSLIDER_H_ */
