/* PodcastParser.cpp */

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



/*
 *
 * valid_document = first tag LIKE <rss version="2.0">
 * valid_item = channel.item.enclosore::(type).contains(audio) > channel.item.enclosure == nullptr
 *
 *
 * MetaData.title = channel.item.title
 * MetaData.artist = itunes:author > channel.item.author
 * MetaData.album = channel.title
 * MetaData.year = channel.item.pubDate.year > channel.pubDate.yeicon.url = > item.image.url > itunes:image (1400x1400) px 
 *
 *
 *
 * */
#include <QString>
#include <QDomDocument>

#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Parser/PodcastParser.h"
#include "Components/CoverLookup/CoverLocation.h"
#include "Components/CoverLookup/CoverLookup.h"

int find_year(QString str) {

    int idx = str.indexOf(QRegExp("[0-9]{4,4}"));

    if(idx >= 0) {
        return str.mid(idx, 4).toInt();
    }

    return 0;
}

bool  PodcastParser::parse_podcast_xml_file_content(const QString& content, MetaDataList& v_md) {

    v_md.clear();

    QDomDocument doc("PodcastFile");
    doc.setContent(content);

    QDomElement docElement = doc.documentElement();
    QDomNode entry = docElement.firstChildElement("channel");

    if(!entry.hasChildNodes()) return false;

    QString author;
    QStringList categories;
    QString album;
	QImage img;
	QString fallback_url;

    for(int c = 0; c<entry.childNodes().size(); c++) {

        QDomNode channel_child = entry.childNodes().at(c);
        QString nodename = channel_child.nodeName();
        QDomElement e = channel_child.toElement();

        if(!nodename.compare("title", Qt::CaseInsensitive)) {
            album = e.text();
        }

        else if(!nodename.compare("itunes:author", Qt::CaseInsensitive)) {
            author = e.text();
        }

        else if(!nodename.compare("itunes:category", Qt::CaseInsensitive)) {

            QStringList genres = e.text().split(QRegExp(",|/|;|\\."));
            for(int i=0; i<genres.size(); i++) {
                genres[i] = genres[i].trimmed();
            }

            categories.append(genres);
        }

		else if(!nodename.compare("image", Qt::CaseInsensitive) && img.isNull()) {
            if(!channel_child.hasChildNodes()) continue;

            for(int i=0; i<channel_child.childNodes().size(); i++) {
                QDomNode item_child = channel_child.childNodes().at(i);
                QString ic_nodename = item_child.nodeName();
                QDomElement ic_e = item_child.toElement();
                if(!ic_nodename.compare("url", Qt::CaseInsensitive)) {
					CoverLocation cl = CoverLocation::get_cover_location(album, author);
                    QString img_url = ic_e.text();
					cl.search_url = img_url;
					CoverLookup* cover = new CoverLookup();
					cover->fetch_cover(cl);
					continue;
                }
            }
		}

		/*else if(!nodename.compare("itunes:image", Qt::CaseInsensitive)) {
			CoverLocation cl = CoverLocation::get_cover_location(album, author);
			QString img_url = ic_e.text();
			cl.search_url = img_url;
			CoverLookup* cover = new CoverLookup();
			cover->fetch_cover(cl);
			continue;
		}*/

        // item
        else if(!nodename.compare("item", Qt::CaseInsensitive)) {

            if(!channel_child.hasChildNodes()) continue;
            MetaData md;
            md.genres.push_back("Podcasts");
            md.genres.append(categories);

            md.album = QString::fromUtf8(album.toStdString().c_str());
            md.artist = QString::fromUtf8(author.toStdString().c_str());

            for(int i=0; i<channel_child.childNodes().size(); i++)
            {

                QDomNode item_child = channel_child.childNodes().at(i);
                QString ic_nodename = item_child.nodeName();
                QDomElement ic_e = item_child.toElement();

                if(!ic_nodename.compare("title", Qt::CaseInsensitive)) {
                    md.title = QString::fromUtf8(ic_e.text().toStdString().c_str());
					md.add_custom_field("1title", "Title", md.title);
                }

				else if(!ic_nodename.compare("description", Qt::CaseInsensitive)){
					QString text = QString::fromUtf8(ic_e.text().toStdString().c_str());
					md.add_custom_field("2desciption", "Description", text);
				}

				else if(!ic_nodename.compare("enclosure", Qt::CaseInsensitive)){

					QDomNamedNodeMap map = ic_e.attributes();
					for(int i=0; i<map.count(); i++){
						QDomAttr attr = map.item(i).toAttr();
						if(attr.name().compare("url", Qt::CaseInsensitive) == 0){
							md.set_filepath(attr.value());
						}
					}
				}

                else if(!ic_nodename.compare("link", Qt::CaseInsensitive)) {
					fallback_url = ic_e.text();
                }

                else if(!ic_nodename.compare("enclosure", Qt::CaseInsensitive)) {
                    md.filesize = ic_e.attribute("length").toLong();
                }

                else if(!ic_nodename.compare("author") && md.artist.size() == 0) {
                    md.artist = QString::fromUtf8(ic_e.text().toStdString().c_str());
                }

                else if(!ic_nodename.compare("itunes:author", Qt::CaseInsensitive)) {
                    md.artist = QString::fromUtf8(ic_e.text().toStdString().c_str());
                }

                else if(!ic_nodename.compare("itunes:duration", Qt::CaseInsensitive)) {

                    QString text = ic_e.text();
                    QStringList lst = text.split(":");
                    int len = 0;
                    for(int i=lst.size() -1; i>=0; i--) {
                        if(i == lst.size() -1)
                            len += lst[i].toInt();
                        else if(i == lst.size() -2) {
                            len += lst[i].toInt() * 60;
                        }
                        else if(i == lst.size() -3) {
                            len += lst[i].toInt() * 3600;
                        }
                    }

                    md.length_ms = len * 1000;
                }


                else if(!ic_nodename.compare("pubDate", Qt::CaseInsensitive) ||
                        !ic_nodename.compare("dc:date", Qt::CaseInsensitive)) {
                    md.year = find_year(ic_e.text());
                }
			}

			if(md.filepath().isEmpty()){
				md.set_filepath(fallback_url);
			}

			if( !md.filepath().isEmpty() ){
				v_md << std::move(md);
			}
        } // item
    }

	if( !img.isNull() ){
		QString cover_path = CoverLocation::get_cover_location(album, author).cover_path;
		img.save( cover_path );
	}

    return (v_md.size() > 0);
}
