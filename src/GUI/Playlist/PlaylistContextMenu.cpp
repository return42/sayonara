#include "PlaylistContextMenu.h"

PlaylistContextMenu::PlaylistContextMenu(QObject *parent) :
	LibraryContextMenu(parent)
{
	_play_next_copy_action = new QAction(GUI::get_icon("fwd"), tr("Play next") + " (" + tr("copy") + ")", this);
	_play_next_move_action = new QAction(GUI::get_icon("fwd"), tr("Play next") + " (" + tr("move") + ")", this);

	connect(_play_next_copy_action, &QAction::triggered, this, &PlaylistContextMenu::sig_play_next_copy_clicked);
	connect(_play_next_move_action, &QAction::triggered, this, &PlaylistContextMenu::sig_play_next_move_clicked);

	QList<QAction*> actions;
	actions << _play_next_copy_action
			<< _play_next_move_action;

	this->addSeparator();
	this->addActions(actions);

	for(QAction* action : actions){
		action->setVisible(false);
	}
}

PlaylistContextMenu::~PlaylistContextMenu()
{
	delete _play_next_copy_action;
	delete _play_next_move_action;
}


void PlaylistContextMenu::show_actions(LibraryContexMenuEntries entries)
{
	LibraryContextMenu::show_actions(entries);
	_play_next_copy_action->setVisible(entries & EntryPlayNextCopy);
	_play_next_move_action->setVisible(entries & EntryPlayNextCopy);
}


LibraryContexMenuEntries PlaylistContextMenu::get_entries() const
{

	LibraryContexMenuEntries entries = LibraryContextMenu::get_entries();

	if(_play_next_copy_action->isVisible()){
		entries |= EntryPlayNextCopy;
	}
	if(_play_next_move_action->isVisible()){
		entries |= EntryPlayNextMove;
	}

	return entries;
}
