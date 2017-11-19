#ifndef GUI_SEARCHPREFERENCES_H
#define GUI_SEARCHPREFERENCES_H

#include "Interfaces/PreferenceDialog/PreferenceWidget.h"
#include "Utils/Pimpl.h"

UI_FWD(GUI_SearchPreferences)

class GUI_SearchPreferences :
	public Preferences::Base
{
	Q_OBJECT
	UI_CLASS(GUI_SearchPreferences)

public:
	GUI_SearchPreferences(const QString& identifier);
	~GUI_SearchPreferences();

public:
	QString action_name() const override;
	bool commit() override;
	void revert() override;
	void init_ui() override;
	void retranslate_ui() override;
};

#endif // GUI_SEARCHPREFERENCES_H
