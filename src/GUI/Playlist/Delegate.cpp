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
#include <QListView>

#include "Utils/Utils.h"
#include "Utils/MetaData/MetaData.h"
#include "Utils/Settings/Settings.h"

#include "Delegate.h"
#include "Model.h"

#define PLAYLIST_BOLD 70

PlaylistItemDelegate::PlaylistItemDelegate(QListView* parent) :
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

	painter->save();

	QPalette palette = option.palette;
	QRect rect(option.rect);

	int row = index.row();
	int row_height = rect.height();
	int max_width = rect.width();

	int y = rect.topLeft().y() + row_height - 1;

	const PlaylistItemModel* model = static_cast<const PlaylistItemModel*>(index.model());
	const MetaData& md = model->metadata(row);

	bool is_playing = md.pl_playing;
	bool is_selected = (option.state & QStyle::State_Selected);

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

	else if(is_playing || is_selected)
	{
		QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);

		if(is_selected){
			painter->fillRect(option.rect, col_highlight);
		}

		else if(is_playing)
		{
			col_highlight.setAlpha(160);
			painter->fillRect(option.rect, col_highlight);
		}
	}

	if(_drag_row == row) {
		painter->drawLine(QLine(0, y, max_width, y));
	}

	QFont font = option.font;

	/** Time **/
	QString str;
	int offset_x = 4;

	bool bold = font.bold();
	QString time_string = Util::cvt_ms_to_string(md.length_ms, true, true, false);

	painter->translate(-4, 0);
	font.setWeight(QFont::Normal);
	painter->setFont(font);
	QFontMetrics fm(font);
	int time_width = fm.width(time_string) * 2;
	painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, time_string);
	if(bold){
		font.setWeight(PLAYLIST_BOLD);
	}

	painter->setFont(font);
	painter->translate(4, 0);

	if(_show_numbers){
		offset_x = draw_number(painter, row + 1, font, rect);
	}

	if(!time_string.isEmpty()){
		rect.setWidth(rect.width() - time_width);
	}
	else {
		rect.setWidth(rect.width() - 20);
	}


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

			str.replace("%title%", md.title);
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


QWidget* PlaylistItemDelegate::createEditor(QWidget* parent,
										   const QStyleOptionViewItem& option,
										   const QModelIndex& index) const
{
	Q_UNUSED(parent)
	Q_UNUSED(option)
	Q_UNUSED(index)

	return nullptr;
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

int PlaylistItemDelegate::draw_number(QPainter* painter, int number, QFont& font, QRect& rect) const
{
	font.setWeight(PLAYLIST_BOLD);

	QString str;
	QFontMetrics fm(font);

	painter->translate(4, 0);
	str = QString::number(number) + ". ";

	painter->setFont(font);
	painter->drawText(rect,
					  (Qt::AlignLeft | Qt::AlignVCenter),
					  str);


	font.setWeight(QFont::Normal);
	painter->setFont(font);

	return fm.width(str);
}

void PlaylistItemDelegate::_sl_show_numbers_changed()
{
	_show_numbers = _settings->get(Set::PL_ShowNumbers);
}

void PlaylistItemDelegate::_sl_look_changed()
{
	_entry_template = _settings->get(Set::PL_EntryLook);
}


