#include "SomaFMStationView.h"

SomaFMStationView::SomaFMStationView(QWidget* parent) :
    SearchableTableView(parent)
{

}

SomaFMStationView::~SomaFMStationView() {}


int SomaFMStationView::get_index_by_model_index(const QModelIndex& idx) const
{
    return idx.row();
}

QModelIndex SomaFMStationView::get_model_index_by_index(int idx) const
{
    return this->model()->index(idx, 0);
}
