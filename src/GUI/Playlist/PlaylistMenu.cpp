/* PlaylistMenu.cpp */

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

#include "PlaylistMenu.h"
#include "Helper/Playlist/PlaylistMode.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"


PlaylistMenu::PlaylistMenu(QWidget* parent) :
	QMenu(parent),
	SayonaraClass()
{

	_timer = new QTimer(this);

	_action_rep1 = new QAction(QString(), this);
	_action_repAll = new QAction(QString(), this);
	_action_append = new QAction(QString(), this);
	_action_shuffle = new QAction(QString(), this);
	_action_dynamic = new QAction(QString(), this);
	_action_gapless = new QAction(QString(), this);
	_action_shutdown = new QAction(QString(), this);

	QList<QAction*> actions;
	actions << _action_rep1
			<< _action_repAll
			<< _action_append
			<< _action_dynamic
			<< _action_shuffle
			<< _action_gapless
			<< addSeparator()
			<< _action_shutdown;

	for(QAction* action : actions){
		action->setCheckable(true);
		action->setVisible(true);
	}

	this->addActions(actions);

	Playlist::Mode plm = _settings->get(Set::PL_Mode);

	_action_rep1->setChecked(Playlist::Mode::isActive(plm.rep1()));
	_action_append->setChecked(Playlist::Mode::isActive(plm.append()));
	_action_repAll->setChecked(Playlist::Mode::isActive(plm.repAll()));
	_action_dynamic->setChecked(Playlist::Mode::isActive(plm.dynamic()));
	_action_shuffle->setChecked(Playlist::Mode::isActive(plm.shuffle()));
	_action_gapless->setChecked(Playlist::Mode::isActive(plm.gapless()));
	_action_shutdown->setChecked(false);

	connect(_timer, &QTimer::timeout, this, &PlaylistMenu::timed_out);
	connect(_action_rep1, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_repAll, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_shuffle, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_dynamic, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_gapless, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_shutdown, &QAction::triggered, this, &PlaylistMenu::change_plm);
	connect(_action_shutdown, &QAction::triggered, this, &PlaylistMenu::sig_shutdown);

	REGISTER_LISTENER(Set::PL_Mode, plm_changed);
	REGISTER_LISTENER(Set::Player_Language, language_changed);
}

void PlaylistMenu::language_changed()
{
	_action_rep1->setText(Lang::get(Lang::Repeat1));
	_action_repAll->setText(Lang::get(Lang::RepeatAll));
	_action_append->setText(Lang::get(Lang::Append));
	_action_shuffle->setText(Lang::get(Lang::Shuffle));
	_action_dynamic->setText(Lang::get(Lang::DynamicPlayback));
	_action_gapless->setText(Lang::get(Lang::GaplessPlayback));
	_action_shutdown->setText(Lang::get(Lang::Shutdown));
}


void PlaylistMenu::timed_out()
{
	for(QAction* action : this->actions()){
		action->setDisabled(false);
	}
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

	Playlist::Mode plm = _settings->get(Set::PL_Mode);

	_action_append->setChecked(plm.append() == Playlist::Mode::On);
	_action_rep1->setChecked(plm.rep1() == Playlist::Mode::On);
	_action_repAll->setChecked(plm.repAll() == Playlist::Mode::On);
	_action_shuffle->setChecked(plm.shuffle() == Playlist::Mode::On);
	_action_dynamic->setChecked(plm.dynamic() == Playlist::Mode::On);
	_action_gapless->setChecked(plm.gapless() == Playlist::Mode::On);

	_action_append->setVisible(plm.append() != Playlist::Mode::Disabled);
	_action_rep1->setVisible(plm.rep1() != Playlist::Mode::Disabled);
	_action_repAll->setVisible(plm.repAll() != Playlist::Mode::Disabled);
	_action_shuffle->setVisible(plm.shuffle() != Playlist::Mode::Disabled);
	_action_dynamic->setVisible(plm.dynamic() != Playlist::Mode::Disabled);
	_action_gapless->setVisible(plm.gapless() != Playlist::Mode::Disabled);

}

void PlaylistMenu::change_plm(){

	Playlist::Mode plm = _settings->get(Set::PL_Mode);
	plm.setRep1(_action_rep1->isChecked());
	plm.setRepAll(_action_repAll->isChecked());
	plm.setShuffle(_action_shuffle->isChecked());
	plm.setAppend(_action_append->isChecked());
	plm.setDynamic(_action_dynamic->isChecked());
	plm.setGapless(_action_gapless->isChecked());

	_settings->set(Set::PL_Mode, plm);

}

void PlaylistMenu::set_shutdown(bool b){
	_action_shutdown->setChecked(b);
}


