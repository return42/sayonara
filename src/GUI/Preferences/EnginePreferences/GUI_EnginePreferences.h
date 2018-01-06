#ifndef GUI_ENGINEPREFERENCES_H
#define GUI_ENGINEPREFERENCES_H

#include "Interfaces/PreferenceDialog/PreferenceWidget.h"

UI_FWD(GUI_EnginePreferences)

class GUI_EnginePreferences :
	public Preferences::Base
{
	Q_OBJECT
	UI_CLASS(GUI_EnginePreferences)

public:
	explicit GUI_EnginePreferences(const QString& identifier);
	virtual ~GUI_EnginePreferences();

	// Base interface
	QString action_name() const override;
	bool commit() override;
	void revert() override;
	void init_ui() override;
	void retranslate_ui() override;
};

#endif // GUI_ENGINEPREFERENCES_H
