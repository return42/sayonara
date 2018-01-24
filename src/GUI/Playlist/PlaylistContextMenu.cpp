#include "PlaylistContextMenu.h"
#include "BookmarksMenu.h"

#include "Components/PlayManager/PlayManager.h"

struct PlaylistContextMenu::Private
{
	BookmarksMenu*	bookmarks_menu=nullptr;
	QAction*		bookmarks_action=nullptr;

	Private(PlaylistContextMenu* parent)
	{
		bookmarks_menu = new BookmarksMenu(parent);
		bookmarks_action = parent->addMenu(bookmarks_menu);
	}
};

PlaylistContextMenu::PlaylistContextMenu(QWidget *parent) :
	LibraryContextMenu(parent)
{
	m = Pimpl::make<Private>(this);

	connect(m->bookmarks_menu, &BookmarksMenu::sig_bookmark_pressed, [](Seconds timestamp){
		PlayManager::instance()->seek_abs_ms(timestamp * 1000);
	});
}

PlaylistContextMenu::~PlaylistContextMenu() {}

void PlaylistContextMenu::set_bookmarks_visible(bool b)
{
	m->bookmarks_action->setVisible(b && m->bookmarks_menu->has_bookmarks());
}

bool PlaylistContextMenu::is_bookmarks_visible() const
{
	return m->bookmarks_action->isVisible();
}
