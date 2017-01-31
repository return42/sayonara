#ifndef ABSTRACT_MP4_FRAME_H_
#define ABSTRACT_MP4_FRAME_H_

#include "Helper/Tagging/AbstractFrame.h"
#include <QString>
#include <taglib/tag.h>
#include <taglib/tstring.h>
#include <taglib/tstringlist.h>
#include <taglib/mp4tag.h>

namespace MP4
{
	template<typename Model_t>
	class MP4Frame :
			protected Tagging::AbstractFrame<TagLib::MP4::Tag>
	{
		protected:
			TagLib::MP4::ItemListMap::ConstIterator find_key(const TagLib::MP4::ItemListMap& ilm) const
			{
				for(TagLib::MP4::ItemListMap::ConstIterator it=ilm.begin(); it!=ilm.end(); it++){
					QString key = this->key();
					if( this->cvt_string(it->first).compare(key, Qt::CaseInsensitive) == 0){
						return it;
					}
				}

				return ilm.end();
			}

			virtual bool map_tag_to_model(Model_t& model)=0;
			virtual bool map_model_to_tag(const Model_t& model)=0;


		public:
			MP4Frame(TagLib::Tag* tag, const QString& identifier) :
				Tagging::AbstractFrame<TagLib::MP4::Tag>(tag, identifier) {}

			virtual ~MP4Frame() {}

			bool read(Model_t& model)
			{
				TagLib::MP4::Tag* key = this->tag();
				if(!key) {
					return false;
				}

				const TagLib::MP4::ItemListMap& ilm = key->itemListMap();

				bool found = (find_key(ilm) != ilm.end());
				if(!found){
					return false;
				}

				bool success = map_tag_to_model(model);

				return success;
			}

			bool write(const Model_t& model)
			{
				TagLib::MP4::Tag* tag = this->tag();
				if(!tag) {
					return false;
				}

				TagLib::MP4::ItemListMap& ilm = tag->itemListMap();

				auto itcopy=ilm.begin();
				for(auto it=ilm.begin(); it!=ilm.end(); it++)
				{
					QString key = this->key();
					if( this->cvt_string(it->first).compare(key, Qt::CaseInsensitive) == 0)
					{
						ilm.erase(it);
						it = itcopy;
					}

					else{
						itcopy = it;
					}
				}

				return map_model_to_tag(model);
			}
	};
}

#endif // ABSTRACT_MP4_FRAME_H_
