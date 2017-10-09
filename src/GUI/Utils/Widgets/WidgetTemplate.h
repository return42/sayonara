/* WidgetTemplate.h */

/* Copyright (C) 2011-2017  Lucio Carreras
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

#ifndef SAYONARAWIDGETTEMPLATE_H
#define SAYONARAWIDGETTEMPLATE_H

#include "GUI/Utils/GUIClass.h"
#include "Utils/Settings/SayonaraClass.h"
#include "Utils/Settings/SettingNotifier.h"
#include "Utils/Settings/SettingKey.h"
#include <QShowEvent>

class QWidget;

#define combo_current_index_changed_int	static_cast<void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged)
#define combo_activated_int	static_cast<void (QComboBox::*) (int)>(&QComboBox::activated)
#define spinbox_value_changed_int	static_cast<void (QSpinBox::*) (int)>(&QSpinBox::valueChanged)

namespace Gui
{
	template<typename T>
	/**
	 * @brief Template for Sayonara Widgets. This template is responsible for holding a reference to the settings
	 * @ingroup Widgets
	 * @ingroup Interfaces
	 */

	class WidgetTemplate :
			public T,
			protected SayonaraClass
	{
	public:

		template<typename... Args>
		WidgetTemplate(Args&&... args) :
			T(std::forward<Args>(args)...),
			SayonaraClass()
		{
			Set::listen(Set::Player_Language, this, &WidgetTemplate<T>::language_changed);
			Set::listen(Set::Player_Style, this, &WidgetTemplate<T>::skin_changed);
		}

		virtual ~WidgetTemplate() {}

		virtual void language_changed() {}
		virtual void skin_changed() {}

		virtual void showEvent(QShowEvent* e) override
		{
			language_changed();
			skin_changed();

			T::showEvent(e);
		}
	};
}

#endif // SAYONARAWIDGETTEMPLATE_H
