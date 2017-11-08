/* GUI_IconPreferences.cpp */

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



#include "GUI_IconPreferences.h"
#include "GUI/Preferences/ui_GUI_IconPreferences.h"
#include "Utils/Language.h"
#include "Utils/Logger/Logger.h"
#include "Utils/Settings/Settings.h"

#include <QStringList>
#include <QIcon>
#include <QDir>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>

class IconRadioButton : public QRadioButton
{
private:
	QString _data;

public:
	using QRadioButton::QRadioButton;

	void set_data(const QString& data)
	{
		_data = data;
	}

	QString data() const
	{
		return _data;
	}
};

struct GUI_IconPreferences::Private
{
	QHash<QString, IconRadioButton*> rb_map;
	QString original_theme;

	Private()
	{
		original_theme = QIcon::themeName();
	}
};

GUI_IconPreferences::GUI_IconPreferences(const QString& identifier) :
	Base(identifier)
{
	m = Pimpl::make<Private>();
}

GUI_IconPreferences::~GUI_IconPreferences() {}

static
void add_line(QWidget* widget)
{
	QLayout* layout = widget->layout();
	QFrame* line = new QFrame(widget);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);

	layout->addWidget(line);
}


static
void add_header_entry(const QString& name, QWidget* widget)
{
	QLayout* layout = widget->layout();

	add_line(widget);

	QLabel* label = new QLabel(widget);
	label->setText(name);
	label->setStyleSheet("font-weight: bold;");
	layout->addWidget(label);

	add_line(widget);
}


static
IconRadioButton* add_radio_button(const QString& text, const QString& data, QWidget* widget, bool ignore_errors)
{
	if(!ignore_errors){
		QIcon::setThemeName(text);
		QIcon icon = QIcon::fromTheme("media-playback-start");
		if(icon.isNull()){
			return nullptr;
		}
	}

	IconRadioButton* rb = new IconRadioButton(text, widget);
	rb->set_data(data);

	return rb;
}

void GUI_IconPreferences::init_ui()
{
	if(is_ui_initialized()){
		return;
	}

	setup_parent(this, &ui);

	QWidget* widget = ui->scroll_area_widget;
	QLayout* layout = widget->layout();
	add_line(widget);

	IconRadioButton* rb_automatic = add_radio_button(
					tr("Automatic") +
					" (" + Lang::get(Lang::Default) + ")",
					"",
					 widget,
					true
	);

	layout->addWidget(rb_automatic);

	rb_automatic->setStyleSheet("font-weight: bold;");
	m->rb_map[""] = rb_automatic;


	QStringList icon_paths = QIcon::themeSearchPaths();
	for(const QString& icon_path : icon_paths)
	{
		QDir d(icon_path);
		QStringList subdirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

		if(subdirs.isEmpty()){
			continue;
		}

		QList<IconRadioButton*> buttons;
		for(const QString& subdir : subdirs) {

			IconRadioButton* rb = add_radio_button(subdir, subdir, widget, false);
			if(!rb){
				continue;
			}

			connect(rb, &QRadioButton::toggled, [=](bool checked){
				if(checked){
					this->theme_changed(rb->data());
				}
			});

			m->rb_map[subdir] = rb;

			if(m->original_theme.compare(QIcon::themeName()) == 0){
				rb->setStyleSheet("font-weight: bold;");
			}

			buttons << rb;
		}

		if(!buttons.isEmpty()){
			add_header_entry(icon_path, widget);
		}

		for(IconRadioButton* rb : buttons)
		{

			layout->addWidget(rb);
		}
	}

	revert();
}

void GUI_IconPreferences::retranslate_ui()
{
	IconRadioButton* rb = m->rb_map[""];
	if(rb){
		rb->setText(
					tr("Automatic") +
					" (" + Lang::get(Lang::Default) + ")"
		);
	}
}

QString GUI_IconPreferences::action_name() const
{
	return tr("Icons");
}

void GUI_IconPreferences::commit()
{
	for(const QString& key : m->rb_map.keys())
	{
		IconRadioButton* rb = m->rb_map[key];
		rb->setStyleSheet("font-weight: normal;");
		if(rb->isChecked()){
			_settings->set(Set::Icon_Theme, key);
			rb->setStyleSheet("font-weight: bold;");
			m->original_theme = rb->data();
		}
	}
}

void GUI_IconPreferences::revert()
{
	for(const QString& key : m->rb_map.keys())
	{
		IconRadioButton* rb = m->rb_map[key];
		rb->setChecked(key.compare(m->original_theme) == 0);
	}

	QIcon::setThemeName(m->original_theme);
}

static void apply_icon(const QString& n, const QString& theme_name, QLabel* label)
{
	QIcon::setThemeName(theme_name);
	QIcon icon = QIcon::fromTheme(n);
	label->setPixmap(icon.pixmap(32, 32).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));

}

void GUI_IconPreferences::theme_changed(const QString& theme)
{
	apply_icon("media-playback-start", theme, ui->lab_play);
	apply_icon("media-skip-backward", theme, ui->lab_bwd);
	apply_icon("media-skip-forward", theme, ui->lab_fwd);
	apply_icon("media-playback-stop", theme, ui->lab_stop);
}
