#ifndef FILELISTVIEW_H
#define FILELISTVIEW_H

#include "GUI/Helper/SearchableWidget/SearchableListView.h"
#include <QModelIndexList>
#include <QFileSystemModel>
#include "Helper/SayonaraClass.h"
#include "Helper/MetaData/MetaDataList.h"

class LibraryContextMenu;
class FileListView :
		public SearchableListView,
		private SayonaraClass

{

	Q_OBJECT

signals:
	void sig_info_clicked();
	void sig_delete_clicked();
	void sig_play_next_clicked();
	void sig_append_clicked();

public:
	FileListView(QWidget* parent=nullptr);

	QModelIndexList get_selected_rows() const;
	QFileSystemModel* get_model() const;
	MetaDataList read_metadata() const;
	QStringList get_filelist() const;

private:
	LibraryContextMenu*	_context_menu=nullptr;
	QFileSystemModel*	_model=nullptr;

private:
	void mousePressEvent(QMouseEvent* event) override;
	void init_context_menu();

};


#endif // FILELISTVIEW_H
