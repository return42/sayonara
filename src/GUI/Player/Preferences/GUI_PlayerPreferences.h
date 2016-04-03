#ifndef GUI_PLAYERPREFERENCES_H
#define GUI_PLAYERPREFERENCES_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"
#include "GUI/Player/Preferences/ui_GUI_PlayerPreferences.h"


class GUI_PlayerPreferences :
		public PreferenceWidgetInterface,
		private Ui::GUI_PlayerPreferences
{
	Q_OBJECT

	friend class PreferenceInterface<SayonaraWidget>;
	friend class PreferenceWidgetInterface;

public:
	explicit GUI_PlayerPreferences(QWidget *parent = 0);
	~GUI_PlayerPreferences();



protected:
	void init_ui();
	void language_changed();



public:
	QString get_action_name() const;
	QLabel*get_title_label();
	void commit();
	void revert();
};

#endif // GUI_PLAYERPREFERENCES_H
