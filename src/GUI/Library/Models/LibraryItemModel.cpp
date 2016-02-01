/* LibraryItemModel.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "LibraryItemModel.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QModelIndex>


LibraryItemModel::LibraryItemModel(QList<ColumnHeader>& headers) {

		_n_all_cols = headers.size();
		_n_rows = 0;

		_cols_active = QVector<bool>(_n_all_cols);

		int i=0;
		for(const ColumnHeader& h : headers) {
			_header_names.push_back(h.getTitle());
			_cols_active[i] = true;
		}
}

LibraryItemModel::~LibraryItemModel() {

}

void LibraryItemModel::set_new_header_names(QStringList& lst) {
    _header_names = lst;
}


QVariant LibraryItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const{

	 if (role != Qt::DisplayRole)
			 return QVariant();

	 int idx_col = calc_shown_col(section);
	 if(idx_col >= _header_names.size()){
		 return QVariant();
	 }

	 if (orientation == Qt::Horizontal){
		 return _header_names[idx_col];
	 }
	 return QVariant();

}

int LibraryItemModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return _n_rows;
}


QStringList LibraryItemModel::get_header_names() const {
	return _header_names;
}


int LibraryItemModel::get_n_cols() const {
	return _n_all_cols;
}

// input: seen column
// returns: real index of column
int LibraryItemModel::calc_shown_col(int col) const {

	int idx_col = 0;
	int n_active = -1;

	for(idx_col=0; idx_col<_n_all_cols; idx_col++) {

		if(_cols_active[idx_col]) {
			n_active++;
		}

		if(n_active == col) break;
	}

	return idx_col;
}

bool LibraryItemModel::is_col_shown(int col) const{
	return _cols_active[col];
}


int LibraryItemModel::columnCount(const QModelIndex& parent) const{

	Q_UNUSED(parent);

	int n_active = 0;

	for(int i=0; i<_n_all_cols; i++) {
		if(_cols_active[i]) {
			n_active++;
		}
	}

	return n_active;
}


bool LibraryItemModel::insertColumns(int position, int cols, const QModelIndex &index) {
	Q_UNUSED(index)

    beginInsertColumns(QModelIndex(), position, position+cols-1);

	for(int i=position; i<position+cols; i++) {

		_cols_active[i] = true;
	}

    endInsertColumns();
	return true;
}


bool LibraryItemModel::removeColumns(int position, int cols, const QModelIndex &index) {
	Q_UNUSED(index)

    beginRemoveColumns(QModelIndex(), position, position+cols-1);
	for(int i=0; i<_n_all_cols; i++) {
        _cols_active[i] = false;
    }

    endRemoveColumns();
    return true;
}

bool LibraryItemModel::removeRows(int row, int count, const QModelIndex& index){
	Q_UNUSED(index)

	beginRemoveRows(QModelIndex(), row, row + count - 1);
	_n_rows -= count;
	for(int i=row; i<row + count; i++){
		_selections.removeOne( get_id_by_row(i) );
	}

	endRemoveRows();

	return true;
}

bool LibraryItemModel::insertRows(int row, int count, const QModelIndex& index)
{
	Q_UNUSED(index)
	beginInsertRows(QModelIndex(), row, row + count - 1);
	_n_rows += count;
	endInsertRows();

	return true;
}


QMap<QChar, QString> LibraryItemModel::getExtraTriggers() {
	QMap<QChar, QString> map;
	return map;
}

void LibraryItemModel::set_mimedata(const MetaDataList& v_md){
	_md_mimedata = v_md;
}

CustomMimeData* LibraryItemModel::get_mimedata(){
	CustomMimeData* mimedata = new CustomMimeData();
	QList<QUrl> urls;

	for(const MetaData& md : _md_mimedata){
		QUrl url(QString("file://") + md.filepath());
		urls << url;
	}

	mimedata->setMetaData(_md_mimedata);
	mimedata->setText("tracks");
	mimedata->setUrls(urls);

	return mimedata;
}


bool LibraryItemModel::has_selections(){
	return !_selections.isEmpty();
}


void LibraryItemModel::add_selections(IdxList rows){
	for(int row : rows){
		_selections << get_id_by_row(row);
	}
	std::sort(_selections.begin(), _selections.end());
}

void LibraryItemModel::add_selection(int row){
	_selections << get_id_by_row(row);
	std::sort(_selections.begin(), _selections.end());
}

bool LibraryItemModel::is_selected(int id)
{
	return _selections.contains(id);
}


void LibraryItemModel::remove_selection(int id){

	auto lambda = [id](int tmp_id){
		return tmp_id == id;
	};
	auto it=std::remove_if(_selections.begin(), _selections.end(), lambda);
	_selections.erase(it);
}

void LibraryItemModel::clear_selections(){
	_selections.clear();
}
