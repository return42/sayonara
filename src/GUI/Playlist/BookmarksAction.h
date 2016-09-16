#ifndef BOOKMARKS_ACTION_H
#define BOOKMARKS_ACTION_H

class MetaData;
class Bookmarks;
class Bookmark;

#include <QMenu>
#include <QWidget>
#include <QList>


class BookmarksAction : 
	public QMenu
{
	Q_OBJECT

signals:
	void sig_bookmark_pressed(quint32 time_sec);

private:
	Bookmarks* _bookmarks=nullptr;

public:
	BookmarksAction(QWidget* parent);
	virtual ~BookmarksAction();

	void set_metadata(const MetaData& md);

private slots:
	void action_pressed();
	void bookmarks_changed(const QList<Bookmark>& bookmarks);

	
};


#endif
