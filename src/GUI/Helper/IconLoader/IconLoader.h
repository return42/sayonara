#ifndef IconLoader_H_
#define IconLoader_H_

#include "Helper/globals.h"
#include "Helper/SayonaraClass.h"

#include <QString>
#include <QMap>
#include <QIcon>
#include <QPixmap>

class Settings;
class IconLoader
{

	SINGLETON(IconLoader)

private:
		QString 				_theme;
		QStringList				_theme_paths;
		QMap<QString, QIcon> 	_icons;
		Settings*				_settings=nullptr;

private:
		QStringList load_ancestors(const QString& index_theme_file);

public:
		void add_icon_names(const QStringList& icon_names);
		QIcon get_icon(const QString& name, const QString& dark_name);
		bool has_std_icon(const QString& name) const;
};

#endif
