/* GUI_AlternativeCovers.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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


/*
 * GUI_AlternativeCovers.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: Lucio Carreras
 */

#include "GUI_AlternativeCovers.h"
#include "GUI/Covers/ui_GUI_AlternativeCovers.h"
#include "GUI/Helper/SayonaraWidget/SayonaraLoadingBar.h"

#include "AlternativeCoverItemDelegate.h"
#include "AlternativeCoverItemModel.h"

#include "Components/Covers/CoverLocation.h"
#include "Components/Covers/CoverLookupAlternative.h"
#include "Components/Library/LibraryManager.h"

#include "Helper/Message/Message.h"
#include "Helper/Language.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QRegExp>
#include <QPixmap>
#include <QList>
#include <QModelIndex>

struct GUI_AlternativeCovers::Private
{
	int						cur_idx;
	CoverLocation			cover_location;
	QStringList				filelist;
	bool					is_searching;
	SayonaraLoadingBar*		loading_bar=nullptr;

	AlternativeCoverItemModel*		model=nullptr;
	AlternativeCoverItemDelegate*	delegate=nullptr;

	CoverLookupAlternative*			cl_alternative=nullptr;

	~Private()
	{
		delete model;
		delete delegate;

		if(cl_alternative) {
			cl_alternative->stop();
		}
	}
};


GUI_AlternativeCovers::GUI_AlternativeCovers(QWidget* parent) :
	SayonaraDialog(parent)
{
	ui = new Ui::AlternativeCovers();
	m = Pimpl::make<GUI_AlternativeCovers::Private>();

	ui->setupUi(this);

	m->loading_bar = new SayonaraLoadingBar(ui->tv_images);
	m->cur_idx = -1;
	m->is_searching = false;

	m->model = new AlternativeCoverItemModel(this);
	m->delegate = new AlternativeCoverItemDelegate(this);

	ui->tv_images->setModel(m->model);
	ui->tv_images->setItemDelegate(m->delegate);

	connect(ui->btn_ok, &QPushButton::clicked, this, &GUI_AlternativeCovers::ok_clicked);
	connect(ui->btn_apply, &QPushButton::clicked, this, &GUI_AlternativeCovers::apply_clicked);
	connect(ui->btn_search, &QPushButton::clicked, this, &GUI_AlternativeCovers::search_clicked);
	connect(ui->tv_images, &QTableView::pressed, this, &GUI_AlternativeCovers::cover_pressed);	
	connect(ui->btn_file, &QPushButton::clicked, this, &GUI_AlternativeCovers::open_file_dialog);
	connect(ui->btn_close, &QPushButton::clicked, this, &SayonaraDialog::close);

	language_changed();
}


GUI_AlternativeCovers::~GUI_AlternativeCovers()
{
	delete_all_files();

	delete ui;
}


void GUI_AlternativeCovers::language_changed()
{
	ui->retranslateUi(this);
	ui->btn_search->setText(Lang::get(Lang::Search));
	ui->btn_close->setText(Lang::get(Lang::Close));
	ui->btn_apply->setText(Lang::get(Lang::Apply));
}

void GUI_AlternativeCovers::connect_and_start(const CoverLocation& cl)
{
	reset_model();
	delete_all_files();

	m->cover_location = cl;
	m->cl_alternative = new CoverLookupAlternative(this, cl, m->model->rowCount() * m->model->columnCount() + 5);

	connect(m->cl_alternative, &CoverLookupAlternative::sig_cover_found, this, &GUI_AlternativeCovers::cl_new_cover);
	connect(m->cl_alternative, &CoverLookupAlternative::sig_finished, this, &GUI_AlternativeCovers::cl_finished);

	m->is_searching = true;

	ui->btn_ok->setEnabled(false);
	ui->btn_apply->setEnabled(false);
	ui->btn_search->setText( Lang::get(Lang::Stop) );
	ui->le_search->setText(cl.search_term());
	ui->lab_info->setText(cl.search_term());

	m->cl_alternative->start();
	m->loading_bar->show();

	show();
}

void GUI_AlternativeCovers::start(const CoverLocation& cl)
{
	if(!cl.valid()){
		return;
	}

	ui->le_search->setText( cl.search_term() );
	ui->rb_local->setChecked(false);
	ui->rb_online->setChecked(true);

	connect_and_start(cl);
}

