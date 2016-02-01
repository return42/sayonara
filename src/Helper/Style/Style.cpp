/* Style.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * Style.cpp
 *
 *  Created on: Oct 27, 2011
 *      Author: luke
 */


#include "Helper/Style/Style.h"
#include <QApplication>
#include <QFont>


#define NEWLINE "\n";

QString Style::get_style(bool dark, QString font_family, int font_size) {

	QFont font = QApplication::font();

	if(font_family.isEmpty()){
		font_family = font.family();
	}

	if(font_size <= 0){
		font_size = font.pointSize();
	}

    QString style;
    QString share_path = Helper::get_share_path();


	if(!dark){
		Helper::read_file_into_str(share_path + "standard.css", style);

	}

    else{
		Helper::read_file_into_str(share_path + "dark.css", style);
        style.replace("<<SHARE_PATH>>", share_path);
    }

	style.replace("<<FONT_FAMILY>>", font_family);
	style.replace("<<FONT_SIZE>>", QString::number(font_size));

    return style;
}


QString Style::get_tv_style(bool dark, QPalette* p) {

	Q_UNUSED(dark)
	Q_UNUSED(p)
    return  "";

}


QString Style::get_v_slider_style(bool dark, int percent) {
	Q_UNUSED(percent)
    if(!dark) return "";

    QString darker_grey = "#2B2B2B";
    QString dark_grey = "#555555";

    QString orange = "#e8841a";
    QString back_col = orange;

    QString style =  QString("QSlider {") +
            "border-radius: 4px; " +
            "background: #383838; " +

        "}" +

        "QSlider::handle:vertical {" +
       "     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "+
         "                                 stop: 0 #6B6B6B, stop: 0.4 " + dark_grey + ");" +
        "    height: 10px;" +
        "    margin-left: -5px; " +
        "    margin-right: -5px; " +
        "    min-width: 12px;" +
        "    border: 1px solid #2C2C2C; " +
        "    border-radius: 4px; " +
        "}" +

        "QSlider::handle:vertical:disabled {" +
        "    background: transparent;"
        "    height: 0px;"
        "	 border-width: 0px;"
        "}" +


        "QSlider::groove:vertical { " +
        "    background: #383838;" +
        "    width: 6px; " +
        "    left: 10px; right: 10px; " +
        "}" +


        "QSlider::add-page:vertical {"+
        /*"    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, "  +
        "          stop:0 #E88417, stop: 0.3 #C46600, " +
        "          stop: 0.7 #C46600, stop:1 #E88417); " +*/
        //"      background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(255, 0, 0, 255), stop:0.505051 rgba(255, 228, 0, 255), stop:1 rgba(55, 239, 78, 255)); "

        "    background-color: " + back_col + "; "
        "    border-radius: 2px;" +

       "}"+

        "QSlider::sub-page:vertical, QSlider::add-page:vertical:disabled {"+
        "   background: " + darker_grey + ";"+
        //     "    background-color: " + back_col + "; "
        "   border-radius: 2px;" +

        "}";

    return style;
}
