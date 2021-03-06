/* PlaylistItemDelegate.h */

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

#ifndef PLAYLISTITEMDELEGATE_H_
#define PLAYLISTITEMDELEGATE_H_

#include "Utils/Settings/SayonaraClass.h"
#include "GUI/Utils/Delegates/StyledItemDelegate.h"

class QListView;
class PlaylistItemDelegate :
		public Gui::StyledItemDelegate,
		public SayonaraClass
{
	Q_OBJECT

public:
	PlaylistItemDelegate(QListView* parent);
	virtual ~PlaylistItemDelegate();

	void paint( QPainter *painter, const QStyleOptionViewItem &option,
						 const QModelIndex &index) const override;


	QWidget* createEditor(QWidget *parent,
						  const QStyleOptionViewItem &option,
						  const QModelIndex &index) const override;

    void set_drag_index(int row);
    bool is_drag_index(int row) const;
    int  drag_index() const;


private:
	int					_drag_row;
	bool				_show_numbers;
	QString				_entry_template;


private slots:
	int draw_number(QPainter* painter, int number, QFont& font, QRect& rect) const;

	void _sl_show_numbers_changed();
	void _sl_look_changed();
};

#endif /* PLAYLISTITEMDELEGATEINTERFACE_H_ */
