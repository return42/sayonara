#ifndef ABSTRACT_XIPH_FRAME_H_
#define ABSTRACT_XIPH_FRAME_H_

#include <QString>
#include <taglib/tag.h>
#include <taglib/xiphcomment.h>
#include <taglib/tstring.h>
#include <taglib/tstringlist.h>


namespace Xiph
{
    template<typename Model_t>
    class AbstractFrame
    {
	private:
	    QString _key;
	    TagLib::Ogg::XiphComment* _tag=nullptr;

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

	    virtual bool map_tag_to_model( const TagLib::String& value, Model_t& model )=0;
	    virtual bool map_model_to_tag( const Model_t& model, TagLib::Ogg::XiphComment* tag )=0;


	public:
	    AbstractFrame(TagLib::Tag* tag, const QString& identifier)
	    {
		_key = identifier;
		_tag = dynamic_cast<TagLib::Ogg::XiphComment*>(tag);
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

		TagLib::Ogg::FieldListMap map = _tag->fieldListMap();
		TagLib::Ogg::FieldListMap::ConstIterator it = map.find( cvt_string(_key) );
		if(it == map.end()){
		    return false;
		}

		else{
		    bool success = map_tag_to_model(it->second.front(), model );
		    return success;
		}
	    }

	    bool write(const Model_t& model)
	    {
		if(!_tag)
		{
		    return false;
		}

		_tag->removeField(cvt_string(_key));

		bool success = map_model_to_tag( model, _tag );
		return success;
	    }
    };
}

#endif // ABSTRACTFRAME_H
