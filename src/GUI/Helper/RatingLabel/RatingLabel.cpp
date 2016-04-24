/* RatingLabel.cpp */

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


#include "Helper/globals.h"

#include "RatingLabel.h"
#include "Helper/Random/RandomGenerator.h"

#include <QPainter>
#include <QRect>
#include <QColor>

const int Offset_X = 3;

Rating::Rating() {
    _rating = 0;

}
Rating::Rating(quint8 rating) {
    _rating = rating;

}

quint8 Rating::get_rating() const {

    return _rating;
}

void Rating::set_rating(quint8 rating) {
    _rating = rating;
}


void Rating::paint(QPainter *painter, const QRect &rect,
			   const QPalette &palette, bool has_focus) const
{

	Q_UNUSED(palette)

	painter->save();
	painter->translate(rect.x() + Offset_X, rect.y() + (rect.height() * 25) / 100);

	QRect rating_rect;
    QColor col;
	int wrect = (rect.width() - (2 * Offset_X)) / 5 + 1;
	int hrect = (rect.height() * 50) / 100;

    for(int rating = 0; rating < _rating; rating++) {

		rating_rect.setRect(0, 0, wrect - Offset_X, hrect);

		if(has_focus){
			col = SAYONARA_ORANGE_COL.light(130);
		}
		else{
			col = SAYONARA_ORANGE_COL.dark(110);
		}

		painter->fillRect(rating_rect, col);
		painter->translate(wrect, 0);
    }

    for(int rating= _rating; rating < 5; rating ++) {

		rating_rect.setRect(0, 0, wrect - Offset_X, hrect);
		col = QColor(0, 0, 0, 50);

		painter->fillRect(rating_rect, col);
		painter->translate(wrect, 0);
	}

	painter->restore();
}



RatingLabel::RatingLabel(QWidget *parent, bool enabled) :
	QLabel(parent),
	_rating(0)
{
	RandomGenerator rnd;

	_enabled = enabled;
	_parent = parent;
	_id = rnd.get_number(0, 1000000);

    QSizePolicy p(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    this->setSizePolicy(p);
	this->setMouseTracking(true);
}

RatingLabel::~RatingLabel() {
    _id = 0;
}

int RatingLabel::get_id() const{

    return _id;
}


int RatingLabel::calc_rating(QPoint pos) const{

	double drating = (double) ((pos.x() * 6.0) / width()) + 0.25;
	int rating = (int) (drating);

    if(rating > 5) rating = 5;

    return rating;
}


void RatingLabel::paintEvent(QPaintEvent *e) {
	QLabel::paintEvent(e);
    QPainter painter(this);

	_rating.paint(&painter, rect(), palette(), this->hasFocus());
}


void RatingLabel::mouseMoveEvent(QMouseEvent *e) {

	if(!_enabled) {
		return;
	}

	if(!hasFocus()){
		return;
	}

	int rating = calc_rating(e->pos());
	this->update_rating(rating);
}


void RatingLabel::mousePressEvent(QMouseEvent *e) {

	if(!_enabled) {
		return;
	}

    int rating = calc_rating(e->pos());
	update_rating(rating);
}


void RatingLabel::mouseReleaseEvent(QMouseEvent *e) {
	Q_UNUSED(e);

	if(!_enabled) {
		return;
	}

    emit sig_finished(true);
}


void RatingLabel::focusInEvent(QFocusEvent* e) {
	Q_UNUSED(e);

}

void RatingLabel::focusOutEvent(QFocusEvent* e) {
	Q_UNUSED(e);

	if(!_enabled) {
		return;
	}

	emit sig_finished(false);
}

void RatingLabel::update_rating(int rating) {
    _rating.set_rating(rating);
    update();
}

void RatingLabel::increase() {
    int rating = _rating.get_rating();

	rating = std::min(5, rating + 1);

    update_rating(rating);
}

void RatingLabel::decrease() {
    int rating = _rating.get_rating();

	rating = std::max(rating - 1, 0);

    update_rating(rating);
}

void RatingLabel::set_rating(Rating rating) {

    _rating = rating;
    update();
}

Rating RatingLabel::get_rating() const{

    return _rating;
}

void RatingLabel::kill_yourself() {
    emit sig_finished(false);
}

