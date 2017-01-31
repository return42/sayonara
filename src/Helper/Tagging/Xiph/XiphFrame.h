#ifndef ABSTRACT_XIPH_FRAME_H_
#define ABSTRACT_XIPH_FRAME_H_

#include "Helper/Tagging/AbstractFrame.h"
#include <QString>
#include <taglib/tag.h>
#include <taglib/xiphcomment.h>
#include <taglib/tstring.h>
#include <taglib/tstringlist.h>

namespace Xiph
{
	template<typename Model_t>
	class XiphFrame :
			protected Tagging::AbstractFrame<TagLib::Ogg::XiphComment>
	{
		protected:
			virtual bool map_tag_to_model(Model_t& model)=0;
			virtual bool map_model_to_tag(const Model_t& model)=0;

			bool value(TagLib::String& str) const
			{
				TagLib::Ogg::XiphComment* tag = this->tag();
				const TagLib::Ogg::FieldListMap& map = tag->fieldListMap();
				TagLib::Ogg::FieldListMap::ConstIterator it = map.find( this->tag_key() );
				if(it == map.end()){
					str = TagLib::String();
					return false;
				}

				str = it->second.front();
				return true;
			}

			void set_value(const TagLib::String& value)
			{
				TagLib::Ogg::XiphComment* tag = this->tag();
				tag->addField(this->tag_key(), value, true);
			}

			void set_value(const QString& value)
			{
				TagLib::String str = this->cvt_string(value);
				set_value(str);
			}

		public:
			XiphFrame(TagLib::Tag* tag, const QString& identifier) :
				Tagging::AbstractFrame<TagLib::Ogg::XiphComment>(tag, identifier) {}

			virtual ~XiphFrame() {}

			bool read(Model_t& model)
			{
				if(!this->tag()){
					return false;
				}

				bool success = map_tag_to_model(model);

				return success;
			}

			bool write(const Model_t& model)
			{
				TagLib::Ogg::XiphComment* tag = this->tag();
				if(!tag) {
					return false;
				}

				tag->removeField( this->tag_key() );

				return map_model_to_tag(model);
			}
	};
}

#endif // ABSTRACTFRAME_H
