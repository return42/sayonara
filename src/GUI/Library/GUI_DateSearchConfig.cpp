#include "GUI_DateSearchConfig.h"
#include "GUI/Library/ui_GUI_DateSearchConfig.h"
#include "Helper/Library/DateFilter.h"

struct GUI_DateSearchConfig::Private
{
	Library::DateFilter org_filter;
	Library::DateFilter edited_filter;
};

static bool check_name(const QString& name)
{
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
}

GUI_DateSearchConfig::~GUI_DateSearchConfig()
{
	delete ui; ui=nullptr;
}

void GUI_DateSearchConfig::language_changed()
{
	ui->retranslateUi(this);
}

void GUI_DateSearchConfig::ok_clicked()
{
	if(!check_name(ui->le_title->text())){
		return;
	}

	commit();

	_m->org_filter = _m->edited_filter;

	this->close();
}

void GUI_DateSearchConfig::save_as_clicked()
{
	if(!check_name(ui->le_title->text())){
		return;
	}

	QString new_name = "Todo";

	commit();

	_m->edited_filter.set_name(new_name);
	_m->org_filter = _m->edited_filter;

	this->close();
}

void GUI_DateSearchConfig::cancel_clicked()
{
	this->close();
}

void GUI_DateSearchConfig::commit()
{
	Library::DateFilter f = _m->org_filter;

	switch(ui->comboBox->currentIndex())
	{

		case 0:
		{
			Library::DateFilter::TimeSpan time_span;

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
			Library::DateFilter::TimeSpan time_span;

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
			Library::DateFilter::TimeSpan time_span_from, time_span_to;

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
				//todo
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
