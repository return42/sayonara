#include "IconLoader.h"
#include "Helper/Settings/Settings.h"
#include "Helper/DirectoryReader/DirectoryReader.h"
#include <QDir>
#include <QFile>
#include <QRegExp>

IconLoader::IconLoader(){

	_theme = QIcon::themeName();
	_theme_paths = QIcon::themeSearchPaths();
	_settings = Settings::getInstance();

	for(const QString& theme_path : _theme_paths){

		QString full_theme_path = theme_path + "/" + _theme;
		QString index_path = full_theme_path + "/theme.index";
		if(!QFile::exists(index_path)){
			continue;
		}

		_theme_paths += load_ancestors(index_path);
	}

	sp_log(Log::Debug) << "Theme paths " << _theme_paths;

}

IconLoader::~IconLoader(){


}

QStringList IconLoader::load_ancestors(const QString &index_theme_file){

	QFile f(index_theme_file);
	if(!f.open(QFile::ReadOnly)){
		return QStringList();
	}

	QRegExp re_inherit(".*Inhertits\\d=\\d(.*)");

	QStringList ancestor_list;

	while(f.canReadLine()){
		QByteArray arr = f.readLine();
		QString line = QString::fromLocal8Bit(arr);
		QString ancestors;

		int idx = re_inherit.indexIn(line);
		if(idx < 0){
			continue;
		}

		ancestors = re_inherit.cap(1);
		ancestor_list = ancestors.split(',');
		break;
	}

	f.close();

	for(auto it=ancestor_list.begin(); it != ancestor_list.end(); it++){
		*it = it->trimmed();
	}

	return ancestor_list;
}


void IconLoader::add_icon_names(const QStringList& icon_names){

	DirectoryReader dir_reader;
	dir_reader.set_filter("*.png");

	for(const QString& icon_name : icon_names){

		QIcon icon = QIcon::fromTheme(icon_name);
		if( !icon.isNull() ){
			_icons[icon_name] = icon;
			sp_log(Log::Debug) << "Could load icon from theme: " << icon_name;
			continue;
		}

		for(const QString& theme_path : _theme_paths){

			bool found = false;
			QString full_theme_path = theme_path + "/" + _theme;

			QDir full_theme_dir(full_theme_path);

			QStringList files = dir_reader.find_files_rec(full_theme_dir, icon_name);
			sp_log(Log::Debug) << "Search for " << icon_name << " in " << full_theme_path << ": " << files;

			for(const QString& file : files){

				if(file.contains("48")){
					found = true;
				}

				else if(file.contains("32")){
					_icons[icon_name] = QIcon(file);
					found = true;
				}

				else if(file.contains("24")){
					found = true;
				}

				if(found){
					sp_log(Log::Debug) << "Found icon " << icon_name << " in " << file;
					_icons[icon_name] = QIcon(file);
					break;
				}
			}

			if(found){
				break;
			}
		}
	}
}

QIcon IconLoader::get_icon(const QString& name, const QString& dark_name)
{
	bool dark = (_settings->get(Set::Player_Style) == 1);

	if(!dark){

		if(!has_std_icon(name)){
			QStringList lst; lst << name;
			add_icon_names(lst);

			if(has_std_icon(name)){
				return _icons[name];
			}
		}

		else{
			return _icons[name];
		}
	}

	return Helper::get_icon(dark_name);
}


bool IconLoader::has_std_icon(const QString& name) const
{
	return _icons.keys().contains(name);
}
