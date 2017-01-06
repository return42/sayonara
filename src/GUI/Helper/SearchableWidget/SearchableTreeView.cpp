/* SearchableTreeView.cpp */

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

#include "SearchableTreeView.h"
#include "AbstractSearchModel.h"
#include "MiniSearcher.h"

SearchableTreeView::SearchableTreeView(QWidget* parent) :
    QTreeView(parent),
    SearchViewInterface(this)
{
    _mini_searcher = new MiniSearcher(this, MiniSearcherButtons::BothButtons);

    connect(_mini_searcher, &MiniSearcher::sig_text_changed, this, &SearchableTreeView::edit_changed);
    connect(_mini_searcher, &MiniSearcher::sig_find_next_row, this, &SearchableTreeView::fwd_clicked);
    connect(_mini_searcher, &MiniSearcher::sig_find_prev_row, this, &SearchableTreeView::bwd_clicked);
}

SearchableTreeView::~SearchableTreeView() {}

MiniSearcher* SearchableTreeView::mini_searcher() const
{
    return _mini_searcher;
}

void SearchableTreeView::keyPressEvent(QKeyEvent* event)
{
    handleKeyPress(event);
    QTreeView::keyPressEvent(event);
}

void SearchableTreeView::edit_changed(const QString& str)
{
    select_match(str, SearchDirection::First);
}

void SearchableTreeView::fwd_clicked()
{
    QString str = _mini_searcher->get_current_text();
    select_match(str, SearchDirection::Next);
}

void SearchableTreeView::bwd_clicked()
{
    QString str = _mini_searcher->get_current_text();
    select_match(str, SearchDirection::Prev);
}
