/* PlaylistMenu.cpp */

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



#include "PlaylistMenu.h"
#include "GUI_PlaylistEntryLook.h"


PlaylistMenu::PlaylistMenu(QWidget* parent) :
	QMenu(parent),
	SayonaraClass()
{

	_timer = new QTimer(this);

	_action_rep1 = new QAction(tr("Repeat 1"), this);
	_action_repAll = new QAction(tr("Repeat all"), this);
	_action_append = new QAction(tr("Append"), this);
	_action_shuffle = new QAction(tr("Shuffle"), this);
	_action_dynamic = new QAction(tr("Dynamic playback"), this);
	_action_gapless = new QAction(tr("Gapless playback"), this);
	_action_shutdown = new QAction(tr("Shutdown"), this);
	_action_numbers = new QAction(tr("Numbers"), this);
	_action_look = new QAction(tr("Playlist look"), this);

	QList<QAction*> actions;
	actions << _action_rep1
			<< _action_repAll
			<< _action_append
			<< _action_dynamic
			<< _action_shuffle
			<< _action_gapless
			<< addSeparator()
			<< _action_numbers
			<< _action_look
			<< addSeparator()
			<< _action_shutdown;

	for(QAction* action : actions){
		action->setCheckable(true);
		action->setVisible(true);
	}

	_action_look->setCheckable(false);

	this->addActions(actions);


	PlaylistMode plm = _settings->get(Set::PL_Mode);

	_action_rep1->setChecked(plm.rep1);
	_action_append->setChecked(plm.append);
	_action_repAll->setChecked(plm.repAll);
	_action_dynamic->setChecked(plm.dynamic);
	_action_shuffle->setChecked(plm.shuffle);
	_action_gapless->setChecked(plm.gapless);
	_action_shutdown->setChecked(false);
	_action_numbers->setChecked(_settings->get(Set::PL_ShowNumbers));

	connect(_timer, &QTimer::timeout, this, &PlaylistMenu::timed_out);
	connect(_action_rep1, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_repAll, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_shuffle, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_dynamic, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_gapless, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_shutdown, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_numbers, &QAction::triggered, this, &PlaylistMenu::show_numbers);
	connect(_action_shutdown, &QAction::triggered, this, &PlaylistMenu::sig_shutdown);
	connect(_action_look, &QAction::triggered, this, &PlaylistMenu::change_look);

	REGISTER_LISTENER(Set::PL_Mode, plm_changed);
	REGISTER_LISTENER(Set::Player_Language, language_changed);
}

void PlaylistMenu::language_changed(){
	_action_rep1->setText(tr("Repeat 1"));
	_action_repAll->setText(tr("Repeat all"));
	_action_append->setText(tr("Append"));
	_action_shuffle->setText(tr("Shuffle"));
	_action_dynamic->setText(tr("Dynamic playback"));
	_action_gapless->setText(tr("Gapless playback"));
	_action_shutdown->setText(tr("Shutdown"));
	_action_numbers->setText(tr("Numbers"));
	_action_look->setText(tr("Playlist look"));
}

void PlaylistMenu::change_look()
{
	GUI_PlaylistEntryLook* entry_look = new GUI_PlaylistEntryLook(this);
	entry_look->exec();
}

void PlaylistMenu::timed_out()
{
	for(QAction* action : this->actions()){
		action->setDisabled(false);
	}
}

void PlaylistMenu::show_numbers(){
	_settings->set(Set::PL_ShowNumbers, _action_numbers->isChecked());
}

void PlaylistMenu::showEvent(QShowEvent* e)
{

	for(QAction* action : this->actions()){
		action->setEnabled(false);
	}

	QMenu::showEvent(e);

	_timer->setInterval(250);
	_timer->start();
}

void PlaylistMenu::plm_changed(){

	PlaylistMode plm = _settings->get(Set::PL_Mode);

	_action_append->setChecked(plm.append);
	_action_rep1->setChecked(plm.rep1);
	_action_repAll->setChecked(plm.repAll);
	_action_shuffle->setChecked(plm.shuffle);
	_action_dynamic->setChecked(plm.dynamic);
	_action_gapless->setChecked(plm.gapless);

	_action_rep1->setVisible(plm.ui_rep1);
	_action_append->setVisible(plm.ui_append);
	_action_repAll->setVisible(plm.ui_repAll);
	_action_dynamic->setVisible(plm.ui_dynamic);
	_action_shuffle->setVisible(plm.ui_shuffle);
	_action_gapless->setVisible(plm.ui_gapless);
}

void PlaylistMenu::change_plm(){

	PlaylistMode plm = _settings->get(Set::PL_Mode);
	plm.rep1 = _action_rep1->isChecked();
	plm.repAll = _action_repAll->isChecked();
	plm.shuffle = _action_shuffle->isChecked();
	plm.append = _action_append->isChecked();
	plm.dynamic = _action_dynamic->isChecked();
	plm.gapless = _action_gapless->isChecked();

	_settings->set(Set::PL_Mode, plm);

}

void PlaylistMenu::set_shutdown(bool b){
	_action_shutdown->setChecked(b);
}


