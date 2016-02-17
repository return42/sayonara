#include "MetaDataList.h"

MetaDataList::MetaDataList() :
	QVector<MetaData>()
{
	_cur_played_track = -1;
}

MetaDataList::MetaDataList(int n_elems) :
	QVector<MetaData>(n_elems)
{
	_cur_played_track = -1;
}

MetaDataList::MetaDataList(const MetaDataList& lst) :
	QVector(lst)
{
	_cur_played_track = lst._cur_played_track;
}

MetaDataList::~MetaDataList() {

}

void MetaDataList::set_cur_play_track_idx_only(int idx) {
	_cur_played_track = idx;
}

void MetaDataList::set_cur_play_track(int idx) {

	_cur_played_track = -1;
	if(idx < 0 || idx >= this->size()) {
		return;
	}

	int tmp_idx=0;
	for(auto it=this->begin(); it != this->end(); it++ ){
		it->pl_playing = (idx == tmp_idx);
		tmp_idx++;
	}

	_cur_played_track = idx;
}

MetaDataList& MetaDataList::insert_tracks(const MetaDataList& v_md, int tgt_idx){

	if(v_md.size() == 0) {
		return *this;
	}

	int end_point;
	int cur_track;

	tgt_idx = std::max(0, tgt_idx);
	tgt_idx = std::min(this->size(), tgt_idx);

	end_point = this->size();
	cur_track = this->get_cur_play_track();

	this->resize(this->size() + v_md.size());

	std::move_backward( this->begin() + tgt_idx, this->begin() + end_point, this->end());

	for(const MetaData& md : v_md) {
		this->operator[](tgt_idx) = std::move(md);
		this->operator[](tgt_idx).is_disabled = !(Helper::check_track(md));

		tgt_idx++;
	}

	if(cur_track >= tgt_idx){
		_cur_played_track = cur_track;
	}

	return *this;
}

MetaDataList& MetaDataList::move_tracks(const IdxList& rows, int tgt_idx){

	MetaDataList v_md_to_move(rows.size());
	MetaDataList v_md_before_tgt;
	MetaDataList v_md_after_tgt;

	int cur_track[3] = {-1,-1,-1};
	int i=0;
	int idx_to_move=0;
	for(auto it=this->begin(); it!=this->end(); it++, i++) {

		const MetaData& md = *it;

		if(!rows.contains(i) && i < tgt_idx){
			v_md_before_tgt << std::move( md );
			if(md.pl_playing){
				cur_track[0] = v_md_before_tgt.size() - 1;
			}
		}

		else if(!rows.contains(i) && i >= tgt_idx){
			v_md_after_tgt << std::move( md );
			if(md.pl_playing){
				cur_track[2] = v_md_after_tgt.size() - 1;
			}
		}

		else if(rows.contains(i)){
			v_md_to_move[idx_to_move] = std::move( md );
			if(md.pl_playing){
				cur_track[1] = v_md_after_tgt.size() - 1;
			}

			idx_to_move++;
		}
	}

	cur_track[1] += v_md_before_tgt.size();
	cur_track[2] += v_md_before_tgt.size() + v_md_to_move.size();

	int start_idx = 0;

	std::move(v_md_before_tgt.begin(), v_md_before_tgt.end(), this->begin());

	start_idx += v_md_before_tgt.size();

	std::move(v_md_to_move.begin(), v_md_to_move.end(), this->begin() + start_idx);

	start_idx += v_md_to_move.size();

	std::move(v_md_after_tgt.begin(), v_md_after_tgt.end(), this->begin() + start_idx);

	for(int i=0; i<2; i++){
		if(cur_track[i] >= 0){
			_cur_played_track = cur_track[i];
			break;
		}
	}

	return *this;
}