void GUI_AlternativeCovers::ok_clicked()
{
	apply_clicked();
	close();
}

void GUI_AlternativeCovers::apply_clicked()
{
	if(m->cur_idx == -1) {
		return;
	}

	RowColumn rc = m->model->cvt_2_row_col(m->cur_idx);

	QModelIndex idx = m->model->index(rc.row, rc.col);

	if(!idx.isValid()) {
		return;
	}

	QString cover_path = m->model->data(idx, Qt::UserRole).toString();
	QFile file(cover_path);

	if(!file.exists()) {
		Message::warning(tr("This cover does not exist"));
		return;
	}

	QImage img(cover_path);
	if(img.isNull()){
		return;
	}

	img.save(m->cover_location.cover_path());

	emit sig_cover_changed(m->cover_location);
}

void GUI_AlternativeCovers::search_clicked()
{
	if(m->is_searching && m->cl_alternative){
		m->cl_alternative->stop();
		return;
	}

	if(!ui->le_search->text().isEmpty()){
		QString text = ui->le_search->text();
		m->cover_location.set_search_term(text);
	}

	else{
		ui->le_search->setText( m->cover_location.search_term() );
	}

	connect_and_start(m->cover_location);
}


void GUI_AlternativeCovers::cl_new_cover(const QString& cover_path)
{
	m->filelist << cover_path;

	int n_files = m->filelist.size();

	RowColumn rc_last =     m->model->cvt_2_row_col( n_files - 1 );
	RowColumn rc_cur_idx =  m->model->cvt_2_row_col( m->cur_idx );
	bool is_valid =         m->model->is_valid(rc_cur_idx.row, rc_cur_idx.col);

	m->model->set_cover(rc_last.row, rc_last.col, cover_path);

	ui->btn_ok->setEnabled(is_valid);
	ui->btn_apply->setEnabled(is_valid);
	ui->lab_status->setText( tr("%1 covers found").arg(n_files) ) ;
}


void GUI_AlternativeCovers::cl_finished(bool b)
{
	Q_UNUSED(b)

	m->is_searching = false;

	ui->btn_search->setText(Lang::get(Lang::Search));

	m->cl_alternative->deleteLater();
	m->cl_alternative = nullptr;
	m->loading_bar->hide();
}


void GUI_AlternativeCovers::cover_pressed(const QModelIndex& idx)
{
	int row = idx.row();
	int col = idx.column();
	QSize sz = m->model->get_cover_size(idx);
	bool valid = m->model->is_valid(row, col);
	m->cur_idx = m->model->cvt_2_idx(row, col);

	ui->btn_ok->setEnabled(valid);
	ui->btn_apply->setEnabled(valid);

	QString size_str = QString("%1x%2").arg(sz.width()).arg(sz.height());
	ui->lab_img_size->setText( size_str );
}


void GUI_AlternativeCovers::reset_model()
{
	m->model->reset();
	ui->lab_status->clear();
}


void GUI_AlternativeCovers::open_file_dialog()
{
    QStringList filters;
        filters << "*.jpg";
        filters << "*.png";
        filters << "*.gif";

    QStringList lst = QFileDialog::getOpenFileNames(this,
                                  tr("Open image files"),
								  QDir::homePath(),
                                  filters.join(" "));
	if(lst.isEmpty())
	{
		return;
	}

	reset_model();

	int idx = 0;
	for(const QString& path : lst)
	{
		RowColumn rc = m->model->cvt_2_row_col( idx );
		m->model->set_cover(rc.row, rc.col, path);

        idx ++;
    }
}


void GUI_AlternativeCovers::delete_all_files()
{
	for(const QString& cover_path : m->filelist) {
		if(CoverLocation::isInvalidLocation(cover_path)){
			continue;
		}

		QFile f(cover_path);
		f.remove();
	}

	m->filelist.clear();
}


void GUI_AlternativeCovers::closeEvent(QCloseEvent *e)
{
	if(m->cl_alternative) {
		m->cl_alternative->stop();
	}

	m->loading_bar->hide();

	delete_all_files();

	SayonaraDialog::closeEvent(e);
}
