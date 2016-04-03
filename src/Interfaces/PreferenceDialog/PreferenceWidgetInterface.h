#ifndef PREFERENCEWIDGETINTERFACE_H
#define PREFERENCEWIDGETINTERFACE_H


#include "PreferenceInterface.h"


/**
 * @brief Abstract Interface you should use when creating a preference dialog.
 *
 * If you wish to subclass, reimplement void get_action_name() and void init_ui().\n
 * In every function that makes use of the widgets call bool is_ui_initialized() first.\n
 * Call setup_parent(this) in init_ui() first.\n
 * If you wish to reimplement void language_changed(), call PreferenceDialogInterface::language_changed at the end.
 * @ingroup Interfaces
 */
class PreferenceWidgetInterface : public PreferenceInterface<SayonaraWidget>
{
	Q_OBJECT

	friend class PreferenceInterface<SayonaraDialog>;

public:
	/**
	 * @brief Standard constructor
	 * @param parent
	 */
	PreferenceWidgetInterface(QWidget* parent=nullptr);
	virtual ~PreferenceWidgetInterface();
};

#endif // PREFERENCEWIDGETINTERFACE_H
