#include "SimilarArtists.h"
#include "Helper/MetaData/ArtistList.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Compressor/Compressor.h"

SimilarArtists::SimilarArtists() {}
SimilarArtists::~SimilarArtists() {}

static QString get_file(const QString& artist) const
{
    QString sayonara_path = Helper::get_sayonara_path() + "/" + "similar_artists/";
    sayonara_path = Helper::File::clean_filename(sayonara_path);
    QDir dir(sayonara_path);

    QStringList name_filters;
        name_filters << "*.comp";

    QDir::Filters filters = (QDir::Filters)(QDir::Files);

    QStringList files = dir.entryList(name_filters, filters);
    if(files.isEmpty()){
        return QString();
    }

    QString target_name = artist.name + ".comp";
    QString result_filename;
    for(const QString& str : files)
    {
        if(str.compare(target_name, Qt::CaseInsensitive) == 0)
        {
            result_filename = dir.filePath(target_name);
            break;
        }
    }

    return result_filename;
}

QMap<QString, double>
SimilarArtists::get_similar_artists(const QString& artist)
{
    QMap<QString, double> sim_artist_map;
    QString filename = get_filename(artist);
    if(filename.isEmpty()){
        return sim_artist_map;
    }

    QByteArray content, decomp;
    bool success = Helper::File::read_file_into_byte_arr(filename, content);
    if(!success){
        return sim_artist_map;
    }

    decomp = Compressor::decompress(content);
	if(decomp.isEmpty()){
		return sim_artist_map;
	}

	QStringList sim_artists  = QString::fromLocal8Bit(decomp).split("\n");
    for(const QString& sim_artist : sim_artists){
        QStringList lst = sim_artist.split('\t');
        if(lst.size() < 3){
			continue;
		}

		QString match = lst[0];
		QString artist_name = lst[2];

        sim_artist_map[artist_name] = match.toDouble();
    }

    return sim_artist_map;
}

QStringList SimilarArtists::get_similar_artist_names(const QString &artist) const
{
    return QStringList( get_similar_artists(artist).keys() );
}
