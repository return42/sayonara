/* SearchableTableView.h */

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

#ifndef SEARCHABLETABLEVIEW_H
#define SEARCHABLETABLEVIEW_H

#include "SearchableView.h"

#include <QTableView>
#include <QString>
#include <QWidget>


class Settings;
class MiniSearcher;
class SearchModelInterface;

class SearchableTableView :
		public QTableView,
		public SearchViewInterface
{
	Q_OBJECT

private slots:
	void edit_changed(const QString& str);
	void fwd_clicked();
	void bwd_clicked();

private:
	MiniSearcher* _mini_searcher=nullptr;

protected:
	MiniSearcher* mini_searcher() const override;
	void keyPressEvent(QKeyEvent *event) override;


public:
	explicit SearchableTableView(QWidget* parent=nullptr);
	virtual ~SearchableTableView();
};


#endif
