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

#include <QLabel>
#include <QListView>
#include <QPainter>
#include <QFontMetrics>
#include <QApplication>
#include <QTableView>

#include "Utils/Utils.h"
#include "Utils/MetaData/MetaData.h"
#include "Utils/Settings/Settings.h"

#include "Delegate.h"
#include "Model.h"

#define PLAYLIST_BOLD 70

PlaylistItemDelegate::PlaylistItemDelegate(QTableView* parent) :
	StyledItemDelegate(parent),
	SayonaraClass()
{
	_drag_row = -1;
	_show_numbers = _settings->get(Set::PL_ShowNumbers);
	_entry_template = _settings->get(Set::PL_EntryLook);

	Set::listen(Set::PL_ShowNumbers, this, &PlaylistItemDelegate::_sl_show_numbers_changed, false);
	Set::listen(Set::PL_EntryLook, this, &PlaylistItemDelegate::_sl_look_changed, false);
}

PlaylistItemDelegate::~PlaylistItemDelegate() {}

void PlaylistItemDelegate::paint(QPainter *painter,	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(!index.isValid()) {
		return;
	}

	int col = index.column();
	StyledItemDelegate::paint(painter, option, index);
	if(col != 1)
	{
		return;
	}

	painter->save();

	QPalette palette = option.palette;
	QRect rect(option.rect);

	int row = index.row();
	int row_height = rect.height();
	int max_width = rect.width();

	int y = rect.topLeft().y() + row_height - 1;

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

	if(_drag_row == row) {
		painter->drawLine(QLine(0, y, max_width, y));
	}

	QFont font = option.font;

	/** Time **/
	QString str;
	int offset_x = 4;

	bool bold = font.bold();

	painter->translate(-4, 0);
	font.setWeight(QFont::Normal);
	painter->setFont(font);
	QFontMetrics fm(font);
	if(bold){
		font.setWeight(PLAYLIST_BOLD);
	}

	painter->setFont(font);
	painter->translate(4, 0);

	for(int i=0; i<_entry_template.size(); i++)
	{
		bool print = (i == _entry_template.size() - 1);

		QChar c = _entry_template.at(i);

		if(c == '*'){
			print = true;
		}

		else if(c == '\''){
			print = true;
		}

		else {
			str += c;
		}

		if(print){
			QFontMetrics fm(font);
			painter->translate(offset_x, 0);

			str.replace("%title%", md.title());
			str.replace("%nr%", "");
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
	_drag_row = row;
}

bool PlaylistItemDelegate::is_drag_index(int row) const
{
	return (row == _drag_row);
}

int PlaylistItemDelegate::drag_index() const
{
	return _drag_row;
}

void PlaylistItemDelegate::_sl_show_numbers_changed()
{
	_show_numbers = _settings->get(Set::PL_ShowNumbers);
}

void PlaylistItemDelegate::_sl_look_changed()
{
	_entry_template = _settings->get(Set::PL_EntryLook);
}

QSize PlaylistItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QFontMetrics fm(option.font);
	if(index.column() == 0){
		QString str = QString("%1.").arg(index.model()->rowCount() * 10 + 1);
		return QSize(fm.width(str), 20);
	}

	if(index.column() == 1)
	{
		return StyledItemDelegate::sizeHint(option, index);
	}

	return QSize(fm.width("123:23"), 20);
}
