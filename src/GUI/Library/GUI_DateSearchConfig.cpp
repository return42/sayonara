/* GUI_DateSearchConfig.cpp */

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

#include "GUI_DateSearchConfig.h"
#include "GUI/Library/ui_GUI_DateSearchConfig.h"
#include "Helper/Library/DateFilter.h"
#include "Helper/Language.h"

#include <QInputDialog>
#include <QShowEvent>

struct GUI_DateSearchConfig::Private
{
	Library::DateFilter org_filter;
	Library::DateFilter edited_filter;
	GUI_DateSearchConfig::Result result;
};

static bool check_name(const QString& name)
{
	if(name.isEmpty()){
		return false;
	}

	return (!name.contains(",") && !name.contains(";"));
}


GUI_DateSearchConfig::GUI_DateSearchConfig(QWidget* parent) :
	SayonaraDialog(parent)
{
	_m = Pimpl::make<GUI_DateSearchConfig::Private>();

	ui = new Ui::GUI_DateSearchConfig();
	ui->setupUi(this);

	ui->comboBox->setCurrentIndex(0);
	ui->stackedWidget->setCurrentIndex(0);

	connect(ui->btn_ok, &QPushButton::clicked, this, &GUI_DateSearchConfig::ok_clicked);
	connect(ui->btn_save_as, &QPushButton::clicked, this, &GUI_DateSearchConfig::save_as_clicked);
	connect(ui->btn_cancel, &QPushButton::clicked, this, &GUI_DateSearchConfig::cancel_clicked);

	language_changed();
}

GUI_DateSearchConfig::~GUI_DateSearchConfig()
{
	delete ui; ui=nullptr;
}

void GUI_DateSearchConfig::language_changed()
{
	ui->retranslateUi(this);

	ui->rb_bwf_days->setText(Lang::get(Lang::Days));
	ui->rb_bwf_months->setText(Lang::get(Lang::Months));
	ui->rb_bwf_weeks->setText(Lang::get(Lang::Weeks));
	ui->rb_bwf_years->setText(Lang::get(Lang::Weeks));

	ui->rb_bwt_days->setText(Lang::get(Lang::Days));
	ui->rb_bwt_months->setText(Lang::get(Lang::Months));
	ui->rb_bwt_weeks->setText(Lang::get(Lang::Weeks));
	ui->rb_bwt_years->setText(Lang::get(Lang::Weeks));

	ui->rb_nt_days->setText(Lang::get(Lang::Days));
	ui->rb_nt_months->setText(Lang::get(Lang::Months));
	ui->rb_nt_weeks->setText(Lang::get(Lang::Weeks));
	ui->rb_nt_years->setText(Lang::get(Lang::Weeks));

	ui->btn_cancel->setText(Lang::get(Lang::Cancel));
	ui->btn_save_as->setText(Lang::get(Lang::SaveAs).triplePt());
	ui->le_title->setPlaceholderText(Lang::get(Lang::EnterName));
}

void GUI_DateSearchConfig::ok_clicked()
{
	if(!check_name(ui->le_title->text())){
		ui->le_title->setFocus();
		return;
	}

	commit();

	_m->org_filter = _m->edited_filter;
	_m->result = GUI_DateSearchConfig::Result::Replace;

	this->close();
}

void GUI_DateSearchConfig::save_as_clicked()
{
	QString new_name = QInputDialog::getText(this,
											 Lang::get(Lang::SaveAs),
											 Lang::get(Lang::SaveAs));

	if(!check_name(new_name)){
		return;
	}

	if(new_name.isEmpty()){
		_m->result = GUI_DateSearchConfig::Result::Cancelled;
		return;
	}

	commit();

	_m->edited_filter.set_name(new_name);
	_m->org_filter = _m->edited_filter;
	_m->result = GUI_DateSearchConfig::Result::New;

	this->close();
}

void GUI_DateSearchConfig::cancel_clicked()
{
	_m->result = GUI_DateSearchConfig::Result::Cancelled;
	this->close();
}

void GUI_DateSearchConfig::commit()
{
	Library::DateFilter f = _m->org_filter;

	switch(ui->comboBox->currentIndex())
	{
		case 0:
		{
			Library::DateFilter::TimeSpan time_span = Library::DateFilter::TimeSpan::Years;

			if(ui->rb_nt_days->isChecked()){
				time_span = Library::DateFilter::TimeSpan::Days;
			}
			else if(ui->rb_nt_weeks->isChecked()){
				time_span = Library::DateFilter::TimeSpan::Weeks;
			}
			else if(ui->rb_nt_months->isChecked()){
				time_span = Library::DateFilter::TimeSpan::Months;
			}
			else if(ui->rb_nt_years->isChecked()){
				time_span = Library::DateFilter::TimeSpan::Years;
			}

			f.set_newer_than(time_span, (quint8) ui->sb_nt->value());
		}
		break;


		case 1:
		{
			Library::DateFilter::TimeSpan time_span = Library::DateFilter::TimeSpan::Years;

			if(ui->rb_ot_days->isChecked()){
				time_span = Library::DateFilter::TimeSpan::Days;
			}
			else if(ui->rb_ot_weeks->isChecked()){
				time_span = Library::DateFilter::TimeSpan::Weeks;
			}
			else if(ui->rb_ot_months->isChecked()){
				time_span = Library::DateFilter::TimeSpan::Months;
			}
			else if(ui->rb_ot_years->isChecked()){
				time_span = Library::DateFilter::TimeSpan::Years;
			}

			f.set_older_than(time_span, (quint8) ui->sb_ot->value());
		}
		break;


		case 2:
		{
			Library::DateFilter::TimeSpan time_span_from = Library::DateFilter::TimeSpan::Years;
			Library::DateFilter::TimeSpan time_span_to = Library::DateFilter::TimeSpan::Years;

			if(ui->rb_bwf_days->isChecked()){
				time_span_from = Library::DateFilter::TimeSpan::Days;
			}
			else if(ui->rb_bwf_weeks->isChecked()){
				time_span_from = Library::DateFilter::TimeSpan::Weeks;
			}
			else if(ui->rb_bwf_months->isChecked()){
				time_span_from = Library::DateFilter::TimeSpan::Months;
			}
			else if(ui->rb_bwf_years->isChecked()){
				time_span_from = Library::DateFilter::TimeSpan::Years;
			}

			if(ui->rb_bwt_days->isChecked()){
				time_span_to = Library::DateFilter::TimeSpan::Days;
			}
			else if(ui->rb_bwt_weeks->isChecked()){
				time_span_to = Library::DateFilter::TimeSpan::Weeks;
			}
			else if(ui->rb_bwt_months->isChecked()){
				time_span_to = Library::DateFilter::TimeSpan::Months;
			}
			else if(ui->rb_bwt_years->isChecked()){
				time_span_to = Library::DateFilter::TimeSpan::Years;
			}

			f.set_between(time_span_from, (quint8) ui->sb_bwf->value(),
						  time_span_to, (quint8) ui->sb_bwt->value());
		}
		break;

		default:
			break;
	}

	f.set_name(ui->le_title->text());

	_m->edited_filter = f;
}


