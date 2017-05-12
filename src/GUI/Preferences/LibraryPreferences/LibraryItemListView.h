#ifndef LIBRARYITEMLISTVIEW_H
#define LIBRARYITEMLISTVIEW_H

#include <QListView>

class LibraryItemListView :
		public QListView
{
public:
	LibraryItemListView(QWidget* parent=nullptr);
	~LibraryItemListView();
};

#endif // LIBRARYITEMLISTVIEW_H
