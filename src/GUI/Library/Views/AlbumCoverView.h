#ifndef ALBUMCOVERVIEW_H
#define ALBUMCOVERVIEW_H

#include "GUI/Library/Views/LibraryView.h"

class AlbumCoverView :
		public LibraryView
{
public:
	explicit AlbumCoverView(QWidget* parent=nullptr);
	virtual ~AlbumCoverView();

	int get_index_by_model_index(const QModelIndex &idx) const override;
};

#endif // ALBUMCOVERVIEW_H
