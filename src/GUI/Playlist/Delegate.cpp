/* PlaylistItemDelegate.cpp */

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

#include <QPainter>
#include <QFontMetrics>
#include <QTableView>

#include "Utils/Utils.h"
#include "Utils/MetaData/MetaData.h"
#include "Utils/Settings/Settings.h"

#include "Delegate.h"
#include "Model.h"

const static int PLAYLIST_BOLD=70;

struct PlaylistItemDelegate::Private
{
	int		drag_row;
	QString	entry_look;

	Private() :
		drag_row(-1)
	{
		entry_look = Settings::instance()->get(Set::PL_EntryLook);
	}
};

PlaylistItemDelegate::PlaylistItemDelegate(QTableView* parent) :
	StyledItemDelegate(parent),
	SayonaraClass()
{
	m = Pimpl::make<Private>();

	Set::listen(Set::PL_EntryLook, this, &PlaylistItemDelegate::sl_look_changed, false);
}

PlaylistItemDelegate::~PlaylistItemDelegate() {}

void PlaylistItemDelegate::paint(QPainter *painter,	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(!index.isValid()) {
		return;
	}

	QPalette palette = option.palette;
	QRect rect(option.rect);

	int col = index.column();
	int row = index.row();
	int row_height = rect.height();

	int y = rect.topLeft().y() + row_height - 1;

	StyledItemDelegate::paint(painter, option, index);

	if(m->drag_row == row) {
		painter->drawLine(QLine(rect.x(), y, rect.x() + rect.width(), y));
	}

	if(col != PlaylistItemModel::ColumnName::Description) {
		return;
	}

	painter->save();

	const PlaylistItemModel* model = static_cast<const PlaylistItemModel*>(index.model());
	const MetaData& md = model->metadata(row);

	if(md.is_disabled)
	{
		QColor col_text = palette.color(QPalette::Disabled, QPalette::Foreground);
		if(_settings->get(Set::Player_Style) == 1){
			col_text.setAlpha(196);
		}

		QPen pen = painter->pen();
		pen.setColor(col_text);
		painter->setPen(pen);
	}

	QFont font = option.font;

	/** Time **/
	QString str;
	int offset_x = 4;

	bool bold = font.bold();

	painter->translate(-4, 0);
	font.setWeight(QFont::Normal);
	painter->setFont(font);

	if(bold){
		font.setWeight(PLAYLIST_BOLD);
	}

	painter->setFont(font);
	painter->translate(4, 0);

	for(int i=0; i<m->entry_look.size(); i++)
	{
		bool print = (i == m->entry_look.size() - 1);

		QChar c = m->entry_look.at(i);

		if(c == '*'){
			print = true;
		}

		else if(c == '\''){
			print = true;
		}

		else {
			str += c;
		}

		if(print)
		{
			QFontMetrics fm(font);
			painter->translate(offset_x, 0);

			str.replace("%title%", md.title());
			str.replace("%nr%", QString::number(md.track_num));
			str.replace("%artist%", md.artist());
			str.replace("%album%", md.album());

			painter->drawText(rect,
							  (Qt::AlignLeft | Qt::AlignVCenter),
							  fm.elidedText(str, Qt::ElideRight, rect.width()));

			offset_x = fm.width(str);
			rect.setWidth(rect.width() - offset_x);
			str = "";
		}

		if(c == '*')
		{
			if(font.weight() == PLAYLIST_BOLD){
				font.setWeight(QFont::Normal);
			}
			else {
				font.setWeight(PLAYLIST_BOLD);
			}
			painter->setFont(font);
		}

		else if(c == '\''){
			font.setItalic(!font.italic());
			painter->setFont(font);
		}
	}

	painter->restore();
}

void PlaylistItemDelegate::set_drag_index(int row)
{
	m->drag_row = row;
}

bool PlaylistItemDelegate::is_drag_index(int row) const
{
	return (row == m->drag_row);
}

int PlaylistItemDelegate::drag_index() const
{
	return m->drag_row;
}


void PlaylistItemDelegate::sl_look_changed()
{
	m->entry_look = _settings->get(Set::PL_EntryLook);
}
