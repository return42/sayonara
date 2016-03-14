/* PreferenceDialogInterface.h */

/* Copyright (C) 2011-2016  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include "GUI/Helper/SayonaraWidget.h"

#include <QAction>
#include <QByteArray>
#include <QShowEvent>
#include <QCloseEvent>
#include <QLabel>

/**
 * @brief Abstract Interface you should use when creating a preference dialog.
 *
 * If you wish to subclass, reimplement void get_action_name() and void init_ui().\n
 * In every function that makes use of the widgets call bool is_ui_initialized() first.\n
 * Call setup_parent(this) in init_ui() first.\n
 * If you wish to reimplement void language_changed(), call PreferenceDialogInterface::language_changed at the end.
 * @ingroup Interfaces
 */
class PreferenceDialogInterface : public SayonaraDialog
{
	Q_OBJECT

private:
	QAction*	_action=nullptr;
	bool		_is_initialized;
	QByteArray	_geometry;


protected slots:
	/**
	 * @brief If this method is overridden, call the PreferenceDialogInterface::language_changed at the end.\n
	 * It sets the title label, window title and the action name in the preferences menu\n
	 * There's no need to call a listener to this function\n
	 * Check for is_ui_initialized()
	 */
	void language_changed() override;


protected:

	/**
	 * @brief has to be implemented and should return the translated action text
	 * @return translated action name
	 */
	virtual QString get_action_name() const=0;
	virtual QLabel* get_title_label()=0;

	/**
	 * @brief call setup_parent(this) here.\n
	 * initialize compoenents and connections here.\n
	 * After calling setup_parent(this), the preference Dialog is ready to use, language_changed() is called automatically
	 */
	virtual void init_ui()=0;

	template<typename T>
	/**
	 * @brief Sets up the Preference dialog. After this method, the dialog is "ready to use"\n
	 * This method should be the first to be called when calling init_ui()
	 * @param widget should always be "this"
	 */
	void setup_parent(T* widget) final {

		widget->setupUi(widget);
		widget->setModal(true);

		QLabel* title_label = widget->get_title_label();
		if(title_label){
			title_label->setText(widget->get_action_name());
		}

		_is_initialized = true;

		widget->language_changed();
	}


	void showEvent(QShowEvent* e) override;
	void closeEvent(QCloseEvent* e) override;

	/**
	 * @brief checks if ui has already been initialized.
	 * @return false, if the widget has never been activated before, true else
	 */
	virtual bool is_ui_initialized() const final;

public:
	/**
	 * @brief Standard constructor
	 * @param parent
	 */
	PreferenceDialogInterface(QWidget* parent=nullptr);
	virtual ~PreferenceDialogInterface();

	/**
	 * @brief get action with translated text
	 * @return
	 */
	virtual QAction* get_action() final;
};

typedef QList<PreferenceDialogInterface*> PreferenceDialogList;

#endif // PREFERENCEDIALOG_H
