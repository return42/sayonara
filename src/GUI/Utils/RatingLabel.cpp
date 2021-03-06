/* RatingLabel.cpp */

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

#include "Utils/globals.h"

#include "RatingLabel.h"
#include "GUI/Utils/GuiUtils.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

#include <algorithm>

const int Offset_X = 3;

struct RatingLabel::Private
{
	QWidget*	parent=nullptr;

	QPixmap 	pm_active;
	QPixmap 	pm_inactive;

	int			rating;
	uint8_t     icon_size;
	bool		enabled;

	Private(QWidget* parent, bool enabled) :
		parent(parent),
		rating(0),
		icon_size(14),
		enabled(enabled)
	{
		pm_active = Gui::Util::pixmap("star.png", QSize(icon_size, icon_size), true);
		pm_inactive = Gui::Util::pixmap("star_disabled.png", QSize(icon_size, icon_size), true);
	}
};

RatingLabel::RatingLabel(QWidget* parent, bool enabled) :
	QLabel(parent)
{
	m = Pimpl::make<Private>(parent, enabled);

	QSizePolicy p(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	this->setSizePolicy(p);
	this->setMouseTracking(true);
	this->setStyleSheet("background: transparent;");
}

RatingLabel::~RatingLabel() {}

int RatingLabel::calc_rating(QPoint pos) const
{
	double drating = (double) ((pos.x() * 1.0) / (m->icon_size + 2.0)) + 0.5;
	int rating = (int) (drating);

	rating=std::min(rating, 5);
	rating=std::max(rating, 0);

	return rating;
}

void RatingLabel::paintEvent(QPaintEvent *e)
{
	QLabel::paintEvent(e);

	QPainter painter(this);

	painter.save();
	int offset_y = (this->height() - m->icon_size) / 2;

	painter.translate(rect().x() + Offset_X, rect().y() + offset_y );
	for(int rating = 0; rating < m->rating; rating++)
	{
		painter.drawPixmap(0, 0, m->icon_size, m->icon_size, m->pm_active);
		painter.translate(m->icon_size + 2, 0);
	}

	for(int rating = m->rating; rating < 5; rating++)
	{
		painter.drawPixmap(0, 0, m->icon_size, m->icon_size, m->pm_inactive);
		painter.translate(m->icon_size + 2, 0);
	}

	painter.restore();
}


void RatingLabel::mouseMoveEvent(QMouseEvent *e)
{
	if(!m->enabled) {
		return;
	}

	if(!hasFocus()){
		return;
	}

	int rating = calc_rating(e->pos());
	this->update_rating(rating);
}


void RatingLabel::mousePressEvent(QMouseEvent *e)
{
	if(!m->enabled) {
		return;
	}

	int rating = calc_rating(e->pos());
	update_rating(rating);
}


void RatingLabel::mouseReleaseEvent(QMouseEvent *e)
{
	Q_UNUSED(e);

	if(!m->enabled) {
		return;
	}

	emit sig_finished(true);
}


void RatingLabel::focusInEvent(QFocusEvent* e)
{
	Q_UNUSED(e);
}

void RatingLabel::focusOutEvent(QFocusEvent* e)
{
	Q_UNUSED(e);

	if(!m->enabled) {
		return;
	}

	emit sig_finished(false);
}

void RatingLabel::update_rating(int rating)
{
	m->rating = rating;
	update();
}

void RatingLabel::set_rating(int rating)
{
	m->rating = rating;
	update();
}

int RatingLabel::get_rating() const
{
	return m->rating;
}


