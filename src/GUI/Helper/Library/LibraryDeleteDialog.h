#ifndef LIBRARYDELETEDIALOG_H
#define LIBRARYDELETEDIALOG_H

#include <QMessageBox>
#include "Helper/Library/LibraryNamespaces.h"

class LibraryDeleteDialog :
	public QMessageBox
{
private:
    Library::TrackDeletionMode _answer;
    int	_n_tracks;

public:
    LibraryDeleteDialog(int n_tracks, QWidget* parent=nullptr);
    virtual ~LibraryDeleteDialog();

    void set_num_tracks(int n_tracks);

    int exec() override;
    Library::TrackDeletionMode answer() const;
};


#endif // LIBRARYDELETEDIALOG_H
