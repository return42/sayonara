/* GUI_LibraryPreferences.cpp */

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

#include "GUI_LibraryPreferences.h"
#include "GUI/Preferences/ui_GUI_LibraryPreferences.h"
#include "LibraryListModel.h"

#include "GUI/Helper/Delegates/StyledItemDelegate.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Library/SearchMode.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"
#include "Helper/Logger/Logger.h"

#include <QFileDialog>
#include <QInputDialog>

struct GUI_LibraryPreferences::Private
{
	LibraryListModel* model = nullptr;
};

GUI_LibraryPreferences::GUI_LibraryPreferences(QWidget* parent) :
	PreferenceWidgetInterface(parent)
{
	_m = Pimpl::make<Private>();
}

GUI_LibraryPreferences::~GUI_LibraryPreferences()
{
	if(ui)
	{
		delete ui; ui=nullptr;
	}
}

void GUI_LibraryPreferences::init_ui()
{
	setup_parent(this, &ui);

	_m->model = new LibraryListModel(ui->lv_libs);

	ui->lv_libs->setModel(_m->model);
	ui->lv_libs->setItemDelegate(new StyledItemDelegate(ui->lv_libs));

	connect(ui->btn_new, &QPushButton::clicked, this, &GUI_LibraryPreferences::new_clicked);
	connect(ui->btn_rename, &QPushButton::clicked, this, &GUI_LibraryPreferences::rename_clicked);
	connect(ui->btn_delete, &QPushButton::clicked, this, &GUI_LibraryPreferences::delete_clicked);
	connect(ui->btn_add, &QPushButton::clicked, this, &GUI_LibraryPreferences::add_clicked);
	connect(ui->btn_clear, &QPushButton::clicked, this, &GUI_LibraryPreferences::clear_clicked);

	connect(ui->le_name, &QLineEdit::textChanged, this, &GUI_LibraryPreferences::library_text_changed);
	connect(ui->le_path, &QLineEdit::textChanged, this, &GUI_LibraryPreferences::library_text_changed);

	connect(ui->btn_up, &QPushButton::clicked, this, &GUI_LibraryPreferences::up_clicked);
	connect(ui->btn_down, &QPushButton::clicked, this, &GUI_LibraryPreferences::down_clicked);

	ui->sw_lib_manager->setCurrentIndex(0);

	revert();
}

QString GUI_LibraryPreferences::get_action_name() const
{
	return Lang::get(Lang::Library);
}


void GUI_LibraryPreferences::commit()
{
	Library::SearchModeMask mask = 0;
	if(ui->cb_case_insensitive->isChecked()){
		mask |= Library::CaseInsensitve;
	}

	if(ui->cb_no_special_chars->isChecked()){
		mask |= Library::NoSpecialChars;
	}

	if(ui->cb_no_accents->isChecked()){
		mask |= Library::NoDiacriticChars;
	}

	_settings->set(Set::Lib_DC_DoNothing, ui->rb_dc_do_nothing->isChecked());
	_settings->set(Set::Lib_DC_PlayIfStopped, ui->rb_dc_play_if_stopped->isChecked());
	_settings->set(Set::Lib_DC_PlayImmediately, ui->rb_dc_play_immediately->isChecked());
	_settings->set(Set::Lib_DD_DoNothing, ui->rb_dd_do_nothing->isChecked());
	_settings->set(Set::Lib_DD_PlayIfStoppedAndEmpty, ui->rb_dd_start_if_stopped_and_empty->isChecked());
	_settings->set(Set::Lib_SearchMode, mask);

	_m->model->commit();
}

