#ifndef GUI_PLAYERMENUBAR_H
#define GUI_PLAYERMENUBAR_H

#include <QMenuBar>
#include "GUI/Utils/Widgets/WidgetTemplate.h"
#include "Utils/Pimpl.h"

class QMenu;
class QAction;

class Menubar :
	public Gui::WidgetTemplate<QMenuBar>
{
	Q_OBJECT
	PIMPL(Menubar)

signals:
	void sig_close_clicked();
	void sig_logger_clicked();

public:
	Menubar(QWidget* parent=nullptr);
	~Menubar();

	void insert_player_plugin_action(QAction* action);
	void insert_preference_action(QAction* action);
	QAction* update_library_action(QMenu* current_library_menu, const QString& name);
	void show_library_action(bool visible);

private:
	void init_connections();

protected:
	void language_changed() override;
	void skin_changed() override;

private slots:
	void open_dir_clicked();
	void open_files_clicked();
	void shutdown_clicked();
	void skin_toggled(bool b);
	void show_library_toggled(bool b);
	void show_fullscreen_toggled(bool b);
	void help_clicked();
	void about_clicked();
	void awa_translators_finished();
};

#endif // GUI_PLAYERMENUBAR_H
