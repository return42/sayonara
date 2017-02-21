#include "LFMCoverFetcher.h"
#include "Helper/Helper.h"
#include "Components/StreamPlugins/LastFM/LFMGlobals.h"

#include <QStringList>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QMap>
#include <QUrl>

bool LFMCoverFetcher::can_fetch_cover_directly() const
{
	return false;
}

QDomNode find_artist_node(const QDomNode& node, const QString& prefix)
{
	if(node.nodeName().compare("artist", Qt::CaseInsensitive) == 0 ||
	   node.nodeName().compare("album", Qt::CaseInsensitive) == 0){
		return node;
	}

	if(node.hasChildNodes()){
		return find_artist_node(node.firstChild(), prefix + "  ");
	}

	else if(!node.nextSibling().isNull()){
		return find_artist_node(node.nextSibling(), prefix);
	}

	else
	{
		return QDomNode();
	}
}

QStringList LFMCoverFetcher::calc_addresses_from_website(const QByteArray& website) const
{
	QDomDocument doc("LastFM Cover");
	doc.setContent(website);

	QDomNode root_node = doc.firstChild();
	QDomNode artist_node = find_artist_node(root_node, "");

	if(artist_node.isNull()){
		return QStringList();
	}

	QDomNodeList nodes = artist_node.childNodes();
	if(nodes.isEmpty()){
		return QStringList();
	}

	QStringList attributes; attributes << "extralarge" << "large" << "";
	QMap<QString, QString> lfm_covers;

	for(int i=0; i<nodes.size(); i++){
		QDomNode node = nodes.item(i);
		QString name = node.toElement().tagName();
		if(name.compare("image", Qt::CaseInsensitive) == 0){
			QDomNode attr_node = node.attributes().namedItem("size");
			QString size_attr = attr_node.nodeValue();

			QString url = node.toElement().text();

			lfm_covers[size_attr] = url;
		}
	}

	QStringList ret;
	for(const QString& attr : attributes){
		QString url = lfm_covers[attr];
		if(!url.isEmpty()){
			ret << url;
		}
	}

	return ret;
}


QString LFMCoverFetcher::get_artist_address(const QString& artist) const
{
	QString str = QString("http://ws.audioscrobbler.com/2.0/?method=artist.getinfo&artist=" +
						  QUrl::toPercentEncoding(artist) +
						  "&api_key=") + LFM_API_KEY;

	return str;
}

QString LFMCoverFetcher::get_album_address(const QString& artist, const QString& album) const
{
	QString str = QString("http://ws.audioscrobbler.com/2.0/?method=album.getinfo&artist=" +
						  QUrl::toPercentEncoding(artist) +
						  "&album=" +
						  QUrl::toPercentEncoding(album) +
						  "&api_key=") + LFM_API_KEY;

	return str;
}

QString LFMCoverFetcher::get_search_address(const QString& str) const
{
	Q_UNUSED(str)
	return QString();
}

bool LFMCoverFetcher::is_search_supported() const
{
	return false;
}

bool LFMCoverFetcher::is_album_supported() const
{
	return true;
}

bool LFMCoverFetcher::is_artist_supported() const
{
	return true;
}

int LFMCoverFetcher::get_estimated_size() const
{
	return 300;
}

QString LFMCoverFetcher::get_keyword() const
{
	return "audioscrobbler";
}
