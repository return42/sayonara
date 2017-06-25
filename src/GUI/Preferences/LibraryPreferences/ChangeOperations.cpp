#include "ChangeOperations.h"
#include "Components/Library/LibraryManager.h"


ChangeOperation::ChangeOperation()
{
	_library_manager = LibraryManager::getInstance();
}

ChangeOperation::~ChangeOperation() {}


MoveOperation::MoveOperation(int from, int to) :
	_from(from), _to(to)
{}

void MoveOperation::exec()
{
	_library_manager->move_library(_from, _to);
}

RenameOperation::RenameOperation(qint8 id, const QString& new_name) :
	_id(id),
	_new_name(new_name)
{}

void RenameOperation::exec()
{
	_library_manager->rename_library(_id, _new_name);
}

RemoveOperation::RemoveOperation(qint8 id) :
	_id(id)
{}

void RemoveOperation::exec()
{
	_library_manager->remove_library(_id);
}

AddOperation::AddOperation(const QString& name, const QString& path) :
	_name(name), _path(path)
{}


void AddOperation::exec()
{
	_library_manager->add_library(_name, _path);
}

ChangePathOperation::ChangePathOperation(qint8 id, const QString& new_path) :
	_id(id),
	_new_path(new_path)
{}

void ChangePathOperation::exec()
{
	_library_manager->change_library_path(_id, _new_path);
}
