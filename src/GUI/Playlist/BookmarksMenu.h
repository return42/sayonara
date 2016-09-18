#ifndef BOOKMARKS_ACTION_H
#define BOOKMARKS_ACTION_H

class MetaData;
class Bookmarks;
class Bookmark;

#include <QMenu>
#include <QWidget>
#include <QList>


class BookmarksMenu :
	public QMenu
{
	Q_OBJECT

signals:
	void sig_bookmark_pressed(quint32 time_sec);

private:
	Bookmarks*	_bookmarks=nullptr;

public:
	BookmarksMenu(QWidget* parent);
	virtual ~BookmarksMenu();

	void set_metadata(const MetaData& md);
	bool has_bookmarks() const;

private slots:
	void action_pressed();
	void bookmarks_changed(const QList<Bookmark>& bookmarks);

	
};


#endif
