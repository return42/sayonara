/* RatingLabel.h */

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



#ifndef RATINGLABEL_H
#define RATINGLABEL_H

#include <QLabel>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QPalette>
#include <QPainter>
#include <QRect>
#include <QPoint>

class Rating {

public:
	Rating();
	Rating(quint8 rating);

	void paint(QPainter *painter, const QRect &rect,
			   const QPalette &palette, bool has_focus) const;

	void set_rating(quint8 rating);
	quint8 get_rating() const;

private:
	quint8 _rating;
};


Q_DECLARE_METATYPE(Rating)

class RatingLabel : public QLabel
{
	Q_OBJECT

signals:
	void sig_finished(bool);


public:
	RatingLabel(QWidget *parent, bool enabled=true);
	virtual ~RatingLabel();

	virtual void paintEvent(QPaintEvent *e) override;
	virtual void focusInEvent(QFocusEvent* e) override;
	virtual void focusOutEvent(QFocusEvent* e) override;
	virtual void mousePressEvent(QMouseEvent *ev) override;
	virtual void mouseReleaseEvent(QMouseEvent* ev) override;
	virtual void mouseMoveEvent(QMouseEvent *ev) override;

	void set_rating(Rating rating);
	Rating get_rating() const;

	int get_id() const;

	void kill_yourself();

	void increase();
	void decrease();



private:

	QWidget*	_parent=nullptr;
	bool		_enabled;
	int			_id;
	Rating		_rating;

	void update_rating(int rating);
	int calc_rating(QPoint pos) const;

};



#endif // RATINGLABEL_H
