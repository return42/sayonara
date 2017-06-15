/* GUI_MTP.cpp */

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

#include "GUI_MTP.h"
#include "GUI/MTP/ui_GUI_MTP.h"
#include "Components/MTP/MTP.h"
#include "Components/MTP/MTP_CopyFiles.h"

#include "GUI/Helper/Delegates/StyledItemDelegate.h"
#include "GUI/Helper/GUI_Helper.h"
#include "GUI/Helper/CustomMimeData.h"

#include "Helper/Message/Message.h"
#include "Helper/globals.h"
#include "Helper/Logger/Logger.h"

#include <thread>
#include <functional>

#include <QTreeWidgetItem>
#include <QList>
#include <QHash>
#include <QDropEvent>

static void open_device(GUI_MTP* mtp, MTP_RawDevicePtr raw_device)
{
	MTP_DevicePtr device = raw_device->open();
	mtp->device_opened(device);
}

struct GUI_MTP::Private
{
	bool						initialized;

	MTP*						mtp=nullptr;
	MTP_CopyFiles*				mtp_copy_files=nullptr;

	QList<MTP_RawDevicePtr>		raw_devices;
	QList<MTP_DevicePtr>		open_devices;
	QList<MTP_StoragePtr>		storages;

	QHash<quint32, MTP_FolderPtr> folders;
	QHash<quint32, MTP_FilePtr>	  files;

	Private()
	{
		initialized = false;
	}
};


GUI_MTP::GUI_MTP(QWidget* parent) :
	SayonaraDialog(parent)
{
	_m = Pimpl::make<Private>();
}


GUI_MTP::~GUI_MTP()
{
	if(_m->open_devices.size() > 0){
		sp_log(Log::Debug, this) << "Devices open: " << _m->open_devices.first().use_count();
	}

	if(_m->raw_devices.size() > 0){
		sp_log(Log::Debug, this) << "Raw Devices open: " << _m->raw_devices.first().use_count();
	}

	if(_m->mtp_copy_files){
		delete _m->mtp_copy_files;
	}

	_m->files.clear();
	_m->folders.clear();
	_m->storages.clear();
	_m->open_devices.clear();
	_m->raw_devices.clear();
}


void GUI_MTP::refresh_clicked()
{
	_m->raw_devices.clear();
	_m->open_devices.clear();
	_m->storages.clear();
	_m->folders.clear();
	_m->files.clear();

	if(_m->mtp == nullptr){
		_m->mtp = new MTP(this);

		connect(_m->mtp, &MTP::finished, this, &GUI_MTP::scan_thread_finished);
	}

	ui->btn_go->setEnabled(false);
	ui->btn_go->setText("Initializing...");
	_m->mtp->start();
}

void GUI_MTP::delete_clicked()
{
	for(QTreeWidgetItem* item : ui->tree_view->selectedItems())
	{
		quint32 id = item->data(1, 0).toInt();
		if(id == 0){
			continue;
		}

		if(!_m->files.contains(id)){
			continue;
		}

		MTP_FilePtr file = _m->files[id];
		file->remove();
		_m->files.remove(id);

		item->setDisabled(true);
		item->setSelected(false);

		int cur_storage_idx = ui->combo_storages->currentIndex();
		_m->storages[cur_storage_idx]->remove_id(id);
		folder_idx_changed(item->parent(), 0);
		ui->tree_view->removeItemWidget(item, 0);
	}

	for(QTreeWidgetItem* item : ui->tree_view->selectedItems())
	{
		quint32 id = item->data(1, 0).toInt();
		if(id == 0){
			continue;
		}

		if(!_m->folders.contains(id)){
			continue;
		}

		MTP_FolderPtr folder = _m->folders[id];
		folder->remove();
		_m->folders.remove(id);

		item->setDisabled(true);
		item->setSelected(false);
		int cur_storage_idx = ui->combo_storages->currentIndex();
		_m->storages[cur_storage_idx]->remove_id(id);
		folder_idx_changed(item->parent(), 0);
		ui->tree_view->removeItemWidget(item, 0);

	}
}


void GUI_MTP::scan_thread_finished()
{
	ui->combo_devices->clear();

	sp_log(Log::Debug, this) << "Scan thread finished";

	ui->btn_go->setEnabled(true);
	ui->btn_go->setText("Refresh");

	QList<MTP_RawDevicePtr> raw_devices = _m->mtp->get_raw_devices();
	if(raw_devices.size() == 0){
		Message::warning("No devices found");
		return;
	}

	for(MTP_RawDevicePtr raw_device : raw_devices){
		QString device_string = raw_device->get_device_string();

		_m->raw_devices << raw_device;
		ui->combo_devices->addItem(device_string);
	}
}


