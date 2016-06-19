#ifndef GUI_PLAYERPLUGIN_H
#define GUI_PLAYERPLUGIN_H

#include <QWidget>
#include <QResizeEvent>

#include "GUI/PlayerPlugins/ui_GUI_PlayerPlugin.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"

class PlayerPluginInterface;
class GUI_PlayerPlugin :
		public SayonaraWidget,
		private Ui::GUI_PlayerPlugin

{
	Q_OBJECT

public:
	explicit GUI_PlayerPlugin(QWidget *parent = 0);
	void set_content(PlayerPluginInterface* player_plugin);
	void show(PlayerPluginInterface* player_plugin);

private:
	PlayerPluginInterface* _current_plugin=nullptr;

private slots:
	void close_clicked();
	void language_changed() override;
};



#endif // GUI_PLAYERPLUGIN_H
