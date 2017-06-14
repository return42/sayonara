#ifndef GUI_ICONPREFERENCES_H
#define GUI_ICONPREFERENCES_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"
#include "Helper/Pimpl.h"

class QWidget;
namespace Ui
{
	class GUI_IconPreferences;
}

class GUI_IconPreferences :
		public PreferenceWidgetInterface
{
	Q_OBJECT
	PIMPL(GUI_IconPreferences)

public:
	explicit GUI_IconPreferences(QWidget* parent=nullptr);
	virtual ~GUI_IconPreferences();

protected:
	void init_ui() override;
	void retranslate_ui() override;

public:
	QString get_action_name() const override;
	void commit() override;
	void revert() override;

private:
	Ui::GUI_IconPreferences* ui=nullptr;
};

#endif // GUI_ICONPREFERENCES_H
