#ifndef PLAYLISTCONTEXTMENU_H
#define PLAYLISTCONTEXTMENU_H

#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"

class PlaylistContextMenu :
		public LibraryContextMenu
{
	Q_OBJECT

	enum Entry {
		EntryPlayNextCopy=(LibraryContextMenu::EntryLast << 1),
		EntryPlayNextMove=(LibraryContextMenu::EntryLast << 2)
	};

public:
	explicit PlaylistContextMenu(QWidget *parent=nullptr);
	virtual ~PlaylistContextMenu();

	void show_actions(LibraryContexMenuEntries entries) override;
	LibraryContexMenuEntries get_entries() const override;


signals:
	void sig_play_next_copy_clicked();
	void sig_play_next_move_clicked();

private:
	QAction* _play_next_copy_action=nullptr;
	QAction* _play_next_move_action=nullptr;

};

#endif // PLAYLISTCONTEXTMENU_H
