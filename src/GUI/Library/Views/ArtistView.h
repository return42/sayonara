#ifndef ARTISTVIEW_H
#define ARTISTVIEW_H

#include "TableView.h"
#include "Utils/Pimpl.h"
#include "Utils/Library/Sortorder.h"

namespace Library
{
	class ArtistView :
			public Library::TableView
	{
		PIMPL(ArtistView)
		public:
			explicit ArtistView(QWidget* parent=nullptr);
			~ArtistView();

		// ItemView interface
		protected:
			void selection_changed(const IndexSet& indexes) override;
			void merge_action_triggered() override;
			void play_next_clicked() override;
			void middle_clicked() override;
			void append_clicked() override;
			void refresh_clicked() override;

		// TableView interface
		protected:
			void init_view(AbstractLibrary* library) override;
			ColumnHeaderList column_headers() const override;
			BoolList shown_columns() const override;
			Library::SortOrder sortorder() const override;

			void columns_changed() override;
			void sortorder_changed(SortOrder s) override;

		private slots:
			void double_clicked(const QModelIndex& index);
			void artists_ready();
			void use_clear_button_changed();
	};
}

#endif // ARTISTVIEW_H
