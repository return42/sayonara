#ifndef GUI_PLAYLISTPREFERENCES_H
#define GUI_PLAYLISTPREFERENCES_H


#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"
#include "GUI/Preferences/PlaylistPreferences/ui_GUI_PlaylistPreferences.h"


class GUI_PlaylistPreferences :
		public PreferenceWidgetInterface,
		protected Ui::GUI_PlaylistPreferences
{
	Q_OBJECT

	friend class PreferenceWidgetInterface;
	friend class PreferenceInterface<SayonaraWidget>;

public:
	explicit GUI_PlaylistPreferences(QWidget *parent = 0);
	~GUI_PlaylistPreferences();

	void commit() override;
	void revert() override;

private:
	void init_ui() override;
	QString get_action_name() const override;

private slots:
	void language_changed() override;
	void cb_toggled(bool b);

};

#endif // GUI_PLAYLISTPREFERENCES_H
