/* CoverButton.h */

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

#ifndef COVER_BUTTON_H
#define COVER_BUTTON_H

#include <QObject>
#include <QPushButton>
#include <QIcon>
#include "Helper/Pimpl.h"

class GUI_AlternativeCovers;
class CoverLookup;
class CoverLocation;


/**
 * @brief The CoverButton class
 * @ingroup GUICovers
 */
class CoverButton : public QPushButton
{
	Q_OBJECT

signals:
	void sig_cover_replaced();
	void sig_cover_found();

public:
	explicit CoverButton(QWidget* parent=nullptr);
	virtual ~CoverButton();

	void set_cover_location(const CoverLocation& cl);
	void force_icon(const QIcon& icon);

private:
	PIMPL(CoverButton)


private slots:
	void cover_button_clicked();
	void alternative_cover_fetched(const CoverLocation& cl);
	void cover_found(const CoverLocation& cl);
	void set_cover_image(const QString& cover_path);
};

#endif
