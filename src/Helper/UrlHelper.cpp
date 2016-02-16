#include "UrlHelper.h"
#include <QRegExp>

QString Helper::Url::get_protocol(const QString &url){
	QRegExp re("(.+)://.*");

	if(re.indexIn(url) > 0){
		return re.cap(1);
	}

	return "";
}

QString Helper::Url::get_base_url(const QString &url){
	QRegExp re("(.+://[^/]+).*");

	int idx = re.indexIn(url);
	if(idx < 0){
		return url;
	}

	return re.cap(1);
}

QString Helper::Url::get_filename(const QString &url){
	QRegExp re(".+://.+/(.*)");

	int idx = re.indexIn(url);
	if(idx < 0){
		return "";
	}

	return re.cap(1);
}
