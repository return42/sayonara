/* RatingLabel.h */

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

#ifndef RATINGLABEL_H
#define RATINGLABEL_H

#include <QLabel>
#include <QPoint>

#include "Utils/Pimpl.h"

/**
 * @brief The RatingLabel class
 * @ingroup GUIHelper
 */
class RatingLabel : public QLabel
{
	Q_OBJECT
	PIMPL(RatingLabel)

signals:
	void sig_finished(bool);


public:
	RatingLabel(QWidget *parent, bool enabled=true);
	virtual ~RatingLabel();

	void set_rating(int rating);
	int get_rating() const;


protected:
	void paintEvent(QPaintEvent *e) override;
	void focusInEvent(QFocusEvent* e) override;
	void focusOutEvent(QFocusEvent* e) override;
	void mousePressEvent(QMouseEvent *ev) override;
	void mouseReleaseEvent(QMouseEvent* ev) override;
	void mouseMoveEvent(QMouseEvent *ev) override;

private:
	void update_rating(int rating);
	int calc_rating(QPoint pos) const;
};

#endif // RATINGLABEL_H
