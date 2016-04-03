
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



#include "HeaderView.h"
#include <QFontMetrics>


HeaderView::HeaderView(Qt::Orientation orientation, QWidget* parent) :
	QHeaderView(orientation, parent)
{
	this->setSectionsClickable(true);
	this->setStretchLastSection(true);
	this->setHighlightSections(false);
}

QSize HeaderView::sizeHint() const
{
	QSize size = QHeaderView::sizeHint();
	QFontMetrics fm(this->font());

	int height = std::max(fm.height() + 10, 20);

	size.setHeight(height);

	return size;
}
