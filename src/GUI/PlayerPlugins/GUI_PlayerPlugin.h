#ifndef GUI_PLAYERPLUGIN_H
#define GUI_PLAYERPLUGIN_H

#include <QHideEvent>
#include <QCloseEvent>

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

private:
	void close_cur_plugin();
	void hideEvent(QHideEvent* e) override;
	void closeEvent(QCloseEvent *e) override;

private slots:
	void language_changed() override;
};



#endif // GUI_PLAYERPLUGIN_H
