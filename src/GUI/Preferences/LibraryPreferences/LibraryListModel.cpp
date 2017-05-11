#include "LibraryListModel.h"
#include "Components/Library/LibraryManager.h"
#include "Helper/Library/LibraryInfo.h"
#include "Interfaces/LibraryInterface/LibraryPluginHandler.h"

#include <QList>

struct LibraryListModel::Private
{
	LibraryManager* library_manager=nullptr;
	QList<LibraryInfo> library_info;

	Private()
	{
		library_manager = LibraryManager::getInstance();
		refresh_info();
	}

	void refresh_info()
	{
		library_info = library_manager->get_all_libraries();
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
	return _m->library_info.size();
}

QVariant LibraryListModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();
	if(row < 0 || row >= rowCount()){
		return QVariant();
	}

	if(role == Qt::DisplayRole)
	{
		return _m->library_info[row].name();
	}

	else if(role == Qt::ToolTipRole)
	{
		return _m->library_info[row].path();
	}

	return QVariant();
}

void LibraryListModel::append_row(const QString& name, const QString& path)
{
	qint8 library_id = _m->library_manager->add_library(name, path);
	if(library_id >= 0){
		_m->refresh_info();

		emit dataChanged(index(0), index(rowCount()));
	}
}

void LibraryListModel::move_row(int row_idx, int new_idx)
{
	if(row_idx >= 0 && row_idx < _m->library_info.size() &&
		new_idx >= 0 && new_idx <= _m->library_info.size())
	{
		//_m->library_info.move(row_idx, new_idx);
	}

	emit dataChanged(index(0), index(rowCount()));
}

void LibraryListModel::remove_row(int row_idx)
{
	if(row_idx >= 0 && row_idx < _m->library_info.size()){
		qint8 library_id = _m->library_info[row_idx].id();
		_m->library_manager->remove_library(library_id);
		_m->refresh_info();
	}

	emit dataChanged(index(0), index(rowCount()));
}

void LibraryListModel::reset()
{
	_m->library_manager->revert();
	_m->refresh_info();
	emit dataChanged(index(0), index(rowCount()));
}