void GUI_MTP::device_idx_changed(int idx)
{
	_m->storages.clear();
	ui->combo_storages->clear();

	if( !between(idx, _m->raw_devices) ){
		return;
	}

	MTP_RawDevicePtr raw_device = _m->raw_devices[idx];


	std::thread* open_thread = new std::thread(open_device, this, raw_device);
	Q_UNUSED(open_thread);

	ui->btn_go->setText(tr("Opening") + "...");
	ui->btn_go->setDisabled(true);
}


void GUI_MTP::device_opened(MTP_DevicePtr device)
{
	ui->btn_go->setText(tr("Refresh"));
	ui->btn_go->setDisabled(false);

	if(!device)
	{
		//Message::warning("Cannot open device");
		return;
	}

	_m->open_devices << device;

	QList<MTP_StoragePtr> storages = device->storages();
	sp_log(Log::Debug, this) << "Device " << device->id() << " has " << storages.size() << " storages";

	for(MTP_StoragePtr storage : storages){
		QString name = storage->name() + ": " + storage->identifier();
		sp_log(Log::Debug, this) << "New Storage: " << name;
		_m->storages << storage;
		ui->combo_storages->addItem(name);
	}

	if(ui->combo_storages->currentIndex() == -1 && ui->combo_storages->count() > 0){
		ui->combo_storages->setCurrentIndex(0);
	}
}

void GUI_MTP::storage_idx_changed(int idx)
{
	if( !between(idx, _m->storages) ){
		return;
	}

	MTP_StoragePtr storage = _m->storages[idx];
	ui->tree_view->clear();

	QList<MTP_FolderPtr> folders = storage->folders();
	sp_log(Log::Debug, this) << "Storage has " << folders.size() << " folders";
	enable_drag_drop(folders.size() > 0);

	for(MTP_FolderPtr folder : folders)
	{
		_m->folders[folder->id()] = folder;
		QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << folder->name());

		item->setData(1, 0, folder->id());
		item->setText(0, folder->name());
		item->addChild(new QTreeWidgetItem());

		ui->tree_view->addTopLevelItem(item);
	}
}

void GUI_MTP::folder_idx_expanded(QTreeWidgetItem *item)
{
	Q_UNUSED(item)
}

void GUI_MTP::folder_idx_changed(QTreeWidgetItem* item, int column)
{
	Q_UNUSED(column)
	ui->btn_delete->setEnabled(true);

	if(!item){
		return;
	}

	quint32 folder_id = item->data(1, 0).toInt();
	if(folder_id == 0){
		return;
	}

	MTP_FolderPtr folder = _m->folders[folder_id];
	QList<MTP_FolderPtr> children = folder->children();

	for(int i = item->childCount() - 1; i>=0; i--){
		item->takeChild(i);
	}

	for(MTP_FolderPtr folder : children){
		_m->folders[folder->id()] = folder;

		QTreeWidgetItem* child_item;

		child_item = new QTreeWidgetItem();

		child_item->setData(1, 0, folder->id());
		child_item->setText(0, folder->name());
		child_item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
		child_item->addChild(new QTreeWidgetItem());

		item->addChild(child_item);
	}

	MTP_StoragePtr storage = _m->storages[ui->combo_storages->currentIndex()];
	QList<MTP_FilePtr> files = storage->files_of_folder(folder->id());

	for(MTP_FilePtr file : files)
	{
		_m->files[file->id()] = file;

		QTreeWidgetItem* child_item = new QTreeWidgetItem();

		child_item->setText(0, file->filename());
		child_item->setData(1, 0, file->id());
		child_item->setIcon(0, GUI::get_icon("undo"));
		child_item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);

		item->addChild(child_item);
	}
}

void GUI_MTP::enable_drag_drop(bool b)
{
	ui->tree_view->setAcceptDrops(b);
	ui->tree_view->setDragEnabled(b);
	this->setAcceptDrops(b);
}


void GUI_MTP::dragEnterEvent(QDragEnterEvent* e)
{
	e->accept();
}


