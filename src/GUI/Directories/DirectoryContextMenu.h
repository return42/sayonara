#ifndef DIRECTORYCONTEXTMENU_H
#define DIRECTORYCONTEXTMENU_H

#include "GUI/Utils/ContextMenu/LibraryContextMenu.h"

class DirectoryContextMenu :
		public LibraryContextMenu
{
	Q_OBJECT
	PIMPL(DirectoryContextMenu)

signals:
	void sig_create_dir_clicked();
	void sig_rename_clicked();
	void sig_collapse_all_clicked();

public:
	enum Mode
	{
		Dir=0,
		File
	};

	DirectoryContextMenu(Mode mode, QWidget* parent);
	~DirectoryContextMenu();

	void set_create_dir_visible(bool b);
	void set_rename_visible(bool b);
	void set_collapse_all_visibled(bool b);

protected:
	void language_changed() override;
	void skin_changed() override;
};

#endif // DIRECTORYCONTEXTMENU_H
