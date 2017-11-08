#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include "GUI/Utils/Widgets/Dialog.h"

class QString;
class QAction;

namespace Preferences
{
	class Base;
}

class PreferenceDialog :
	public Gui::Dialog
{

public:
	using Gui::Dialog::Dialog;

	virtual QString action_name() const=0;
	virtual QAction* action()=0;

	virtual void register_preference_dialog(Preferences::Base* dialog)=0;
	virtual void show_preference_dialog(const QString& identifier)=0;
};

#endif // PREFERENCEDIALOG_H

