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

#include <QFrame>
#include <QLineEdit>
#include <QBoxLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QScrollArea>
#include <QShortcut>
#include <QAbstractItemView>

class SearchableTableView;
class SearchableListView;

enum class MiniSearcherButtons : quint8
{
  NoButton=0,
  FwdButton,
  BwdButton,
  BothButtons
};

class MiniSearcherLineEdit : public QLineEdit 
{
	Q_OBJECT

signals:
	void sig_tab_pressed();
	void sig_esc_pressed();
	void sig_le_focus_lost();

public:
	explicit MiniSearcherLineEdit(QWidget* parent=nullptr);
	virtual ~MiniSearcherLineEdit();

	void focusOutEvent(QFocusEvent* e) override;
};


class MiniSearcher : public QFrame
{
    Q_OBJECT

signals:
    void sig_reset();
	void sig_text_changed(const QString&);
	void sig_find_next_row();
	void sig_find_prev_row();

private slots:

	void line_edit_text_changed(const QString&);
	void line_edit_focus_lost();
	void left_clicked();
	void right_clicked();


private:
	QAbstractItemView*		_parent=nullptr;
	QMap<QChar, QString>    _triggers;
	QPushButton*            _left_button=nullptr;
	QPushButton*            _right_button=nullptr;
	MiniSearcherLineEdit*   _line_edit=nullptr;
	QBoxLayout*             _layout=nullptr;
	QShortcut*				_esc_shortcut=nullptr;

	bool is_initiator(QKeyEvent* event);
    void init(QString text);
	void init_layout(MiniSearcherButtons b);


public:

	MiniSearcher(QAbstractItemView* parent, MiniSearcherButtons b=MiniSearcherButtons::NoButton);

    bool check_and_init(QKeyEvent* event);
	void set_extra_triggers(const QMap<QChar, QString>& triggers);
	QString get_current_text();

	void keyPressEvent(QKeyEvent *e) override;
	void showEvent(QShowEvent* e) override;
	void hideEvent(QHideEvent *e) override;
	void focusOutEvent(QFocusEvent* e) override;


public slots:
	void reset();
};

#endif // MINISEARCHER_H