MetaDataList& MetaDataList::randomize(){
	RandomGenerator generator;

	for(auto it=this->begin(); it != this->end(); it++){
		int rnd_number = generator.get_number(0, this->size()-1);
		std::swap(*it, *(this->begin() + rnd_number));
	}

	return *this;
}

MetaDataList MetaDataList::extract_tracks(std::function<bool (const MetaData &)> func) const
{
	if(this->isEmpty()){
		return MetaDataList();
	}

	MetaDataList v_md;
	auto it = this->begin();

	while(it != this->end()){

		it = std::find_if(it, this->end(), func);
		if(it == this->end()){
			break;
		}

		else{
			v_md << *it;
			it++;
		}
	}

	return v_md;
}

MetaDataList MetaDataList::extract_tracks(const IdxList& idx_list) const
{
	MetaDataList v_md;
	for(int idx : idx_list){
		v_md << this->operator [](idx);
	}
	return v_md;
}

MetaDataList& MetaDataList::remove_track(int idx){
	if(!between(idx, 0, this->size())){
		return *this;
	}

	for(auto it=this->begin() + idx; it!=this->end(); it++){

		auto it_next = it + 1;
		*it = std::move(*it_next);
	}

	this->removeLast();
	return *this;
}

MetaDataList& MetaDataList::remove_tracks(IdxList rows){

	if(this->isEmpty() || rows.isEmpty()){
		return *this;
	}

	// first, sort the idxs list
	auto lambda = [](int i, int j) -> bool
	{
		return i<j;
	};


	std::sort(rows.begin(), rows.end(), lambda);

	int row_writing_idx=0;
	int deleted_rows=0;

	auto it_idx = rows.begin();

	for(int i=0; i<this->size(); i++){

		if(i == *it_idx && it_idx != rows.end()){

			deleted_rows++;
			it_idx++;
		}

		else{

			if(i != row_writing_idx){
				this->operator [](row_writing_idx) = std::move(this->operator [](i));
			}

			row_writing_idx++;
		}
	}
	this->resize(this->size() - deleted_rows);
	return *this;
}

int MetaDataList::get_cur_play_track() const {
	return _cur_played_track;
}


bool MetaDataList::contains(const MetaData& md) const {

	for(auto it = this->begin(); it != this->end(); it++) {

		if( it->is_equal(md) ) {
			return true;
		}
	}

	return false;
}

IdxList MetaDataList::findTracks(int id) const {

	IdxList ret;
	int idx=0;

	if(id == -1) {
		return ret;
	}

	auto lambda = [&id, &idx, &ret](const MetaData& md) {
		if(md.id == id){
			ret << idx;
		}

		idx++;
	};

	std::for_each(this->begin(), this->end(), lambda);

	return ret;
}

IdxList MetaDataList::findTracks(const QString& path) const {

	IdxList ret;
	int idx=0;

#ifdef Q_OS_UNIX
	Qt::CaseSensitivity sensitivity = Qt::CaseSensitive;
#else
	Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive;
#endif

	auto lambda = [&ret, &idx, &path, &sensitivity](const MetaData& md){
		if(md.filepath().compare(path, sensitivity) == 0){
			ret << idx;
		}

		idx++;
	};

	std::for_each(this->begin(), this->end(), lambda);

	return ret;
}


QStringList MetaDataList::toStringList() const {

	QStringList lst;

	auto lambda = [&lst](const MetaData& md){
		if(md.id >= 0){
			lst << QString::number(md.id);
		}
		else{
			lst << md.filepath();
		}
	};

	for_each(this->begin(), this->end(), lambda);

	return lst;
}

MetaDataInfo* MetaDataList::get_info() const {
	MetaDataInfo* info = new MetaDataInfo(this);
	return info;
}

AlbumInfo* MetaDataList::get_album_info() const {
	AlbumInfo* info = new AlbumInfo(this);
	return info;
}

ArtistInfo* MetaDataList::get_artist_info() const {
	ArtistInfo* info = new ArtistInfo(this);
	return info;
}

