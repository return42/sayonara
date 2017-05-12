#include "LibraryListModel.h"
#include "Components/Library/LibraryManager.h"
#include "Helper/Library/LibraryInfo.h"
#include "Interfaces/LibraryInterface/LibraryPluginHandler.h"
#include "Helper/globals.h"

#include <QList>
#include <QPair>

typedef QPair<int, int> MoveOperation;
struct LibraryListModel::Private
{
	LibraryManager* library_manager=nullptr;
	QList<LibraryInfo> library_info;
	QList<LibraryInfo> shown_library_info;
	QList<MoveOperation> move_operations;

	QMap<LibName, LibPath> new_lib_map;
	QMap<qint8, LibName> renamed_lib_map;

	QList<qint8> delete_libs;
	QMap<LibName, qint8> delete_lib_map;

	Private()
	{
		library_manager = LibraryManager::getInstance();
		refresh_info();
	}

	void refresh_info()
	{
		library_info = library_manager->get_all_libraries();
		shown_library_info = library_info;
	}
};

LibraryListModel::LibraryListModel(QObject* parent) :
	QAbstractListModel(parent)
{
	_m = Pimpl::make<Private>();
}

LibraryListModel::~LibraryListModel() {}

int LibraryListModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return _m->shown_library_info.size();
}

QVariant LibraryListModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();
	if(row < 0 || row >= rowCount()){
		return QVariant();
	}

	if(role == Qt::DisplayRole)
	{
		return _m->shown_library_info[row].name();
	}

	else if(role == Qt::ToolTipRole)
	{
		return _m->shown_library_info[row].path();
	}

	return QVariant();
}

void LibraryListModel::append_row(const LibName& name, const LibPath& path)
{
	_m->new_lib_map[name] = path;
	_m->shown_library_info << LibraryInfo(name, path, -1);

	emit dataChanged(index(0), index(rowCount()));

}

void LibraryListModel::rename_row(int row, const LibName& new_name)
{
	if(!between(row, _m->shown_library_info)) {
		return;
	}

	LibraryInfo info = _m->shown_library_info[row];

	if(_m->new_lib_map.contains(info.name())){
		_m->new_lib_map[new_name] = _m->new_lib_map.take(info.name());
	}

	else {
		_m->shown_library_info[row] =
				LibraryInfo(new_name, info.path(), info.id());

		_m->renamed_lib_map[info.id()] = new_name;
	}
}

void LibraryListModel::move_row(int row_idx, int new_idx)
{
	_m->shown_library_info.move(row_idx, new_idx);
	_m->move_operations << MoveOperation(row_idx, new_idx);

	emit dataChanged(index(0), index(rowCount()));
}

void LibraryListModel::remove_row(int row_idx)
{
	if(!between(row_idx, _m->shown_library_info)) {
		return;
	}

	LibraryInfo info = _m->shown_library_info[row_idx];

	if(_m->new_lib_map.contains(info.name())){
		_m->new_lib_map.remove(info.name());
	}

	else {
		_m->delete_libs << info.id();
		_m->delete_lib_map[info.name()] = info.id();
	}

	_m->shown_library_info.removeAt(row_idx);

	emit dataChanged(index(0), index(rowCount()));
}

QStringList LibraryListModel::get_all_names() const
{
	QStringList ret;

	for(const LibraryInfo& info : _m->shown_library_info){
		ret << info.name();
	}

	return ret;
}


QStringList LibraryListModel::get_all_paths() const
{
	QStringList ret;

	for(const LibraryInfo& info : _m->shown_library_info){
		ret << info.path();
	}

	return ret;
}


void LibraryListModel::reset()
{
	_m->shown_library_info = _m->library_info;
	_m->new_lib_map.clear();
	_m->renamed_lib_map.clear();
	_m->delete_lib_map.clear();
	_m->delete_libs.clear();
	_m->move_operations.clear();

	emit dataChanged(index(0), index(rowCount()));
}

void LibraryListModel::commit()
{
	for(const LibName& name : _m->new_lib_map.keys()){
		QString path =  _m->new_lib_map[name];
		_m->library_manager->add_library(name, path);
	}

	for(const LibName& name : _m->delete_lib_map.keys()){
		qint8 library_id = _m->delete_lib_map[name];
		_m->library_manager->remove_library(library_id);
	}

	for(qint8 library_id : _m->renamed_lib_map.keys())
	{
		QString name = _m->renamed_lib_map[library_id];
		_m->library_manager->rename_library(library_id, name);
	}

	for(const MoveOperation& op : _m->move_operations){
		_m->library_manager->move_library(op.first, op.second);
	}

	_m->new_lib_map.clear();
	_m->renamed_lib_map.clear();
	_m->delete_lib_map.clear();
	_m->delete_libs.clear();

	_m->refresh_info();
}

