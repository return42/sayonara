/* IconLoader.cpp */

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

#include "IconLoader.h"

#include "Components/DirectoryReader/DirectoryReader.h"
#include "GUI/Utils/GuiUtils.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Logger/Logger.h"

#include <QIcon>
#include <QMap>

static QMap<QString, QIcon> s_icons;
static QString s_theme;

#ifdef Q_OS_WIN
QString get_win_icon_name(const QString& name)
{
    QString icon_name = QString(":/IconsWindows/") + name + ".png";
    return icon_name;
}
#endif


QIcon IconLoader::icon(const QStringList &names, const QString &dark_name)
{
    bool dark = (Settings::instance()->get(Set::Player_Style) == 1);

    for(const QString& name : names)
    {
        QIcon icon;
        if(!dark)
        {
#ifdef Q_OS_WIN
            icon = QIcon(get_win_icon_name(name));
#else
            icon = QIcon::fromTheme(name);
#endif
        }

        else
        {
#ifdef Q_OS_WIN
            icon = QIcon(get_win_icon_name(name));
#else
            icon = Gui::Util::icon(dark_name);
#endif
        }

        if(!icon.isNull())
        {
            return icon;
        }
    }

    return Gui::Util::icon(dark_name);
}


QIcon IconLoader::icon(const QString& name, const QString& dark_name)
{
    QStringList names{name};
    return IconLoader::icon(names, dark_name);
}


void IconLoader::change_theme()
{
    Settings* s = Settings::instance();
    QString theme = s->get(Set::Icon_Theme);

    if(s_theme == theme){
        return;
    }

    QIcon::setThemeName(theme);

    s_icons.clear();
}
