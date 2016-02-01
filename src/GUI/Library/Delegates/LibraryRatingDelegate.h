/* LibraryRatingDelegate.h */

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


#ifndef LIBRARY_RATING_DELEGATE_H
#define LIBRARY_RATING_DELEGATE_H

#include <QItemDelegate>
#include "GUI/Library/Views/LibraryView.h"
#include "GUI/Library/Models/LibraryItemModel.h"


class LibraryRatingDelegate : public QItemDelegate
{
    Q_OBJECT

signals:
    void sig_rating_changed(int);

public:
	LibraryRatingDelegate(LibraryView* parent, bool enabled);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    

private slots:
    void destroy_editor(bool save=false);


protected:
    LibraryView* _parent;
	bool _enabled;
    
};

#endif // LibraryRatingDelegate_H
