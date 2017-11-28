#ifndef GUI_RELOADLIBRARYDIALOG_H
#define GUI_RELOADLIBRARYDIALOG_H

#include "GUI/Utils/Widgets/Dialog.h"

#include "Utils/Pimpl.h"
#include "Utils/Library/LibraryNamespaces.h"

UI_FWD(GUI_ReloadLibraryDialog)

class GUI_ReloadLibraryDialog :
	public Gui::Dialog
{
	Q_OBJECT
	PIMPL(GUI_ReloadLibraryDialog)
	UI_CLASS(GUI_ReloadLibraryDialog)

signals:
	void sig_accepted(Library::ReloadQuality quality);

public:
	explicit GUI_ReloadLibraryDialog(const QString& library_name, QWidget *parent=nullptr);
	~GUI_ReloadLibraryDialog();

	void set_quality(Library::ReloadQuality quality);

private slots:
	void ok_clicked();
	void cancel_clicked();
	void combo_changed(int);

protected:
	void language_changed() override;
};

#endif // GUI_RELOADLIBRARYDIALOG_H
