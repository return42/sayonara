#ifndef SOMAFMSTATIONVIEW_H
#define SOMAFMSTATIONVIEW_H

#include "GUI/Helper/SearchableWidget/SearchableTableView.h"

class SomaFMStationView :
	public SearchableTableView
{
public:
	SomaFMStationView(QWidget* parent=nullptr);
	~SomaFMStationView();

	// SayonaraSelectionView interface
public:
	int get_index_by_model_index(const QModelIndex& idx) const override;
	QModelIndex get_model_index_by_index(int idx) const override;
};

#endif // SOMAFMSTATIONVIEW_H
