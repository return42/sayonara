#ifndef DIRECTORYTREEVIEW_H
#define DIRECTORYTREEVIEW_H

#include <QTreeView>
#include <QModelIndexList>
#include "Helper/SayonaraClass.h"
#include "Helper/MetaData/MetaDataList.h"

class LibraryContextMenu;
class AbstractSearchFileTreeModel;
class DirectoryTreeView :
		public QTreeView,
		private SayonaraClass
{

	Q_OBJECT

signals:
	void sig_info_clicked();
	void sig_delete_clicked();
	void sig_play_next_clicked();
	void sig_append_clicked();

public:
	DirectoryTreeView(QWidget* parent=nullptr);

	AbstractSearchFileTreeModel* get_model() const;

	QModelIndexList			get_selected_rows() const;
	MetaDataList read_metadata() const;

private:
	LibraryContextMenu*				_context_menu=nullptr;
	AbstractSearchFileTreeModel*	_model = nullptr;

private:
	void mousePressEvent(QMouseEvent* event) override;
	void init_context_menu();


private slots:
	void _sl_library_path_changed();

};

#endif // DIRECTORYTREEVIEW_H
