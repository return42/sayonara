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

/**
 * @brief The PreferenceDialogInterface class
 * @ingroup Interfaces
 */
class PreferenceDialogInterface : public SayonaraDialog
{
	Q_OBJECT

private:
	QAction*	_action=nullptr;
	bool		_is_initialized;
	QByteArray	_geometry;

	void set_ui_initialized();

protected slots:
	void language_changed() override;


protected:

	/**
	 * @brief has to be implemented and should return the translated action text
	 * @return translated action name
	 */
	virtual QString get_action_name() const=0;

	virtual void init_ui()=0;

	template<typename T>
	void setup_parent(T* widget){

		widget->setupUi(widget);
		widget->setModal(true);

		_is_initialized = true;

		widget->language_changed();
	}

	void showEvent(QShowEvent* e) override;
	void closeEvent(QCloseEvent* e) override;

	bool is_ui_initialized() const;

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
