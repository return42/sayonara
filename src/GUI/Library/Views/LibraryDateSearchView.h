#ifndef LIBRARYDATESEARCHVIEW_H
#define LIBRARYDATESEARCHVIEW_H

#include "GUI/Helper/SearchableWidget/SearchableListView.h"
#include "Helper/Pimpl.h"

#include <QContextMenuEvent>

namespace Library { class DateFilter; }

class LibraryDateSearchView :
		public SearchableListView
{
	Q_OBJECT

	PIMPL(LibraryDateSearchView)

public:
	explicit LibraryDateSearchView(QWidget* parent=nullptr);
	~LibraryDateSearchView();

	Library::DateFilter get_filter(int row) const;

protected:
	void contextMenuEvent(QContextMenuEvent* e) override;

protected slots:
	void new_clicked();
	void edit_clicked();
	void delete_clicked();
};


#endif // LIBRARYDATESEARCHVIEW_H