void GUI_DateSearchConfig::set_filter(const Library::DateFilter& filter)
{
	_m->result = GUI_DateSearchConfig::Result::Cancelled;

	Library::DateFilter::Type type = filter.type();
	Library::DateFilter::TimeSpanMap time_span_map = filter.time_span_map();

	QMap<Library::DateFilter::TimeSpan, QRadioButton*>
			button_span_nt, button_span_ot, button_span_bwf, button_span_bwt;

	button_span_nt.insert(Library::DateFilter::TimeSpan::Days, ui->rb_nt_days);
	button_span_nt.insert(Library::DateFilter::TimeSpan::Weeks, ui->rb_nt_weeks);
	button_span_nt.insert(Library::DateFilter::TimeSpan::Months, ui->rb_nt_months);
	button_span_nt.insert(Library::DateFilter::TimeSpan::Years, ui->rb_nt_years);

	button_span_ot.insert(Library::DateFilter::TimeSpan::Days, ui->rb_ot_days);
	button_span_ot.insert(Library::DateFilter::TimeSpan::Weeks, ui->rb_ot_weeks);
	button_span_ot.insert(Library::DateFilter::TimeSpan::Months, ui->rb_ot_months);
	button_span_ot.insert(Library::DateFilter::TimeSpan::Years, ui->rb_ot_years);

	button_span_bwf.insert(Library::DateFilter::TimeSpan::Days, ui->rb_bwf_days);
	button_span_bwf.insert(Library::DateFilter::TimeSpan::Weeks, ui->rb_bwf_weeks);
	button_span_bwf.insert(Library::DateFilter::TimeSpan::Months, ui->rb_bwf_months);
	button_span_bwf.insert(Library::DateFilter::TimeSpan::Years, ui->rb_bwf_years);

	button_span_bwt.insert(Library::DateFilter::TimeSpan::Days, ui->rb_bwt_days);
	button_span_bwt.insert(Library::DateFilter::TimeSpan::Weeks, ui->rb_bwt_weeks);
	button_span_bwt.insert(Library::DateFilter::TimeSpan::Months, ui->rb_bwt_months);
	button_span_bwt.insert(Library::DateFilter::TimeSpan::Years, ui->rb_bwt_years);

	switch(type)
	{
		case Library::DateFilter::Type::NewerThan:
		{
			Library::DateFilter::TimeSpan span = time_span_map[0].first;
			QRadioButton* btn = button_span_nt[span];
			btn->setChecked(true);

			ui->sb_nt->setValue(time_span_map[0].second);

			ui->comboBox->setCurrentIndex(0);
			ui->stackedWidget->setCurrentIndex(0);
		}
		break;


		case Library::DateFilter::Type::OlderThan:
		{
			Library::DateFilter::TimeSpan span = time_span_map[0].first;
			QRadioButton* btn = button_span_ot[span];
			btn->setChecked(true);

			ui->sb_ot->setValue(time_span_map[0].second);

			ui->comboBox->setCurrentIndex(1);
			ui->stackedWidget->setCurrentIndex(1);

		}
		break;


		case Library::DateFilter::Type::Between:
		{
			if(time_span_map.size() < 2){
				return;
			}

			Library::DateFilter::TimeSpan span_from = time_span_map[0].first;
			Library::DateFilter::TimeSpan span_to = time_span_map[1].first;

			QRadioButton* btn_from = button_span_bwf[span_from];
			QRadioButton* btn_to = button_span_bwt[span_to];
			btn_from->setChecked(true);
			btn_to->setChecked(true);

			ui->sb_bwf->setValue(time_span_map[0].second);
			ui->sb_bwt->setValue(time_span_map[1].second);

			ui->comboBox->setCurrentIndex(2);
			ui->stackedWidget->setCurrentIndex(2);
		}

		break;

		default:
			break;
	}

	_m->org_filter = filter;
	_m->edited_filter = Library::DateFilter();

	ui->le_title->setText(filter.name());
}

Library::DateFilter GUI_DateSearchConfig::get_edited_filter() const
{
	return _m->edited_filter;
}

GUI_DateSearchConfig::Result GUI_DateSearchConfig::get_result() const
{
	return _m->result;
}
