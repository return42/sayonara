/* GUI_MTP.h */

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

#ifndef GUI_MTP_H
#define GUI_MTP_H

#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"
#include "Components/MTP/MTP_Typedefs.h"
#include "Helper/Pimpl.h"

class QTreeWidgetItem;

namespace Ui { class GUI_MTP; }

class MTP_CopyFiles;
class MTP;
class GUI_MTP :
		public SayonaraDialog
{
	Q_OBJECT

public:
	explicit GUI_MTP(QWidget* parent=nullptr);
	virtual ~GUI_MTP();

	void device_opened(MTP_DevicePtr device);

private slots:
	void scan_thread_finished();

	void device_idx_changed(int idx);
	void storage_idx_changed(int idx);
	void folder_idx_changed(QTreeWidgetItem* item, int column);
	void folder_idx_expanded(QTreeWidgetItem* item);

	void refresh_clicked();
	void delete_clicked();

	void progress_changed(int progress);
	void copy_thread_finished();

private:
	PIMPL(GUI_MTP)

	Ui::GUI_MTP*				ui=nullptr;


private:
	void enable_drag_drop(bool b);

	void dragEnterEvent(QDragEnterEvent* e) override;
	void dragMoveEvent(QDragMoveEvent* e) override;
	void dragLeaveEvent(QDragLeaveEvent* e) override;
	void dropEvent(QDropEvent* e) override;

	void showEvent(QShowEvent* e) override;
};

#endif // GUI_MTP_H