void GUI_MTP::dragMoveEvent(QDragMoveEvent* e)
{
	e->accept();

	QPoint pos = e->pos();
	ui->tree_view->clearSelection();

	QTreeWidgetItem* item = ui->tree_view->itemAt(pos - ui->tree_view->pos());

	if(!item){
		return;
	}

	item->setSelected(true);
}

void GUI_MTP::dragLeaveEvent(QDragLeaveEvent* e)
{
	e->accept();
}

void GUI_MTP::dropEvent(QDropEvent* e)
{
	e->accept();
	quint32 folder_id;
	MetaDataList v_md;
	QTreeWidgetItem* item;
	MTP_FolderPtr folder;
	const QMimeData* mime_data;
	const CustomMimeData* cmd;

	QPoint pos;

	pos = e->pos();
	item = ui->tree_view->itemAt(pos - ui->tree_view->pos());

	if(!item){
		return;
	}

	folder_id = item->data(1, 0).toInt();
	if(folder_id == 0){
		return;
	}

	folder = _m->folders[folder_id];
	sp_log(Log::Debug, this) << "Will drop into folder " << folder->name();

	mime_data = e->mimeData();
	if(!mime_data){
		sp_log(Log::Debug, this) << "Cannot drop metadata";
		return;
	}

	cmd = static_cast<const CustomMimeData*>(mime_data);

	if(cmd)
	{
		if(_m->mtp_copy_files){
			delete _m->mtp_copy_files;
			_m->mtp_copy_files = nullptr;
		}

		enable_drag_drop(false);

		v_md = cmd->getMetaData();

		if(v_md.isEmpty() && cmd->hasUrls())
		{
			QList<QUrl> urls = mime_data->urls();
			QStringList str_urls;

			for(const QUrl& url : urls){
				str_urls << url.toLocalFile();
			}

			_m->mtp_copy_files = new MTP_CopyFiles(str_urls, folder, nullptr);
			sp_log(Log::Debug, this) << "Will drop " << str_urls.size() << " files ";
		}

		else{
			_m->mtp_copy_files = new MTP_CopyFiles(v_md, folder, nullptr);
			sp_log(Log::Debug, this) << "Will drop " << v_md.size() << " Tracks ";
		}

		ui->btn_delete->setEnabled(false);
		ui->btn_go->setEnabled(false);

		connect(_m->mtp_copy_files, &MTP_CopyFiles::sig_progress, this, &GUI_MTP::progress_changed);
		connect(_m->mtp_copy_files, &MTP_CopyFiles::finished, this, &GUI_MTP::copy_thread_finished);

		_m->mtp_copy_files->start();
	}
}

void GUI_MTP::showEvent(QShowEvent* e)
{
	if(!_m->initialized)
	{
		ui = new Ui::GUI_MTP();
		ui->setupUi(this);

		ui->tree_view->setExpandsOnDoubleClick(true);
		ui->tree_view->setAnimated(true);
		ui->tree_view->setAutoExpandDelay(100);
		ui->tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
		ui->tree_view->setItemDelegate(new StyledItemDelegate(this));

		ui->pb_progress->hide();

		enable_drag_drop(false);

		_m->mtp = nullptr;
		_m->mtp_copy_files = nullptr;

		connect(ui->tree_view, &QTreeWidget::itemActivated, this, &GUI_MTP::folder_idx_changed);
		connect(ui->tree_view, &QTreeWidget::itemExpanded, this, &GUI_MTP::folder_idx_expanded);
		connect(ui->combo_devices, combo_current_index_changed_int, this, &GUI_MTP::device_idx_changed);
		connect(ui->combo_storages, combo_current_index_changed_int, this, &GUI_MTP::storage_idx_changed);
		connect(ui->btn_go, &QPushButton::clicked, this, &GUI_MTP::refresh_clicked);
		connect(ui->btn_delete, &QPushButton::clicked, this, &GUI_MTP::delete_clicked);

		ui->btn_delete->setEnabled(false);
		_m->initialized = true;
	}

	SayonaraDialog::showEvent(e);
}

void GUI_MTP::progress_changed(int progress)
{
	if(progress == -1){
		ui->pb_progress->hide();
	}

	else{
		ui->pb_progress->show();
		ui->pb_progress->setValue(progress);
	}
}

void GUI_MTP::copy_thread_finished()
{
	ui->pb_progress->hide();
	enable_drag_drop(true);
	ui->btn_delete->setEnabled(true);
	ui->btn_go->setEnabled(true);
}
