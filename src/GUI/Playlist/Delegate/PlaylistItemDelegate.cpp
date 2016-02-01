/* PlaylistItemDelegate.cpp */

/* Copyright (C) 2011-2016- 2014  Lucio Carreras
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

#include "PlaylistItemDelegate.h"
#include "GUI/Playlist/Model/PlaylistItemModel.h"

#include <QFontMetrics>

PlaylistItemDelegate::PlaylistItemDelegate(QListView* parent) :
	QStyledItemDelegate(parent),
	SayonaraClass()
{

	_parent = parent;
	_max_width = parent->width();
	_drag_row = -1;
	_show_numbers = _settings->get(Set::PL_ShowNumbers);
	_row_height = 20;

	REGISTER_LISTENER_NO_CALL(Set::PL_ShowNumbers, _sl_show_numbers_changed);
}

PlaylistItemDelegate::~PlaylistItemDelegate() {

}


void PlaylistItemDelegate::paint( QPainter *painter,
						const QStyleOptionViewItem &option,
						const QModelIndex &index) const
{

	if(!index.isValid()) return;

	painter->save();

	QRect rect(option.rect);
	rect.setWidth(_max_width);

	int row = index.row();
	int y = rect.topLeft().y() +  _row_height -1;

	const PlaylistItemModel* model = static_cast<const PlaylistItemModel*>(index.model());
	const MetaData& md = model->get_md(row);

	if(option.state & QStyle::State_Selected){

		QPalette palette = _parent->palette();
		QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);

		painter->fillRect(rect, col_highlight);
	}

	else if(md.pl_playing){

		QPalette palette = _parent->palette();
		QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);
		col_highlight.setAlpha(160);

		painter->fillRect(rect, col_highlight);
	}

	if(_drag_row == row) {
		painter->drawLine(QLine(0, y, _max_width, y));
	}


	QFont font = painter->font();
	int point_size = font.pointSize();

	/** Time **/
	QString time_string = Helper::cvt_ms_to_string(md.length_ms, true, true, false);
	font.setBold(false);
	font.setPointSize(point_size -1);
	painter->setFont(font);

	painter->translate(-4, 0);
	painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, time_string);
	painter->translate(4, 0);


	/** Title **/

	QString title_string;
	if(_show_numbers){
		title_string = QString::number(row + 1) + ". " + md.title;
	}
	else{
		title_string = md.title;
	}

	font.setBold(true);
	font.setPointSize(point_size);
	painter->setFont(font);

	QFontMetrics fm(font);


	if(!time_string.isEmpty()){
		rect.setWidth(rect.width() - 50);
	}

	painter->translate(2, 0);
	painter->drawText(rect,
					  (Qt::AlignLeft | Qt::AlignVCenter),
					  fm.elidedText(title_string, Qt::ElideRight, rect.width()) );


	/** Artist **/
	int x_offset = fm.width(title_string) + 5;

	font.setBold(false);
	font.setPointSize(point_size -1);
	QFontMetrics fm2(font);

	painter->setFont(font);

	rect.setWidth(rect.width() - x_offset);

	painter->translate(x_offset, 0);
	painter->drawText(rect,
					  (Qt::AlignLeft | Qt::AlignVCenter),
					  fm2.elidedText(md.artist, Qt::ElideRight, rect.width()));

	painter->restore();
}


QSize PlaylistItemDelegate::sizeHint(const QStyleOptionViewItem &option,
	                     const QModelIndex &index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);

	return QSize( _max_width, _row_height);
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


void PlaylistItemDelegate::set_max_width(int w) {
	_max_width = w;
}

int PlaylistItemDelegate::get_row_height() const
{
	return _row_height;
}

void PlaylistItemDelegate::set_drag_index(int row){
	_drag_row = row;
}

bool PlaylistItemDelegate::is_drag_index(int row) const
{
	return (row == _drag_row);
}

int PlaylistItemDelegate::get_drag_index() const
{
	return _drag_row;
}

void PlaylistItemDelegate::_sl_show_numbers_changed()
{
	_show_numbers = _settings->get(Set::PL_ShowNumbers);
}

