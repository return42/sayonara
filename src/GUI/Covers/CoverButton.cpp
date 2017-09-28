/* CoverButton.cpp */

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

#include "CoverButton.h"
#include "GUI_AlternativeCovers.h"
#include "Components/Covers/CoverLookup.h"
#include "Components/Covers/CoverLocation.h"
#include "Helper/FileHelper.h"
#include "Helper/Helper.h"

struct CoverButton::Private
{
	GUI_AlternativeCovers* 	alternative_covers=nullptr;
	CoverLookup*			cover_lookup=nullptr;
	CoverLocation 			search_cover_location;
	QString					text;
    QString					current_cover_path;
    QStringList             tmp_paths;
    bool                    cover_forced;

    Private() :
        cover_forced(false)
    {}
};


CoverButton::CoverButton(QWidget* parent) : 
	QPushButton(parent)
{
	m = Pimpl::make<CoverButton::Private>();

    m->current_cover_path = CoverLocation::getInvalidLocation().preferred_path();
	m->search_cover_location = CoverLocation::getInvalidLocation();

	this->setIconSize(this->size());
    this->setIcon(get_cur_icon());

	connect(this, &QPushButton::clicked, this, &CoverButton::cover_button_clicked);
}

CoverButton::~CoverButton()
{
    Helper::File::delete_files(m->tmp_paths);
}


void CoverButton::set_cover_image(const QString& cover_path)
{
    m->current_cover_path = cover_path;
    m->cover_forced = false;

    this->setIcon(get_cur_icon());
    this->setToolTip("");
}


void CoverButton::set_cover_location(const CoverLocation& cl)
{
	m->search_cover_location = cl;

    if(!m->cover_lookup)
    {
		m->cover_lookup = new CoverLookup(this);
		connect(m->cover_lookup, &CoverLookup::sig_cover_found, this, &CoverButton::set_cover_image);
	}

    m->cover_lookup->fetch_cover(cl);
}

void CoverButton::force_cover(const QPixmap &pm)
{
    QString tmp_path = Helper::sayonara_path("covers") + "/tmp_" + Helper::random_string(16) + ".png";

    m->current_cover_path = Helper::File::clean_filename(tmp_path);
    m->tmp_paths << m->current_cover_path;
    m->cover_forced = true;

    pm.save(m->current_cover_path);

    this->setIcon(get_cur_icon());
}

void CoverButton::force_cover(const QImage &img)
{
    force_cover(QPixmap::fromImage(img));
}

QIcon CoverButton::get_cur_icon() const
{
    QPixmap pm = QPixmap(m->current_cover_path)
            .scaled(this->iconSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return QIcon(pm);
}


void CoverButton::cover_button_clicked()
{
    if(m->cover_forced){
        return;
    }

    if(!m->alternative_covers)
    {
        m->alternative_covers = new GUI_AlternativeCovers(this);

        connect(m->alternative_covers, &GUI_AlternativeCovers::sig_cover_changed,
                this, &CoverButton::alternative_cover_fetched );
    }

    m->alternative_covers->start(m->search_cover_location);
}


void CoverButton::alternative_cover_fetched(const CoverLocation& cl)
{
	if(cl.valid()){
		emit sig_cover_replaced();
	}

	set_cover_image(cl.cover_path());
}


void CoverButton::cover_found(const CoverLocation& cl)
{
    /* If cover was forced while CoverLookup was still running */
    if(m->cover_forced && (sender() == m->cover_lookup)) {
        return;
    }

	if(cl.valid()){
		emit sig_cover_found();
	}

	set_cover_image(cl.cover_path());
}



void CoverButton::resizeEvent(QResizeEvent* e)
{
    QPushButton::resizeEvent(e);

    QSize sz = this->size();
    sz.setHeight(sz.height() - 10);
    sz.setWidth(sz.width() - 10);

    this->setIconSize(sz);
    this->setIcon(get_cur_icon());
}
