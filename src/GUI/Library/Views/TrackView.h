#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include "TableView.h"
#include "Utils/Pimpl.h"
#include "Utils/Library/Sortorder.h"

class AbstractLibrary;
namespace Library
{
	class TrackView :
		public Library::TableView
	{
		Q_OBJECT
		PIMPL(TrackView)

	public:
		explicit TrackView(QWidget* parent=nullptr);
		~TrackView();

	private:
		//from Library::TableView
		void init_view(AbstractLibrary* library) override;
		ColumnHeaderList column_headers() const override;
		BoolList shown_columns() const override;
		SortOrder sortorder() const override;

		void sortorder_changed(SortOrder s) override;
		void columns_changed() override;

		// from Library::ItemView
		void middle_clicked() override;
		void play_next_clicked() override;
		void append_clicked() override;
		void selection_changed(const IndexSet& lst) override;
		void refresh_clicked() override;

	private slots:
		void double_clicked(const QModelIndex& idx);
		void tracks_ready();
	};
}

#endif // TRACKVIEW_H
