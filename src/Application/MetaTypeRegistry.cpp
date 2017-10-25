#include "MetaTypeRegistry.h"
#include <QObject>

#include "Utils/MetaData/MetaDataList.h"
#include "Utils/MetaData/Album.h"
#include "Utils/MetaData/Artist.h"
#include "Utils/MetaData/Genre.h"
#include "Utils/typedefs.h"
#include "Utils/Logger/Logger.h"
#include "Utils/Set.h"

#include "Components/Covers/CoverLocation.h"

MetaTypeRegistry::MetaTypeRegistry()
{
	qRegisterMetaType<StringPair>("StringPair");
	qRegisterMetaType<IntList>("IntList");
	qRegisterMetaType<IDList>("IDList");
	qRegisterMetaType<IdxList>("IdxList");
	qRegisterMetaType<BoolList>("BoolList");
	qRegisterMetaType<ArtistID>("ArtistID");
	qRegisterMetaType<AlbumID>("AlbumID");
	qRegisterMetaType<TrackID>("TrackID");
	qRegisterMetaType<ID>("ID");
	qRegisterMetaType<IntSet>("IntSet");
	qRegisterMetaType<IndexSet>("IndexSet");
	qRegisterMetaType<IDSet>("IDSet");

	qRegisterMetaType<MetaData>("MetaData");
	qRegisterMetaType<MetaDataList>("MetaDataList");
	qRegisterMetaType<Album>("Album");
	qRegisterMetaType<AlbumList>("AlbumList");
	qRegisterMetaType<Artist>("Artist");
	qRegisterMetaType<ArtistList>("ArtistList");
	qRegisterMetaType<Genre>("Genre");

	qRegisterMetaType<Log>("Log");
	qRegisterMetaType<Cover::Location>("Cover::Location");

	qRegisterMetaType<uint64_t>("uint64_t");
	qRegisterMetaType<uint32_t>("uint32_t");

	qRegisterMetaType<int64_t>("int64_t");
	qRegisterMetaType<int32_t>("int32_t");

	qRegisterMetaType<SP::Set<int>>("SP::Set<int>");
}

MetaTypeRegistry::~MetaTypeRegistry()
{}

