#ifndef ALBUMCOVERVIEW_H
#define ALBUMCOVERVIEW_H

#include "GUI/Library/Views/LibraryView.h"

class AlbumCoverView :
		public LibraryView
{
public:
	explicit AlbumCoverView(QWidget* parent=nullptr);
	virtual ~AlbumCoverView();

	// SayonaraSelectionView interface
public:
	int get_index_by_model_index(const QModelIndex& idx) const override;
	QModelIndex get_model_index_by_index(int idx) const override;

	MD::Interpretation get_metadata_interpretation() const override;
};

#endif // ALBUMCOVERVIEW_H
