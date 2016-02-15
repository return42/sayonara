#include "ASXParser.h"

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

ASXParser::ASXParser(const QString& filename) :
	AbstractPlaylistParser(filename)
{
}


void ASXParser::parse()  {


	QDomDocument doc("AsxFile");
	doc.setContent(_file_content);

	QDomElement docElement = doc.documentElement();
	QDomNode child_node = docElement.firstChild();
	QDomNode entry;

	QString node_name = child_node.nodeName();
	if(node_name.compare("entry", Qt::CaseInsensitive) == 0){
		entry = child_node.toElement();
	}

	if(!entry.hasChildNodes()) {
		return;
	}

	do{

		MetaData md;

		md.length_ms = 0;
		md.album = "";


		for(int i=0; i<entry.childNodes().size(); i++)
		{

			QDomNode node = entry.childNodes().at(i);
			QString node_name = node.nodeName().toLower();

			QDomElement e = node.toElement();
			if(e.isNull()){
				continue;
			}

			if(node_name.compare("ref") == 0) {

				QString file_path = parse_ref_node(node);
				md.artist = file_path;
				md.set_filepath(file_path);
			}

			else if(!node_name.compare("title")) {
				md.title = e.text();
			}

			else if(!node_name.compare("album")) {
				md.album = e.text();
			}

			else if(!node_name.compare("author")) {
				md.artist = e.text();
			}
		}

		_v_md << md;

		entry = entry.nextSibling();

	} while (!entry.isNull());
}

QString ASXParser::parse_ref_node(QDomNode node){

	QDomElement e = node.toElement();
	QDomNamedNodeMap map = e.attributes();

	for(int j=0; j<map.size(); j++){

		QDomNode item_node = map.item(j);
		QString nodename = item_node.nodeName();
		if(nodename.compare("href", Qt::CaseInsensitive) == 0){

			QString path = e.attribute(nodename);
			return  get_absolute_filename(path);
		}
	}

	return "";
}
