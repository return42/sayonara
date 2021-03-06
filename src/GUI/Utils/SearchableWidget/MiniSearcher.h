/* MiniSearcher.h */

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

#ifndef MINISEARCHER_H
#define MINISEARCHER_H

#include "Utils/Pimpl.h"
#include "GUI/Utils/Widgets/WidgetTemplate.h"

#include <QObject>
#include <QFrame>
#include <QMap>

class QAbstractItemView;
class QEvent;
class QKeyEvent;
class QFocusEvent;
class QHideEvent;

class MiniSearchEventFilter :
        public QObject
{
    Q_OBJECT

signals:
    void sig_tab_pressed();
    void sig_focus_lost();

public:
    using QObject::QObject;

protected:
    bool eventFilter(QObject* o, QEvent* e) override;
};


class MiniSearcher :
        public Gui::WidgetTemplate<QFrame>
{
    Q_OBJECT
	PIMPL(MiniSearcher)

signals:
    void sig_reset();
	void sig_text_changed(const QString&);
	void sig_find_next_row();
	void sig_find_prev_row();

private slots:
	void line_edit_focus_lost();
	void left_clicked();
	void right_clicked();

private:
	bool is_initiator(QKeyEvent* event) const;
    void init(const QString& text);
    bool check_and_init(QKeyEvent* event);
	QRect calc_geo() const;

protected:
    void language_changed() override;

    void keyPressEvent(QKeyEvent* e) override;

	void showEvent(QShowEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;

public:
    MiniSearcher(QAbstractItemView* parent);
	virtual ~MiniSearcher();

    void    handle_key_press(QKeyEvent* e);
    void    set_extra_triggers(const QMap<QChar, QString>& triggers);
    QString get_current_text();
    void    set_number_results(int results);
    void    add_tooltip_text(const QString& str);
    void    reset_tooltip();
	void	set_padding(int padding);

public slots:
	void reset();
};

#endif // MINISEARCHER_H
