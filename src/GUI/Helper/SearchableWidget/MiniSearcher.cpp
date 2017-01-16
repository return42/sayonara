/* MiniSearcher.cpp */

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

#include "MiniSearcher.h"
#include "GUI/Helper/GUI_Helper.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QShortcut>
#include <QKeySequence>
#include <QAbstractItemView>


MiniSearcherLineEdit::MiniSearcherLineEdit(QWidget* parent) :
	QLineEdit(parent) {}

MiniSearcherLineEdit::~MiniSearcherLineEdit() {}


void MiniSearcherLineEdit::focusOutEvent(QFocusEvent* e)
{
	emit sig_le_focus_lost();
	QLineEdit::focusOutEvent(e);
}

struct MiniSearcher::Private
{
	QAbstractItemView*		parent=nullptr;
	QMap<QChar, QString>    triggers;
	QPushButton*            left_button=nullptr;
	QPushButton*            right_button=nullptr;
	MiniSearcherLineEdit*   line_edit=nullptr;
	QBoxLayout*             layout=nullptr;
	QShortcut*				esc_shortcut=nullptr;
};


MiniSearcher::MiniSearcher(QAbstractItemView* parent, MiniSearcherButtons b) :
	QFrame(parent)
{
	_m = Pimpl::make<MiniSearcher::Private>();
	_m->parent = parent;

	init_layout(b);
}

MiniSearcher::~MiniSearcher() {}

void MiniSearcher::init_layout(MiniSearcherButtons b)
{
    bool left=false;
    bool right=false;

	_m->line_edit = new MiniSearcherLineEdit(this);
	_m->line_edit->setMaximumWidth(100);

	connect(_m->line_edit, &MiniSearcherLineEdit::textChanged, this, &MiniSearcher::line_edit_text_changed);
	connect(_m->line_edit, &MiniSearcherLineEdit::sig_tab_pressed, this, &MiniSearcher::right_clicked);
	connect(_m->line_edit, &MiniSearcherLineEdit::sig_le_focus_lost, this, &MiniSearcher::line_edit_focus_lost);
	connect(_m->line_edit, &MiniSearcherLineEdit::sig_esc_pressed, this, &MiniSearcher::close);

	_m->layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
	_m->layout->setContentsMargins(4, 4, 4, 4);
	_m->layout->setSpacing(3);
	_m->layout->addWidget(_m->line_edit);

	switch(b)
	{
		case MiniSearcherButtons::BothButtons:
			left = true; right = true;
			break;

		case MiniSearcherButtons::BwdButton:
			left = true;
			break;

		case MiniSearcherButtons::FwdButton:
			right = true;

		case MiniSearcherButtons::NoButton:

		default:
		break;
	}

	if(left) {
		_m->left_button = new QPushButton(this);
		_m->left_button->setIcon(GUI::get_icon("bwd"));
		_m->left_button->setVisible(true);
		_m->left_button->setFlat(true);
		_m->left_button->setFocusPolicy(Qt::ClickFocus);

		connect(_m->left_button, &QPushButton::clicked, this, &MiniSearcher::left_clicked);

		_m->layout->addWidget(_m->left_button);
	}

	if(right) {
		_m->right_button = new QPushButton(this);
		_m->right_button->setIcon(GUI::get_icon("fwd"));
		_m->right_button->setVisible(true);
		_m->right_button->setFlat(true);
		_m->right_button->setFocusPolicy(Qt::ClickFocus);

		connect(_m->right_button, &QPushButton::clicked, this, &MiniSearcher::right_clicked);

		_m->layout->addWidget(_m->right_button);
	}

	this->hide();
}


bool MiniSearcher::is_initiator(QKeyEvent* event) const
{
    QString text = event->text();

	if(event->modifiers() & Qt::ControlModifier){
		return false;
	}

	if(text.isEmpty()){
		return false;
	}

	if(text[0].isLetterOrNumber()){
		return true;
	}

	if(_m->triggers.contains(text[0]) ){
		return true;
	}

	return false;
}


void MiniSearcher::keyPressEvent(QKeyEvent* event)
{
	int key = event->key();

	switch(key)
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			if(this->isVisible()) {
				reset();
			}

			break;

		case Qt::Key_Down:
			right_clicked();
			break;

		case Qt::Key_Up:
			left_clicked();
			break;

		default:
			QFrame::keyPressEvent(event);
			break;
	}
}


void MiniSearcher::showEvent(QShowEvent* e)
{
	if(!_m->esc_shortcut){
		_m->esc_shortcut = new QShortcut(QKeySequence(Qt::Key_Escape),
									  this,
									  SLOT(reset()),
									  SLOT(reset()),
									  Qt::WidgetWithChildrenShortcut);
	}

	_m->esc_shortcut->setEnabled(true);

	QFrame::showEvent(e);
}

void MiniSearcher::hideEvent(QHideEvent* e)
{
	_m->esc_shortcut->setEnabled(false);
	_m->parent->setFocus();

	QFrame::hideEvent(e);
}


void MiniSearcher::focusOutEvent(QFocusEvent* e)
{
	this->hide();

	QFrame::focusOutEvent(e);
}


void MiniSearcher::line_edit_text_changed(const QString& str)
{
	emit sig_text_changed(str);
}


void MiniSearcher::line_edit_focus_lost()
{
	if(	_m->left_button->hasFocus() ||
		_m->right_button->hasFocus() ||
		this->hasFocus())
	{
		return;
	}

	reset();
}


void MiniSearcher::left_clicked()
{
	emit sig_find_prev_row();
	_m->line_edit->setFocus();
}


void MiniSearcher::right_clicked()
{
	emit sig_find_next_row();
	_m->line_edit->setFocus();
}


void MiniSearcher::init(QString text)
{
	QScrollBar* v_scrollbar = _m->parent->verticalScrollBar();
	QScrollBar* h_scrollbar = _m->parent->horizontalScrollBar();

	int sb_width = v_scrollbar->width();
	int sb_height = h_scrollbar->height();
	int par_width = _m->parent->width();
	int par_height = _m->parent->height();
    int new_width, new_height;

	if(!v_scrollbar->isVisible()) sb_width = 0;
	if(!h_scrollbar->isVisible()) sb_height = 0;

	new_width = par_width - (sb_width + 135);
	new_height = par_height - (sb_height + 40);

	this->setGeometry(new_width, new_height, 130, 35);

	_m->line_edit->setFocus();
	_m->line_edit->setText(text);

	this->show();
}


void MiniSearcher::reset()
{
	_m->line_edit->setText("");

	if(this->isVisible()){
		_m->parent->setFocus();
	}

	this->hide();
}


bool MiniSearcher::check_and_init(QKeyEvent *event)
{
	if(!is_initiator(event)) {
		return false;
	}

    if(!this->isVisible()) {
        init(event->text());
        return true;
    }

	return false;
}


void MiniSearcher::set_extra_triggers(const QMap<QChar, QString>& triggers)
{
	_m->triggers = triggers;
	QString tooltip;

	for(const QChar& key : triggers.keys()) {
		tooltip += QString(key) + " = " + triggers.value(key) + "\n";
	}

	tooltip.remove(tooltip.size() -1, 1);

	this->setToolTip(tooltip);
}


QString MiniSearcher::get_current_text()
{
	return _m->line_edit->text();
}


