
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


#include "GUI_FontConfig.h"
#include <QApplication>

GUI_FontConfig::GUI_FontConfig(QWidget* parent) :
	PreferenceDialogInterface(parent),
	Ui::GUI_FontConfig()
{
}

GUI_FontConfig::~GUI_FontConfig()
{

}

QString GUI_FontConfig::get_action_name() const
{
	return tr("Fonts");
}

void GUI_FontConfig::init_ui()
{
	setup_parent(this);

	_font_db = new QFontDatabase();

	int cur_font_idx;
	QString cur_family;
	QStringList families = _font_db->families();

	connect(combo_fonts, &QComboBox::currentTextChanged, this, &GUI_FontConfig::combo_text_changed);	
	connect(btn_default, &QPushButton::clicked, this, &GUI_FontConfig::default_clicked);
	connect(btn_apply, &QPushButton::clicked, this, &GUI_FontConfig::apply_clicked);

	combo_fonts->addItems(families);

	cur_family = _settings->get(Set::Player_FontName);
	_cur_font_size = _settings->get(Set::Player_FontSize);

	cur_font_idx = combo_fonts->findText(cur_family);

	if(cur_font_idx >= 0){
		combo_fonts->setCurrentIndex(cur_font_idx);
	}

	else{
		default_clicked();
	}
}


void GUI_FontConfig::combo_text_changed(const QString& font_name){

	QList<int> point_sizes = _font_db->pointSizes(font_name);

	combo_sizes->clear();
	for(int sz : point_sizes){
		combo_sizes->addItem(QString::number(sz));
	}


	int cur_font_size_idx;
	int font_size = _cur_font_size;
	if(font_size <= 0){
		font_size = QApplication::font().pointSize();
	}

	cur_font_size_idx = combo_sizes->findText(QString::number(font_size));

	if(cur_font_size_idx >= 0){
		combo_sizes->setCurrentIndex(cur_font_size_idx);
	}

	_is_default = false;
}

void GUI_FontConfig::commit()
{
	apply_clicked();
}

void GUI_FontConfig::revert(){

}

void GUI_FontConfig::apply_clicked()
{
	bool ok;
	int font_size;

	font_size = combo_sizes->currentText().toInt(&ok);
	_settings->set(Set::Player_FontName, combo_fonts->currentText());

	if(ok){
		_settings->set(Set::Player_FontSize, font_size);
	}
}

void GUI_FontConfig::default_clicked()
{
	QFont font = QApplication::font();

	int cur_font_idx = combo_fonts->findText(font.family());
	if(cur_font_idx >= 0){
		combo_fonts->setCurrentIndex(cur_font_idx);
	}

	int cur_font_size_idx = combo_sizes->findText(QString::number(font.pointSize()));
	if(cur_font_size_idx >= 0){
		combo_sizes->setCurrentIndex(cur_font_size_idx);
	}

	_is_default = true;
}


void GUI_FontConfig::language_changed()
{
	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
	PreferenceDialogInterface::language_changed();
}

void GUI_FontConfig::skin_changed()
{

}

QLabel* GUI_FontConfig::get_title_label()
{
	return lab_title;
}
