#ifndef LIBRARYNAMESPACES_H
#define LIBRARYNAMESPACES_H

#include <QtGlobal>

namespace Library {

	enum class TrackDeletionMode : quint8 {
		None=0,
		OnlyLibrary,
		AlsoFiles
	};

	enum class ReloadQuality : quint8 {
		Fast=0,
		Accurate,
		Unknown
	};
}


#endif // LIBRARYNAMESPACES_H
