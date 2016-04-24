#include "GUI_Helper.h"
#include "Helper/Logger/Logger.h"


QIcon GUI::get_icon(const QString& icon_name){

	QString path;

	if(icon_name.endsWith(".png")){
		path = icon_name;
		// alles paletti
	}

	else if(!icon_name.endsWith(".svg.png")){

		path = icon_name + ".svg.png";
	}

	path.prepend(":/Icons/");


	QIcon icon = QIcon(path);
	if(icon.isNull()){
		sp_log(Log::Warning) << "Icon " << path << " does not exist";
	}

	return icon;
}

QPixmap GUI::get_pixmap(const QString& icon_name, QSize sz, bool keep_aspect){

	QString path = QString(":/Icons/") + icon_name;
	if(path.endsWith(".png")){
		// alles paletti
	}

	else if(!path.endsWith(".svg.png")){
		path += ".svg.png";
	}

	QPixmap pixmap(path);

	if(pixmap.isNull()){
		sp_log(Log::Warning) << "Pixmap " << path << " does not exist";
	}

	if(sz.width() == 0){
		return pixmap;
	}

	else{
		if(keep_aspect){
			return pixmap.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}

		else{
			return pixmap.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		}
	}

	return pixmap;
}



