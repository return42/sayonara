#ifndef ALBUMCOVERVIEW_H
#define ALBUMCOVERVIEW_H

#include "GUI/Library/Views/LibraryView.h"
#include "Helper/Pimpl.h"

class AlbumCoverView :
		public LibraryView
{
	Q_OBJECT

signals:
	void sig_zoom_changed(int zoom);

public:
	explicit AlbumCoverView(QWidget* parent=nullptr);
	virtual ~AlbumCoverView();

	// SayonaraSelectionView interface
public:
	int get_index_by_model_index(const QModelIndex& idx) const override;
	QModelIndex get_model_index_by_index(int idx) const override;

	MD::Interpretation get_metadata_interpretation() const override;

protected:
	void wheelEvent(QWheelEvent *e) override;

private:
	PIMPL(AlbumCoverView)
};

#endif // ALBUMCOVERVIEW_H
