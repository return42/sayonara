#ifndef PLAYLISTCONTEXTMENU_H
#define PLAYLISTCONTEXTMENU_H

#include "GUI/Utils/ContextMenu/LibraryContextMenu.h"

class PlaylistContextMenu :
		public LibraryContextMenu
{
	Q_OBJECT
	PIMPL(PlaylistContextMenu)

public:
	explicit PlaylistContextMenu(QWidget* parent);
	~PlaylistContextMenu();

	void set_bookmarks_visible(bool b);
	bool is_bookmarks_visible() const;
};


#endif // PLAYLISTCONTEXTMENU_H
