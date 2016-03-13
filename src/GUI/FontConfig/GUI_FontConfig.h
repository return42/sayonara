
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




#ifndef GUI_FONTCONFIG_H
#define GUI_FONTCONFIG_H

#include "Interfaces/PreferenceDialog/PreferenceDialogInterface.h"
#include "GUI/FontConfig/ui_GUI_FontConfig.h"

#include <QString>
#include <QFontDatabase>


class GUI_FontConfig :
		public PreferenceDialogInterface,
		protected Ui::GUI_FontConfig
{
	Q_OBJECT

	friend class PreferenceDialogInterface;

public:
	GUI_FontConfig(QWidget* parent=nullptr);
	virtual ~GUI_FontConfig();

	QString get_action_name() const override;

protected:
	void init_ui() override;

protected slots:
	void ok_clicked();
	void cancel_clicked();
	void apply_clicked();
	void default_clicked();
	void combo_text_changed(const QString& font_name);

	void language_changed() override;
	void skin_changed() override;
	QLabel* get_title_label() override;

private:
	QFontDatabase*	_font_db=nullptr;
	int				_cur_font_size;
	int				_cur_font_weight;
	bool			_is_default;
};



#endif // FONTCONFIG_H
