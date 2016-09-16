#include "BookmarksAction.h"
#include "Components/Bookmarks/Bookmarks.h"
#include "Components/Bookmarks/Bookmark.h"
#include "Helper/MetaData/MetaData.h"

BookmarksAction::BookmarksAction(QWidget* parent) :
	QMenu(parent)
{
	_bookmarks = new Bookmarks(this);
	this->setTitle(tr("Bookmarks"));
	connect(_bookmarks, &Bookmarks::sig_bookmarks_changed, this, &BookmarksAction::bookmarks_changed);
}

BookmarksAction::~BookmarksAction(){}

void BookmarksAction::bookmarks_changed(const QList<Bookmark>& bookmarks)
{
	this->clear();

	for(const Bookmark& bookmark : bookmarks){
		QAction* action = this->addAction(bookmark.get_name());
		action->setData(bookmark.get_time());
		connect(action, &QAction::triggered, this, &BookmarksAction::action_pressed);
	}
}

void BookmarksAction::action_pressed()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	int time = action->data().toInt();
	emit sig_bookmark_pressed(time);
}


