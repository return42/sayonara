#ifndef LIBRARYTABLEVIEW_H
#define LIBRARYTABLEVIEW_H

#include "LibraryView.h"
#include "Helper/Library/Sortorder.h"
#include "Helper/Pimpl.h"

class HeaderView;
class LibraryTableView : public LibraryView
{
	Q_OBJECT
	PIMPL(LibraryTableView)

signals:
	void sig_columns_changed(const BoolList&);
	void sig_sortorder_changed(Library::SortOrder);

private:
	LibraryTableView(const LibraryTableView& other)=delete;
	LibraryTableView& operator=(const LibraryTableView& other)=delete;

public:
	explicit LibraryTableView(QWidget* parent=nullptr);
	~LibraryTableView();

	void set_table_headers(const ColumnHeaderList& headers, const BoolList& shown_cols, Library::SortOrder sorting);
	int get_index_by_model_index(const QModelIndex& idx) const override;
	QModelIndex get_model_index_by_index(int idx) const override;

protected:
	HeaderView*	get_header_view();
	void resizeEvent(QResizeEvent* e) override;

protected slots:
	void header_actions_triggered(const BoolList& shown_cols);
	void sort_by_column(int column_idx);
	void language_changed();
};

#endif // LIBRARYTABLEVIEW_H
