#ifndef STDICONLOADER_H_
#define STDICONLOADER_H_

#include "Helper/globals.h"

#include <QString>
#include <QMap>
#include <QIcon>

class StdIconLoader {

	SINGLETON(StdIconLoader)

private:
		QString 				_theme;
		QStringList				_theme_paths;
		QMap<QString, QIcon> 	_icons;

private:
		QStringList load_ancestors(const QString& index_theme_file);

public:
		void add_icon_names(const QStringList& icon_names);
		QIcon get_std_icon(const QString& name) const;
		bool has_std_icon(const QString& name) const;
};

#endif
