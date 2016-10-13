/* CoverButton.cpp */

/* Copyright (C) 2011-2015  Lucio Carreras
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


#include "CoverButton.h"
#include "GUI_AlternativeCovers.h"
#include "Components/Covers/CoverLookup.h"
#include "Components/Covers/CoverLocation.h"

struct CoverButton::Private
{
	GUI_AlternativeCovers* 	alternative_covers=nullptr;
	CoverLookup*			cover_lookup=nullptr;
	CoverLocation 			search_cover_location;
	QString					text;
	bool 					cover_forced;
};


CoverButton::CoverButton(QWidget* parent) : 
	QPushButton(parent)
{
	_m = Pimpl::make<CoverButton::Private>();

	_m->cover_forced = false;
	_m->search_cover_location = CoverLocation::getInvalidLocation();

	connect(this, &QPushButton::clicked, this, &CoverButton::cover_button_clicked);
}

CoverButton::~CoverButton() {}

void CoverButton::cover_button_clicked(){

	if(!_m->alternative_covers){
		_m->alternative_covers = new GUI_AlternativeCovers(this);

		connect(_m->alternative_covers, &GUI_AlternativeCovers::sig_cover_changed,
				this, &CoverButton::alternative_cover_fetched );
	}

	_m->alternative_covers->start(_m->search_cover_location);
}


void CoverButton::set_cover_location(const CoverLocation& cl){

	_m->search_cover_location = cl;

	if(!_m->cover_lookup){
		_m->cover_lookup = new CoverLookup(this);
		connect(_m->cover_lookup, &CoverLookup::sig_cover_found, this, &CoverButton::set_cover_image);
	}

	_m->cover_forced = false;
	_m->cover_lookup->fetch_cover(cl);
}

void CoverButton::force_icon(const QIcon& icon){

	_m->cover_forced = true;

	this->setIcon(icon);
	this->setToolTip("MP3 Tag");
}


void CoverButton::alternative_cover_fetched(const CoverLocation& cl){

	if(cl.valid()){
		emit sig_cover_replaced();
	}

	set_cover_image(cl.cover_path());
}


void CoverButton::cover_found(const CoverLocation &cl){

	if(cl.valid()){
		emit sig_cover_found();
	}

	set_cover_image(cl.cover_path());
}


void CoverButton::set_cover_image(const QString& cover_path){

	if( _m->cover_forced && sender() == _m->cover_lookup){
		return;
	}

	QIcon icon(cover_path);
	this->setIcon(icon);
	this->setToolTip("");
}


