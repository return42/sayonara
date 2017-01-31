#ifndef XIPH_ALBUMARTIST_H_
#define XIPH_ALBUMARTIST_H_

#include "XiphFrame.h"
#include <QString>

/**
 * @ingroup Tagging
 */
namespace Xiph
{
	/**
	 * @brief The AlbumArtistFrame class
	 * @ingroup Xiph
	 */
	class AlbumArtistFrame :
		public Xiph::XiphFrame<QString>
    {
	public:
	    AlbumArtistFrame(TagLib::Tag* tag);
		~AlbumArtistFrame();

	protected:
		bool map_tag_to_model(QString& model) override;
		bool map_model_to_tag(const QString& model) override;
    };
}

#endif // ALBUMARTIST_H
