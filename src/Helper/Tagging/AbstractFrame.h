#ifndef TAGGINGABSTRACTFRAME_H
#define TAGGINGABSTRACTFRAME_H

#include "Helper/Pimpl.h"

#include <QString>

class QString;
namespace TagLib
{
	class Tag;
	class String;
}


namespace Tagging
{
	class AbstractFrameHelper
	{
		public:
			explicit AbstractFrameHelper(const QString& key=QString());
			virtual ~AbstractFrameHelper();

		protected:
			QString cvt_string(const TagLib::String str) const;
			TagLib::String cvt_string(const QString& str) const;
			QString key() const;
			TagLib::String tag_key() const;

		private:
			PIMPL(AbstractFrameHelper)
	};

	template<typename TagImpl>
	class AbstractFrame :
			public AbstractFrameHelper
	{
		private:
				TagImpl*	_tag=nullptr;

		protected:
			AbstractFrame(TagLib::Tag* tag, const QString& key=QString()) :
				AbstractFrameHelper(key)
			{
				_tag = dynamic_cast<TagImpl*>(tag);
			}

			TagImpl* tag() const
			{
				return _tag;
			}

			void set_tag(TagImpl* tag)
			{
				_tag  = tag;
			}
	};
}
#endif // TAGGINGABSTRACTFRAME_H
