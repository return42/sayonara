#include "GUI_PlayerPlugin.h"
#include "Interfaces/PlayerPlugin/PlayerPlugin.h"

GUI_PlayerPlugin::GUI_PlayerPlugin(QWidget *parent) :
	SayonaraWidget(parent),
	Ui::GUI_PlayerPlugin()
{
	setupUi(this);

	connect(btn_close, &QPushButton::clicked, this, &GUI_PlayerPlugin::close_clicked);
}


void GUI_PlayerPlugin::show(PlayerPluginInterface* player_plugin)
{
	if(_current_plugin){
		_current_plugin->hide();
		_current_plugin->setParent(nullptr);
		this->layout()->removeWidget(_current_plugin);
	}

	_current_plugin = player_plugin;

	bool show_title = player_plugin->is_title_shown();

	header_widget->setVisible(show_title);
	lab_title->setText(player_plugin->get_display_name());

	verticalLayout->insertWidget(1, player_plugin);

	player_plugin->resize(this->width(), player_plugin->height());
	player_plugin->show();

	QWidget::show();
}


void GUI_PlayerPlugin::close_clicked()
{
	if(_current_plugin) {
		_current_plugin->close();
	}

	this->close();
}

void GUI_PlayerPlugin::language_changed() {
	if(_current_plugin){
		lab_title->setText(_current_plugin->get_display_name());
	}
}
