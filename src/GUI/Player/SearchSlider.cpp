/* SearchSlider.cpp

 * Copyright (C) 2012  
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
#include "SearchSlider.h"
#include "Helper/Logger/Logger.h"
#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QAbstractSlider>


#include <cmath>


SearchSlider::SearchSlider(QWidget* parent) :
	QSlider(parent)
{
	_searching = false;
}

SearchSlider::~SearchSlider() {

}


bool SearchSlider::event(QEvent *e){

	QWheelEvent* we;
	QKeyEvent* ke;
	int delta_val = 5;

	switch(e->type()){

		case QEvent::Wheel:

			if(!this->isEnabled()) break;

			we = (QWheelEvent*) e;
			if(we->modifiers() & Qt::ShiftModifier){
				delta_val = 10;
			}

			else if(we->modifiers() & Qt::AltModifier){
				delta_val = 50;
			}


			if(we->delta() > 0){
				setValue(value() + delta_val);
			}
			else{
				setValue(value() - delta_val);
			}

			emit_new_val(value());
			break;

		default:
			break;
	}

	return QSlider::event(e);
}

void SearchSlider::mousePressEvent(QMouseEvent* e){
	QSlider::mousePressEvent(e);

	int percent = (e->pos().x() * 100) / geometry().width();
	int new_val =  (this->maximum() * percent) / 100;
	setValue(new_val);

	_searching = true;
}

void SearchSlider::mouseReleaseEvent(QMouseEvent* e){

	QSlider::mouseReleaseEvent(e);

	_searching = false;

	int percent = (e->pos().x() * 100) / geometry().width();
	int new_val =  (this->maximum() * percent) / 100;

	emit_new_val( new_val );
}

void SearchSlider::mouseMoveEvent(QMouseEvent *e){

	QSlider::mouseMoveEvent(e);

	if(_searching){
		int percent = (e->pos().x() * 100) / geometry().width();
		int new_val =  (this->maximum() * percent) / 100;
		QSlider::setValue(new_val);
	}

	emit_new_val(this->value());
}


void SearchSlider::setValue(int i){

	if(_searching) return;

	QSlider::setValue(i);
}

void SearchSlider::increment(int i){

	setValue( value() + i );

	emit_new_val(this->value());
}

void SearchSlider::decrement(int i){

	setValue( value() - i );

	emit_new_val(this->value());
}


void SearchSlider::increment_10()
{
	increment(10);
}

void SearchSlider::decrement_10()
{
	decrement(10);
}

void SearchSlider::increment_50()
{
	increment(50);
}

void SearchSlider::decrement_50()
{
	decrement(50);
}






void SearchSlider::emit_new_val(int value)
{
	if(value < 0) value = 0;
	if(value > maximum()) value = maximum();

	emit sig_slider_moved(value);
}
