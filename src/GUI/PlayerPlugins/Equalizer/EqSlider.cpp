/* EqSlider.cpp */

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



#include "EqSlider.h"

EqSlider::EqSlider(QWidget *parent) :
	QSlider(parent)
{
	_idx = -1;
}

void  EqSlider::setData(int idx, QLabel* label){
	_label = label;
	_idx = idx;
}

QLabel* EqSlider::getLabel() const
{
	return _label;
}

bool EqSlider::event(QEvent *e){

	switch(e->type()){

		case QEvent::HoverEnter:
			emit sig_slider_got_focus(_idx);
			break;


		case QEvent::HoverLeave:

			if(!this->hasFocus()){
				emit sig_slider_lost_focus(_idx);
			}

			break;

		default: break;
	}

	return QSlider::event(e);

}

void EqSlider::focusInEvent(QFocusEvent* e){
	QSlider::focusInEvent(e);
	emit sig_slider_got_focus(_idx);
}

void EqSlider::focusOutEvent(QFocusEvent* e){
	QSlider::focusOutEvent(e);

	emit sig_slider_lost_focus(_idx);
}

int EqSlider::getIndex() const
{
	return _idx;
}

void EqSlider::setIndex(int idx){
	_idx = idx;
}

void EqSlider::sliderChange(SliderChange change){
	QSlider::sliderChange(change);

	if(change == QAbstractSlider::SliderValueChange){

		emit sig_value_changed(_idx, this->value());
	}
}
