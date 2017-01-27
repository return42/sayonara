#ifndef ABSTRACT_MP4_FRAME_H_
#define ABSTRACT_MP4_FRAME_H_

#include <QString>
#include <taglib/tag.h>
#include <taglib/tstring.h>
#include <taglib/tstringlist.h>
#include <taglib/mp4tag.h>
#include "Helper/Logger/Logger.h"

namespace MP4
{
		template<typename Model_t>
		class AbstractFrame
		{
			private:
				QString _key;
				TagLib::MP4::Tag* _tag=nullptr;

			protected:
				TagLib::String cvt_string(const QString& str) const
				{
					return TagLib::String(str.toUtf8().data(), TagLib::String::Type::UTF8);
				}

				QString cvt_string(const TagLib::String str) const
				{
					return QString(str.toCString(true));
				}

				QString key() const
				{
					return _key;
				}

				TagLib::MP4::ItemListMap::ConstIterator find_key(const TagLib::MP4::ItemListMap& ilm) const
				{
					for(TagLib::MP4::ItemListMap::ConstIterator it=ilm.begin(); it!=ilm.end(); it++){
						if( cvt_string(it->first).compare(_key, Qt::CaseInsensitive) == 0){
							return it;
						}
					}

					return ilm.end();
				}

				virtual bool map_tag_to_model( TagLib::MP4::Tag* value, Model_t& model )=0;
				virtual bool map_model_to_tag( const Model_t& model, TagLib::MP4::Tag* tag )=0;


			public:
				AbstractFrame(TagLib::Tag* tag, const QString& identifier)
				{
					_key = identifier;
					_tag = dynamic_cast<TagLib::MP4::Tag*>(tag);
					if(!_tag){
						return;
					}
				}

				virtual ~AbstractFrame() {}

				bool read(Model_t& model)
				{
					if(!_tag){
						return false;
					}

					const TagLib::MP4::ItemListMap& ilm = _tag->itemListMap();

					bool found = (find_key(ilm) != ilm.end());
					if(!found){
						sp_log(Log::Debug) << "Could not find: " << _key;
						return false;
					}

					bool success = map_tag_to_model( _tag, model );

					return success;
				}

				bool write(const Model_t& model)
				{
					if(!_tag)
					{
						return false;
					}

					TagLib::MP4::ItemListMap& ilm = _tag->itemListMap();

					auto itcopy=ilm.begin();
					for(auto it=ilm.begin(); it!=ilm.end(); it++){
						if( cvt_string(it->first).compare(_key, Qt::CaseInsensitive) == 0)
						{
							ilm.erase(it);
							it = itcopy;
						}

						else{
							itcopy = it;
						}
					}

					bool success = map_model_to_tag( model, _tag );

					return success;
				}
		};
}

#endif // ABSTRACT_MP4_FRAME_H_
