#include "ImportCache.h"
#include "Helper/Settings/Settings.h"

ImportCache::ImportCache()
{
	_library_path = Settings::getInstance()->get(Set::Lib_Path);
}

void ImportCache::clear()
{
	_files.clear();
	_v_md.clear();
	_src_dst_map.clear();
}

void ImportCache::add_soundfile(const MetaData& md)
{
	if(md.filepath().isEmpty()){
		return;
	}

	_v_md << md;
	_src_md_map[md.filepath()] = md;
}

void ImportCache::add_standard_file(const QString& filename, const QString& parent_dir)
{
	if(filename.isEmpty()){
		return;
	}

	_files << filename;

	QString pure_src_filename = Helper::File::get_filename_of_path(filename);
	QString target_subdir;

	if(!parent_dir.isEmpty()){

		QString file_dir = Helper::File::get_parent_directory(filename);
		QString sub_dir = file_dir.remove(Helper::File::get_absolute_filename(parent_dir));
		QString pure_srcdir = Helper::File::get_filename_of_path(parent_dir);

		target_subdir = pure_srcdir + "/" + target_subdir + "/";
	}

	_src_dst_map[filename] = target_subdir + pure_src_filename;
}

QStringList ImportCache::get_files() const
{
	return _files;
}

MetaDataList ImportCache::get_soundfiles() const
{
	return _v_md;
}

QString ImportCache::get_target_filename(const QString &src_filename, const QString& target_directory) const
{
	return _library_path + "/" + target_directory + "/" + _src_dst_map[src_filename];
}

MetaData ImportCache::get_metadata(const QString& filename) const
{
	return _src_md_map[filename];
}


void ImportCache::change_metadata(const MetaDataList& v_md_old, const MetaDataList& v_md_new)
{
	_v_md = v_md_new;
	for(const MetaData& md : v_md_new){
		_src_md_map[md.filepath()] = md;
	}
}
