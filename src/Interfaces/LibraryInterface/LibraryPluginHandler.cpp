/* LibraryPluginHandler.cpp */

/* Copyright (C) 2011-2017  Lucio Carreras
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

#include "LibraryPluginHandler.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidgetTemplate.h"
#include "LibraryContainer/LibraryContainer.h"
#include "Components/Library/LibraryManager.h"
#include "Helper/Library/LibraryInfo.h"
#include "GUI/Library/LocalLibraryContainer.h"
#include "GUI/Library/EmptyLibraryContainer.h"

#include "Helper/globals.h"
#include "Helper/Helper.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Logger/Logger.h"

#include <QDir>
#include <QIcon>
#include <QPair>
#include <QLayout>
#include <QComboBox>
#include <QPluginLoader>

struct LibraryPluginHandler::Private
{
	QList<LibraryContainerInterface*>	libraries;
	QList<QPair<QString, QIcon>>		library_entries;
	int									cur_idx;
	QWidget*							library_parent=nullptr;
	LibraryPluginHandler*				plugin_handler=nullptr;

	Private(LibraryPluginHandler* plugin_handler) :
		cur_idx(-1),
		plugin_handler(plugin_handler) {}


	void insert_local_libraries()
	{
		QList<LibraryInfo> library_infos = LibraryManager::getInstance()->get_all_libraries();
		for(const LibraryInfo& library_info : library_infos)
		{
			if(library_info.id() < 0){
				continue;
			}

			sp_log(Log::Debug, plugin_handler) << "Add local library "
									 << library_info.name() << ": "
									 << library_info.path();

			libraries << new LocalLibraryContainer(library_info);
		}

		if(libraries.isEmpty()){
			libraries << new EmptyLibraryContainer();
		}
	}

	void insert_dll_libraries()
	{
		QDir plugin_dir = QDir(Helper::lib_path());
		QStringList dll_filenames = plugin_dir.entryList(QDir::Files);

		for(const QString& filename : dll_filenames)
		{
			QObject* raw_plugin;
			LibraryContainerInterface* container;

			QPluginLoader loader(plugin_dir.absoluteFilePath(filename));

			raw_plugin = loader.instance();
			if(!raw_plugin) {
				sp_log(Log::Warning, plugin_handler) << "Cannot load plugin: " << filename << ": " << loader.errorString();
				loader.unload();
				continue;
			}

			container = dynamic_cast<LibraryContainerInterface*>(raw_plugin);
			if(!container) {
				loader.unload();
				continue;
			}

			sp_log(Log::Info, plugin_handler) << "Found library plugin " << container->get_display_name();
			libraries << container;
		}
	}

	void insert_containers(const QList<LibraryContainerInterface*>& containers)
	{
		for(LibraryContainerInterface* container : containers)
		{
			if(!container){
				continue;
			}

			sp_log(Log::Debug, plugin_handler) << "Add plugin " << container->get_display_name();
			libraries << container;
		}
	}
};

LibraryPluginHandler::LibraryPluginHandler() :
	QObject(nullptr),
	SayonaraClass()
{
	_m = Pimpl::make<Private>(this);

	REGISTER_LISTENER(Set::Player_Language, language_changed);
}

LibraryPluginHandler::~LibraryPluginHandler() {}

void LibraryPluginHandler::init(const QList<LibraryContainerInterface*>& containers)
{
	QString cur_plugin = _settings->get(Set::Lib_CurPlugin);

	_m->insert_local_libraries();
	_m->insert_containers(containers);
	_m->insert_dll_libraries();

	sp_log(Log::Info, this) << "Found " << _m->libraries.size() << " library types";

	int i=0;
	bool found = false;
	for(LibraryContainerInterface* container : _m->libraries )
	{
		if(cur_plugin.compare(container->get_name()) == 0){
			_m->cur_idx = i;
			init_library(i);
			emit sig_idx_changed(i);
			found = true;
			break;
		}

		i++;
	}

	if(!found)
	{
		_m->cur_idx = 0;
		init_library(0);
		emit sig_idx_changed(0);
	}
}


void LibraryPluginHandler::init_library(int idx)
{
	LibraryContainerInterface* library = _m->libraries[idx];
	if(library->is_initialized()){
		return;
	}

	library->init_ui();
	library->set_initialized();
	QWidget* ui = library->get_ui();
	ui->setParent(_m->library_parent);

	QLayout* layout = ui->layout();
	if(layout){
		layout->setContentsMargins(5, 0, 8, 0);
	}

	QComboBox* libchooser = library->get_libchooser();
	libchooser->setIconSize(QSize(16,16));
	libchooser->setMinimumWidth(200);
	libchooser->setMaximumWidth(200);
	libchooser->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	for(LibraryContainerInterface* other_library : _m->libraries){
		libchooser->addItem(other_library->get_icon(), other_library->get_display_name());
	}

	libchooser->setCurrentIndex(idx);
	libchooser->setItemDelegate(new ComboBoxDelegate(ui));

	connect(libchooser, combo_activated_int, this, &LibraryPluginHandler::index_changed);
}


void LibraryPluginHandler::index_changed(int idx)
{
	_m->cur_idx = idx;

	init_library(_m->cur_idx);

	int i=0;
	for(LibraryContainerInterface* container : _m->libraries)
	{
		if(!container->is_initialized()) {
			i++;
			continue;
		}
	
		QComboBox* libchooser = container->get_libchooser();
		libchooser->setItemIcon(i, container->get_icon());

		QWidget* ui = container->get_ui();
		QWidget* parent = ui->parentWidget();

		ui->setVisible(i == idx);

		if(i == idx){

			libchooser->setCurrentIndex(i);

			if(parent){
				ui->resize(parent->size());
			}

			_settings->set(Set::Lib_CurPlugin, container->get_name());

			ui->update();
		}

		i++;
	}

	emit sig_idx_changed(idx);
}

LibraryContainerInterface* LibraryPluginHandler::get_cur_library() const
{
	if(!between(_m->cur_idx, _m->libraries)) {
		return nullptr;
	}

	return _m->libraries[_m->cur_idx];
}

int LibraryPluginHandler::get_cur_library_idx() const
{
	return _m->cur_idx;
}



void LibraryPluginHandler::set_library_parent(QWidget* parent)
{
	_m->library_parent = parent;

	for(LibraryContainerInterface* container : _m->libraries){
		if(container->is_initialized()){
			container->get_ui()->setParent(parent);
		}
	}
}

void LibraryPluginHandler::language_changed()
{
	for(LibraryContainerInterface* container : _m->libraries)
	{
		if(!container->is_initialized()){
			sp_log(Log::Debug, this) << container->get_display_name() << " is not initialized ";
			continue;
		}

		QComboBox* libchooser = container->get_libchooser();
		int i=0;

		for(LibraryContainerInterface* container2 : _m->libraries){
			libchooser->setItemText(i, container2->get_display_name());
			i++;
		}
	}
}


void LibraryPluginHandler::add_local_library(const LibraryInfo& library, int idx)
{
	LocalLibraryContainer* llc = new LocalLibraryContainer(library);
	_m->libraries.insert(idx, llc);

	for(LibraryContainerInterface* container : _m->libraries)
	{
		if(!container->is_initialized()){
			continue;
		}

		QComboBox* lib_chooser = container->get_libchooser();
		lib_chooser->insertItem(idx, llc->get_icon(), llc->get_display_name());
	}
}

void LibraryPluginHandler::rename_local_library(qint8 library_id, const QString& new_name)
{
	int idx = -1;
	int i=0;
	for(LibraryContainerInterface* container : _m->libraries)
	{
		LocalLibraryContainer* llc = dynamic_cast<LocalLibraryContainer*>(container);
		if(llc){
			if(llc->get_id() == library_id) {
				llc->set_name(new_name);
				idx = i;
				break;
			}

			i++;
		}
	}

	if(idx < 0) {
		return;
	}

	for(LibraryContainerInterface* container : _m->libraries)
	{
		if(!container->is_initialized()){
			continue;
		}

		QComboBox* lib_chooser = container->get_libchooser();
		lib_chooser->setItemText(idx, new_name);
	}
}

void LibraryPluginHandler::remove_local_library(qint8 library_id)
{
	int idx = -1;
	int i=0;
	for(LibraryContainerInterface* container : _m->libraries)
	{
		LocalLibraryContainer* llc = dynamic_cast<LocalLibraryContainer*>(container);
		if(llc){
			if(llc->get_id() == library_id){
				idx = i;
				break;
			}

			i++;
		}
	}

	if(idx < 0) {
		return;
	}

	if(idx == _m->cur_idx){
		if(idx != 0) {
			this->index_changed(0);
		}

		else {
			this->index_changed(1);
		}
	}

	_m->libraries.removeAt(idx);
	for(LibraryContainerInterface* container : _m->libraries)
	{
		if(!container->is_initialized()){
			continue;
		}

		QComboBox* lib_chooser = container->get_libchooser();
		lib_chooser->removeItem(idx);
	}
}

void LibraryPluginHandler::move_local_library(int old_row, int new_row)
{
	for(LibraryContainerInterface* container : _m->libraries)
	{
		if(!container->is_initialized()){
			continue;
		}

		QComboBox* lib_chooser = container->get_libchooser();

		QIcon icon = lib_chooser->itemIcon(old_row);
		QString text = lib_chooser->itemText(old_row);
		lib_chooser->removeItem(old_row);

		if(new_row > old_row){
			new_row--;
		}

		lib_chooser->insertItem(new_row,
								icon,
								text);
	}

	_m->libraries.move(old_row, new_row);
}

void LibraryPluginHandler::remove_index(int idx)
{
	for(LibraryContainerInterface* container : _m->libraries)
	{
		if(!container->is_initialized()) {
			continue;
		}

		if(idx >= 0 && idx < container->get_libchooser()->count()){
			container->get_libchooser()->removeItem(idx);
		}
	}

	_m->cur_idx = get_cur_library()->get_libchooser()->currentIndex();

}

void LibraryPluginHandler::set_current_index(int idx)
{
	LibraryContainerInterface* container = this->get_cur_library();

	if(container->is_initialized()){
		container->get_libchooser()->setCurrentIndex(idx);
	}

	index_changed(idx);
}


QList<LibraryContainerInterface*> LibraryPluginHandler::get_libraries() const
{
	return _m->libraries;
}

