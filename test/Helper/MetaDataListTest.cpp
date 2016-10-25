#include <QObject>
#include <QTest>
#include <QDebug>

#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Set.h"
#include <algorithm>

class MetaDataListTest : public QObject
{
	Q_OBJECT

private slots:
	void cur_idx_test();
	void insert_test();
	void remove_test();
	void move_test();
};



static MetaDataList create_v_md(int min, int max)
{
	MetaDataList v_md;
	for(int i=min; i<max; i++)
	{
		MetaData md;
		md.id = i;
		md.title = QString("title %1").arg(i);
		md.artist = QString("artist %1").arg(i);
		md.album = QString("album %1").arg(i);

		v_md << md;
	}

	return v_md;
}


void MetaDataListTest::cur_idx_test()
{
	int cur_track = 10;
	MetaDataList v_md = create_v_md(0, 53);

	QVERIFY(v_md.get_cur_play_track() == -1);

	v_md.set_cur_play_track(cur_track);
	QVERIFY(v_md.get_cur_play_track() == cur_track);

	int i=0;
	for(const MetaData& md : v_md)
	{
		bool is_cur_track = (i == cur_track);
		QVERIFY(md.pl_playing == is_cur_track);
		i++;
	}
}

void MetaDataListTest::insert_test()
{
	MetaDataList v_md = create_v_md(0, 53);
	MetaDataList inserted_md = create_v_md(100, 105);

	int insert_idx = 8;
	int old_size = v_md.size();
	int cur_track = 10;

	v_md.set_cur_play_track(cur_track);

	v_md.insert_tracks(inserted_md, insert_idx);
	QVERIFY(v_md.size() == old_size + inserted_md.size());

	cur_track += inserted_md.size();
	QVERIFY(v_md.get_cur_play_track() == cur_track);

	int i=0;
	for(const MetaData& md : v_md)
	{
		bool is_cur_track = (i == cur_track);
		QVERIFY(md.pl_playing == is_cur_track);
		i++;
	}
}


static SP::Set<int> create_idx_set(int n, int max_val, int ignore_val=-1)
{
	SP::Set<int> indexes;
	for(int i=0; i<n; i++){
		int idx = (i*7) % max_val;
		if(idx == ignore_val){
			continue;
		}

		indexes.insert(idx);
	}

	return indexes;
}

void MetaDataListTest::remove_test()
{
	MetaDataList v_md = create_v_md(0, 53);
	int old_size = v_md.size();
	int cur_track = 10;

	SP::Set<int> remove_indexes = create_idx_set(15, v_md.size(), cur_track);

	v_md.set_cur_play_track(cur_track);

	int n_tracks_before_cur_track = std::count_if(remove_indexes.begin(), remove_indexes.end(), [&cur_track](int idx){
		return (idx <= cur_track);
	});

	v_md.remove_tracks(remove_indexes);
	cur_track -= n_tracks_before_cur_track;

	QVERIFY(v_md.size() == (int) (old_size - remove_indexes.size()));
	QVERIFY(v_md.get_cur_play_track() == cur_track);

	int i=0;
	for(const MetaData& md : v_md)
	{
		bool is_cur_track = (i == cur_track);
		QVERIFY(md.pl_playing == is_cur_track);
		i++;
	}

	v_md.remove_track(cur_track);
	cur_track = -1;

	QVERIFY(v_md.get_cur_play_track() == cur_track);

	i=0;
	for(const MetaData& md : v_md)
	{
		bool is_cur_track = (i == cur_track);
		QVERIFY(md.pl_playing == is_cur_track);
		i++;
	}
}


void MetaDataListTest::move_test()
{
	for(int rounds = 0; rounds < 10; rounds++)
	{
		MetaDataList v_md = create_v_md(0, 53);
		SP::Set<int> move_indexes = create_idx_set(15, v_md.size());
		QList<int> idxs = move_indexes.toList();
		int target_idx = 8;

		move_indexes.clear();
		for(int i: idxs){
			move_indexes.insert( (i + rounds) % v_md.size() );
		}

		int cur_track = v_md.get_cur_play_track();
		int old_size = v_md.size();

		MetaDataList moved_md;
		for(int i : move_indexes){
			moved_md << v_md[i];
		}

		int n_tracks_before_cur_track = std::count_if(move_indexes.begin(), move_indexes.end(), [&cur_track](int idx){
			return (idx < cur_track);
		});

		int n_tracks_before_target_idx = std::count_if(move_indexes.begin(), move_indexes.end(), [&target_idx](int idx){
			return (idx < target_idx);
		});

		v_md.move_tracks(move_indexes, target_idx);
		QVERIFY(v_md.size() == old_size);

		for(size_t i=0; i<move_indexes.size(); i++){
			int idx = i + target_idx - n_tracks_before_target_idx;
			QVERIFY(v_md[idx].id == moved_md[i].id);
		}

		if(n_tracks_before_cur_track == 0){
			QVERIFY(v_md.get_cur_play_track() == cur_track);
		}

		else if(target_idx <= cur_track){
			int new_cur_track = cur_track - n_tracks_before_cur_track + move_indexes.size();
			QVERIFY(v_md.get_cur_play_track() == new_cur_track);
		}

		else if(target_idx > cur_track){
			int new_cur_track = cur_track - n_tracks_before_cur_track;
			QVERIFY(v_md.get_cur_play_track() == new_cur_track);
		}
	}
}


QTEST_MAIN(MetaDataListTest)

#include "MetaDataListTest.moc"