void GUI_LibraryPreferences::revert()
{
	Library::SearchModeMask mask = _settings->get(Set::Lib_SearchMode);

	ui->cb_case_insensitive->setChecked(mask & Library::CaseInsensitve);
	ui->cb_no_special_chars->setChecked(mask & Library::NoSpecialChars);
	ui->cb_no_accents->setChecked(mask & Library::NoDiacriticChars);

	ui->rb_dc_do_nothing->setChecked(_settings->get(Set::Lib_DC_DoNothing));
	ui->rb_dc_play_if_stopped->setChecked(_settings->get(Set::Lib_DC_PlayIfStopped));
	ui->rb_dc_play_immediately->setChecked(_settings->get(Set::Lib_DC_PlayImmediately));
	ui->rb_dd_do_nothing->setChecked(_settings->get(Set::Lib_DD_DoNothing));
	ui->rb_dd_start_if_stopped_and_empty->setChecked(_settings->get(Set::Lib_DD_PlayIfStoppedAndEmpty));

	_m->model->reset();
}

void GUI_LibraryPreferences::retranslate_ui()
{
	ui->retranslateUi(this);

	ui->le_name->setPlaceholderText(Lang::get(Lang::EnterName));
	ui->btn_new->setText(Lang::get(Lang::New));
	ui->btn_clear->setText(Lang::get(Lang::Clear));
	ui->btn_delete->setText(Lang::get(Lang::Remove));
	ui->btn_rename->setText(Lang::get(Lang::Rename));
}


void GUI_LibraryPreferences::new_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("New Library"), QDir::homePath());

	if(dir.isEmpty()){
		ui->sw_lib_manager->setCurrentIndex(0);
		return;
	}

	QDir d(dir);
	QString dir_proposal = Helper::cvt_str_to_first_upper(d.dirName());

	ui->le_name->setText(dir_proposal);
	ui->le_path->setText(dir);

	ui->btn_new->setEnabled(false);
	ui->btn_delete->setEnabled(false);
	ui->lv_libs->setEnabled(false);

	ui->sw_lib_manager->setCurrentIndex(1);
}

void GUI_LibraryPreferences::rename_clicked()
{
	QModelIndex idx = ui->lv_libs->currentIndex();
	if(!idx.isValid()){
		return;
	}

	QString current_text = _m->model->data(idx, Qt::DisplayRole).toString();

	QString new_name = QInputDialog::getText(this,
						  Lang::get(Lang::EnterName),
						  Lang::get(Lang::EnterName),
						  QLineEdit::Normal,
						  current_text);

	if(new_name.isEmpty() ||
	   new_name.compare(current_text, Qt::CaseInsensitive) == 0)
	{
		return;
	}

	_m->model->rename_row(idx.row(), new_name);
}

void GUI_LibraryPreferences::delete_clicked()
{
	QModelIndex idx = ui->lv_libs->currentIndex();
	if(!idx.isValid()){
		return;
	}

	_m->model->remove_row(idx.row());
}

void GUI_LibraryPreferences::add_clicked()
{
	ui->sw_lib_manager->setCurrentIndex(0);
	_m->model->append_row(ui->le_name->text(), ui->le_path->text());
	clear_clicked();
}

void GUI_LibraryPreferences::clear_clicked()
{
	ui->le_name->clear();
	ui->le_path->clear();

	ui->btn_new->setEnabled(true);
	ui->btn_delete->setEnabled(true);
	ui->lv_libs->setEnabled(true);

	ui->sw_lib_manager->setCurrentIndex(0);
}

void GUI_LibraryPreferences::library_text_changed(const QString& str)
{
	Q_UNUSED(str)

	QStringList names = _m->model->all_names();
	QStringList paths = _m->model->all_paths();

	ui->btn_add->setDisabled(
				(ui->le_name->text().isEmpty()) ||
				(ui->le_path->text().isEmpty()) ||
				(names.contains(str, Qt::CaseInsensitive)) ||
				(paths.contains(str, Qt::CaseInsensitive))
	);
}


void GUI_LibraryPreferences::up_clicked()
{
	int row = ui->lv_libs->currentIndex().row();

	_m->model->move_row(row, row-1);
	ui->lv_libs->setCurrentIndex(_m->model->index(row - 1));
}

void GUI_LibraryPreferences::down_clicked()
{
	int row = ui->lv_libs->currentIndex().row();

	_m->model->move_row(row, row+1);
	ui->lv_libs->setCurrentIndex(_m->model->index(row + 1));
}
