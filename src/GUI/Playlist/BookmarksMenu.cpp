#include "BookmarksMenu.h"

#include "Components/Bookmarks/Bookmarks.h"
#include "Components/Bookmarks/Bookmark.h"
#include "Helper/MetaData/MetaData.h"

BookmarksMenu::BookmarksMenu(QWidget* parent) :
	QMenu(parent)
{
	_bookmarks = new Bookmarks(this);

	this->setTitle(tr("Bookmarks"));

	connect(_bookmarks, &Bookmarks::sig_bookmarks_changed, this, &BookmarksMenu::bookmarks_changed);
}

BookmarksMenu::~BookmarksMenu(){}

bool BookmarksMenu::has_bookmarks() const
{
	return (this->actions().size() > 0);
}

void BookmarksMenu::bookmarks_changed(const QList<Bookmark>& bookmarks)
{
	this->clear();

	for(const Bookmark& bookmark : bookmarks){
		QString name = bookmark.get_name();
		if(name.isEmpty()){
			continue;
		}

		QAction* action = this->addAction(name);
		action->setData(bookmark.get_time());
		connect(action, &QAction::triggered, this, &BookmarksMenu::action_pressed);
	}
}

void BookmarksMenu::action_pressed()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	int time = action->data().toInt();
	emit sig_bookmark_pressed(time);
}


