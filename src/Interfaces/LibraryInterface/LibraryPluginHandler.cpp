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
#include "LibraryPluginCombobox.h"

#include "GUI/Helper/GUI_Helper.h"
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
#include <QLayout>
#include <QPluginLoader>
#include <QFrame>
#include <QVBoxLayout>
#include <QFontMetrics>
#include <QSize>

struct LibraryPluginHandler::Private
{
	LibraryContainerInterface*			current_library=nullptr;

	LibraryContainerInterface*			empty_library=nullptr;
	QList<LocalLibraryContainer*>		local_libraries;
	QList<LibraryContainerInterface*>	library_containers;
	QList<LibraryContainerInterface*>	dll_libraries;

	QWidget*							library_parent=nullptr;
	LibraryPluginHandler*				plugin_handler=nullptr;

	Private(LibraryPluginHandler* plugin_handler) :
		plugin_handler(plugin_handler)
	{}

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

			local_libraries << new LocalLibraryContainer(library_info);
		}

		if(local_libraries.isEmpty()){
			empty_library = new EmptyLibraryContainer();
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

			sp_log(Log::Info, plugin_handler) << "Found library plugin " << container->display_name();
			dll_libraries << container;
		}
	}

	void insert_containers(const QList<LibraryContainerInterface*>& containers)
	{
		for(LibraryContainerInterface* container : containers)
		{
			if(!container){
				continue;
			}

			sp_log(Log::Debug, plugin_handler) << "Add plugin " << container->display_name();
			library_containers << container;
		}
	}

	QList<LibraryContainerInterface*> all_libraries() const
	{
		QList<LibraryContainerInterface*> container;
		if(empty_library) {
			container << empty_library;
		}

		for(LocalLibraryContainer* llc : local_libraries){
			container << static_cast<LibraryContainerInterface*>(llc);
		}

		container << library_containers;
		container << dll_libraries;

		return container;
	}
};


LibraryPluginHandler::LibraryPluginHandler() :
	QObject(nullptr),
	SayonaraClass()
{
	_m = Pimpl::make<Private>(this);
}

LibraryPluginHandler::~LibraryPluginHandler() {}

void LibraryPluginHandler::init(const QList<LibraryContainerInterface*>& containers)
{
	QString cur_plugin = _settings->get(Set::Lib_CurPlugin);

	_m->insert_local_libraries();
	_m->insert_containers(containers);
	_m->insert_dll_libraries();

	bool found = false;

	for(LibraryContainerInterface* container : _m->all_libraries())
	{
		QString name = container->name();
		if(name.compare(cur_plugin) == 0){
			set_current_library(container);
			found = true;
			break;
		}
	}

	if(!found){
		set_current_library(_m->all_libraries().first());
	}

	emit sig_initialized();
}


void LibraryPluginHandler::init_library(LibraryContainerInterface* library)
{
	if(library->is_initialized()){
		return;
	}

	library->init_ui();
	library->set_initialized();

	QWidget* ui = library->widget();
	ui->setParent(_m->library_parent);

	QLayout* layout = ui->layout();
	if(layout){
		layout->setContentsMargins(5, 0, 8, 0);
	}

	QFrame* header_frame = library->header();
	if(header_frame)
	{
		LibraryPluginCombobox* combo_box = new LibraryPluginCombobox(library->display_name(), nullptr);
		combo_box->setIconSize(QSize(16, 16));

		QLayout* layout = new QVBoxLayout(header_frame);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(combo_box);

		header_frame->setFrameShape(QFrame::NoFrame);
		header_frame->setLayout(layout);

		connect(combo_box, SIGNAL(activated(int)),
				this, SLOT(current_library_changed(int)));
	}
}


void LibraryPluginHandler::set_library_parent(QWidget* parent)
{
	_m->library_parent = parent;

	for(LibraryContainerInterface* container : _m->all_libraries()){
		if(container->is_initialized()){
			container->widget()->setParent(parent);
		}
	}
}


void LibraryPluginHandler::current_library_changed(int library_idx)
{
	Q_UNUSED(library_idx)

	LibraryPluginCombobox* combo_box = static_cast<LibraryPluginCombobox*>(sender());
	if(!combo_box){
		return;
	}

	QString name = combo_box->currentData().toString();
	set_current_library(name);
}

void LibraryPluginHandler::set_current_library(const QString& name)
{
	for(LibraryContainerInterface* container : _m->all_libraries())
	{
		QString container_name = container->name();
		if(container_name.compare(name) != 0)
		{
			if(container->is_initialized()){
				container->widget()->setVisible(false);
			}

			continue;
		}

		_m->current_library = container;
		init_library(container);

		QWidget* widget = container->widget();
		QWidget* parent = widget->parentWidget();

		widget->resize(parent->size());
		widget->setVisible(true);
		widget->update();
	}

	_settings->set(Set::Lib_CurPlugin, name);

	emit sig_current_library_changed(name);
}

void LibraryPluginHandler::set_current_library(LibraryContainerInterface* container)
{
	if(!container){
		return;
	}

	set_current_library(container->name());
}


void LibraryPluginHandler::add_local_library(const LibraryInfo& library)
{
	LocalLibraryContainer* llc = new LocalLibraryContainer(library);

	int idx = 0;
	int elc_idx = -1;

	_m->local_libraries << llc;
	bool empty_library_found = (_m->empty_library != nullptr);

	if(empty_library_found)
	{
		if(_m->empty_library->is_initialized()){
			_m->empty_library->widget()->setVisible(false);
		}
		_m->empty_library->deleteLater();
		_m->empty_library = nullptr;
	}

	emit sig_libraries_changed();

	if(empty_library_found) {
		set_current_library(llc);
	}
}

void LibraryPluginHandler::rename_local_library(qint8 library_id, const QString& new_name)
{
	for(LocalLibraryContainer* llc : _m->local_libraries)
	{
		if(llc->id() == library_id) {
			llc->set_name(new_name);
			break;
		}
	}

	emit sig_libraries_changed();
}

#include <QMenu>
void LibraryPluginHandler::remove_local_library(qint8 library_id)
{
	int idx = -1;
	int i=0;

	LocalLibraryContainer* removed_llc=nullptr;
	for(LocalLibraryContainer* llc : _m->local_libraries)
	{
		if(llc->id() == library_id)
		{
			idx = i;
			removed_llc = llc;

			if(llc->is_initialized()){
				llc->widget()->setVisible(false);
				QMenu* menu = llc->menu();
				if(menu){
					menu->menuAction()->setVisible(false);
				}
			}

			break;
		}

		i++;
	}

	if(idx < 0) {
		return;
	}

	_m->local_libraries.removeAt(idx);
	if(_m->local_libraries.isEmpty()){
		_m->empty_library = new EmptyLibraryContainer();
	}

	emit sig_libraries_changed();

	if(_m->current_library == removed_llc){
		set_current_library(_m->empty_library);
	}
}

void LibraryPluginHandler::move_local_library(int old_row, int new_row)
{
	_m->local_libraries.move(old_row, new_row);

	emit sig_libraries_changed();
}

LibraryContainerInterface*LibraryPluginHandler::current_library() const
{
	return _m->current_library;
}

QMenu* LibraryPluginHandler::current_library_menu() const
{
	if(!_m->current_library){
		return nullptr;
	}

	return _m->current_library->menu();
}


QList<LibraryContainerInterface*> LibraryPluginHandler::get_libraries() const
{
	return _m->all_libraries();
}

