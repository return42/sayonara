#ifndef GUI_EDITLIBRARY_H
#define GUI_EDITLIBRARY_H

#include <QDialog>
#include "Helper/Pimpl.h"
#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"

namespace Ui {
	class GUI_EditLibrary;
}

class GUI_EditLibrary :
        public SayonaraDialog
{
	Q_OBJECT
	PIMPL(GUI_EditLibrary)

signals:
	void sig_accepted();
	void sig_recected();

public:

	explicit GUI_EditLibrary(const QString& name, const QString& path, QWidget *parent = 0);
	explicit GUI_EditLibrary(QWidget *parent = 0);
	~GUI_EditLibrary();

private:
	Ui::GUI_EditLibrary *ui=nullptr;


private slots:
	void ok_clicked();
	void cancel_clicked();
	void choose_dir_clicked();

	void language_changed() override;
	void skin_changed() override;

public:

	enum class EditMode
	{
		New=0,
		Edit=1
	};

	QString name() const;
	QString path() const;

	bool has_name_changed() const;
	bool has_path_changed() const;

	EditMode edit_mode() const;
};

#endif // GUI_EDITLIBRARY_H