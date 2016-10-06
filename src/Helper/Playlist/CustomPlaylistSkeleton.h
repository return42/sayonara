#include <QString>

/**
 * @brief The CustomPlaylistSkeleton class
 * @ingroup PlaylistHelper
 */
class CustomPlaylistSkeleton {

private:

	int				_id;
	QString			_name;
	bool			_is_temporary;
	int				_num_tracks;

public:
	int id() const;
	void set_id(int id);

	QString name() const;
	void set_name(const QString& name);

	bool temporary() const;
	void set_temporary(bool temporary);

	virtual int num_tracks() const;
	virtual void set_num_tracks(int num_tracks);

	CustomPlaylistSkeleton();
	CustomPlaylistSkeleton(const CustomPlaylistSkeleton& s);
	virtual ~CustomPlaylistSkeleton();
};

typedef QList<CustomPlaylistSkeleton> CustomPlaylistSkeletons;


