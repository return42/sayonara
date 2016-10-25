#include <QObject>
#include <QTest>
#include <QDebug>

#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Set.h"
#include <algorithm>

class MetaDataListTest : public QObject
{
	Q_OBJECT

private:
	MetaDataList create();


private slots:
	void cur_idx_test();
	void insert_test();
	void remove_test();
};



MetaDataList MetaDataListTest::create()
{
	MetaDataList v_md;
	for(int i=0; i<53; i++)	
	{
		MetaData md;
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
	MetaDataList v_md = this->create();

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
	MetaDataList v_md = this->create();

	int old_size = v_md.size();
	int cur_track = 10;
	v_md.set_cur_play_track(cur_track);

	int insert_idx = 8;
	MetaDataList inserted_md;
	for(int i=100; i<105; i++)
	{
		MetaData md;
		md.title = QString("title %1").arg(i);
		md.artist = QString("artist %1").arg(i);
		md.album = QString("album %1").arg(i);
		inserted_md << md;
	}

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


void MetaDataListTest::remove_test()
{
	MetaDataList v_md = this->create();

	int old_size = v_md.size();
	int cur_track = 10;
	v_md.set_cur_play_track(cur_track);

	SP::Set<int> removed_md;
	for(int i=0; i<15; i++){
		int idx = (i*7) % old_size;
		if(idx == cur_track){
			continue;
		}

		removed_md.insert(idx);
	}

	QList<int> idx_list = removed_md.toList();
	int n_tracks_before_cur_track = std::count_if(idx_list.begin(), idx_list.end(), [&cur_track](int idx){
		return (idx < cur_track);
	});

	qDebug() << "Tracks before: " << n_tracks_before_cur_track;
	qDebug() << "Cur track: " << cur_track;

	v_md.remove_tracks(removed_md);
	cur_track -= n_tracks_before_cur_track;

	qDebug() << "Cur track new: " << cur_track;
	qDebug() << "Cur track list: " << v_md.get_cur_play_track();

	QVERIFY(v_md.size() == (old_size - idx_list.size()));
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


QTEST_MAIN(MetaDataListTest)

#include "MetaDataListTest.moc"

