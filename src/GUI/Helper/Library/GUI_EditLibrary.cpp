#include "GUI_EditLibrary.h"
#include "GUI/Helper/ui_GUI_EditLibrary.h"
#include "Helper/Language.h"

#include <QFileDialog>
#include <QSizePolicy>

struct GUI_EditLibrary::Private
{
	QString old_name;
	QString old_path;

	EditMode edit_mode;
};

GUI_EditLibrary::GUI_EditLibrary(QWidget *parent) :
	SayonaraDialog (parent),
	ui(new Ui::GUI_EditLibrary)
{
	ui->setupUi(this);

	_m = Pimpl::make<Private>();
	_m->edit_mode = EditMode::New;

	ui->btn_choose_dir->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	connect(ui->btn_ok, &QPushButton::clicked, this, &GUI_EditLibrary::ok_clicked);
	connect(ui->btn_cancel, &QPushButton::clicked, this, &GUI_EditLibrary::cancel_clicked);
	connect(ui->btn_choose_dir, &QPushButton::clicked, this, &GUI_EditLibrary::choose_dir_clicked);

	language_changed();
}

GUI_EditLibrary::GUI_EditLibrary(const QString& name, const QString& path, QWidget* parent) :
	GUI_EditLibrary(parent)
{
	_m->edit_mode = EditMode::Edit;

	_m->old_name = name;
	_m->old_path = path;

	ui->le_name->setText(name);
	ui->le_path->setText(path);
	ui->lab_title->setText(Lang::get(Lang::Edit));

	this->setWindowTitle(ui->lab_title->text());
	this->setAttribute(Qt::WA_DeleteOnClose);
}


GUI_EditLibrary::~GUI_EditLibrary()
{
	delete ui; ui = nullptr;
}

void GUI_EditLibrary::ok_clicked()
{
	emit sig_accepted();
	close();
}

void GUI_EditLibrary::cancel_clicked()
{
	ui->le_path->clear();
	ui->le_name->clear();

	emit sig_recected();
	close();
}

void GUI_EditLibrary::choose_dir_clicked()
{
	QString old_dir = _m->old_path;
	if(old_dir.isEmpty()){
		old_dir = QDir::homePath();
	}

	QString new_dir = QFileDialog::getExistingDirectory(this,
														Lang::get(Lang::Directory),
														old_dir,
														QFileDialog::ShowDirsOnly);

	if(new_dir.isEmpty()){
		new_dir = _m->old_path;
	}

	ui->le_path->setText(new_dir);
}

QString GUI_EditLibrary::name() const
{
	return ui->le_name->text();
}

QString GUI_EditLibrary::path() const
{
	return ui->le_path->text();
}

bool GUI_EditLibrary::has_name_changed() const
{
	return (name() != _m->old_name);
}

bool GUI_EditLibrary::has_path_changed() const
{
	return (path() != _m->old_path);
}

GUI_EditLibrary::EditMode GUI_EditLibrary::edit_mode() const
{
	return _m->edit_mode;
}

void GUI_EditLibrary::language_changed()
{
	SayonaraDialog::language_changed();

	ui->btn_ok->setText(Lang::get(Lang::OK));
	ui->btn_cancel->setText(Lang::get(Lang::Cancel));
	ui->lab_path->setText(Lang::get(Lang::Directory));
	ui->lab_name->setText(Lang::get(Lang::Name));

	if(_m->edit_mode == EditMode::New) {
		ui->lab_title->setText(Lang::get(Lang::New));
	} else {
		ui->lab_title->setText(Lang::get(Lang::Edit));
	}

	this->setWindowTitle(ui->lab_title->text());
}

void GUI_EditLibrary::skin_changed()
{
	SayonaraDialog::skin_changed();
}
