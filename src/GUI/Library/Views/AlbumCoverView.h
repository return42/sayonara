#ifndef ALBUMCOVERVIEW_H
#define ALBUMCOVERVIEW_H

#include "GUI/Library/Views/LibraryView.h"
#include "Helper/Pimpl.h"

class AlbumCoverModel;
class AlbumCoverView :
		public LibraryView
{
	Q_OBJECT
	PIMPL(AlbumCoverView)

public:
	explicit AlbumCoverView(QWidget* parent=nullptr);
	virtual ~AlbumCoverView();

	int get_index_by_model_index(const QModelIndex& idx) const override;
	QModelIndex get_model_index_by_index(int idx) const override;

	void setModel(AlbumCoverModel* model);
	void refresh();

protected:
	void wheelEvent(QWheelEvent* e) override;
	void resizeEvent(QResizeEvent* e) override;

	QStyleOptionViewItem viewOptions() const override;

private:
	void change_zoom(int zoom=-1);

	void setModel(QAbstractItemModel* m) override;
	void setModel(LibraryItemModel* m) override;
};

#endif // ALBUMCOVERVIEW_H
