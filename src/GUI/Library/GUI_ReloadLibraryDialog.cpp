#include "GUI_ReloadLibraryDialog.h"
#include "GUI/Library/ui_GUI_ReloadLibraryDialog.h"

#include "Utils/Language.h"
#include <QComboBox>

struct GUI_ReloadLibraryDialog::Private
{
	QString library_name;

	Private(const QString& library_name) :
		library_name(library_name)
	{}
};

GUI_ReloadLibraryDialog::GUI_ReloadLibraryDialog(const QString& library_name, QWidget *parent) :
	Gui::Dialog(parent),
	ui(new Ui::GUI_ReloadLibraryDialog)
{
	m = Pimpl::make<Private>(library_name);

	ui->setupUi(this);

	this->setModal(true);

	connect(ui->btn_ok, &QPushButton::clicked, this, &GUI_ReloadLibraryDialog::ok_clicked);
	connect(ui->btn_cancel, &QPushButton::clicked, this, &GUI_ReloadLibraryDialog::cancel_clicked);
}


GUI_ReloadLibraryDialog::~GUI_ReloadLibraryDialog()
{
	delete ui;
}

void GUI_ReloadLibraryDialog::set_quality(Library::ReloadQuality quality)
{
	switch(quality)
	{
		case Library::ReloadQuality::Accurate:
			ui->combo_quality->setCurrentIndex(1);
			break;
		default:
			ui->combo_quality->setCurrentIndex(0);
	}
}


void GUI_ReloadLibraryDialog::language_changed()
{
	ui->btn_ok->setText(Lang::get(Lang::OK));
	ui->btn_cancel->setText(Lang::get(Lang::Cancel));
	ui->lab_title->setText(Lang::get(Lang::ReloadLibrary) + ": " + m->library_name);
	ui->lab_select_reload_quality->setText(tr("Select reload mode"));

	ui->combo_quality->clear();
	ui->combo_quality->addItem(tr("Check for changed files (fast)"));
	ui->combo_quality->addItem(tr("Deep scan (slow)"));

	this->setWindowTitle(Lang::get(Lang::ReloadLibrary) + ": " + m->library_name);
}

void GUI_ReloadLibraryDialog::ok_clicked()
{
	int cur_idx = ui->combo_quality->currentIndex();
	if(cur_idx == 0)
	{
		emit sig_accepted(Library::ReloadQuality::Fast);
	}

	else if(cur_idx == 1)
	{
		emit sig_accepted(Library::ReloadQuality::Accurate);
	}

	close();
}

void GUI_ReloadLibraryDialog::cancel_clicked()
{
	ui->combo_quality->setCurrentIndex(0);

	close();
}
