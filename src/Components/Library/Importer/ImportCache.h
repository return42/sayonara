#ifndef IMPORTCACHE_H
#define IMPORTCACHE_H

#include <QMap>
#include <QString>
#include <QStringList>
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/FileHelper.h"

class ImportCache
{

private:

	QString					_src_directory;
	QString					_library_path;
	MetaDataList			_v_md;
	QMap<QString, MetaData> _src_md_map;
	QMap<QString, QString>	_src_dst_map;
	QStringList				_files;

public:

	ImportCache(const QString& src_directory=QString());


	void			clear();

	void			add_soundfile(const MetaData& md);
	void			add_standard_file(const QString& filename, const QString& parent_dir);

	QStringList		get_files() const;
	MetaDataList	get_soundfiles() const;
	QString			get_target_filename(const QString& src_filename, const QString& target_directory) const;
	MetaData		get_metadata(const QString& filename) const;

};

#endif // IMPORTCACHE_H
